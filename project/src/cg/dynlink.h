#ifndef DYNLINK_H
#define DYNLINK_H

#include <QtCore>

//=================================================================================================
// Класс, динамическую связь для задержанного списка.
class DynLink: public QObject {
    int delNum; // Номер задержанного списка
    int node;   // Номер задержанного узла
public:
    DynLink(int dn, int n): delNum(dn), node(n) {}

    // Вывод номера задержанного списка
    int GetDelNum() {return delNum;}
    // Вывод номера узла
    int GetNode() {return node;}

    // Тестовый вывод динамической связи
    void TestOut(QTextStream& out);
    QString GetAsStr();
};

#endif // DYNLINK_H
