#pragma once
#include "ParseUtil.h"

#define _VERSION_	"1.40"

typedef list<string> StringList;

/////////////////////////////////////////////////////////////////////////////////////////////
//			Token 
/////////////////////////////////////////////////////////////////////////////////////////////
class Token : public ShareHandleBase
{
public:
	Token(const char* name = 0) : name_(name) { assert(name); }
	Token(const Token& token) : name_(token.name()) {}
	virtual ~Token() {}
	const char* name() const { return name_.c_str(); }
	virtual Token* clone() const { return const_cast<Token*>(this); } // ������� ����������� ������ ���������� ������
	virtual void affect(class Compiler& comp) const { throw unexpected_token(name()); } // �������� �������� ������� ������
protected:
	string name_;
};
		
/////////////////////////////////////////////////////////////////////////////////////////////
//			Token List
/////////////////////////////////////////////////////////////////////////////////////////////
class TokenList : public list<ShareHandle<Token> >, virtual public Token
{
	friend class Compiler;
//	struct eqstr { bool operator()(const char* s1, const char* s2) const { return !strcmp(s1, s2); } };
//	typedef hash_map<const char*, Token*, hash<const char*>, eqstr> Map;
	typedef hash_map<string, Token*> Map;

	Map map;
	const TokenList* parent;

protected:
	int lock_addition;

public:
	TokenList(const char* name) : Token(name) { lock_addition = 0; parent = 0; }
	TokenList(const TokenList& tokens, const char* name = 0);
	const Token* find(const char* name) const;
	const Token* find_local(const char* name) const;
	void run(const char* name, Compiler& comp) const { find(name)->affect(comp); }
	void add(Token* token);
	void addFront(Token* token);
	void parse(class Compiler& comp);
	void clear() { list<ShareHandle<Token> >::clear(); map.clear(); } 
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
	int using_namespace;

	Section(const char* name) : Token(name), TokenList(name) { using_namespace = 0; }
	int variables() const;
	bool definition(bool rebuild, StringList& dependencies);
	bool declaration(bool rebuild);
	unsigned description();
	void copy(class ParameterSection& prm);
	string align_path(const string& str);
};

/////////////////////////////////////////////////////////////////////////////////////////////
//			Parser
/////////////////////////////////////////////////////////////////////////////////////////////
class Parser 
{
	string fname;
	int line;
	char* buffer;
	char* current_token;
	char* next_token;
	char* prev_token;
	char replaced_symbol;
public:
	Parser(const char* fname = 0);
	~Parser();
	void open(const char* fname);
	const char* file_name() const { return fname.c_str(); }	
	unsigned int CRC(unsigned int crc);

	const char* get_token();	
	void put_token();	
	void skip_token(const char* token) { if(strcmp(get_token(), token)) throw expected_token(token); }
	friend XBuffer& operator<= (XBuffer& os, const Parser& p) { char full[MAX_PATH]; _fullpath( full, p.fname.c_str(), MAX_PATH ); os < full < '(' <= p.line < ") : "; return os; }
	operator const char*() const { return current_token; }
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

	StringList dependencies;

	const Token* refine_name_first_token;
	string full_refined_name;

	int uniqueNameCounter_;

	bool sectionUpdated_;

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

	TokenList& top_context();
	
protected:
	string exe_path;

public:							
	Compiler();
	void clear();
	int parse_file(const char* fname, XBuffer& sout); // returns the number of errors
	bool compile(const char* fname, bool rebuild); // returns 1 if succeeds
	bool sectionUpdated() const { return sectionUpdated_; }
	Section* getSection(const char* name);

	void create_section(const char* name);
	Section& section() { xassert(!sections.empty()); return *sections.back(); }

	void open(const char* fname) { parsers.push_back(Parser()); parser().open(fname); dependencies.push_back(fname); }
	Parser& parser() { xassert(!parsers.empty()); return parsers.back(); }

	TokenList& context() { return *contexts.back(); }
	void push_context(TokenList* cont) { cont->parent = contexts.empty() ? 0 : &context(); contexts.push_back(cont); }
	void pop_context() { contexts.pop_back(); }
	string context_prefix() const;

	string generate_unique_name(const char* name);
	void add_new_emulated(Token* token);
	void remove_new_emulated(Token* token);

	const char* get_token(int enable_eof = 0);
	void skip_token(const char* token) { if(strcmp(get_token(), token)) throw expected_token(token); }
	bool get_token_if(const char* token);
	
