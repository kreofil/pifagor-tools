/*==============================================================================
   File: pfg_parser.cpp

   Description:
        Pifagor programming language parser(syntax analyzer).
        Input: array of lexema, debug information.
        Output: information graph, debug information.

   Credits: AUTHORS

   License: GNU General Public License Version 3 or later
            http://www.gnu.org/licenses/gpl.html
==============================================================================*/
#ifndef PFG_PARSER_CPP
#define PFG_PARSER_CPP

#include "pfg_parser2.h"

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <direct.h>
#endif

pfg_parser::pfg_parser()
{
    curRig=NULL;
    vLastObj=NULL;
}

pfg_parser::~pfg_parser()
{
    funcRigs.clear();
    if(curRig)
        delete curRig;
    if(vLastObj)
        delete vLastObj;
    mlVars.clear();
    mlBlockBreaks.clear();
    imports.clear();
    ntFuncs.clear();
    mbFuncs.clear();
    funcPos.clear();
    delete[] error_message;
}


/*------------------------------------------------------------------------------
  Lexema equal opearation
------------------------------------------------------------------------------*/
bool operator==(lexema l, lex type)
{
    if (l._type==type) return true;
    return false;
};
/*------------------------------------------------------------------------------
  Set variable for current block
------------------------------------------------------------------------------*/
void pfg_parser::set_var_id(wstring name, MemLocation id)
{
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> set_var_id %ls %ld:%ld block level:%ld\n", name.c_str(), id.GetMemType(), id.GetIndex(), cur_block);
    fflush(dbg);
#endif

    mlVars[cur_block][name]=id;
    curRig->AppendName(id,QString::fromStdWString(name));

#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< set_var_id set\n");
    fflush(dbg);
#endif
}

/*------------------------------------------------------------------------------
    Main translation function.
------------------------------------------------------------------------------*/
bool pfg_parser::parse(pfg_scanner* scan)
{
    err=scan->err;
    dbg=scan->dbg;

#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> parse\n");
    fflush(dbg);
#endif
    scan->print(dbg);
    buf=scan->buf;
    pos=buf;
    len=scan->buf_len;
    cur_block=0;
    curDelay=0;
    maxDelay=0;
    error_begin=pos;
    error_end=pos;
    error_flag=true;
    error_message=new wchar_t[256];
    wcscpy(error_message,L"\0");

    funcRigs.clear();
    mlVars.clear();

    mlVars.resize(100);
    mlBlockBreaks.resize(100);
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER start len=%ld\n",len);
    fflush(dbg);
#endif



    if (is_program())
    {
#ifdef PARSER_DEBUG
        fwprintf(dbg, L"PARSER<< program ok\n");
        fflush(dbg);
#endif
        funcPos.push_back(Positions(pos->pos.GetAll()));
        return true;
    }
    else
    {
        fwprintf(err, L"Error parsing program: %ls\n", error_message);fflush(err);
        fwprintf(err, L"  start  str:%d pos:%d\n", error_begin->pos.GetBeginRow(), error_begin->pos.GetBeginCol());fflush(err);
        fwprintf(err, L"  finish str:%d pos:%d\n", error_end->pos.GetBeginRow(), error_end->pos.GetBeginCol());fflush(err);
        return false;
    };
}

/*------------------------------------------------------------------------------
    Constant parser.
------------------------------------------------------------------------------*/
bool pfg_parser::parse_const(pfg_scanner* scan)
{
    buf=scan->buf;
    pos=buf;
    len=scan->buf_len;
    curRig=new Rig();

    error_message=new wchar_t[256];
    wcscpy(error_message,L"\0");

    BegActor *uselessArgument=new BegActor();
    uselessArgument->SetDelay(0);
    uselessArgument->pos=Positions(0,0,0,0);
    if (is_const())
    {
        curRig->Append(uselessArgument);
        curRig->Append(new ExtRef("argument"));
        funcRigs.push_back(curRig);
        return true;
    }
    return false;
};

/*----------------------------------------------------------------------------*/
bool pfg_parser::is_const()
{
    SpecType stF;
    ErrorType stErr;

    if (is_const_datalist() )   {                   goto _true;     };
    //if (is_const_parlist()  )   {                   goto _true;     };
    //if (is_const_delaylist())   {                   goto _true;     };
    //if (is_const_asynclist())   {                   goto _true;     };
    if (*pos==val_bool      )   {                   goto _bool;     };
    if (*pos==val_int       )   {                   goto _int;      };
    if (*pos==val_float     )   {                   goto _float;    };
    if (*pos==val_char     )   {                    goto _char;    };
    if (*pos==val_str       )   {                   goto _str;      };

    if (*pos==spec_add      )   { stF=addSpec;      goto _ifunc;}
    if (*pos==spec_sub      )   { stF=minusSpec;    goto _ifunc;}
    if (*pos==spec_mul      )   { stF=astSpec;      goto _ifunc;}
    if (*pos==spec_div      )   { stF=slashSpec;    goto _ifunc;}
    if (*pos==spec_idiv     )   { stF=percentSpec;  goto _ifunc;}
    if (*pos==spec_brackets )   { stF=dalistSpec;   goto _ifunc;}
    if (*pos==spec_braces   )   { stF=palistSpec;   goto _ifunc;}
    if (*pos==spec_cbrackets)   { stF=delistSpec;   goto _ifunc;}
    if (*pos==spec_dot      )   { stF=dotSpec;      goto _ifunc;}
    if (*pos==spec_ddot     )   { stF=dotsSpec;     goto _ifunc;}
    if (*pos==spec_lower    )   { stF=ltSpec;       goto _ifunc;}
    if (*pos==spec_greater  )   { stF=gtSpec;       goto _ifunc;}
    if (*pos==spec_len      )   { stF=vertSpec;     goto _ifunc;}
    if (*pos==spec_trans    )   { stF=sharpSpec;    goto _ifunc;}
    if (*pos==spec_le       )   { stF=leSpec;       goto _ifunc;}
    if (*pos==spec_ge       )   { stF=geSpec;       goto _ifunc;}
    if (*pos==spec_equal    )   { stF=eqSpec;       goto _ifunc;}
    if (*pos==spec_nequal   )   { stF=neSpec;       goto _ifunc;}
    if (*pos==spec_quest    )   { stF=questSpec;    goto _ifunc;}
    if (*pos==spec_dup      )   { stF=dupSpec;      goto _ifunc;}
    if (*pos==kw_value      )   { stF=valSpec;      goto _ifunc;}
    if (*pos==kw_type       )   { stF=typeSpec;     goto _ifunc;}
    if (*pos==kw_in         )   { stF=inSpec;       goto _ifunc;}
    if (*pos==kw_async      )   { stF=aslistSpec;   goto _ifunc;}

    if (*pos==kw_signal     )   { stF=signalSpec;   goto _ifunc;}
    if (*pos==kw_int        )   { stF=intSpec;      goto _ifunc;}
    if (*pos==kw_float      )   { stF=floatSpec;    goto _ifunc;}
    if (*pos==kw_char       )   { stF=charSpec;     goto _ifunc;}
    if (*pos==kw_bool       )   { stF=boolSpec;     goto _ifunc;}

    if (*pos==kw_basefuncerror) { stErr=BASEFUNCERROR;  goto _ierror;}
    if (*pos==kw_typeerror)     { stErr=TYPEERROR;      goto _ierror;}
    if (*pos==kw_bounderror)    { stErr=BOUNDERROR;     goto _ierror;}
    if (*pos==kw_interror)      { stErr=INTERROR;       goto _ierror;}
    if (*pos==kw_realerror)     { stErr=REALERROR;      goto _ierror;}
    if (*pos==kw_zerodivide)    { stErr=ZERODIVIDE;     goto _ierror;}
    if (*pos==kw_valueerror)    { stErr=VALUEERROR;     goto _ierror;}
    goto _false;
_bool:
    vLastObj=new BoolValue(pos->b);
    pos++;
    goto _true;
_int:
    vLastObj = new IntValue(pos->i);
    pos++;
    goto _true;
_float:
    vLastObj = new FloatValue(pos->f);
    pos++;
    goto _true;
_char:
    vLastObj = new CharValue(pos->c);
    pos++;
    goto _true;
_str:
    vLastObj = new DataListValue(QString::fromStdWString(pos->s));
    pos++;
    goto _true;
_ifunc:
    vLastObj = new SpecValue(stF);
    pos++;
    goto _true;
_ierror:
    vLastObj = new ErrorValue(stErr);
    pos++;
    goto _true;
_false:
    return false;
_true:
    curRig->Append(vLastObj);
    return true;
};

