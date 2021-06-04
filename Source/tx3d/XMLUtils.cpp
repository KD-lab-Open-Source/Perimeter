/**
 *	"Procedural 3D Texture Library"
 *	
 *	Utility methods for parsing XML and serializing "tx3d" library objects to XML
 *
 *	@version $Id: XMLUtils.cpp,v 1.2 2002/11/07 13:50:54 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "XMLUtils.hpp"
#include <stdio.h>

using namespace tx3d;

const string XMLUtils::VALUE = "value";
const string XMLUtils::TYPE = "type";

//------"serializing"-methods-------------

string XMLUtils::floatToString(float d) {
	//TODO: revision needed
	char floatBuffer[30 + 1];
//	sprintf(floatBuffer, "%1.1f", d);
	sprintf(floatBuffer, "%f", d);
	return string(floatBuffer);
}

string XMLUtils::intToString(int i) {
	//TODO: revision needed
	char intBuffer[11 + 1];
	sprintf(intBuffer, "%d", i);
	return string(intBuffer);
}

string XMLUtils::vectorToString(const Vector3D& v) {
	return
		floatToString(v.x) + "," + floatToString(v.y) + "," + floatToString(v.z);
}

string XMLUtils::createTag(const string& tagName,
						   const string& propertyName,
						   const string& value,
						   const string& content) {
	string res("<" + tagName);
	if (propertyName != "") {
		res += " " + propertyName + "='" + value + "'";
	}

	if (content == "") {
		res += "/>";
	} else {
		res += ">" + content + "</" + tagName + ">";
	}
	return res;
}

string XMLUtils::vectorToTag(const string& tagName, const Vector3D& v) {
	return
		createTag(tagName, VALUE, vectorToString(v));
}

string XMLUtils::floatToTag(const string& tagName, float d) {
	return
		createTag(tagName, VALUE, floatToString(d));
}

string XMLUtils::intToTag(const string& tagName, int i) {
	return
		createTag(tagName, VALUE, intToString(i));
}

string XMLUtils::serializableToTag(const string& tagName, const Serializable& s) {
	string type = s.getType();
	return 
		createTag(
			tagName,
			(type == "") ? "" : TYPE,
			type,
			s.paramString()
		);
}

//------"parsing"-methods-------------

Vector3D XMLUtils::vectorFromString(const string& v) {
	Vector3D res;
	string::size_type i = v.find(",");
	res.x = atof(v.substr(0, i).c_str());
	if (i != string::npos) {
		string::size_type j = v.find(",", ++i);
		res.y = atof(v.substr(i, j - i).c_str());
		if (j != string::npos) {
			res.z = atof(v.substr(++j).c_str());
		}
	}
	return res;
}

string XMLUtils::extractPropertyFromTag(const string& propertyName, const string& tag) {
	string::size_type i = tag.find(propertyName);
	string::size_type j = tag.find(">");
	if (i != string::npos && i < j) {
		i = tag.find_first_of("'\"", i + propertyName.length());
		if (i != string::npos) {
			j = i;
			i++;
			j = tag.find(tag[j], i);
			if (j != string::npos) {
				return
					tag.substr(i, j - i);
			}
		}
	}
	return "";
}

string XMLUtils::extractContentFromTag(const string& tag) {
	int level = 0;
	string::size_type tagBegin = string::npos;
	string::size_type tagEnd = string::npos;
	string::size_type begin = 0;
	string::size_type end = 0;
	while(end != string::npos) {
		begin = tag.find("<", end);
		if (begin != string::npos && begin != (tag.length() - 1)) {
			if ((begin + 3) < tag.length() && tag.substr(begin, 4) == "<!--") {
				end = tag.find("-->", begin + 3);
				if (end != string::npos) {
					end += 3;
				}
			} else {
				end = tag.find(">", begin + 1);
				if (end != string::npos) {
					if (tag[begin + 1] == '/') {
						level--;
						if (level == 0) {
							tagEnd = begin;
							end = string::npos;
						}
					} else if (tag[end - 1] != '/') {
						if (level == 0) {
							tagBegin = end + 1;
						}
						level++;
					}
				}
			}
		} else {
			end = string::npos;
		}
	}
	if (tagBegin != string::npos && tagEnd != string::npos) {
		return tag.substr(tagBegin, tagEnd - tagBegin);
	} else {
		return "";
	}
}

string XMLUtils::typeFromTag(const string& tag) {
	return
		extractPropertyFromTag(TYPE, tag);
}

Vector3D XMLUtils::vectorFromTag(const string& tag) {
	return
		vectorFromString(extractPropertyFromTag(VALUE, tag));
}


float XMLUtils::floatFromTag(const string& tag) {
	return
		atof(extractPropertyFromTag(VALUE, tag).c_str());
}

int XMLUtils::intFromTag(const string& tag) {
	return
		atof(extractPropertyFromTag(VALUE, tag).c_str());
}

bool XMLUtils::isTagHasName(const string& tag, const string& tagName) {
//TODO: revision needed
	return
		tag.find("<" + tagName) == 0 || tag.find("</" + tagName) == 0;
}

string XMLUtils::extractTagFromXML( const string& tagName,
									  int orderNumber,
									  const string& xml ) {
	int level = 0;
	int order = 0;
	string::size_type begin = 0;
	string::size_type end = 0;
	string::size_type tagBegin = string::npos;
	string::size_type tagEnd = string::npos;
	while(end != string::npos) {
		begin = xml.find("<", end);
		if (begin != string::npos && begin != (xml.length() - 1)) {
			if ((begin + 3) < xml.length() && xml.substr(begin, 4) == "<!--") {
				end = xml.find("-->", begin + 3);
				if (end != string::npos) {
					end += 3;
				}
			} else {
				end = xml.find(">", begin + 1);
				if (end != string::npos) {
					if (xml[begin + 1] == '/') {
						level--;
						if (level == 0 && isTagHasName(xml.substr(begin, end - begin), tagName)) {
							if (order == orderNumber) {
								tagEnd = end;
								end = string::npos;
							} else {
								order++;
							}
						}
					} else if (xml[end - 1] == '/') {
						if (level == 0 && isTagHasName(xml.substr(begin, end - begin), tagName)) {
							if (order == orderNumber) {
								tagBegin = begin;
								tagEnd = end;
								end = string::npos;
							} else {
								order++;
							}
						}
					} else {
						if (level == 0 && isTagHasName(xml.substr(begin, end - begin), tagName)) {
							tagBegin = begin;
						}
						level++;
					}
				}
			}
		} else {
			end = string::npos;
		}
	}

	if (tagBegin != string::npos && tagEnd != string::npos) {
		return xml.substr(tagBegin, tagEnd - tagBegin + 1);
	} else {
		return "";
	}
}

void XMLUtils::indentXML(string& xml) {
	int level = 0;
	string::size_type begin = 0;
	string::size_type end = 0;
	while(end != string::npos) {
		begin = xml.find("<", end);
		if (begin != string::npos && begin != (xml.length() - 1)) {
			if ((begin + 3) < xml.length() && xml.substr(begin, 4) == "<!--") {
				end = xml.find("-->", begin + 3);
				if (end != string::npos) {
					end += 3;
				}
			} else {
				end = xml.find(">", begin + 1);
				if (end != string::npos) {
					if (xml[begin + 1] == '/') {
						level--;
						xml.insert(begin, "\n" + string(level, '\t'));
					} else if (xml[end - 1] == '/') {
						xml.insert(begin, "\n" + string(level, '\t'));
					} else {
						xml.insert(begin, "\n" + string(level, '\t'));
						level++;
					}
				}
			}
		} else {
			end = string::npos;
		}
	}
}
