/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "StdAfx.h"

#include "xutil.h"
#include "tweaks.h"
#include "files/files.h"
#include "qd_textdb.h"

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

const char* qdTextDB::getText(const char* text_id) const
{
    if (text_id != nullptr && *(text_id) != 0) {
        std::string id_str = text_id;
        strlwr(id_str.data());
        if (0 < texts_.count(id_str)) {
            return texts_.at(id_str).text_.c_str();
        }

#ifdef PERIMETER_DEBUG
        //Ignore certain missing texts
        if (!startsWith(id_str, "interface.tips.names.")
         && !startsWith(id_str, "interface.menu.buttonlabels.name")
         && !startsWith(id_str, "mission frames.")
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
    std::string id_str = text_id;
    strlwr(id_str.data());
	qdTextMap::const_iterator it = texts_.find(id_str);
	if(it != texts_.end())
		return it->second.sound_.c_str();

	static const char* const str = "";
	return str;
}

const char* qdTextDB::getComment(const char* text_id) const
{
#ifndef _FINAL_VERSION_
    std::string id_str = text_id;
    strlwr(id_str.data());
	qdTextMap::const_iterator it = texts_.find(id_str);
	if(it != texts_.end())
		return it->second.comment_.c_str();
#endif

	static const char* const str = "";
	return str;
}

void qdTextDB::add_entry(const std::string& id_str, const qdText& text, bool replace_old_texts) {
    //Avoid adding if text already exists
    if (replace_old_texts || texts_.count(id_str) == 0) {
        texts_[id_str] = text;
    }
}

bool qdTextDB::load(const char* file_name, const char* comments_file_name,
                    bool clear_old_texts, bool replace_old_texts, bool format_txt)
{
    XStream fh(0);
    if (!fh.open(convert_path_content(file_name), XS_IN)) {
        fprintf(stderr, "Error loading texts: %s\n", file_name);
        return false;
    }

    if (clear_old_texts) {
        clear();
    }
    
    if (format_txt) {
        std::string acc_line;
        char c = 0;
        bool escape = false;
        std::vector<std::string> lines;
        
        do {
            //Read char
            fh.read(&c,1);
            
            //Only handle special chars if not escaped
            if (!escape) {
                if (c == '\n') {
                    //Newline, store accumulator and clear
                    lines.emplace_back(acc_line);
                    acc_line.clear();
                    continue;
                } else if (c == '\\') {
                    //Escape char
                    escape = true;
                    continue;
                } else if (c == '#' && acc_line.empty()) {
                    //This is a comment, skip
                    continue;
                }
            }
            
            //Transform \ + n into \n
            if (c == 'n' && escape) {
                c = '\n';
            }
            
            //Append current char
            acc_line += c;
        } while(!fh.eof());
        if (!acc_line.empty()) {
            lines.emplace_back(acc_line);
        }
        
        load_lines(lines, replace_old_texts);
    } else {
        int text_cnt;
        fh > text_cnt;

        for (int i = 0; i < text_cnt; i++) {
            int id_length;
            fh > id_length;

            char* id_str = static_cast<char*>(malloc((id_length + 1) * sizeof(char)));
            fh.read(id_str, id_length);
            id_str[id_length] = 0;
            strlwr(id_str);

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

            add_entry(id_str, qdText(txt_str, snd_str), replace_old_texts);

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

void qdTextDB::load_lines(const std::vector<std::string>& lines, bool replace_old_texts) {
    for (auto& line : lines) {
        size_t pos = line.find('=');
        if(pos == std::string::npos) {
            continue;
        }
        std::string id_str = line.substr(0, pos);
        if (id_str.empty()) continue;
        std::string txt_str = line.substr(pos + 1);
        if (txt_str.empty()) continue;
        strlwr(id_str.data());
        add_entry(id_str, qdText(txt_str, ""), replace_old_texts);
    }
}

void qdTextDB::load_supplementary_texts(const std::string& lang) {
    //Load per language texts
    //TODO
    if (lang == "Russian") {
        load_lines({
            
        }, false);
    }
    
    //Load english ones to fill any previously missing texts
    load_lines({
       //Game content names
       "GAME_CONTENT.PERIMETER=Perimeter",
       "GAME_CONTENT.PERIMETER_ET=Perimeter: Emperor's Testament",
       "GAME_CONTENT.PERIMETER_HD=Perimeter: HD",
       //Extra interface messages
       "Interface.Menu.Messages.GameContentMissing=This contains game content that is not present or enabled in your installation, make sure these are installed and enabled in your game:\n",
       "Interface.Menu.Messages.WorldMissing=This contains a map/world that is not present or enabled in your installation, make sure that is installed and enabled in your game:\n\n",
       //New main menu items
       "Interface.Menu.ButtonLabels.MULTI PLAYER=MULTI PLAYER",
       "Interface.Menu.ButtonLabels.ADDONS=ADDONS",
       "Interface.Menu.ButtonLabels.ADDON ENABLE STATE=This addon is:",
       "Interface.Menu.ButtonLabels.CONTENTS=CONTENTS",
       "Interface.Menu.ButtonLabels.DIRECT=DIRECT",
       "Interface.Menu.ButtonLabels.Password=Password:",
       "Interface.Menu.ButtonLabels.Port=Port:",
       "Interface.Menu.ButtonLabels.SERVER TYPE=Server Type:",
       "Interface.Menu.ComboItems.Private Server=Private/Local Server",
       "Interface.Menu.ComboItems.Public Server=Public/Online Server",
       "Interface.Menu.Multiplayer.StartNewGame=Start a new game",
       "Interface.Menu.Messages.WrongIPPort=IP port is wrong",
       "Interface.Menu.Messages.Multiplayer.IncorrectContent=Server has different game content",
       //Addon menu messages
       "Interface.Menu.Messages.Confirmations.PendingChanges=Pending changes will not be applied, return to main menu?",
       "Interface.Menu.Messages.Confirmations.ApplyAddonsChanges=Game restart is required to apply changes, are you sure?"
   }, false);
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
