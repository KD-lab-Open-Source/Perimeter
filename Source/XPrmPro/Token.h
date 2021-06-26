#pragma once
#include "ParseUtil.h"

#define _VERSION_	"2.15"

/////////////////////////////////////////////////////////////////////////////////////////////
//	  WriteStream 
/////////////////////////////////////////////////////////////////////////////////////////////
inline XBuffer& operator<(XBuffer& buf, const string& str) { buf < str.c_str(); return buf; }

class WriteStream
{
public:
	WriteStream() : buffer_(100000, 1), offset_("\r\n") {}
	void push() { offset_ += "\t"; }
	void pop() { offset_.pop_back(); }
	XBuffer& operator()() { buffer_ < offset_.c_str(); return buffer_; }
	XBuffer& buffer() { return buffer_; }
	char* c_str() { return buffer_.address(); }

private:
	string offset_;
	XBuffer buffer_;
};


/////////////////////////////////////////////////////////////////////////////////////////////
//			Token 
/////////////////////////////////////////////////////////////////////////////////////////////
class Token : public ShareHandleBase
{
public:
	enum Type {
		UNDEFINED = 0,
		VARIABLE = 1,
		DATA_TYPE = 2
		};

	enum DeclarationMode {
		DECLARE_VARIABLES = VARIABLE,
		DECLARE_DATA_TYPES = DATA_TYPE,
		DECLARE_EXTERN = DECLARE_DATA_TYPES << 1
	};

	Token(const char* name = 0) : name_(name), nameAlt_(name), type_(UNDEFINED) { assert(name); lastTokenCreated_ = this; }
	Token(const Token& token) : name_(token.name()), nameAlt_(token.nameAlt()) { lastTokenCreated_ = this; }
	virtual ~Token() {}
	const char* name() const { return name_.c_str(); }
	const char* nameAlt() const { return nameAlt_.c_str(); }
	bool hasAlt() const { return name_ != nameAlt_; }
	void setNameAlt(const char* nameAlt) { nameAlt_ = nameAlt; }
	Type type() const { return type_; }
	virtual void affect(class Compiler& comp) const { throw unexpected_token(name()); } // основное действие данного токена
	virtual void declaration(WriteStream& buf, bool declExtern = false) const {}
	virtual void definition(WriteStream& buf, const char* parentPrefix) const {}
	virtual void initialization(WriteStream& buf) const {}
	virtual void save(WriteStream& buf, const char* parentPrefix) const {}
	virtual void load(WriteStream& buf, const char* parentPrefix) const {}
	virtual void edit(WriteStream& buf) const {}
	virtual void apply(WriteStream& buf) const {}

	static Token* lastTokenCreated() { return lastTokenCreated_; }
	static void clearLastTokenCreated() { lastTokenCreated_ = 0; }

protected:
	string name_;
	string nameAlt_;
	Type type_;

	static Token* lastTokenCreated_;
};
		
/////////////////////////////////////////////////////////////////////////////////////////////
//			Token List
/////////////////////////////////////////////////////////////////////////////////////////////
class TokenList : public list<ShareHandle<Token> >, virtual public Token
{
public:
	TokenList(const char* name) : Token(name), initialize_(false) { parent_ = 0; }
	TokenList(const TokenList& tokens, const char* name = 0);
	const Token* find(const char* name) const;
	const Token* find_local(const char* name) const;
	void run(const char* name, Compiler& comp) const { find(name)->affect(comp); }
	void add(Token* token);
	void parse(class Compiler& comp);
	void clear() { list<ShareHandle<Token> >::clear(); map.clear(); } 

	void setParent(TokenList* parent) { parent_ = parent; }
	const TokenList* parent() const { return parent_; }

	void setInitialize() { initialize_ = true; }

protected:
	bool initialize_;
	
private:
	friend class Compiler;
	struct eqstr { bool operator()(const char* s1, const char* s2) const { return !strcmp(s1, s2); } };
	typedef hash_map<const char*, Token*, hash<const char*>, eqstr> Map;
	
	Map map;
	const TokenList* parent_;
};

/////////////////////////////////////////////////////////////////////////////////////////////
//			Section
/////////////////////////////////////////////////////////////////////////////////////////////
class Section : public TokenList
{
public:
	string definition_file;
	string declaration_file;
	string script_file;
	bool globalDeclaration_;

	Section(const char* name) : Token(name), TokenList(name), globalDeclaration_(false){}
	int variables() const;
	void definition();
	void declaration();

private:
	void declare(WriteStream& buf, int declMode) const;
};