	const Token* refine_name(const Token* token, bool external_call = true);
	bool refined_token_from_current_context();
	const char* refined_name() const { return full_refined_name.c_str(); }

	int eval_i();
	double eval_f();
	string eval_s();
};
					    
/////////////////////////////////////////////////////////////////////////////////////////////
//	  WriteStream - ���������� ��� �������� ����� �����.
/////////////////////////////////////////////////////////////////////////////////////////////
class WriteStream : public XBuffer
{
	enum { line_lenght = 80 };
	int offset_;
public:
	WriteStream() : XBuffer(100000, 1), offset_(0) {}
	void check_line() { if(tell() - offset_ > line_lenght){ (*this) < " \r\n "; offset_ = tell(); } }
};

/////////////////////////////////////////////////////////////////////////////////////////////
//		DataType
/////////////////////////////////////////////////////////////////////////////////////////////
class DescriptedToken : virtual public Token 
{
public:
	virtual void description(unsigned& crc) const = 0;
	virtual int sizeOf() const = 0;
};

class DataType : public DescriptedToken 
{
protected:
	string type_name_;
	int size_;
public:
	DataType(const char* type_name, int size) : type_name_(type_name), size_(size) {}
	const char* type_name() const { return type_name_.c_str(); }
	int sizeOf() const { return size_; }
	void description(unsigned& crc) const { crc = CRC(type_name_.c_str(), crc); }
	virtual class Variable* create(const char* name) const = 0;
	void affect(Compiler& comp) const;
};

class Variable : public DescriptedToken 
{
protected:
	const DataType& type;
	int assigment_flag;
	int static_flag;
	int new_emulation_flag;
	string refine_name_prefix;
	friend struct StaticToken;
	friend DataType;
	friend class ArrayVariable;
	friend class PointerVariable;
public:
	Variable(const DataType& type_) : type(type_) { assigment_flag = 0; static_flag = 0; new_emulation_flag = 0; }
	Variable(const Variable& var) : type(var.type) { (DescriptedToken&)*this = var; assigment_flag = 0; static_flag = var.static_flag; new_emulation_flag = var.new_emulation_flag; }
	virtual void init(Compiler& comp){ if(assigment_flag) throw parsing_error((string("Redefenition of variable: ") + name()).c_str()); assigment_flag = 1; }
	virtual void write_name(XBuffer& buf) const { if(new_emulation_flag) buf < "static "; buf < refine_name_prefix.c_str() < type.type_name() < " " < name(); }
	virtual void write_value(WriteStream& buf) const = 0;
	virtual void copy_value(void* value) const = 0;
	void description(unsigned& crc) const { type.description(crc); crc = CRC(name(), crc); }
	int sizeOf() const { return type.sizeOf(); }
	int declarable() const { return !static_flag && !new_emulation_flag; }
	int definible() const { return !static_flag; }
	Token* clone() const = 0;
	void affect(Compiler& comp) const;
	bool newEmulated() const { return new_emulation_flag; }
};

template <class T>
class DataTypeTemplate : public DataType 
{
public:
	DataTypeTemplate(const char* name, const char* type_name, int size) : Token(name), DataType(type_name, size) {}
	Variable* create(const char* name) const { return new T(name, *this); }
};


/////////////////////////////////////////////////////////////////////////////////////////////
//		BaseDataType
/////////////////////////////////////////////////////////////////////////////////////////////
class IntConstant : virtual public Token
{
protected:
	int value;
public:
	IntConstant(const char* name, int val) : Token(name), value(val) {}
	operator int() const { return value; }
};

class IntVariable : public Variable, public IntConstant
{
public:
	IntVariable(const char* name, const DataType& type) : Token(name), Variable(type), IntConstant(name, 0) {}
	void init(Compiler& comp){ Variable::init(comp); value = comp.eval_i(); }
	void set(int val) { value = val; }
	void write_value(WriteStream& buf) const { buf <= value; }
	void copy_value(void* val) const { *(int*)val = value; }
	Token* clone() const { return new IntVariable(*this); } 
};

class DoubleVariable : public Variable
{
	double value;
public:
	DoubleVariable(const char* name, const DataType& type) : Token(name), Variable(type), value(0) {}
	void init(Compiler& comp) { Variable::init(comp); value = comp.eval_f(); }
	void write_value(WriteStream& buf) const { buf <= value; }
	void copy_value(void* val) const { *(double*)val = value; }
	Token* clone() const { return new DoubleVariable(*this); } 
	operator double() const { return value; }
};

