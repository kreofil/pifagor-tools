#include "../cg/cg.h"
#include "../rigparser/rigparser.h"

int main(int argc, char* argv[]) {
    QTextStream cout(stdout);

    if((argc < 2) || (argc > 3)) {
        cout << "Incorrect number of arguments!" << endl;
        cout << "Use next format: rig2cg <input rig file name> [<output cg file name>]"  << endl;
        return 0;
    }

    QString iFileName = argv[1];
    QString oFileName = argv[2];
    iFileName = argv[1];
    if (argc == 2) {
        QStringList strList = iFileName.split(".");
        int size = strList.size();
        if(size > 0) {
            QString tmpStr = strList[size-1];
            if(tmpStr == "rig") {
                strList[size-1] = "cg";
            }
            oFileName = strList.join(".");
        } else {
            oFileName = iFileName + ".cg";
        }
    } else { // if (argc == 3) {
        oFileName = argv[2];
    }

    Rig rig;
    RigParser parser(&rig, iFileName);

    parser.Start();
    //rig2cg.TestOut(cout);

    Cg cg;
    cg.Transform(&rig);

    //cg.TestOut(cout);
    cg.SaveText(oFileName);
    return 0;
}

