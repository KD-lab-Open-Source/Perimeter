#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

#include "Dictionary.hxx"

class Dictionary : DictionaryPrm
{
public:
	Dictionary(const char* fileName)
	{
		XStream ff(0);
		if(fileName && ff.open(fileName, XS_IN)){
			ff.close();
			ScriptParser parser(fileName, ScriptParser::LOAD, 0);
			parser >> (*this);
		}
		vector<DictionaryRecord>::iterator i;
		FOR_EACH(dictionaryRecords, i)
			map_.insert(Map::value_type(key(string(i->parentName), string(i->oldName)), string(i->newName)));
	}
	const string& map(const string& parentName, const string& oldName)
	{
		Map::iterator i = map_.find(key(parentName, oldName));
		if(i != map_.end())
			return i->second;
		return oldName;
	}
	
	void append(const char* parentName, const char* oldName, const char* newName)
	{
		dictionaryRecords.push_back(DictionaryRecord());
		DictionaryRecord& record = dictionaryRecords.back();
		record.parentName = parentName;
		record.oldName = oldName;
		record.newName = newName;
		map_.insert(Map::value_type(key(string(record.parentName), string(record.oldName)), string(record.newName)));
	}
	
	void remove(const char* parentName, const char* oldName)
	{
		vector<DictionaryRecord>::iterator i;
		FOR_EACH(dictionaryRecords, i)
			if(string(i->parentName) == parentName && string(i->oldName) == oldName){
				dictionaryRecords.erase(i);
				break;
			}
			map_.erase(key(string(parentName), string(oldName)));
	}
	
	void save(const char* fileName)
	{
		ScriptParser parser(fileName, ScriptParser::SAVE_TEXT);
		parser << (*this);
	}
	
	string key(const string& parentName, const string& oldName)
	{
		return parentName + "::"+ oldName;
	}
	
	int size() const { return dictionaryRecords.size(); }
	
private:
	typedef hash_map<string, string> Map;
	Map map_;
};

#endif //__DICTIONARY_H__