#include "stdafx.h"
#include "Token.h"
#include "ParseUtil.h"

Token* Token::lastTokenCreated_;

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
		delete buffer;
}	

void Parser::open(const char* fname_)
{
	XStream ff(0);
	if(!ff.open((fname = fname_).c_str(), XS_IN))
		throw logic_error(string("File not found: ") + fname);
	int len = ff.size();
	if(buffer)
		delete buffer;
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
				if(Token::lastTokenCreated()){
					while(*i == ' ' || *i == '\t')
						++i;
					const char* marker = i;
					while(*i != '\r' && *i != '\n' && *i != 0)
						++i;
					char symbol = *i;
					*i = 0;
					Token::lastTokenCreated()->setNameAlt(marker);
					*i = symbol;
				}
				else{
					if((i = strstr(i, "\n")) == 0) 
						return 0;
				}
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
			if(*i == '\n'){
				line++;
				Token::clearLastTokenCreated();
			}
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
			while(__iscsym(*j) || *j == '.' || (*j == '+' || *j == '-') && (*(j - 1) == 'E' || *(j - 1) == 'e'))
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
								if(*j == '#' && *(j + 1) == '#' || // ##
									*j == ':' && *(j + 1) == ':' || // ::
									*j == '&' && *(j + 1) == '&' || // &&
									*j == '|' && *(j + 1) == '|' || // ||
									*j == '.' && *(j + 1) == '*' || // .*
									*j == '+' && *(j + 1) == '+' || // ++
									*j == '-' && *(j + 1) == '-' || // --
									(*j == '+' || *j == '-' || *j == '*' || *j == '/' || *j == '%' || 
									*j == '^' || *j == '|' || *j == '&' || 
									*j == '!' || *j == '<' || *j == '>' || *j == '=') && *(j + 1) == '=') // x=
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

///////////////////////////////////////////////////////////////////////////
//		Compiler
///////////////////////////////////////////////////////////////////////////
string Compiler::full_refined_name;

Compiler::Compiler(bool dontDeclare)
{
	dontDeclare_ = dontDeclare;
	clear();
}

void Compiler::clear() 
{ 
	sections.clear(); 
	contexts.clear(); 
	parsers.clear(); 
	push_context(new TokenList(""));
	context().add(new DataTypeTemplate<Variable>("bool"));
	context().add(new DataTypeTemplate<Variable>("char"));
	context().add(new DataTypeTemplate<Variable>("unsigned_char", "unsigned char"));
	context().add(new DataTypeTemplate<Variable>("int"));
	context().add(new DataTypeTemplate<Variable>("unsigned_int", "unsigned int"));
	context().add(new DataTypeTemplate<Variable>("long"));
	context().add(new DataTypeTemplate<Variable>("unsigned_long", "unsigned long"));
	context().add(new DataTypeTemplate<Variable>("float"));
	context().add(new DataTypeTemplate<Variable>("double"));
	context().add(new DataTypeTemplate<Variable>("string", "PrmString"));
	
	context().add(new DataTypeTemplate<Variable>("Vect2i"));
	context().add(new DataTypeTemplate<Variable>("Vect2f"));
	context().add(new DataTypeTemplate<Variable>("Vect3f"));
	context().add(new DataTypeTemplate<Variable>("QuatF"));
	context().add(new DataTypeTemplate<Variable>("Mat3f"));
	context().add(new DataTypeTemplate<Variable>("MatXf"));

	context().add(new PreprocessorToken);
	context().add(new CreateSectionToken);
	context().add(new StructToken);
	context().add(new EnumToken);
	context().add(new BitVectorToken);
	context().add(new ArrayToken);
	context().add(new VectorToken);
	context().add(new CustomToken);
	context().add(new InitializeToken(dontDeclare_));

	refine_name_first_token = 0;
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

	return token;
}

string Compiler::context_prefix() const 
{
	string prefix;
	for(ContextList::const_reverse_iterator ci = contexts.rbegin(); ci != contexts.rend();++ci)
	{
		const TokenList* context = *ci;
		if(dynamic_cast<const StructDataType*>(context))
			prefix = "::" + prefix;
		else
			break;
		prefix = context->name() + prefix;
	}
	return prefix;
}

int Compiler::parse_file(const char* fname, XBuffer& sout)
{
	int errors = 0;
	sout < "XPrmPro v" < _VERSION_ < ": compiling...\r\n" < fname < "\r\n";
	string full_name = exe_path + fname;
	parsers.clear();
	open(full_name.c_str());
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

	catch(expected_token exc){
		sout <= parser() < exc.what() < " expected\r\n";
		errors++;
		}
	catch(unexpected_token exc){
		sout <= parser() < "unexpected token: " < exc.what() < "\r\n";
		errors++;
		}
	catch(duplicate_token exc){
		sout <= parser() < "duplicate token: " < exc.what() < "\r\n";
		errors++;
		}
	catch(parsing_error exc){
		sout <= parser() < exc.what() < "\r\n";
		errors++;
		}

	return errors;
}

bool Compiler::compile(const char* fname)
{
	int errors = 0;
	try {
		XBuffer bout(1024, 1);
		errors = parse_file(fname, bout);
		cout << bout;
		if(!errors){
			SectionList::iterator i;
			FOR_EACH(sections, i){
				(*i)->declaration(); 
				(*i)->definition();
				}
			}
		else
			cout << fname << ": " << errors << " error(s)" << endl;
		}
	catch(logic_error exc){
		cout << exc.what() << "\r\n";
		errors++;
		}
	
	return !errors;
}				      

///////////////////////////////////////////////////////////////////////////
//		TokenList
///////////////////////////////////////////////////////////////////////////
TokenList::TokenList(const TokenList& tokens, const char* name) 
: Token(name ? name : tokens.name()),
initialize_(false)
{
	parent_ = 0;
	const_iterator i;
	FOR_EACH(tokens, i)
		add(*i);
}

const Token* TokenList::find(const char* name) const 
{ 
	Map::const_iterator i = map.find(name); 
	if(i != map.end())
		return i->second;
	if(parent())
		return parent()->find(name);

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
	if(strlen(token->name()) && map.find(token->name()) != map.end()) 
		throw duplicate_token(token->name()); 
	else { 
		push_back(token); 
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


///////////////////////////////////////////////////////////////////////////
//		Section
///////////////////////////////////////////////////////////////////////////
int Section::variables() const
{
	int cnt = 0;
	const_iterator i;
	FOR_EACH(*this, i){
		const Variable* var = dynamic_cast<const Variable*>(&**i);
		if(var)
			cnt++;
		}
	return cnt;
}

void Section::declare(WriteStream& buf, int declMode) const
{
	const_iterator i;
	FOR_EACH(*this, i)
		if((*i)->type() & declMode)
			(*i)->declaration(buf, declMode & DECLARE_EXTERN);
	buf();
}

void Section::declaration()
{
	WriteStream buf;
	buf() < "//////////////////////////////////////////////////////////////////////////////////////////////";
	buf() < "//	XScript declaration";
	buf() < "//	Section: " < name();
	buf() < "//	Number of variables: " <= variables();
	buf() < "//	This is computer generated code, don't try to change it";
	buf() < "//////////////////////////////////////////////////////////////////////////////////////////////";
	
	string prepName = "_XPRM_SECTION_";
	prepName += name();

	buf() < "#ifndef " < prepName;
	buf() < "#define " < prepName;
	buf();

	buf() < "#include \"XPrmPro.h\"";
	buf();

	declare(buf, DECLARE_DATA_TYPES);

	if(globalDeclaration_)
		declare(buf, DECLARE_VARIABLES | DECLARE_EXTERN);
	
	buf() < "struct " < name() < " {";
	buf.push();
	
	if(!globalDeclaration_)
		declare(buf, DECLARE_VARIABLES);
	
	buf() < name() < "();";
	buf() < "TreeNode* edit() const;";
	buf() < "void apply(const TreeNode* node);";
	buf() < "friend ScriptParser& operator<<(ScriptParser& parser, const " < name() < "& data);";
	buf() < "friend ScriptParser& operator>>(ScriptParser& parser, " < name() < "& data);";
	if(initialize_)
		buf() < "void initialize();";
	buf.pop();
	buf() < "};";
	buf();

	EnumList::writeDescriptorPrototypes(buf);
	buf();

	buf() < "#endif //" < prepName;
	
	buf() < "//////////////////////////////////////////////////////////////////////////////////////////////";
	buf() < "//	XScript end: " < name();
	buf() < "//////////////////////////////////////////////////////////////////////////////////////////////";
	
	cout << "Creating declaration of section \"" << name() << "\" in " << declaration_file << endl;
	XStream ff(declaration_file.c_str(), XS_OUT);
	ff < buf.c_str();
}

void Section::definition()
{
	WriteStream buf;
	buf() < "//////////////////////////////////////////////////////////////////////////////////////////////";
	buf() < "//	XScript definition";
	buf() < "//	Section: " < name();
	buf() < "//	Number of variables: " <= variables();
	buf() < "//	This is computer generated code, don't try to change it";
	buf() < "//////////////////////////////////////////////////////////////////////////////////////////////";

	buf() < "#include <typeinfo>";
	buf() < "#include \"XPrmProInternal.h\"";
	buf();

	xassert(parent());
	const_iterator i;
	FOR_EACH(*parent(), i)
		if((*i)->type() == DATA_TYPE && dynamic_cast<const EnumList*>(&**i))
			(*i)->definition(buf, "");

	if(globalDeclaration_)
		declare(buf, DECLARE_VARIABLES);

	string varName = variable_name(name());
	string parentPrefix = globalDeclaration_ ? "" : varName + ".";

	FOR_EACH(*this, i)
		if((*i)->type() == DATA_TYPE)
			(*i)->definition(buf, parentPrefix.c_str());

	buf() < name() < "::" < name() < "()";
	buf() < "{";
	buf.push();
	
	FOR_EACH(*this, i)
		if((*i)->type() == VARIABLE)
			(*i)->initialization(buf);

	if(initialize_){
		buf();
		buf() < "initialize();";
	}
	
	buf.pop();
	buf() < "}";
	buf();

	// Save
	buf() < "ScriptParser& operator<<(ScriptParser& parser, const " < name() < "& " < varName.c_str() < ")";
	buf() < "{";
	buf.push();
	buf() < "parser << \"" < name() < "\" << ASSIGNMENT << OPEN_BRACKET;";
	
	FOR_EACH(*this, i)
		if((*i)->type() == VARIABLE)
			(*i)->save(buf, parentPrefix.c_str());
	
	buf() < "parser << CLOSE_BRACKET << SEMICOLON;";
	buf() < "return parser;";
	buf.pop();
	buf() < "}";
	buf();
	
	// Load
	buf() < "ScriptParser& operator>>(ScriptParser& parser, " < name() < "& " < varName.c_str() < ")";
	buf() < "{";
	buf.push();
	buf() < "parser >> \"" < name() < "\" >> ASSIGNMENT >> OPEN_BRACKET;";
	buf() < "SmartName smartNameVar(\"" < name() < "\");";
	buf() < "parser >> smartNameVar;";
	buf() < "string smartNamePrev = smartNameVar;";
	buf() < "for(;;){";
	buf.push();
	
	FOR_EACH(*this, i)
		if((*i)->type() == VARIABLE)
			(*i)->load(buf, parentPrefix.c_str());
	
	buf() < "if(smartNameVar == \"}\")";
	buf() < "\tbreak;";
	
	buf() < "if(smartNameVar == smartNamePrev)";
	buf() < "\tparser >> SKIP_VALUE >> SEMICOLON >> smartNameVar;";
	buf() < "else";
	buf() < "\tsmartNamePrev = smartNameVar;";
	buf.pop();
	buf() < "}";
	
	buf() < "parser >> SEMICOLON;";

	if(initialize_){
		buf();
		buf() < parentPrefix < "initialize();";
	}

	buf() < "return parser;";
	buf.pop();
	buf() < "}";
	buf();

	// Edit
	buf() < "TreeNode* " < name() < "::edit() const";
	buf() < "{";
	buf.push();
	buf() < "TreeNode* treeNode = new TreeNode(\"" < name() < "\", \"\", \"" < name() < "\");";
	FOR_EACH(*this, i)
		if((*i)->type() == VARIABLE){
			if((*i)->hasAlt()){
				buf() < "treeNode->push_back(createNode(!useAlternativeNames() ? \"" < (*i)->name() < "\" : \"" < (*i)->nameAlt() < "\", " < (*i)->name() < "));";
			}
			else{
				buf() < "treeNode->push_back(createNode(\"" < (*i)->name() < "\", " < (*i)->name() < "));";
				buf() < "if(useAlternativeNames())";
				buf() < "\ttreeNode->children().back()->setHidden();";
			}
		}
	buf() < "return treeNode;";
	buf.pop();
	buf() < "}";
	buf();

	// Apply
	buf() < "void " < name() < "::apply(const TreeNode* treeNode)";
	buf() < "{";
	buf.push();
	buf() < "TreeNode::const_iterator i = treeNode->children().begin();";
	FOR_EACH(*this, i)
		if((*i)->type() == VARIABLE){
			buf() < "applyNode(" < (*i)->name() < ", *i);";
			buf() < "++i;";
		}
	buf.pop();
	buf() < "}";
	buf();



	buf() < "//////////////////////////////////////////////////////////////////////////////////////////////";
	buf() < "//	XScript end: " < name();
	buf() < "//////////////////////////////////////////////////////////////////////////////////////////////";

	cout << "Creating definition of section \"" << name() << "\" in " << definition_file << endl;
	XStream ff(definition_file.c_str(), XS_OUT);
	ff < buf.c_str();
}

/////////////////////////////////////////////////////////////////////////////////////////////
//		Array
/////////////////////////////////////////////////////////////////////////////////////////////
/*
void ArrayVariable::init(Compiler& comp) 
{  
	Variable::init(comp);
	int first_init = empty();
	iterator it = begin();
	current_size = 0;
	comp.skip_token("{");
	string token = comp.get_token();
	if(token != string("}")){
		comp.parser().put_token();
		while(1){
			Variable* v;
			if(first_init){
//				v = type.create("");
//				push_back(v);
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
			string token = comp.get_token();
			if(token == "}")
				break;
			if(token != string(","))
				throw unexpected_token(token.c_str());
			}
		}

	if(!size_var_name.empty())
		const_cast<IntVariable*>(dynamic_cast<const IntVariable*>(comp.context().find_local(size_var_name.c_str())))->set(current_size);

	while(size() < declare_size){ // добавляем недостающие элементы
//		Variable* v = type.create("");
//		push_back(v);
	}
}

void ArrayVariable::write_value(XBuffer& buf) const 
{ 
	const_iterator i;
	buf() < "{ ";
	FOR_EACH(*this, i){
		(*i)->write_value(buf);
		buf() < ", ";
		}
	if(!empty())
		buf -= 2;
	buf() < " }";
}
*/

///////////////////////////////////////////////////////////////////////////
//				Pointer
///////////////////////////////////////////////////////////////////////////
/*
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
				static DataTypeTemplate<IntVariable> IntDataType("int", "int");
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
//			Variable* var = type->create(variable_refine_name.c_str());
//			var->new_emulation_flag = 1;
//			comp.add_new_emulated(var);
//			var->refine_name_prefix = refine_name_prefix;
//			var->init(comp);
//			variable = var;
//			if(!size_var_name.empty())
//				const_cast<IntVariable*>(dynamic_cast<const IntVariable*>(comp.context().find_local(size_var_name.c_str())))->set(1);
		}
	}
	else
		throw unexpected_token(comp.get_token());
}
*/
///////////////////////////////////////////////////////////////////////////
//		Struct
///////////////////////////////////////////////////////////////////////////
StructDataType::StructDataType(Compiler& comp) 
: Token(comp.get_token()), 
TokenList(name()),
DataTypeTemplate<StructVariable>(name()),
baseClass_(0)
{ 
	comp.context().add(this);

	if(comp.get_token_if(":")){
		const StructDataType* base = dynamic_cast<const StructDataType*>(comp.context().find(comp.get_token()));
		if(!base)
			throw logic_error("Unable to inherit from struct");

		const_cast<StructDataType*>(base)->addDerived(this);
	}

	comp.skip_token("{"); 

	add(new DelegateToken(comp.dontDeclare()));
	
	parse(comp); 
}

void StructDataType::addDerived(StructDataType* derived)
{
	derived->baseClass_ = this;

	StructDataType* base = this;
	while(base->baseClass())
		base = base->baseClass();

	base->derivedClasses_.push_back(derived);
}

const char* StructDataType::mostBaseClassName() const
{
	return baseClass() ? baseClass()->mostBaseClassName() : name();
}

void StructDataType::declaration(WriteStream& buf, bool declExtern) const 
{
	if(!baseClass()){
		if(!base())
			buf() < "struct " < name() < " {";
		else
			buf() < "struct " < name() < " : ShareHandleBase {";
	}
	else
		buf() < "struct " < name() < " : " < baseClass()->name() < " {";
	buf.push();
	const_iterator i;
	FOR_EACH(*this, i)
		(*i)->declaration(buf);

	buf();
	buf() < name() < "();";
	string varName = variable_name(name());
	if(base()){
		buf() < "virtual ~" < name() < "() {}";
		buf() < "virtual TreeNode* edit(const char* treeNodeName) const;";
		buf() < "virtual void apply(const TreeNode* treeNode);";
		buf() < "virtual void save(ScriptParser& parser) const;";
		buf() < "virtual void load(ScriptParser& parser);";
	}
	else{
		buf() < "TreeNode* edit(const char* treeNodeName) const;";
		buf() < "void apply(const TreeNode* treeNode);";
		buf() < "void save(ScriptParser& parser) const;";
		buf() < "void load(ScriptParser& parser);";
	}

	if(initialize_)
		buf() < "void initialize();";

	buf.pop();
	if(!delegate_code.empty())
		buf() < delegate_code.c_str();
	buf() < "};";
	buf();
}

void StructDataType::definition(WriteStream& buf, const char* parentPrefix) const
{
	string varName = variable_name(name());

	const_iterator i;
	FOR_EACH(*this, i)
		if((*i)->type() == DATA_TYPE)
			(*i)->definition(buf, parentPrefix);
		
	// Initialization
	buf() < name() < "::" < name() < "()";
	buf() < "{";
	buf.push();
	
	FOR_EACH(*this, i)
		if((*i)->type() == VARIABLE)
			(*i)->initialization(buf);
			
	if(initialize_){
		buf();
		buf() < "initialize();";
	}

	buf.pop();
	buf() < "}";
	buf();

	// Save
	buf() < "void " < name() < "::save(ScriptParser& parser) const ";
	buf() < "{";
	buf.push();

	buf() < "parser << OPEN_BRACKET;";

	saveRecursive(buf);

	buf() < "parser << CLOSE_BRACKET;";
	buf.pop();
	buf() < "}";
	buf();

	buf() < "inline ScriptParser& operator<<(ScriptParser& parser, const " < name() < "& " < varName < ")";
	buf() < "{ ";
	buf() < "\t" < varName < ".save(parser);";
	buf() < "\treturn parser;";
	buf() < "}";
	buf();

	// Load
	buf() < "void " < name() < "::load(ScriptParser& parser)";
	buf() < "{";
	buf.push();
	buf() < "parser >> OPEN_BRACKET;";
	buf() < "SmartName smartNameVar(\"" < mostBaseClassName() < "\");";
	buf() < "parser >> smartNameVar;";
	buf() < "string smartNamePrev = smartNameVar;";
	buf() < "for(;;){";
	buf.push();
	
	loadRecursive(buf);

	buf() < "if(smartNameVar == \"}\")";
	buf() < "\tbreak;";
	
	buf() < "if(smartNameVar == smartNamePrev)";
	buf() < "\tparser >> SKIP_VALUE >> SEMICOLON >> smartNameVar;";
	buf() < "else";
	buf() < "\tsmartNamePrev = smartNameVar;";
	buf.pop();
	buf() < "}";

	if(initialize_){
		buf();
		buf() < "initialize();";
	}

	buf.pop();
	buf() < "}";
	buf();

	buf() < "inline ScriptParser& operator>>(ScriptParser& parser, " < name() < "& " < varName < ")";
	buf() < "{ ";
	buf() < "\t" < varName < ".load(parser);";
	buf() < "\treturn parser;";
	buf() < "}";
	buf();

	// Edit
	buf() < "TreeNode* " < name() < "::edit(const char* treeNodeName) const";
	buf() < "{";
	buf.push();
	buf() < "TreeNode* treeNode = new TreeNode(treeNodeName ? treeNodeName : \"\", !useAlternativeNames() ? \"" < name() < "\" : \"" < nameAlt() < "\", \"" < name() < "\", TreeNode::STATIC);";
	editRecursive(buf);
	buf() < "return treeNode;";
	buf.pop();
	buf() < "}";
	buf();

	buf() < "inline TreeNode* createNode(const char* name, const " < name() < "& " < varName < ")";
	buf() < "{ ";
	buf() < "\treturn " < varName < ".edit(name);";
	buf() < "}";
	buf();

	// Apply
	buf() < "void " < name() < "::apply(const TreeNode* treeNode)";
	buf() < "{";
	buf.push();
	buf() < "TreeNode::const_iterator i = treeNode->children().begin();";
	applyRecursive(buf);
	buf.pop();
	buf() < "}";
	buf();
	
	buf() < "inline void applyNode(" < name() < "& " < varName < ", const TreeNode* treeNode)";
	buf() < "{ ";
	buf() < "\t" < varName < ".apply(treeNode);";
	buf() < "}";
	buf();

	if(base()){
		// Save
		buf() < "inline void savePointer(ScriptParser& parser, const " < name() < "* " < varName < ")";
		buf() < "{";
		buf() < "\tparser << typeid(*" < varName < ").name() << SPACE << *" < varName < ";";
		buf() < "}";
		buf();

		// Load
		buf() < "inline void loadPointer(ScriptParser& parser, " < name() < "*& " < varName < ")";
		buf() < "{";
		buf.push();
		buf() < "SmartName typeName(\"" < name() < "\");";
		buf() < "parser >> typeName;";
		buf() < "if(typeName == typeid(" < name() < ").name())";
		buf() < "\t" < varName < " = new " < name() < ";";
		StructList::const_iterator si;
		FOR_EACH(derivedClasses_, si){
			buf() < "else if(typeName == typeid(" < (*si)->name() < ").name())";
			buf() < "\t" < varName < " = new " < (*si)->name() < ";";
		}
		buf() < "else";
		buf() < "\txassertStr(typeName == \"\" && \"XPrmPro: Unknown type will be ignored\", (typeName.parentName() + \"::\" + typeName).c_str());";
		buf();
		buf() < "if(" < varName < ")";
		buf() < "\tparser >> *" < varName < ";";
		buf() < "else";
		buf() < "\tparser >> SKIP_VALUE;";
		buf.pop();
		buf() < "}";
		buf();

		// Edit
		buf() < "const TreeNode* treeNodeSelector" < name() < "(int typeIndex)";
		buf() < "{";
		buf.push();

		int typeSize = 1 + derivedClasses_.size();
		buf() < "xassert(typeIndex >= 0 && typeIndex < " <= typeSize < ");";
		buf();
		buf() < "static TreeNode* treeNodes[" <= typeSize + 1 < "] = {";
		buf.push();
		buf() < "createNode(0, " < name() < "()),";
		FOR_EACH(derivedClasses_, si)
			buf() < "createNode(0, " < (*si)->name() < "()),";
		buf() < "0";
		buf.pop();
		buf() < "};";
		buf();
		buf() < "return treeNodes[typeIndex];";
		buf.pop();
		buf() < "}";
		buf();
		
		buf() < "inline TreeNode* createNode(const char* name, const ShareHandle<" < name() < ">& " < varName < ")";
		buf() < "{";
		buf.push();
		buf() < "if(!name)";
		buf() < "\tname = \"\";";
		buf();
		buf() < "static string comboList;";
		buf() < "static string comboListAlt;";
		buf() < "static bool inited = false;";
		buf() < "if(!inited){";
		buf() < "\tinited = true;";
		buf() < "\tcomboList += \"" < name() < "\";";
		buf() < "\tcomboListAlt += \"" < nameAlt() < "\";";
		FOR_EACH(derivedClasses_, si){
			buf() < "\tcomboList += \"|\";";
			buf() < "\tcomboList += \"" < (*si)->name() < "\";";
			buf() < "\tcomboListAlt += \"|\";";
			buf() < "\tcomboListAlt += \"" < (*si)->nameAlt() < "\";";
		}
		buf() < "}";

		buf() < "TreeNode* treeNode = new TreeNode(name, \"\", \"" < name() < "*\", TreeNode::POLYMORPHIC, !useAlternativeNames() ? comboList.c_str() : comboListAlt.c_str(), 0, &treeNodeSelector" < name() < ");";
		buf() < "if(" < varName < "){";
		buf.push();
		buf() < "ShareHandle<TreeNode> tmpNode = createNode(\"\", *" < varName < ");";
		buf() < "treeNode->setValue(tmpNode->value().c_str());";
		buf() < "treeNode->setColor(tmpNode->color());";
		buf() < "TreeNode::const_iterator i;";
		buf() < "FOR_EACH(tmpNode->children(), i)";
		buf() < "\ttreeNode->push_back(*i);";
		buf.pop();
		buf() < "}";
		buf() < "return treeNode;";
		buf.pop();
		buf() < "}";
		buf();


		// Apply
		buf() < "inline void applyNode(ShareHandle<" < name() < ">& " < varName < ", const TreeNode* treeNode)";
		buf() < "{";
		buf.push();

		buf() < "string typeName = treeNode->value();";
		buf() < "if(typeName != \"\"){";
		buf.push();
		buf() < "if(" < varName < ")";
		buf() < "\t" < varName < " = 0;";
		buf() < "if(typeName == (!useAlternativeNames() ? \"" < name() < "\" : \"" < nameAlt() < "\"))";
		buf() < "\t" < varName < " = new " < name() < ";";
		FOR_EACH(derivedClasses_, si){
			buf() < "else if(typeName == (!useAlternativeNames() ? \"" < (*si)->name() < "\" : \"" < (*si)->nameAlt() < "\"))";
			buf() < "\t" < varName < " = new " < (*si)->name() < ";";
		}
		buf() < "else";
		buf() < "\txassert(0);";
		buf() < "applyNode(*" < varName < ", treeNode);";
		buf.pop();
		buf() < "}";
		buf() < "else";
		buf() < "\t" < varName < " = 0;";
		buf.pop();
		buf() < "}";
		buf();
	}
}

void StructDataType::saveRecursive(WriteStream& buf) const
{
	if(baseClass())
		baseClass()->saveRecursive(buf);
	
	const_iterator i;
	FOR_EACH(*this, i)
		if((*i)->type() == VARIABLE)
			(*i)->save(buf, "");
}

void StructDataType::loadRecursive(WriteStream& buf) const
{
	if(baseClass())
		baseClass()->loadRecursive(buf);
	
	const_iterator i;
	FOR_EACH(*this, i)
		if((*i)->type() == VARIABLE)
			(*i)->load(buf, "");
}

void StructDataType::editRecursive(WriteStream& buf) const
{
	if(baseClass())
		baseClass()->editRecursive(buf);
	
	const_iterator i;
	FOR_EACH(*this, i)
		if((*i)->type() == VARIABLE){
			if(strcmp((*i)->name(), "internalColor_")){
				if((*i)->hasAlt()){
					buf() < "treeNode->push_back(createNode(!useAlternativeNames() ? \"" < (*i)->name() < "\" : \"" < (*i)->nameAlt() < "\", " < (*i)->name() < "));";
				}
				else{
					buf() < "treeNode->push_back(createNode(\"" < (*i)->name() < "\", " < (*i)->name() < "));";
					buf() < "if(useAlternativeNames())";
					buf() < "\ttreeNode->children().back()->setHidden();";
				}
			}
			else
				buf() < "treeNode->setColor(internalColor_);";
		}
}

void StructDataType::applyRecursive(WriteStream& buf) const
{
	if(baseClass())
		baseClass()->applyRecursive(buf);
	
	const_iterator i;
	FOR_EACH(*this, i)
		if((*i)->type() == VARIABLE){
			if(strcmp((*i)->name(), "internalColor_")){
				buf() < "applyNode(" < (*i)->name() < ", *i);";
				buf() < "++i;";
			}
			else
				buf() < "internalColor_ = treeNode->color();";
		}
}


////////////////////////////////////////////////////////////////////////////////////////
//		Enum
////////////////////////////////////////////////////////////////////////////////////////
EnumList::UsedList EnumList::usedList_;
void EnumList::writeDescriptorPrototypes(WriteStream& buf)
{
	UsedList::const_iterator i;
	FOR_EACH(usedList_, i)
		(*i)->writeDescriptorPrototype(buf);
}

void EnumList::init(Compiler& comp)
{
	int value = 0;
	string name = comp.get_token(); 
	if(name != string("}"))
		for(;;){
			IntConstant* var = new IntConstant(name.c_str(), 0);
			string dir = comp.get_token();
			if(dir == "=")
				value = comp.eval_i();
			var->setValue(value++);
			comp.context().add(var);
			push_back(var);
			if(dir == "=")
				dir = comp.get_token();
			if(dir == "}")
				break;
			if(dir == ","){
				name = comp.get_token();
				continue;
			}
			throw unexpected_token(dir.c_str());
		}
	comp.skip_token(";");
}

void EnumList::declaration(WriteStream& buf, bool declExtern) const 
{
	buf() < "enum " < name() < " {";
	buf.push();
	const_iterator i;
	int value = 0;
	FOR_EACH(*this, i){
		const IntConstant* var = *i;
		buf() < var->name();
		if(value != *var)
			buf.buffer() < " = " <= (value = *var); 
		buf.buffer() < ",";
		value++;
	}
	buf.pop();
	if(!empty())
		buf.buffer() -= 1;
	buf() < "};";
	buf();
}

void EnumList::writeDescriptorPrototype(WriteStream& buf) const
{
	buf() < "const EnumDescriptor<" < prefix < name() < ">& getEnumDescriptor(" < prefix < name() < " type);";
}

void EnumList::definition(WriteStream& buf, const char* parentPrefix) const
{
	if(used_){
		string prepName = "_XPRM_ENUM_";
		if(!prefix.empty()){
			prepName += prefix;
			prepName.replace(prepName.size() - 2, 2, "_");
		}
		prepName += name();
		buf();
		buf() < "#ifndef " < prepName;
		buf() < "#define " < prepName;
		buf();

		string varName = variable_name(name());

		// EnumDescriptor
		string descriptorType = prefix;
		if(!descriptorType.empty())
			descriptorType.erase(descriptorType.size() - 2, 2);
		descriptorType += name();
		string descriptorName = "enum" + descriptorType;
		descriptorType = "Enum" + descriptorType;
		buf() < "struct " < descriptorType < " : EnumDescriptor<" < prefix < name() < ">";
		buf() < "{";
		buf() < "\t" < descriptorType < "();";
		buf() < "};";
		buf();

		buf() < "const EnumDescriptor<" < prefix < name() < ">& getEnumDescriptor(" < prefix < name() < " type)";
		buf() < "{";
		buf() < "\tstatic " < descriptorType < " descriptor;";
		buf() < "\treturn descriptor;";
		buf() < "}";
		buf();

		buf() < descriptorType < "::" < descriptorType < "()";
		buf() < ": EnumDescriptor<" < prefix < name() < ">(\"" < name() < "\")";
		buf() < "{";
		const_iterator i;
		FOR_EACH(*this, i)
			buf() < "\tadd(" < prefix < (*i)->name() < ", \"" < (*i)->name() < "\", \"" < (*i)->nameAlt() < "\");";
		buf() < "}";
		buf();

		buf() < "#endif //" < prepName;
		buf();
	}
}

void EnumList::checkEnumId(const char* name) const
{
	const_iterator i;
	FOR_EACH(*this, i)
		if(!strcmp((*i)->name(), name))
			return;

	throw logic_error("Unable to convert to enum");
}


void EnumToken::affect(Compiler& comp) const
{
	string enum_name = comp.get_token();
	if(enum_name == "{")
		enum_name = "";
	else{
		if(!is_name(enum_name.c_str()))
			throw parsing_error("Enum name expected");
		comp.skip_token("{");
	}
	EnumList* enum_list = new EnumList(enum_name.c_str(), comp.context_prefix().c_str());
	comp.context().add(enum_list);
	enum_list->init(comp);
}


///////////////////////////////////////////////////////////////////////////
//		BitVector
///////////////////////////////////////////////////////////////////////////
void BitVectorVariable::init(Compiler& comp)
{
	if(comp.get_token_if(";"))
		return;

	comp.skip_token("="); 

	for(;;){
		const char* name = comp.get_token(); 
		dynamic_cast<const EnumList&>(dataType_).checkEnumId(name);
		defaultValue_ += name;

		if(comp.get_token_if(";"))
			return;

		comp.skip_token("|");
		defaultValue_ += " | ";
	}
}

void BitVectorToken::affect(Compiler& comp) const
{
	comp.skip_token("<");

	const EnumList* enumList = dynamic_cast<const EnumList*>(comp.context().find(comp.get_token()));
	if(!enumList)
		throw logic_error("Unable to convert enum type");

	comp.skip_token(">");

	enumList->setUsed();

	BitVectorVariable* var = new BitVectorVariable(comp.get_token(), *enumList);
	var->init(comp);
	comp.context().add(var);
}

///////////////////////////////////////////////////////////////////////////
//		Vector
///////////////////////////////////////////////////////////////////////////
void ArrayToken::affect(Compiler& comp) const
{
	comp.skip_token("<");
	
	const DataType* dataType = dynamic_cast<const DataType*>(comp.refine_name(comp.context().find(comp.get_token())));
	if(!dataType)
		throw logic_error("Unable to convert data type");

	const EnumList* enumList = dynamic_cast<const EnumList*>(dataType);
	if(enumList)
		enumList->setUsed();

	Variable* var;
	if(comp.get_token_if("*")){
		comp.skip_token(",");
		int size = comp.eval_i();
		comp.skip_token(">");
		var = new ArrayPointerVariable(comp.get_token(), *dataType, size);
	}
	else{
		comp.skip_token(",");
		int size = comp.eval_i();
		comp.skip_token(">");
		var = new ArrayVariable(comp.get_token(), *dataType, size);
	}
	
	var->init(comp);
	comp.context().add(var);
}

///////////////////////////////////////////////////////////////////////////
//		Vector
///////////////////////////////////////////////////////////////////////////
void VectorToken::affect(Compiler& comp) const
{
	comp.skip_token("<");
	
	const DataType* dataType = dynamic_cast<const DataType*>(comp.refine_name(comp.context().find(comp.get_token())));
	if(!dataType)
		throw logic_error("Unable to convert data type");

	const EnumList* enumList = dynamic_cast<const EnumList*>(dataType);
	if(enumList)
		enumList->setUsed();

	Variable* var;
	if(comp.get_token_if("*")){
		comp.skip_token(">");
		var = new VectorPointerVariable(comp.get_token(), *dataType);
	}
	else{
		comp.skip_token(">");
		var = new VectorVariable(comp.get_token(), *dataType);
	}
	
	var->init(comp);
	comp.context().add(var);
}

///////////////////////////////////////////////////////////////////////////
//		Custom
///////////////////////////////////////////////////////////////////////////
void CustomToken::affect(Compiler& comp) const
{
	comp.skip_token("<");
	string customValue = comp.get_token();
	comp.skip_token(">");

	Variable* var = new CustomVariable(comp.get_token(), *this, customValue.c_str());
	
	var->init(comp);
	comp.context().add(var);
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
				if(fabs(d) < 1e-10)
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

		throw parsing_error("Unable to convert to double");
		}
	else
		throw unexpected_token(name);
}

///////////////////////////////////////////////////////////////////////////
//		String Calculator
///////////////////////////////////////////////////////////////////////////
string Compiler::eval_s()
{
	return expr_s();
}

string Compiler::expr_s()
{
	string  left = prim_s();

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

string Compiler::prim_s()
{
	const char* name = get_token(); 
	
	if(*name == '"') // String Literal
		return strip_string(name);
	
	const Token* token = refine_name(context().find(name));
	throw unexpected_token(name);
}


