#include "../cgparser/cgparser.h"
#include "cg2dot.h"


int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    QTextStream cout(stdout);
    QString iFileName;
    QString oFileName;
    QString gFileName;
    bool gFlag = false;

    QStringList args = app.arguments();
    int argListLen = args.length();

    if((argListLen < 2) || (argListLen > 3)) {
        cout << "Incorrect number of arguments!" << endl;
        cout << "Use next format: cg2dot [-png] <input cg file name>"  << endl;
        return 0;
    }

    if (argListLen == 2) {
        iFileName = args.at(1);;
        oFileName = iFileName + ".dot";
    } else if (argListLen == 3) {
        if(args.at(1) == QString("-png")) {
            gFlag = true;
            iFileName = args.at(2);
            oFileName = iFileName + ".dot";
            gFileName = iFileName + ".png";
        } else {
            cout << "Incorrect command format!" << endl;
            cout << "Use next format: cg2dot [-png] <input cg file name>"  << endl;
            return 0;
        }
    }

    //QTextStream cout(stdout);
    Cg cg;
    CgParser parser(&cg, iFileName);
    parser.Start();
    Cg2Dotter cg2dotter;
    QString dotText = cg2dotter.Cg2Dot(cg);

    //qDebug() << dotText;
    QFile fout(oFileName);
    if (!fout.open(QIODevice::WriteOnly | QIODevice::Text)) {
        cout << "Impossible to open file " << oFileName << " to write data." << endl;
        return -1;
    }

    QTextStream sout(&fout);
    sout << dotText;
    sout.flush();

    // Запуск внешнего процесса, если количество аргументов равно 3
    if(gFlag) {
        QProcess extProcess;
        QStringList arguments;
        arguments << "-Tpng" << oFileName << "-o" << gFileName;
        extProcess.startDetached("dot", arguments);
        //extProcess.start("dot", arguments);
        //extProcess.waitForFinished(-1);
    }
    return 0;
}


