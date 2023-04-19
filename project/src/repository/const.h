#ifndef CONST_H
#define CONST_H

#include <QString>

// типы ОС 
enum enmOS
{
    OS_ERR = 0,                                  // ошибка определения ОС
    OS_MAC,                                      // MacOS
    OS_NIX,                                      // Unix, Linux,..
    OS_WIN                                       // MS Windows
};

// типы СУБД
enum enmDB
{
    DB_ANY = -2,                                 // любая (для поиска)
    DB_OTHER,                                    // неизвестная (для обработки ошибок)
    DB_FOLDER,                                   // структура каталогов ОС
    //DB_XML                                     // XML-файл
    DB_FTP,                                      // FTP
    DB_MYSQL,                                    // MySQL
    DB_SQLITE,                                   // SQLite
    DB_DB2,                                      // IBM DB2
    DB_POSTGRESQL                                // PostgreSQL
};
const int FILE_INI = 1;

// вспомогательные константы QuickFile
const QString FILE_NOT_EXIST = "EMPTY_FILE";
const QString FILE_LOCAL_HOST = "localhost";
enum enmFileStatus
{
    FILE_NOT_INIT = 0,
    FILE_NOT_SUPPORT,
    FILE_INIT
};

// параметры SQL-таблиц
const QString SQL_TABLE = "MAIN_TABLE";          // название таблицы
const QString SQL_TABLE_NAME = "name";           // имя файла
const QString SQL_TABLE_LINK = "link";           // ссылка для скачивания
const QString SQL_TABLE_FILE = "file";           // файл

// тип записи (Note)
enum enmNameNote
{
    NAME_NOTE = 0,                               // краткое название "записи"
    NAME_FULL,                                   // полное название "записи"
    NAME_REP                                     // название репозитория
};

// названия параметров конф. файла (QuickConfig)
const QString PARAM_LINKS = "LINKS";

// тип файла (Note, NoteVersion)
enum FileTypes
{
    FILE_ABOUT = -1,                             // описание
    FILE_ORIG,                                   // исходный вид содержимого
    FILE_RIG,
    FILE_CG,
    FILE_RDOT,
    FILE_CDOT,
    FILE_SYMTAB
};

// тип списка задач (ListTask)
enum enmQueue
{
    LIST_QUEUE = 1,                              // очередь
    LIST_STACK                                   // стек
};

// кодировки:
/*
const char* CODEC_LIN_UTF8 = "UTF-8";
const char* CODEC_WIN_IBM866 = "IBM866";
const char* CODEC_WIN_1251 = "Windows-1251";
const char* CODEC_MAIN_IN = CODEC_LIN_UTF8;
const char* CODEC_MAIN_OUT = CODEC_LIN_UTF8;
*/

// тип сообщения:
enum enmMsg
{
    WRITE_MSG = 1,                               // диалоговое
    WRITE_MSG_SILENT,                            // диалоговое с галочкой "Больше не показывать"
    WRITE_PRINT                                  // фоновым текстом
};

// набор кнопок:
enum enmButton
{
    BUTTON_OK = 1,                               // ОК
    BUTTON_OK_CANCEL,                            // ОК + Отмена
    BUTTON_YES_NO                                // Да + Нет
};

#endif // CONST_H
