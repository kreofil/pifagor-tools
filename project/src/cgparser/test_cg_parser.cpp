#include "cgparser.h"

int main(int argc, char* argv[]) {
    QTextStream cout(stdout);

    if(argc != 2) {
        cout << "Incorrect number of arguments!" << endl;
        cout << "Use next format: test_cg_parser <input cg file name>"  << endl;
        return 0;
    }

    Cg cg;
    CgParser parser(&cg, QString(argv[1]));
    parser.Start();

    /////parser.TestOut();
    //cg.TestOut(cout);

    QString oFileName = QString(argv[1]) + "-cg_parser_test.txt";

    QFile fout(oFileName);
    if (!fout.open(QIODevice::WriteOnly | QIODevice::Text)) {
        cout << "Impossible to open file " << oFileName << " to write data." << endl;
        return -1;
    }
    QTextStream sout(&fout);
    cg.TestOut(sout);

    return 0;
}

