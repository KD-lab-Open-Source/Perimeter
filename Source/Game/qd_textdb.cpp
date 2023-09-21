/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "StdAfx.h"

#include "xutil.h"
#include "tweaks.h"
#include "files/files.h"
#include "qd_textdb.h"
#include "codepages/codepages.h"
#include "Localization.h"

/* ----------------------------- STRUCT SECTION ----------------------------- */
/* ----------------------------- EXTERN SECTION ----------------------------- */
/* --------------------------- PROTOTYPE SECTION ---------------------------- */
/* --------------------------- DEFINITION SECTION --------------------------- */

qdTextDB::qdTextDB()
{
}

qdTextDB::~qdTextDB()
{
	clear();
}

qdTextDB& qdTextDB::instance()
{
	static qdTextDB db;
	return db;
}

void qdTextDB::clear() {
    texts_.clear();
}

qdTextDB::qdText* qdTextDB::get_entry(const char* id_str, qdText* default_text) {
    if (0 < texts_.count(id_str)) {
        return &texts_.at(id_str);
    }
    return default_text;
}

void convert_text_to_txt(std::string& text) {
    string_replace_all(text, "\\", "\\\\");
    string_replace_all(text, "\r", "\\r");
    string_replace_all(text, "\n", "\\n");
}

void qdTextDB::exportTexts(const char* text_id) {
    const std::string& locale = getLocale();
    std::string id_str;
    if (text_id != nullptr && *(text_id) != 0) {
        id_str = string_to_lower(text_id);
    }

    XStream f ("Texts_" + locale + ".txt", XS_OUT);
    for (auto& t : texts_) {
        if (id_str.empty() || startsWith(t.first, id_str)) {
            //Write main text
            f.write_str(t.second.id_);
            f.write('=');
            std::string line;
            if (locale.empty()) {
                line = t.second.text_;
            } else {
                line = convertToUnicode(t.second.text_, locale);
            }
            if (!line.empty()) {
                convert_text_to_txt(line);
                f.write_str(line);
            }
            f.write('\n');

            //Write audio
            line = t.second.sound_;
            if (!line.empty()) {
                convert_text_to_txt(line);
                f.write_str(t.second.id_);
                f.write_str(".text_audio=");
                f.write_str(line);
                f.write('\n');
            }

#ifndef _FINAL_VERSION_
            //Write comment
            line = t.second.comment_;
            if (!line.empty()) {
                convert_text_to_txt(line);
                f.write_str(t.second.id_);
                f.write_str(".text_comment=");
                f.write_str(line);
                f.write('\n');
            }
#endif
        }
    }
    f.close();
}

const char* qdTextDB::getText(const char* text_id) const
{
    if (text_id != nullptr && *(text_id) != 0) {
        std::string id_str = string_to_lower(text_id);
        if (0 < texts_.count(id_str)) {
            return texts_.at(id_str).text_.c_str();
        }

#ifdef PERIMETER_DEBUG
        //Ignore certain missing texts
        if (!startsWith(id_str, "interface.tips.names.")
         && !startsWith(id_str, "interface.menu.buttonlabels.name")
         && !startsWith(id_str, "mission frames.")
         && !startsWith(id_str, "mapnames.")
         && !endsWith(id_str, "interface.menu.buttonlabels.")) {
            fprintf(stderr, "getText missing: '%s'\n", id_str.c_str());
        }
#endif
    }
    static const char* const str = "";
	return str;
}

const char* qdTextDB::getSound(const char* text_id) const
{
    std::string id_str = string_to_lower(text_id);
	qdTextMap::const_iterator it = texts_.find(id_str);
	if(it != texts_.end())
		return it->second.sound_.c_str();

	static const char* const str = "";
	return str;
}

const char* qdTextDB::getComment(const char* text_id) const
{
#ifndef _FINAL_VERSION_
    std::string id_str = string_to_lower(text_id);
	qdTextMap::const_iterator it = texts_.find(id_str);
	if(it != texts_.end())
		return it->second.comment_.c_str();
#endif

	static const char* const str = "";
	return str;
}

void qdTextDB::add_entry(const std::string& id_str, const qdText& text, bool replace_old_texts) {
    //Avoid adding if text already exists
    if (replace_old_texts) {
        texts_.insert_or_assign(id_str, text);
    } else {
        texts_.insert(std::pair(id_str, text));
    }
}

