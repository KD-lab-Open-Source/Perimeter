#ifndef __QD_TEXTDB_H__
#define __QD_TEXTDB_H__

// hash_map is an old and non-standard MS extension
// see https://docs.microsoft.com/en-us/cpp/standard-library/hash-map?view=msvc-160
#if defined(_MSC_VER) && (_MSC_VER < 1900)
#include <hash_map> 
#else
#include <unordered_map>
#endif
#include <string>
#include <list>
/// База данных с текстами.
class qdTextDB
{
public:
	qdTextDB();
	~qdTextDB();

	/// Очистка базы.
	void clear(){ texts_.clear(); }

	/// Возвращает текст с идентификатором text_id.
	/**
	Если текст не найден - вернет пустую строку.
	*/
	const char* getText(const char* text_id) const;

	/// Возвращает звук к тексту с идентификатором text_id.
	const char* getSound(const char* text_id) const;

	/// Возвращает комментарий текста с идентификатором text_id.
	const char* getComment(const char* text_id) const;

	/// Загрузка базы.
	/**
	Если clear_old_texts == true, то загруженная в данный момент база очищается.
	В финальной версии база комментариев игнорируется.
	*/
	bool load(const char* file_name,const char* comments_file_name = NULL,bool clear_old_texts = true);

	typedef std::list<std::string> IdList;
	void getIdList(const char* mask, IdList& idList);

	static qdTextDB& instance();

private:

	struct qdText
	{
		qdText(const char* text,const char* snd) : text_(text), sound_(snd) { }

		std::string text_;
		std::string sound_;
#ifndef _FINAL_VERSION_
		std::string comment_;
#endif
	};

	// hash_map is an old and non-standard MS extension
	// see https://docs.microsoft.com/en-us/cpp/standard-library/hash-map?view=msvc-160
#if defined(_MSC_VER) && (_MSC_VER < 1900)
	typedef std::hash_map<std::string, qdText> qdTextMap;
#else
	typedef std::unordered_map<std::string, qdText> qdTextMap;
#endif
	
	qdTextMap texts_;
};

#endif // __QD_TEXTDB_H__
