#include "stdafx.h"
#include "Token.h"
#include "ParseUtil.h"

///////////////////////////////////////////////////////////////////////////
//		Parser
///////////////////////////////////////////////////////////////////////////
Parser::Parser(const char* fname)
{
	buffer = 0;
	if(fname)
		open(fname);
}

Parser::~Parser()
{
	if(buffer)
		delete[] buffer;
}	

void Parser::open(const char* fname_)
{
	XStream ff(0);
    fname = convert_path_xprm(fname_);
	if(!ff.open(fname, XS_IN))
		throw std::logic_error(std::string("File not found: ") + fname + " " + fname_);
	int len = ff.size();
	if(buffer)
		delete[] buffer;
	buffer = new char[len + 1];
	ff.read(buffer, len);
	buffer[len] = 0;
	ff.close();
	current_token = next_token = prev_token = buffer;
	replaced_symbol = 0;
	line = 1;
}

const char* Parser::get_token()	
{
	if(replaced_symbol)
		*next_token = replaced_symbol;
	prev_token = current_token;
	current_token = next_token;

	// Search begin of token
	char*& i = current_token;
	for(;;){
		if(!*i)
			return 0; // eof

		if(*i == '/'){	     
			if(*(i+1) == '/'){ // //-comment
				i += 2;
				if((i = strstr(i, "\n")) == 0) 
					return 0;
				i++;
				line++;
				continue;
				}
			if(*(i+1) == '*'){ // /* */-comment
				i += 2;
				while(!(*i == '*' && *(i + 1) == '/')){
					if(!*i)
						return 0; // error
					if(*i == '\n')
						line++;
					i++;
					}
				i += 2;
				continue;
				}
			}
		if(isspace(*i)){
			if(*i == '\n')
				line++;
			i++;
			}
		else
			break;
		}

	// Search end of token
	char*& j = next_token = current_token;
	if(isalpha(*j) || *j == '_'){ // Name
		j++;
		while(__iscsym(*j))
			j++;
		}
	else
		if(isdigit(*j) || (*j == '.' && isdigit(*(j + 1)))){ // Numerical Literal
			j++;
			while(__iscsym(*j) || *j == '.' || ((*j == '+' || *j == '-') && (*(j - 1) == 'E' || *(j - 1) == 'e')))
				j++;
			}
		else
			if(*j == '"'){ // Character Literal 
				j++;
				if((j = strstr(j, "\"")) == 0) 
					return 0; // error
				j++;
				}
			else
				if(*j == '-' && *(j + 1) == '>'){ // ->
					j += 2;
					if(*j == '*')			// ->*
						j++;
					}
				else
					if(*j == '<' && *(j + 1) == '<'){ // <<
						j += 2;
						if(*j == '=')			// <<=
							j++;
						}
					else
						if(*j == '>' && *(j + 1) == '>'){ // >>
							j += 2;
							if(*j == '=')			// >>=
								j++;
							}
						else
							if(*j == '.' && *(j + 1) == '.' && *(j + 2) == '.') // ...
								j += 3;
							else
								if((*j == '#' && *(j + 1) == '#') || // ##
									(*j == ':' && *(j + 1) == ':') || // ::
									(*j == '&' && *(j + 1) == '&') || // &&
									(*j == '|' && *(j + 1) == '|') || // ||
									(*j == '.' && *(j + 1) == '*') || // .*
									(*j == '+' && *(j + 1) == '+') || // ++
									(*j == '-' && *(j + 1) == '-') || // --
									((*j == '+' || *j == '-' || *j == '*' || *j == '/' || *j == '%' || 
									*j == '^' || *j == '|' || *j == '&' || 
									*j == '!' || *j == '<' || *j == '>' || *j == '=') && *(j + 1) == '=')) // x=
										j += 2;
								else
									j++;

	replaced_symbol = *next_token;
	*next_token = 0;		
	return current_token;
}

void Parser::put_token()
{
	xassert(current_token > prev_token);
	if(replaced_symbol)
		*next_token = replaced_symbol;
	next_token = current_token;
	current_token = prev_token;
	replaced_symbol = *next_token;
	*next_token = 0;		
}

unsigned int Parser::CRC(unsigned int crc)
{
	return ::CRC(buffer, crc);
}

XBuffer& operator<=(XBuffer& os, const Parser& p) {
    std::string full = convert_path_native(p.fname);
    full = std::filesystem::absolute(std::filesystem::u8path(full)).u8string();
    os < full.c_str() < '(' <= p.line < ") : "; return os;
}


///////////////////////////////////////////////////////////////////////////
//		Compiler
///////////////////////////////////////////////////////////////////////////
Compiler::Compiler()
{
	reload();
}

Compiler::~Compiler() {
    clear();
}

void Compiler::clear()
{
    sections.clear();
    contexts.clear();
    parsers.clear();
    dependencies.clear();
}

