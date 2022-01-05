/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "StdAfx.h"

#include "xutil.h"
#include "tweaks.h"
#include "files/files.h"
#include "qd_textdb.h"
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

bool qdTextDB::load(const std::string& locale, const char* file_name, const char* comments_file_name,
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
            if (escape) {
                //Transform \ + n into \n
                if (c == 'n') {
                    c = '\n';
                }
                escape = false;
            } else {
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
            
            //Append current char
            acc_line += c;
        } while(!fh.eof());
        if (!acc_line.empty()) {
            lines.emplace_back(acc_line);
        }
        
        load_lines(lines, replace_old_texts);
    } else {
        bool russian = locale == "russian";
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

            bool discard = false;
            if (!russian && strcmp(id_str, "Interface.Tips.mission_editor_help") == 0) {
                //Was never translated in other languages in releases so we discard it
                discard = true;
            }
            if (!discard) {
                add_entry(id_str, qdText(txt_str, snd_str), replace_old_texts);
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

        std::string id_str = line.substr(0, pos);
        if (id_str.empty()) continue;
        strlwr(id_str.data());

        std::string txt_str = line.substr(pos + 1);
        if (txt_str.empty()) continue;
        if (!locale.empty()) {
            txt_str = getLocaleString(txt_str.c_str(), locale);
        }

        add_entry(id_str, qdText(txt_str, ""), replace_old_texts);
    }
}