/////////////////////////////////////////////////////////////////////////////////////////////
//			Parser
/////////////////////////////////////////////////////////////////////////////////////////////
class Parser 
{
public:
	Parser(const char* fname = 0);
	~Parser();
	void open(const char* fname);
	const char* file_name() const { return fname.c_str(); }	

	const char* get_token();	
	void put_token();	
	void skip_token(const char* token) { if(strcmp(get_token(), token)) throw expected_token(token); }
	friend XBuffer& operator<= (XBuffer& os, const Parser& p) { char full[MAX_PATH]; _fullpath( full, p.fname.c_str(), MAX_PATH ); os < full < '(' <= p.line < ") : "; return os; }
	operator const char*() const { return current_token; }
	
private:
	string fname;
	int line;
	char* buffer;
	char* current_token;
	char* next_token;
	char* prev_token;
	char replaced_symbol;
};

/////////////////////////////////////////////////////////////////////////////////////////////
//			Compiler
/////////////////////////////////////////////////////////////////////////////////////////////
class Compiler 
{
	typedef list<ShareHandle<TokenList> > ContextList;
	ContextList contexts;

	typedef list<ShareHandle<Section> > SectionList;
	SectionList sections;

	typedef list<Parser> ParserList;
	ParserList parsers;

	const Token* refine_name_first_token;
	static string full_refined_name;

	bool dontDeclare_;

	int or_i(); // |
	int xor_i(); // ^
	int and_i(); // &
	int shift_i(); // <<, >>
	int expr_i(); // +, -
	int term_i(); // *, /
	int prim_i(); // -, (), const, name

	double expr_f(); // +, -
	double term_f(); // *, /
	double prim_f(); // -, (), const, name

	string expr_s();
	string prim_s();

protected:
	string exe_path;

public:							
	Compiler(bool dontDeclare);
	void clear();
	int parse_file(const char* fname, XBuffer& sout); // returns the number of errors
	bool compile(const char* fname); // returns 1 if succeeds
	Section* getSection(const char* name);

	void create_section(const char* name);
	Section& section() { xassert(!sections.empty()); return *sections.back(); }

	void open(const char* fname) { parsers.push_back(Parser()); parser().open(fname); }
	Parser& parser() { xassert(!parsers.empty()); return parsers.back(); }

	TokenList& context() { return *contexts.back(); }
	void push_context(TokenList* cont) { cont->setParent(contexts.empty() ? 0 : &context()); contexts.push_back(cont); }
	void pop_context() { contexts.pop_back(); }
	string context_prefix() const;

	const char* get_token(int enable_eof = 0);
	void skip_token(const char* token) { if(strcmp(get_token(), token)) throw expected_token(token); }
	bool get_token_if(const char* token);
	
	const Token* refine_name(const Token* token, bool external_call = true);
	static const char* refined_name() { return full_refined_name.c_str(); }

	bool dontDeclare() const { return dontDeclare_; }

	int eval_i();
	double eval_f();
	string eval_s();
};
					    
/////////////////////////////////////////////////////////////////////////////////////////////
//		DataType
/////////////////////////////////////////////////////////////////////////////////////////////
class DataType : virtual public Token 
{
public:
	DataType(const char* name) : Token(name) { type_ = DATA_TYPE; }
	virtual const char* typeName() const { return name(); }
};

class Variable : public Token 
{
public:
	Variable(const char* name, const DataType& dataType) : Token(name), dataType_(dataType), typeName_(dataType.typeName()) { type_ = VARIABLE; }
	virtual void init(Compiler& comp)
	{ 
		defaultValue_ = "";
		if(comp.get_token_if("=")){
			string sep = "";
			while(!comp.get_token_if(";")){
				defaultValue_ += sep; 
				defaultValue_ += comp.get_token(); 
				sep = " ";
			}
		}
		else
			comp.skip_token(";"); 
	}
	void declaration(WriteStream& buf, bool declExtern) const 
	{ 
		buf() < (declExtern ? "extern " : "") < typeName() < " " < name() < ";"; 
	}
	void initialization(WriteStream& buf) const 
	{ 
		buf() < name() < " = " < defaultValue_.c_str() < ";"; 
	}
	void save(WriteStream& buf, const char* parentPrefix) const 
	{ 
		buf() < "parser << OFFSET << \"" < name() < "\" << ASSIGNMENT << " < parentPrefix < name() < " << SEMICOLON;"; 
	}
	void load(WriteStream& buf, const char* parentPrefix) const 
	{ 
		buf() < "if(smartNameVar == \"" < name() < "\")";
		buf() < "\tparser >> ASSIGNMENT >> " < parentPrefix < name() < " >> SEMICOLON >> smartNameVar;";
	}

