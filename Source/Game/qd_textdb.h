#ifndef __QD_TEXTDB_H__
#define __QD_TEXTDB_H__


#include <unordered_map>
#include <string>
#include <list>
#include <utility>
/// База данных с текстами.
class qdTextDB
{
public:
	qdTextDB();
	~qdTextDB();

	/// Очистка базы.
	void clear();

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
	В финальной версии база комментариев игнорируется.
	*/
	bool load(const std::string& locale, const char* file_name, const char* comments_file_name, bool replace_old_texts, bool format_txt);

    ///Loads texts from lines using a basic format as:
    ///text_id=text content\nmore content etc
    void load_lines(const std::vector<std::string>& lines, bool replace_old_texts, const std::string& locale = "");

    ///Includes texts that must be set always
    void load_replacement_texts(const std::string& locale);

    ///Includes texts that are not usually in retail game translations, these only are added if DB don't have them already
    void load_supplementary_texts(const std::string& locale);
    
    ///Load texts from folder
    void load_from_directory(const std::string& locale, const std::string& path, bool exclude_mods);

    ///Exports currently loaded texts starting with text_id or all if null/empty into Texts.txt file in game root
    void exportTexts(const char* text_id);

	typedef std::list<std::string> IdList;
	void getIdList(const char* mask, IdList& idList);

	static qdTextDB& instance();

private:
	class qdText
	{
    public:
        qdText() = default;
		qdText(std::string id, std::string text, std::string snd) : id_(std::move(id)), text_(std::move(text)), sound_(std::move(snd)) { }
        ~qdText() = default;

        std::string id_;
		std::string text_;
		std::string sound_;
#ifndef _FINAL_VERSION_
		std::string comment_;
#endif
	};

    /// Obtain qdText for this entry if exists
    qdTextDB::qdText* get_entry(const char* id_str, qdText* default_text);

    void add_entry(const std::string& id_str, const qdText& text, bool replace_old_texts);

    typedef std::unordered_map<std::string, qdText> qdTextMap;
	qdTextMap texts_;
};

#endif // __QD_TEXTDB_H__
