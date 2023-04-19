#include "dynlink.h"

//=================================================================================================
// Тестовый вывод внешней ссылки
void DynLink::TestOut(QTextStream& out) {
    out << delNum << " {" << node << "}" << endl;
}
QString DynLink::GetAsStr() {
    return QString("%1 {%2}\n").arg(delNum).arg(node);
}