/*----------------------------------------------------------------------------*/
bool pfg_parser::is_const_datalist()
{
    vl_iterator start=pos;
    DataListValue* curDataList=new DataListValue();
//_0:
    if (*pos==obracket){ pos++; goto _1;};
    goto _false;
_1:
    if (is_const())
    {
        curDataList->Append(vLastObj);
        goto _2;
    };
    goto _false;
_2:
    if (*pos==comma){ pos++; goto _3;}
    if (*pos==cbracket){ pos++; goto _true;}
    goto _false;
_3:
    if (is_const())
    {
        curDataList->Append(vLastObj);
        goto _2;
    };
    goto _false;
_false:
    delete curDataList;
    pos=start; return false;
_true:
    vLastObj=curDataList;
    return true;
};
/*----------------------------------------------------------------------------*/
bool pfg_parser::is_const_parlist()
{
    vl_iterator start=pos;
    ParListValue* curParList=new ParListValue();
//_0:
    if (*pos==obrace){ pos++; goto _1;};
    goto _false;
_1:
    if (is_const())
    {
        curParList->Append(vLastObj);
        goto _2;
    };
    goto _false;
_2:
    if (*pos==comma){ pos++; goto _3;}
    if (*pos==cbrace){ pos++; goto _true;}
    goto _false;
_3:
    if (is_const())
    {
        curParList->Append(vLastObj);
        goto _2;
    };
    goto _false;
_false:
    delete curParList;
    pos=start; return false;
_true:
    vLastObj=curParList;
    return true;
};
/*------------------------------------------------------------------------------
    Left designation parser.
    $ designation_left := {/ id "<<" /} description {">>"id} .
    example: pi<<3.14
------------------------------------------------------------------------------*/
bool pfg_parser::is_designation_l()
{
    vl_iterator start=pos;
    vl_iterator identPos;
    RigPos rpMid=curRig->GetPos();
    long oldCurDelay=curDelay;
    long oldMaxDelay=maxDelay;
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> is_designation_l parse start\n");
    fflush(dbg);
#endif
    //_0:
    if (*pos==id)   { identPos=pos; pos++; goto _1;};
    goto _false;
_1:
    if (*pos==spec_lassign) {pos++; goto _3;};
    goto _false;
_3:
    if (is_expression() )   goto _true;
    if (is_object()     )   goto _true;
    if (is_block()      )   goto _true;
    goto _false;
_false:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_designation_l false\n");
    fflush(dbg);
#endif
    if (error_flag)
    {
        error_flag  = false;
        error_begin = start;
        error_end   = pos;
        wcscpy(error_message,L"Error in left designation statement (>>)");
    };
    pos=start;
    curRig->Rollback(rpMid);
    curDelay=oldCurDelay;
    maxDelay=oldMaxDelay;

    return false;
_true:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_designation_l true %ls=node %ld\n", start->s, mlLastObj.GetIndex());
    fflush(dbg);
#endif
    //error_flag=true;
    if (wcscmp(start->s,L"return")==0)
    {
        RetActor *returnGuy=new RetActor(mlLastObj);
        returnGuy->pos.Set(identPos->pos.GetAll());
        curRig->Append(returnGuy);
    }
    else
        if (wcscmp(start->s,L"break")==0)
        {
            mlBlockBreaks[cur_block].push_back(mlLastObj);
        }
        else
        {
            set_var_id(start->s,mlLastObj);
        };
    return true;
}

