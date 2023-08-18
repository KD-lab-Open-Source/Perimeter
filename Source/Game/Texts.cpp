#include <vector>
#include "qd_textdb.h"
#include "xutl.h"

void qdTextDB::load_replacement_texts(const std::string& locale) {
    //Community credits
    /*
    if (startsWith(locale, "russian")) {
        load_lines({
           "Interface.Credits.Community=&FF3333Сообщество \"Периметра\"\n\n&FFFFFFPLACEHOLDER",
        }, true, locale);
    } else {
        load_lines({
           "Interface.Credits.Community=&FF3333\"Perimeter\" Community\n\n&FFFFFFPLACEHOLDER",
       }, true, locale);
    }
    */
    
    //Used for all langs
    load_lines({
       "Interface.Menu.ButtonLabels.<<<=<<<",
       "Interface.Menu.ButtonLabels.>>>=>>>",
       //Empty to not mess with ,'s
       ""
   }, true, locale);
}

void qdTextDB::load_supplementary_texts(const std::string& locale) {
    //Load per language texts
    if (startsWith(locale, "russian")) {
        load_lines({
           "GAME_CONTENT.PERIMETER=Периметр",
           "GAME_CONTENT.PERIMETER_ET=Периметр: Завет Императора",
           "GAME_CONTENT.PERIMETER_HD=Периметр: HD",
           "Interface.Menu.Messages.ReplayGameVersionDifferent=Этот повтор был сохранен в другой версии и может отображаться неправильно. Использованная версия игры:",
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
           "Interface.Menu.ButtonLabels.START SPLASH=ОТОБРАЖАТЬ НАЧАЛЬНЫЕ РОЛИКИ",
           "Interface.Menu.ButtonLabels.CAMERA MODE=РЕЖИМ КАМЕРЫ",
           "Interface.Menu.ButtonLabels.UI POSITION=ПОЗИЦИОНИРОВАНИЕ UI",
           "Interface.Menu.ButtonLabels.GRAB INPUT=ЗАХВАТ ВВОДА",
           "Interface.Menu.ButtonLabels.FOG=ТУМАН",
           "Interface.Menu.ButtonLabels.Network=Сеть",
           "Interface.Menu.ComboItems.Centered=По центру",
           "Interface.Menu.ComboItems.Left side=Левая сторона",
           "Interface.Menu.ComboItems.Right side=Правая сторона",
           "Interface.Menu.ComboItems.Stretched=Растянутый",
           "Interface.Menu.ComboItems.Screen=Экран",
           "Interface.Menu.ComboItems.Windowed=Оконный",
           "Interface.Menu.ComboItems.Private Server=Частный/Локальный сервер",
           "Interface.Menu.ComboItems.Public Server=Публичный/Интернет сервер",
           "Interface.Menu.ComboItems.No restrictions=Нет ограничений",
           "Interface.Menu.ComboItems.Classic=Классический",
           "Interface.Menu.Multiplayer.StartNewGame=Начать новую игру",
           "Interface.Menu.Multiplayer.Server=сервер",
           "Interface.Menu.Messages.WrongIPPort=Этот IP-адрес недоступен",
           "Interface.Menu.Messages.Multiplayer.IncorrectContent=Сервер содержит другие игровые ресурсы",
           "Interface.Menu.Messages.Multiplayer.IncorrectArch=Сервер имеет другую битность или архитектуру ЦПУ, другой тип билда (Debug/Release), операционную систему или использован другой компилятор (MSVC/Clang/GCC), пожалуйста, убедитесь, что они совпадают",
           "Interface.Menu.Messages.Multiplayer.SignatureError=Проверка подписи или CRC не прошла, соединение может быть ненадёжным",
           "Interface.Menu.Messages.Multiplayer.HostTerminated=Хост прекратил игру или отключился",
           "Interface.Menu.Messages.Multiplayer.StartingGame=Игра начинается...",
           "Interface.Menu.Messages.Confirmations.PendingChanges=Отложенные изменения не будут применены, вернуться в главное меню?",
           "Interface.Menu.Messages.Confirmations.ApplyChangesRestart=Чтобы применить изменения, требуется перезапуск игры, вы уверены?",
           //Empty to not mess with ,'s
           ""
       }, false, locale);
    } else {
        //Load english for rest until they are translated
        load_lines({
           "GAME_CONTENT.PERIMETER=Perimeter",
           "GAME_CONTENT.PERIMETER_ET=Perimeter: Emperor's Testament",
           "GAME_CONTENT.PERIMETER_HD=Perimeter: HD",
           "Interface.Menu.Messages.ReplayGameVersionDifferent=This replay was saved with a different version and may not display correctly, used game version:",
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
           "Interface.Menu.ButtonLabels.START SPLASH=DISPLAY INTRO VIDEOS",
           "Interface.Menu.ButtonLabels.CAMERA MODE=CAMERA MODE",
           "Interface.Menu.ButtonLabels.UI POSITION=UI POSITIONING",
           "Interface.Menu.ButtonLabels.GRAB INPUT=CAPTURE INPUT",
           "Interface.Menu.ButtonLabels.FOG=FOG",
           "Interface.Menu.ButtonLabels.Network=Network",
           "Interface.Menu.ComboItems.Centered=Centered",
           "Interface.Menu.ComboItems.Left side=Left side",
           "Interface.Menu.ComboItems.Right side=Right side",
           "Interface.Menu.ComboItems.Stretched=Stretched",
           "Interface.Menu.ComboItems.Screen=Screen",
           "Interface.Menu.ComboItems.Windowed=Windowed",
           "Interface.Menu.ComboItems.Private Server=Private/Local Server",
           "Interface.Menu.ComboItems.Public Server=Public/Online Server",
           "Interface.Menu.ComboItems.No restrictions=No restrictions",
           "Interface.Menu.ComboItems.Classic=Classic",
           "Interface.Menu.Multiplayer.StartNewGame=Start a new game",
           "Interface.Menu.Multiplayer.Server=Server",
           "Interface.Menu.Messages.WrongIPPort=IP port is wrong",
           "Interface.Menu.Messages.Multiplayer.IncorrectContent=Server has different game content",
           "Interface.Menu.Messages.Multiplayer.IncorrectArch=Server has different bits or CPU architecture, different build type (Debug/Release), Operating System or used a different compiler (MSVC/Clang/GCC), please ensure they match",
           "Interface.Menu.Messages.Multiplayer.SignatureError=Signature or CRC checks failed, connection may be unreliable",
           "Interface.Menu.Messages.Multiplayer.HostTerminated=Host has terminated session or disconnected",
           "Interface.Menu.Messages.Multiplayer.StartingGame=Starting the game...",
           "Interface.Menu.Messages.Confirmations.PendingChanges=Pending changes will not be applied, return to main menu?",
           "Interface.Menu.Messages.Confirmations.ApplyChangesRestart=Game restart is required to apply changes, are you sure?",
           //Empty to not mess with ,'s
           ""
       }, false, locale);
    }

    //Fill any previously missing texts
    load_lines({
       //These should be in released games but seems to be absent sometimes
       "Interface.Tips.ToClanPostfix=(&FF00FFclan&FFFFFF)",
       "Interface.Tips.ToAllPostfix=&FFFFFF",
       "Interface.Tips.ToClanPrefix=&FF00FFClan&FFFFFF: ",
       "Interface.Tips.ToAllPrefix=&FFFFFF",
       //Was not translated
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
    }, false, locale);
}
