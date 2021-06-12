#include "stdafx.h"
#include "XPrmProInternal.h"
#include "Dictionary.h"

///////////////////////////////////////////////////////////////////////////////////////
//			String Util
///////////////////////////////////////////////////////////////////////////////////////
inline void replace(string& s, const char*src, const char* dest)
{
	int pos = 0;
	while(1){
		pos = s.find(src, pos);
		if(pos >= s.size())
			break;
		s.replace(pos, strlen(src), dest);
		pos += strlen(dest);
		}
}

inline string& expand_spec_chars(string& s)
{
	replace(s, "\\", "\\\\");
	replace(s, "\n", "\\n");
	replace(s, "\r", "\\r");
	replace(s, "\a", "\\a");
	replace(s, "\t", "\\t");
	replace(s, "\v", "\\v");
	replace(s, "\"", "\\\"");
	return s;
}

inline string& collapse_spec_chars(string& s)
{
	int pos = 0;
	while(1){
		pos = s.find("\\", pos);
		if(pos >= s.size() - 1)
			break;
		char* dest;
		switch(s[pos + 1]){
			case '\\':
				dest = "\\";
				break;
			case 'n':
				dest = "\n";
				break;
			case 'r':
				dest = "\r";
				break;
			case 'a':
				dest = "\a";
				break;
			case 't':
				dest = "\t";
				break;
			case 'v':
				dest = "\v";
				break;
			case '"':
				dest = "\"";
				break;

			default:
				xassert(0);
				ErrH.Abort("Unknown special character");
			}

		s.replace(pos, 2, dest);
		pos += strlen(dest);
		}

	return s;
}

///////////////////////////////////////////////////////////////////////////////////////
//			ScriptParser
///////////////////////////////////////////////////////////////////////////////////////

const char* ScriptParser::tokenNames_[NUMBER_OF_TOKENS] = 
{
	"INVALID",
	"END_OF_FILE",
	
	"{",
	"}",
	
	"OFFSET",
	"=",
	";",
	",",
	"|",
	"\n",
	" ",
	
	"SKIP_VALUE"
};

ScriptParser::ScriptParser(const char* fname, Mode mode, const char* dictionaryName) :
buffer(10, 1),
dictionary_(*new Dictionary(dictionaryName))
{
	open(fname, mode);
}

ScriptParser::~ScriptParser() 
{
	close();

	delete &dictionary_;
}

void ScriptParser::open(const char* fname, Mode mode)
{
	file_name = fname;
	switch(mode)
	{
	case LOAD: {
		XStream ff(fname, XS_IN);
		buffer.alloc(ff.size() + 1);
		ff.read(buffer.address(), ff.size());
		buffer[(int)ff.size()] = 0;
		if(buffer[0] == 'b' && buffer[1] == 'i' && buffer[2] == 'n') {
			binary = true;
			buffer += 3;
		}
		else 
			binary = false;
		load_ = true;
		replaced_symbol = 0;
		putTokenOffset_ = 0;
		line = 1;
		break;
		}

	case SAVE_TEXT:
	case SAVE_BINARY:
		binary = mode == SAVE_BINARY;
		load_ = false;
		buffer.alloc(10000);
		buffer.SetDigits(6);
		break;
	}
}

bool ScriptParser::close()
{
	if(!load_){
		xassert(offset.empty() && "Block isnt closed");
		XStream ff(0);
		if(ff.open(file_name.c_str(), XS_IN)){
			if(ff.size() == buffer.tell()){
				PtrHandle<char> buf = new char[ff.size()];
				ff.read(buf, ff.size());
				if(!memcmp(buffer, buf, buffer.tell()))
					return true;
			}
		}
		ff.close();
		ff.open(file_name.c_str(), XS_OUT);
		ff.write(buffer.address(), buffer.tell());
		return !ff.ioError();
	}
	else
		buffer.alloc(10);
	return true;
}

//-----------------------------------

const char* ScriptParser::getToken()	
{
	xassert(!binary);
	xassert(!replaced_symbol && "Unreleased token");
	
	putTokenOffset_ = buffer.tell();

	// Search begin of token
	for(;;){
		if(!buffer())
			return 0; // eof

		if(buffer() == '/'){	     
			if(buffer[(int)buffer.tell() + 1] == '/'){ // //-comment
				buffer += 2;
				if(!buffer.search("\n")) 
					return 0; // eof
				++buffer;
				line++;
				continue;
				}
			if(buffer[(int)buffer.tell() + 1] == '*'){ // /* */-comment
				buffer += 2;
				while(!(buffer() == '*' && buffer[(int)buffer.tell() + 1] == '/')){
					if(!buffer())
						return 0; // error, eof
					if(buffer() == '\n')
						line++;
					++buffer;
					}
				buffer += 2;
				continue;
				}
			}
		if(isspace(buffer())){
			if(buffer() == '\n')
				line++;
			++buffer;
			}
		else
			break;
		}

	const char* marker = &buffer();

	// Search end of token
	if(buffer() == '"'){ // Character Literal 
		++buffer;
		for(;;){
			if(!buffer.search("\"")) 
				return 0; // error
			--buffer;
			unsigned char prevChar = buffer();
			buffer += 2;
			if(prevChar != '\\')
				break;
		}
	}
	else
		if(buffer() == '}' || buffer() == '{')
			++buffer;
		else
			while(!isspace((++buffer)()) && buffer() != '}' && buffer() != '{' && buffer() != ';' && buffer() != ',' && buffer());

	replaced_symbol = buffer();
	buffer() = 0;
	return marker;
}