/*------------------------------------------------------------------------------
    Right designation parser.
    $ designation_right := [ description ">>"] id {/ ">>" id /}.
    example: 3.14>>pi
------------------------------------------------------------------------------*/
bool pfg_parser::is_designation_r()
{
    vl_iterator start=pos;
    vl_iterator identPos;
    RigPos rpMid=curRig->GetPos();
    long oldCurDelay=curDelay;
    long oldMaxDelay=maxDelay;
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> is_designation_r parse start\n");
    fflush(dbg);
#endif
    //_0:
    if (is_expression() )   goto _2;
    if (is_object()     )   goto _2;
    if (is_block()      )   goto _2;
    goto _false;
_2:
    if (*pos==spec_rassign) {pos++; goto _4;};
    goto _false;
_4:
    if (*pos==id) {identPos=pos; pos++; goto _true;};
    goto _false;
_false:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_designation_r false\n");
    fflush(dbg);
#endif
    if (error_flag)
    {
        error_flag  = false;
        error_begin = start;
        error_end   = pos;
        wcscpy(error_message,L"Error in right designation statement (<<)");
    };
    pos=start;
    curRig->Rollback(rpMid);
    curDelay=oldCurDelay;
    maxDelay=oldMaxDelay;
    return false;
_true:
    pos--;
    vl_iterator fin=pos;
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_designation_r true %ls=node %ld\n", fin->s, mlLastObj.GetIndex());
    fflush(dbg);
#endif
    //error_flag=true;
    if (wcscmp(fin->s,L"return")==0)
    {
        RetActor *returnGuy=new RetActor(mlLastObj);
        returnGuy->pos.Set(identPos->pos.GetAll());
        curRig->Append(returnGuy);
    }
    else
        if (wcscmp(fin->s,L"break")==0)
        {
            mlBlockBreaks[cur_block].push_back(mlLastObj);
        }
        else
        {
            set_var_id(fin->s,mlLastObj);
        }
    pos++;
    return true;
}

/*------------------------------------------------------------------------------
    Element parser.
    $ designation := designation_left | designation_right.
    $ element := designation | expression | object | block.
------------------------------------------------------------------------------*/
bool pfg_parser::is_element()
{
    vl_iterator start=pos;
    RigPos rpMid=curRig->GetPos();
    long oldCurDelay=curDelay;
    long oldMaxDelay=maxDelay;
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> is_element start\n");
    fflush(dbg);
#endif
    //_0:
    if (is_designation_l()  ) goto _true;
    if (is_designation_r()  ) goto _true;
    if (is_expression()     ) goto _true;
    if (is_object()         ) goto _true;
    if (is_block()          ) goto _true;
    goto _false;
_false:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_element false\n");
    fflush(dbg);
#endif
    if (error_flag)
    {
        error_flag  = false;
        error_begin = start;
        error_end   = pos;
        wcscpy(error_message,L"Element expected");
    };
    pos=start;
    curRig->Rollback(rpMid);
    curDelay=oldCurDelay;
    maxDelay=oldMaxDelay;
    return false;
_true:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_element true\n");
    fflush(dbg);
#endif
    return true;
}

/*------------------------------------------------------------------------------
    Object parser.
    $ object := atom | complex | func.
------------------------------------------------------------------------------*/
bool pfg_parser::is_object()
{
    vl_iterator start=pos;
    RigPos rpMid=curRig->GetPos();
    long oldCurDelay=curDelay;
    long oldMaxDelay=maxDelay;
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> is_object start\n");
    fflush(dbg);
#endif
    //_0:
    if (is_atom()||is_complex()||is_func()) { goto _true; }
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_object false\n");
    fflush(dbg);
#endif
    if (error_flag)
    {
        error_flag  = false;
        error_begin = start;
        error_end   = pos;
        wcscpy(error_message,L"Object expected");
    };
    pos=start;
    curRig->Rollback(rpMid);
    curDelay=oldCurDelay;
    maxDelay=oldMaxDelay;
    return false;
_true:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_object true\n");
    fflush(dbg);
#endif
    return true;
}

/*------------------------------------------------------------------------------
    Function link parser.
------------------------------------------------------------------------------*/
bool pfg_parser::is_func()
{
    vl_iterator start=pos;
    RigPos rpMid=curRig->GetPos();
    long oldCurDelay=curDelay;
    long oldMaxDelay=maxDelay;
    SpecType stF;
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> is_func start\n");
    fflush(dbg);
#endif
    //_0:
    if (*pos==spec_add      )   { stF=addSpec;      goto _true;}
    if (*pos==spec_sub      )   { stF=minusSpec;    goto _true;}
    if (*pos==spec_mul      )   { stF=astSpec;      goto _true;}
    if (*pos==spec_div      )   { stF=slashSpec;    goto _true;}
    if (*pos==spec_idiv     )   { stF=percentSpec;  goto _true;}    
    if (*pos==spec_brackets )   { stF=dalistSpec;   goto _true;}
    if (*pos==spec_braces   )   { stF=palistSpec;   goto _true;}
    if (*pos==spec_cbrackets)   { stF=delistSpec;   goto _true;}
    if (*pos==spec_dot      )   { stF=dotSpec;      goto _true;}
    if (*pos==spec_ddot     )   { stF=dotsSpec;     goto _true;}
    if (*pos==spec_lower    )   { stF=ltSpec;       goto _true;}
    if (*pos==spec_greater  )   { stF=gtSpec;       goto _true;}
    if (*pos==spec_len      )   { stF=vertSpec;     goto _true;}
    if (*pos==spec_trans    )   { stF=sharpSpec;    goto _true;}
    if (*pos==spec_le       )   { stF=leSpec;       goto _true;}
    if (*pos==spec_ge       )   { stF=geSpec;       goto _true;}
    if (*pos==spec_equal    )   { stF=eqSpec;       goto _true;}
    if (*pos==spec_nequal   )   { stF=neSpec;       goto _true;}
    if (*pos==spec_quest    )   { stF=questSpec;    goto _true;}
    if (*pos==spec_dup      )   { stF=dupSpec;      goto _true;}
    if (*pos==kw_value      )   { stF=valSpec;      goto _true;}
    if (*pos==kw_type       )   { stF=typeSpec;     goto _true;}
    if (*pos==kw_in         )   { stF=inSpec;       goto _true;}
    if (*pos==kw_async      )   { stF=aslistSpec;   goto _true;}

    if (*pos==kw_signal     )   { stF=signalSpec;   goto _true;}
    if (*pos==kw_int        )   { stF=intSpec;      goto _true;}
    if (*pos==kw_float      )   { stF=floatSpec;    goto _true;}
    if (*pos==kw_char       )   { stF=charSpec;     goto _true;}
    if (*pos==kw_bool       )   { stF=boolSpec;     goto _true;}
    if (*pos==kw_datalist   )   { stF=dalistSpec;   goto _true;}
    if (*pos==kw_parlist    )   { stF=palistSpec;   goto _true;}
    if (*pos==kw_delaylist  )   { stF=delistSpec;   goto _true;}
    if (*pos==kw_async      )   { stF=aslistSpec;   goto _true;}

    //_false:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_func false\n");
    fflush(dbg);
#endif
    if (error_flag)
    {
        error_flag  = false;
        error_begin = start;
        error_end   = pos;
        wcscpy(error_message,L"Function expected");
    };
    pos=start;
    curRig->Rollback(rpMid);
    curDelay=oldCurDelay;
    maxDelay=oldMaxDelay;
    return false;
_true:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_func true\n");
    fflush(dbg);
#endif
    //error_flag=true;
    mlLastObj=MemLocation(spec, stF);
    pos++;
    return true;
}

