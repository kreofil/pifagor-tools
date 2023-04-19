#include "csignal.h"

//=================================================================================================
// Тестовый вывод внешней ссылки
void Signal::TestOut(QTextStream& out) {
    out << " ("<< node << ", " << input << ")" << endl;
}

QString Signal::GetAsStr()
{
    return QString("(%1, %2)").arg(node).arg(input);
}

