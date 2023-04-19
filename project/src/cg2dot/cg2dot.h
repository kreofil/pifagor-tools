#ifndef CG2DOT_H
#define CG2DOT_H

#include <QtCore>
#include <QVector>
#include <QMap>
#include <QCoreApplication>

#include "../cg/cg.h"

class Cg2Dotter
{
private:
    struct trio {int node, input, count;};
    QString CgDigraph(QString name);
    QString CgNodeParam();
    QString CgEdgeParam();
    QString NameNode(QString s1);
    QString TerminalNode(QString s1);
    QString SignalNode(QString s1, QString s2);
    QString ControlNode(QString s1, QString s2);
    QString CtrlLink(QString s1, QString s2, QString s3);
    QString InvisibleLink(QString s1, QString s2);
    QString DelimeterLink(QString s1, QString s2);
    QString DelayStr(QString s1, QString s2);
public:
    QString Cg2Dot (Cg& cg);
    bool SaveCgDot(Cg& cg, QString fileName);
};

#endif // CG2DOT_H