/*------------------------------------------------------------------------------
    Atom parser.
    parses values of internal types: bool,int,float,string,error
------------------------------------------------------------------------------*/
bool pfg_parser::is_atom()
{
    vl_iterator start=pos;
    RigPos rpMid=curRig->GetPos();
    long oldCurDelay=curDelay;
    long oldMaxDelay=maxDelay;
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> is_atom start\n");
    fflush(dbg);
#endif
    //_0:
    if (is_error())
    {
        return true;
    }
    if (*pos==val_bool||*pos==val_int||*pos==val_float||*pos==val_str||*pos==val_char)
    {
        goto _true;
    };


    //_false:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_atom false\n");
    fflush(dbg);
#endif
    if (error_flag){
        error_flag = false;
        error_begin = start;
        error_end = pos;
        wcscpy(error_message,L"Atom expected");
    };
    pos=start;
    curRig->Rollback(rpMid);
    curDelay=oldCurDelay;
    maxDelay=oldMaxDelay;
    return false;
_true:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_atom true\n");
    fflush(dbg);
#endif
    if (*pos==val_bool)
    {
        // create new constant node with bool element inside
        mlLastObj=curRig->Append(new BoolValue(pos->b));
    };
    if (*pos==val_int)
    {
        // create new constant node with int element inside
        mlLastObj=curRig->Append(new IntValue(pos->i));
    };
    if (*pos==val_float)
    {
        // create new constant node with float element inside
        mlLastObj=curRig->Append(new FloatValue(pos->f));
    };
    if (*pos==val_str)
    {
        // create new constant node with datalist of chars element inside
        mlLastObj=curRig->Append(new DataListValue(QString::fromStdWString(pos->s)));
    };
    if (*pos==val_char)
    {
        // create new constant node with datalist of chars element inside
        mlLastObj=curRig->Append(new CharValue(pos->c));
    };
    pos++;
    return true;
}
/*------------------------------------------------------------------------------
    Error parser.
    parses values of internal error types
------------------------------------------------------------------------------*/
bool pfg_parser::is_error()
{
    ErrorType stErr;
    if (*pos==kw_basefuncerror) { stErr=BASEFUNCERROR;  goto _true;}
    if (*pos==kw_typeerror)     { stErr=TYPEERROR;      goto _true;}
    if (*pos==kw_bounderror)    { stErr=BOUNDERROR;     goto _true;}
    if (*pos==kw_interror)      { stErr=INTERROR;       goto _true;}
    if (*pos==kw_realerror)     { stErr=REALERROR;      goto _true;}
    if (*pos==kw_zerodivide)    { stErr=ZERODIVIDE;     goto _true;}
    if (*pos==kw_valueerror)    { stErr=VALUEERROR;     goto _true;}
    return false;
_true:
    mlLastObj=curRig->Append(new ErrorValue(stErr));
    pos++;
    return true;
}


/*------------------------------------------------------------------------------
    Complex parser.
    $ complex := datalist | parlist | delaylist | string.
------------------------------------------------------------------------------*/
bool pfg_parser::is_complex()
{
    vl_iterator start=pos;
    RigPos rpMid=curRig->GetPos();
    long oldCurDelay=curDelay;
    long oldMaxDelay=maxDelay;
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> is_complex start\n");
    fflush(dbg);
#endif
    //_0:
    if (is_datalist()   )   { goto _true;};
    if (is_parlist()    )   { goto _true;};
    if (is_delaylist()  )   { goto _true;};
    if (is_asynclist()  )   { goto _true;};
    if (*pos==val_str   )   { pos++; goto _true;};
    //_false:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_complex false\n");
    fflush(dbg);
#endif
    if (error_flag)
    {
        error_flag  = false;
        error_begin = start;
        error_end   = pos;
        wcscpy(error_message,L"Complex expected");
    };
    pos=start;
    curRig->Rollback(rpMid);
    curDelay=oldCurDelay;
    maxDelay=oldMaxDelay;
    return false;
_true:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_complex true\n");
    fflush(dbg);
#endif
    //error_flag=true;
    return true;
}

/*------------------------------------------------------------------------------
    Datalist parser.
    $ datalist := "(" element {"," element} ")".
------------------------------------------------------------------------------*/
bool pfg_parser::is_datalist()
{
    vl_iterator start=pos;
    RigPos rpMid=curRig->GetPos();
    long oldCurDelay=curDelay;
    long oldMaxDelay=maxDelay;
    DataListActor *dataListAct = new DataListActor();
    dataListAct->SetDelay(curDelay);
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> is_datalist start\n");
    fflush(dbg);
#endif
    //_0:
    if (*pos==obracket){pos++; goto _1;}
    goto _false;
_1:
    if (is_element())
    {
        dataListAct->AddToList(mlLastObj.GetMemType(),mlLastObj.GetIndex());
        goto _2;
    };
    goto _false;
_2:
    if (*pos==comma){pos++; goto _3;}
    if (*pos==cbracket){ pos++; goto _true;}
    goto _false;
_3:
    if (is_element())
    {
        dataListAct->AddToList(mlLastObj.GetMemType(),mlLastObj.GetIndex());
        goto _2;
    };
    goto _false;
_false:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_datalist false\n");
    fflush(dbg);
#endif
    if (error_flag)
    {
        error_flag  = false;
        error_begin = start;
        error_end   = pos;
        wcscpy(error_message,L"Datalist definition expected");
    };
    pos=start;
    curRig->Rollback(rpMid);
    curDelay=oldCurDelay;
    maxDelay=oldMaxDelay;
    delete dataListAct;
    return false;
_true:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_datalist true\n");
    fflush(dbg);
#endif
    dataListAct->pos.Set(start->pos.GetBeginRow(),start->pos.GetBeginCol(),(pos-1)->pos.GetEndRow(),(pos-1)->pos.GetEndCol());
    mlLastObj=curRig->Append(dataListAct);
    return true;
}

