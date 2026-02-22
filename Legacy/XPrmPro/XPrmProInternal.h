#ifndef __SCRIPT_PARSER_H__
#define __SCRIPT_PARSER_H__

#include "XPrmPro.h"
#include "TreeInterface.h"

class SmartName : public string
{
public:
	SmartName(const char* parentName) { parentName_ = parentName; }
	SmartName& operator=(const string& value) { (string&)(*this) = value; return *this; }
	const string& parentName() const { return parentName_; }
	
private:
	string parentName_;
};

//-----------------------------------
bool useAlternativeNames();
void setUseAlternativeNames(bool use);

//-----------------------------------
enum ScriptToken
{
	INVALID,
	END_OF_FILE,

	OPEN_BRACKET,
	CLOSE_BRACKET,

	OFFSET,
	ASSIGNMENT,
	SEMICOLON,
	COMMA,
	SEPARATOR, 
	END_OF_LINE,
	SPACE,
	SKIP_VALUE,

	NUMBER_OF_TOKENS
};

class ScriptParser
{
public:
	enum Mode {
		LOAD,
		SAVE_TEXT,
		SAVE_BINARY
	};

	ScriptParser(const char* fname, Mode mode, const char* dictionaryName = 0);
	~ScriptParser();

	void open(const char* fname, Mode mode);
	bool close(); // return write status

	// Loading
	bool findSection(const char* sectionName); // for multisection files only: if found - ready to load
	void putToken(); // get back last token only
	ScriptParser& operator>>(const ScriptToken& token);
	ScriptParser& operator>>(const char* name);
	ScriptParser& operator>>(SmartName& smartName);
	ScriptParser& operator>>(string& str);
	ScriptParser& operator>>(bool& val) { string str; (*this) >> str; val = str == "true"; return *this; }
	ScriptParser& operator>>(char& val) { if(binary) buffer > val; else buffer >= val; return *this; }
	ScriptParser& operator>>(int& val) { if(binary) buffer > val; else buffer >= val; return *this; }
	ScriptParser& operator>>(long& val) { if(binary) buffer > val; else buffer >= val; return *this; }
	ScriptParser& operator>>(float& val) { if(binary) buffer > val; else buffer >= val; return *this; }
	ScriptParser& operator>>(double& val) { if(binary) buffer > val; else buffer >= val; return *this; }
	ScriptParser& operator>>(unsigned char& val) { if(binary) buffer > val; else buffer >= val; return *this; }
	ScriptParser& operator>>(unsigned int& val) { if(binary) buffer > val; else buffer >= val; return *this; }
	ScriptParser& operator>>(unsigned long& val) { if(binary) buffer > val; else buffer >= val; return *this; }
	
	ScriptParser& operator>>(Vect2i& v) { return *this >> OPEN_BRACKET >> v.x >> v.y >> CLOSE_BRACKET; }
	ScriptParser& operator>>(Vect2f& v) { return *this >> OPEN_BRACKET >> v.x >> v.y >> CLOSE_BRACKET; }
	ScriptParser& operator>>(Vect3f& v) { return *this >> OPEN_BRACKET >> v.x >> v.y >> v.z >> CLOSE_BRACKET; }
	ScriptParser& operator>>(QuatF& q) { return *this >> OPEN_BRACKET >> q.s() >> q.x() >> q.y() >> q.z() >> CLOSE_BRACKET; }
	ScriptParser& operator>>(Mat3f& m) { return *this >> OPEN_BRACKET >> m.xrow() >> m.yrow() >> m.zrow() >> CLOSE_BRACKET; }
	ScriptParser& operator>>(MatXf& x) { return *this >> OPEN_BRACKET >> x.rot() >> x.trans() >> CLOSE_BRACKET; }
	ScriptParser& operator>>(PrmString& prmString);
	ScriptParser& operator>>(CustomString& customString) { PrmString prmString; (*this) >> prmString; customString = prmString; return *this; }
	
