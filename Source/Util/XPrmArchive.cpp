#include "StdAfx.h"

#include "XPrmArchive.h"

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
XPrmOArchive::XPrmOArchive(const char* fname) :
buffer_(10, 1)
{
	open(fname);
}

XPrmOArchive::~XPrmOArchive() 
{
	close();
}

void XPrmOArchive::open(const char* fname)
{
	fileName_ = fname;
	buffer_.alloc(10000);
	buffer_.SetDigits(6);
}

bool XPrmOArchive::close()
{
	xassert(offset_.empty() && "Block isnt closed");
	XStream ff(0);
	if(ff.open(fileName_.c_str(), XS_IN)){
		if(ff.size() == buffer_.tell()){
			PtrHandle<char> buf = new char[ff.size()];
			ff.read(buf, ff.size());
			if(!memcmp(buffer_, buf, buffer_.tell()))
				return true;
		}
	}
	ff.close();
	ff.open(fileName_.c_str(), XS_OUT);
	ff.write(buffer_.address(), buffer_.tell());
	return !ff.ioError();
}

//////////////////////////////////////////////////////
void XPrmOArchive::saveStringEnclosed(const char* prmString)
{
	if(prmString){
		string s1 = prmString;
		expand_spec_chars(s1);
		buffer_ < "\"" < s1.c_str() < "\"";
	}
	else
		buffer_ < "0";
}

//////////////////////////////////////////////
XPrmIArchive::XPrmIArchive(const char* fname) :
buffer_(10, 1)
{
	if(fname && !open(fname))
		ErrH.Abort("File not found: ", XERR_USER, 0, fname);
}

XPrmIArchive::~XPrmIArchive() 
{
	close();
}

bool XPrmIArchive::open(const char* fname)
{
	fileName_ = fname;
	XStream ff(0);
	if(!ff.open(fname, XS_IN))
		return false;
	buffer_.alloc(ff.size() + 1);
	ff.read(buffer_.address(), ff.size());
	buffer_[(int)ff.size()] = 0;
	replaced_symbol = 0;
	putTokenOffset_ = 0;
	readingStarts_.push_back(0);
	return true;
}

void XPrmIArchive::close()
{
	buffer_.alloc(10);
}

const char* XPrmIArchive::getToken()	
{
	xassert(!replaced_symbol && "Unreleased token");
	
	putTokenOffset_ = buffer_.tell();

	// Search begin of token
	const char* i = &buffer_();
	for(;;){
		if(!*i)
			return 0; // eof

		if(*i == '/'){	     
			if(*(i + 1) == '/'){ // //-comment
				i += 2;
				if((i = strstr(i, "\n")) == 0) 
					return 0;
				i++;
				continue;
				}
			if(*(i + 1) == '*'){ // /* */-comment
				i += 2;
				while(!(*i == '*' && *(i + 1) == '/')){
					if(!*i)
						return 0; // error
					i++;
					}
				i += 2;
				continue;
				}
			}
		if(isspace(*i))
			i++;
		else
			break;
		}

	// Search end of token
	const char* marker = i;
	if(isalpha(*i) || *i == '_'){ // Name
		i++;
		while(__iscsym(*i))
			i++;
		}
	else
		if(isdigit(*i) || (*i == '.' && (isdigit(*(i + 1)) || *(i + 1) == 'f'))){ // Numerical Literal
			i++;
			while(__iscsym(*i) || *i == '.' || ((*i == '+' || *i == '-') && (*(i - 1) == 'E' || *(i - 1) == 'e')))
				i++;
		}
		else
			if(*i == '"'){ // Character Literal 
				i++;
				if((i = strstr(i, "\"")) == 0) 
					return 0; // error
				i++;
				}
			else
				if(*i == '-' && *(i + 1) == '>'){ // ->
					i += 2;
					if(*i == '*')			// ->*
						i++;
					}
				else
					if(*i == '<' && *(i + 1) == '<'){ // <<
						i += 2;
						if(*i == '=')			// <<=
							i++;
						}
					else
						if(*i == '>' && *(i + 1) == '>'){ // >>
							i += 2;
							if(*i == '=')			// >>=
								i++;
							}
						else
							if(*i == '.' && *(i + 1) == '.' && *(i + 2) == '.') // ...
								i += 3;
							else
								if((*i == '#' && *(i + 1) == '#') || // ##
									(*i == ':' && *(i + 1) == ':') || // ::
									(*i == '&' && *(i + 1) == '&') || // &&
									(*i == '|' && *(i + 1) == '|') || // ||
									(*i == '.' && *(i + 1) == '*') || // .*
									(*i == '+' && *(i + 1) == '+') || // ++
									(*i == '-' && *(i + 1) == '-') || // --
									((*i == '+' || *i == '-' || *i == '*' || *i == '/' || *i == '%' || 
									*i == '^' || *i == '|' || *i == '&' || 
									*i == '!' || *i == '<' || *i == '>' || *i == '=') && *(i + 1) == '=')) // x=
										i += 2;
								else
									i++;

	buffer_.set(i - buffer_.address());
	replaced_symbol = buffer_();
	buffer_() = 0;
	return marker;
}

void XPrmIArchive::releaseToken()
{
	//	xassert(replaced_symbol);
	buffer_() = replaced_symbol;
	replaced_symbol = 0;
}

void XPrmIArchive::putToken() 
{
	buffer_.set(putTokenOffset_);
}


void XPrmIArchive::passString(const char* token)
{
	const char* s = getToken();
	xassert(s);
	string name = s;
	releaseToken();
	if(name != token){
		XBuffer msg;
		msg  < "Expected Token: \"" < token
			< "\", Received Token: \"" < name.c_str() < "\", file: \"" < fileName_.c_str() < "\", line: " <= line();
		xassertStr(0 && "Expected another token", msg);
		ErrH.Abort(msg);
	}
}

bool XPrmIArchive::loadString(string& str)
{
	const char* s = getToken();
	xassert(s);
	str = s;
	releaseToken();
	if(str[0] == '"'){
		if(str[str.size() - 1] != '"'){
			XBuffer err;
			err < "Quotes aren't closed: " < s < "\n";
			err < "Line: " <= line();
			ErrH.Abort(err);
			}
		str.erase(0, 1);
		str.pop_back();
		collapse_spec_chars(str);
	}
	else if(str == "0")
		return false;
	return true;
}

void XPrmIArchive::skipValue()
{
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

bool XPrmIArchive::findSection(const char* sectionName)
{
	for(int i = 0; i < 2; i++){
		string name;
		loadString(name);
		if(name == sectionName){
			putToken();
			return true;
		}
		else{
			passString("=");
			skipValue();
			passString(";");
			bool endOfFile = !getToken();
			releaseToken();
			if(!endOfFile)
				putToken();
			else{
				buffer_.set(0);
				replaced_symbol = 0;
				putTokenOffset_ = 0;
			}
		}
	}
	return false;
}

int XPrmIArchive::line() const 
{
	return 1 + count((const char*)buffer_, (const char*)buffer_ + buffer_.offset, '\n');
}