class FloatVariable : public Variable
{
	double value;
public:
	FloatVariable(const char* name, const DataType& type) : Token(name), Variable(type), value(0) {}
	void init(Compiler& comp) { Variable::init(comp); value = comp.eval_f(); }
	void write_value(WriteStream& buf) const { buf <= value < "f"; }
	void copy_value(void* val) const { *(float*)val = (float)value; }
	Token* clone() const { return new FloatVariable(*this); } 
	operator double() const { return value; }
};

class StringVariable : public Variable
{
	bool zeroValue;
	string value;
public:
	StringVariable(const char* name, const DataType& type) : Token(name), Variable(type), zeroValue(false) {}
	void init(Compiler& comp) { Variable::init(comp); if(!comp.get_token_if("0")){ value = comp.eval_s(); zeroValue = false; } else zeroValue = true; }
	void write_value(WriteStream& buf) const { if(!zeroValue) buf < "\"" < value.c_str() < "\""; else buf < "0"; }
	void copy_value(void* val) const 
	{ 
		char*& str = *(char**)val;
		if(!zeroValue){
			string collapsed = value;
			collapse_spec_chars(collapsed);
			if(!str || collapsed != string(str))
				str = _strdup(collapsed.c_str());
		}
		else
			str = 0;
	}
	Token* clone() const { return new StringVariable(*this); } 
	operator string() const { return value; }
};

/////////////////////////////////////////////////////////////////////////////////////////////
//		Array
/////////////////////////////////////////////////////////////////////////////////////////////
class ArrayVariable : public Variable, list<ShareHandle<Variable> >
{
	string size_var_name;
	int current_size;
	int declare_size;
public:
	ArrayVariable(const char* name, const DataType& type, const string& size_var_name_, int declare_size_) : Token(name), Variable(type), size_var_name(size_var_name_), declare_size(declare_size_) {}
	void init(Compiler& comp);
	void write_value(WriteStream& buf) const;
	void copy_value(void* val) const;
	void write_name(XBuffer& buf) const { Variable::write_name(buf); buf < "["; if(declare_size) buf <= size(); buf < "]"; }
	void description(unsigned& crc) const { Variable::description(crc); crc = CRC(size(), crc); }
	int sizeOf() const { return type.sizeOf()*size(); }
	Token* clone() const;
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
	PointerVariable(const char* name, const DataType& type) : Token(name), Variable(type), variable(0), inited_by_array(0) {}
	void init(Compiler& comp);
	void write_value(WriteStream& buf) const { if(variable){ if(!inited_by_array) buf < "&"; buf < variable_refine_name.c_str(); } else buf < "0"; }
	void copy_value(void* val) const {} // ��������� ������ �������� � ��������� ��������
	void write_name(XBuffer& buf) const { buf < refine_name_prefix.c_str() < type.type_name() < " const* " < name(); }
	void description(unsigned& crc) const { Variable::description(crc); if(variable) variable->description(crc); }
	int sizeOf() const { return 4; }
	Token* clone() const { return new PointerVariable(*this); } 
//	const Variable& at(int index) const { return variable->at(index); } 
};


/////////////////////////////////////////////////////////////////////////////////////////////
//		Struct
/////////////////////////////////////////////////////////////////////////////////////////////
class StructDataType : public DataType, public TokenList
{
	friend struct DelegateToken;
	friend struct StaticToken;
	int dont_declare;
	string delegate_code;
public:
	StructDataType(Compiler& comp);
	Variable* create(const char* name) const;
	void description(unsigned& crc) const;
	void declaration(XBuffer& buf) const;
};

class StructVariable : public Variable, TokenList
{
public:
	StructVariable(const char* name, const StructDataType& type) : Token(name), TokenList(type, name), Variable(type){}
	StructVariable(const StructVariable& struct_var) : Token(struct_var.name()), TokenList(struct_var, struct_var.name()), Variable(struct_var){}
	void init(Compiler& comp);
	void write_value(WriteStream& buf) const;
	void copy_value(void* val) const;
	Token* clone() const { return new StructVariable(*this); } 
};

struct StructToken : Token 
{
	StructToken() : Token("struct") {}
	void affect(Compiler& comp) const
	{
		comp.context().add(new StructDataType(comp));
		comp.skip_token(";");
	}
};

#include "InstrumentalTokens.h"