	// Saving
	ScriptParser& operator<<(ScriptToken token);
	ScriptParser& operator<<(const char* str);
	ScriptParser& operator<<(const string& str) { return *this << str.c_str(); }
	ScriptParser& operator<<(bool val) { (*this) << (val ? "true" : "false"); return *this; }
	ScriptParser& operator<<(char val) { if(binary) buffer < val; else buffer <= val; return *this; }
	ScriptParser& operator<<(int val) { if(binary) buffer < val; else buffer <= val; return *this; }
	ScriptParser& operator<<(long val) { if(binary) buffer < val; else buffer <= val; return *this; }
	ScriptParser& operator<<(float val) { if(binary) buffer < val; else buffer <= val; return *this; }
	ScriptParser& operator<<(double val) { if(binary) buffer < val; else buffer <= val; return *this; }
	ScriptParser& operator<<(unsigned char val) { if(binary) buffer < val; else buffer <= val; return *this; }
	ScriptParser& operator<<(unsigned int val) { if(binary) buffer < val; else buffer <= val; return *this; }
	ScriptParser& operator<<(unsigned long val) { if(binary) buffer < val; else buffer <= val; return *this; }
	
	ScriptParser& operator<<(const Vect2i& v) { return *this << OPEN_BRACKET << OFFSET << v.x << SPACE << v.y << END_OF_LINE << CLOSE_BRACKET; }
	ScriptParser& operator<<(const Vect2f& v) { return *this << OPEN_BRACKET << OFFSET << v.x << SPACE << v.y << END_OF_LINE << CLOSE_BRACKET; }
	ScriptParser& operator<<(const Vect3f& v) { return *this << OPEN_BRACKET << OFFSET << v.x << SPACE << v.y << SPACE << v.z << END_OF_LINE << CLOSE_BRACKET; }
	ScriptParser& operator<<(const QuatF& q) { return *this << OPEN_BRACKET << OFFSET << q.s() << SPACE << q.x() << SPACE << q.y() << SPACE << q.z() << END_OF_LINE << CLOSE_BRACKET; }
	ScriptParser& operator<<(const Mat3f& m) { return *this << OPEN_BRACKET << OFFSET << m.xrow() << m.yrow() << m.zrow() << END_OF_LINE << CLOSE_BRACKET; }
	ScriptParser& operator<<(const MatXf& x) { return *this << OPEN_BRACKET << OFFSET << x.rot() << x.trans() << END_OF_LINE << CLOSE_BRACKET; }
	ScriptParser& operator<<(const PrmString& prmString);
	ScriptParser& operator<<(const CustomString& customString) { PrmString prmString = customString; (*this) << prmString; return *this; }
			
	// Pointers
	template<class T>
	ScriptParser& operator<<(const ShareHandle<T>& val) { if(val) savePointer(*this, &*val); else (*this) << "0"; return *this; }
	
	template<class T>
	ScriptParser& operator>>(ShareHandle<T>& val) 
	{ 
		T* object = 0; 
		
		string type;
		(*this) >> type;
		if(type != "0"){
			putToken();
			loadPointer(*this, object);
		}
		
		val = object; 
		return *this; 
	}

	// array
	template<class T, int size> 
	ScriptParser& operator<<(const ArrayWrapper<T, size>& container) 
	{ 
		(*this) << OPEN_BRACKET; 
		(*this) << OFFSET << size << SEMICOLON;
		for(int i = 0; i < size; ++i)
			(*this) << OFFSET << container[i] << (i < size - 1? COMMA : END_OF_LINE);
		(*this) << CLOSE_BRACKET;
		return *this;
	}

	template<class T, int size> 
	ScriptParser& operator>>(ArrayWrapper<T, size>& container) 
	{ 
		int sizeIn; 
		(*this) >> OPEN_BRACKET; 
		(*this) >> sizeIn >> SEMICOLON;
		int n = min(size, sizeIn);
		for(int i = 0; i < n; ++i)
			(*this) >> container[i] >> (--sizeIn ? COMMA : END_OF_LINE); 
		T element;
		while(sizeIn)
			(*this) >> element >> (--sizeIn ? COMMA : END_OF_LINE); 
		(*this) >> CLOSE_BRACKET;
		return *this;
	}

	// vector
	template<class T> 
	ScriptParser& operator<<(const vector<T>& container) 
	{ 
		(*this) << OPEN_BRACKET; 
		int size = container.size();
		(*this) << OFFSET << size << SEMICOLON;
		for(vector<T>::const_iterator i = container.begin(); i != container.end(); ++i)
			(*this) << OFFSET << *i << (--size ? COMMA : END_OF_LINE);
		(*this) << CLOSE_BRACKET;
		return *this;
	}

