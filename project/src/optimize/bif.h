#ifndef BIF_H
#define BIF_H

#include <QString>

namespace dfg {
  struct Inode;

  Inode* get_bif(QString op);
}

#endif // BIF_H
