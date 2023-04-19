#include <iostream>
#include "dfg.h"
#include "optimization/methods.h"
#include <fstream>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QFileInfo>

using namespace std;

int main(int argc, char *argv[]) {
  QTextStream Qcout(stdout);

  QCoreApplication app(argc, argv);
  QCoreApplication::setApplicationName("pifagor-optimizer");
  QCoreApplication::setApplicationVersion("1.0");

  QCommandLineParser parser;
  parser.setApplicationDescription("optimizer for Pifagor program");
  parser.addHelpOption();
  parser.addVersionOption();

  QCommandLineOption dead_code_option(QStringList() << "dc", "dead code optimization <function_name>.", "function_name");
  parser.addOption(dead_code_option);

  QCommandLineOption equivalent_transformations_option(QStringList() << "eq", "equivalent transformations optimization <function_name>.", "function_name");
  parser.addOption(equivalent_transformations_option);

  QCommandLineOption inline_option(QStringList() << "in", "inline optimization. Used with --where and --what options.");
  parser.addOption(inline_option);

  QCommandLineOption repeated_code_option(QStringList() << "rep", "repeated code optimization <function_name>.", "function_name");
  parser.addOption(repeated_code_option);

  QCommandLineOption recursive_invariant_option(QStringList() << "req_inv", "reqursive invariant optimization <function_name>. Helps function_name prints in stdout, it's .rig-file moves in current directory.", "function_name");
  parser.addOption(recursive_invariant_option);

  QCommandLineOption parlist_invariant_option(QStringList() << "par_inv", "parallel list invariant optimization <function_name> [other .rig-files]. Helps function_name prints in stdout, it's .rig-file moves in current directory.");
  parser.addOption(parlist_invariant_option);

  QCommandLineOption where_option(QStringList() << "where", "<function_name>.", "function_name");
  parser.addOption(where_option);

  QCommandLineOption what_option(QStringList() << "what", "<function_name>.", "function_name");
  parser.addOption(what_option);

  QCommandLineOption goal_option(QStringList() << "goal", "<function_name>.", "function_name");
  parser.addOption(goal_option);

  // Process the actual command line arguments given by the user
  parser.process(app);

  bool isDC = parser.isSet(dead_code_option);
  bool isEq = parser.isSet(equivalent_transformations_option);
  bool isRep = parser.isSet(repeated_code_option);
  bool isReqInv = parser.isSet(recursive_invariant_option);
  bool isParInv = parser.isSet(parlist_invariant_option);
  bool isIn = parser.isSet(inline_option);
  bool isWhere = parser.isSet(where_option);
  bool isWhat = parser.isSet(what_option);
  bool isGoal = parser.isSet(goal_option);

  // проверка опций
  if ((isDC + isEq + isRep + isReqInv + isParInv + isIn) != 1) {
    Qcout << "you need use signle optimization option";
    throw "bad option count";
  }

  // обработка флагов
  if (isDC || isEq || isRep || isReqInv) {
    QString path, filename;

    if (isDC)
      path = parser.value("dc");
    else if (isEq)
      path = parser.value("eq");
    else if (isRep)
      path = parser.value("rep");
    else if (isReqInv)
      path = parser.value("req_inv");

    dfg::DFG* dfg = dfg::import_dfg("", path);

    if (isDC) {
      dfg::optimization::dead_code(dfg);
      dfg::export_dfg(filename, dfg);
      delete dfg;
    }
    else if (isEq) {
      dfg::optimization::equivalent_transformations(dfg);
      dfg::export_dfg(filename, dfg);
      delete dfg;
    }
    else if (isRep) {
      dfg::optimization::repeated_code(dfg);
      dfg::export_dfg(filename, dfg);
      delete dfg;
    }
    else if (isReqInv) {
      dfg::DFG* helps_dfg;
      try {
        helps_dfg = dfg::optimization::recursive_invariant(dfg);
      }
      catch (...) {
        cout << "can't optimize\n";
        return -1;
      }

      dfg::export_dfg(helps_dfg->function_name, helps_dfg);
      dfg::export_dfg(dfg->function_name, dfg);

      Qcout << dfg->function_name << "\n";
      delete helps_dfg;
      delete dfg;
    }
  }

  if (isIn) {
    if (!isWhat || !isWhere) {
      Qcout << "bad command format. Use --what and --where options with --in.";
      throw "bad invariant format";
    }

    QString what = parser.value("what"),
            where = parser.value("where");

    dfg::DFG* dfg_what = dfg::import_dfg("", what);
    dfg::DFG* dfg_where = dfg::import_dfg("", where);

    dfg::optimization::inline_substitution(dfg_where, dfg_what);

    dfg::export_dfg(where, dfg_where);
    delete dfg_what;
    delete dfg_where;
  }

  if (isParInv) {
    if (!isGoal) {
      Qcout << "bad command format. Use --goal with --par_inv.";
      throw "bad invariant format";
    }

    QString goal = parser.value("goal");

    dfg::DFG* dfg_goal = dfg::import_dfg("", goal);
    std::vector<dfg::DFG*> dfgs;

    const QStringList positionalArguments = parser.positionalArguments();

    for (QString func : positionalArguments) {
      dfgs.push_back(dfg::import_dfg("", func));
    }

    std::vector<dfg::DFG*> opts = dfg::optimization::parlist_invariant(dfg_goal, dfgs);
    for (dfg::DFG* opt_dfg : opts) {

      dfg::export_dfg(opt_dfg->function_name, opt_dfg);

      Qcout << opt_dfg->function_name << "\n";
      delete opt_dfg;
    }
    dfg::export_dfg(dfg_goal->function_name, dfg_goal);
    delete dfg_goal;
  }
}