	template<class T> 
	ScriptParser& operator>>(vector<T>& container) 
	{ 
		int size; 
		(*this) >> OPEN_BRACKET; 
		(*this) >> size >> SEMICOLON;
		container.clear(); 
		container.reserve(size);
		while(size > 0){ 
			container.push_back(T()); 
			(*this) >> container.back() >> (--size ? COMMA : END_OF_LINE); 
		}
		(*this) >> CLOSE_BRACKET;
		return *this;
	}


	// EnumWrapper
	template<class T> 
	ScriptParser& operator<<(const EnumWrapper<T>& enumVar) 
	{ 
		return (*this) << getEnumDescriptor(T(0)).name(enumVar);
	}
	
	template<class T> 
	ScriptParser& operator>>(EnumWrapper<T>& enumVar) 
	{
		const EnumDescriptor<T>& descriptor = getEnumDescriptor(T(0));
		SmartName smartNameVar(descriptor.typeName());
		(*this) >> smartNameVar;
		enumVar = descriptor.keyByName(smartNameVar.c_str());
		return *this;
	}

	// BitVector
	template<class T> 
	ScriptParser& operator<<(const BitVector<T>& bitVector) 
	{ 
		buffer < getEnumDescriptor(T(0)).nameCombination(bitVector).c_str();
		return *this;
	}

	template<class T> 
	ScriptParser& operator>>(BitVector<T>& bitVector) 
	{ 
		const EnumDescriptor<T>& descriptor = getEnumDescriptor(T(0));
		bitVector = 0;
		for(;;){
			SmartName smartNameVar(descriptor.typeName());
			(*this) >> smartNameVar;
			if(smartNameVar == ";"){
				putToken();
				break;
			}
			else if(smartNameVar == "|")
				continue;
			bitVector = bitVector | descriptor.keyByName(smartNameVar.c_str());
		}
		return *this;
	}
	 

private:
	typedef unsigned char TokenRepresentation;

	// Common
	bool binary;
	bool load_;
	XBuffer buffer;

	// Save
	string offset;
	string file_name;
	
	// Load
	char replaced_symbol;
	int putTokenOffset_;
	int line;

	class Dictionary& dictionary_;

	static const char* tokenNames_[NUMBER_OF_TOKENS];

	// Private Methods
	const char* getToken();
	void releaseToken();
};

template<class T>
void savePointer(ScriptParser& parser, const T* object)
{
	parser << *object;
}

template<class T>
void loadPointer(ScriptParser& parser, T*& object)
{
	object = new T;
	parser >> *object;
}

//#ifndef _FINAL_VERSION_
//#define xassertStr(exp, str) { string s = #exp; s += "\n"; s += str; xxassert(exp,s.c_str()); }
//#else
//#define xassertStr(exp, str) 
//#endif

//-----------------------------------
//	Tree-Control Stuff

// createNode(0, T()) creates default node
inline TreeNode* createNode(const char* name, const bool& value) { if(!name) return createNode("", bool(1)); static const char* comboList = "true|false"; static const char* comboListAlt = "Да|Нет"; return new TreeNode(name, !useAlternativeNames() ? (value ? "true" : "false") : (value ? "Да" : "Нет"), "bool", TreeNode::COMBO, !useAlternativeNames() ? comboList : comboListAlt); }
inline TreeNode* createNode(const char* name, const char& value) { if(!name) return createNode("", char(0)); XBuffer buf; buf <= value; return new TreeNode(name, buf, "char"); }
inline TreeNode* createNode(const char* name, const int& value) { if(!name) return createNode("", int(0)); XBuffer buf; buf <= value; return new TreeNode(name, buf, "int"); }
inline TreeNode* createNode(const char* name, const long& value) { if(!name) return createNode("", int(0)); XBuffer buf; buf <= value; return new TreeNode(name, buf, "long"); }
inline TreeNode* createNode(const char* name, const float& value) { if(!name) return createNode("", float(0)); XBuffer buf; buf <= value; return new TreeNode(name, buf, "float"); }
inline TreeNode* createNode(const char* name, const double& value) { if(!name) return createNode("", double(0)); XBuffer buf; buf <= value; return new TreeNode(name, buf, "double"); }
inline TreeNode* createNode(const char* name, const unsigned char& value) { if(!name) return createNode("", char(0)); XBuffer buf; buf <= value; return new TreeNode(name, buf, "unsigned char"); }
inline TreeNode* createNode(const char* name, const unsigned int& value) { if(!name) return createNode("", int(0)); XBuffer buf; buf <= value; return new TreeNode(name, buf, "unsigned int"); }
inline TreeNode* createNode(const char* name, const unsigned long& value) { if(!name) return createNode("", int(0)); XBuffer buf; buf <= value; return new TreeNode(name, buf, "unsigned long"); }

