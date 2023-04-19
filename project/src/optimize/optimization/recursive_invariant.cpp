#include "methods.h"
#include "dfg_functions.h"
#include "invariant_functions.h"
#include <QDebug>

using namespace dfg;
using namespace dfg::optimization;

DFG* dfg::optimization::recursive_invariant(DFG *sub_dfg) {
  // ищем узел-аргумент рекурсивного вызова
  Inode *rec_arg_list;
  {
    Inode *func = find_single_node(sub_dfg, Inode::Type::ext, sub_dfg->function_name);
    if (func->def_useRefs.size() == 0)
      throw CantGetNodeEx("ext0 not used");
    Inode *rec_call = *func->def_useRefs.begin();
    rec_arg_list = rec_call->useDefRefs[0];
  }

  // узлы, относящиеся к инварианту будем помечать чтобы проверять быстрее
  set_attached_value(sub_dfg, Inode::AttachedNodeData(Unknown, "", 0.));

  // обходим аргументы, проверяем их на неизменяемость
  std::list<Inode*> invars;
  for (size_t arg_num = 0; arg_num < rec_arg_list->useDefRefs.size(); ++arg_num) {
    Inode *get_arg = rec_arg_list->useDefRefs[arg_num];
    if (get_arg->type != Inode::Type::act || get_arg->value != ":")
      continue;

    Inode *fun_arg = get_arg->useDefRefs[0];
    if (fun_arg->type != Inode::Type::act || fun_arg->value != "arg")
      continue;

    Inode *arg_num_node = get_arg->useDefRefs[1];
    bool ok;
    if (arg_num_node->type != Inode::Type::loc)
      continue;
    auto value = arg_num_node->value.toUInt(&ok);
    if (false == ok && value == arg_num)
      continue;

    get_arg->attached.intValue = ConstArg;
    invars.push_back(get_arg);
  }

  // обходим список узлов-инваривантов, добавляем туда новые
  for (auto it = invars.begin(); it != invars.end(); ++it) {
    Inode* node = *it;

    if (node->type == Inode::Type::act && node->value == "[---]")
      continue;

    for (Inode* child : node->def_useRefs) {
      if (child->attached.intValue != Unknown)
        continue;

      bool is_invar = true;
      for (Inode* dep : child->useDefRefs) {
        if (dep->type == Inode::Type::act && dep->attached.intValue == Unknown) {
          is_invar = false;
          break;
        }
      }

      if (is_invar == true) {
        child->attached.intValue = Invar;
        invars.push_back(child);
      }
    }
  }

  // получаем зависимости инвариантов (константы)
  //std::list<Inode*> invars_deps;
  for (Inode* node : invars) {
    for (Inode* dep : node->useDefRefs) {
      if (dep->type != Inode::Type::act && dep->attached.intValue != InvDep) {
        dep->attached.intValue = InvDep;
        invars.push_back(dep);
      }
    }
  }

  // получаем используемые инварианты (значения, которые есть смысл передавать
  // в качестве дополнительных аргментов)
  std::list<Inode*> using_invars;
  for (Inode* node : invars) {
    if (node->attached.intValue == ConstArg)
      continue; // аргументы уже добавлены в список параметров
    for (Inode* dep : node->def_useRefs) {
      if (dep->attached.intValue == Unknown) {
        using_invars.push_back(node);
        break;
      }
    }
  }

  //qDebug() << invars;
  //qDebug() << invars_deps;

  Inode* arg_sub = find_single_node(sub_dfg, Inode::Type::act, "arg");
  invars.push_back(arg_sub);

  // клонирую подставляемую функцию:
  QString old_fname = sub_dfg->function_name;
  QString new_fname = sub_dfg->function_name+"_sub";

  DFG* main_dfg = new DFG(old_fname);
  std::map<Inode*, Inode*> invars_to_main = clone(invars, main_dfg);

  // добавление кода вызова функции (новой, основной)
  if (true)
  {
    Inode *arg_main = invars_to_main[arg_sub];
    Inode *subfun_node = new Inode(Inode::Type::ext, new_fname, 0);
    Inode *parlist = new Inode(Inode::Type::bif, "[]", 0);
    Inode *arg2parlist = new Inode(Inode::Type::act, ":", 0);
    arg2parlist->useDefRefs.push_back(arg_main);
    arg2parlist->useDefRefs.push_back(parlist);
    parlist->def_useRefs.push_back(arg2parlist);
    arg_main->def_useRefs.push_back(arg2parlist);
    Inode *callarglist = new Inode(Inode::Type::act, "(---)", 0);
    callarglist->useDefRefs.push_back(arg2parlist);
    arg2parlist->def_useRefs.push_back(callarglist);
    for (Inode* using_invar : using_invars) {
      Inode *node_in_main = invars_to_main[using_invar];
      callarglist->useDefRefs.push_back(node_in_main);
      node_in_main->def_useRefs.push_back(callarglist);
    }
    Inode *call_sub = new Inode(Inode::Type::act, ":", 0);
    call_sub->useDefRefs.push_back(callarglist);
    call_sub->useDefRefs.push_back(subfun_node);
    subfun_node->def_useRefs.push_back(call_sub);
    callarglist->def_useRefs.push_back(call_sub);
    Inode* ret = new Inode(Inode::Type::act, "return", 0);
    ret->useDefRefs.push_back(call_sub);
    call_sub->def_useRefs.push_back(ret);
    Inode *ext0 = new Inode(Inode::Type::ext, old_fname, 0);

    main_dfg->nodes.push_back(arg_main);
    main_dfg->nodes.push_back(subfun_node);
    main_dfg->nodes.push_back(parlist);
    main_dfg->nodes.push_back(arg2parlist);
    main_dfg->nodes.push_back(callarglist);
    main_dfg->nodes.push_back(call_sub);
    main_dfg->nodes.push_back(ret);
    main_dfg->nodes.push_back(ext0);
  }

  // замена обращений к инварианту из оптимизируемой функции (старой, сабфункции)
  if (true) {
    Inode* ext0 = find_single_node(sub_dfg, Inode::Type::ext, old_fname);
    sub_dfg->function_name = new_fname;
    ext0->value = new_fname;

    // добавляем операции получения аргументов
    // обрабатываем эти аргументы
    int arg_num = rec_arg_list->useDefRefs.size();
    for (Inode* using_invar : using_invars) {
      arg_num++;
      Inode *arg_num_node = new Inode(Inode::Type::loc, QString::number(arg_num), 0);
      Inode *get_arg_node = new Inode(Inode::Type::act, ":", 0);
      get_arg_node->useDefRefs.push_back(arg_sub);
      get_arg_node->useDefRefs.push_back(arg_num_node);
      arg_sub->def_useRefs.push_back(get_arg_node);
      arg_num_node->def_useRefs.push_back(get_arg_node);
      sub_dfg->nodes.push_back(arg_num_node);
      sub_dfg->nodes.push_back(get_arg_node);

      // заменяем использование инварианта исопльзованием аргумента
      remove_node(sub_dfg, using_invar, get_arg_node);

      // добавляем дополнительные параметры в рекурсивный вызов
      rec_arg_list->useDefRefs.push_back(get_arg_node);
      get_arg_node->def_useRefs.push_back(rec_arg_list);
    }
  }

  return main_dfg;
}
