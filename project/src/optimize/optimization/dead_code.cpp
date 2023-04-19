#include "methods.h"
#include "dfg_functions.h"

using namespace dfg;
using namespace dfg::optimization;

void dfg::optimization::dead_code(DFG *dfg) {
  Inode *retNode = find_single_node(dfg, Inode::Type::act, "return");

  std::set<Inode*> used = collect_use_def(dfg, retNode);

  for (auto it = dfg->nodes.begin(); it != dfg->nodes.end();) {
    Inode* node = *it;
    if (used.find(node) != used.end() ||
        node->type == Inode::Type::bif ||
        (node->type == Inode::Type::ext && node->value == dfg->function_name))
      ++it;
    else {
      delete *it;
      it = dfg->nodes.erase(it);
    }
  }
}