inline TreeNode* createNode(const char* name, const Vect2i& value) { if(!name) return createNode("", Vect2i::ZERO); XBuffer buf; buf <= value; return new TreeNode(name, buf, "Vect2i"); }
inline TreeNode* createNode(const char* name, const Vect2f& value) { if(!name) return createNode("", Vect2f::ZERO); XBuffer buf; buf <= value; return new TreeNode(name, buf, "Vect2f"); }
inline TreeNode* createNode(const char* name, const Vect3f& value) { if(!name) return createNode("", Vect3f::ZERO); XBuffer buf; buf <= value; return new TreeNode(name, buf, "Vect3f"); }
inline TreeNode* createNode(const char* name, const QuatF& value) { if(!name) return createNode("", QuatF::ID); XBuffer buf; buf <= value; return new TreeNode(name, buf, "QuatF"); }
inline TreeNode* createNode(const char* name, const Mat3f& value) { if(!name) return createNode("", Mat3f::ID); XBuffer buf; buf <= value; return new TreeNode(name, buf, "Mat3f"); }
inline TreeNode* createNode(const char* name, const MatXf& value) { if(!name) return createNode("", MatXf::ID); XBuffer buf; buf <= value.rot() <= value.trans(); return new TreeNode(name, buf, "MatXf"); }
inline TreeNode* createNode(const char* name, const PrmString& value) { if(!name) return createNode("", PrmString("")); return new TreeNode(name, value ? value : "", "string"); }
inline TreeNode* createNode(const char* name, const CustomString& value) { if(!name) return createNode("", CustomString("", value.customValueFunc())); return new TreeNode(name, value, "custom", TreeNode::CUSTOM, 0, 0, 0, value.customValueFunc()); }


inline void applyNode(bool& value, const TreeNode* node) {	value = node->value() == (!useAlternativeNames() ? "true" : "Да"); }
inline void applyNode(char& value, const TreeNode* node) { node->valueBuffer() >= value; }
inline void applyNode(int& value, const TreeNode* node) { node->valueBuffer() >= value; }
inline void applyNode(long& value, const TreeNode* node) { node->valueBuffer() >= value; }
inline void applyNode(float& value, const TreeNode* node) { node->valueBuffer() >= value; }
inline void applyNode(double& value, const TreeNode* node) { node->valueBuffer() >= value; }
inline void applyNode(unsigned char& value, const TreeNode* node) { node->valueBuffer() >= value; }
inline void applyNode(unsigned int& value, const TreeNode* node) { node->valueBuffer() >= value; }
inline void applyNode(unsigned long& value, const TreeNode* node) { node->valueBuffer() >= value; }

inline void applyNode(Vect2i& value, const TreeNode* node) { node->valueBuffer() >= value; }
inline void applyNode(Vect2f& value, const TreeNode* node) { node->valueBuffer() >= value; }
inline void applyNode(Vect3f& value, const TreeNode* node) { node->valueBuffer() >= value; }
inline void applyNode(QuatF& value, const TreeNode* node) { node->valueBuffer() >= value; }
inline void applyNode(Mat3f& value, const TreeNode* node) { node->valueBuffer() >= value; }
inline void applyNode(MatXf& value, const TreeNode* node) { node->valueBuffer() >= value.rot() >= value.trans(); }
inline void applyNode(PrmString& value, const TreeNode* node) { value = node->value(); }
inline void applyNode(CustomString& value, const TreeNode* node) { value = node->value(); }

// Arrays
template<class T, int size> 
inline TreeNode* createNode(const char* name, const ArrayWrapper<T, size>& container)
{ 
	TreeNode* defaultNode = createNode(0, T());
	TreeNode* treeNode = new TreeNode(name, "", (defaultNode->type() + "[]").c_str(), TreeNode::VECTOR, 0, defaultNode);
	for(int i = 0; i < size; i++)
		treeNode->push_back(createNode("", container[i]));
	return treeNode;
}

