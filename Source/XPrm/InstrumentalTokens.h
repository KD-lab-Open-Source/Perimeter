
////////////////////////////////////////////////////////////////////////////////////////
//		Preprocessor 
////////////////////////////////////////////////////////////////////////////////////////
struct IncludeToken : Token 
{
	IncludeToken() : Token("include") {}

	void affect(Compiler& comp) const 
	{
        std::string path = convert_path_xprm(comp.parser().file_name());
        size_t pos = path.rfind(PATH_SEP) + 1;
		path.erase(pos, path.size() - pos);
		path += strip_string(comp.get_token());
		comp.open(path.c_str());
		}
};

struct PreprocessorToken : TokenList 
{
	PreprocessorToken() :  Token("#"), TokenList(0)
	{
		add(new IncludeToken);
	}

	void affect(Compiler& comp) const 
	{
		run(comp.get_token(), comp);
	}
};

////////////////////////////////////////////////////////////////////////////////////////
//		Section header 
////////////////////////////////////////////////////////////////////////////////////////
struct ParseSectionDeclarationToken : Token
{
	ParseSectionDeclarationToken() : Token("declaration") {}
	void affect(Compiler& comp) const
	{
		comp.skip_token("=");
		comp.section().declaration_file = convert_path_xprm(strip_string(comp.get_token()).c_str());
	}
};

struct ParseSectionDefinitionToken : Token
{
	ParseSectionDefinitionToken() : Token("definition") {}
	void affect(Compiler& comp) const
	{
		comp.skip_token("=");
		comp.section().definition_file = convert_path_xprm(strip_string(comp.get_token()).c_str());
	}
};

struct ParseSectionScriptPathToken : Token
{
	ParseSectionScriptPathToken() : Token("source") {}
	void affect(Compiler& comp) const
	{
		comp.skip_token("=");
		comp.section().script_file = convert_path_xprm(strip_string(comp.get_token()).c_str());
	}
};

struct ParseSectionUsingNamespaceToken : Token
{
	ParseSectionUsingNamespaceToken() : Token("using_namespace") {}
	void affect(Compiler& comp) const
	{
		comp.section().using_namespace = 1;
	}
};

struct CreateSectionToken : TokenList 
{
	CreateSectionToken() : Token("section"), TokenList(0)
	{
		add(new ParseSectionDeclarationToken);
		add(new ParseSectionDefinitionToken);
		add(new ParseSectionScriptPathToken);
		add(new ParseSectionUsingNamespaceToken);
	}

	void affect(Compiler& comp) const
	{
		comp.create_section(comp.get_token());
		comp.skip_token("(");
		int first_directive = 1;
		for(;;){
			const char* directive = comp.get_token();
			
			if(!strcmp(directive, ")"))
				break;
			
			if(!first_directive && !strcmp(directive, ","))
				directive = comp.get_token();
			first_directive = 0;

			run(directive, comp);
			}

		comp.skip_token("{");

		if(comp.section().declaration_file.size() == 0)
			throw parsing_error("Declaration file undefined");

		if(comp.section().definition_file.size() == 0)
			throw parsing_error("Definition file undefined");

		comp.section().parse(comp);
	}
};

////////////////////////////////////////////////////////////////////////////////////////
//		Enum
////////////////////////////////////////////////////////////////////////////////////////
class EnumVariable : public Variable, public IntConstant
{
    std::string enum_value;
public:
	EnumVariable(const char* name, const DataType& type) : Token(name), Variable(type), IntConstant(name, 0) {}
	void init(Compiler& comp);
	void write_value(WriteStream& buf) const { buf < enum_value.c_str(); }
	void copy_value(void* val) const { *(int*)val = value; }
	Token* clone() const { return new EnumVariable(*this); } 
};

enum _any_enum_name_ {};

