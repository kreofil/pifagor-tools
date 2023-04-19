#include <QtTest/QtTest>
#include <fstream>
#include <iostream>
#include <../inter2/alu.h>
#include "../repository/msgServer.h"

using namespace std;
Q_DECLARE_METATYPE(Value*)

class GeneralTester
{
public:
    void makeHead();
    void runTest();
};
void GeneralTester::makeHead()
{
    QTest::addColumn<QString>("funcName");
    QTest::addColumn<Value*>("argument");
    QTest::addColumn<QString>("answer");
}

//вызываем подготовленные тесты
void GeneralTester::runTest()
{
    MsgServer* msgServer=MsgServer::getMsgServer();
    msgServer->turnAll(false);

    QFETCH(QString, funcName);
    QFETCH(Value*,argument);
    QFETCH(QString, answer);

    //cout << qPrintable(funcName) << "(" << qPrintable(argument->GetAsStr()) << ")" << endl;

    CentralManager *cm;
    cm=new CentralManager();
    cm->Init((char*)qPrintable(funcName),argument);
    cm->Start();

    //QString finalRez=cm->GetFinalRez()->GetAsStr();

    QCOMPARE(cm->GetFinalRez()->GetAsStr(),answer);
}

class QTestInter2: public QObject
{
    Q_OBJECT
public:
private:
    GeneralTester generalTester;
private slots:
    void initTestCase();
    void cleanupTestCase();

    void testErrors_data();
    void testErrors();

    void testInter_data();
    void testInter();
    void testParList_data();
    void testParList();
    void testExtern_data();
    void testExtern();
    void testExternDelay_data();
    void testExternDelay();

    void testSpecPass_data();
    void testSpecPass();
    void testExternPass_data();
    void testExternPass();
    void testAsyncList_data();
    void testAsyncList();

    void testFact_data();
    void testFact();
    void testSort_data();
    void testSort();
    void testTailFact_data();
    void testTailFact();
    void testTailReverse_data();
    void testTailReverse();
    void testVecSum_data();
    void testVecSum();
    void testNdAbs_data();
    void testNdAbs();
    void testHanoy_data();
    void testHanoy();
    void testQuadr_data();
    void testQuadr();

    void testPointList_data();
    void testPointList();

    void test2Points_data();
    void test2Points();


    void testFilter();
    void testFilter_data();
    void testFolds();
    void testFolds_data();
    void testGenerate();
    void testGenerate_data();
    void testUncurry();
    void testUncurry_data();

    void testVecEquals();
    void testVecEquals_data();

    void testIssue6();
    void testIssue6_data();
    void testIssue9();
    void testIssue9_data();
    void testIssue18();
    void testIssue18_data();
    void testIssue22();
    void testIssue22_data();
    void testIssue23();
    void testIssue23_data();
    void testIssue24();
    void testIssue24_data();
    void testIssue26();
    void testIssue26_data();
    void testIssue28();
    void testIssue28_data();
    void testIssue29();
    void testIssue29_data();
    void testIssue30();
    void testIssue30_data();
    void testIssue35();
    void testIssue35_data();
    void testIssue37();
    void testIssue37_data();
    void testIssue38();
    void testIssue38_data();
    void testIssue39();
    void testIssue39_data();
};

//предварительная подготовка
//прекомпиляция тестовых pfg, получение РИГ и УГ
//TODO - меньше слов. Пусть ограничится "все хорошо" или "ошибка при трансляции того-то"
void QTestInter2::initTestCase()
{
    QProcess::execute("./trans2 -t ../src_pif/qtTests/extern.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/inter.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/parlist.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/delaylist.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/specPass.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/externPass.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/asynclist.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/RL_fact.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/RL_sort.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/RL_tailFact.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/RL_factLeftRec.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/RL_factTailRec.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/RL_tailReverse.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/RL_vecSum.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/errors.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/pointList.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/2points.pfg");

    QProcess::execute("./trans2 -t ../src_pif/qtTests/RL_ndAbs.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/RL_quadrEquat.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/RL_ndFacts.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/RL_hanoy.pfg");

    QProcess::execute("./trans2 -t ../src_pif/qtTests/predicates.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/filter.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/folds.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/generate.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/uncurry.pfg");

    QProcess::execute("./trans2 -t ../src_pif/qtTests/RL_vecEquals.pfg");

    QProcess::execute("./trans2 -t ../src_pif/qtTests/bugIssue6.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/bugIssue9.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/bugIssue18.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/bugIssue22.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/bugIssue23.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/bugIssue24.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/bugIssue26.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/bugIssue28.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/bugIssue29.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/bugIssue30.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/bugIssue35.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/bugIssue37.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/bugIssue38.pfg");
    QProcess::execute("./trans2 -t ../src_pif/qtTests/bugIssue39.pfg");

    QProcess::execute("./cgen2 -n qtTests.extern1 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.extern2 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.externMain1 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.externMain2 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.inter1 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.inter2 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.inter3 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.parlist1 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.parlist2 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.parlist3 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.parlist4.opener 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.parlist4 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.parlist5 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.delayOpener 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.delayReturner 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.delayMain1 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.delayMain2 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.passSpec 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.openSpec 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.passExtern 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.openExtern 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.summer 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.multer 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.asOpenerMain 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.asSummerMain 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.asSummer 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.asOpener 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.fact 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.sort.hoar.getsort 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.sort.hoar.getmind 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.sort.hoar.mind 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.tailFact 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.tailReverse 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.callTailReverse 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.errors.error1 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.errors.error2 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.easyPointTest 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.hardPointTest 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.my2points 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.isPosit 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.isNegat 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.isZero 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.isAny 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.filter.filter 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.filter.callFilter 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.folds.foldr 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.folds.foldl 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.generate.generate 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.generate.callGenerate 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.bigFunc 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.uncurry 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.callUncurry 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.VecSum.If 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.VecSum.Lambda01 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.VecSum.Lambda02 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.VecSum.Lambda03 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.VecSum.Lambda04 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.VecSum.Lambda05 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.VecSum.Lambda06 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.VecSum.PostInterp 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.VecSum.PrefInterp 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.VecSum.VecSum01 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.VecSum.VecSum02 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.VecSum.VecSum03 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.VecSum.VecSum04 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.VecSum.VecSum05 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.VecSum.VecSum06 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.FactLeftRec.FactLeftRec01 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.FactLeftRec.FactLeftRec02 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.FactLeftRec.Lambda02 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.FactTailRec.Fact01 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.FactTailRec.Fact02 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.FactTailRec.FactTailRec01 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.FactTailRec.FactTailRec02 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.FactTailRec.Lambda02 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.hanoy.H 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.hanoy.RecHanoy 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.hanoy.rHanoy 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.nonDelay.Abs 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.nonDelay.F01_4 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.nonDelay.F02_pos 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.nonDelay.fact01 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.nonDelay.fact01_rec 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.nonDelay.fact02 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.nonDelay.fact02_0 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.nonDelay.fact02_1 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.nonDelay.fact02_neg 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.nonDelay.fact02_pos 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.quadr.Abs 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.quadr.CalcSqrt 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.quadr.OneRoot 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.quadr.Sqrt 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.quadr.SqrtR 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.quadr.SquareRoot 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.quadr.TwoRoots 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.equals 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.vecEquals 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.issue6.equals 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue6.vec_equals 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue6.test_equals 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.issue9.emptyList 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue9.emptyListLen 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue9.nonEmptyList 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue9.nonEmptyListLen 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.issue18.division 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue18.main 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.issue22.equals 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue22.asynch 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue22.main 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.issue23.mod 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue23.mod1 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue23.notequal 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue23.func 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue23.get 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.issue24.numb 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.issue26.equals 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue26.numpos 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue26.main 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.issue28.main 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.issue29.form 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue29.main 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.issue30.min 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue30.main 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.issue35.main 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.issue37.abs 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue37.isClose 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue37.sqrt 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue37.main 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.issue38.inversion 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue38.main 2>log2.txt");

    QProcess::execute("./cgen2 -n qtTests.issue39.or 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue39.indschr 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue39.indsstr_no_intersections 2>log2.txt");
    QProcess::execute("./cgen2 -n qtTests.issue39.main 2>log2.txt");
}
//зачистка хвостов
//пока пусто
void QTestInter2::cleanupTestCase()
{

}

