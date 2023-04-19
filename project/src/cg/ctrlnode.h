#ifndef CTRLNODE_H
#define CTRLNODE_H

#include <QtCore>
#include <QVector>

#include "automat.h"

//=================================================================================================
// Промежуточная структура, задающая связь между узлами управляющего графа (узел - приемник)
class Link: public QObject {
    int node;   // Номер узла УГ, в который передается сигнал
    int input;  // Номер входа в узле УГ на который поступает сигнал
public:
    Link(int n, int i): node(n), input(i) {}

    // Получение информации об узле
    int GetNode() {return node;}
    // Получение информации о входе
    int GetInput() {return input;}
    // Тестовый вывод связи между узлами УГ
    void TestOut(QTextStream& out);
    QString GetAsStr();
};

//=================================================================================================
// Класс, определяющий узел УГ.
class CtrlNode: public QObject {
    int delNum;         // Номер задержанного списка
    Automat *automat;    // Управляющий автомат, связанный с данным узлом УГ
    AutomatState baseState;
    int iNode;          // Номер вершины РИГ, связанной с данной вершиной УГ
    QVector<Link*> linkList;    // Список узлов УГ, определяющих управляющие связи
public:
    CtrlNode(int dn, Automat *a, AutomatState as, int i): delNum(dn), automat(a), baseState(as), iNode(i) {}
    ~CtrlNode() {Clear();}

    // Добавление связи между узлами управляющего графа
    void Append(int n, int i);
    // Получение сведений о номере задержанного списка
    int GetDelNum() {return delNum;}
    // Получение имени автомата
    QString GetAutomatName() {return automat->GetName();}
    // Получение типа автомата
    int GetAutomatState() {return 0; }
    // Получение типа автомата
    autoType GetAutomatType() {return automat->GetAutomatType(); }
    // Получение информации об используемом узле РИГ
    int GetInode() {return iNode;}
    // Получение ссылки на список связей
    QVector<Link*>* GetLinkList() {return &linkList;}
    // Получение длины списка связей
    int GetLinkListSize() {return linkList.size();}
    // Очистка класса от динамических структур перед возможным повторным использованием
    void Clear();

    // Тестовый вывод узла УГ
    void TestOut(QTextStream& out);
    QString GetAsStr();

};

#endif // CTRLNODE_H
