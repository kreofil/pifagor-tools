#include "ctrlnode.h"

// Тестовый вывод связи между узлами УГ
void Link::TestOut(QTextStream& out) {
    out << " link(" << node << "," << input << ")" << endl;
}
QString Link::GetAsStr() {
    return QString(" (%1,%2)").arg(node).arg(input);
}

// Добавление связи между узлами управляющего графа
void CtrlNode::Append(int n, int i) {
    Link *l = new Link(n, i);
    linkList.append(l);
}

//=================================================================================================
// Очистка класса от динамических структур перед возможным повторным использованием
void CtrlNode::Clear() {
    // Очистка списков
    linkList.clear();
}


// Тестовый вывод узла УГ
void CtrlNode::TestOut(QTextStream& out) {
    out << "delList = " << delNum << ", ";
    automat->TestOut(out);
    out << ", RIG node = " << iNode << endl;
    int linkSize = linkList.size();
    out << "linkSize = " << linkSize << endl;
    out << "Links:" << endl;
    for(int i = 0; i < linkSize; ++i) {
        out <<  i << ": ";
        linkList[i]->TestOut(out);
    }
}

QString CtrlNode::GetAsStr()
{
    QString rez=QString("{%1}, ").arg(delNum);
    rez+=automat->GetAsStr();
    rez+=QString(", RIGN %1; ").arg(iNode);
    int linkSize = linkList.size();
    //rez+=QString("linkSize = %1").arg(linkSize);
    rez+=QString("links:");
    for(int i = 0; i < linkSize; ++i) {
        rez+=linkList[i]->GetAsStr();
    }
    return rez;
}
