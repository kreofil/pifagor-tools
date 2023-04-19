#include "../cgparser/cgparser.h"

int main(int argc, char* argv[]) {
    QTextStream cout(stdout);

    if(argc != 3) {
        cout << "Incorrect number of arguments!" << endl;
        cout << "Use next format: cg2cg <input cg file name> <output cg file name>"  << endl;
        return 0;
    }

    QString iFileName = argv[1];
    QString oFileName = argv[2];

    Cg cg;
    CgParser parser(&cg, iFileName);

    parser.Start();

    //cg2cg.TestOut(cout);
    cg.SaveText(oFileName);
    return 0;
}

