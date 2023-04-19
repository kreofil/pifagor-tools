#include "rig2jsoner.h"
#include "../rigparser/rigparser.h"

int main(int argc, char* argv[]) {
    QTextStream cout(stdout);

    if (argc!= 3) {
        cout << "Incorrect number of arguments!" << endl;
        cout << "Use next format: rig2json <input rig file name> <output file name>"  << endl;
        return 0;
    }

    QString iFileName = argv[1];
    QString oFileName = argv[2];

    Rig rig;
    RigParser parser(&rig, iFileName);

    parser.Start();

    Rig2Jsoner rig2jsoner;
    rig2jsoner.Transform(&rig);

    rig2jsoner.Save(oFileName);
    return 0;
}

