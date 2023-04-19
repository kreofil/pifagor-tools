#ifndef CG_H
#define CG_H

#include <QVector>
#include <QString>

#include "ctrlnode.h"
#include "csignal.h"
#include "dynlink.h"
#include "../rig/rig.h"

struct Trio {int source, reciver, input;};

//=================================================================================================
// Класс, определяющий структуру управляющего графа (УГ).
// Содержит все необходимые слои, обеспечивающие хранение ссылок на внешние сущности,
// константы и узлы УГ
class Cg: public QObject {
    QVector<CtrlNode*> ctrlNodeList;    // Список узлов УГ, определяющих управляющие связи
                                        // и автоматы ФПП программы.
    QVector<Signal*> signalList;        // Список сигналов.
    QVector<DynLink*> dynLinkList;      // Список динамических связей.
    QString name;                       // Имя управляющего графа == имени функции
public:
    // Конструктор УГ.
    Cg();
    // Деструктор УГ
    ~Cg() {Clear();}

    // Добавление в УГ управляющего узла ФПП программы.
    void Append(CtrlNode* n);
    // Добавление синала в УГ.
    void Append(Signal* s);
    // Добавление динамической связи в УГ.
    void Append(DynLink* d);

    // Установка имени УГ
    void SetName(QString n) {name = n;}
    // Получение имени УГ
    QString GetName() {return name;}
    char* GetCharName();
    // Доступ к списку узлов УГ
    QVector<CtrlNode*>* GetCtrlNodeList() { return &ctrlNodeList; }
    // Доступ к списку сигналов
    QVector<Signal*>* GetSignalList() { return &signalList; }
    // Доступ к списку динамических связей УГ
    QVector<DynLink*>* GetDynLinkList() { return &dynLinkList; }

    // Очистка класса от динамических структур перед возможным повторным использованием
    void Clear(); 
	
    // Тестовый вывод списка узлов УГ
    void TestOutCtrlNodeList(QTextStream& out);
    // Тестовый вывод списка сигналов
    void TestOutSignalList(QTextStream& out);
    // Тестовый вывод списка динамических связей
    void TestOutDynLinkList(QTextStream& out);
    // Тестовый вывод всех составляющих УГ
    void TestOut(QTextStream& out);

    QString GetAsStrCtrlNodeList();
    QString GetAsStrSignalList();
    QString GetAsStrDynLinkList();
    QString GetAsStr();

    // Преобразование РИГ в УГ
    void Transform(Rig *rig);

    QString GetTextRepresentation();

    // Генерация текстового представления
    bool SaveText(QString fileName);
    // Генерация dot- представления
    bool SaveDot(QString fileName);
};

#endif // CG_H