/*------------------------------------------------------------------------------
    Parallel list parser.
    $ parlist := "[" element {"," element} "]".
------------------------------------------------------------------------------*/
bool pfg_parser::is_parlist()
{
    vl_iterator start=pos;
    RigPos rpMid=curRig->GetPos();
    long oldCurDelay=curDelay;
    long oldMaxDelay=maxDelay;
    ParListActor *parListAct = new ParListActor();
    parListAct->SetDelay(curDelay);

#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> is_parlist start\n");
    fflush(dbg);
#endif
    //_0:
    if (*pos==obrace){pos++; goto _1;}
    goto _false;
_1:
    if (is_element())
    {
        parListAct->AddToList(mlLastObj.GetMemType(),mlLastObj.GetIndex());
        goto _2;
    };
    goto _false;
_2:
    if (*pos==comma){pos++; goto _3;}
    if (*pos==cbrace){ pos++; goto _true;}
    goto _false;
_3:
    if (is_element())
    {
        parListAct->AddToList(mlLastObj.GetMemType(),mlLastObj.GetIndex());
        goto _2;
    };
    goto _false;
_false:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_parlist false\n");
    fflush(dbg);
#endif
    if (error_flag)
    {
        error_flag  = false;
        error_begin = start;
        error_end   = pos;
        wcscpy(error_message,L"Parallel list definition expected");
    };
    pos=start;
    curRig->Rollback(rpMid);
    curDelay=oldCurDelay;
    maxDelay=oldMaxDelay;
    delete parListAct;
    return false;
_true:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_parlist true\n");
    fflush(dbg);
#endif
    parListAct->pos.Set(start->pos.GetBeginRow(),start->pos.GetBeginCol(),(pos-1)->pos.GetEndRow(),(pos-1)->pos.GetEndCol());
    mlLastObj=curRig->Append(parListAct);
    return true;
}

/*------------------------------------------------------------------------------
    Delay list parser.
    $ delaylist := "{" element {"," element} "}".
------------------------------------------------------------------------------*/
bool pfg_parser::is_delaylist()
{
    vl_iterator start=pos;
    RigPos rpMid=curRig->GetPos();
    long oldCurDelay=curDelay;
    long oldMaxDelay=maxDelay;
    maxDelay++;
    curDelay=maxDelay;
    ParListActor *parListAct = new ParListActor();
    parListAct->SetDelay(curDelay);

#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> is_delaylist start\n");
    fflush(dbg);
#endif
    //_0:
    if (*pos==ocbracket){pos++; goto _1;}
    goto _false;
_1:
    if (is_element())
    {
        parListAct->AddToList(mlLastObj.GetMemType(),mlLastObj.GetIndex());
        goto _2;
    };
    goto _false;
_2:
    if (*pos==comma){pos++; goto _3;}
    if (*pos==ccbracket){ pos++; goto _true;}
    goto _false;
_3:
    if (is_element())
    {
        parListAct->AddToList(mlLastObj.GetMemType(),mlLastObj.GetIndex());
        goto _2;
    };
    goto _false;
_false:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_delaylist false\n");
    fflush(dbg);
#endif
    if (error_flag)
    {
        error_flag = false;
        error_begin = start;
        error_end = pos;
        wcscpy(error_message,L"Delaylist definition expected");
    };
    pos=start;
    curRig->Rollback(rpMid);
    curDelay=oldCurDelay;
    maxDelay=oldMaxDelay;
    delete parListAct;
    return false;
_true:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_delaylist true\n");
    fflush(dbg);
#endif
    //if(parListAct->memList.size()>1)
    //{
        parListAct->pos.Set(start->pos.GetBeginRow(),start->pos.GetBeginCol(),(pos-1)->pos.GetEndRow(),(pos-1)->pos.GetEndCol());
        mlLastObj=curRig->Append(parListAct);
    //}
    bool listIsReal=false;
    long taDelay;
    if(mlLastObj.GetMemType()==act)
        if((taDelay=(*(curRig->GetActorList()))[mlLastObj.GetIndex()]->GetDelay())==curDelay)
        {
            mlLastObj=curRig->Append(new DelayValue(curDelay,mlLastObj.GetIndex()));
            listIsReal=true;
        }
    curDelay=oldCurDelay;
    if(!listIsReal)
        maxDelay=oldMaxDelay;
    return true;
}

/*------------------------------------------------------------------------------
    Asynchronous list parser.
    $ asynclist := "async" "(" element {"," element} ")".
------------------------------------------------------------------------------*/
bool pfg_parser::is_asynclist()
{
    vl_iterator start=pos;
    RigPos rpMid=curRig->GetPos();
    long oldCurDelay=curDelay;
    long oldMaxDelay=maxDelay;
    AsListActor *asListAct = new AsListActor();
    asListAct->SetDelay(curDelay);
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> is_asynclist start\n");
    fflush(dbg);
#endif
    //_0:
    if (*pos==kw_async){pos++; goto _1;}
    goto _false;
_1:
    if (*pos==obracket){pos++; goto _2;}
    goto _false;
_2:
    if (is_element())
    {
        asListAct->AddToList(mlLastObj.GetMemType(),mlLastObj.GetIndex());
        goto _3;
    };
    goto _false;
_3:
    if (*pos==comma){pos++; goto _4;}
    if (*pos==cbracket){ pos++; goto _true;}
    goto _false;
_4:
    if (is_element())
    {
        asListAct->AddToList(mlLastObj.GetMemType(),mlLastObj.GetIndex());
        goto _3;
    };
    goto _false;
_false:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_asynclist false\n");
    fflush(dbg);
#endif
    if (error_flag)
    {
        error_flag = false;
        error_begin = start;
        error_end = pos;
        wcscpy(error_message,L"Asynchronous list definition expected");
    };
    pos=start;
    curRig->Rollback(rpMid);
    curDelay=oldCurDelay;
    maxDelay=oldMaxDelay;
    delete asListAct;
    return false;
_true:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_asynclist true\n");
    fflush(dbg);
#endif
    asListAct->pos.Set(start->pos.GetBeginRow(),start->pos.GetBeginCol(),(pos-1)->pos.GetEndRow(),(pos-1)->pos.GetEndCol());
    mlLastObj=curRig->Append(asListAct);
    return true;
}