void Compiler::reload() 
{
    clear();
	push_context(new TokenList(""));
	context().add(new DataTypeTemplate<IntVariable>("int", "int", sizeof(int)));
	context().add(new DataTypeTemplate<DoubleVariable>("double", "double", sizeof(double)));
	context().add(new DataTypeTemplate<FloatVariable>("float", "float", sizeof(float)));
	context().add(new DataTypeTemplate<StringVariable>("string", "char const*", sizeof(char*)));
  
	context().add(new PreprocessorToken);
	context().add(new CreateSectionToken);
	context().add(new StructToken);
	context().add(new EnumToken);
	context().add(new TypedefToken);
	context().add(new StaticToken);

	refine_name_first_token = 0;
	uniqueNameCounter_ = 0;
}


void Compiler::create_section(const char* name) 
{
	sections.push_back(new Section(name));
	section().script_file = parser().file_name();
}

const char* Compiler::get_token(int enable_eof) 
{ 
	const char* token = parser().get_token(); 
	if(token)
		return token;
	parsers.pop_back();
	if(!parsers.empty())
		return get_token();
	if(!enable_eof)
		throw parsing_error("Unexpected end of file");
	return 0;
}

bool Compiler::get_token_if(const char* token) 
{ 
	if(!strcmp(get_token(), token)) 
		return true; 
	else{ 
		parser().put_token();
		return false;
	}
}

const Token* Compiler::refine_name(const Token* token, bool external_call)
{
	if(external_call){
		refine_name_first_token = token;
		full_refined_name = token->name();
		}	
	else
		full_refined_name += token->name();

	const StructVariable* struct_var = dynamic_cast<const StructVariable*>(token);
	if(struct_var){
		const char* str = get_token();
		if(!strcmp(str, ".")){
			full_refined_name += ".";
			return refine_name(struct_var->find_local(get_token()), false);
			}
		else
			parser().put_token();
		}

	const StructDataType* struct_type = dynamic_cast<const StructDataType*>(token);
	if(struct_type){
		const char* str = get_token();
		if(!strcmp(str, "::")){
			full_refined_name += "::";
			return refine_name(struct_type->find_local(get_token()), false);
			}
		else
			parser().put_token();
		}

	const ArrayVariable* array_var = dynamic_cast<const ArrayVariable*>(token);
	if(array_var){
		const char* str = get_token();
		if(!strcmp(str, "[")){
			int index = eval_i();
			const Token* elem = &array_var->at(index);
			skip_token("]");
			full_refined_name += "[";
			XBuffer buf(16);
			buf <= index;
			full_refined_name += buf;
			full_refined_name += "]";
			return refine_name(elem, false);
			}
		else
			parser().put_token();
		}

	return token;
}

bool Compiler::refined_token_from_current_context()
{
	const TokenList& tokens = context();
	return find(tokens.begin(), tokens.end(), ShareHandle<Token>(const_cast<Token*>(refine_name_first_token))) != tokens.end();
}

std::string Compiler::context_prefix() const 
{
	std::string prefix;
	for(ContextList::const_reverse_iterator ci = contexts.rbegin(); ci != contexts.rend();++ci)
	{
		const TokenList* context = *ci;
		if(dynamic_cast<const StructVariable*>(context))
			prefix = "." + prefix;
		else if(dynamic_cast<const StructDataType*>(context))
			prefix = "::" + prefix;
		else
			break;
		prefix = context->name() + prefix;
	}
	return prefix;
}

std::string Compiler::generate_unique_name(const char* var_name)
{
	XBuffer buffer(256, 1);
	//buffer < section().name() < "_" < context_prefix().c_str() < var_name < "_" <= counter++;
	buffer < context_prefix().c_str() < var_name < "_" <= uniqueNameCounter_++;
	std::string name = std::string(buffer);
	replace(name, ".", "_");
	replace(name, "::", "_");
	return name;
}

TokenList& Compiler::top_context() 
{
	for(ContextList::reverse_iterator ci = contexts.rbegin(); ci != contexts.rend();++ci)
	{
		TokenList* context = *ci;
		if(dynamic_cast<const StructVariable*>(context))
			continue;
		else if(dynamic_cast<const StructDataType*>(context))
			continue;
		else
			return *context;
	}
	return *contexts.front();
}

void Compiler::add_new_emulated(Token* token)
{
	top_context().addFront(token);
}

void Compiler::remove_new_emulated(Token* token)
{
	top_context().remove(token);
}

int Compiler::parse_file(const char* fname, XBuffer& sout)
{
	int errors = 0;
	sout < "XPrm v" < _VERSION_ < ": compiling...\n" < fname < "\n";
	//string full_name = exe_path + fname;
	parsers.clear();
	//open(full_name.c_str());
	open(fname);
	try {
		for(;;){
			const char* token_name = get_token(1);
			if(!token_name)
				break;
			const Token* token = refine_name(context().find(token_name));
			token->affect(*this);
			//context().run(token_name, *this);
			}
		}

	catch(const expected_token& exc){
		sout <= parser() < exc.what() < " expected\n";
		errors++;
		}
	catch(const unexpected_token& exc){
		sout <= parser() < "unexpected token: " < exc.what() < "\n";
		errors++;
		}
	catch(const duplicate_token& exc){
		sout <= parser() < "duplicate token: " < exc.what() < "\n";
		errors++;
		}
	catch(const parsing_error& exc){
		sout <= parser() < exc.what() < "\n";
		errors++;
		}
	catch(const std::exception& exc){
		sout <= parser() < exc.what() < "\n";
		errors++;
	}



	return errors;
}