//тестовые примеры на интерпретацию
void QTestInter2::testInter_data()
{
    generalTester.makeHead();
    IntValue* i1=new IntValue(1);
    IntValue* i10=new IntValue(10);
    IntValue* i212=new IntValue(212);

    QTest::newRow("1+2") << "qtTests.inter1" << (Value*)i1 << "3";
    QTest::newRow("X*2 (1)") << "qtTests.inter2" << (Value*)i1 << "2";
    QTest::newRow("X*2 (10)") << "qtTests.inter2" << (Value*)i10 << "20";
    QTest::newRow("X*2 (212)") << "qtTests.inter2" << (Value*)i212 << "424";

    int mas[]={10,5};
    DataListValue* testDL = new DataListValue(mas);

    DataListValue* dl1=new DataListValue();
    dl1->Append(new IntValue(10));
    dl1->Append(new IntValue(5));
    QTest::newRow("X:(+,-,*,/);X=(10,5)") << "qtTests.inter3" << (Value*)dl1 << "(15,5,50,2.0)";
    DataListValue* dl2=new DataListValue();
    dl2->Append(new IntValue(20));
    dl2->Append(new IntValue(2));
    QTest::newRow("X:(+,-,*,/);X=(20,2)") << "qtTests.inter3" << (Value*)dl2 << "(22,18,40,10.0)";
}


//тестовые примеры на параллельные списки
void QTestInter2::testParList_data()
{
    generalTester.makeHead();

    IntValue* i1=new IntValue(1);
    QTest::newRow("(10,20):[+,-,*,/];") << "qtTests.parlist1" << (Value*)i1 << "[30,-10,200,0.5]";
    DataListValue* dl1=new DataListValue();
    dl1->Append(new IntValue(15));
    dl1->Append(new IntValue(3));
    QTest::newRow("X:[+,-,*,/];X=(15,3)") << "qtTests.parlist2" << (Value*)dl1 << "[18,12,45,5.0]";
    DataListValue* dl2=new DataListValue();
    dl2->Append(new IntValue(16));
    dl2->Append(new IntValue(8));
    QTest::newRow("X:[+,-,*,/];X=(16,8)") << "qtTests.parlist2" << (Value*)dl2 << "[24,8,128,2.0]";
    QTest::newRow("[(10,20),(30,40),(50,60)]:[+,-,*,/]") << "qtTests.parlist3" << (Value*)i1
                                                         << "[30,70,110,-10,-10,-10,200,1200,3000,0.5,0.75,0.833333333333]";
    QTest::newRow("opening newly parlists into parlists") << "qtTests.parlist4" << (Value*)dl1 << "[(1,2),(3,4),(3,4),(5,6)]";
    QTest::newRow("opening newly parlists into parlists 2") << "qtTests.parlist5" << (Value*)dl1 << "[1,2,3,4]";
}



//тестовые примеры на внешние функции
void QTestInter2::testExtern_data()
{
    generalTester.makeHead();

    IntValue* i1=new IntValue(10);
    QTest::newRow("extern call;2*X+5;X=10") << "qtTests.externMain1" << (Value*)i1 << "25";
    DataListValue* dl1=new DataListValue();
    dl1->Append(new IntValue(15));
    dl1->Append(new IntValue(3));
    QTest::newRow("extern call;2*X1*2*X2+15;X=(15,3)") << "qtTests.externMain2" << (Value*)dl1 << "195";
}



