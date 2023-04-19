#include "extref.h"

//=================================================================================================
// Тестовый вывод внешней ссылки
void ExtRef::TestOut(QTextStream& out) {
    out << name << endl;
}
QString ExtRef::GetAsStr()
{
    return name;
}