void qdTextDB::load_supplementary_texts(const std::string& locale) {
    //Load per language texts
    if (locale == "russian") {
        load_lines({
           "GAME_CONTENT.PERIMETER=Периметр",
           "GAME_CONTENT.PERIMETER_ET=Периметр: Завет Императора",
           "GAME_CONTENT.PERIMETER_HD=Периметр: HD",
           "Interface.Menu.Messages.GameContentMissing=Содержит ресурсы, которые не представлены или не включены в этой копии игры, убедитесь, что они установлены и включены:\n",
           "Interface.Menu.Messages.WorldMissing=Содержит карту, которая не представлена или не включена в этой копии игры, убедитесь, что она установлена и включена:\n\n",
           "Interface.Menu.ButtonLabels.MULTIPLAYER=МУЛЬТИПЛЕЕР",
           "Interface.Menu.ButtonLabels.MODS=МОДЫ",
           "Interface.Menu.ButtonLabels.MOD ENABLE STATE=мод состояние:",
           "Interface.Menu.ButtonLabels.CHANGE_CONTENTS=СМЕНИТЬ\nКАМПАНИЮ",
           "Interface.Menu.ButtonLabels.DIRECT=ВВОД IP",
           "Interface.Menu.ButtonLabels.Password=Пароль:",
           "Interface.Menu.ButtonLabels.Port=Порт:",
           "Interface.Menu.ButtonLabels.SERVER TYPE=Тип сервера:",
           "Interface.Menu.ButtonLabels.CANCEL=ОТМЕНА",
           "Interface.Menu.ButtonLabels.RUN IN BACKGROUND=ЗАПУСТИТЬ ИГРУ НА ФОНЕ",
           "Interface.Menu.ButtonLabels.UI POSITION=ПОЗИЦИОНИРОВАНИЕ UI",
           "Interface.Menu.ButtonLabels.GRAB INPUT=ЗАХВАТ ВВОДА",
           "Interface.Menu.ButtonLabels.FOG=ТУМАН",
           "Interface.Menu.ComboItems.Centered=По центру",
           "Interface.Menu.ComboItems.Left side=Левая сторона",
           "Interface.Menu.ComboItems.Right side=Правая сторона",
           "Interface.Menu.ComboItems.Stretched=Растянутый",
           "Interface.Menu.ComboItems.Screen=Экран",
           "Interface.Menu.ComboItems.Windowed=Оконный",
           "Interface.Menu.ComboItems.Private Server=Частный/Локальный сервер",
           "Interface.Menu.ComboItems.Public Server=Публичный/Интернет сервер",
           "Interface.Menu.Multiplayer.StartNewGame=Начать новую игру",
           "Interface.Menu.Messages.WrongIPPort=Этот IP-адрес недоступен",
           "Interface.Menu.Messages.Multiplayer.IncorrectContent=Сервер содержит другие игровые ресурсы",
           "Interface.Menu.Messages.Multiplayer.IncorrectArch=Сервер имеет другую битность или архитектуру ЦПУ, другой тип билда (Debug/Release), операционную систему или использован другой компилятор (MSVC/Clang/GCC), пожалуйста, убедитесь, что они совпадают",
           "Interface.Menu.Messages.Multiplayer.SignatureError=Проверка подписи или CRC не прошла, соединение может быть ненадёжным",
           "Interface.Menu.Messages.Multiplayer.HostTerminated=Хост прекратил игру или отключился",
           "Interface.Menu.Messages.Confirmations.PendingChanges=Отложенные изменения не будут применены, вернуться в главное меню?",
           "Interface.Menu.Messages.Confirmations.ApplyChangesRestart=Чтобы применить изменения, требуется перезапуск игры, вы уверены?",
           //Empty to not mess with ,'s
           ""
        }, false, locale);
    }

    //Load english ones to fill any previously missing texts
    load_lines({
       "GAME_CONTENT.PERIMETER=Perimeter",
       "GAME_CONTENT.PERIMETER_ET=Perimeter: Emperor's Testament",
       "GAME_CONTENT.PERIMETER_HD=Perimeter: HD",
       "Interface.Menu.Messages.GameContentMissing=This contains game content that is not present or enabled in your installation, make sure these are installed and enabled in your game:\n",
       "Interface.Menu.Messages.WorldMissing=This contains a map/world that is not present or enabled in your installation, make sure that is installed and enabled in your game:\n\n",
       "Interface.Menu.ButtonLabels.MULTIPLAYER=MULTIPLAYER",
       "Interface.Menu.ButtonLabels.MODS=MODS",
       "Interface.Menu.ButtonLabels.MOD ENABLE STATE=Mod state:",
       "Interface.Menu.ButtonLabels.CHANGE_CONTENTS=CHANGE\nCAMPAIGN",
       "Interface.Menu.ButtonLabels.DIRECT=ENTER IP",
       "Interface.Menu.ButtonLabels.Password=Password:",
       "Interface.Menu.ButtonLabels.Port=Port:",
       "Interface.Menu.ButtonLabels.SERVER TYPE=Server Type:",
       "Interface.Menu.ButtonLabels.CANCEL=CANCEL",
       "Interface.Menu.ButtonLabels.RUN IN BACKGROUND=RUN GAME IN BACKGROUND",
       "Interface.Menu.ButtonLabels.UI POSITION=UI POSITIONING",
       "Interface.Menu.ButtonLabels.GRAB INPUT=CAPTURE INPUT",
       "Interface.Menu.ButtonLabels.FOG=FOG",
       "Interface.Menu.ComboItems.Centered=Centered",
       "Interface.Menu.ComboItems.Left side=Left side",
       "Interface.Menu.ComboItems.Right side=Right side",
       "Interface.Menu.ComboItems.Stretched=Stretched",
       "Interface.Menu.ComboItems.Screen=Screen",
       "Interface.Menu.ComboItems.Windowed=Windowed",
       "Interface.Menu.ComboItems.Private Server=Private/Local Server",
       "Interface.Menu.ComboItems.Public Server=Public/Online Server",
       "Interface.Menu.Multiplayer.StartNewGame=Start a new game",
       "Interface.Menu.Messages.WrongIPPort=IP port is wrong",
       "Interface.Menu.Messages.Multiplayer.IncorrectContent=Server has different game content",
       "Interface.Menu.Messages.Multiplayer.IncorrectArch=Server has different bits or CPU architecture, different build type (Debug/Release), Operating System or used a different compiler (MSVC/Clang/GCC), please ensure they match",
       "Interface.Menu.Messages.Multiplayer.SignatureError=Signature or CRC checks failed, connection may be unreliable",
       "Interface.Menu.Messages.Multiplayer.HostTerminated=Host has terminated session or disconnected",
       "Interface.Menu.Messages.Confirmations.PendingChanges=Pending changes will not be applied, return to main menu?",
       "Interface.Menu.Messages.Confirmations.ApplyChangesRestart=Game restart is required to apply changes, are you sure?",
       //These should be in released games but seems to be absent sometimes
       "Interface.Tips.ToClanPostfix=(&FF00FFto clan&FFFFFF)",
       "Interface.Tips.ToAllPostfix=(&FF00FFto all&FFFFFF)",
       "Interface.Tips.ToClanPrefix=&FF00FFTo clan&FFFFFF: ",
       "Interface.Tips.ToAllPrefix=&FF00FFTo all&FFFFFF: ",
       //Not translated
       "Interface.Tips.mission_editor_help=Mission Editor:\n\n"
       "Q - Player \"Me\"\n"
       "W - Player \"World\"\n"
       "E - Player \"Enemy\"\n\n"
       "Working with an object:\n\n"
       "F, INS - Create object\n"
       "D, DEL - Delete object\n"
       "Shift-D - Delete Object Silently\n\n"
       "Ctrl-C - Save Object\n"
       "Ctrl-V - Copy Object\n\n"
       "V + mouse_move - Move object\n"
       "Shift-V + mouse_move - Move object by Z\n"
       "C + mouse_move - Rotate the object\n"
       "Shift-C + mouse_move - Move object\n"
       "Ctrl-Shift-C + mouse_move - Move object directly\n"
       "X + mouse_move - Zoom in / out the object\n\n"
       "F4 - Edit Text Data\n"
       "Ctrl-Enter - Trigger Editor from Ilyuha\n"
       "Ctrl-S - Record Mission\n"
       "Ctrl-O - Open mission file\n"
       "Ctrl-Shift-O - Reopen the current mission\n"
       "Ctrl-M - Remember the camera of the current player\n\n"
       "Ctrl-H - Enable / disable indestructibility editing\n"
       "         Edit LeftMouse _clockwise_,\n"
       "         Shift - destructible\n"
       "Enter - End the current region (the right button is busy rotating)\n"
       "Cancel - Cancel the current region\n"
       "Ctrl-Alt-H - Erase All Indestructibility\n\n"
       "Camera:\n"
       "F9 - Add Spline Control Point\n"
       "Shift-F9 - Delete the last point of the spline\n"
       "Ctrl-F9 - start / stop playback\n"
       "Ctrl-Shift-F9 - Clear Current Spline\n"
       "Ctrl-Alt-F9 - write current spline ",
       //Empty to not mess with ,'s
       ""
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