//тестовые примеры на передачу и возврат задержанных списков
void QTestInter2::testExternDelay_data()
{
    generalTester.makeHead();

    DataListValue* dl1=new DataListValue();
    dl1->Append(new IntValue(10));
    dl1->Append(new IntValue(1));
    QTest::newRow("qtTests.delayMain1(10,1)=2*10=20") << "qtTests.delayMain1" << (Value*)dl1 << "20";

    DataListValue* dl2=new DataListValue();
    dl2->Append(new IntValue(20));
    dl2->Append(new IntValue(2));
    QTest::newRow("qtTests.delayMain1(20,2)=20+3=23") << "qtTests.delayMain1" << (Value*)dl2 << "23";

    DataListValue* dl3=new DataListValue();
    dl3->Append(new IntValue(30));
    dl3->Append(new IntValue(1));
    QTest::newRow("qtTests.delayMain2(30,1)=3*30=90") << "qtTests.delayMain2" << (Value*)dl3 << "90";

    DataListValue* dl4=new DataListValue();
    dl4->Append(new IntValue(40));
    dl4->Append(new IntValue(2));
    QTest::newRow("qtTests.delayMain2(40,2)=40+5=45") << "qtTests.delayMain2" << (Value*)dl4 << "45";
}




//проверка передачи спек-операции в другую функцию
void QTestInter2::testSpecPass_data()
{
    generalTester.makeHead();

    DataListValue* dl1=new DataListValue();
    dl1->Append(new IntValue(1));
    dl1->Append(new IntValue(2));
    QTest::newRow("qtTests.passSpec(1,2)=1+2=3") << "qtTests.passSpec" << (Value*)dl1 << "3";

    DataListValue* dl2=new DataListValue();
    dl2->Append(new IntValue(10));
    dl2->Append(new IntValue(20));
    QTest::newRow("qtTests.passSpec(10,20)=10+20=30") << "qtTests.passSpec" << (Value*)dl2 << "30";
}



//проверка передачи экстерн-объекта в другую функцию
void QTestInter2::testExternPass_data()
{
    generalTester.makeHead();

    DataListValue* dl1=new DataListValue();
    dl1->Append(new IntValue(10));
    dl1->Append(new IntValue(20));
    QTest::newRow("qtTests.passExtern(10,20)=(10+20,10*20)=(30,200)") << "qtTests.passExtern" << (Value*)dl1 << "(30,200)";

    DataListValue* dl2=new DataListValue();
    dl2->Append(new IntValue(25));
    dl2->Append(new IntValue(40));
    QTest::newRow("qtTests.passExtern(25,40)=(25+40,25*40)=(65,1000)") << "qtTests.passExtern" << (Value*)dl2 << "(65,1000)";
}


