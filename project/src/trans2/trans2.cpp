/*==============================================================================
   File: trans2.cpp

   Description:
       Pifagor RIG translator 2.0

   Credits: AUTHORS

   License: GNU General Public License Version 3 or later
            http://www.gnu.org/licenses/gpl.html
==============================================================================*/

#include <cstdlib>
#include <string.h>
#include <wchar.h>
#include <locale.h>

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <direct.h>
#endif

using namespace std;

#include "pfg_parser2.h"
#include <../repository/repFacade.h>
#include <../repository/msgServer.h>
#include "../repository/quickMsg.h"
#include "../rig2dot/rig2dot.h"

#define TRANS_SUCCESS    0
#define ARGNUM_ERROR    1
#define INFILE_ERROR    2
#define ERRFILE_ERROR    3
#define DBGFILE_ERROR    4
#define TRANS_ERROR    5

#define NO_DEBUG_HEAP

/*========================================================
    Input arguments:
        <key> <input_file> [<error_file> [<debug_file>]]
    where:
        <key> is
            -c for constant translation
            -t for text output format
========================================================*/

void trans_arg_error()
{
    QuickMsg msg(WRITE_PRINT,"transArgError");
    MsgServer* msgServer=MsgServer::getMsgServer();
    msgServer->turnOne("transArgError",true);

    msg.write("Wrong input format. Correct format:");
    msg.write("\t<key> <input_file> [error_file> [debug_file]]");
    msg.write("where <key> is");
    msg.write("\t-c for constant translation");
    msg.write("\t-t for text output format");
    msg.write("\t-r for text output format and translation from repository pfg");
    msg.write("\t-i for single function inplace translation (result file autosaves in same directory as source code");
}

int main(int argc, char *argv[]) {

    MsgServer* msgServer=MsgServer::getMsgServer();
    msgServer->turnAll(true);

    QuickMsg msg(WRITE_PRINT,"trans");
    msgServer->turnOne("trans",true);

    setlocale(LC_CTYPE,"en_US.UTF-8");
    //setlocale(LC_CTYPE,"ru_RU.UTF-8");

    FILE* in    = 0; //input
    FILE* err   = 0; //translation errors output
    FILE* dbg   = 0; //debug information

    NameMaker nameMaker;

    if (argc < 3)                                                               //некорректное число аргументов
    {
        trans_arg_error();
        return ARGNUM_ERROR;
    };

    if(strcmp(argv[1],"-r") && strcmp(argv[1],"-t")
            && strcmp(argv[1],"-c") && strcmp(argv[1],"-i"))   //некорректные входные аргументы
    {
        trans_arg_error();
        return ARGNUM_ERROR;
    }


    if(!strcmp(argv[1],"-r"))
    {
        in=fopen(nameMaker.getCodeName(argv[2]).toUtf8().data(),"r");
    }
    else
    {
        in   = fopen(argv[2],"r");
    }
    
    // Если не указаны четвертый (а следовательно и пятый) аргументы, то имена файлов ошибок назначаются по умолчанию.
    // error.txt - файл ошибок по умолчанию.
    // debug.txt - файл отладочной информации по умолчанию
    // Файлы создаются в текущем каталоге программы (пока).

    if(argc < 4) {
      err = fopen("error.txt", "w");
      dbg = fopen("debug.txt", "w");
    } else if(argc < 5) {
      err  = fopen(argv[3],"w");
      dbg = fopen("debug.txt", "w");
    } else {
      err  = fopen(argv[3],"w");
      dbg  = fopen(argv[4],"w");
    }

    //file error handling
    if (in == 0)
    {
        msg.write("Input file error");
        fwprintf(stdout, L"Input file opening error.\n");
        return INFILE_ERROR;
    };
    if (err == 0)
    {
        msg.write("Errors file error");
        fwprintf(stdout, L"Errors file opening error.\n");
        return ERRFILE_ERROR;
    };
    if (dbg == 0)
    {
        msg.write("Debug file error");
        fwprintf(stdout, L"Debug information file opening error.\n");
        return DBGFILE_ERROR;
    };


    //translation
    pfg_scanner my_scaner(in, err, dbg);
    my_scaner.print(dbg);

    pfg_parser my_parser;
    if(strcmp(argv[1],"-c"))
    {
        if (!my_parser.parse(&my_scaner))
        {
            //Translation error handling
            msg.write("Program error\n");
            fwprintf(err, L"Program error %d %d\n", my_parser.error_begin->pos.GetBeginRow(), my_parser.error_begin->pos.GetBeginCol());          
            return TRANS_ERROR;
        };
    }
    else
    {
        if (!my_parser.parse_const(&my_scaner))
        {
            //Translation error handling
            msg.write("Constant error\n");
            fwprintf(err, L"Constant error %d %d\n", my_parser.error_begin->pos.GetBeginRow(), my_parser.error_begin->pos.GetBeginCol());
            return TRANS_ERROR;
        };
    }

    fclose(in);

    RepFacade* repFacade=new RepFacade;
    repFacade->init();

    QString data;
    char* funcName;
    Rig* curRig;

    if (!strcmp(argv[1],"-i"))
    {
        curRig=my_parser.funcRigs[0];           //TODO больше одной функции неприемлемо!
        funcName=new char[strlen(argv[2])+10];
        strcpy(funcName,argv[2]);
        char *index=strstr(funcName,".pfg\0");
        strcpy(index,".rig\0");
        curRig->SaveText(funcName);
    }

    if (!strcmp(argv[1],"-t") || !strcmp(argv[1],"-r"))                       //text
    {
        for(unsigned long i=0;i<my_parser.funcRigs.size();i++)
        {
            curRig=my_parser.funcRigs[i];
            funcName=curRig->GetCharName();
            data=curRig->GetTextRepresentation();
            repFacade->saveRig(funcName,data);
        }
    }

    if(!strcmp(argv[1],"-c"))                       //translated constant saving
    {
        my_parser.funcRigs[0]->SaveText("arg.rig");        
    }


    if(!strcmp(argv[1],"-r"))                       //-r - symtabs
    {
        for(unsigned long i=0;i<my_parser.funcPos.size()-1;i++)
        {
            curRig=my_parser.funcRigs[i];
            funcName=curRig->GetCharName();
            data=curRig->GetSymbolTable();
            repFacade->saveSymbolTable(funcName,data);
        }
    }

    Positions curPos;
    if(!strcmp(argv[1],"-t"))                       //-t - source and symtabs
    {
        for(unsigned long i=0;i<my_parser.funcPos.size()-1;i++)
        {
            //позиция функции
            //...либо от начала файла до конца текущей функции
            if(!i)
            {
                curPos.Set(1,1,my_parser.funcPos[i].GetEndRow(),my_parser.funcPos[i].GetEndCol());
            }
            //...либо от конца прошлой до конца текущей функции
            else
            {
                curPos.Set(my_parser.funcPos[i-1].GetEndRow(),my_parser.funcPos[i-1].GetEndCol()+1,
                        my_parser.funcPos[i].GetEndRow(),my_parser.funcPos[i].GetEndCol());
            }
            repFacade->saveCode(argv[2],curPos,my_parser.funcRigs[i]->GetCharName());
            curRig=my_parser.funcRigs[i];
            funcName=curRig->GetCharName();
            data=curRig->GetSymbolTable();
            repFacade->saveSymbolTable(funcName,data);
        }
    }

    delete repFacade;
    fclose(err);
    fclose(dbg);
    msg.write("Translation of "+QString(argv[2])+" completed successfully");
    return TRANS_SUCCESS;
}