struct EnumList : DataType, std::list<ShareHandle<IntConstant> >
{
	std::string prefix;
	int dont_declare;
	EnumList(const char* name, const char* prefix_, bool dont_declare_) : Token(name), DataType(name, sizeof(_any_enum_name_)), prefix(prefix_), dont_declare(dont_declare_) {}
	void declaration(XBuffer& buf) const 
	{
		if(dont_declare)
			return;
		buf < "enum " < name() < " {\r\n";
		const_iterator i;
		int value = 0;
		FOR_EACH(*this, i){
			const IntConstant* var = *i;
			buf < var -> name();
			if(value != *var)
				buf < " = " <= (value = *var); 
			buf < ",\r\n";
			value++;
			}
		buf -= 3;
		buf < "\r\n};\r\n";
	}
	Variable* create(const char* name) const { return new EnumVariable(name, *this); }
};

class EnumToken : public Token 
{
	mutable bool make_static;
public:
	EnumToken() : Token("enum"), make_static(false) {}
	void affect(Compiler& comp) const
	{
        std::string enum_name = comp.get_token();
		if(enum_name == "{")
			enum_name = "";
		else{
			if(!is_name(enum_name.c_str()))
				throw parsing_error("Enum name expected");
			comp.skip_token("{");
			}
		EnumList* enum_list = new EnumList(enum_name.c_str(), comp.context_prefix().c_str(), make_static);
		make_static = false;
		comp.context().add(enum_list);
		int value = 0;
        std::string name = comp.get_token(); 
		if(name != std::string("}"))
			for(;;){
                std::string dir = comp.get_token();
				if(dir == "=")
					value = comp.eval_i();
				IntConstant* var = new IntConstant(name.c_str(), value++);
				comp.context().add(var);
				enum_list -> push_back(var);
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
	void makeStatic() const { make_static = true; }
};

//////////////////////////////////////////////////////////////////////
//	Delegate
//////////////////////////////////////////////////////////////////////
struct DelegateToken : Token
{
	DelegateToken() : Token("delegate") {}
	void affect(Compiler& comp) const
	{
		comp.skip_token(":");
		int blocks = 0;
		const char* begin = comp.parser() + 1;
		while(1){
            std::string token = comp.get_token();
			if(token == "{")
				blocks++;
			if(token == "}" && !blocks--){
					comp.parser().put_token();
					dynamic_cast<StructDataType&>(comp.context()).delegate_code = begin;
					return;
					}
			}
	}
};

//////////////////////////////////////////////////////////////////////
//	Typedef: 
// typedef float real;
//////////////////////////////////////////////////////////////////////
struct TypedefType : Token
{
	const DataType& type;
	TypedefType(const char* name, const DataType& type_) : Token(name), type(type_) {}
	void affect(Compiler& comp) const
	{
		type.affect(comp);
	}
};

struct TypedefToken : Token
{
	TypedefToken() : Token("typedef") {}
	void affect(Compiler& comp) const
	{
		const DataType* type = dynamic_cast<const DataType*>(comp.context().find(comp.get_token()));
		if(!type)
			throw parsing_error("Typedef: type expected");

		comp.context().add(new TypedefType(comp.get_token(), *type));
		comp.skip_token(";");
	}
};

////////////////////////////////////////////////////////////////////////////////////
//	Static declaration
////////////////////////////////////////////////////////////////////////////////////
struct StaticToken : Token
{
	StaticToken() : Token("static") {}
	void affect(Compiler& comp) const
	{
		const Token* token = comp.refine_name(comp.context().find(comp.get_token()));
		
		const EnumToken* enum_token = dynamic_cast<const EnumToken*>(token);
		if(enum_token)
		{
			enum_token->makeStatic();
			enum_token->affect(comp);
			return;
		}

		if(!dynamic_cast<const DataType*>(token) && !dynamic_cast<const TypedefType*>(token) && !dynamic_cast<const StructToken*>(token))
			throw parsing_error("Static: type expected");

		token -> affect(comp);
		Variable* var = dynamic_cast<Variable*>(&*comp.context().back());
		if(var)
			var -> static_flag = 1;
		else
		{
			StructDataType* str = dynamic_cast<StructDataType*>(&*comp.context().back());
			if(str)
				str->dont_declare = 1;
			else
				throw parsing_error("Static: internal error");
		}
	}
};

