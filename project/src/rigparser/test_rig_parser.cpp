#include "rigparser.h"

int main(int argc, char* argv[]) {
    QTextStream cout(stdout);

    if(argc != 2) {
        cout << "Incorrect number of arguments!" << endl;
        cout << "Use next format: test_rig_parser <input rig file name>"  << endl;
        return 0;
    }

    Rig rig;
    RigParser parser(&rig, QString(argv[1]));
    parser.Start();

    /////parser.TestOut();
    //rig.TestOut(cout);

    QString oFileName = QString(argv[1]) + "-rig_parser_test.txt";

    QFile fout(oFileName);
    if (!fout.open(QIODevice::WriteOnly | QIODevice::Text)) {
        cout << "Impossible to open file " << oFileName << " to write data." << endl;
        return -1;
    }
    QTextStream sout(&fout);
    rig.TestOut(sout);

    return 0;
}
