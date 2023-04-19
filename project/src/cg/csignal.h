#ifndef CSIGNAL_H
#define CSIGNAL_H

#include <QtCore>

//=================================================================================================
// Класс, определяющий сигнал, передаваемый по управляющим связям УГ.
class Signal: public QObject {
    int node;   // Номер узла УГ, в который передается сигнал
    int input;  // Номер входа в узле УГ на который поступает сигнал
public:
    Signal() {}
    Signal(int n, int i): node(n), input(i) {}

    // Установка параметров сигнала после его формирования
    void Set(int n, int i) {node = n; input = i;}

    // Вывод номера узла
    int GetNode() {return node;}
    // Вывод номера входа
    int GetInput() {return input;}

    // Тестовый вывод сигнала
    void TestOut(QTextStream& out);
    QString GetAsStr();
};

#endif // CSIGNAL_H
