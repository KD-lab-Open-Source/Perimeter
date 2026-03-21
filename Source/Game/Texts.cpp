#include <vector>
#include "StdAfx.h"
#include "xutl.h"
#include "UnitAttribute.h"
#include "GameContent.h"
#include "qd_textdb.h"

#ifdef GPX
constexpr const char* CREDITS_HEADER_RUS = R"(

&FFFFBBПосвящено памяти
Андрея "КранК" Кузьмина
1971 - 2022


&66CCFFGamePix
gamepix.com&FFFFFF

&CCCCFFВеб версия&FFFFFF
Александр "caiiiycuk" Гурьянов
Станислав "sta3ik" Павлов


&66CCFFК-Д ЛАБ
kdlab.com&FFFFFF

Андрей "КранК" Кузьмин
)";
//END CREDITS_HEADER_RUS

constexpr const char* CREDITS_HEADER_ENG = R"(

&FFFFBBDedicated to the memory of
Andrey "KranK" Kuzmin
1971 - 2022


&66CCFFGamePix
gamepix.com&FFFFFF

&CCCCFFWeb Version&FFFFFF
Alexander "caiiiycuk" Guryanov
Stanislav "sta3ik" Pavlov

&66CCFFK-D LAB
kdlab.com&FFFFFF

Andrey "KranK" Kuzmin
)";
//END CREDITS_HEADER_ENG

#else

constexpr const char* CREDITS_HEADER_RUS = R"(
&66CCFFК-Д ЛАБ
kdlab.com&FFFFFF


&FFFFBBПосвящено памяти
Андрея "КранК" Кузьмина
1971 - 2022
)"; //END CREDITS_HEADER_RUS

constexpr const char* CREDITS_HEADER_ENG = R"(
&66CCFFK-D LAB
kdlab.com&FFFFFF


&FFFFBBDedicated to the memory of
Andrey "KranK" Kuzmin
1971 - 2022
)"; //END CREDITS_HEADER_ENG
#endif

constexpr const char* CREDITS_PERIMETER_TITLE_RUSSIAN = "ПЕРИМЕТР";
constexpr const char* CREDITS_PERIMETER_RUSSIAN = R"(
&CCCCFFПродюсер&FFFFFF
Игровая концепция и вселенная,
сценарий

Юлия "Ylitka" Новикова (Шапошникова)
&CCCCFFГлавный гейм-дизайнер&FFFFFF
Дизайн миссий, AI

Евгений "GEorGE" Новиков
&CCCCFFРуководитель проекта&FFFFFF
Планирование, гейм-дизайн, инструменты

Александр "Sicher" Котляр
&CCCCFFГлавный программист&FFFFFF
Проектирование, логическое ядро, AI

Влад Спицкий
&CCCCFFГлавный художник&FFFFFF
2D/3D арт, моделинг, анимация,
спецэффекты, общий визуальный стиль

Дмитрий "Balmer" Поскряков
&CCCCFFПрограммист-технолог&FFFFFF
3D engine, логика, оптимизация,
инструменты

Александр "Скморр" Поречнов
&CCCCFFПрограммист&FFFFFF
Интерфейсы, инструменты, технологии

Евгений "$teeler" Худенко
&CCCCFFКонтент-программист&FFFFFF
Логика

Алексей "Ivn" Иванов
&CCCCFFПрограммист-технолог&FFFFFF
Редактор ландшафтов, геопроцессы,
сетевой код

Геннадий Тепленко
&CCCCFFКонцепт-арт,
3D-моделинг, текстурирование&FFFFFF

Олег "White" Беляев
&CCCCFFДизайнер миров&FFFFFF
Текстурирование объектов

Анна "Kemushi" Наместник
&CCCCFFДизайнер миров&FFFFFF

Илья Швецов
&CCCCFFПрограммист&FFFFFF
Инструменты

Марина "catfish" Гусева
&CCCCFFQ&A менеджер, PR менеджер&FFFFFF

Михаил "ChSnark" Пискунов
&CCCCFFЛитературное наполнение, сценарий&FFFFFF

Вадим Чалый
&CCCCFFЗвукорежиссер&FFFFFF

&9966FFМузыкальные композиции&FFFFFF
Виктор "Ruber" Краснокутский
Максим Сергеев

&9966FFСпециальные благодарности&FFFFFF
Андрей "Shodan" Аксенов
&CCCCFFКонсультации по
оптимизации 3D engine&FFFFFF

Роман "Guro" Гунявый
&CCCCFFКонцепт-арт&FFFFFF

Юрий Некрасов
&CCCCFFРассказы&FFFFFF

