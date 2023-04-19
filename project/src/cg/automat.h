#ifndef AUTOMAT_H
#define AUTOMAT_H

#include <QtCore>
//#include <QString>
//=================================================================================================
// Структура пакета данных для шага
class AutomatState
{
public:
    int state;
    int dataListLength;
    AutomatState();
    AutomatState(int newState);
};
enum autoResultCode { AR_NOTHING, AR_EXECUTE, AR_ASYNC_ADD};
class AutomatResult
{
public:
    AutomatState retState;
    autoResultCode resultCode;
    AutomatResult() {}
    AutomatResult(AutomatState newState, autoResultCode newresult) { retState=newState; resultCode=newresult; }
};
enum autoType {AT_ARG, AT_INTER, AT_DLIST, AT_PLIST, AT_ALIST, AT_RET};
//=================================================================================================
// Класс, определяющий автомат, используемый для управления сигналами в вершине УГ.
class Automat: public QObject {
public:
    Automat() {}

    // Получение имени автомата
    virtual QString GetName() = 0;

    // Тестовый вывод базовой части автомата
    virtual void TestOut(QTextStream& out);
    virtual QString GetAsStr();

    // Обработка сигнала
    virtual AutomatResult ProcessSignal(AutomatState curState, int input) = 0;
    // Возврат типа
    virtual autoType GetAutomatType() =0;
};

//=================================================================================================
// Класс, определяющий автомат, выполняющий функции операции интерпретации
class InterpAutomat: public Automat {
public:
    InterpAutomat(): Automat() {}

    // Получение имени автомата
    QString GetName() {return QString(":");}

    // Тестовый вывод автомата
    void TestOut(QTextStream& out);
    QString GetAsStr();

    // Обработка сигнала
    AutomatResult ProcessSignal(AutomatState curState, int input);
    // Возврат типа
    autoType GetAutomatType() {return AT_INTER; }
};

//=================================================================================================
// Класс, определяющий автомат, выполняющий функции списка данных
class DlistAutomat: public Automat {
public:
    DlistAutomat(): Automat() {}
    // Получение имени автомата
    QString GetName() {return QString("(---)");}

    // Тестовый вывод автомата
    void TestOut(QTextStream& out);
    QString GetAsStr();

    // Обработка сигнала
    AutomatResult ProcessSignal(AutomatState curState, int input);
    // Возврат типа
    autoType GetAutomatType() {return AT_DLIST; }
};

//=================================================================================================
// Класс, определяющий автомат, выполняющий функции параллельного списка
class PlistAutomat: public Automat {
public:
    PlistAutomat(): Automat() {}
    // Получение имени автомата
    QString GetName() {return QString("[---]");}

    // Тестовый вывод автомата
    void TestOut(QTextStream& out);
    QString GetAsStr();

    // Обработка сигнала
    AutomatResult ProcessSignal(AutomatState curState, int input);
    // Возврат типа
    autoType GetAutomatType() {return AT_PLIST; }
};

//=================================================================================================
// Класс, определяющий автомат, выполняющий функции асинхронного списка
class AlistAutomat: public Automat {
public:
    AlistAutomat(): Automat() {}

    // Получение имени автомата
    QString GetName() {return QString("<(--)");}

    // Тестовый вывод автомата
    void TestOut(QTextStream& out);
    QString GetAsStr();

    // Обработка сигнала
    AutomatResult ProcessSignal(AutomatState curState, int input);
    // Возврат типа
    autoType GetAutomatType() {return AT_ALIST; }
};

//=================================================================================================
// Класс, определяющий автомат, выполняющий функции вершины arg
class ArgAutomat: public Automat {
public:
    ArgAutomat(): Automat() {}

    // Получение имени автомата
    QString GetName() {return QString("arg");}

    // Тестовый вывод автомата
    void TestOut(QTextStream& out);
    QString GetAsStr();

    // Обработка сигнала
    AutomatResult ProcessSignal(AutomatState curState, int input);
    // Возврат типа
    autoType GetAutomatType() {return AT_ARG; }
};

//=================================================================================================
// Класс, определяющий автомат, выполняющий функции вершины return
class RetAutomat: public Automat {
public:
    RetAutomat(): Automat() {}

    // Получение имени автомата
    QString GetName() {return QString("return");}

    // Тестовый вывод автомата
    void TestOut(QTextStream& out);
    QString GetAsStr();

    // Обработка сигнала
    AutomatResult ProcessSignal(AutomatState curState, int input);
    // Возврат типа
    autoType GetAutomatType() {return AT_RET; }
};


#endif // AUTOMAT_H
