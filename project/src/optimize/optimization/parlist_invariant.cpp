#include "methods.h"
#include "dfg_functions.h"
#include <QDebug>
#include "invariant_functions.h"

using namespace dfg;
using namespace dfg::optimization;

std::vector<DFG*> dfg::optimization::parlist_invariant(
    DFG* dfg, std::vector<DFG*>& all_dfgs) {

  auto interp_ops = find_nodes(dfg, Inode::Type::act, ":");

  std::vector<DFG*> opt_functions;

  for (Inode* call : interp_ops) {
    unsigned int delay = call->delay;
    // поиск подграфа, означающего вызов функции в цикле
    Inode* func = call->useDefRefs[Slots::Func];
    if (func->type != Inode::Type::ext)
      continue;

    Inode* parlist = call->useDefRefs[Slots::Data];
    if (parlist->type != Inode::Type::act || parlist->value != ":")
      continue;

    Inode* parlist_op = parlist->useDefRefs[Slots::Func];
    if (parlist_op->type != Inode::Type::bif || parlist_op->value != "[]")
      continue;

    Inode* transp = parlist->useDefRefs[Slots::Data];
    if (transp->type != Inode::Type::act || transp->value != ":")
      continue;

    Inode* transp_op = transp->useDefRefs[Slots::Func];
    if (transp_op->type != Inode::Type::bif || transp_op->value != "#")
      continue;

    Inode *call_arg_list = transp->useDefRefs[Slots::Data];
    if (call_arg_list->type != Inode::Type::act || call_arg_list->value != "(---)")
      continue;

    // получаю список неизменяемых аргументов
    std::map<int, Inode*> const_args;
    int idx = 0;
    Inode* len_node = nullptr;
    Inode* dup_node = nullptr;
    for (Inode* arg : call_arg_list->useDefRefs) {
      ++idx;
      if (arg->type != Inode::Type::act || arg->value != ":")
        continue;

      Inode* dup_op = arg->useDefRefs[Slots::Func];
      if (dup_op->type != Inode::Type::bif || dup_op->value != "dup")
        continue;

      Inode* dup_list = arg->useDefRefs[Slots::Data];
      if (dup_list->type != Inode::Type::act || dup_list->value != "(---)")
        continue;

      if (len_node == nullptr)
        len_node = dup_list->useDefRefs[1];

      if (dup_node == nullptr)
        dup_node = dup_op;

      const_args[idx] = dup_list->useDefRefs[0];
    }

    // копирую подставляемую функцию
    DFG* func_dfg = nullptr;
    for (DFG* dfg : all_dfgs)
      if (dfg->function_name == func->value) {
        func_dfg = dfg;
        break;
      }
    if (func_dfg == nullptr) {
      throw NoSuchFunction(func->value.toLatin1());
    }

    DFG* func_dfg_copy = new DFG(func->value);
    clone(func_dfg->nodes, func_dfg_copy);
    func_dfg_copy->function_name += "_cst_args";
    for (auto const_arg : const_args) {
      func_dfg_copy->function_name += QString::fromLatin1("_") + QString::number(const_arg.first);
    }
    Inode *ext0 = find_single_node(func_dfg_copy, Inode::Type::ext, func_dfg->function_name);
    ext0->value = func_dfg_copy->function_name;

    opt_functions.push_back(func_dfg_copy);
    all_dfgs.push_back(func_dfg_copy);

    // добавляю в список invars узлы получения неизменяемых аргументов
    std::list<Inode*> invars;
    Inode* arg = find_single_node(func_dfg_copy, Inode::Type::act, "arg");

    set_attached_value(func_dfg_copy, Inode::AttachedNodeData(Unknown, "", 0.));

    std::map<int, Inode*> get_args_map;
    for (auto const_arg : const_args) {
      Inode *const_arg_num_node = find_single_node(func_dfg_copy, Inode::Type::loc, QString::number(const_arg.first));

      for (Inode* get_arg : arg->def_useRefs) {
        if (get_arg->type != Inode::Type::act || get_arg->value != ":")
          continue;

        if (const_arg_num_node != get_arg->useDefRefs[Slots::Func])
          continue;

        get_arg->attached.intValue = ConstArg;
        get_args_map[const_arg.first] = get_arg;
        invars.push_back(get_arg);
      }
    }

    //TODO: copy caste
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

    //TODO: copy paste
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

    //TODO: copy paste
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

    // перемещаю узлы
    std::map<Inode*, Inode*> invars_to_main = clone(invars, dfg);

    // заменяю узлы получения аргументов функции узлами со значенями в функции dfg
    // std::map<int, Inode*> const_args - хранит список узлов-значений в функции dfg
    //  при этом:
    //    int - это номер аргмента функции
    //    Inode* - указатель на узел в функции dfg
    // std::map<int, Inode*> get_args_map - хранит список узлов получения
    //  аргументов в функции func_dfg_copy, при этом:
    //    int - это номер аргмента функции
    //    Inode* - указатель на узел в функции func_dfg_copy
    // std::map<Inode*, Inode*> invars_to_main - хранит соответствие между узлами
    //  в функциях func_dfg_copy и dfg

    for (auto func_dfg_get_arg : get_args_map) {
      Inode *dfg_get_arg = invars_to_main[func_dfg_get_arg.second];
      Inode *dfg_arg_value_node = const_args[func_dfg_get_arg.first];
      remove_node(dfg, dfg_get_arg, dfg_arg_value_node);
    }

    // добавляю инварианты в качестве дополнительных аргументов:

    int constant_number = call_arg_list->useDefRefs.size();
    for (Inode* invar : using_invars) {
      Inode* invar_dfg = invars_to_main[invar];

      // -- в функцию dfg добавится (Inv, Len):dup и ссылки в список вызова
      constant_number++;

      Inode *invar_dup_list = new Inode(Inode::Type::act, "(---)", delay);
      invar_dup_list->useDefRefs.push_back(invar_dfg);
      invar_dup_list->useDefRefs.push_back(len_node);
      invar_dfg->def_useRefs.push_back(invar_dup_list);
      len_node->def_useRefs.push_back(invar_dup_list);

      Inode *invar_dup = new Inode(Inode::Type::act, ":", delay);
      invar_dup->useDefRefs.push_back(invar_dup_list);
      invar_dup->useDefRefs.push_back(dup_node);
      invar_dup_list->def_useRefs.push_back(invar_dup);
      dup_node->def_useRefs.push_back(invar_dup);

      call_arg_list->useDefRefs.push_back(invar_dup);
      invar_dup->def_useRefs.push_back(call_arg_list);

      dfg->nodes.push_back(invar_dup_list);
      dfg->nodes.push_back(invar_dup);

      // в функцию func_dfg_copy добавится код получения аргумента constant_number
      // и этим узлом заменится инвариант

      Inode *constant_number_node = new Inode(Inode::Type::loc, QString::number(constant_number), 0);
      Inode *get_arg_node = new Inode(Inode::Type::act, ":", 0);
      get_arg_node->useDefRefs.push_back(arg);
      get_arg_node->useDefRefs.push_back(constant_number_node);
      arg->def_useRefs.push_back(get_arg_node);
      constant_number_node->def_useRefs.push_back(get_arg_node);

      remove_node(func_dfg_copy, invar, get_arg_node);

      func_dfg_copy->nodes.push_back(constant_number_node);
      func_dfg_copy->nodes.push_back(get_arg_node);
    }

    //qDebug() << invars;
    //qDebug() << using_invars;
  }
  return opt_functions;
}