&9966FFДополнительное программирование&FFFFFF
Андрей Суханов
Михаил Жинко
Игорь Василенко

&9966FFГолоса:&FFFFFF
Виктор Воеводин
Павел Прибыток
Елена Акланова


&EF7C79Копирайты&FFFFFF

Программный продукт для PC
"Периметр" разработан
компанией "К-Д ЛАБ".
Все права защищены. (C) 2024

Все права на вселенную "Периметра"
принадлежат компании "К-Д ЛАБ".

Все права на музыку,
используемую в игре,
принадлежат компании "К-Д ЛАБ".

Ogg Vorbis (tm) Xiph.Org
Copyright (C) 1994-2024 Xiph.Org

)"; //END CREDITS_PERIMETER_RUSSIAN

constexpr const char* CREDITS_PERIMETER_ET_TITLE_RUSSIAN = "ПЕРИМЕТР: Завет Императора";
constexpr const char* CREDITS_PERIMETER_ET_RUSSIAN = R"(
&CCCCFFКонцепт-дизайнер&FFFFFF
Игровая концепция и вселенная

Юлия "Ylitka" Новикова (Шапошникова)
&CCCCFFИсполнительный продюсер &FFFFFF
Гейм-дизайн, дизайн миссий, AI

Евгений "GEorGE" Новиков
&CCCCFFРуководитель проекта &FFFFFF
Планирование, инструменты

Александр "Sicher" Котляр
&CCCCFFГлавный программист&FFFFFF
Проектирование, логическое ядро, AI

Влад Спицкий
&CCCCFFГлавный художник&FFFFFF
2D/3D арт, спецэффекты, 
общий визуальный стиль

Олег "White" Беляев
&CCCCFFПродюсер, Дизайнер миров &FFFFFF
Управление контентом, 2D арт, миры

Михаил "ChSnark" Пискунов
&CCCCFFГлавный сценарист&FFFFFF
Сюжет, редактирование

Дмитрий "Balmer" Поскряков
&CCCCFFПрограммист-технолог&FFFFFF
Технология, логика, 
оптимизация, спецэффекты

Евгений "$teeler" Худенко
&CCCCFFКонтент-программист &FFFFFF
Логика, звук

Алексей "Ivn" Иванов
&CCCCFFПрограммист-технолог&FFFFFF
Технология, редактор ландшафтов, сеть

Елена Ермакова
&CCCCFFМенеджер контента&FFFFFF
Сборка миссий

Елена "Penguin" Худенко
&CCCCFFPR-менеджер&FFFFFF

Геннадий Тепленко
&CCCCFF3D художник&FFFFFF

Анна "Kemushi" Наместник
&CCCCFFДизайнер миров &FFFFFF
Текстурирование

Николай "Nitewish" Шлюев
&CCCCFFQA-менеджер&FFFFFF

Павел "LordTyred" Костин
&CCCCFFГолоса и звукорежиссура&FFFFFF

Вадим Чалый
&CCCCFFЗвукорежиссер&FFFFFF

&CCCCFFМузыка &FFFFFF
Андрей Комаров aka Vishudha Kali
Андрей Гладков
Виктор "Ruber" Краснокутский
Максим Сергеев

&CCCCFFДиалоги&FFFFFF
Юрий Некрасов

&9966FFДополнительное программирование&FFFFFF
Александр "Скморр" Поречнов
Илья Швецов
Андрей Суханов
Михаил Жинко
Игорь Василенко

&9966FFДополнительное тестирование, баланс&FFFFFF
Клан [RUL]:
Антон "ToR the Artist" Страхов
Алексей "Don Reba" Бадалов
Михаил "Gremez" Бородин
Александр "@!!ex" Басов
Вадим "Zamorochenniy" Загидуллин
Андрей "Drunya" Большаков
Евгений "Johnbook" Книга

&9966FFСпециальные благодарности &FFFFFF
Андрей "Shodan" Аксенов
&CCCCFFКонсультации по оптимизации 3D engine &FFFFFF

Роман "Guro" Гунявый
&CCCCFFКонцепт-арт&FFFFFF


&EF7C79Копирайты&FFFFFF

Программный продукт для PC
"Периметр: Завет Императора"
разработан компанией "К-Д ЛАБ".
Все права защищены. (C) 2024

Все права на вселенную
"Периметр: Завет Императора"
принадлежат компании "К-Д ЛАБ".

Все права на музыку,
используемую в игре,
принадлежат компании "К-Д ЛАБ".

Ogg Vorbis (tm) Xiph.Org
Copyright (C) 1994-2024 Xiph.Org

)"; //END CREDITS_PERIMETER_ET_RUSSIAN

