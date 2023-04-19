#ifndef VALUE_H
#define VALUE_H

#include <QtCore>
#include <vector>
using namespace std;
// Признак (тип) значения
enum ValueType {
    IntValueType, BoolValueType, FloatValueType, CharValueType,
    SpecValueType, DelayValueType, ErrorValueType,
    ExternValueType, MockValueType, NullValueType,
    DataListValueType, ParListValueType, AsyncListValueType
};

//=================================================================================================
// Базовый класс, определяющий общий интерфейс для всех значений
class Value:public QObject {
protected:
    ValueType valueType;
    int input;
public:
    Value();

    // Получение типа значения
    ValueType GetValueType() {return valueType;}
    Value* GetValue() {return this;}
    void SetInput(int newInput) { input=newInput; }
    int GetInput() { return input; }
    virtual Value* ParGetElement(int elementNum);
    virtual int GetUID(int elemNum) { return 0; }
    // Получение текстового представления значения
    virtual QString GetAsStr() = 0;
    // Тестовый вывод значения
    virtual void TestOut(QTextStream& out) = 0;
    // Как-бы клонировщик
    virtual Value* Clone() = 0;    
};
//=================================================================================================
// Класс логического значения
class BoolValue: public Value {
    bool value;
public:
    BoolValue(bool v): value(v) {valueType=BoolValueType;}

    // получение логического значения
    bool GetBoolValue() { return value;}
    // Получение текстового представления значения
    QString GetAsStr();
    // Тестовый вывод целочисленного значения
    void TestOut(QTextStream& out);
    // Как-бы клонировщик
    Value* Clone();
};

//=================================================================================================
// Класс целого числа
class IntValue: public Value {
    int value;
public:
    IntValue(int v): value(v) {valueType = IntValueType;}

    // получение целочисленного значения
    int GetIntValue() { return value;}
    // Получение текстового представления значения
    QString GetAsStr();
    // Тестовый вывод целочисленного значения
    void TestOut(QTextStream& out);
    // Как-бы клонировщик
    Value* Clone();
};

//=================================================================================================
// Класс действительного числа
class FloatValue: public Value {
    double value;
public:
    FloatValue(double v): value(v) {valueType = FloatValueType;}

    // получение вещественного значения
    double GetFloatValue() { return value;}
    // Получение текстового представления значения
    QString GetAsStr();
    // Тестовый вывод действительного значения
    void TestOut(QTextStream& out);
    // Как-бы клонировщик
    Value* Clone();
};

//=================================================================================================
// Класс символа
class CharValue: public Value {
    QChar value;
public:
    CharValue(QChar v): value(v) {valueType = CharValueType;}

    // получение символьного значения
    QChar GetCharValue() { return value;}
    // Получение текстового представления значения
    QString GetAsStr();
    // Тестовый вывод символа
    void TestOut(QTextStream& out);
    // Как-бы клонировщик
    Value* Clone();
};

//=================================================================================================
// Перечислимый тип, задающий специальные обозначения
enum SpecType {
    dotSpec,        // 00: "." - пустое или сигнальное значение
    addSpec,        // 01: "+" - плюс
    minusSpec,      // 02: "-" - минус
    astSpec,        // 03: "*" - звездочка
    slashSpec,      // 04: "/" - слэш
    percentSpec,    // 05: "%" - процент
    dotsSpec,       // 06: ".." - многоточие
    dupSpec,        // 07: "dup" - дубликаты
    ltSpec,         // 08: "<" - меньше
    leSpec,         // 09: "<=" - меньше или равно
    eqSpec,         // 10: "=" - равно
    neSpec,         // 11: "!=" - не равно
    geSpec,         // 12: ">=" - больше или равно
    gtSpec,         // 13: ">" - больше
    vertSpec,       // 14: "|" - вертикальная черта
    questSpec,      // 15: "?" - вопрос
    sharpSpec,      // 16: "#" - решетка
    trueSpec,       // 17: "true" - истина
    falseSpec,      // 18: "false" - ложь
    typeSpec,       // 19: "type" - тип
    valSpec,        // 20: "value" - значение
    boolSpec,       // 21: "bool" - булевый
    charSpec,       // 22: "char" - символьный
    intSpec,        // 23: "int" - целый
    floatSpec,      // 24: "float" - действительный
    strSpec,        // 25: "string" - строковый
    dalistSpec,     // 26: "()" - список данных
    palistSpec,     // 27: "[]" - параллельный список
    delistSpec,     // 28: "{}" - задержанный список
    aslistSpec,     // 29: "<()" - асинхронный список
    errorSpec,      // 30: "error" - ошибка
    inSpec,         // 31: "in"
    signalSpec,     // 32: signal - преобразователь в сигнал
    lastSpec        // X: "последняя" спецоперация - позволяет получить число спецопераций
};

