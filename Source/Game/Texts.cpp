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
           "GAME_CONTENT.PERIMETER=Периметр",
           "GAME_CONTENT.PERIMETER_ET=Периметр: Завет Императора",
           "Controls.Input.NoInputSet=Не назначено",
           "Controls.Input.MouseLeftButton=Левая кнопка мыши",
           "Controls.Input.MouseRightButton=Правая кнопка мыши",
           "Controls.Input.MouseMiddleButton=Средняя кнопка мыши",
           "Controls.Input.MouseButton4=Кнопка мыши 4",
           "Controls.Input.MouseButton5=Кнопка мыши 5",
           "Controls.CTRL_TIME_NORMAL=Обычная скорость",
           "Controls.CTRL_TIME_DEC=Уменьшить скорость",
           "Controls.CTRL_TIME_INC=Увеличить скорость",
           "Controls.CTRL_CAMERA_MOVE_UP=Переместить камеру вверх",
           "Controls.CTRL_CAMERA_MOVE_DOWN=Переместить камеру вниз",
           "Controls.CTRL_CAMERA_MOVE_LEFT=Переместить камеру влево",
           "Controls.CTRL_CAMERA_MOVE_RIGHT=Переместить камеру вправо",
           "Controls.CTRL_CAMERA_ROTATE_UP=Повернуть камеру вверх",
           "Controls.CTRL_CAMERA_ROTATE_DOWN=Повернуть камеру вниз",
           "Controls.CTRL_CAMERA_ROTATE_LEFT=Повернуть камеру влево",
           "Controls.CTRL_CAMERA_ROTATE_RIGHT=Повернуть камеру вправо",
           "Controls.CTRL_CAMERA_ZOOM_INC=Уменьшить масштаб камеры",
           "Controls.CTRL_CAMERA_ZOOM_DEC=Увеличить масштаб камеры",
           "Controls.CTRL_CAMERA_MOUSE_LOOK=Вращение камеры мышью",
           "Controls.CTRL_CAMERA_MOUSE_MOVE=Перемещение камеры мышью",
           "Controls.CTRL_ESCAPE=Выйти",
           "Controls.CTRL_CAMERA_SAVE1=Сохранить камеру 1",
           "Controls.CTRL_CAMERA_SAVE2=Сохранить камеру 2",
           "Controls.CTRL_CAMERA_SAVE3=Сохранить камеру 3",
           "Controls.CTRL_CAMERA_SAVE4=Сохранить камеру 4",
           "Controls.CTRL_CAMERA_SAVE5=Сохранить камеру 5",
           "Controls.CTRL_CAMERA_RESTORE1=Восстановить камеру 1",
           "Controls.CTRL_CAMERA_RESTORE2=Восстановить камеру 2",
           "Controls.CTRL_CAMERA_RESTORE3=Восстановить камеру 3",
           "Controls.CTRL_CAMERA_RESTORE4=Восстановить камеру 4",
           "Controls.CTRL_CAMERA_RESTORE5=Восстановить камеру 5",
           "Controls.CTRL_CAMERA_TO_EVENT=Камера на событие",
           "Controls.CTRL_LOAD=Загрузить игру",
           "Controls.CTRL_SAVE=Сохранить игру",
           "Controls.CTRL_HOLD_PRODUCTION=Приостановить производство",
           "Controls.CTRL_TOGGLE_LIFEBARS=Переключить индикаторы здоровья",
           "Controls.CTRL_RESUME_PRODUCTION=Возобновить производство",
           "Controls.CTRL_TOGGLE_FPS=Показать/Скрыть FPS",
           "Interface.Menu.Mods.MetadataOrigin=Источник:",
           "Interface.Menu.Mods.ModOrigin.game_mods=Папка с модификациями",
           "Interface.Menu.Mods.ModOrigin.steam=Мастерская Steam",
           "Interface.Menu.Mods.ErrorDisallowedFile=Модификация содержит файлы, расположенные по недопустимым путям",
           "Interface.Menu.Mods.MetadataVersion=Версия:",
           "Interface.Menu.Mods.MetadataAuthors=Авторы:",
           "Interface.Menu.Mods.MetadataDescription=Описание:",
           "Interface.Menu.Mods.MetadataSite=Сайт:",
           "Interface.Menu.Mods.MetadataLicense=Лицензия:",
           "Interface.Menu.Mods.Errors=Ошибки:",
           "Interface.Menu.Mods.ErrorMissingAttribute=Отсутствует атрибут в mod.ini:",
           "Interface.Menu.Mods.ErrorGameTooOld=Игра слишком старая, нужна версия:",
           "Interface.Menu.Mods.ErrorMissingModInfo=Отсутствует mod.ini",
           "Interface.Menu.Mods.ErrorDuplicateContent=Содержимое уже загружено",
           "Interface.Menu.Mods.ErrorRequiredContentMissing=Мод требует контента, который не установлен",
           "Interface.Menu.Mods.ErrorRequiredContentDisabled=Мод требует отключенного контента",
           "Interface.Menu.Mods.ErrorDisallowedContentEnabled=Мод несовместим с активным в данный момент контентом",
           "Interface.Menu.Messages.ReplayGameVersionDifferent=Этот повтор был сохранен в другой версии и может отображаться неправильно. Использованная версия игры:",
           "Interface.Menu.Messages.GameContentMissing=Содержит ресурсы, которые не представлены или не включены в этой копии игры, убедитесь, что они установлены и включены:\n",
           "Interface.Menu.Messages.GameContentSwitch=Содержит неактивные ресурсы. Переключите кампанию на следующую:",
           "Interface.Menu.Messages.WorldMissing=Содержит карту, которая не представлена или не включена в этой копии игры, убедитесь, что она установлена и включена:\n\n",
           "Interface.Menu.Messages.PressNewControlInput=Нажмите кнопку или сочетание для:",
           "Interface.Menu.ButtonLabels.MULTIPLAYER=МУЛЬТИПЛЕЕР",
           "Interface.Menu.ButtonLabels.MODS=МОДЫ",
           "Interface.Menu.ButtonLabels.COMMUNITY=КОМЬЮНИТИ",
           "Interface.Menu.ButtonLabels.K-D LAB=К-Д ЛАБ",
           "Interface.Menu.ButtonLabels.ASSOCIATION=АССОЦИАЦИЯ",
           "Interface.Menu.ButtonLabels.SOCIAL MEDIA=СОЦИАЛЬНЫЕ СЕТИ",
           "Interface.Menu.ButtonLabels.SOURCE CODE=ИСХОДНЫЙ КОД",
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
           "Interface.Menu.ButtonLabels.GUIDE=РУКОВОДСТВО",
           "Interface.Menu.ButtonLabels.VSYNC=ВЕРТИКАЛЬНАЯ СИНХРОНИЗАЦИЯ",
           "Interface.Menu.ButtonLabels.VOICE VOLUME=ГРОМКОСТЬ ГОЛОСА",
           "Interface.Menu.ButtonLabels.SPEECH VOLUME=ГРОМКОСТЬ ГОЛОСА БРИФИНГОВ И ЗАДАНИЙ",
           "Interface.Menu.ButtonLabels.CONTROLS=УПРАВЛЕНИЕ",
           "Interface.Menu.ButtonLabels.CHANGE=ИЗМЕНИТЬ",
           "Interface.Menu.ButtonLabels.RESTORE=ВОССТАНОВИТЬ",
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
           "Interface.Tips.Multiplayer.HasPassword=Имеет пароль",
           "Interface.Tips.Multiplayer.GameStarted=Игра уже началась",
           "Interface.Tips.Multiplayer.Game=Игра",
           "Interface.Tips.Multiplayer.Map=Карта",
           "Interface.Tips.Multiplayer.CurrentPlayers=Текущие игроки",
           "Interface.Tips.Multiplayer.Ping=Пинг",
           "Interface.Menu.Multiplayer.NoRelayAvailable=Не удаётся получить список открытых игр.\nВозможно, проблемы с сетью или сервером",
           "Interface.Menu.Multiplayer.NoGameRunning=На данный момент нет доступных игр для\nприсоединения. Вы можете создать новую\nигру, чтобы играть с другими игроками",
           "Interface.Tips.PopupUnitDerived=\n&00FFFFПравая кнопка мыши&FFFFFF заказ базовых роботов\nдля одной единицы, с &00FFFFShift&FFFFFF для пяти единиц.",
           "Interface.Menu.Messages.WrongIPPort=Этот IP-адрес недоступен",
           "Interface.Menu.Messages.Multiplayer.IncorrectContent=Сервер содержит другие игровые ресурсы",
           "Interface.Menu.Messages.Multiplayer.IncorrectArch=Сервер имеет другую битность или архитектуру ЦПУ, другой тип билда (Debug/Release), операционную систему или использован другой компилятор (MSVC/Clang/GCC), пожалуйста, убедитесь, что они совпадают",
           "Interface.Menu.Messages.Multiplayer.SignatureError=Проверка подписи или CRC не прошла, соединение может быть ненадёжным",
           "Interface.Menu.Messages.Multiplayer.HostTerminated=Хост прекратил игру или отключился",
           "Interface.Menu.Messages.Multiplayer.StartingGame=Игра начинается...",
           "Interface.Menu.Messages.Multiplayer.ChatTipClan=Нажмите &00FFFFInsert&FFFFFF чтобы написать игрокам из своего клана, &00FFFFControl + Insert&FFFFFF или &00FFFFControl + Space&FFFFFF чтобы написать всем игрокам.",
           "Interface.Menu.Messages.Multiplayer.ChatTip=Нажмите &00FFFFInsert&FFFFFF, &00FFFFControl + Insert&FFFFFF или &00FFFFControl + Space&FFFFFF чтобы написать всем игрокам.",
           "Interface.Menu.Messages.Multiplayer.Kicked=Вас кикнули.",
           "Interface.Menu.Messages.Confirmations.PendingChanges=Отложенные изменения не будут применены, вернуться в главное меню?",
           "Interface.Menu.Messages.Confirmations.ApplyChangesRestart=Чтобы применить изменения, требуется перезапуск игры, вы уверены?",
           "Interface.Menu.Messages.Confirmations.OpenWeb=Открыть сайт?",
           "Integrations.RichPresence.Activity.Menu=Меню",
           "Integrations.RichPresence.Activity.Campaign=Кампания",
           "Integrations.RichPresence.Location.Battle=Сражения",
           "Integrations.RichPresence.Activity.MultiplayerList=Поиск серверов для мультиплеера",
           "Integrations.RichPresence.Activity.MultiplayerWaiting=Ожидание начала многопользовательской сессии",
           "Integrations.RichPresence.Activity.MultiplayerPlaying=Играет в многопользовательском режиме",
           //Empty to not mess with ,'s
           ""
       }, false, locale);
    } else {
        //Load english for rest until they are translated
        load_lines({
           "GAME_CONTENT.PERIMETER=Perimeter",
           "GAME_CONTENT.PERIMETER_ET=Perimeter: Emperor's Testament",
           "Controls.Input.NoInputSet=No input set",
           "Controls.Input.MouseLeftButton=Left mouse button",
           "Controls.Input.MouseRightButton=Right mouse button",
           "Controls.Input.MouseMiddleButton=Middle mouse button",
           "Controls.Input.MouseButton4=Mouse button 4",
           "Controls.Input.MouseButton5=Mouse button 5",
           "Controls.CTRL_TIME_NORMAL=Normal speed",
           "Controls.CTRL_TIME_DEC=Decrease speed",
           "Controls.CTRL_TIME_INC=Increase speed",
           "Controls.CTRL_CAMERA_MOVE_UP=Move camera up",
           "Controls.CTRL_CAMERA_MOVE_DOWN=Move camera down",
           "Controls.CTRL_CAMERA_MOVE_LEFT=Move camera left",
           "Controls.CTRL_CAMERA_MOVE_RIGHT=Move camera right",
           "Controls.CTRL_CAMERA_ROTATE_UP=Rotate camera up",
           "Controls.CTRL_CAMERA_ROTATE_DOWN=Rotate camera down",
           "Controls.CTRL_CAMERA_ROTATE_LEFT=Rotate camera left",
           "Controls.CTRL_CAMERA_ROTATE_RIGHT=Rotate camera right",
           "Controls.CTRL_CAMERA_ZOOM_INC=Decrease camera zoom",
           "Controls.CTRL_CAMERA_ZOOM_DEC=Increase camera zoom",
           "Controls.CTRL_CAMERA_MOUSE_LOOK=Mouse camera rotation",
           "Controls.CTRL_CAMERA_MOUSE_MOVE=Mouse camera moving",
           "Controls.CTRL_ESCAPE=Quit",
           "Controls.CTRL_CAMERA_SAVE1=Save camera 1",
           "Controls.CTRL_CAMERA_SAVE2=Save camera 2",
           "Controls.CTRL_CAMERA_SAVE3=Save camera 3",
           "Controls.CTRL_CAMERA_SAVE4=Save camera 4",
           "Controls.CTRL_CAMERA_SAVE5=Save camera 5",
           "Controls.CTRL_CAMERA_RESTORE1=Restore camera 1",
           "Controls.CTRL_CAMERA_RESTORE2=Restore camera 2",
           "Controls.CTRL_CAMERA_RESTORE3=Restore camera 3",
           "Controls.CTRL_CAMERA_RESTORE4=Restore camera 4",
           "Controls.CTRL_CAMERA_RESTORE5=Restore camera 5",
           "Controls.CTRL_CAMERA_TO_EVENT=Camera to event",
           "Controls.CTRL_LOAD=Load game",
           "Controls.CTRL_SAVE=Save game",
           "Controls.CTRL_HOLD_PRODUCTION=Hold production",
           "Controls.CTRL_TOGGLE_LIFEBARS=Toggle healthbars",
           "Controls.CTRL_RESUME_PRODUCTION=Resume production",
           "Controls.CTRL_TOGGLE_FPS=Show/Hide FPS",
           "Interface.Menu.Mods.MetadataVersion=Version:",
           "Interface.Menu.Mods.MetadataAuthors=Authors:",
           "Interface.Menu.Mods.MetadataDescription=Description:",
           "Interface.Menu.Mods.MetadataSite=Site:",
           "Interface.Menu.Mods.MetadataLicense=License:",
           "Interface.Menu.Mods.MetadataOrigin=Origin:",
           "Interface.Menu.Mods.ModOrigin.game_mods=Mods directory in game",
           "Interface.Menu.Mods.ModOrigin.steam=Steam Workshop",
           "Interface.Menu.Mods.Errors=Errors:",
           "Interface.Menu.Mods.ErrorMissingAttribute=Missing attribute in mod.ini:",
           "Interface.Menu.Mods.ErrorGameTooOld=Game is too old, needs version:",
           "Interface.Menu.Mods.ErrorMissingModInfo=Missing mod.ini",
           "Interface.Menu.Mods.ErrorDuplicateContent=Content is already loaded",
           "Interface.Menu.Mods.ErrorRequiredContentMissing=Mod requires content that is not installed",
           "Interface.Menu.Mods.ErrorRequiredContentDisabled=Mod requires content that is disabled",
           "Interface.Menu.Mods.ErrorDisallowedContentEnabled=Mod is not compatible with currently enabled content",
           "Interface.Menu.Mods.ErrorDisallowedFile=Mod contains files that are placed in disallowed locations",
           "Interface.Menu.Messages.ReplayGameVersionDifferent=This replay was saved with a different version and may not display correctly, used game version:",
           "Interface.Menu.Messages.GameContentMissing=Contains game content that is not present or enabled in your installation, make sure these are installed and enabled in your game:\n",
           "Interface.Menu.Messages.GameContentSwitch=Contains game content that is not selected, please change the campaign to the following content:\n",
           "Interface.Menu.Messages.WorldMissing=This contains a map/world that is not present or enabled in your installation, make sure that is installed and enabled in your game:\n\n",
           "Interface.Menu.Messages.PressNewControlInput=Press new input binding for:",
           "Interface.Menu.ButtonLabels.MULTIPLAYER=MULTIPLAYER",
           "Interface.Menu.ButtonLabels.MODS=MODS",
           "Interface.Menu.ButtonLabels.COMMUNITY=COMMUNITY",
           "Interface.Menu.ButtonLabels.K-D LAB=K-D LAB",
           "Interface.Menu.ButtonLabels.ASSOCIATION=ASSOCIATION",
           "Interface.Menu.ButtonLabels.SOCIAL MEDIA=SOCIAL MEDIA",
           "Interface.Menu.ButtonLabels.SOURCE CODE=SOURCE CODE",
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
           "Interface.Menu.ButtonLabels.GUIDE=GUIDE",
           "Interface.Menu.ButtonLabels.VSYNC=VERTICAL SYNC",
           "Interface.Menu.ButtonLabels.VOICE VOLUME=VOICE VOLUME",
           "Interface.Menu.ButtonLabels.SPEECH VOLUME=VOICE VOLUME OF BRIEFINGS AND TASKS",
           "Interface.Menu.ButtonLabels.CONTROLS=CONTROLS",
           "Interface.Menu.ButtonLabels.CHANGE=CHANGE",
           "Interface.Menu.ButtonLabels.RESTORE=RESTORE",
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
           "Interface.Menu.Multiplayer.NoRelayAvailable=Public game listing couldn't be fetched.\nPossible network or server issues.",
           "Interface.Menu.Multiplayer.NoGameRunning=There is no game to join currently.\nYou can create one to play with others.",
           "Interface.Tips.Multiplayer.HasPassword=Has password",
           "Interface.Tips.Multiplayer.GameStarted=Game has already started",
           "Interface.Tips.Multiplayer.Game=Game",
           "Interface.Tips.Multiplayer.Map=Map",
           "Interface.Tips.Multiplayer.CurrentPlayers=Current players",
           "Interface.Tips.Multiplayer.Ping=Ping",
           "Interface.Tips.PopupUnitDerived=\n&00FFFFRight mouse button&FFFFFF to order basic robots\nfor one unit, with &00FFFFShift&FFFFFF for five units.",
           "Interface.Menu.Messages.WrongIPPort=IP port is wrong",
           "Interface.Menu.Messages.Multiplayer.IncorrectContent=Server has different game content",
           "Interface.Menu.Messages.Multiplayer.IncorrectArch=Server has different bits or CPU architecture, different build type (Debug/Release), Operating System or used a different compiler (MSVC/Clang/GCC), please ensure they match",
           "Interface.Menu.Messages.Multiplayer.SignatureError=Signature or CRC checks failed, connection may be unreliable",
           "Interface.Menu.Messages.Multiplayer.HostTerminated=Host has terminated session or disconnected",
           "Interface.Menu.Messages.Multiplayer.StartingGame=Starting the game...",
           "Interface.Menu.Messages.Multiplayer.ChatTipClan=Press &00FFFFInsert&FFFFFF to chat with your clan players, &00FFFFControl + Insert&FFFFFF or &00FFFFControl + Space&FFFFFF to chat with all players.",
           "Interface.Menu.Messages.Multiplayer.ChatTip=Press &00FFFFInsert&FFFFFF, &00FFFFControl + Insert&FFFFFF or &00FFFFControl + Space&FFFFFF to chat with all players.",
           "Interface.Menu.Messages.Multiplayer.Kicked=You got kicked.",
           "Interface.Menu.Messages.Confirmations.PendingChanges=Pending changes will not be applied, return to main menu?",
           "Interface.Menu.Messages.Confirmations.ApplyChangesRestart=Game restart is required to apply changes, are you sure?",
           "Interface.Menu.Messages.Confirmations.OpenWeb=Open the website?",
           "Integrations.RichPresence.Activity.Menu=Menu",
           "Integrations.RichPresence.Activity.Campaign=Campaign",
           "Integrations.RichPresence.Activity.Battle=Battle",
           "Integrations.RichPresence.Activity.MultiplayerList=Searching for multiplayer servers",
           "Integrations.RichPresence.Activity.MultiplayerWaiting=Waiting for multiplayer session to start",
           "Integrations.RichPresence.Activity.MultiplayerPlaying=Playing in multiplayer mode",
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