const char* CREDITS_COMMON_RUSSIAN = R"(
&EF7C79Видеоролики&FFFFFF

&9966FFTriDigitalStudio&FFFFFF
Михаил Шагин
Алексей Томин
Андрей Ковызин
Андрей Медведев
Виталий Шубин
Валентин Пеньков
Наталья Мазевич
Виктор Очиров
Андрей Литке
Антон Гридчин
Константин Скачинский
Александр Коршунов
Дмитрий Кухарь
Константин Саранчин
Вячеслав Мошкин

&9966FFArtisan3D&FFFFFF
Дмитрий Морозов
Алексей Москаленко
Анна Архипова
Александр Попов


&EF7C79Работа над "Legate Edition" переизданием&FFFFFF

&9966FFСохранение и предоставление исходного кода&FFFFFF
Александр "Аид" Дормидонтов
Александр "Sicher" Котляр
Алексей "Don Reba" Бадалов

&9966FFПодготовка и релиз Open Source версии&FFFFFF
Дмитрий "DileSoft" Лейкин,
Александр "caiiiycuk" Гурьянов

&9966FFКоммьюнити-менеджмент&FFFFFF
Дмитрий "DiTolk" Толкунов
Дмитрий "DileSoft" Лейкин

Ion "Cat" Agorria
&CCCCFFПрограммист&FFFFFF
Развитие игры, модернизация кода,
портирование под Linux и MacOS

Иван "DeltaDesignRus" Петров
&CCCCFFCG generalist&FFFFFF
Развитие игры, HD контент,
контент-археология, соцсети, WiKi

Дмитрий "nrader" Уваров
Тестирование, правки баланса, сборка игры

Akakiy
Тестирование, идеи, знаток лора, организация

Pete in his Jeep
Тестирование,
создание мода "True English Compaign"

Александр "@!!ex" Басов
Широкоэкранный патч
Инструменты сообщества
Реверс-инжиниринг игровых форматов данных

&9966FFАссоциация KD-Lab&FFFFFF
Менеджмент, маркетинг
Юлия "Ylitka" Новикова
Евгений "George" Новиков
Марина Гусева

&9966FFТестирование&FFFFFF
Василий Новиков
EFLFE
August
ViHtor
shikoist

&9966FFСторонних библиотек&FFFFFF
SDL2
SDL2_image
SDL2_net
SDL2_mixer
zlib
FFmpeg
Boost
DXVK Native
sokol_gfx
pevents
simpleini


&99EE66И ОГРОМНОЕ СПАСИБО
ЗА ВАШУ ПОДДЕРЖКУ!

Мы будем рады увидеть отзывы
по адресу&FFFFFF
kdlab.com
kdlab.com/community
github.com/KD-lab-Open-Source

)"; //END CREDITS_COMMON_RUSSIAN

constexpr const char* CREDITS_PERIMETER_TITLE_ENGLISH = "PERIMETER";
constexpr const char* CREDITS_PERIMETER_ENGLISH = R"(
&CCCCFFProducer&FFFFFF
Game Concept, Game Universe,
Scenarios

Yulia "Ylitka" Novikova (Shaposhnikova)
&CCCCFFLead Game Designer&FFFFFF
Missions, AI

Evgeny "GEorGE" Novikov
&CCCCFFProject Manager&FFFFFF
Game design, Tools, Balancing

Alexander "Sicher" Kotliar
&CCCCFFLead Programmer&FFFFFF
Architecture, Logic, AI

Vlad Spizky 
&CCCCFFLead Artist&FFFFFF 
Graphic Design, Art,
Models, Special Effects

Dmitry "Balmer" Poskryakov 
&CCCCFFProgrammer&FFFFFF 
Technology, Logic,
Optimization, Special Effects

Alexander "Scmorr" Porechnov
&CCCCFFProgrammer&FFFFFF
Interface, Tools, Technology

Eugene "Steeler" Hudenko
&CCCCFFProgrammer&FFFFFF
Logic, Sound

Alexey "Ivn" Ivanov
&CCCCFFProgrammer&FFFFFF
Technology, Landscape Editor,
Network

Gennady Teplenko
&CCCCFF3D Artist&FFFFFF
Models, Art, Textures

Oleg "White" Belyaev 
&CCCCFF2D Artist, World Designer&FFFFFF
Art, Maps, Textures

Anna "Kemushi" Namestnik
&CCCCFFWorld Designer&FFFFFF

Ilya Shvetsov
&CCCCFFProgrammer&FFFFFF
Tools

Marina "catfish" Guseva
&CCCCFFQA Manager&FFFFFF