void QTestInter2::test2Points_data()
{
    generalTester.makeHead();

    //голый инт подавать нельзя. Вообще кроме списка данных ничего нельзя
    IntValue* i1=new IntValue(10);
    QTest::newRow("10:..=BASEFUNCERROR") << "qtTests.my2points" << (Value*)i1 << "BASEFUNCERROR";

    //список данных длиной больше 3 элементов нельзя.
    DataListValue* dl1=new DataListValue();
    dl1->Append(new IntValue(10));
    dl1->Append(new IntValue(15));
    dl1->Append(new IntValue(10));
    dl1->Append(new IntValue(15));
    QTest::newRow("(10,15,10,15):..=BASEFUNCERROR") << "qtTests.my2points" << (Value*)dl1 << "BASEFUNCERROR";

    //2 флоата нельзя. Только 3.
    DataListValue* dl2=new DataListValue();
    dl2->Append(new FloatValue(10.5));
    dl2->Append(new FloatValue(15.2));
    QTest::newRow("(10.5,15.2):..=BASEFUNCERROR") << "qtTests.my2points" << (Value*)dl2 << "BASEFUNCERROR";

    //2 инта от большего к меньшему нельзя
    DataListValue* dl3=new DataListValue();
    dl3->Append(new IntValue(10));
    dl3->Append(new IntValue(5));
    QTest::newRow("(10,5):..=BOUNDERROR") << "qtTests.my2points" << (Value*)dl3 << "BOUNDERROR";

    //нормальный пример. Два инта, шаг по умолчанию 1.
    DataListValue* dl4=new DataListValue();
    dl4->Append(new IntValue(10));
    dl4->Append(new IntValue(15));
    QTest::newRow("(10,15):..=(10,11,12,13,14,15)") << "qtTests.my2points" << (Value*)dl4 << "(10,11,12,13,14,15)";

    //нормальный пример. Три инта, шаг положительный, диапазон ровный.
    DataListValue* dl5=new DataListValue();
    dl5->Append(new IntValue(10));
    dl5->Append(new IntValue(20));
    dl5->Append(new IntValue(2));
    QTest::newRow("(10,20,2):..=(10,12,14,16,18,20)") << "qtTests.my2points" << (Value*)dl5 << "(10,12,14,16,18,20)";

    //нормальный пример. Три инта, шаг положительный, диапазон неровный.
    DataListValue* dl6=new DataListValue();
    dl6->Append(new IntValue(10));
    dl6->Append(new IntValue(21));
    dl6->Append(new IntValue(2));
    QTest::newRow("(10,21,2):..=(10,12,14,16,18,20)") << "qtTests.my2points" << (Value*)dl6 << "(10,12,14,16,18,20)";

    //нормальный пример. Три инта, шаг отрицательный, диапазон ровный.
    DataListValue* dl7=new DataListValue();
    dl7->Append(new IntValue(20));
    dl7->Append(new IntValue(10));
    dl7->Append(new IntValue(-2));
    QTest::newRow("(20,10,-2):..=(20,18,16,14,12,10)") << "qtTests.my2points" << (Value*)dl7 << "(20,18,16,14,12,10)";

    //нормальный пример. Три инта, шаг отрицательный, диапазон неровный.
    DataListValue* dl8=new DataListValue();
    dl8->Append(new IntValue(20));
    dl8->Append(new IntValue(9));
    dl8->Append(new IntValue(-2));
    QTest::newRow("(20,9,-2):..=(20,18,16,14,12,10)") << "qtTests.my2points" << (Value*)dl8 << "(20,18,16,14,12,10)";

    //нормальный пример. Три флоата, шаг положительный, диапазон ровный.
    DataListValue* dl9=new DataListValue();
    dl9->Append(new FloatValue(10.5));
    dl9->Append(new FloatValue(19.5));
    dl9->Append(new FloatValue(1.5));
    QTest::newRow("(10.5,19.5,1.5):..=(10.5,12.0,13.5,15.0,16.5,18.0,19.5)")
            << "qtTests.my2points" << (Value*)dl9 << "(10.5,12.0,13.5,15.0,16.5,18.0,19.5)";

    //нормальный пример. Три флоата, шаг положительный, диапазон неровный.
    DataListValue* dl10=new DataListValue();
    dl10->Append(new FloatValue(10.5));
    dl10->Append(new FloatValue(20.5));
    dl10->Append(new FloatValue(1.5));
    QTest::newRow("(10.5,20.5,1.5):..=(10.5,12.0,13.5,15.0,16.5,18.0,19.5)")
            << "qtTests.my2points" << (Value*)dl10 << "(10.5,12.0,13.5,15.0,16.5,18.0,19.5)";

    //нормальный пример. Три флоата, шаг отрицательный, диапазон ровный.
    DataListValue* dl11=new DataListValue();
    dl11->Append(new FloatValue(19.5));
    dl11->Append(new FloatValue(10.5));
    dl11->Append(new FloatValue(-1.5));
    QTest::newRow("(19.5,10.5,-1.5):..=(19.5,18.0,16.5,15.0,13.5,12.0,10.5)")
            << "qtTests.my2points" << (Value*)dl11 << "(19.5,18.0,16.5,15.0,13.5,12.0,10.5)";

    //нормальный пример. Три флоата, шаг отрицательный, диапазон ровный.
    DataListValue* dl12=new DataListValue();
    dl12->Append(new FloatValue(19.5));
    dl12->Append(new FloatValue(9.5));
    dl12->Append(new FloatValue(-1.5));
    QTest::newRow("(19.5,9.5,-1.5):..=(19.5,18.0,16.5,15.0,13.5,12.0,10.5)")
            << "qtTests.my2points" << (Value*)dl12 << "(19.5,18.0,16.5,15.0,13.5,12.0,10.5)";

    //ненормальный пример. Три инта, шаг положительный, а должен быть отрицательный.
    DataListValue* dl13=new DataListValue();
    dl13->Append(new IntValue(20));
    dl13->Append(new IntValue(10));
    dl13->Append(new IntValue(2));
    QTest::newRow("(20,10,2):..=BOUNDERROR") << "qtTests.my2points" << (Value*)dl13 << "BOUNDERROR";

    //ненормальный пример. Три инта, шаг отрицательный, а должен быть положительный.
    DataListValue* dl14=new DataListValue();
    dl14->Append(new IntValue(10));
    dl14->Append(new IntValue(20));
    dl14->Append(new IntValue(-2));
    QTest::newRow("(10,20,-2):..=BOUNDERROR") << "qtTests.my2points" << (Value*)dl14 << "BOUNDERROR";

    //ненормальный пример. Три флоата, шаг положительный, а должен быть отрицательный.
    DataListValue* dl15=new DataListValue();
    dl15->Append(new FloatValue(20.5));
    dl15->Append(new FloatValue(10.5));
    dl15->Append(new FloatValue(2.5));
    QTest::newRow("(20.5,10.5,2.5):..=BOUNDERROR") << "qtTests.my2points" << (Value*)dl15 << "BOUNDERROR";

    //ненормальный пример. Три флоата, шаг отрицательный, а должен быть положительный.
    DataListValue* dl16=new DataListValue();
    dl16->Append(new FloatValue(10.5));
    dl16->Append(new FloatValue(20.5));
    dl16->Append(new FloatValue(-2.5));
    QTest::newRow("(10.5,20.5,-2.5):..=BOUNDERROR") << "qtTests.my2points" << (Value*)dl16 << "BOUNDERROR";

}




void QTestInter2::testAsyncList_data()
{
    generalTester.makeHead();

    IntValue* i1=new IntValue(1);
    QTest::newRow("qtTests.asOpenerMain=(1,(3,(2,(5,(4,(\"END\")))))))")
            << "qtTests.asOpenerMain"
            << (Value*)i1
            << "(1,(3,(2,(5,(4,(\"END\"))))))";
    QTest::newRow("qtTests.asSummerMain=15") << "qtTests.asSummerMain" << (Value*)i1 << "15";
}




//RL-тест: расчет факториала
void QTestInter2::testFact_data()
{
    generalTester.makeHead();

    QString str="X!;X=";
    int fact=1;
    IntValue* x;
    for(int i=1;i<10;i++)
    {
        x=new IntValue(i);
        fact*=i;
        QTest::newRow(qPrintable(str+QString::number(i))) << "qtTests.fact" << (Value*)x << QString::number(fact);
        QTest::newRow(qPrintable(str+QString::number(i)))
                << "qtTests.FactLeftRec.FactLeftRec01"
                << (Value*)x << QString::number(fact);
        QTest::newRow(qPrintable(str+QString::number(i)))
                << "qtTests.FactLeftRec.FactLeftRec02"
                << (Value*)x << QString::number(fact);
        QTest::newRow(qPrintable(str+QString::number(i)))
                << "qtTests.FactTailRec.Fact01"
                << (Value*)x << QString::number(fact);
        QTest::newRow(qPrintable(str+QString::number(i)))
                << "qtTests.FactTailRec.Fact02"
                << (Value*)x << QString::number(fact);

        QTest::newRow(qPrintable(str+QString::number(i)))
                << "qtTests.nonDelay.fact01"
                << (Value*)x << QString::number(fact);
        QTest::newRow(qPrintable(str+QString::number(i)))
                << "qtTests.nonDelay.fact02"
                << (Value*)x << QString::number(fact);
    }
}