bool Compiler::compile(const char* input, const char* sources, bool rebuild, bool fail_outdated)
{
    int errors = 0;
    std::string fname = convert_path_xprm(input);
    sectionUpdated_ = false;
    try {
        XBuffer bout(1024, 1);
        errors = parse_file(fname.c_str(), bout);
        std::cout << bout;
        if(!errors){
            SectionList::iterator i;
            FOR_EACH(sections, i){
                int updated = (*i)->declaration(sources, rebuild, fail_outdated);
                if ((*i)->definition(sources, updated || rebuild, fail_outdated, dependencies)) {
                    updated |= true;
                }
                sectionUpdated_ |= updated;
            }
        }
        else
            std::cout << fname << ": " << errors << " error(s)" << std::endl;
    }
    catch(const std::exception& exc){
        std::cout << "Compilation error: " << exc.what() << "\n";
        errors++;
    }

    return !errors;
}

///////////////////////////////////////////////////////////////////////////
//		TokenList
///////////////////////////////////////////////////////////////////////////
TokenList::TokenList(const TokenList& tokens, const char* name) 
: Token(name ? name : tokens.name())
{
	parent = 0;
	lock_addition = 0;
	const_iterator i;
	FOR_EACH(tokens, i)
		add((*i)->clone());
	lock_addition = 1;
}

TokenList::~TokenList() {
    TokenList::clear();
}

const Token* TokenList::find(const char* name) const 
{ 
	Map::const_iterator i = map.find(name); 
	if(i != map.end())
		return i->second;
	if(parent)
		return parent->find(name);

	throw unexpected_token(name);
}

const Token* TokenList::find_local(const char* name) const 
{ 
	Map::const_iterator i = map.find(name); 
	if(i != map.end())
		return i->second;
	throw unexpected_token(name);
}

void TokenList::add(Token* token)
{ 
	if(lock_addition)
		throw parsing_error("Attempt to add to const section");
	if(strlen(token->name()) && map.find(token->name()) != map.end()) 
		throw duplicate_token(token->name()); 
	else { 
		push_back(token); 
		map.insert(Map::value_type(token->name(), token)); 
    }
}

void TokenList::addFront(Token* token)
{ 
	if(lock_addition)
		throw parsing_error("Attempt to add to const section");
	if(strlen(token->name()) && map.find(token->name()) != map.end()) 
		throw duplicate_token(token->name()); 
	else { 
		iterator i;
		FOR_EACH(*this, i)
			if(!dynamic_cast<StructDataType*>(&**i))
				break;
		insert(i, token); 
		map.insert(Map::value_type(token->name(), token)); 
	} 
}

void TokenList::parse(Compiler& comp)
{
	comp.push_context(this);
	for(;;){
		const char* token_name = comp.get_token();

		if(!strcmp(token_name, "}"))
			break;

		const Token* token = comp.refine_name(comp.context().find(token_name));
		token->affect(comp);
		//run(token_name, comp);
		}
	addRef();
	comp.pop_context();
	decrRef();
}

void TokenList::clear() {
    for (auto& h : *this) {
        h = nullptr;
    }
    list<ShareHandle<Token>>::clear();
    map.clear();
}

///////////////////////////////////////////////////////////////////////////
//		Section
///////////////////////////////////////////////////////////////////////////
int Section::variables() const
{
	int cnt = 0;
	const_iterator i;
	FOR_EACH(*this, i){
		const Variable* var = dynamic_cast<const Variable*>(&**i);
		if(var && var->declarable())
			cnt++;
		}
	return cnt;
}

unsigned Section::description()
{					    
	unsigned description = 83838383;
	iterator i;
	FOR_EACH(*this, i){
		const Variable* var = dynamic_cast<Variable*>(&**i);
		if(var && var->definible())
			var->description(description);
	}
	return description;
}

std::string Section::align_path(const char* sources, const std::string& str)
{
    size_t n;
	std::string path = convert_path_xprm(script_file.c_str());

	//Check and remove the initial sources dir
    if (sources) {
        std::string sources_path = sources;
        terminate_with_char(sources_path, PATH_SEP);
        n = path.find(sources_path);
        if (n != std::string::npos && n == 0) {
            path.erase(0, sources_path.length());
            path = std::string(".") + PATH_SEP + path;
        }
    }
    
    //Remove filename from path
	n = path.rfind(PATH_SEP);
	if(n == std::string::npos)
		n = 0;
	else 
		n++;
	path.erase(n, path.size());

	//Get filename
	std::string name(str);
	n = name.rfind(PATH_SEP);
	if(n == std::string::npos)
		n = 0;
	else 
		n++;
	name.erase(0, n);
	
	return path + name;
}

///Ensure that differences are because of content and not because differing newline styles, in future we should use \n only
bool differentContent(std::string generated, std::string existing, bool fail_outdated) {
    return generated != existing;
}

