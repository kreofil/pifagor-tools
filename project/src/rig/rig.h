#ifndef RIG_H
#define RIG_H

#include <QVector>
#include <map>
#include "actor.h"
#include "extref.h"
#include "value.h"
#include "../globals/globals.h"
using namespace std;
//=================================================================================================
// Структура для хранения текущей позиции по всем векторам РИГ
struct RigPos
{
    long externPos;
    long localPos;
    long actorPos;
};

//=================================================================================================
// Класс, определяющий структуру реверсивного информационного графа (РИГ).
// Содержит все необходимые слои, обеспечивающие хранение ссылок на внешние сущности,
// константы и узлы РИГ
class Rig: public QObject {
    QString name;                   //Имя функции
    QVector<ExtRef*> extRefList;         // Список внешних ссылок.
    QVector<Value*> localConstList; // Список локальных констант функции.
    QVector<Actor*> actorList;           // Список узлов РИГ, определяющих операторы ФПП программы.
    QVector< QVector<QString> > nameTable;
public:
    // Геттеры имени - как QString и как char*
    QString GetName() { return name; }
    char* GetCharName();
    // Сеттер имени
    void SetName(QString newName) { name=newName; }
    // Конструктор РИГ.
    Rig();
    Rig(QString newName);
    ~Rig() { Clear();}
    // Клонировщик
    Rig* Clone();
    // Добавление внешней ссылки в РИГ.
    MemLocation Append(ExtRef* extRef);
    // Добавление локальной константы в РИГ.
    MemLocation Append(Value* value);
    // Добавление в РИГ оператора ФПП программы.
    MemLocation Append(Actor* actor);
    // Добавление имени в таблицу имен РИГ
    void AppendName(MemLocation id, QString name);

    // Доступ к списку внешних ссылок
    QVector<ExtRef*>* GetExtRefList() { return &extRefList; }
    // Доступ к списку локальных констант
    QVector<Value*>* GetlocalConstList() { return &localConstList; }
    // Доступ к списку узлов (акторов) РИГ
    QVector<Actor*>* GetActorList() { return &actorList; }

    // Очиска содержимого класса от накопившихся данных
    void Clear();

    // Тестовый вывод списка внешних ссылок
    void TestOutExtRef(QTextStream& out);
    // Тестовый вывод списка локальных констант функции
    void TestOutLocalConst(QTextStream& out);
    // Тестовый вывод списка операций РИГ
    void TestOutActor(QTextStream& out);
    // Тестовый вывод всех составляющих РИГ
    void TestOut(QTextStream& out);

    QString GetAsStrExtRef();
    QString GetAsStrLocalConst();
    QString GetAsStrActor();
    QString GetAsStr();

    //получаем РИГ одной строкой
    QString GetTextRepresentation();

    //генерируем таблицу символов РИГ
    QString GetSymbolTable();

    // Генерация текстового представления
    bool SaveText(QString fileName);
    // Генерация dot- представления
    bool SaveDot(QString fileName);

    //Получение текущей позиции
    RigPos GetPos();
    // Откат на заданную позицию
    void Rollback(RigPos position);

    void test1();
};

#endif // RIG_H