//RL-тест: сортировка Хоара
void QTestInter2::testSort_data()
{
    generalTester.makeHead();

    DataListValue* dl1=new DataListValue();
    dl1->Append(new IntValue(1));
    dl1->Append(new IntValue(2));
    dl1->Append(new IntValue(3));
    dl1->Append(new IntValue(4));
    dl1->Append(new IntValue(5));
    dl1->Append(new IntValue(4));
    dl1->Append(new IntValue(3));
    dl1->Append(new IntValue(2));
    dl1->Append(new IntValue(1));
    QTest::newRow("sort") << "qtTests.sort.hoar.getsort" << (Value*)dl1 <<"(1,1,2,2,3,3,4,4,5)";
}



//RL-тест: хвостовой расчет факториала
void QTestInter2::testTailFact_data()
{
    generalTester.makeHead();

    QString str="X!;X=";
    int fact=1;
    DataListValue *dl;
    for(int i=1;i<10;i++)
    {
        dl=new DataListValue();
        dl->Append(new IntValue(i));
        dl->Append(new IntValue(1));
        fact*=i;
        QTest::newRow(qPrintable(str+QString::number(i))) << "qtTests.tailFact" << (Value*)dl << QString::number(fact);
    }
}


//RL-тест: хвостовой реверс списка
void QTestInter2::testTailReverse_data()
{
    generalTester.makeHead();

    const int maximum=10;
    DataListValue *dl1;
    DataListValue *dl2;
    dl1=new DataListValue();
    dl2=new DataListValue();

    for(int i=1;i<maximum;i++)
    {
        dl1->Append(new IntValue(i));
        dl2->Append(new IntValue(maximum-i));
    }

    QTest::newRow(qPrintable("listReverse"+dl1->GetAsStr())) << "qtTests.callTailReverse" << (Value*)dl1 << dl2->GetAsStr();
}

void QTestInter2::testErrors_data()
{
    generalTester.makeHead();

    IntValue* i1=new IntValue(1);
    QTest::newRow("qtTests.errors.error1=[1,2,3,4,5]") << "qtTests.errors.error1" << (Value*)i1 << "[1,2,3,4,5]";
    QTest::newRow("qtTests.errors.error2=[1,2,3,4,5]") << "qtTests.errors.error2" << (Value*)i1 << "[1,2,3,4,5]";
}



void QTestInter2::testFilter_data()
{
    generalTester.makeHead();

    QString str="X!;X=";
    DataListValue *dl,*posDl,*negDl,*zeroDl;
    ParListValue *result;
    dl=new DataListValue();
    posDl=new DataListValue();
    negDl=new DataListValue();
    zeroDl=new DataListValue();
    result=new ParListValue();
    for(int i=1;i<=5;i++)
    {
        dl->Append(new IntValue(i));
        dl->Append(new IntValue(-i));
        dl->Append(new IntValue(0));

        posDl->Append(new IntValue(i));
        negDl->Append(new IntValue(-i));
        zeroDl->Append(new IntValue(0));
    }
    result->Append(posDl);
    result->Append(negDl);
    result->Append(zeroDl);

    QTest::newRow(qPrintable("Filter(1,-1,0,...,5,-5,0)")) << "qtTests.filter.callFilter" << (Value*)dl << result->GetAsStr();
}



void QTestInter2::testFolds_data()
{
    generalTester.makeHead();

    DataListValue *dl,*list;
    dl=new DataListValue();
    list=new DataListValue();
    dl->Append(new SpecValue(slashSpec));
    dl->Append(new FloatValue(2.0));
    list->Append(new FloatValue(4.0));
    list->Append(new FloatValue(8.0));
    list->Append(new FloatValue(5.0));
    dl->Append(list);

    QTest::newRow(qPrintable("Foldr(/, 2.0, (4.0,8.0,5.0))")) << "qtTests.folds.foldr" << (Value*)dl << "1.25";
    QTest::newRow(qPrintable("Foldl(/, 2.0, (4.0,8.0,5.0))")) << "qtTests.folds.foldl" << (Value*)dl << "0.0125";
}



void QTestInter2::testGenerate_data()
{
    generalTester.makeHead();

    DataListValue *dl,*posDl,*negDl,*anyDl;
    ParListValue *result;
    dl=new DataListValue();
    posDl=new DataListValue();
    negDl=new DataListValue();
    anyDl=new DataListValue();
    result=new ParListValue();
    for(int i=-10;i<=10;i++)
    {
        if(i>0)
            posDl->Append(new IntValue(i));
        if(i<0)
            negDl->Append(new IntValue(i));
        anyDl->Append(new IntValue(i));
    }
    result->Append(posDl);
    result->Append(negDl);
    result->Append(anyDl);

    QTest::newRow(qPrintable("callGenerate")) << "qtTests.generate.callGenerate" << (Value*)posDl << result->GetAsStr();
}




void QTestInter2::testPointList_data()
{
    generalTester.makeHead();
    Value *x=new IntValue(0);
    QTest::newRow(qPrintable("easyPointTest")) << "qtTests.easyPointTest" << x << "(1,2,3)";
    QTest::newRow(qPrintable("hardPointTest")) << "qtTests.hardPointTest" << x << "(3,-2,-6)";
}