/*----------------------------------------------------------------------------*/
vl_iterator funcNamePos;
vl_iterator exprNamePos;
/*------------------------------------------------------------------------------
    Function/Type definition parser.
    $ function := "funcdef" [ argument ] "{" [ element {";" element }] "}".
------------------------------------------------------------------------------*/
bool pfg_parser::is_function()
{
    MemLocation externLocation;
    vl_iterator start=pos;
    curDelay=0;
    maxDelay=0;
    bool funcCreated=false;
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> is_function start\n");
    fflush(dbg);
#endif
    //_0:
    if (*pos==kw_funcdef){pos++; goto _0f;}
    if (*pos==kw_typedef){pos++; goto _0f;}
    if (*pos==kw_prefunc){pos++; goto _true;}
    goto _false;
_0f:
    //create function
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER is_function new funcdef %ls\n", funcNamePos->s);
    fflush(dbg);
#endif
    funcPos.push_back(Positions(funcNamePos->pos.GetAll()));
    curRig=new Rig(QString::fromStdWString(funcNamePos->s));
    funcRigs.push_back(curRig);
    //this is a first extern reference - on yourself
    //curRig->Append(new ExtRef(QString::fromStdWString(funcNamePos->s)));

    externLocation = curRig->Append(new ExtRef(QString::fromStdWString(funcNamePos->s)));
    set_var_id(funcNamePos->s,externLocation);

    funcCreated=true;
    goto _01;
_01:
    if (*pos==id)
    {
        BegActor *arg;
        arg=new BegActor();
        arg->SetDelay(0);
        arg->pos.Set(pos->pos.GetAll());
        curRig->Append(arg);
        set_var_id(pos->s,MemLocation(act,0));
        pos++;
        goto _1;
    }
    else
    {
        BegActor *arg;
        arg=new BegActor();
        arg->SetDelay(0);
        arg->pos.Set(pos->pos.GetAll());
        curRig->Append(arg);
        set_var_id(L"~",MemLocation(act,0));
    }
    goto _1;
_1:
    if (*pos==ocbracket){pos++; goto _2;}
    goto _false;
_2:
    if (*pos==ccbracket){pos++; goto _true;};
    if (is_element()){goto _3;};
    goto _false;
_3:
    if (*pos==ccbracket){pos++; goto _true;};
    if (*pos==scolon){pos++; goto _2;};
    goto _false;

_false:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_function false %d\n", error_flag);
    fflush(dbg);
#endif
    if(funcCreated)
    {
        funcPos.pop_back();
        funcRigs.pop_back();
    }
    if (error_flag)
    {
        error_flag  = false;
        error_begin = start;
        error_end   = pos;
        wcscpy(error_message,L"Function/type definition expected");
    };
    pos=start;
    return false;
_true:
#ifdef PARSER_DEBUG
    //последним объектом текущей функции является её закрывающая скобка
    //коль скоро мы тут - скобка была по адресу (pos-1)
    //записываем координаты этой скобки в конечные координаты позиций нашей функции
    vl_iterator funcEndPos=(pos-1);

    int curIndex=funcPos.size()-1;
    funcPos[curIndex].Set(funcPos[curIndex].GetBeginRow(),funcPos[curIndex].GetBeginCol(),
                       funcEndPos->pos.GetEndRow(),funcEndPos->pos.GetEndCol());

    fwprintf(dbg, L"  is_function true\n");
    fflush(dbg);
    mlVars.clear();
    mlVars.resize(100);
#endif
    //error_flag=true;
    return true;
}

/*------------------------------------------------------------------------------
    Block parser.
    $ block := "block" "{" [ element {";" element }] "}".
------------------------------------------------------------------------------*/
bool pfg_parser::is_block()
{
    vl_iterator start=pos;
    RigPos rpMid=curRig->GetPos();
    long oldCurDelay=curDelay;
    long oldMaxDelay=maxDelay;
    cur_block++;
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> is_block start\n");
    fflush(dbg);
#endif
    //_0:
    if (*pos==kw_block){pos++; goto _1;}
    goto _false;
_1:
    if (*pos==ocbracket){pos++; goto _2;}
    goto _false;
_2:
    if (*pos==ccbracket){pos++; goto _true;};
    if (is_element()){goto _3;};
    goto _false;
_3:
    if (*pos==ccbracket){pos++; goto _true;};
    if (*pos==scolon){pos++; goto _2;};
    goto _false;
_false:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_block false\n");
    fflush(dbg);
#endif
    mlVars[cur_block].clear();
    mlBlockBreaks[cur_block].clear();
    cur_block--;
    if (error_flag)
    {
        error_flag  = false;
        error_begin = start;
        error_end   = pos;
        wcscpy(error_message,L"Wrong block definition");
    };
    pos=start;
    curRig->Rollback(rpMid);
    curDelay=oldCurDelay;
    maxDelay=oldMaxDelay;
    return false;
_true:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_block true\n");
    fflush(dbg);
#endif
    if (mlBlockBreaks[cur_block].size()==0) goto _false;
    if (mlBlockBreaks[cur_block].size()>1) goto _false;
    mlLastObj=mlBlockBreaks[cur_block][0];
    mlBlockBreaks[cur_block].clear();
    mlVars[cur_block].clear();

    cur_block--;
    return true;
}

