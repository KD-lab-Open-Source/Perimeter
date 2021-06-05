# Периметр 

Чат для обсуждения: https://t.me/PerimeterGame

(с) ООО "КД ВИЖЕН" (Калининград)

[English](README.eng.md)

Весь код, за исключением сторонних библиотек, публикуется под лицензией GPLv3. Код сторонних библиотек (где указана иная лицензия) публикуется под лицензией этих библиотек.

## Состав репозитория

* Source/ - проверенная ветка, которая работает с ресурсами из Steam.
* XLibs.Net/ - header-only библиотеки используемые игрой, реально используется:
  * boost/
  * stl/ - самописная обертка над стандартной библиотекой
  * VC7.1 - SDK от микрософта (нужны только либы)
  * VC8 - SDK от микрософта (нужны только либы)
  * XPrm.exe - утилита генерации, нужна если вы хотите собирать Scripts
* MSDXSDK_02_06 - DirectX SDK используемый игрой

## Что потребуется

Проверено что игра собирается в окружении Windows XP / Windows 7 + Visual Studio 2005. Кроме того потребуется
установить Windows SDK 7.1 для воспроизведения видео, скачать можно по ссылке:

https://developer.microsoft.com/ru-ru/windows/downloads/sdk-archive/

В самом низу есть версия для Windows XP и Windows 7.

## Сборка

**Все нижеследующее уже сделано для папки Source (возможно, потребуется скорректировать пути и установить нужные SDK). В Visual Studio 2005 достаточно просто выбрать проект Game и Rebuild.**

Игра собирается без особых проблем, главное правильно настроить **Additinal Include Direcotires** и **Additional Link Directories**,
об этом ниже. Основная проблема была с кусочками кода написанными в стиле:

```C++
for (int i = ...) {
  //...
}

//дальше идет использование i
```

Но современные компиляторы так делать не разрешают. Исправить это легко, - достаточно вынести определение переменной за цикл.

## Пошаговая инструкция по сборке

Открываем в `VS2005` solution `Source/PerimetrAddon.sln`.

**ВАЖНО** Если вы сделали clean, to вам нужно восстановить файлы triggereditor.* из репозитория, потому что он не собирается!

Последовательность в которой нужно собриать подпроекты:
* Scripts
* PluginMax
* Terra
* Tx3D Library
* ~~TriggerEditor~~ - **есть ошибки, можно не собирать**
* Peer
* Render
* PerimeterSound
* Game

Выбираем подпроект в меню студии и выполняем `Project Only -> Build Only <Имя проекта>`, просто `Build` не будет работать так как есть ошибки в `TriggerEditor`,
сама игра это проект `Game` его выход это `Game.exe`.

![изображение](https://user-images.githubusercontent.com/1727152/114376550-e9dba480-9baf-11eb-822e-49e47d9853fe.png)

**Для Source зависимости прописаны и собираются в Win7. Возможно потребуется внести корректировки.**

Для **каждого проекта** нужно добавить следующие пути поиска `*.h` файлов (Project Properties | C++ | General | Additional Include Directories):
* **XLibs.Net\boost**
* **XLibs.Net\stl**
* **XLibs.Net**

**ВАЖНО** Помещать их в самый конец списка.
![изображение](https://user-images.githubusercontent.com/1727152/114377393-d0872800-9bb0-11eb-8991-1668535b9ded.png)

После этого выбранный проект должен собраться.

Для проекта **Render** помимо `XLibs.Net` нужно добавить:
* **Microsoft SDKs\Windows\v7.1\Include**
* **MSDXSDK_02_06\Include - Должен идти после v7.1**

Для проектов **PerimeterSound, **Game** помимо `XLibs.Net` нужно добавить:
* **Microsoft SDKs\Windows\v7.1\Include**

Все проекты собраны, кроме **Game** - он не линкуется, но компилируется. Что бы это исправить нужно добавить следующие пути поиска `*.lib` файлов (Project Properties | C++ | Linker | Additional Library Directories):
* **Microsoft SDKs\Windows\v7.1\Include**
* **MSDXSDK_02_06\Lib\x86**
* **XLibs.Net\VC7.1**
* **XLibs.Net\VC8**

## Запуск игры
Скопируйте файлы GameDBG.exe, PrmEdit.dll, ScriptEditor.dll, TriggerEditor.dll и Scripts\GameShellSq.prm в папку с игрой из Steam.