void QTestInter2::testUncurry_data()
{
    generalTester.makeHead();
    Value *x=new IntValue(0);
    QTest::newRow(qPrintable("callUncurry")) << "qtTests.callUncurry" << x << "5678";
}


void QTestInter2::testVecSum_data()
{
    generalTester.makeHead();
    DataListValue* dataList=new DataListValue();
    int sum=0;
    for(int i=1;i<10;i++)
    {
        dataList->Append(new IntValue(i));
        sum+=i;
    }
    QTest::newRow(qPrintable("VecSum01")) << "qtTests.VecSum.VecSum01" << (Value*)dataList << QString("%1").arg(sum);
    QTest::newRow(qPrintable("VecSum02")) << "qtTests.VecSum.VecSum02" << (Value*)dataList << QString("%1").arg(sum);
    QTest::newRow(qPrintable("VecSum03")) << "qtTests.VecSum.VecSum03" << (Value*)dataList << QString("%1").arg(sum);
    QTest::newRow(qPrintable("VecSum04")) << "qtTests.VecSum.VecSum04" << (Value*)dataList << QString("%1").arg(sum);
    QTest::newRow(qPrintable("VecSum05")) << "qtTests.VecSum.VecSum05" << (Value*)dataList << QString("%1").arg(sum);
    QTest::newRow(qPrintable("VecSum06")) << "qtTests.VecSum.VecSum06" << (Value*)dataList << QString("%1").arg(sum);
}



void QTestInter2::testNdAbs_data()
{
    generalTester.makeHead();
    IntValue* i1=new IntValue(10);
    IntValue* i2=new IntValue(-12);
    QTest::newRow(qPrintable("ndAbs(10)")) << "qtTests.nonDelay.Abs" << (Value*)i1 << QString("10");
    QTest::newRow(qPrintable("ndAbs(-12)")) << "qtTests.nonDelay.Abs" << (Value*)i2 << QString("12");
}

void QTestInter2::testHanoy_data()
{
    generalTester.makeHead();
    IntValue* x;
    QString name;
    QString rezults[]={"(0,2)",
                       "[(0,1),(0,2),(1,2)]",
                       "[(0,2),(0,1),(2,1),(0,2),(1,0),(1,2),(0,2)]"};
    for(int i=1;i<=3;i++)
    {
        x=new IntValue(i);
        name=QString("Hanoy(%1)").arg(i);
        QTest::newRow(qPrintable(name)) << "qtTests.hanoy.RecHanoy" << (Value*)x << rezults[i-1];
    }
}

void QTestInter2::testQuadr_data()
{
    generalTester.makeHead();
    DataListValue* d1=new DataListValue();
    DataListValue* d2=new DataListValue();
    d1->Append(new IntValue(1));
    d1->Append(new IntValue(-4));
    d1->Append(new IntValue(4));

    d2->Append(new IntValue(1));
    d2->Append(new IntValue(2));
    d2->Append(new IntValue(-35));

    QTest::newRow(qPrintable("x^2-4*x+4")) << "qtTests.quadr.SquareRoot" << (Value*)d1 << QString("2.0");
    QTest::newRow(qPrintable("x^2+2*x-35")) << "qtTests.quadr.SquareRoot" << (Value*)d2 << QString("(5.0,-7.0)");
}



void QTestInter2::testVecEquals_data()
{
    generalTester.makeHead();
    DataListValue* d1=new DataListValue();
    d1->Append(new IntValue(1));
    d1->Append(new IntValue(2));

    DataListValue* d2=new DataListValue();
    d2->Append(new IntValue(5));
    d2->Append(new IntValue(5));

    DataListValue *d3=new DataListValue();
    d3->Append(new IntValue(1));
    d3->Append(new IntValue(2));
    d3->Append(new IntValue(3));

    DataListValue *d4=new DataListValue();
    d4->Append(new IntValue(4));
    d4->Append(new IntValue(5));
    d4->Append(new IntValue(6));

    DataListValue *d5=new DataListValue();
    d5->Append(d3);
    d5->Append(d4);

    DataListValue *d6=new DataListValue();
    d6->Append(new IntValue(1));
    d6->Append(new IntValue(2));
    d6->Append(new IntValue(3));

    DataListValue *d7=new DataListValue();
    d7->Append(d3);
    d7->Append(d6);

    DataListValue *d8=new DataListValue();
    d8->Append(new IntValue(1));
    d8->Append(new IntValue(2));
    DataListValue *d9=new DataListValue();
    d9->Append(d8);
    d9->Append(new IntValue(3));

    DataListValue *d10=new DataListValue();
    d10->Append(new IntValue(2));
    d10->Append(new IntValue(3));
    DataListValue *d11=new DataListValue();
    d11->Append(new IntValue(1));
    d11->Append(d10);

    DataListValue *d12=new DataListValue();
    d12->Append(d9);
    d12->Append(d11);

    DataListValue *str1 = new DataListValue("hello");
    DataListValue *str2 = new DataListValue("hello");
    DataListValue *str3 = new DataListValue("hodor");
    DataListValue *d13=new DataListValue();
    d13->Append(str1);
    d13->Append(str2);

    DataListValue *d14=new DataListValue();
    d14->Append(str1);
    d14->Append(str3);

    QTest::newRow(qPrintable("non equal numbers")) << "qtTests.equals" << (Value*)d1 << QString("false");
    QTest::newRow(qPrintable("equal numbers")) << "qtTests.equals" << (Value*)d2 << QString("true");
    QTest::newRow(qPrintable("non equal lists")) << "qtTests.equals" << (Value*)d5 << QString("false");
    QTest::newRow(qPrintable("equal lists")) << "qtTests.equals" << (Value*)d7 << QString("true");
    QTest::newRow(qPrintable("lists with different structure")) << "qtTests.equals" << (Value*)d12 << QString("false");
    QTest::newRow(qPrintable("equal strings")) << "qtTests.equals" << (Value*)d13 << QString("true");
    QTest::newRow(qPrintable("non equal strings")) << "qtTests.equals" << (Value*)d14 << QString("false");
}