/*------------------------------------------------------------------------------
    Expression parser.
    $ expression := term {("^" expression | ":" term) [ else term]}.
    example: x, (x,2):-
------------------------------------------------------------------------------*/
bool pfg_parser::is_expression()
{
    vl_iterator start=pos;
    RigPos rpMid=curRig->GetPos();
    long oldCurDelay=curDelay;
    long oldMaxDelay=maxDelay;

    InterpActor *intAct;
    MemLocation nullLoc(loc,-1);

    vector<long> line;
    vector<MemLocation> mlLine;
    vector<Positions> posP;

    const long IDCOL=-1, IDANG=-2, IDDCOL=-3, IDDANG=-4;
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> is_expression start\n");
    fflush(dbg);
#endif
    //_0:
    if (is_term())
    {
        mlLine.push_back(mlLastObj);
        line.push_back(1);
        posP.push_back(Positions(-1,-1,-1,-1));
        goto _1;
    }
    goto _false;
_1:
    if (*pos==colon)
    {
        mlLine.push_back(nullLoc);
        line.push_back(IDCOL);
        posP.push_back(Positions(pos->pos.GetAll()));
        pos++;
        goto _2;
    };
    if (*pos==ang)
    {
        mlLine.push_back(nullLoc);
        line.push_back(IDANG);
        posP.push_back(Positions(pos->pos.GetAll()));
        pos++;
        goto _2;
    };
    if (*pos==dcolon)
    {
        mlLine.push_back(nullLoc);
        line.push_back(IDDCOL);
        posP.push_back(Positions(pos->pos.GetAll()));
        pos++;
        goto _2;
    };
    if (*pos==dang)
    {
        mlLine.push_back(nullLoc);
        line.push_back(IDDANG);
        posP.push_back(Positions(pos->pos.GetAll()));
        pos++;
        goto _2;
    };
    goto _true_id;
_2:
    if (is_term())
    {
        mlLine.push_back(mlLastObj);
        line.push_back(1);
        posP.push_back(Positions(-1,-1,-1,-1));
        goto _3;
    };
    goto _false;
_3:
    if (*pos==kw_else){pos++;goto _4;};
    goto _5;
_4:
    if (is_term()){goto _5;};
_5:
    if (*pos==colon)
    {
        mlLine.push_back(nullLoc);
        line.push_back(IDCOL);
        posP.push_back(Positions(pos->pos.GetAll()));
        pos++;
        goto _2;
    };
    if (*pos==ang)
    {
        mlLine.push_back(nullLoc);
        line.push_back(IDANG);
        posP.push_back(Positions(pos->pos.GetAll()));
        pos++;
        goto _2;
    };
    if (*pos==dcolon)
    {
        mlLine.push_back(nullLoc);
        line.push_back(IDDCOL);
        posP.push_back(Positions(pos->pos.GetAll()));
        pos++;
        goto _2;
    };
    if (*pos==dang)
    {
        mlLine.push_back(nullLoc);
        line.push_back(IDDANG);
        posP.push_back(Positions(pos->pos.GetAll()));
        pos++;
        goto _2;
    };
    goto _true;
_false:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_expression false\n");
    fflush(dbg);
#endif
    if (error_flag)
    {
        error_flag  = false;
        error_begin = start;
        error_end   = pos;
        wcscpy(error_message,L"Expression expected");
    };
    pos=start;
    curRig->Rollback(rpMid);
    curDelay=oldCurDelay;
    maxDelay=oldMaxDelay;
    return false;
_true:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_expression true\n");
    fflush(dbg);
#endif
    //error_flag=true;

    if(line.size()>1)
    {
        for(long i=line.size()-1;i>=0;i--)
        {
            if(line[i]==IDANG)
            {
                intAct=new InterpActor(mlLine[i+1],mlLine[i-1]);
                intAct->SetDelay(curDelay);
                intAct->pos.Set(posP[i].GetAll());

                line[i-1]=1;
                mlLastObj=curRig->Append(intAct);
                mlLine[i-1]=mlLastObj;

                mlLine.erase(mlLine.begin()+i+1);
                mlLine.erase(mlLine.begin()+i);

                line.erase(line.begin()+i+1);
                line.erase(line.begin()+i);

                posP.erase(posP.begin()+i+1);
                posP.erase(posP.begin()+i);
            }
        }
        for(long i=0;i<line.size();i++)
        {
            if(line[i]==IDCOL)
            {
                intAct=new InterpActor(mlLine[i-1],mlLine[i+1]);
                intAct->SetDelay(curDelay);
                intAct->pos.Set(posP[i].GetAll());

                line[i-1]=1;
                mlLastObj=curRig->Append(intAct);
                mlLine[i-1]=mlLastObj;

                mlLine.erase(mlLine.begin()+i+1);
                mlLine.erase(mlLine.begin()+i);

                line.erase(line.begin()+i+1);
                line.erase(line.begin()+i);

                posP.erase(posP.begin()+i+1);
                posP.erase(posP.begin()+i);
                i--;
            }
        }
    }
    mlLastObj=mlLine[0];
    return true;
_true_id:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_expression true id\n");
    fflush(dbg);
#endif
    return true;
}

/*------------------------------------------------------------------------------
    Term parser.
    $ term := object | block | id.
------------------------------------------------------------------------------*/
bool pfg_parser::is_term(){
    vl_iterator start=pos;
    RigPos rpMid=curRig->GetPos();
    long oldCurDelay=curDelay;
    long oldMaxDelay=maxDelay;
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> is_term start\n");
    fflush(dbg);
#endif
    //_0:
    if (is_object())    goto _true;
    if (is_block())     goto _true;
    if (*pos==id)       {pos++; goto _true;};
    //_false:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_term false\n");
    fflush(dbg);
#endif
    if (error_flag)
    {
        error_flag  = false;
        error_begin = start;
        error_end   = pos;
        wcscpy(error_message,L"Term expected");
    };
    pos=start;
    curRig->Rollback(rpMid);
    curDelay=oldCurDelay;
    maxDelay=oldMaxDelay;
    return false;
_true:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_term true\n");
    fflush(dbg);
#endif
    pos--;
    if (*pos==id)
    {
        bool var_found=false;
        for (long bi=cur_block; bi>=0; bi--)
            if (mlVars[bi].find(pos->s)!=mlVars[bi].end())	//object found in var-table
            {
                mlLastObj=mlVars[bi][pos->s];
                var_found=true;
                break;
            };
        if (!var_found)						//object is extern
            mlLastObj=curRig->Append(new ExtRef(QString::fromStdWString(pos->s)));
    }
    pos++;
    return true;
}

/*------------------------------------------------------------------------------
    Program parser.
    $ program := described { ";" described }.
------------------------------------------------------------------------------*/
bool pfg_parser::is_program()
{
    vl_iterator start=pos;
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> is_program start\n");
    fflush(dbg);
#endif


    if(!get_imports())
    {
#ifdef PARSER_DEBUG
        fwprintf(dbg, L"PARSER imports false\n");
        fflush(dbg);
#endif
        return false;
    }

    if(!check_imports())
    {
#ifdef PARSER_DEBUG
        fwprintf(dbg, L"PARSER imports' check false\n");
        fflush(dbg);
#endif
        return false;
    }

_0:

#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER is_program parse: %d\n",pos-buf);
    fflush(dbg);
#endif

    if (pos-buf>=len) {goto _true;}
    if (*pos==no_lexema) {goto _true;}
    if (*pos==scolon){pos++; goto _0;}
    if (is_described()){goto _0;}
    if (is_expr_design()){ goto _0;}
    //goto _true;

    //_false:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_program false\n");
    fflush(dbg);
#endif
    if (error_flag)
    {
        error_flag  = false;
        error_begin = start;
        error_end   = pos;
        wcscpy(error_message,L"Program expected");
    };
    pos=start;
    return false;
_true:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_program true\n");
    fflush(dbg);
#endif
    //error_flag=true;
    return true;
}

