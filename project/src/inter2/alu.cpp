#include "alu.h"

//функция АЛУ
//TODO ALU1 переписать это позорище
Value* ALU::Interpret(Value* func, Value* arg, DataWorker* caller,int nodeNum, int posIndex)
{
    Value *rez=NULL;
    IntValue* intArg;
    FloatValue* floatArg;
    CharValue* charArg;
    BoolValue* boolArg;
    DataListValue* dataListArg;
    AsyncListValue* asyncListArg;
    ParListValue* parListArg;
    SpecValue* specFunc;
    DelayValue* delayAnsw;
    ValueType argType,funcType;
    QVector<Value*> elements;
    QVector<Value*> transElements;
    qDebug() << "Interpreter called!";
    qDebug() << "Function:\t" << func->GetAsStr();
    qDebug() << "Argument:\t" << arg->GetAsStr();

    if(arg==NULL || func==NULL)
        return new NullValue();

    argType=arg->GetValueType();
    funcType=func->GetValueType();

    if(argType==NullValueType || funcType==NullValueType)
        return new NullValue();

    // приводим аргумент к нужному типу (для упрощения читаемости дальнейшего кода)
    switch (argType)
    {
        case IntValueType:
            intArg=(IntValue*)arg;
            break;
        case FloatValueType:
            floatArg=(FloatValue*)arg;
            break;
        case CharValueType:
            charArg=(CharValue*)arg;
            break;
        case BoolValueType:
            boolArg=(BoolValue*)arg;
            break;
        case DataListValueType:
            dataListArg=(DataListValue*)arg;
            elements=*(dataListArg->GetElementList());
            break;
        case ParListValueType:
            parListArg=(ParListValue*)arg;
            elements=*(parListArg->GetElementList());
            break;
        case AsyncListValueType:
            asyncListArg=(AsyncListValue*)arg;
            break;
        default:
            break;
    }
    if (argType==DelayValueType || funcType==DelayValueType)    //раскрываем задержанный список
    {
        delayAnsw=new DelayValue(-1,-1);
        return delayAnsw;
    }
    if (argType==MockValueType)
    {
        return new MockValue();
    }
    if(funcType==SpecValueType)             // вызов спецоперации
    {
        specFunc=(SpecValue*)func;
        qDebug() << "Func is Spec of" << specFunc->GetAddr();
        switch(specFunc->GetSpecValue())
        {
            case dotSpec:                   // .
            {
                return arg;
                break;
            }
            case addSpec:                   // +
            {
                switch (argType)
                {
                    case IntValueType:
                        return new IntValue(intArg->GetIntValue());
                    case FloatValueType:
                        return new FloatValue(floatArg->GetFloatValue());
                    case DataListValueType:
                        if(elements.size()!=2)
                            return new ErrorValue(BASEFUNCERROR);
                        Value *elem1,*elem2;
                        elem1=elements[0];
                        elem2=elements[1];
                        if(elem1->GetValueType()==BoolValueType && elem2->GetValueType()==BoolValueType)
                        {
                            bool bvalue1,bvalue2;
                            bvalue1=((BoolValue*)elem1)->GetBoolValue();
                            bvalue2=((BoolValue*)elem2)->GetBoolValue();
                            return new BoolValue(bvalue1||bvalue2);
                        }
                        if(elem1->GetValueType()==IntValueType)
                        {
                            int ivalue1=((IntValue*)elem1)->GetIntValue();
                            if(elem2->GetValueType()==IntValueType)
                            {
                                int ivalue2=((IntValue*)elem2)->GetIntValue();
                                return new IntValue(ivalue1+ivalue2);
                            }
                            if(elem2->GetValueType()==FloatValueType)
                            {
                                double fvalue2=((FloatValue*)elem2)->GetFloatValue();
                                return new FloatValue(ivalue1+fvalue2);
                            }
                            return new ErrorValue(BASEFUNCERROR);
                        }

                        if(elem1->GetValueType()==FloatValueType)
                        {
                            double fvalue1=((FloatValue*)elem1)->GetFloatValue();
                            if(elem2->GetValueType()==IntValueType)
                            {
                                int ivalue2=((IntValue*)elem2)->GetIntValue();
                                return new FloatValue(fvalue1+ivalue2);
                            }
                            if(elem2->GetValueType()==FloatValueType)
                            {
                                double fvalue2=((FloatValue*)elem2)->GetFloatValue();
                                return new FloatValue(fvalue1+fvalue2);
                            }
                            return new ErrorValue(BASEFUNCERROR);
                        }
                        return new ErrorValue(BASEFUNCERROR);
                    default:
                        return new ErrorValue(BASEFUNCERROR);
                }
            }
            case minusSpec:                 // -
            {
                switch (argType)
                {
                    case IntValueType:
                        return new IntValue(-(intArg->GetIntValue()));
                    case FloatValueType:
                        return new FloatValue(-(floatArg->GetFloatValue()));
                    case BoolValueType:
                        return new BoolValue(!(boolArg->GetBoolValue()));
                    case DataListValueType:
                        if(elements.size()!=2)
                            return new ErrorValue(BASEFUNCERROR);
                        Value *elem1,*elem2;
                        elem1=elements[0];
                        elem2=elements[1];
                        if(elem1->GetValueType()==BoolValueType && elem2->GetValueType()==BoolValueType)
                        {
                            bool bvalue1,bvalue2;
                            bvalue1=((BoolValue*)elem1)->GetBoolValue();
                            bvalue2=((BoolValue*)elem2)->GetBoolValue();
                            return new BoolValue(bvalue1!=bvalue2);
                        }
                        if(elem1->GetValueType()==IntValueType)
                        {
                            int ivalue1=((IntValue*)elem1)->GetIntValue();
                            if(elem2->GetValueType()==IntValueType)
                            {
                                int ivalue2=((IntValue*)elem2)->GetIntValue();
                                return new IntValue(ivalue1-ivalue2);
                            }
                            if(elem2->GetValueType()==FloatValueType)
                            {
                                double fvalue2=((FloatValue*)elem2)->GetFloatValue();
                                return new FloatValue(ivalue1-fvalue2);
                            }
                            return new ErrorValue(BASEFUNCERROR);
                        }
                        if(elem1->GetValueType()==FloatValueType)
                        {
                            double fvalue1=((FloatValue*)elem1)->GetFloatValue();
                            if(elem2->GetValueType()==IntValueType)
                            {
                                int ivalue2=((IntValue*)elem2)->GetIntValue();
                                return new FloatValue(fvalue1-ivalue2);
                            }
                            if(elem2->GetValueType()==FloatValueType)
                            {
                                double fvalue2=((FloatValue*)elem2)->GetFloatValue();
                                return new FloatValue(fvalue1-fvalue2);
                            }
                            return new ErrorValue(BASEFUNCERROR);
                        }
                        return new ErrorValue(BASEFUNCERROR);
                    default:
                        return new ErrorValue(BASEFUNCERROR);
                }
            }
            case astSpec:                   // *
            {
                if(argType!=DataListValueType)
                    return new ErrorValue(BASEFUNCERROR);
                if(elements.size()!=2)
                    return new ErrorValue(BASEFUNCERROR);
                Value *elem1,*elem2;
                elem1=elements[0];
                elem2=elements[1];
                if(elem1->GetValueType()==BoolValueType && elem2->GetValueType()==BoolValueType)
                {
                    bool bvalue1,bvalue2;
                    bvalue1=((BoolValue*)elem1)->GetBoolValue();
                    bvalue2=((BoolValue*)elem2)->GetBoolValue();
                    return new BoolValue(bvalue1&&bvalue2);
                }
                if(elem1->GetValueType()==IntValueType)
                {
                    int ivalue1=((IntValue*)elem1)->GetIntValue();
                    if(elem2->GetValueType()==IntValueType)
                    {
                        int ivalue2=((IntValue*)elem2)->GetIntValue();
                        return new IntValue(ivalue1*ivalue2);
                    }
                    if(elem2->GetValueType()==FloatValueType)
                    {
                        double fvalue2=((FloatValue*)elem2)->GetFloatValue();
                        return new FloatValue(ivalue1*fvalue2);
                    }
                    return new ErrorValue(BASEFUNCERROR);
                }
                if(elem1->GetValueType()==FloatValueType)
                {
                    double fvalue1=((FloatValue*)elem1)->GetFloatValue();
                    if(elem2->GetValueType()==IntValueType)
                    {
                        int ivalue2=((IntValue*)elem2)->GetIntValue();
                        return new FloatValue(fvalue1*ivalue2);
                    }
                    if(elem2->GetValueType()==FloatValueType)
                    {
                        double fvalue2=((FloatValue*)elem2)->GetFloatValue();
                        return new FloatValue(fvalue1*fvalue2);
                    }
                    return new ErrorValue(BASEFUNCERROR);
                }
                return new ErrorValue(BASEFUNCERROR);
            }
            case slashSpec:                 // /
            {
                if(argType!=DataListValueType)
                    return new ErrorValue(BASEFUNCERROR);
                if(elements.size()!=2)
                    return new ErrorValue(BASEFUNCERROR);
                Value *elem1,*elem2;
                elem1=elements[0];
                elem2=elements[1];
                if(elem1->GetValueType()==IntValueType)
                {
                    int ivalue1=((IntValue*)elem1)->GetIntValue();
                    if(elem2->GetValueType()==IntValueType)
                    {
                        int ivalue2=((IntValue*)elem2)->GetIntValue();
                        if(ivalue2==0)
                            return new ErrorValue(ZERODIVIDE);
                        return new FloatValue(double(ivalue1)/ivalue2);
                    }
                    if(elem2->GetValueType()==FloatValueType)
                    {
                        double fvalue2=((FloatValue*)elem2)->GetFloatValue();
                        if(fvalue2==0)
                            return new ErrorValue(ZERODIVIDE);
                        return new FloatValue(ivalue1/fvalue2);
                    }
                    return new ErrorValue(BASEFUNCERROR);
                }
                if(elem1->GetValueType()==FloatValueType)
                {
                    double fvalue1=((FloatValue*)elem1)->GetFloatValue();
                    if(elem2->GetValueType()==IntValueType)
                    {
                        int ivalue2=((IntValue*)elem2)->GetIntValue();
                        if(ivalue2==0)
                            return new ErrorValue(ZERODIVIDE);
                        return new FloatValue(fvalue1/ivalue2);
                    }
                    if(elem2->GetValueType()==FloatValueType)
                    {
                        double fvalue2=((FloatValue*)elem2)->GetFloatValue();
                        if(fvalue2==0)
                            return new ErrorValue(ZERODIVIDE);
                        return new FloatValue(fvalue1/fvalue2);
                    }
                    return new ErrorValue(BASEFUNCERROR);
                }
                return new ErrorValue(BASEFUNCERROR);
            }
            case percentSpec:               // %
            {
                if(argType!=DataListValueType)
                    return new ErrorValue(BASEFUNCERROR);
                if(elements.size()!=2)
                    return new ErrorValue(BASEFUNCERROR);
                if(elements[0]->GetValueType()!=IntValueType || elements[1]->GetValueType()!=IntValueType)
                    return new ErrorValue(BASEFUNCERROR);
                IntValue *ielem1,*ielem2;
                ielem1=(IntValue*)elements[0];
                ielem2=(IntValue*)elements[1];
                int ivalue1,ivalue2;
                ivalue1=ielem1->GetIntValue();
                ivalue2=ielem2->GetIntValue();
                if(ivalue2==0)
                    return new ErrorValue(ZERODIVIDE);
                rez=new DataListValue();
                ((DataListValue*)rez)->Append(new IntValue(ivalue1/ivalue2));
                ((DataListValue*)rez)->Append(new IntValue(ivalue1%ivalue2));
                return rez;
            }
            case eqSpec:                    // =
            {
                if(argType!=DataListValueType)
                    return new ErrorValue(BASEFUNCERROR);
                if(elements.size()!=2)
                    return new ErrorValue(BASEFUNCERROR);
                Value *elem1,*elem2;
                elem1=elements[0];
                elem2=elements[1];
                if(elem1->GetValueType()==IntValueType)
                {
                    int ivalue1=((IntValue*)elem1)->GetIntValue();
                    if(elem2->GetValueType()==IntValueType)
                    {
                        int ivalue2=((IntValue*)elem2)->GetIntValue();
                        return new BoolValue(ivalue1==ivalue2);
                    }
                    if(elem2->GetValueType()==FloatValueType)
                    {
                        double fvalue2=((FloatValue*)elem2)->GetFloatValue();
                        return new BoolValue(ivalue1==fvalue2);
                    }
                    return new ErrorValue(BASEFUNCERROR);
                }
                if(elem1->GetValueType()==FloatValueType)
                {
                    double fvalue1=((FloatValue*)elem1)->GetFloatValue();
                    if(elem2->GetValueType()==IntValueType)
                    {
                        int ivalue2=((IntValue*)elem2)->GetIntValue();
                        return new BoolValue(fvalue1==ivalue2);
                    }
                    if(elem2->GetValueType()==FloatValueType)
                    {
                        double fvalue2=((FloatValue*)elem2)->GetFloatValue();
                        return new BoolValue(fvalue1==fvalue2);
                    }
                    return new ErrorValue(BASEFUNCERROR);
                }
                if(elem1->GetValueType()==CharValueType)
                    if(elem2->GetValueType()==CharValueType)
                    {
                        QChar cvalue1,cvalue2;
                        cvalue1=((CharValue*)elem1)->GetCharValue();
                        cvalue2=((CharValue*)elem2)->GetCharValue();
                        return new BoolValue(cvalue1==cvalue2);
                    }
                if(elem1->GetValueType()==BoolValueType)
                    if(elem2->GetValueType()==BoolValueType)
                    {
                        bool bvalue1,bvalue2;
                        bvalue1=((BoolValue*)elem1)->GetBoolValue();
                        bvalue2=((BoolValue*)elem2)->GetBoolValue();
                        return new BoolValue(bvalue1==bvalue2);
                    }
                if(elem1->GetValueType()==SpecValueType)
                    if(elem2->GetValueType()==SpecValueType)
                    {
                        SpecType spec1,spec2;
                        spec1=((SpecValue*)elem1)->GetSpecValue();
                        spec2=((SpecValue*)elem2)->GetSpecValue();
                        return new BoolValue(spec1==spec2);
                    }
                if(elem1->GetValueType()==ErrorValueType)
                {
                    if(elem2->GetValueType()==ErrorValueType)
                    {
                        ErrorType error1, error2;
                        error1=((ErrorValue*)elem1)->GetErrorValue();
                        error2=((ErrorValue*)elem2)->GetErrorValue();
                        return new BoolValue(error1==error2);
                    }
                }
                if(elem1->GetValueType()==ExternValueType)
                {
                    if(elem2->GetValueType()==ExternValueType)
                    {
                        int epid1,epid2;
                        int extid1,extid2;
                        int funcid1,funcid2;
                        epid1=((ExternValue*)elem1)->GetEPID();
                        epid2=((ExternValue*)elem2)->GetEPID();
                        extid1=((ExternValue*)elem1)->GetExternValue();
                        extid2=((ExternValue*)elem2)->GetExternValue();
                        funcid1=caller->GetFuncUID(epid1,extid1);
                        funcid2=caller->GetFuncUID(epid2,extid2);
                        return new BoolValue(funcid1==funcid2);
                    }
                }
                return new ErrorValue(BASEFUNCERROR);
            }
            case neSpec:                    // !=
            {
                if(argType!=DataListValueType)
                    return new ErrorValue(BASEFUNCERROR);
                if(elements.size()!=2)
                    return new ErrorValue(BASEFUNCERROR);
                Value *elem1,*elem2;
                elem1=elements[0];
                elem2=elements[1];
                if(elem1->GetValueType()==IntValueType)
                {
                    int ivalue1=((IntValue*)elem1)->GetIntValue();
                    if(elem2->GetValueType()==IntValueType)
                    {
                        int ivalue2=((IntValue*)elem2)->GetIntValue();
                        return new BoolValue(ivalue1!=ivalue2);
                    }
                    if(elem2->GetValueType()==FloatValueType)
                    {
                        double fvalue2=((FloatValue*)elem2)->GetFloatValue();
                        return new BoolValue(ivalue1!=fvalue2);
                    }
                    return new ErrorValue(BASEFUNCERROR);
                }
                if(elem1->GetValueType()==FloatValueType)
                {
                    double fvalue1=((FloatValue*)elem1)->GetFloatValue();
                    if(elem2->GetValueType()==IntValueType)
                    {
                        int ivalue2=((IntValue*)elem2)->GetIntValue();
                        return new BoolValue(fvalue1!=ivalue2);
                    }
                    if(elem2->GetValueType()==FloatValueType)
                    {
                        double fvalue2=((FloatValue*)elem2)->GetFloatValue();
                        return new BoolValue(fvalue1!=fvalue2);
                    }
                    return new ErrorValue(BASEFUNCERROR);
                }
                if(elem1->GetValueType()==CharValueType)
                    if(elem2->GetValueType()==CharValueType)
                    {
                        QChar cvalue1,cvalue2;
                        cvalue1=((CharValue*)elem1)->GetCharValue();
                        cvalue2=((CharValue*)elem2)->GetCharValue();
                        return new BoolValue(cvalue1!=cvalue2);
                    }
                if(elem1->GetValueType()==BoolValueType)
                    if(elem2->GetValueType()==BoolValueType)
                    {
                        bool bvalue1,bvalue2;
                        bvalue1=((BoolValue*)elem1)->GetBoolValue();
                        bvalue2=((BoolValue*)elem2)->GetBoolValue();
                        return new BoolValue(bvalue1!=bvalue2);
                    }
                if(elem1->GetValueType()==SpecValueType)
                    if(elem2->GetValueType()==SpecValueType)
                    {
                        SpecType spec1,spec2;
                        spec1=((SpecValue*)elem1)->GetSpecValue();
                        spec2=((SpecValue*)elem2)->GetSpecValue();
                        return new BoolValue(spec1!=spec2);
                    }
                if(elem1->GetValueType()==ErrorValueType)
                {
                    if(elem2->GetValueType()==ErrorValueType)
                    {
                        ErrorType error1, error2;
                        error1=((ErrorValue*)elem1)->GetErrorValue();
                        error2=((ErrorValue*)elem2)->GetErrorValue();
                        return new BoolValue(error1!=error2);
                    }
                }
                if(elem1->GetValueType()==ExternValueType)
                {
                    if(elem2->GetValueType()==ExternValueType)
                    {
                        int epid1,epid2;
                        int extid1,extid2;
                        int funcid1,funcid2;
                        epid1=((ExternValue*)elem1)->GetEPID();
                        epid2=((ExternValue*)elem2)->GetEPID();
                        extid1=((ExternValue*)elem1)->GetExternValue();
                        extid2=((ExternValue*)elem2)->GetExternValue();
                        funcid1=caller->GetFuncUID(epid1,extid1);
                        funcid2=caller->GetFuncUID(epid2,extid2);
                        return new BoolValue(funcid1!=funcid2);
                    }
                }
                return new ErrorValue(BASEFUNCERROR);
            }
            case gtSpec:                    // >
            {
                if(argType!=DataListValueType)
                    return new ErrorValue(BASEFUNCERROR);
                if(elements.size()!=2)
                    return new ErrorValue(BASEFUNCERROR);
                Value *elem1,*elem2;
                elem1=elements[0];
                elem2=elements[1];
				if(elem1->GetValueType()==IntValueType)
                {
                    int ivalue1=((IntValue*)elem1)->GetIntValue();
                    if(elem2->GetValueType()==IntValueType)
                    {
                        int ivalue2=((IntValue*)elem2)->GetIntValue();
                        return new BoolValue(ivalue1>ivalue2);
                    }
                    if(elem2->GetValueType()==FloatValueType)
                    {
                        double fvalue2=((FloatValue*)elem2)->GetFloatValue();
                        return new BoolValue(ivalue1>fvalue2);
                    }
                    return new ErrorValue(BASEFUNCERROR);
                }
                if(elem1->GetValueType()==FloatValueType)
                {
                    double fvalue1=((FloatValue*)elem1)->GetFloatValue();
                    if(elem2->GetValueType()==IntValueType)
                    {
                        int ivalue2=((IntValue*)elem2)->GetIntValue();
                        return new BoolValue(fvalue1>ivalue2);
                    }
                    if(elem2->GetValueType()==FloatValueType)
                    {
                        double fvalue2=((FloatValue*)elem2)->GetFloatValue();
                        return new BoolValue(fvalue1>fvalue2);
                    }
                    return new ErrorValue(BASEFUNCERROR);
                }
                if(elem1->GetValueType()==CharValueType)
                    if(elem2->GetValueType()==CharValueType)
                    {
                        QChar cvalue1,cvalue2;
                        cvalue1=((CharValue*)elem1)->GetCharValue();
                        cvalue2=((CharValue*)elem2)->GetCharValue();
                        return new BoolValue(cvalue1>cvalue2);
                    }
                if(elem1->GetValueType()==BoolValueType)
                    if(elem2->GetValueType()==BoolValueType)
                    {
                        bool bvalue1,bvalue2;
                        bvalue1=((BoolValue*)elem1)->GetBoolValue();
                        bvalue2=((BoolValue*)elem2)->GetBoolValue();
                        return new BoolValue(bvalue1>bvalue2);
                    }
                return new ErrorValue(BASEFUNCERROR);
            }
            case geSpec:                    // >=
            {
                if(argType!=DataListValueType)
                    return new ErrorValue(BASEFUNCERROR);
                if(elements.size()!=2)
                    return new ErrorValue(BASEFUNCERROR);
                Value *elem1,*elem2;
                elem1=elements[0];
                elem2=elements[1];
                if(elem1->GetValueType()==IntValueType)
                {
                    int ivalue1=((IntValue*)elem1)->GetIntValue();
                    if(elem2->GetValueType()==IntValueType)
                    {
                        int ivalue2=((IntValue*)elem2)->GetIntValue();
                        return new BoolValue(ivalue1>=ivalue2);
                    }
                    if(elem2->GetValueType()==FloatValueType)
                    {
                        double fvalue2=((FloatValue*)elem2)->GetFloatValue();
                        return new BoolValue(ivalue1>=fvalue2);
                    }
                    return new ErrorValue(BASEFUNCERROR);
                }
                if(elem1->GetValueType()==FloatValueType)
                {
                    double fvalue1=((FloatValue*)elem1)->GetFloatValue();
                    if(elem2->GetValueType()==IntValueType)
                    {
                        int ivalue2=((IntValue*)elem2)->GetIntValue();
                        return new BoolValue(fvalue1>=ivalue2);
                    }
                    if(elem2->GetValueType()==FloatValueType)
                    {
                        double fvalue2=((FloatValue*)elem2)->GetFloatValue();
                        return new BoolValue(fvalue1>=fvalue2);
                    }
                    return new ErrorValue(BASEFUNCERROR);
                }
                if(elem1->GetValueType()==CharValueType)
                    if(elem2->GetValueType()==CharValueType)
                    {
                        QChar cvalue1,cvalue2;
                        cvalue1=((CharValue*)elem1)->GetCharValue();
                        cvalue2=((CharValue*)elem2)->GetCharValue();
                        return new BoolValue(cvalue1>=cvalue2);
                    }
                if(elem1->GetValueType()==BoolValueType)
                    if(elem2->GetValueType()==BoolValueType)
                    {
                        bool bvalue1,bvalue2;
                        bvalue1=((BoolValue*)elem1)->GetBoolValue();
                        bvalue2=((BoolValue*)elem2)->GetBoolValue();
                        return new BoolValue(bvalue1>=bvalue2);
                    }
                return new ErrorValue(BASEFUNCERROR);
            }
            case ltSpec:                    // <
            {
                if(argType!=DataListValueType)
                    return new ErrorValue(BASEFUNCERROR);
                if(elements.size()!=2)
                    return new ErrorValue(BASEFUNCERROR);
                Value *elem1,*elem2;
                elem1=elements[0];
                elem2=elements[1];
                if(elem1->GetValueType()==IntValueType)
                {
                    int ivalue1=((IntValue*)elem1)->GetIntValue();
                    if(elem2->GetValueType()==IntValueType)
                    {
                        int ivalue2=((IntValue*)elem2)->GetIntValue();
                        return new BoolValue(ivalue1<ivalue2);
                    }
                    if(elem2->GetValueType()==FloatValueType)
                    {
                        double fvalue2=((FloatValue*)elem2)->GetFloatValue();
                        return new BoolValue(ivalue1<fvalue2);
                    }
                    return new ErrorValue(BASEFUNCERROR);
                }
                if(elem1->GetValueType()==FloatValueType)
                {
                    double fvalue1=((FloatValue*)elem1)->GetFloatValue();
                    if(elem2->GetValueType()==IntValueType)
                    {
                        int ivalue2=((IntValue*)elem2)->GetIntValue();
                        return new BoolValue(fvalue1<ivalue2);
                    }
                    if(elem2->GetValueType()==FloatValueType)
                    {
                        double fvalue2=((FloatValue*)elem2)->GetFloatValue();
                        return new BoolValue(fvalue1<fvalue2);
                    }
                    return new ErrorValue(BASEFUNCERROR);
                }
                if(elem1->GetValueType()==CharValueType)
                    if(elem2->GetValueType()==CharValueType)
                    {
                        QChar cvalue1,cvalue2;
                        cvalue1=((CharValue*)elem1)->GetCharValue();
                        cvalue2=((CharValue*)elem2)->GetCharValue();
                        return new BoolValue(cvalue1<cvalue2);
                    }
                if(elem1->GetValueType()==BoolValueType)
                    if(elem2->GetValueType()==BoolValueType)
                    {
                        bool bvalue1,bvalue2;
                        bvalue1=((BoolValue*)elem1)->GetBoolValue();
                        bvalue2=((BoolValue*)elem2)->GetBoolValue();
                        return new BoolValue(bvalue1<bvalue2);
                    }
                return new ErrorValue(BASEFUNCERROR);
            }
            case leSpec:                    // <=
            {
                if(argType!=DataListValueType)
                    return new ErrorValue(BASEFUNCERROR);
                if(elements.size()!=2)
                    return new ErrorValue(BASEFUNCERROR);
                Value *elem1,*elem2;
                elem1=elements[0];
                elem2=elements[1];
                if(elem1->GetValueType()==IntValueType)
                {
                    int ivalue1=((IntValue*)elem1)->GetIntValue();
                    if(elem2->GetValueType()==IntValueType)
                    {
                        int ivalue2=((IntValue*)elem2)->GetIntValue();
                        return new BoolValue(ivalue1<=ivalue2);
                    }
                    if(elem2->GetValueType()==FloatValueType)
                    {
                        double fvalue2=((FloatValue*)elem2)->GetFloatValue();
                        return new BoolValue(ivalue1<=fvalue2);
                    }
                    return new ErrorValue(BASEFUNCERROR);
                }
                if(elem1->GetValueType()==FloatValueType)
                {
                    double fvalue1=((FloatValue*)elem1)->GetFloatValue();
                    if(elem2->GetValueType()==IntValueType)
                    {
                        int ivalue2=((IntValue*)elem2)->GetIntValue();
                        return new BoolValue(fvalue1<=ivalue2);
                    }
                    if(elem2->GetValueType()==FloatValueType)
                    {
                        double fvalue2=((FloatValue*)elem2)->GetFloatValue();
                        return new BoolValue(fvalue1<=fvalue2);
                    }
                    return new ErrorValue(BASEFUNCERROR);
                }
                if(elem1->GetValueType()==CharValueType)
                    if(elem2->GetValueType()==CharValueType)
                    {
                        QChar cvalue1,cvalue2;
                        cvalue1=((CharValue*)elem1)->GetCharValue();
                        cvalue2=((CharValue*)elem2)->GetCharValue();
                        return new BoolValue(cvalue1<=cvalue2);
                    }
                if(elem1->GetValueType()==BoolValueType)
                    if(elem2->GetValueType()==BoolValueType)
                    {
                        bool bvalue1,bvalue2;
                        bvalue1=((BoolValue*)elem1)->GetBoolValue();
                        bvalue2=((BoolValue*)elem2)->GetBoolValue();
                        return new BoolValue(bvalue1<=bvalue2);
                    }
                return new ErrorValue(BASEFUNCERROR);
            }
            case vertSpec:                  // |
            {
                if(argType==DataListValueType)
                {
                    // а не (.) ли ты, дружок?
                    // специальный случай. Можно было бы сократить сигнал и в этом случае, но тогда
                    // трудно было бы визуально отличать пустой список () от (.)
                    // Да и вообще, формула (.) в примерах используется давно.
                    if (elements.size()==1)
                        if(elements[0]->GetValueType()==SpecValueType)
                            if(((SpecValue*)elements[0])->GetSpecValue()==dotSpec)
                                return new IntValue(0);

                    // случай обычный и без наворотов
                    return new IntValue(elements.size());
                }
                if(argType==AsyncListValueType)
                {
                    if(asyncListArg->GetHead()==NULL)
                        return new IntValue(0);
                    else
                        return new IntValue(1);
                }
                return new ErrorValue(BASEFUNCERROR);


            }
            case dupSpec:                  // dup
            {
                if(argType!=DataListValueType)
                    return new ErrorValue(BASEFUNCERROR);
                if(elements.size()!=2)
                    return new ErrorValue(BASEFUNCERROR);
                if(elements[1]->GetValueType()!=IntValueType)
                    return new ErrorValue(BASEFUNCERROR);
                int count;
                count=((IntValue*)elements[1])->GetIntValue();
                rez=new DataListValue();
                for(int i=0;i<count;i++)
                    ((DataListValue*)rez)->Append(elements[0]);
                return rez;
            }
            case questSpec:                 // ?
            {
                ParListValue* parListRez;
                if(argType!=DataListValueType)
                    return new ErrorValue(BASEFUNCERROR);
                rez=new ParListValue();
                parListRez=(ParListValue*)rez;
                for(int i=0;i<elements.size();i++)
                {
                    if(elements[i]->GetValueType()!=BoolValueType && elements[i]->GetValueType()!=MockValueType)
                    {
                        delete rez;
                        return new ErrorValue(BASEFUNCERROR);
                    }
                    if(elements[i]->GetValueType()==BoolValueType)
                    {
                        if (((BoolValue*)elements[i])->GetBoolValue()==true )
                        {
                            parListRez->Append(new IntValue(i+1));
                        }
                    }
                }
                if (parListRez->GetElementListSize()==0)
                    rez = new SpecValue (dotSpec);
                return rez;
            }
            case dotsSpec:                   // ..
            {
                if(argType!=DataListValueType)
                    return new ErrorValue(BASEFUNCERROR);
                if(elements.size()==2)
                {
                    if(elements[0]->GetValueType()==IntValueType && elements[1]->GetValueType()==IntValueType)
                    {
                        int ivalue1,ivalue2;
                        ivalue1=((IntValue*)elements[0])->GetIntValue();
                        ivalue2=((IntValue*)elements[1])->GetIntValue();
                        if(ivalue1<=ivalue2)
                        {
                            rez=new DataListValue();
                            for(int i=ivalue1;i<=ivalue2;i++)
                                ((DataListValue*)rez)->Append(new IntValue(i));
                            return rez;
                        }
                        return new ErrorValue(BOUNDERROR);
                    }
                    return new ErrorValue(BASEFUNCERROR);
                }
                if(elements.size()==3)
                {
                    ValueType typeElem1,typeElem2,typeElem3;
                    typeElem1=elements[0]->GetValueType();
                    typeElem2=elements[1]->GetValueType();
                    typeElem3=elements[2]->GetValueType();
                    if(typeElem1!=IntValueType && typeElem1!=FloatValueType)
                        return new ErrorValue(BASEFUNCERROR);
                    if(typeElem2!=IntValueType && typeElem2!=FloatValueType)
                        return new ErrorValue(BASEFUNCERROR);
                    if(typeElem3!=IntValueType && typeElem3!=FloatValueType)
                        return new ErrorValue(BASEFUNCERROR);
                    if(typeElem1==IntValueType && typeElem2==IntValueType && typeElem3==IntValueType)
                    {
                        int ivalue1,ivalue2,ivalue3;
                        ivalue1=((IntValue*)elements[0])->GetIntValue();
                        ivalue2=((IntValue*)elements[1])->GetIntValue();
                        ivalue3=((IntValue*)elements[2])->GetIntValue();

                        if(ivalue1<=ivalue2)
                        {
                            if (ivalue3<=0)
                                return new ErrorValue(BOUNDERROR);
                            rez=new DataListValue();
                            for(int i=ivalue1;i<=ivalue2;i+=ivalue3)
                                ((DataListValue*)rez)->Append(new IntValue(i));
                            return rez;
                        }
                        if(ivalue1>ivalue2)
                        {
                            if (ivalue3>=0)
                                return new ErrorValue(BOUNDERROR);
                            rez=new DataListValue();
                            for(int i=ivalue1;i>=ivalue2;i+=ivalue3)
                                ((DataListValue*)rez)->Append(new IntValue(i));
                            return rez;
                        }
                        return new ErrorValue(BOUNDERROR);
                    }
                    double fvalue1=0,fvalue2=0,fvalue3=0;
                    if(typeElem1==IntValueType)
                        fvalue1=((IntValue*)elements[0])->GetIntValue();
                    if(typeElem1==FloatValueType)
                        fvalue1=((FloatValue*)elements[0])->GetFloatValue();

                    if(typeElem2==IntValueType)
                        fvalue2=((IntValue*)elements[1])->GetIntValue();
                    if(typeElem2==FloatValueType)
                        fvalue2=((FloatValue*)elements[1])->GetFloatValue();

                    if(typeElem3==IntValueType)
                        fvalue3=((IntValue*)elements[2])->GetIntValue();
                    if(typeElem3==FloatValueType)
                        fvalue3=((FloatValue*)elements[2])->GetFloatValue();

                    rez=new DataListValue();
                    if(fvalue1<=fvalue2)
                    {
                        if (fvalue3<=0)
                            return new ErrorValue(BOUNDERROR);
                        for(float f=fvalue1;f<=fvalue2;f+=fvalue3)
                            ((DataListValue*)rez)->Append(new FloatValue(f));
                    }
                    if(fvalue1>fvalue2)
                    {
                        if (fvalue3>=0)
                            return new ErrorValue(BOUNDERROR);
                        for(float f=fvalue1;f>=fvalue2;f+=fvalue3)
                            ((DataListValue*)rez)->Append(new FloatValue(f));
                    }
                    return rez;
                }
                return new ErrorValue(BASEFUNCERROR);
            }
            case sharpSpec:                 // #
            {
                if(argType!=DataListValueType)
                    return new ErrorValue(BASEFUNCERROR);
                rez=new DataListValue();
                DataListValue* tmpList;
                bool stop=false;
                int j=0;
                while(!stop)
                {
                    stop=true;
                    tmpList=new DataListValue();
                    for(int i=0;i<elements.size();i++)
                    {
                        if(elements[i]->GetValueType()!=DataListValueType)
                        {
                            delete rez;
                            delete tmpList;
                            return new ErrorValue(BASEFUNCERROR);
                        }
                        transElements=*((DataListValue*)elements[i])->GetElementList();
                        if(j<transElements.size())
                        {
                            stop=false;
                            tmpList->Append(transElements[j]);
                        }
                    }
                    if(!stop)
                        ((DataListValue*)rez)->Append(tmpList);
                    else
                        delete tmpList;
                    j++;
                }
                return rez;
            }
            case dalistSpec:                 // ()
            {
                rez=new DataListValue();
                if(argType!=ParListValueType)
                {
                    ((DataListValue*)rez)->Append(arg);
                }
                else
                {
                    for(int i=0;i<elements.size();i++)
                        ((DataListValue*)rez)->Append(elements[i]);
                }
                return rez;
            }
            case palistSpec:                 // []
            {
                //FLTR
                rez=new ParListValue();
                if(argType==DataListValueType)
                {
                    if(elements.size()==1)
                        return elements[0];
                    for(int i=0;i<elements.size();i++)
                        ((ParListValue*)rez)->Append(elements[i]);

                }
                else if(argType==AsyncListValueType)
                {
                    AsyncListValue* asyncIndex=asyncListArg;
                    Value* curHead=asyncIndex->GetHead();
                    while(curHead!=NULL)
                    {
                        ((ParListValue*)rez)->Append(curHead);
                        asyncIndex=asyncIndex->GetTail();
                        curHead=asyncIndex->GetHead();
                    }
                }
                else
                    ((ParListValue*)rez)->Append(arg);
                caller->SendExtraSignals((ParListValue*)rez,nodeNum);
                return rez;
            }
            case aslistSpec:      // <()
            {
                if (argType!=DataListValueType)
                    return new ErrorValue(BASEFUNCERROR);
                AsyncListValue* aslistRez=new AsyncListValue();
                for(int i=0;i<elements.size();i++)
                    aslistRez->Append(elements[i]);
                return (Value*)aslistRez;
            }
            case intSpec:   //int
            {
                switch (argType)
                {
                    case IntValueType:
                        rez=new IntValue(intArg->GetIntValue());
                        break;
                    case FloatValueType:
                        rez=new IntValue(floatArg->GetFloatValue());
                        break;
                    case CharValueType:
                        rez=new IntValue(charArg->GetCharValue().toLatin1());
                        break;
                    case BoolValueType:
                        rez=new IntValue(boolArg->GetBoolValue());
                        break;
                    default:
                        rez=new ErrorValue(BASEFUNCERROR);
                        break;
                }
                return rez;
            }
            case floatSpec:   //float
            {
                switch (argType)
                {
                    case IntValueType:
                        rez=new FloatValue(intArg->GetIntValue());
                        break;
                    case FloatValueType:
                        rez=new FloatValue(floatArg->GetFloatValue());
                        break;
                    case CharValueType:
                        rez=new FloatValue(charArg->GetCharValue().toLatin1());
                        break;
                    case BoolValueType:
                        rez=new FloatValue(boolArg->GetBoolValue());
                        break;
                    default:
                        rez=new ErrorValue(BASEFUNCERROR);
                        break;
                }
                return rez;
            }
            case charSpec:   //char
            {
                switch (argType)
                {
                    case IntValueType:
                        rez=new CharValue(intArg->GetIntValue());
                        break;
                    case CharValueType:
                        rez=new CharValue(charArg->GetCharValue().toLatin1());
                        break;
                    default:
                        rez=new ErrorValue(BASEFUNCERROR);
                        break;
                }
                return rez;
            }
            case boolSpec:   //bool
            {
                switch (argType)
                {
                    case IntValueType:
                        rez=new BoolValue(intArg->GetIntValue());
                        break;
                    case FloatValueType:
                        rez=new BoolValue(floatArg->GetFloatValue());
                        break;
                    case BoolValueType:
                        rez=new BoolValue(boolArg->GetBoolValue());
                        break;
                    default:
                        rez=new ErrorValue(BASEFUNCERROR);
                        break;
                }
                return rez;
            }
            case signalSpec:    // signal
            {
                rez=new SpecValue(dotSpec);
                return rez;
            }
            case typeSpec:      // type
            {
                switch (argType)
                {
                    case IntValueType:
                        rez=new SpecValue(intSpec);
                        break;
                    case FloatValueType:
                        rez=new SpecValue(floatSpec);
                        break;
                    case BoolValueType:
                        rez=new SpecValue(boolSpec);
                        break;
                    case CharValueType:
                        rez=new SpecValue(charSpec);
                        break;
                    case DataListValueType:
                        rez=new SpecValue(dalistSpec);
                        break;
                    case ParListValueType:
                        rez=new SpecValue(palistSpec);
                        break;
                    case DelayValueType:
                        rez=new SpecValue(delistSpec);
                        break;
                    case AsyncListValueType:
                        rez=new SpecValue(aslistSpec);
                        break;
                    case SpecValueType:
                        if(((SpecValue*)arg)->GetSpecValue()==dotSpec)
                            rez=new SpecValue(signalSpec);
                        else
                            rez=new ErrorValue(TYPEERROR);
                        break;
                    //TODO - а вот тут надо думать
                    case ErrorValueType:
                    case ExternValueType:
                    case MockValueType:
                    case NullValueType:
                    default:
                        rez=new ErrorValue(TYPEERROR);
                        break;
                }
                return rez;
            }
            default:
                return new ErrorValue(BASEFUNCERROR);
        }
    }
    if(funcType==DataListValueType)         // интерпретация списком данных
    {
        rez=new DataListValue();
        elements=*(((DataListValue*)func)->GetElementList());
        int elemLength;
        elemLength=elements.size();
        for(int i=0;i<elemLength;i++)
            ((DataListValue*)rez)->Append(Interpret(elements[i],arg,caller,0,0));       //TODO ALU2 если хотя бы один элемент окажется экстерн-функцией - у нас будут проблемы, Хьюстон
        return rez;
    }
    if(funcType==IntValueType)              // извлечение элемента
    {
        int ivalue;
        ivalue=((IntValue*)func)->GetIntValue();
        if(arg->GetValueType()==DataListValueType)
        {
            if(ivalue==0)                               //TOFIX
                return new SpecValue(dotSpec);
            if(ivalue>0)
            {
                if(ivalue>elements.size())
                    return new ErrorValue(BOUNDERROR);
                return elements[ivalue-1];
            }
            if(ivalue<0)
            {
                if((-ivalue)>elements.size())
                    return new ErrorValue(BOUNDERROR);
                rez=new DataListValue();
                for(int i=0;i<elements.size();i++)
                {
                    if(i==(-(ivalue+1)))
                        continue;
                    ((DataListValue*)rez)->Append(elements[i]);
                }
                return rez;
            }
        }
        if(arg->GetValueType()==AsyncListValueType)
        {
            if(ivalue==1)
                return asyncListArg->GetHead();
            if(ivalue==-1)
                return asyncListArg->GetTail();
            return new ErrorValue(BASEFUNCERROR);
        }
        return new ErrorValue(BASEFUNCERROR);
    }
    if(funcType==BoolValueType)              // фильтрация
    {
        bool bvalue;
        bvalue=((BoolValue*)func)->GetBoolValue();
        if(bvalue)
            return arg;
        else
            return new SpecValue(dotSpec);
    }
    if(funcType==ExternValueType)       //вызов внешней функции
    {
        qDebug() << "ALU calling extern; funcID " << ((ExternValue*)func)->GetExternValue() << " EPID=" << ((ExternValue*)func)->GetEPID();
        caller->CallExtern(((ExternValue*)func)->GetExternValue(),((ExternValue*)func)->GetEPID(),arg,nodeNum,posIndex);
        return new MockValue();
    }
    return new ErrorValue(BASEFUNCERROR);
}
//Interpret TODO ALU3:
// * сравнение типов?
// * преобразование arg:[] (в параллельный список)
// * преобразование arg:<() (в асинхронный список)
// * преобразование arg:{} (в задержанный список)
// * функция возврата типа
// * функция-сигнал (раскрытие задержанных списков)
// * интерпретация внешней функцией
// * преобразование во внутренний тип (?)
// * преобразование во внешний тип (?)

