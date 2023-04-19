#include "../rigparser/rigparser.h"

int main(int argc, char* argv[]) {
    QTextStream cout(stdout);
    Rig rig;

    if(argc != 3) {
        cout << "Incorrect number of arguments!" << endl;
        cout << "Use next format: rig2rig <input rig file name> <output rig file name>"  << endl;
        return 0;
    }

    QString iFileName = argv[1];
    QString oFileName = argv[2];

    RigParser parser(&rig, iFileName);

    parser.Start();

    //rig2rig.TestOut(cout);
    rig.SaveText(oFileName);
    return 0;
}
