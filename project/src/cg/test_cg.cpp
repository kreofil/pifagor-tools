#include <QtCore>
#include "cg.h"

int main() {
    QTextStream cout(stdout);
    FILE *out=fopen("test_cg_out.txt","w");
    QTextStream fout(out);

    Cg cg;
    // Формирование имени
    cg.SetName(QString("TestName"));

    // Формирование списка узлов
    CtrlNode *tmpNode;
    tmpNode = new CtrlNode(0, new ArgAutomat(), AutomatState(0), 0);
    tmpNode->Append(1,3);
    tmpNode->Append(3,2);
    tmpNode->Append(4,1);
    cg.Append(tmpNode);

    tmpNode = new CtrlNode(1, new InterpAutomat(), AutomatState(1), 1);
    tmpNode->Append(2,1);
    cg.Append(tmpNode);

    tmpNode = new CtrlNode(1, new DlistAutomat(), AutomatState(0), 2);
    cg.Append(tmpNode);

    // Формирование списка сигналов
    cg.Append(new Signal(1, 1));
    cg.Append(new Signal(2, 2));
    cg.Append(new Signal(3, 1));
    cg.Append(new Signal(4, 2));
    cg.Append(new Signal(5, 1));
    cg.Append(new Signal(5, 2));
    cg.Append(new Signal(8, 2));

    // Формирование списка динамических связей
    cg.Append(new DynLink(1, 2));
    cg.Append(new DynLink(3, 4));
    cg.Append(new DynLink(5, 6));
    cg.Append(new DynLink(7, 8));

    cg.TestOut(cout);
    cg.TestOut(fout);
    return 0;
}