//=================================================================================================
// Формирование текстового значения для специального обозначения
QString GetAsStr(SpecType value);

//=================================================================================================
// Класс специального обозначения
class SpecValue: public Value {
    SpecType value;
public:
    SpecValue(SpecType v): value(v) {valueType = SpecValueType;}
    int GetAddr() {return static_cast<int>(value);}

    // получаем SpecType
    SpecType GetSpecValue() {return value;}
    // Получение текстового представления значения
    QString GetAsStr();
    // Тестовый вывод специального обозначения
    void TestOut(QTextStream& out);
    // Как-бы клонировщик
    Value* Clone();
};

//=================================================================================================
// Класс для задержанной константы
class DelayValue: public Value {
    int number;
    int link;
    int EPID;
public:
    DelayValue(int num, int lnk): number(num), link(lnk), EPID(-1) {valueType = DelayValueType;}
    DelayValue(int num, int lnk, int newEPID): number(num), link(lnk), EPID(newEPID) {valueType = DelayValueType;}
    void SetEPID(int newEPID) {EPID=newEPID;}
    // Получение номера задержанного списка
    int GetDelayNumber() {return number;}
    // Получение динамической связи задаваемой задержанной константой
    int GetDelayLink() {return link;}
    // Получение идентификатора EP, создавшего задержанную константу
    int GetEPID() {return EPID;}
    // Получение текстового представления значения
    QString GetAsStr();
    // Тестовый вывод задержанной константы
    void TestOut(QTextStream& out);
    // Как-бы клонировщик
    Value* Clone();
};

//=================================================================================================
// Перечислимый тип, задающий типы ошибок
enum ErrorType {
    BASEFUNCERROR,  // 0: общая ошибка
    TYPEERROR,		// 1: ошибка получения типа
    BOUNDERROR,		// 2: выход за границы массива
    INTERROR,		// 3: ошибка при выполнении целочисленной операции
    REALERROR,		// 4: ошибка при выполнении операции с плавающей точкой
    ZERODIVIDE,		// 5: деление на ноль
    VALUEERROR		// 6: ошибка функции value
};

//=================================================================================================
// Формирование текстового значения для специального обозначения
QString GetAsStr(ErrorType value);

//=================================================================================================
// Класс специального обозначения
class ErrorValue: public Value {
    ErrorType value;
public:
    ErrorValue(ErrorType v): value(v) {valueType = ErrorValueType;}