void QTestInter2::testIssue6_data()
{
    generalTester.makeHead();
    Value *x=new IntValue(0);
    QString issue6expected="((true,(\"numbers_equal\")),(true,(\"numbers_not_equal\")),(true,(\"flatten_list_not_equal\")),(true,(\"flatten_list_equal\")),(true,(\"list_structure_not_equal\")),(true,(\"strings equals\")),(true,(\"strings not equals\")))";
    QTest::newRow(qPrintable("issue 6")) << "qtTests.issue6.test_equals" << x << issue6expected;
}


void QTestInter2::testIssue9_data()
{
    generalTester.makeHead();
    Value *x=new IntValue(0);
    QTest::newRow(qPrintable("issue 9 empty list")) << "qtTests.issue9.emptyList" << x << "(.)";
    QTest::newRow(qPrintable("issue 9 empty list length")) << "qtTests.issue9.emptyListLen" << x << "0";
    QTest::newRow(qPrintable("issue 9 not empty list")) << "qtTests.issue9.nonEmptyList" << x << "(1,2)";
    QTest::newRow(qPrintable("issue 9 not empty list length")) << "qtTests.issue9.nonEmptyListLen" << x << "2";
}



void QTestInter2::testIssue18_data()
{
    generalTester.makeHead();

    IntValue* i1=new IntValue(8);
    IntValue* i2=new IntValue(3);
    IntValue* i3=new IntValue(2);
    IntValue* i4=new IntValue(9);
    DataListValue* dl1=new DataListValue();
    dl1->Append(i1);
    dl1->Append(i2);
    DataListValue* dl2=new DataListValue();
    dl2->Append(i3);
    dl2->Append(i4);
    DataListValue* dl3=new DataListValue();
    dl3->Append(dl1);
    dl3->Append(dl2);
    QTest::newRow(qPrintable("two lists")) << "qtTests.issue18.division" << (Value*)dl3 << "BASEFUNCERROR";

    BoolValue* b1=new BoolValue(true);
    DataListValue* dl4=new DataListValue();
    dl4->Append(i3);
    dl4->Append(b1);
    QTest::newRow(qPrintable("with two different types")) << "qtTests.issue18.division" << (Value*)dl4 << "BASEFUNCERROR";

    CharValue* c1=new CharValue('a');
    CharValue* c2=new CharValue('d');
    DataListValue* dl5=new DataListValue();
    dl5->Append(c1);
    dl5->Append(c2);
    QTest::newRow(qPrintable("with two chars")) << "qtTests.issue18.division" << (Value*)dl5 << "BASEFUNCERROR";


    DataListValue* dl6=new DataListValue();
    dl6->Append(c1);
    DataListValue* dl7=new DataListValue();
    dl7->Append(c2);
    DataListValue* dl8=new DataListValue();
    dl8->Append(dl6);
    dl8->Append(dl7);
    QTest::newRow(qPrintable("with two strings")) << "qtTests.issue18.division" << (Value*)dl8 << "BASEFUNCERROR";

    IntValue* i5=new IntValue(3);
    IntValue* i6=new IntValue(0);
    DataListValue* dl9=new DataListValue();
    dl9->Append(i5);
    dl9->Append(i6);
    QTest::newRow(qPrintable("division by zero")) << "qtTests.issue18.division" << (Value*)dl9 << "ZERODIVIDE";

    QTest::newRow(qPrintable("same, but in code")) << "qtTests.issue18.main" << (Value*)dl9 << "[true,true,true,true]";
}


void QTestInter2::testIssue22_data()
{
    generalTester.makeHead();
    CharValue* a1 = new CharValue('a');

    QTest::newRow(qPrintable("issue 22 five test pack")) << "qtTests.issue22.main" << (Value*)a1
                                                         << "[true,true,false,false,false]";
}

void QTestInter2::testIssue23_data()
{
    generalTester.makeHead();
    CharValue* a1 = new CharValue('a');
    CharValue* b = new CharValue('b');
    CharValue* a2 = new CharValue('a');
    DataListValue *dl1=new DataListValue();
    dl1->Append(a1);
    dl1->Append(b);

    DataListValue *dl2=new DataListValue();
    dl1->Append(a1);
    dl1->Append(a2);

    QTest::newRow(qPrintable("issue 23 chars: a!=b")) << "qtTests.issue23.notequal" << (Value*)dl1 << "true";
    QTest::newRow(qPrintable("issue 23 chars: a==a")) << "qtTests.issue23.notequal" << (Value*)dl2 << "false";
    QTest::newRow(qPrintable("issue 23 funcs")) << "qtTests.issue23.func" << (Value*)dl2 << "[true,true,true,true,true,true]";
}


void QTestInter2::testIssue24_data()
{
    generalTester.makeHead();
    IntValue* i1=new IntValue(2);
    IntValue* i2=new IntValue(3);
    IntValue* i3=new IntValue(4);
    IntValue* i4=new IntValue(6);

    IntValue* i5=new IntValue(0);
    IntValue* i6=new IntValue(4);
    IntValue* i7=new IntValue(-3);
    DataListValue* dl1=new DataListValue();
    dl1->Append(i1);
    dl1->Append(i2);
    dl1->Append(i3);
    dl1->Append(i4);

    DataListValue* dl2=new DataListValue();
    dl2->Append(dl1);
    dl2->Append(i5);

    DataListValue* dl3=new DataListValue();
    dl3->Append(dl1);
    dl3->Append(i6);

    DataListValue* dl4=new DataListValue();
    dl4->Append(dl1);
    dl4->Append(i7);

    QTest::newRow(qPrintable("issue 24: (2,3,4,6):0=.")) << "qtTests.issue24.numb" << (Value*)dl2 << ".";
    QTest::newRow(qPrintable("issue 24: (2,3,4,6):4=6")) << "qtTests.issue24.numb" << (Value*)dl3 << "6";
    QTest::newRow(qPrintable("issue 24: (2,3,4,6):-3=(2,3,6)")) << "qtTests.issue24.numb" << (Value*)dl4 << "(2,3,6)";
}