/*------------------------------------------------------------------------------
    Described function/type/constant parser.
------------------------------------------------------------------------------*/
bool pfg_parser::is_described()
{
    vl_iterator start=pos;
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> is_described start\n");
    fflush(dbg);
#endif
    //_0:
    if (*pos==id){funcNamePos=pos; pos++; goto _1;}
    goto _false;
_1:
    if (*pos==obrace)       {pos++; goto _2;}
    goto _4;
_2:
    if (*pos==val_int)      {pos++; goto _3;}          //TOTAL IGNORE FOR NOW
    if (*pos==val_float)    {pos++; goto _3;}          //TOTAL IGNORE FOR NOW
    goto _false;
_3:
    if (*pos==cbrace)       {pos++; goto _4;}
    goto _false;
_4:
    if (*pos==spec_lassign) {pos++; goto _5;}
    goto _false;
_5:
    if (is_description())   {goto _true;};
    goto _false;

_false:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_described false\n");
    fflush(dbg);
#endif
    if (error_flag)
    {
        error_flag = false;
        error_begin = start;
        error_end = pos;
        wcscpy(error_message,L"Described expected");
    };
    pos=start;
    return false;
_true:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_described true\n");
    fflush(dbg);
#endif
    //error_flag=true;
    return true;
}

/*------------------------------------------------------------------------------
    Description parser.
------------------------------------------------------------------------------*/
bool pfg_parser::is_description()
{
    vl_iterator start=pos;
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> is_description start\n");
    fflush(dbg);
#endif
    //_0:
    if (is_function()){
        goto _true;
    };
    goto _false;
_false:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_description false\n");
    fflush(dbg);
#endif
    if (error_flag)
    {
        error_flag  = false;
        error_begin = start;
        error_end   = pos;
        wcscpy(error_message,L"Description expected");
    };
    pos=start;
    return false;
_true:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_description true\n");
    fflush(dbg);
#endif
    //error_flag=true;
    return true;
}

/*------------------------------------------------------------------------------
    Get imports of modules
------------------------------------------------------------------------------*/
bool pfg_parser::get_imports()
{
_0:
    if (*pos==kw_import) {pos++; goto _1;}
    if (*pos==scolon)    {pos++; goto _0;}
    goto _true;
_1:
    if (*pos==id)
    {
        imports.push_back(pos->s);
        pos++;
        goto _0;
    }
    //_false:
    error_flag = false;
    error_begin = pos;
    error_end = pos;
    wcscpy(error_message,L"Import error: module name not found");
    return false;
_true:
    return true;
}
/*------------------------------------------------------------------------------
    Check imported modules
------------------------------------------------------------------------------*/
bool pfg_parser::check_imports()
{
    //TODO - something
    return true;
}

/*------------------------------------------------------------------------------
    Expression designation parser
------------------------------------------------------------------------------*/
bool pfg_parser::is_expr_design()
{
    vl_iterator start=pos;
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> is_expr_design start\n");
    fflush(dbg);
#endif
    curRig=new Rig("expressia_tempora");
    if (is_expr_l())   { goto _true;};
    if (is_expr_r())   { goto _true;};
    goto _false;
_false:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_expr_design false\n");
    fflush(dbg);
#endif
    if (error_flag)
    {
        error_flag  = false;
        error_begin = start;
        error_end   = pos;
        wcscpy(error_message,L"Error: expression designation expected");
    };
    pos=start;
    return false;
_true:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< expression true\n");
    fflush(dbg);
#endif

    funcPos.push_back(Positions(exprNamePos->pos.GetAll()));
    curRig->SetName(QString::fromStdWString(exprNamePos->s));
    funcRigs.push_back(curRig);

    MemLocation externLocation = curRig->Append(new ExtRef(QString::fromStdWString(exprNamePos->s)));
    set_var_id(exprNamePos->s,externLocation);

    return true;
}
/*------------------------------------------------------------------------------
    Left expression designation parser
------------------------------------------------------------------------------*/
bool pfg_parser::is_expr_l()
{
    //TODO - left expressions
    vl_iterator start=pos;
    RigPos rpMid=curRig->GetPos();
    long oldCurDelay=curDelay;
    long oldMaxDelay=maxDelay;
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> is_expr_l parse start\n");
    fflush(dbg);

#endif
    //_0:
    if (*pos==id)   {exprNamePos=pos; pos++; goto _1;};
    goto _false;
_1:
    if (*pos==spec_lassign) {pos++; goto _3;};
    goto _false;
_3:
    if (is_expression() )   goto _true;
    if (is_object()     )   goto _true;
    if (is_block()      )   goto _true;
    goto _false;
_false:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_expr_l false\n");
    fflush(dbg);
#endif
    if (error_flag)
    {
        error_flag  = false;
        error_begin = start;
        error_end   = pos;
        wcscpy(error_message,L"Error in expression's left designation statement (>>)");
    };
    pos=start;
    curRig->Rollback(rpMid);
    curDelay=oldCurDelay;
    maxDelay=oldMaxDelay;
    return false;
_true:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_expr_l true %ls=node %ld\n", start->s, mlLastObj.GetIndex());
    fflush(dbg);
#endif
    pos++;
    return true;
}
/*------------------------------------------------------------------------------
    Left expression designation parser
------------------------------------------------------------------------------*/
bool pfg_parser::is_expr_r()
{
    //TODO - right expressions
    vl_iterator start=pos;
    RigPos rpMid=curRig->GetPos();
    long oldCurDelay=curDelay;
    long oldMaxDelay=maxDelay;
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER>> is_expr_r parse start\n");
    fflush(dbg);

#endif
    //_0:
    if (is_expression() )   goto _1;
    if (is_object()     )   goto _1;
    if (is_block()      )   goto _1;
    goto _false;
_1:
    if (*pos==spec_rassign) {pos++; goto _3;};
    goto _false;
_3:
    if (*pos==id)   {exprNamePos=pos; goto _true;};
    goto _false;
_false:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_expr_r false\n");
    fflush(dbg);
#endif
    if (error_flag)
    {
        error_flag  = false;
        error_begin = start;
        error_end   = pos;
        wcscpy(error_message,L"Error in expression's right designation statement (>>)");
    };
    pos=start;
    curRig->Rollback(rpMid);
    curDelay=oldCurDelay;
    maxDelay=oldMaxDelay;
    return false;
_true:
#ifdef PARSER_DEBUG
    fwprintf(dbg, L"PARSER<< is_expr_r true %ls=node %ld\n", start->s,  mlLastObj.GetIndex());
    fflush(dbg);
#endif
    //error_flag=true;
    pos++;
    return true;
}


#endif