bool Section::definition(const char* sources, bool rebuild, bool fail_outdated, StringList& dependencies)
{
	std::string head = "//////////////////////////////////////////////////////////////////////////////////////////////\n"
			"//	XScript definition\n"
			"//	Section: " + std::string(name()) + "\n";

	std::string tail =  "//////////////////////////////////////////////////////////////////////////////////////////////\n"
		"//	XScript end: " + std::string(name()) + "\n"
		"//////////////////////////////////////////////////////////////////////////////////////////////\n";

	char* description_str = "\tdescription = ";
	std::string file;
	std::string definition_file_path;
    if (sources) definition_file_path = std::string(sources) + PATH_SEP;
    definition_file_path += definition_file;
	XStream ff(0);
	if(!rebuild && ff.open(definition_file_path.c_str(), XS_IN)) {
		int len = ff.size();
		file.insert((size_type)0, (size_type)len, ' ');
		ff.read(&file[0], len);
		ff.close();
	}

    size_t head_pos = file.find(head);
    /* Doesn't update unless you change the structure
	size_t end_pos = std::string::npos;
	if(head_pos != std::string::npos){
		end_pos = file.find("XScript end:", head_pos);
		if(end_pos == std::string::npos)
			throw std::logic_error(std::string("Section corrupted: ") + name());
		//if(supress_definition_update){ 
        size_t pos = file.find(description_str, head_pos);
		if(pos < end_pos){
			pos += strlen(description_str);
			unsigned prev_description = strtoul(&file[pos], 0, 10);
			if(description() == prev_description)
				return 0;
			}
			//}
		}
    */

	WriteStream buf;
	buf < "//	Number of variables: " <= variables() < "\n";
	buf < "//	This is computer generated code, don't try to change it\n";
	buf < "//////////////////////////////////////////////////////////////////////////////////////////////\n";
	if(using_namespace) buf < "namespace " < name() < "_namespace\n{\n";

	iterator i;
	FOR_EACH(*this, i){
		const Variable* var = dynamic_cast<Variable*>(&**i);
		if(var && var->definible()){
			var->write_name(buf);
			buf < " = ";
			var->write_value(buf);
			buf < ";\n"; 
			continue;
			}
		const StructDataType* str = dynamic_cast<const StructDataType*>(&**i);
		if(str){
			str->declaration(buf);
			continue;
			}
		}

	buf < "\n#ifdef _PRM_EDIT_\nstruct " < name() < "_ParameterSection : ParameterSection\n{\n\t"
	< name() < "_ParameterSection() : ParameterSection(\"" < name() < "\")\n{\n";

	StringList::iterator si;
	FOR_EACH(dependencies, si){
		std::string s_add = "\tadd_dependency(\"";
		s_add += expand_spec_chars(align_path(sources, *si)).c_str();
		s_add += "\"";
		buf < s_add.c_str() < ");\n";
    }
	buf < description_str <= description() < ";\n";

	buf < "\treserve(" <= variables() < ");\n";
	FOR_EACH(*this, i) {
		const Variable* var = dynamic_cast<Variable*>(&**i);
		if(var && var->definible()) {
            unsigned int crc = 83838383;
            var->description(crc);
            buf < "\tadd(&" < var->name() < ", \"" < var->name() < "\", " < std::to_string(crc).c_str() < ");\n";
        }
    }

	buf < "\tadd_parameter_section(this);\n}\n};\n";
	buf < "static " < name() < "_ParameterSection " < name() < "_ParameterSectionObject;\n";
	if(using_namespace) buf < "}\n";
	buf < "#endif  //  _PRM_EDIT_\n";

	std::string file0 = file;
	std::string operation;
	if(head_pos == std::string::npos){ // first time
		operation = "Creating";
    } else {
		operation = "Updating";
    }
    file = head;
    file += buf;
    file += tail;
	if (differentContent(file, file0, fail_outdated)) {
		std::cout << operation << " definition of section \"" << name() << "\" in " << definition_file_path << std::endl;
		ff.open(definition_file_path.c_str(), XS_OUT);
		ff < file.c_str();
        if (fail_outdated) {
            std::cout << file << std::endl;
            throw std::logic_error("Definition needs to be updated! recompile and save changes");
        }
		return 1;
    }
	return 0;
}