Michail "ChSnark" Piskounov
&CCCCFFLiterary Content, Scenarios&FFFFFF

Vadim Chaly
&CCCCFFSound Designer&FFFFFF

&9966FFMusic compositions&FFFFFF
Victor "Ruber" Krasnokutsky
Maxim Sergeev

&9966FFSpecial Thanks&FFFFFF
Andrey "Shodan" Aksenov
&CCCCFFConsultation on
3D engine optimization&FFFFFF

Roman "Guro" Gunyavyi
&CCCCFFConcept Art&FFFFFF

Yuri Nekrasov
&CCCCFFStories&FFFFFF

&9966FFAdditional programming&FFFFFF
Andrey Sukhanov
Mikhail Zhinko
Igor Vasilenko


&EF7C79Copyrights&FFFFFF

The software product for PC
"Perimeter" was developed by
the "K-D LAB" company.
All rights reserved. (C) 2024

All rights to the "Perimeter" universe
belong to the "K-D LAB" company.

All rights to the music used in the
game belong to the "K-D LAB" company.

Ogg Vorbis (tm) Xiph.Org.
Copyright (C) 1994-2024 Xiph.Org.

)"; //END CREDITS_PERIMETER_ENGLISH

constexpr const char* CREDITS_PERIMETER_ET_TITLE_ENGLISH = "PERIMETER: Emperor's Testament";
constexpr const char* CREDITS_PERIMETER_ET_ENGLISH = R"(
&CCCCFFConcept-designer&FFFFFF
Game concept, Game universe

Yulia "Ylitka" Novikova (Shaposhnikova)
&CCCCFFExecutive Producer&FFFFFF
Game Design, Missions, AI

Evgeny "GEorGE" Novikov
&CCCCFFProject Manager&FFFFFF
Management, Tools

Alexander "Sicher" Kotliar
&CCCCFFLead Programmer&FFFFFF
Architecture, Logic, AI

Vlad Spizky 
&CCCCFFLead Artist&FFFFFF 
Graphic Design, 2D\\3D Art,
Special Effects

Oleg "White" Belyaev 
&CCCCFFProducer, World Designer&FFFFFF
Content Management, 2D Art, Maps

Michail "ChSnark" Piskounov
&CCCCFFLead Scenarist&FFFFFF
Story, Editing

Dmitry "Balmer" Poskryakov 
&CCCCFFProgrammer&FFFFFF 
Technology, Logic,
Optimization, Special Effects

Eugene "Steeler" Hudenko
&CCCCFFContent Programmer&FFFFFF
Logic, Sound

Alexey "Ivn" Ivanov
&CCCCFFProgrammer&FFFFFF
Technology, Landscape Editor,
Network

Elena Ermakova
&CCCCFFContent Manager&FFFFFF
Missions Scripting

Elena "Penguin" Hudenko
&CCCCFFPR Manager&FFFFFF

Gennady Teplenko
&CCCCFF3D Artist&FFFFFF

Anna "Kemushi" Namestnik
&CCCCFFWorld Designer&FFFFFF
Maps, Textures

Nikolay "NiteWish" Shluev
&CCCCFFQA Manager&FFFFFF

Pavel "LordTyred" Kostin
&CCCCFFScoring, Sound Designer&FFFFFF

&9966FFMusic&FFFFFF
Andrey Komarov aka Vishudha Kali 
Andrey Gladkov
Victor "Ruber" Krasnokutsky
Maxim Sergeev

&9966FFDialogues&FFFFFF
Yuri Nekrasov

&9966FFAdditional Programming&FFFFFF
Andrey Sukhanov
Igor Vassilenko
Michail Zhinko
Ilya Shvetsov
Alexander Porechnov

&9966FFAdditional Testing, Balance&FFFFFF
Anton "ToR" Strakhov
Alexey "Don Reba" Badalov
Michail "Gremez" Borodin
Alexander "@!!ex" Basov
Vadim "Zamorochenniy" Zagidullin
Andrey "Drunya" Bolshakov
Evgeny "Johnbook" Kniga 

&9966FFAdditional Optimization&FFFFFF
Andrey "shodan" Aksyonov

&9966FFAdditional Graphic Design&FFFFFF
Roman "Guro" Gunyavy

&EF7C79Copyrights&FFFFFF

The software product for PC
"Perimeter: Emperor's Testament"
was developed by the "K-D LAB" company.
All rights reserved. (C) 2024

All rights to the 
"Perimeter: Emperor's Testament"
universe belong to the "K-D LAB" company.

All rights to the music used in the
game belong to the "K-D LAB" company.

Ogg Vorbis (tm) Xiph.Org.
Copyright (C) 1994-2024 Xiph.Org.

)"; //END CREDITS_PERIMETER_ET_ENGLISH

