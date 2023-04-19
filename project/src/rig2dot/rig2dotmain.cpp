#include <QProcess>

#include "../rigparser/rigparser.h"
#include "rig2dot.h"

int main(int argc, char* argv[]) {
    QTextStream cout(stdout);
    QString iFileName;
    QString oFileName;
    QString gFileName;
    bool gFlag = false;

    if((argc < 2) || (argc > 3)) {
        cout << "Incorrect number of arguments!" << endl;
        cout << "Use next format: rig2dot [-png] <input rig file name>"  << endl;
        return 0;
    }

    if (argc == 2) {
        iFileName = argv[1];
        oFileName = iFileName + ".dot";
    } else if (argc == 3) {
        if(QString(argv[1]) == QString("-png")) {
            gFlag = true;
            iFileName = argv[2];
            oFileName = iFileName + ".dot";
            gFileName = iFileName + ".png";
        } else {
            cout << "Incorrect command format!" << endl;
            cout << "Use next format: rig2dot [-png] <input rig file name>"  << endl;
            return 0;
        }
    }

    Rig rig;
    RigParser parser(&rig, iFileName);
    parser.Start();
    Rig2Dotter rig2dotter;
    QString dotText = rig2dotter.Rig2Dot(&rig);

    //qDebug() << dotText;
    QFile fout(oFileName);
    if (!fout.open(QIODevice::WriteOnly | QIODevice::Text)) {
        cout << "Impossible to open file " << oFileName << " to write data." << endl;
        return -1;
    }

    QTextStream sout(&fout);
    sout << dotText;
    sout.flush();
    //rig2dot.Save(QString("rig2dot_test.dot"));

    // Запуск внешнего процесса, если количество аргументов равно 3
    if(gFlag) {
        QProcess extProcess;
        QStringList arguments;
        arguments << "-Tpng" << oFileName << "-o" << gFileName;
        extProcess.startDetached("dot", arguments);
        //extProcess.start("dot", arguments);
        //extProcess.waitForFinished(-1);
        //dot -Tpng rig2dot_test.dot  -o 1.rig.png
    }

    return 0;
}