void ScriptParser::releaseToken()
{
	//	xassert(replaced_symbol);
	buffer() = replaced_symbol;
	replaced_symbol = 0;
}

void ScriptParser::putToken() 
{
	buffer.set(putTokenOffset_);
}

ScriptParser& ScriptParser::operator>>(const ScriptToken& token)
{
	if(token == OFFSET || token == END_OF_LINE){
	}
	else if(token != SKIP_VALUE){
		string name;
		(*this) >> name;
		if(name != tokenNames_[token]){
			xassert(0 && "Expected another token");
			XBuffer msg;
			msg  < "Expected Token: \"" < tokenNames_[token]
				< "\", Received Token: \"" < name.c_str() < "\", file: \"" < file_name.c_str() < "\", line: " <= line;
			ErrH.Abort(msg);
		}
	}
	else{ // SKIP_VALUE
		int open_counter = 0;
		for(;;){
			const char* s = getToken();
			xassert(s);
			string str = s;
			releaseToken();
			if(str == "{")
				++open_counter;
			else if(str == "}")
				--open_counter;
			else if(open_counter == 0 && (str == ";" || str == ",")){
				putToken();
				break;
			}
		}
	}

	return *this;
}

ScriptParser& ScriptParser::operator>>(string& str)
{
	if(binary){
		str = &buffer();
		buffer += str.size() + 1;
	}
	else{
		const char* s = getToken();
		xassert(s);
		str = s;
		releaseToken();
		if(str[0] == '"'){
			if(str[str.size() - 1] != '"'){
				XBuffer err;
				err < "Quotes aren't closed: " < s < "\n";
				err < "Line: " <= line;
				ErrH.Abort(err);
				}
			str.erase(0, 1);
			str.pop_back();
			collapse_spec_chars(str);
		}
	}
	return *this;
}

ScriptParser& ScriptParser::operator>>(const char* str)
{
	string name;
	(*this) >> name;
	xassert(name == str);
	return *this;
}

bool ScriptParser::findSection(const char* sectionName)
{
	// Оптимизировано для последовательной записи
	for(int i = 0; i < 2; i++){
		SmartName name("");
		(*this) >> name;
		if(name == sectionName){
			putToken();
			return true;
		}
		else{
			(*this) >> ASSIGNMENT >> SKIP_VALUE >> SEMICOLON;
			bool endOfFile = !getToken();
			releaseToken();
			if(!endOfFile)
				putToken();
			else{
				buffer.set(!binary ? 0 : 3);
				replaced_symbol = 0;
				putTokenOffset_ = 0;
				line = 1;
			}
		}
	}
	return false;
}

//-----------------------------------

ScriptParser& ScriptParser::operator<<(ScriptToken token)
{
	if(binary)
		buffer < (TokenRepresentation)token;
	else 
		switch(token)
		{
		case OPEN_BRACKET:
			buffer < "{" < "\r\n";
			offset += "\t";
			break;

		case CLOSE_BRACKET:
			xassert(!offset.empty() && "Close block without open");
			offset.pop_back();
			buffer < offset.c_str() < "}";
			break;

		case OFFSET:
			buffer < offset.c_str();
			break;

		case ASSIGNMENT:
			buffer < " = ";
			break;

		case SEPARATOR:
			buffer < " | ";
			break;

		case SEMICOLON:
			buffer < ";" < "\r\n";
			break;

		case COMMA:
			buffer < "," < "\r\n";
			break;

		case END_OF_LINE:
			buffer < "\r\n";
			break;

		case SPACE:
			buffer < " ";
			break;

		default: 
			xassert(0);
		}

	return *this;
}

ScriptParser& ScriptParser::operator<<(const char* str)
{
	if(strlen(str)) {
		const char* s = str;
		int write_quotes = 0;
		while(*s)
			if(isspace(*s++)){
				write_quotes = 1;
				break;
			}
		if(!write_quotes)
			buffer < str;
		else{
			string s1 = str;
			string::iterator i;
			FOR_EACH(s1, i)
				if(*i == '\r'){
					i = s1.erase(i);
					--i;
					}
			expand_spec_chars(s1);
			buffer < "\"" < s1.c_str() < "\"";
		}
	}
	else
		buffer < "\"\"";
	return *this;
}

//-------------------------------------------------------
ScriptParser& ScriptParser::operator<<(const PrmString& prmString)
{
	if(prmString){
		string s1 = prmString;
		expand_spec_chars(s1);
		buffer < "\"" < s1.c_str() < "\"";
	}
	else
		(*this) << 0;

	return *this;
}

ScriptParser& ScriptParser::operator>>(PrmString& prmString)
{
	while(isspace(buffer()))
		++buffer;
	
	if(buffer() == '"'){
		--buffer;
		string token;
		(*this) >> token;
		prmString = token.c_str();
	}
	else if(buffer() == '0'){
		++buffer;
		prmString = 0;
	}
	else
		xassert(0);
	
	return *this;
}

//--------------------------------------
static bool useAlternativeNames_ = true;

bool useAlternativeNames()
{
	return useAlternativeNames_;
}

void setUseAlternativeNames(bool use)
{
	useAlternativeNames_ = use;
}

