/*==============================================================================
   File: cgen2.cpp

   Description:
		PIFAGOR control graph generator

   Credits: AUTHORS

   License: GNU General Public License Version 3 or later
            http://www.gnu.org/licenses/gpl.html
==============================================================================*/

#include <cstdlib>
#include <cstring>
#include <wchar.h>
#include <locale.h>

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    #include <direct.h>
#endif

using namespace std;
			   
#include "../repository/repFacade.h"
#include "../repository/msgServer.h"
#include "../repository/quickMsg.h"
#include "../rigparser/rigparser.h"
#include "../cg/cg.h"
#include "../cg2dot/cg2dot.h"

#define CGEN_SUCCESS	0
#define ARGNUM_ERROR	1
#define INFILE_ERROR	2

/*============================================
    Input arguments:
        -f <input_file> <output_file>
        -n <function_name>
============================================*/

void cgen_arg_error()
{
	printf("\nWrong input format. Correct formats:\n");
    printf("\t-f <input_file> <output_file>\n");
    printf("\t-n <function_name>\n");
}

int main(int argc, char *argv[])
{
    setlocale(LC_CTYPE,"en_US.UTF-8");
	if(argc<3)
	{
		cgen_arg_error();
		return ARGNUM_ERROR;
	}
    if(strcmp(argv[1],"-n") && strcmp(argv[1],"-f"))
    {
        cgen_arg_error();
        return ARGNUM_ERROR;
    }

    Rig rig;
    RigParser* parser;
    Cg cg;

    MsgServer* msgServer=MsgServer::getMsgServer();
    msgServer->turnAll(false);

    RepFacade* repFacade=new RepFacade();

    QuickMsg msg(WRITE_PRINT,"cgen");
    msgServer->turnOne("cgen",true);

    bool result;
    if(!strcmp(argv[1],"-n"))         //-n
    {
        QString rigText=repFacade->getRigStr(argv[2]);
        if(rigText=="")
        {
            msg.write("Can't read the RIG! Error");
            return INFILE_ERROR;
        }
        parser=new RigParser(&rig,rigText,true);

        result=parser->Start();
        if(!result)
        {
            delete repFacade;
            delete parser;
            msg.write("Input file error");
            return INFILE_ERROR;
        }
        cg.Transform(parser->GetRig());

        QString data;
        //making output
        data=cg.GetTextRepresentation();
        repFacade->saveCg(argv[2],data);
    }
    if(!strcmp(argv[1],"-f"))         //-f
    {
        parser=new RigParser(&rig,QString(argv[2]));
        result=parser->Start();
        if(!result)
        {
            delete repFacade;
            delete parser;
            msg.write("Input file error");
            return INFILE_ERROR;
        }
        cg.Transform(parser->GetRig());
        //making output
        cg.SaveText(argv[3]);
    }
    delete repFacade;
    delete parser;
    msg.write("CG generation for "+QString(argv[2])+" completed successfully");
    return CGEN_SUCCESS;
}