const char* CREDITS_COMMON_ENGLISH = R"(
&EF7C79Videos&FFFFFF

&9966FFTriDigitalStudio&FFFFFF
Shagin Mikhail
Tomin Alexey
Kovyzin Andrey
Medvedev Andrey
Shubin Vitaly
Penkov Valentin
Mazevich Natalia
Ochirov Viktor
Litke Andrey
Gridchine Anton
Skachinsky Konstanin
Korshunov Alexander
Kuhary Dmitry
Saranchin Konstantin
Moshkin Vyacheslav

&9966FFArtisan3D&FFFFFF
Morozov Dmitry
Moskalenko Alexey
Arkhipova Anna
Popov Alexander


&EF7C79Work on the "Legate Edition" re-release&FFFFFF

&9966FFPreserving and providing source code&FFFFFF
Alexander "Hades" Dormidontov
Alexander "Sicher" Kotlyar
Alexey "Don Reba" Badalov

&9966FFPreparation and release of Open Source version&FFFFFF
Dmitry "DileSoft" Leikin,
Alexander "caiiiycuk" Guryanov

&9966FFCommunity Management&FFFFFF
Dmitry "DiTolk" Tolkunov
Dmitry "DileSoft" Leikin

Ion "Cat" Agorria
&CCCCFFProgrammer&FFFFFF
Game development, code modernization,
porting to Linux and macOS

Ivan "DeltaDesignRus" Petrov
&CCCCFFCG generalist&FFFFFF
Game development, HD content,
content archeology, social networks, WiKi

Dmitry "nrader" Uvarov
Testing, balance changes, game assembly

Akakiy
Testing, ideas, lore expert, organization

Pete in his Jeep
Testing,
"True English Campaign" mod creator

Alexander "@!!ex" Basov
Widescreen patch
Community tools
Reverse engineering of game file formats

&9966FFKD-Lab Association&FFFFFF
Management, marketing
Yulia "Ylitka" Novikova
Evgeny "George" Novikov
Marina Guseva

&9966FFTesting&FFFFFF
Vasily Novikov
EFLFE
August
ViHtor
shikoist

&9966FFThird-party libraries&FFFFFF
SDL2
SDL2_image
SDL2_net
SDL2_mixer
zlib
FFmpeg
Boost
DXVK Native
sokol_gfx
pevents
simpleini

&99EE66AND THANK YOU SO MUCH
FOR YOUR SUPPORT!

We'd love to hear your feedback at&FFFFFF
kdlab.com
kdlab.com/community
github.com/KD-lab-Open-Source

)"; //END CREDITS_COMMON_ENGLISH

void qdTextDB::load_replacement_texts(const std::string& locale) {
    //Used for all langs
    load_lines({
       "Interface.Menu.ButtonLabels.<<<=<<<",
       "Interface.Menu.ButtonLabels.>>>=>>>",
       //Empty to not mess with ,'s
       ""
    }, true, locale);
    
    //Credits
    std::string credits = "Interface.Credits=";
    bool russian = startsWith(locale, "russian");
    if (russian) {
        if (terGameContentSelect == PERIMETER_ET) {
            credits += CREDITS_PERIMETER_ET_TITLE_RUSSIAN;
            credits += CREDITS_HEADER_RUS;
            credits += CREDITS_PERIMETER_ET_RUSSIAN;
        } else {
            credits += CREDITS_PERIMETER_TITLE_RUSSIAN;
            credits += CREDITS_HEADER_RUS;
            credits += CREDITS_PERIMETER_RUSSIAN;
        }
    } else {
        if (terGameContentSelect == PERIMETER_ET) {
            credits += CREDITS_PERIMETER_ET_TITLE_ENGLISH;
            credits += CREDITS_HEADER_ENG;
            credits += CREDITS_PERIMETER_ET_ENGLISH;
        } else {
            credits += CREDITS_PERIMETER_TITLE_ENGLISH;
            credits += CREDITS_HEADER_ENG;
            credits += CREDITS_PERIMETER_ENGLISH;
        }

    }
    credits += russian ? CREDITS_COMMON_RUSSIAN : CREDITS_COMMON_ENGLISH;
    load_lines({credits}, true, locale);
}

void qdTextDB::load_supplementary_texts(const std::string& locale) {
    //Load per language texts
    if (startsWith(locale, "russian")) {
        load_lines({
#include "Texts_ru.h"
           //Empty to not mess with ,'s
           ""
       }, false, locale);
    } else {
        //Load english for rest until they are translated
        load_lines({
#include "Texts_en.h"
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
