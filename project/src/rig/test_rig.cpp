#include "rig.h"


int main() {
    QTextStream cout(stdout);
    FILE *out=fopen("test_rig_out.txt","w");
    QTextStream fout(out);

    Rig rig;
    rig.Append(new ExtRef(QString("Zero")));
    rig.Append(new ExtRef(QString("One")));
    rig.Append(new ExtRef(QString("Two")));
    rig.Append(new ExtRef(QString("Three")));

    rig.Append(new BoolValue(true));
    rig.Append(new BoolValue(false));
    rig.Append(new IntValue(10));
    rig.Append(new FloatValue(3.14195));
    rig.Append(new CharValue(QChar('Z')));
    rig.Append(new DataListValue(QString("String value")));
    rig.Append(new SpecValue(addSpec));
    rig.Append(new DelayValue(1,4));

    Actor* a;
    a = new InterpActor(loc, 2, ext, 1);
    a->SetDelay(0);
    a->pos.Set(10,20,30,40);
    rig.Append(a);
    a = new InterpActor(act, 0, act, 1);
    a->SetDelay(0);
    a->pos.Set(50,60,70,80);
    rig.Append(a);

    rig.TestOut(cout);
    rig.TestOut(fout);
    return 0;
}