	const char* typeName() const { return typeName_.c_str(); }

protected:
	const DataType& dataType_;
	string typeName_;
	string defaultValue_;
};


class PointerVariable : public Variable
{
public:
	PointerVariable(const char* name, const DataType& type) : Variable(name, type) { typeName_ = template_name("ShareHandle", typeName_); }
	void init(Compiler& comp) { Variable::init(comp); if(defaultValue_ == "") defaultValue_ = "0"; }
};

template <class VarType>
class DataTypeTemplate : public DataType 
{
public:
	DataTypeTemplate(const char* name, const char* typeName = 0) : Token(name), DataType(name), typeName_(typeName ? typeName : name) { type_ = DATA_TYPE; }
	void affect(Compiler& comp) const 
	{ 
		Variable* var;
		if(comp.get_token_if("*"))
			var = new PointerVariable(comp.get_token(), *this);
		else
			var = new VarType(comp.get_token(), *this);
		var->init(comp);
		comp.context().add(var);
	} 

	const char* typeName() const { return typeName_.c_str(); }

private:
	string typeName_;	
};

/////////////////////////////////////////////////////////////////////////////////////////////
//		Struct
/////////////////////////////////////////////////////////////////////////////////////////////
class StructVariable : public Variable
{
public:
	StructVariable(const char* name, const DataType& type) : Variable(name, type) {}
	void init(Compiler& comp) { comp.skip_token(";"); }
	void initialization(WriteStream& buf) const {}
};

class StructDataType : public DataTypeTemplate<StructVariable>, public TokenList
{
public:
	StructDataType(Compiler& comp);
	void declaration(WriteStream& buf, bool declExtern) const;
	void definition(WriteStream& buf, const char* parentPrefix) const;

	void addDerived(StructDataType* derived);
	bool base() const { return !derivedClasses_.empty(); }
	StructDataType* baseClass() const { return baseClass_; }
	
	void saveRecursive(WriteStream& buf) const;
	void loadRecursive(WriteStream& buf) const;
	void editRecursive(WriteStream& buf) const;
	void applyRecursive(WriteStream& buf) const;
	const char* mostBaseClassName() const;

private:
	StructDataType* baseClass_;
	typedef vector<StructDataType*> StructList;
	StructList derivedClasses_;

	string delegate_code;

	friend struct DelegateToken;
};

struct StructToken : Token 
{
	StructToken() : Token("struct") {}
	void affect(Compiler& comp) const
	{
		new StructDataType(comp); // adds in constructor
		comp.skip_token(";");
	}
};


////////////////////////////////////////////////////////////////////////////////////////
//		Enum
////////////////////////////////////////////////////////////////////////////////////////
class IntConstant : virtual public Token
{
protected:
	int value;
public:
	IntConstant(const char* name, int val) : Token(name), value(val) {}
	void setValue(int val) { value = val; }
	operator int() const { return value; }
};

class EnumList : public DataTypeTemplate<Variable>, public list<ShareHandle<IntConstant> >
{
public:
	EnumList(const char* name, const char* prefix_) : Token(name), DataTypeTemplate<Variable>(name), prefix(prefix_), used_(false) {}
	void init(Compiler& comp);
	void declaration(WriteStream& buf, bool declExtern) const;
	void definition(WriteStream& buf, const char* parentPrefix) const;
	void affect(Compiler& comp) const { DataTypeTemplate<Variable>::affect(comp); setUsed(); }
	void checkEnumId(const char* name) const;
	void setUsed() const { if(!used_){ used_ = true; usedList_.push_back(this); } }
	void writeDescriptorPrototype(WriteStream& buf) const;

	const char* typeName() const { typeName_ = template_name("EnumWrapper", Compiler::refined_name()); return typeName_.c_str(); }

	static void writeDescriptorPrototypes(WriteStream& buf);
	
private:
	string prefix;
	mutable string typeName_;
	mutable bool used_;

	typedef list<const class EnumList*> UsedList; 
	static UsedList usedList_;
};

class EnumToken : public Token 
{
public:
	EnumToken() : Token("enum") {}
	void affect(Compiler& comp) const;
};

////////////////////////////////////////////////////////////////////////////////////////
//		BitVector
////////////////////////////////////////////////////////////////////////////////////////
class BitVectorVariable : public Variable
{
public:
	BitVectorVariable(const char* name, const DataType& type) : Variable(name, type) { typeName_ = template_name("BitVector", type.name()); }
	void init(Compiler& comp);
	void initialization(WriteStream& buf) const 
	{ 
		if(!defaultValue_.empty())
			Variable::initialization(buf);
	}
};

