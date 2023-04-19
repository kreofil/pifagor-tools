#include "bif.h"
#include "dfg.h"
#include <map>

dfg::Inode* dfg::get_bif(QString op) {
  return new dfg::Inode(dfg::Inode::Type::bif, op);
}

