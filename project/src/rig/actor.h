#ifndef ACTOR_H
#define ACTOR_H

#include <QtCore>

//=================================================================================================
// Класс, задающий координаты актора в исходном тексте функции
class Positions {
    int pos[4];
public:
    Positions() {}
    Positions(int r0, int c0, int r1, int c1){
        pos[0] = r0; pos[1] = c0;
        pos[2] = r1; pos[3] = c1;
    }
    Positions(int p[]) {
        pos[0] = p[0]; pos[1] = p[1];
        pos[2] = p[2]; pos[3] = p[3];
    }
    Positions(Positions& oldPos) {
        pos[0] = oldPos.GetBeginRow(); pos[1] = oldPos.GetBeginCol();
        pos[2] = oldPos.GetEndRow(); pos[3] = oldPos.GetEndCol();
    }
    Positions(const Positions& oldPos) {
        pos[0] = oldPos.pos[0]; pos[1] = oldPos.pos[1];
        pos[2] = oldPos.pos[2]; pos[3] = oldPos.pos[3];
    }
    // Установка позиции
    void Set(int r0, int c0, int r1, int c1) {
        pos[0] = r0; pos[1] = c0;
        pos[2] = r1; pos[3] = c1;
    }
    void Set(int p[]) {
        pos[0] = p[0]; pos[1] = p[1];
        pos[2] = p[2]; pos[3] = p[3];
    }

    int* GetAll() {return pos;}
    int GetPos(int num) {return pos[num];}
    int GetBeginRow() {return pos[0];}
    int GetBeginCol() {return pos[1];}
    int GetEndRow() {return pos[2];}
    int GetEndCol() {return pos[3];}

    // Тестовый вывод позиций
    void TestOut(QTextStream& out);
};

// Типы акторов РИГ
enum ActorType
{ argType, interpType, datalistType, parlistType, aslistType, retType};

//=================================================================================================
// Базовый класс для построения различных операторов модели ФПП программирования
class Actor: public QObject {
    int delayNum;   // Номер задержанного списка, в который включен актор
protected:
    ActorType actorType;
public:

    Positions pos;  // Координаты, определяющие положение в исходном тексте функции
    QString linkName;   //имя соответствующей актору связи-"переменной"
    Actor();
    void SetDelay(int num) {delayNum = num;}
    int GetDelay() {return delayNum;}
    void SetLinkName(QString name) {linkName=name;}
    QString GetLinkName() {return linkName;}
    ActorType GetActorType() {return actorType;}

    // Получение текстового представления актора
    virtual QString GetAsStr() = 0;
    // Тестовый вывод операции
    virtual void TestOut(QTextStream& out) = 0;
    // Клонировщик
    virtual Actor* Clone() = 0;
};

//=================================================================================================
// Класс начального актора
class BegActor: public Actor {
public:
    BegActor() {actorType = argType;}

    // Получение текстового представления актора
    QString GetAsStr();
    // Тестовый вывод операции
    void TestOut(QTextStream& out);
    // Клонировщик
    Actor* Clone();
};

//=================================================================================================
// Тип памяти, используемый для хранения программных объектов
enum MemType {
    act = 0,    // Акторы РИГ
    ext = 1,    // Внешние ссылки
    loc = 2,    // Локальные константы
    spec = 3    // Специальные константы
};

//=================================================================================================
// Структура, задающая положение в неоднородной памяти
class MemLocation {
    MemType type;
    int index;
public:
    MemLocation(MemType t, int i): type(t), index(i) {}
    MemLocation() {}

    // Установка параметров местоположения
    void Set(MemType t, int i) {
        type = t;
        index = i;
    }
    // Получение типа используемой памяти
    MemType GetMemType() {return type;}

    // Получение дескриптора элемента памяти
    int GetIndex() {return index;}

    bool isSpecParList();

    // Получение текстового представления памяти
    QString GetAsStr();
    // Тестовый вывод параметров местоположения
    void TestOut(QTextStream& out);
};


//=================================================================================================
// Класс оператора интерпретации
class InterpActor: public Actor {
public:
    InterpActor(MemType argType, int argIndex, MemType funcType, int funcIndex);
    InterpActor(MemLocation argLoc, MemLocation funcLoc);

    // Местоположение функции
    MemLocation func;
    // Местоположение аргумента
    MemLocation arg;

    // Получение текстового представления актора
    QString GetAsStr();
    // Тестовый вывод операции интерпретации
    void TestOut(QTextStream& out);
    // Клонировщик
    Actor* Clone();
};

//=================================================================================================
// Промежуточный класс для всех операторов группировки в список
class ListActor: public Actor {
public:
    ListActor() {}
    ~ListActor() {Clear();}

    // Преобразование в строку списка ссылок на аргументы
    QString RefListAsStr();
    // Список местоположений аргументов
    QList<MemLocation> memList;
    void Clear() {memList.clear();}
    // Добавление элемента в список
    void AddToList(MemType argType, int argIndex);
    void AddToList(MemLocation newEl);
    // Клонировщик
    virtual Actor* Clone()=0;
};


//=================================================================================================
// Класс оператора группировки в список данных
class DataListActor: public ListActor {
public:
    DataListActor() {actorType = datalistType;}

    // Получение текстового представления актора
    QString GetAsStr();
    // Тестовый вывод операции группировки в список данных
    void TestOut(QTextStream& out);
    // Клонировщик
    Actor* Clone();
};

//=================================================================================================
// Класс оператора группировки в параллельный список
class ParListActor: public ListActor {
public:
    ParListActor() {actorType = parlistType;}

    // Получение текстового представления актора
    QString GetAsStr();
    // Тестовый вывод операции группировки в список данных
    void TestOut(QTextStream& out);
    // Клонировщик
    Actor* Clone();
};

//=================================================================================================
// Класс оператора группировки в асинхронный список
class AsListActor: public ListActor {
public:
    AsListActor() {actorType = aslistType;}

    // Получение текстового представления актора
    QString GetAsStr();
    // Тестовый вывод операции группировки в список данных
    void TestOut(QTextStream& out);
    // Клонировщик
    Actor* Clone();
};

//=================================================================================================
// Класс оператора возврата
class RetActor: public Actor {
public:
    RetActor(MemType argType, int argIndex);
    RetActor(MemLocation newArg);
    // Местоположение аргумента
    MemLocation arg;

    // Получение текстового представления актора
    QString GetAsStr();
    // Тестовый вывод операции интерпретации
    void TestOut(QTextStream& out);
    // Клонировщик
    Actor* Clone();
};

#endif // ACTOR_H