void QTestInter2::testIssue26_data()
{
    generalTester.makeHead();
    CharValue* a1 = new CharValue('a');

    QTest::newRow(qPrintable("issue 26 test")) << "qtTests.issue26.main" << (Value*)a1
                                               << "[(1,3,6),(.)]";
}
void QTestInter2::testIssue28_data()
{
    generalTester.makeHead();
    CharValue* a1 = new CharValue('a');

    QTest::newRow(qPrintable("issue 28 test")) << "qtTests.issue28.main" << (Value*)a1
                                               << "(TYPEERROR,int,TYPEERROR)";
}

void QTestInter2::testIssue29_data()
{
    generalTester.makeHead();
    CharValue* a1 = new CharValue('a');

    QTest::newRow(qPrintable("issue 29 test")) << "qtTests.issue29.main" << (Value*)a1
                                               << "[BASEFUNCERROR,BASEFUNCERROR,BASEFUNCERROR,BOUNDERROR,BASEFUNCERROR,(1),(1.5)]";
}

void QTestInter2::testIssue30_data()
{
    generalTester.makeHead();
    CharValue* a1 = new CharValue('a');

    QTest::newRow(qPrintable("issue 30 test")) << "qtTests.issue30.main" << (Value*)a1
                                               << "[1,0.01,2]";
}

void QTestInter2::testIssue35_data()
{
    generalTester.makeHead();
    CharValue* a1 = new CharValue('a');

    QTest::newRow(qPrintable("issue 35 test")) << "qtTests.issue35.main" << (Value*)a1
                                               << "0.999996000016";
}
void QTestInter2::testIssue37_data()
{
    generalTester.makeHead();
    CharValue* a1 = new CharValue('a');

    QTest::newRow(qPrintable("issue 37 test")) << "qtTests.issue37.main" << (Value*)a1
                                               << "[true,true,true,true]";
}
void QTestInter2::testIssue38_data()
{
    generalTester.makeHead();
    CharValue* a1 = new CharValue('a');

    QTest::newRow(qPrintable("issue 38 test")) << "qtTests.issue38.main" << (Value*)a1
                                               << "[(\"A\"),(\"CBA\"),(\"FEDCBA\")]";
}

void QTestInter2::testIssue39_data()
{
    generalTester.makeHead();
    CharValue* a1 = new CharValue('a');

    QTest::newRow(qPrintable("issue 39 test")) << "qtTests.issue39.main" << (Value*)a1
                                               << "[(1),(4),(1,5)]";
}


//запускалки тестов.
//Да, они все одинаковые.
//надо это обдумать
void QTestInter2::testInter()
{
    generalTester.runTest();
}
void QTestInter2::testParList()
{
    generalTester.runTest();
}
void QTestInter2::testExtern()
{
    generalTester.runTest();
}
void QTestInter2::testExternDelay()
{
    generalTester.runTest();
}
void QTestInter2::testSpecPass()
{
    generalTester.runTest();
}
void QTestInter2::testExternPass()
{
    generalTester.runTest();
}
void QTestInter2::test2Points()
{
    generalTester.runTest();
}
void QTestInter2::testAsyncList()
{
    generalTester.runTest();
}
void QTestInter2::testFact()
{
    generalTester.runTest();
}
void QTestInter2::testSort()
{
    generalTester.runTest();
}
void QTestInter2::testTailFact()
{
    generalTester.runTest();
}
void QTestInter2::testTailReverse()
{
    generalTester.runTest();
}
void QTestInter2::testErrors()
{
    generalTester.runTest();
}
void QTestInter2::testFilter()
{
    generalTester.runTest();
}
void QTestInter2::testFolds()
{
    generalTester.runTest();
}
void QTestInter2::testGenerate()
{
    generalTester.runTest();
}
void QTestInter2::testPointList()
{
    generalTester.runTest();
}
void QTestInter2::testUncurry()
{
    generalTester.runTest();
}
void QTestInter2::testVecSum()
{
    generalTester.runTest();
}
void QTestInter2::testNdAbs()
{
    generalTester.runTest();
}
void QTestInter2::testHanoy()
{
    generalTester.runTest();
}
void QTestInter2::testQuadr()
{
    generalTester.runTest();
}
void QTestInter2::testVecEquals()
{
    generalTester.runTest();
}
void QTestInter2::testIssue6()
{
    generalTester.runTest();
}
void QTestInter2::testIssue9()
{
    generalTester.runTest();
}
void QTestInter2::testIssue18()
{
    generalTester.runTest();
}
void QTestInter2::testIssue22()
{
    generalTester.runTest();
}
void QTestInter2::testIssue23()
{
    generalTester.runTest();
}
void QTestInter2::testIssue24()
{
    generalTester.runTest();
}
void QTestInter2::testIssue26()
{
    generalTester.runTest();
}
void QTestInter2::testIssue28()
{
    generalTester.runTest();
}
void QTestInter2::testIssue29()
{
    generalTester.runTest();
}
void QTestInter2::testIssue30()
{
    generalTester.runTest();
}
void QTestInter2::testIssue35()
{
    generalTester.runTest();
}
void QTestInter2::testIssue37()
{
    generalTester.runTest();
}
void QTestInter2::testIssue38()
{
    generalTester.runTest();
}
void QTestInter2::testIssue39()
{
    generalTester.runTest();
}
QTEST_MAIN(QTestInter2)

#include "qtTestsInter2.moc"