template<class T, int size> 
inline void applyNode(ArrayWrapper<T, size>& container, const TreeNode* node)
{ 
	int i = 0;
	TreeNode::const_iterator ni;
	FOR_EACH(node->children(), ni)
		applyNode(container[i++], *ni);
}

// vector
template<class T> 
inline TreeNode* createNode(const char* name, const vector<T>& container)
{ 
	TreeNode* defaultNode = createNode(0, T());
	TreeNode* treeNode = new TreeNode(name, "", (defaultNode->type() + "[]").c_str(), TreeNode::VECTOR, 0, defaultNode);
	vector<T>::const_iterator i;
	FOR_EACH(container, i)
		treeNode->push_back(createNode("", *i));
	return treeNode;
}

template<class T> 
inline void applyNode(vector<T>& container, const TreeNode* node)
{ 
	container.clear();
	TreeNode::const_iterator i;
	FOR_EACH(node->children(), i){
		container.push_back(T());
		applyNode(container.back(), *i);
	}
}

// EnumWrapper
template<class T> 
inline TreeNode* createNode(const char* name, const EnumWrapper<T>& enumVar)
{ 
	const EnumDescriptor<T>& descriptor = getEnumDescriptor(T(0));

	if(!name)
		return createNode("", EnumWrapper<T>(descriptor.defaultValue()));
	
	return new TreeNode(name, !useAlternativeNames() ? descriptor.name(enumVar) : descriptor.nameAlt(enumVar), 
		descriptor.typeName(), TreeNode::COMBO, !useAlternativeNames() ? descriptor.comboList() : descriptor.comboListAlt());
}

template<class T> 
inline void applyNode(EnumWrapper<T>& enumVar, const TreeNode* node)
{ 
	const EnumDescriptor<T>& descriptor = getEnumDescriptor(T(0));
	enumVar = !useAlternativeNames() ? descriptor.keyByName(node->value().c_str()) : descriptor.keyByNameAlt(node->value().c_str());
}

template<class T> 
inline TreeNode* createNode(const char* name, const BitVector<T>& bitVector)
{
	if(!name)
		return createNode("", BitVector<T>(0));
	
	const EnumDescriptor<T>& descriptor = getEnumDescriptor(T(0));
	return new TreeNode(name, !useAlternativeNames() ? descriptor.nameCombination(bitVector).c_str() : descriptor.nameAltCombination(bitVector).c_str(), 
		descriptor.typeName(), TreeNode::COMBO_MULTI, !useAlternativeNames() ? descriptor.comboList() : descriptor.comboListAlt());
}

template<class T> 
inline void applyNode(BitVector<T>& bitVector, const TreeNode* treeNode)
{
	bitVector = 0;
	XBuffer valueBuffer = treeNode->valueBuffer();
	
	const EnumDescriptor<T>& descriptor = getEnumDescriptor(T(0));

	for(;;){
		string name = getEnumToken(valueBuffer);
		if(name == "")
			break;
		bitVector  = bitVector | (!useAlternativeNames() ? descriptor.keyByName(name.c_str()) : descriptor.keyByNameAlt(name.c_str()));
	}
}

/*
// Pointers
template<class T>
inline TreeNode* createNode(const char* name, const ShareHandle<T>& val)
{ 
	TreeNode* defaultNode = createNode(0, T());
	TreeNode* node = new TreeNode(name ? name : "", "", (defaultNode->type() + "*").c_str(), TreeNode::POINTER, 0, defaultNode);
	if(val)
		node->push_back(createNode("", &*val));
	return node;
}

template<class T>
inline void applyNode(ShareHandle<T>& val, const TreeNode* node)
{ 
	if(!node->empty()){
		if(!val)
			val = new T;
		applyNode(*val, node);
	}
	else
		val = 0;
}
*/
// Enums
inline string getEnumToken(XBuffer& buffer)
{
	while(buffer() == ' ' || buffer() == '|')
		++buffer;
	const char* marker = &buffer();
	while(buffer() != '|' && buffer() != 0)
		++buffer;
	string str(marker, &buffer() - marker);
	while(str.size() && str[str.size() - 1] == ' ')
		str.pop_back();
	return str;
}


#endif //__SCRIPT_PARSER_H__