bool qdTextDB::load(const std::string& locale, const char* file_name, const char* comments_file_name,
                    bool replace_old_texts, bool format_txt)
{
    XStream fh(0);
    fprintf(stdout, "Loading texts: %s replace: %d\n", file_name, replace_old_texts);
    if (!fh.open(convert_path_content(file_name), XS_IN)) {
        fprintf(stderr, "Error loading texts: %s\n", file_name);
        return false;
    }
    
    if (format_txt) {
        std::string acc_line;
        char c = 0;
        bool escape = false;
        bool comment = false;
        std::vector<std::string> lines;
        
        do {
            //Read char
            fh.read(&c,1);
            
            //Only handle special chars if not escaped
            if (escape) {
                //Transform \ + n into \n
                switch (c) {
                    case 'n':
                        c = '\n';
                        break;
                    case 'r':
                        c = '\r';
                        break;
                    default:
                        break;
                }
                escape = false;
            } else {
                if (c == '\n') {
                    //Newline, store accumulator and clear
                    lines.emplace_back(acc_line);
                    comment = false;
                    escape = false;
                    acc_line.clear();
                    continue;
                } else if (comment || c == '#') {
                    //This is a comment, skip
                    comment = true;
                    continue;
                } else if (c == '\\') {
                    //Escape char
                    escape = true;
                    continue;
                } else if (c == '\r') {
                    continue;
                }
            }
            
            //Append current char
            acc_line += c;
        } while(!fh.eof());
        if (!acc_line.empty()) {
            lines.emplace_back(acc_line);
        }
        
        load_lines(lines, replace_old_texts, locale);
    } else {
        bool russian = startsWith(locale, "russian");
        int text_cnt;
        fh > text_cnt;

        for (int i = 0; i < text_cnt; i++) {
            int id_length;
            fh > id_length;

            char* id_str = static_cast<char*>(malloc((id_length + 1) * sizeof(char)));
            fh.read(id_str, id_length);
            id_str[id_length] = 0;
            std::string id_str_lwr = string_to_lower(id_str);

            int txt_length;
            fh > txt_length;

            char* txt_str = static_cast<char*>(malloc((txt_length + 1) * sizeof(char)));
            fh.read(txt_str, txt_length);
            txt_str[txt_length] = 0;

            int snd_length;
            fh > snd_length;

            char* snd_str = static_cast<char*>(malloc((snd_length + 1) * sizeof(char)));
            fh.read(snd_str, snd_length);
            snd_str[snd_length] = 0;

            bool discard = false;
            if (!russian && id_str_lwr == "interface.tips.mission_editor_help") {
                //Was never translated in other languages in releases, so we discard it
                discard = true;
            }
            if (!discard) {
                add_entry(id_str_lwr, qdText(id_str, txt_str, snd_str), replace_old_texts);
            }

            free(id_str);
            free(txt_str);
            free(snd_str);
        }

        fh.close();
    }

#ifndef _FINAL_VERSION_
	if(comments_file_name){
		if(!fh.open(convert_path_content(comments_file_name),XS_IN))
			return true;

		fh > text_cnt;
		for(int i = 0; i < text_cnt; i++){
			int id_length;
			fh > id_length;

			id_str.resize(id_length);
			fh.read(&*id_str.begin(),id_length);

			int txt_length;
			fh > txt_length;

			txt_str.resize(txt_length);
			fh.read(&*txt_str.begin(),txt_length);

			int snd_length;
			fh > snd_length;

			snd_str.resize(snd_length);
			fh.read(&*snd_str.begin(),snd_length);

			qdTextMap::iterator it = texts_.find(id_str.c_str());
			if(it != texts_.end())
				it->second.comment_ = txt_str;
		}
	}
#endif

	return true;
}

void qdTextDB::load_lines(const std::vector<std::string>& lines, bool replace_old_texts, const std::string& locale) {
    for (auto& line : lines) {
        size_t pos = line.find('=');
        if(pos == std::string::npos) {
            continue;
        }
        
        qdText entry;

        entry.id_ = line.substr(0, pos);
        if (entry.id_.empty()) continue;
        std::string id_str = string_to_lower(entry.id_.c_str());
        std::string line_str = line.substr(pos + 1);
        
        //Check where this string goes to
        if (endsWith(id_str, ".text_audio")) {
            string_replace_all(id_str, ".text_audio", "");
            entry = *get_entry(id_str.c_str(), &entry);
            entry.sound_ = line_str;
        } else if (endsWith(id_str, ".text_comment")) {
            string_replace_all(id_str, ".text_comment", "");
            entry = *get_entry(id_str.c_str(), &entry);
#ifdef _FINAL_VERSION_
            continue;
#else
            entry.comment_ = line_str;
#endif
        } else {
            entry = *get_entry(id_str.c_str(), &entry);
            if (!locale.empty()) {
                line_str = convertToCodepage(line_str.c_str(), locale);
            }
            entry.text_ = line_str;
        }

        add_entry(id_str, entry, replace_old_texts);
    }
}

void qdTextDB::load_from_directory(const std::string& locale, const std::string& path, bool exclude_mods) {
    for (const auto& entry: get_content_entries_directory(path)) {
        //Only load files from non mod folders if exclude_mods is true, otherwise only load from mods
        //This ensures every file is parsed only once, that mod can override base content if required and
        if (startsWith(entry->key_content, "mods/") == exclude_mods) {
            continue;
        }
        std::filesystem::path entry_path = std::filesystem::u8path(entry->key);
        std::string name = entry_path.filename().u8string();
        name = string_to_lower(name.c_str());
        //Ignore any starting file with . like .DS_Store
        if (name[0] == '.') continue;
        //Always ignore files like "texts_comments.btdb"
        if (endsWith(name, "_comments.btdb")) continue;
        bool replace = name.rfind("_noreplace.") == std::string::npos;
        bool txt = getExtension(name, true) == "txt";
        load(locale, entry->path_content.c_str(), nullptr, replace, txt);
    }
}

void qdTextDB::getIdList(const char* mask, IdList& idList)
{
	idList.clear();
	qdTextMap::iterator i;
	FOR_EACH(texts_, i){
		std::string str = i->first;
		if(!i->first.find(mask)){
			std::string str = i->first;
			str.erase(0, strlen(mask) + 1);
			size_t pos = str.find(".");
			if(pos != std::string::npos)
				str.erase(pos, str.size());
			if(std::find(idList.begin(), idList.end(), str) == idList.end())
				idList.push_back(str);
		}
	}

	idList.sort();
}