bool Section::declaration(const char* sources, bool rebuild, bool fail_outdated)
{
	std::string head = "//////////////////////////////////////////////////////////////////////////////////////////////\n"
			"//	XScript declaration\n"
			"//	Section: " + std::string(name()) + "\n";

	std::string tail =  "//////////////////////////////////////////////////////////////////////////////////////////////\n"
		"//	XScript end: " + std::string(name()) + "\n"
		"//////////////////////////////////////////////////////////////////////////////////////////////\n";

	XBuffer buf(100000, 1);
	buf < "//	Number of variables: " <= variables() < "\n";
	buf < "//	This is computer generated code, don't try to change it\n";
	buf < "//////////////////////////////////////////////////////////////////////////////////////////////\n";
	if(using_namespace) buf < "namespace "< name() < "_namespace\n{\n";

	iterator i;
	FOR_EACH(*this, i){
		const Variable* var = dynamic_cast<const Variable*>(&**i);
		if(var && var->declarable()){
			buf < "extern ";
			var->write_name(buf);
			buf < ";\n";
			continue;
			}
		const StructDataType* str = dynamic_cast<const StructDataType*>(&**i);
		if(str){
			str->declaration(buf);
			continue;
			}
		const EnumList* enm = dynamic_cast<const EnumList*>(&**i);
		if(enm){
			enm->declaration(buf);
			continue;
			}
		}

	if(using_namespace) buf < "}\nusing namespace "< name() < "_namespace;\n";

    std::string declaration_file_path;
    if (sources) declaration_file_path = std::string(sources) + PATH_SEP;
    declaration_file_path += declaration_file;
    std::string file;
	XStream ff(0);
	if(!rebuild && ff.open(declaration_file_path.c_str(), XS_IN)){
		int len = ff.size();
		file.insert((size_type)0, (size_type)len, ' ');
		ff.read(&file[0], len);
		ff.close();
	}
    std::string file0 = file;

    std::string operation;
	size_t head_pos = file.find(head);
    if(head_pos == std::string::npos){ // first time
        operation = "Creating";
    } else {
        operation = "Updating";
    }
    file = head;
    file += buf;
    file += tail;
    if (differentContent(file, file0, fail_outdated)) {
        std::cout << operation << " declaration of section \"" << name() << "\" in " << declaration_file_path << std::endl;
		ff.open(declaration_file_path.c_str(), XS_OUT);
		ff < file.c_str();
        if (fail_outdated) {
            std::cout << file << std::endl;
            throw std::logic_error("Declaration needs to be updated! recompile and save changes");
        }
		return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//	XScript begin
//	Section: Name
//	Number of variables:
//	This is computer generated code, don't try to change it.
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
//	XScript end: Name
//////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
//		Variable
/////////////////////////////////////////////////////////////////////////////////////////////
void Variable::affect(Compiler& comp) const  // Assignment
{
	if(!comp.refined_token_from_current_context())
		throw parsing_error((std::string("Attempt to assign variable of top context: ") + name()).c_str());

	//const Token* token = comp.refine_name(this);
	//const Variable* var = dynamic_cast<const Variable*>(token);
	//if(!var)
	//	throw parsing_error("Access to non-variable");
	comp.skip_token("=");
	const_cast<Variable*>(this)->init(comp);
	comp.skip_token(";");
}	

/////////////////////////////////////////////////////////////////////////////////////////////
//		DataType
/////////////////////////////////////////////////////////////////////////////////////////////
void DataType::affect(Compiler& comp) const  // Declaration & definition
{
	bool is_pointer = comp.get_token_if("*");

	std::string refine_name_prefix = comp.refined_name();
    size_t pos = refine_name_prefix.rfind(name());
	if(pos == std::string::npos)
		refine_name_prefix = "";
	else
		refine_name_prefix.erase(pos,refine_name_prefix.size());

	std::string var_name = comp.get_token();
	if(is_pointer){
		comp.skip_token("=");
		PointerVariable* var = new PointerVariable(var_name.c_str(), *this);
		var->refine_name_prefix = refine_name_prefix;
		var->init(comp);
		comp.context().add(var);
	}
	else if(comp.get_token_if("[")){
        std::string size_name;
		int declare_size = 1;
		if(comp.get_token_if("int")){
			size_name = comp.get_token();
			if(!is_name(size_name.c_str()))
				throw parsing_error("Variable name expected");
			static DataTypeTemplate<IntVariable> IntDataType("int", "int", sizeof(int));
			comp.context().add(new IntVariable(size_name.c_str(), IntDataType));
			if(comp.get_token_if("="))
				declare_size = comp.eval_i();
			comp.skip_token("]");
			if(!dynamic_cast<StructDataType*>(&comp.context())) // для открытых (не в структуре) массивов с декларированной переменной не декларировавть размер
				declare_size = 0;
		}
		else if(!comp.get_token_if("]")){
			declare_size = comp.eval_i();
			comp.skip_token("]");
		}

		ArrayVariable* array = new ArrayVariable(var_name.c_str(), *this, size_name, declare_size);
		comp.skip_token("=");
		comp.context().add(array);
		array->refine_name_prefix = refine_name_prefix;
		array->init(comp);
	}
	else if(comp.get_token_if("=")){
		Variable* var = create(var_name.c_str());
		comp.context().add(var);
		var->refine_name_prefix = refine_name_prefix;
		var->init(comp);
	}
	else
		throw unexpected_token(comp.get_token());
	comp.skip_token(";");
}

/////////////////////////////////////////////////////////////////////////////////////////////
//		Array
/////////////////////////////////////////////////////////////////////////////////////////////
void ArrayVariable::init(Compiler& comp) 
{  
	Variable::init(comp);
	int first_init = empty();
	iterator it = begin();
	current_size = 0;
	comp.skip_token("{");
	std::string token = comp.get_token();
	if(token != std::string("}")){
		comp.parser().put_token();
		while(1){
			Variable* v;
			if(first_init){
				v = type.create("");
				push_back(v);
				}
			else{
				if(current_size == size())
					throw parsing_error("Too many elements in reassigned array");
				v = *it;
				++it;
				}
			v->refine_name_prefix = refine_name_prefix;
			v->init(comp);
			current_size++;
			std::string token = comp.get_token();
			if(token == "}")
				break;
			if(token != std::string(","))
				throw unexpected_token(token.c_str());
			}
		}

	if(!size_var_name.empty())
		const_cast<IntVariable*>(dynamic_cast<const IntVariable*>(comp.context().find_local(size_var_name.c_str())))->set(current_size);

	while(size() < declare_size){ // добавляем недостающие элементы
		Variable* v = type.create("");
		push_back(v);
	}
}

void ArrayVariable::write_value(WriteStream& buf) const 
{ 
	const_iterator i;
	buf < "{ ";
	FOR_EACH(*this, i){
		(*i)->write_value(buf);
		buf.check_line();
		buf < ", ";
		}
	if(!empty())
		buf -= 2;
	buf < " }";
}

void ArrayVariable::copy_value(void* val) const
{ 
	int n = 0;
	const_iterator i;
	FOR_EACH(*this, i) {
        (*i)->copy_value((char*) val + n++ * type.sizeOf());
    }
}

void ArrayVariable::description(unsigned& crc) const {
    Variable::description(crc);
    crc = CRC(size(), crc);
    const_iterator i;
    FOR_EACH(*this, i){
        (*i)->description(crc);
    }
}

Token* ArrayVariable::clone() const 
{ 
	ArrayVariable* array = new ArrayVariable(name(), type, size_var_name, declare_size);
	if(!definible())
		array->static_flag = 1;
	const_iterator i;
	FOR_EACH(*this, i)
		array->push_back(dynamic_cast<Variable*>((*i)->clone()));
	array->current_size = current_size;
	return array; 
} 


///////////////////////////////////////////////////////////////////////////
//				Pointer
///////////////////////////////////////////////////////////////////////////
void PointerVariable::init(Compiler& comp)
{
	if(comp.get_token_if("0")){ // Type* pointer = 0;
		variable = 0;
	}
	else if(comp.get_token_if("&")){ // Type* pointer = &object;
		const char* token_name = comp.get_token();
		variable = dynamic_cast<const Variable*>(comp.refine_name(comp.context().find(token_name)));
		variable_refine_name = comp.refined_name();
		if(!variable){
			XBuffer msg;
			msg < "Unable to convert \"" < token_name < "\" to \"" < type.name() < "\"";
			throw parsing_error(msg);
		}
		if(!variable->definible())
			throw parsing_error("Don't use static variables to rerefernce to");
	}
	else if(comp.get_token_if("new")){ // Type* pointer = new Type;
		const char* token_name = comp.get_token();
		const DataType* type = dynamic_cast<const DataType*>(comp.refine_name(comp.context().find(token_name)));
		if(!type){
			XBuffer msg;
			msg < "Unknown type: \"" < token_name < "\"";
			throw parsing_error(msg);
		}
		
		variable_refine_name = comp.generate_unique_name(name());
		if(comp.get_token_if("[")){
			if(comp.get_token_if("int")){
				if(!size_var_name.empty())
					throw parsing_error("Redefinition of size variable");
				size_var_name = comp.get_token();
				if(!is_name(size_var_name.c_str()))
					throw parsing_error("Variable name expected");
				static DataTypeTemplate<IntVariable> IntDataType("int", "int", sizeof(int));
				comp.context().add(new IntVariable(size_var_name.c_str(), IntDataType));
			}
			comp.skip_token("]");
			ArrayVariable* array = new ArrayVariable(variable_refine_name.c_str(), *type, size_var_name, 0);
			array->new_emulation_flag = 1;
			comp.add_new_emulated(array);
			array->refine_name_prefix = refine_name_prefix;
			array->init(comp);
			if(array->size()){
				variable = array;
				inited_by_array = true;
			}
			else
				comp.remove_new_emulated(array);
		}
		else{
			Variable* var = type->create(variable_refine_name.c_str());
			var->new_emulation_flag = 1;
			comp.add_new_emulated(var);
			var->refine_name_prefix = refine_name_prefix;
			var->init(comp);
			variable = var;
			if(!size_var_name.empty())
				const_cast<IntVariable*>(dynamic_cast<const IntVariable*>(comp.context().find_local(size_var_name.c_str())))->set(1);
		}
	}
	else
		throw unexpected_token(comp.get_token());
}

///////////////////////////////////////////////////////////////////////////
//		Struct
///////////////////////////////////////////////////////////////////////////
StructDataType::StructDataType(Compiler& comp) 
: Token(comp.get_token()), 
TokenList(name()), 
DataType(name(), 0) 
{ 
	dont_declare = 0;
	std::string dir = comp.get_token();
	if(dir == ":"){
		const Token* token = comp.context().find(comp.get_token());
		const StructDataType* base = dynamic_cast<const StructDataType*>(token);
		if(base){
			std::string new_name = name();
			static_cast<DataType&>(*this) = static_cast<const DataType&>(*base);
			delegate_code = base->delegate_code;
			TokenList::const_iterator ti;
			FOR_EACH(*base, ti)
				add((*ti)->clone());
			name_ = new_name;
			dont_declare = 1;
			}
		else
			throw parsing_error("Incorrect inheritage");
		comp.skip_token("{"); 
		parse(comp); 
		}
	else{		
		if(dir != std::string("{"))
			throw expected_token("{");

		add(new DelegateToken);
			
		parse(comp); 
		size_ = 0;
		iterator i;
		FOR_EACH(*this, i){
			Variable* var = dynamic_cast<Variable*>(&**i);
			if(var && var->declarable())
				size_ += var->sizeOf();
			}
		}
}

void StructDataType::description(unsigned& crc) const 
{ 
	DataType::description(crc); 
    crc = CRC(this->size(), crc);
	const_iterator i;
	FOR_EACH(*this, i){
		const DescriptedToken* var = dynamic_cast<const DescriptedToken*>(&**i);
		if (var) var->description(crc);
	}
}

void StructDataType::declaration(XBuffer& buf) const 
{
	if(dont_declare)
		return;
	buf < "#ifndef __XScript_struct_" < name() < "__\n";
	buf < "#define __XScript_struct_" < name() < "__\n";
	buf < "#pragma pack( push, __XScript_struct_pack__, 4 )\n";
	buf < "struct " < name() < " {\n";
	const_iterator i;
	FOR_EACH(*this, i){
		const Variable* var = dynamic_cast<const Variable*>(&**i);
		if(var && var->declarable()){
			buf < "\t";
			var->write_name(buf);
			buf < ";\n";
			continue;
			}
		const StructDataType* str = dynamic_cast<const StructDataType*>(&**i);
		if(str){
			str->declaration(buf);
			continue;
			}
		const EnumList* enm = dynamic_cast<const EnumList*>(&**i);
		if(enm){
			enm->declaration(buf);
			continue;
			}
		}
	if(!delegate_code.empty())
		buf < delegate_code.c_str();
	buf < "};\n";
	buf < "#pragma pack( pop, __XScript_struct_pack__)\n";
	buf < "#endif  // __XScript_struct_" < name() < "__\n";
}

Variable* StructDataType::create(const char* name) const 
{	
	return new StructVariable(name, *this); 
}


void StructVariable::init(Compiler& comp)
{
	Variable::init(comp);
	for(;;){
		const char* token_name = comp.get_token();
		if(!strcmp(token_name, "{")){
			parse(comp);
			break;
			}
		else{
			const Token* token = comp.refine_name(comp.context().find(token_name));
			unsigned int this_dsc = 83838383;
			type.description(this_dsc);
			const TokenList* token_list = 0;
			const StructVariable* var = dynamic_cast<const StructVariable*>(token);
			if(var){ // && var->declarable()){
				unsigned int var_dsc = 83838383;
				var->type.description(var_dsc);
				if(var_dsc == this_dsc)
					token_list = var;
				}
			else{
				const StructDataType* base = dynamic_cast<const StructDataType*>(token);
				if(base){
					unsigned int base_dsc = 83838383;
					base->description(base_dsc);
					if(base_dsc == this_dsc)
						token_list = base;
					}
				}
			if(token_list){
				if(token_list != this){ // Не присваивать самому себе
					TokenList::clear();
					lock_addition = 0;
					TokenList::const_iterator ti;
					FOR_EACH(*token_list, ti)
						add((*ti)->clone());
					lock_addition = 1;
					}
 				}
			else{
				XBuffer msg;
				msg < "Unable to convert \"" < token_name < "\" to struct \"" < type.name() < "\"";
				throw parsing_error(msg);
				}

			if(!strcmp(comp.get_token(), ";")){ // Assignment without {}
				comp.parser().put_token();
				return;
				}
			comp.parser().put_token();
			}
		}
}

void StructVariable::write_value(WriteStream& buf) const
{
	const_iterator i;
	buf < "{ ";
	FOR_EACH(*this, i){
		const Variable* var = dynamic_cast<const Variable*>(&**i);
		if(var && var->declarable()){
			var->write_value(buf);
			buf.check_line();
			buf < ", ";
			}
		}
	if(!empty())
		buf -= 2;
	buf < " }";
}

void StructVariable::copy_value(void* val) const
{
	int offs = 0;
	const_iterator i;
	FOR_EACH(*this, i){
		const Variable* var = dynamic_cast<const Variable*>(&**i);
		if(var && var->declarable()){
			var->copy_value((char*)val  +  offs);
			offs += var->sizeOf();
			}
		}
}

void StructVariable::description(unsigned& crc) const
{
    Variable::description(crc);
    const_iterator i;
    FOR_EACH(*this, i) {
        const DescriptedToken* var = dynamic_cast<const DescriptedToken*>(&**i);
        if (var) var->description(crc);
    }
}

///////////////////////////////////////////////////////////////////////////
//		Enum
///////////////////////////////////////////////////////////////////////////
void EnumVariable::init(Compiler& comp)
{ 
	Variable::init(comp); 
	const Token* token = comp.refine_name(comp.context().find(comp.get_token()));
	const EnumList& enum_list = dynamic_cast<const EnumList&>(type);
	EnumList::const_iterator i = find(enum_list.begin(),enum_list.end(),token);
	if(i != enum_list.end())
	{
		value = **i;
		enum_value = enum_list.prefix + token->name();
	}
	else
	{
		const EnumVariable* ev = dynamic_cast<const EnumVariable*>(token);
		if(ev && &ev->type == &type)
		{
			value = ev->value;
			enum_value = ev->enum_value;
		}
		else
		{
			XBuffer msg;
			msg < "Unable to convert \"" < comp.refined_name() < "\" to enum \"" < refine_name_prefix.c_str() < type.name() < "\"";
			throw parsing_error(msg);
		}
	}
}

///////////////////////////////////////////////////////////////////////////
//		Int Calculator
///////////////////////////////////////////////////////////////////////////
int Compiler::eval_i()
{
	return or_i();
}

int Compiler::or_i()
{
	int left = xor_i();

	for(;;){
		const char* token_name = get_token();

		if(!strcmp(token_name, "|"))
			left |= xor_i();
		else{
			parser().put_token();
			return left;
			}
		}
}

int Compiler::xor_i()
{
	int left = and_i();

	for(;;){
		const char* token_name = get_token();

		if(!strcmp(token_name, "^"))
			left ^= and_i();
		else{
			parser().put_token();
			return left;
			}
		}
}

int Compiler::and_i()
{
	int left = shift_i();

	for(;;){
		const char* token_name = get_token();

		if(!strcmp(token_name, "&"))
			left &= shift_i();
		else{
			parser().put_token();
			return left;
			}
		}
}

int Compiler::shift_i()
{
	int left = expr_i();

	for(;;){
		const char* token_name = get_token();

		if(!strcmp(token_name, "<<"))
			left <<= expr_i();
		else
			if(!strcmp(token_name, ">>"))
				left >>= expr_i();
			else{
				parser().put_token();
				return left;
				}
		}
}

int Compiler::expr_i()
{
	int left = term_i();

	for(;;){
		const char* token_name = get_token();

		if(!strcmp(token_name, "+"))
			left += term_i();
		else
			if(!strcmp(token_name, "-"))
				left -= term_i();
			else{
				parser().put_token();
				return left;
				}
		}
}

int Compiler::term_i()
{
	int left = prim_i();

	for(;;){
		const char* token_name = get_token();

		if(!strcmp(token_name, "*"))
			left *= prim_i();
		else
			if(!strcmp(token_name, "/")){
				int d = prim_i();
				if(!d)
					throw parsing_error("Divide by zero");
				left /= d;
				}
			else{
				parser().put_token();
				return left;
				}
		}
}

int Compiler::prim_i()
{
	const char* name = get_token(); 
	
	if(isdigit(*name)) // Numerical Literal
		return atoi(name);
	
	if(!strcmp(name, "-"))
		return -prim_i();

	if(!strcmp(name, "(")){
		int e = or_i();
		skip_token(")");
		return e;
		}

	const Token* token = refine_name(context().find(name));
	if(token){
		const IntConstant* ivar = dynamic_cast<const IntConstant*>(token);
		if(ivar)
			return *ivar;

		throw parsing_error("Unable to convert to int");
		}
	else
		throw unexpected_token(name);
}

///////////////////////////////////////////////////////////////////////////
//		Float Calculator
///////////////////////////////////////////////////////////////////////////

double Compiler::eval_f()
{
	return expr_f();
}

double Compiler::expr_f()
{
	double left = term_f();

	for(;;){
		const char* token_name = get_token();

		if(!strcmp(token_name, "+"))
			left += term_f();
		else
			if(!strcmp(token_name, "-"))
				left -= term_f();
			else{
				parser().put_token();
				return left;
				}
		}
}

double Compiler::term_f()
{
	double left = prim_f();

	for(;;){
		const char* token_name = get_token();

		if(!strcmp(token_name, "*"))
			left *= prim_f();
		else
			if(!strcmp(token_name, "/")){
				double d = prim_f();
				if(xm::abs(d) < 1e-10)
					throw parsing_error("Divide by zero");
				left /= d;
				}
			else{
				parser().put_token();
				return left;
				}
		}
}

double Compiler::prim_f()
{
	const char* name = get_token(); 
	
	if(isdigit(*name) || (strlen(name) > 1 && *name == '.' && isdigit(name[1]))) // Numerical Literal
		return atof(name);
	
	if(!strcmp(name, "-"))
		return -prim_f();

	if(!strcmp(name, "(")){
		double e = expr_f();
		skip_token(")");
		return e;
		}

	const Token* token = refine_name(context().find(name));
	if(token){
		const IntConstant* ivar = dynamic_cast<const IntConstant*>(token);
		if(ivar)
			return *ivar;

		const DoubleVariable* dvar = dynamic_cast<const DoubleVariable*>(token);
		if(dvar)
			return *dvar;

		const FloatVariable* fvar = dynamic_cast<const FloatVariable*>(token);
		if(fvar)
			return *fvar;

		throw parsing_error("Unable to convert to double");
		}
	else
		throw unexpected_token(name);
}

///////////////////////////////////////////////////////////////////////////
//		String Calculator
///////////////////////////////////////////////////////////////////////////
std::string Compiler::eval_s()
{
	return expr_s();
}

std::string Compiler::expr_s()
{
    std::string  left = prim_s();

	for(;;){
		const char* token_name = get_token();

		if(!strcmp(token_name, "+"))
			left += prim_s();
		else{
			parser().put_token();
			return left;
			}
		}
}

std::string Compiler::prim_s()
{
	const char* name = get_token(); 
	
	if(*name == '"') // String Literal
		return strip_string(name);
	
	const Token* token = refine_name(context().find(name));
	if(token){
		const StringVariable* svar = dynamic_cast<const StringVariable*>(token);
		if(svar)
			return *svar;

		throw parsing_error("Unable to convert to string");
		}
	else
		throw unexpected_token(name);
}


