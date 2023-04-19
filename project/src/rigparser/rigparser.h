#ifndef RIGPARSER_H
#define RIGPARSER_H

#include "../rig/rig.h"
#include <fstream>

//=================================================================================================
// Признаки операций РИГ
enum Opc {interp, dlist, plist, alist,ret};

//=================================================================================================
// Класс, определяющий парсер, осуществляющий разбор текстового представления РИГ.
class RigParser {
    void InitValues();  // инициализация всяких внутренних данных

    class EndOfProg {};
    Value *lastValue;   // Последнее полученное постоянное
    Rig* pRig;          // Указатель на подключаемый экземпляр РИГ
    QString inFile;     // Имя входного файла
    QStringList text;   // Список строк, содержащий текстовое представление РИГ
    QStringList::const_iterator it; // Итератор для обхода текста РИГ
    QString value;      // Значение накапливаемой конструкции (лексемы)
    int line;           // Номер текущей строки текста
    int column;         // Номер текущей позиции в строке
    QMap<QString,SpecType> elementaryValues;     // Специальные знаки
    bool flag;          // Флаг, определяющий, что это ожидаемый файл
    bool endOfProgFlag; // Флаг определяющий завершение текста РИГ
public:
    // Конструктор.
    RigParser(Rig* _pRig, QString _inFile);
    RigParser(Rig* _pRig, QString rigText, bool uselessFlag);
    ~RigParser();
    // Функция запуска парсера после всех начальных установок
    bool Start();

    //---------------------------------------------------------------------------------------------
    // Правила, описывающие синтаксис РИГ
    //---------------------------------------------------------------------------------------------

    // Реверсивный информационный граф
    bool IsRig();
    // Описания внешних ссылок и локальных констант
    bool Dcl();
    // Внешние ссылки
    bool IsExtRef();
    // Локальные константы
    bool LocalConst();
    // Граф (сами операции РИГ)
    bool Graph();
    // Заголовок графа
    bool Title();
    // Список вершин графа
    bool ActorList();
    // Начальная вершина графа
    bool IsBegActor(Actor** ppActor);
    // Вершина графа
    bool IsActor(Actor** ppActor);
    // Конечная вершина графа
    //bool EndActor();
    // Оператор
    bool Operator(Actor** ppActor);
    // Позиции
    bool IsPositions(int pos[]);
    // Ссылка на одну из областей памяти
    bool Ref(MemType& memType, int& addr);
    // Операция
    bool Operation(Opc& opc);
    // Значение
    bool IsValue();
    // Целое или действительное
    bool IntReal(Value** ppVal);
    // Логическая переменная
    bool Booler(BoolValue** boolVal);
    // Задержанное
    bool Delayed(DelayValue** ppVal);
    // Элементарное
    bool Elementary(SpecValue** ppVal);
    // Имя
    bool Name(QString& str);
    // Идентификатор
    bool Id(QString& str);
    // Положительное
    // Строка
    bool IsStr(DataListValue** ppVal);
    // Символ
    bool IsChar(CharValue** ppVal);
    // Встроенный тип ошибки
    bool IsError(ErrorValue** ppVal);
    bool Positive(QString& str);
    // Буква
    bool Letter(QChar& sym);
    // Цифра
    bool Digit(QChar& sym);
    // Пропуск
    bool Blank();
    // Конец строки
    bool EndOfLine();
    // Искомое слово
    bool Word(QString str);
    //
    bool IsConstDataList(DataListValue** dataListValue);
    bool IsConstParList(ParListValue** parListValue);
    //---------------------------------------------------------------------------------------------
    // Вспомогательные методы и утилиты
    //---------------------------------------------------------------------------------------------

    // Вывод информации об ошибке в РИГ
    void Error(QString message = "Default error message");
    // Переход на следующую анализируемую строку
    void NextLine();
    // Загрузка текста РИГ
    bool Load(QString fileName);
    // Тестовый вывод загруженного текста
    void TestOut();

    //Символ-геттер
    QChar GetSymbol(int col);

    // Возвращение указателя на РИГ
    Rig* GetRig(){ return pRig; }
};

#endif // RIGPARSER_H