    // Получение текстового представления значения
    QString GetAsStr();
    // Тестовый вывод специального обозначения
    void TestOut(QTextStream& out);
    // Как-бы клонировщик
    Value* Clone();
    // получаем ErrorType
    ErrorType GetErrorValue() {return value;}
};
//=================================================================================================
// Класс ссылки на внешнюю сущность
class ExternValue: public Value {
    int value;
    int EPID;
public:
    ExternValue(int v, int newEPID): value(v), EPID(newEPID) {valueType = ExternValueType;}
    int GetExternValue() {return value;}
    int GetEPID()        {return EPID;}
    // Получение текстового представления значения
    QString GetAsStr();
    // Тестовый вывод специального обозначения
    void TestOut(QTextStream& out);
    // Как-бы клонировщик
    Value* Clone();
};
//=================================================================================================
// Класс объекта-затычки
class MockValue: public Value {
public:
    MockValue() {valueType = MockValueType;}
    // Получение текстового представления значения
    QString GetAsStr();
    // Тестовый вывод специального обозначения
    void TestOut(QTextStream& out);
    // Как-бы клонировщик
    Value* Clone();
};
//=================================================================================================
// Класс пустого значения
class NullValue: public Value {
public:
    NullValue() {valueType = NullValueType;}
    // Получение текстового представления значения
    QString GetAsStr();
    // Тестовый вывод специального обозначения
    void TestOut(QTextStream& out);
    // Как-бы клонировщик
    Value* Clone();
};
//=================================================================================================
// Класс списка
class ListValue: public Value {
protected:
    QVector<Value*> elements;
public:
    ListValue() { elements.clear();}
    void Append(Value *x);
    void SetElement(Value *x, int index);

    // Получение текстового представления значения
    virtual QString GetAsStr()=0;
    // Тестовый вывод специального обозначения
    virtual void TestOut(QTextStream& out)=0;
    // Получение ссылки на список связей
    QVector<Value*>* GetElementList() {return &elements;}
    // Получение длины списка связей
    int GetElementListSize() {return elements.size();}
    // Как-бы клонировщик
    virtual Value* Clone()=0;
};
//=================================================================================================
// Класс списка данных
class DataListValue: public ListValue {
public:
    DataListValue():ListValue() {valueType = DataListValueType; };

    DataListValue(int base[]);
    DataListValue(QString string);
    // Получение текстового представления значения
    QString GetAsStr();
    // Тестовый вывод специального обозначения
    void TestOut(QTextStream& out);
    // Как-бы клонировщик
    Value* Clone();
    // Очистка сигналов
    void ClearSignals();
};
//=================================================================================================
// Класс параллельного списка
class ParListValue: public ListValue {
    QQueue<int> readyIndexes;
    QVector<int> elementInputs;
    QVector<int> elementUIDS;
    int curUID=0;
public:
    ParListValue():ListValue(){valueType = ParListValueType;}
    Value* ParGetElement(int elementNum);
    void Append(Value *x);
    void SetElement(Value *x, int input);
    int GetReadyIndex(int offset);
    int GetUID(int elemNum);
    QVector<Value*>* GetAllList();
    // Получение текстового представления значения
    QString GetAsStr();
    // Тестовый вывод специального обозначения
    void TestOut(QTextStream& out);
    void ConstantReadyFill();
    // Как-бы клонировщик
    Value* Clone();
};
//=================================================================================================
// Класс асинхронного списка
class AsyncListValue: public ListValue {
    Value* head;
    AsyncListValue* tail;
public:
    AsyncListValue();
    // Получение текстового представления значения
    QString GetAsStr();
    // Тестовый вывод специального обозначения
    void TestOut(QTextStream& out);
    // Как-бы клонировщик
    Value* Clone();

    // Геттер головы
    Value* GetHead() {return head;}
    // Cеттер головы
    void SetHead(Value* newHead) {head=newHead;}
    // Геттер хвоста
    AsyncListValue* GetTail() {return tail;}
    // Cеттер хвоста
    void SetTail(AsyncListValue* newTail) {tail=newTail;}
    // добавление нового элемента
    void Append(Value *newValue);

    /*ParListValue():ListValue(){valueType = ParListValueType;}
    Value* ParGetElement(int elementNum);

    void SetElement(Value *x, int index);
    int GetReadyIndex(int offset);
    QVector<Value*>* GetAllList();
    void ConstantReadyFill();*/

};

#endif // VALUE_H
