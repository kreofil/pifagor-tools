/*==============================================================================
   File: link.cpp

   Description:
       Pifagor linker

   Credits: AUTHORS

   License: GNU General Public License Version 3 or later
            http://www.gnu.org/licenses/gpl.html
==============================================================================*/

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
  #include <direct.h>
#else
  #include <sys/stat.h>
  #include <sys/types.h>
#endif

using namespace std;

#include "linker.h"

/*========================================================
    Input arguments:
        <funcname>
    where:
        <funcname> - name of called function
========================================================*/

void link_arg_error()
{
    printf("\nWrong input format. Correct format:\n");
    printf("\t<funcname>\n");
    printf("where <funcname> is name of called function\n");
}

int main(int argc, char *argv[])
{

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    mkdir("./out");
    mkdir("./out/link");
#else
    mkdir("./out",0777);
    mkdir("./out/link",0777);
#endif

    if(argc<2)
    {
        link_arg_error();
        return ARGNUM_ERROR;
    }

    Linker myLink;
    if(myLink.Link(argv[1])==LINK_ERROR)
    {
            printf("link failed\n");
            return LINK_ERROR;
    }
    myLink.PrintAll();

    printf("success");
    return SUCCESS;
}