class BitVectorToken : public Token 
{
public:
	BitVectorToken() : Token("BitVector") {}
	void affect(Compiler& comp) const;
};

////////////////////////////////////////////////////////////////////////////////////////
//		Vector
////////////////////////////////////////////////////////////////////////////////////////
class ArrayVariable : public StructVariable
{
public:
	ArrayVariable(const char* name, const DataType& type, int size) : StructVariable(name, type) 
	{ 
		XBuffer buf(512);
		buf < "ArrayWrapper<" < type.typeName() < ", " <= size < ">";
		typeName_ = buf; 
	}
	void init(Compiler& comp)
	{
		Variable::init(comp);
	}
	void initialization(WriteStream& buf) const 
	{ 
		if(!defaultValue_.empty())
			buf() < name() < ".set(" < defaultValue_.c_str() < ");"; 
	}
};

class ArrayPointerVariable : public StructVariable
{
public:
	ArrayPointerVariable(const char* name, const DataType& type, int size) : StructVariable(name, type) 
	{ 
		XBuffer buf(512);
		buf < "ArrayWrapper<" < template_name("ShareHandle", typeName_) < ", " <= size < ">";
		typeName_ = buf; 
	}
};

class ArrayToken : public Token 
{
public:
	ArrayToken() : Token("array") {}
	void affect(Compiler& comp) const;
};


////////////////////////////////////////////////////////////////////////////////////////
//		Vector
////////////////////////////////////////////////////////////////////////////////////////
class VectorVariable : public StructVariable
{
public:
	VectorVariable(const char* name, const DataType& type) : StructVariable(name, type) 
	{ 
		typeName_ = template_name("vector", type.typeName()); 
	}
};

class VectorPointerVariable : public StructVariable
{
public:
	VectorPointerVariable(const char* name, const DataType& type) : StructVariable(name, type) 
	{ 
		typeName_ = template_name("vector", template_name("ShareHandle", typeName_)); 
	}
};

class VectorToken : public Token 
{
public:
	VectorToken() : Token("vector") {}
	void affect(Compiler& comp) const;
};

////////////////////////////////////////////////////////////////////////////////////////
//		Custom
////////////////////////////////////////////////////////////////////////////////////////
class CustomVariable : public Variable
{
	string customValue_;
public:
	CustomVariable(const char* name, const DataType& type, const char* customValue) : Variable(name, type), customValue_(customValue) 
	{
		typeName_ = "CustomString";
	}
	void initialization(WriteStream& buf) const 
	{ 
		buf() < name() < " = CustomString(" < defaultValue_.c_str() < ", " < customValue_ < ");"; 
	}
};

class CustomToken : public DataType
{
public:
	CustomToken() : Token("custom"), DataType("custom") {}
	void affect(Compiler& comp) const;
};



/////////////////////////////////////////////////////////////////////////////////////////////
//		Array
/////////////////////////////////////////////////////////////////////////////////////////////
/*
class ArrayVariable : public Variable, list<ShareHandle<Variable> >
{
	string size_var_name;
	int current_size;
	int declare_size;
public:
	ArrayVariable(const char* name, const DataType& type, const string& size_var_name_, int declare_size_) : Variable(type), size_var_name(size_var_name_), declare_size(declare_size_) {}
	void init(Compiler& comp);
	void write_value(WriteStream& buf) const;
	void write_name(WriteStream& buf) const { Variable::write_name(buf); buf() < "["; if(declare_size) buf() <= size(); buf() < "]"; }
	int size() const { return list<ShareHandle<Variable> >::size(); }
	const Variable& at(int index) const { if(index < 0 || index >= current_size) throw parsing_error("Array index is out of bound"); const_iterator i = begin(); while(index--) i++; return **i; } 
};

/////////////////////////////////////////////////////////////////////////////////////////////
//		Pointer
/////////////////////////////////////////////////////////////////////////////////////////////
class PointerVariable : public Variable
{
	const Variable* variable;
	string size_var_name;
	string variable_refine_name;
	bool inited_by_array;
public:
	PointerVariable(const char* name, const DataType& type) : Variable(type), variable(0), inited_by_array(0) {}
	void init(Compiler& comp);
	void write_value(WriteStream& buf) const { if(variable){ if(!inited_by_array) buf() < "&"; buf() < variable_refine_name.c_str(); } else buf() < "0"; }
	void copy_value(void* val) const {} // изменение адреса приведет к изменению описания
	void write_name(WriteStream& buf) const { buf() < refine_name_prefix.c_str() < type.type_name() < " const* " < name(); }
//	const Variable& at(int index) const { return variable->at(index); } 
};

*/

#include "InstrumentalTokens.h"