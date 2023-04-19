#include "methods.h"
#include "dfg_functions.h"

using namespace dfg;
using namespace dfg::optimization;

void dfg::optimization::inline_substitution(DFG *to, DFG *from) {
  Inode* func_node_to = find_single_node(to, Inode::Type::ext, from->function_name);

  for (Inode* call : func_node_to->def_useRefs) {
    // для каждого вызова заменяемой функции:
    // нахожу место вызова:
    Inode *arg_to = call->useDefRefs[0]; // 1
    auto call_using_to = call->def_useRefs; // 2

    // клонирую подставляемую функцию:
    DFG* from_copy = new DFG(from->function_name);
    clone(from->nodes, from_copy);
    // удаляю узел ext:0
    Inode* from_name_node = find_single_node(from_copy, Inode::Type::ext, from->function_name);
    from_copy->nodes.remove(from_name_node);

    // нахожу в ней "начало" и "конец"
    Inode* arg_from = find_single_node(from_copy, Inode::Type::act, "arg");

    auto arg_using_from = arg_from->def_useRefs; // 3

    Inode* ret_from = find_single_node(from_copy, Inode::Type::act, "return");

    Inode* result_from = ret_from->useDefRefs[0]; // 4

    // заменяю использования аргумента
    arg_from->def_useRefs.remove(call);
    for (Inode *node : arg_using_from) {
      std::replace(node->useDefRefs.begin(), node->useDefRefs.end(),
                   arg_from, arg_to);
      arg_to->def_useRefs.push_back(node);
    }
    from_copy->nodes.remove(arg_from);
    delete arg_from;

    // заменяю "конец"
    result_from->def_useRefs.remove(ret_from);
    for (Inode *node : call_using_to) {
      // для каждого, кто использовал результат вызова подставленной функции...
      std::replace(node->useDefRefs.begin(), node->useDefRefs.end(),
                   call, result_from);
      result_from->def_useRefs.push_back(node);
    }
    from_copy->nodes.remove(ret_from);
    delete ret_from;

    // дописываю узлы из подставляемой функции в целевой dfg
    for (Inode* node : from_copy->nodes) {
      to->nodes.push_back(node);
    }

    to->nodes.remove(call);
    delete call;
  }
}
