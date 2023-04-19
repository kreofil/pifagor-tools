/*==============================================================================
   File: tester.cpp

   Description:
	   Autotest system
==============================================================================*/
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <locale.h>
#include <vector>
#include <iostream>

using namespace std;

#define MAX_STR_LEN 1000

int str,pos,allPos;
char resChar,idealChar;


//////////////////////////////////////////
//	Input arguments:		//
//		[-hide] [<listfile>]	//
//////////////////////////////////////////

void test_arg_error()
{
	printf("\nWrong input format. Correct format:\n");
	printf("\t[-hide] [<listfile>]\n");
}

bool compFiles(FILE* res, FILE *ideal)
{
	str=1;
	pos=1;
    allPos=0;

    if(res==NULL || ideal==NULL)
        return false;

	fscanf(res,"%c",&resChar);
	fscanf(ideal,"%c",&idealChar);
	while(!feof(res))
	{	
		//printf("%c(%d) vs %c(%d)\n",resChar,resChar,idealChar,idealChar);
		if(feof(res)!=feof(ideal) || resChar!=idealChar)
			return false;
		fscanf(res,"%c",&resChar);
		fscanf(ideal,"%c",&idealChar);		
		pos++;
        allPos++;
		if(resChar=='\n')
		{
			str++;
			pos=0;
		}
	}
	return true;
}
char* readString(FILE *in)
{
    char* rez;
    rez=new char[MAX_STR_LEN];
    bool isGood=false;
    do
    {
        isGood=true;
        fgets(rez,MAX_STR_LEN,in);
        if(feof(in))
            return NULL;
        if(strstr(rez,"//")==rez)
            isGood=false;
        if(rez[0]=='\n')
            isGood=false;
    }while(!isGood);
    char *end=strchr(rez,'\n');
    if(end!=NULL)
        *end='\0';
    return rez;
}

int main(int argc, char *argv[])
{
    setlocale(LC_CTYPE,"en_US.UTF-8");
	char listFileName[MAX_STR_LEN];
    char *launchStr;
    char *outFileName;
    char *idealFileName;
	FILE* listFile;
	FILE* outFile;
	FILE* idealFile;
	int testCount;
	int failedCount;
	vector<int> failedTests;
	bool isHide;
    int launchRet;
	isHide=false;

    strcpy(listFileName,"../tests/list.txt");

	if(argc==2)
	{
		if(!strcmp(argv[1],"-hide"))
			isHide=true;
		else
			strcpy(listFileName,argv[1]);
	}
	if(argc==3)
	{
		if(!strcmp(argv[1],"-hide"))
			isHide=true;
		else
		{
			test_arg_error();
			return EXIT_FAILURE;
		}
		strcpy(listFileName,argv[2]);
	}
	if(argc>3)
	{
		test_arg_error();
		return EXIT_FAILURE;
	}



	listFile=fopen(listFileName,"r");
    if(listFile==NULL)
    {
        printf("can't open listFile\n");
        return 0;
    }

	testCount=0;

    while(true)
    {
        launchStr=readString(listFile);
        if(launchStr==NULL)
            break;
        outFileName=readString(listFile);
        if(outFileName==NULL)
            break;
        idealFileName=readString(listFile);
        if(idealFileName==NULL)
            break;


        testCount++;
        if(isHide)
            sprintf(launchStr,"%s 1>__o.txt 2>__e.txt",launchStr);
        printf("Launch string #%d: %s\n",testCount, launchStr);
        printf("Out file #%d: %s\n",testCount, outFileName);
        printf("Ideal file #%d: %s\n",testCount, idealFileName);


        if((launchRet=system(launchStr))!=0)
        {
            printf("test failed: program returned non-zero value (%d)\n\n", launchRet);
            failedTests.push_back(testCount);
            continue;
        }
        outFile=fopen(outFileName,"r");
        idealFile=fopen(idealFileName,"r");
        printf("#%d: ",testCount);
        if(outFile==NULL)
            printf("can't open outFile\n");
        if(idealFile==NULL)
            printf("can't open idealFile\n");

        if(compFiles(outFile,idealFile))
            printf("%d characters checked\ntest passed\n",allPos);
        else
        {
            printf("%d characters checked\ntest failed at str %d, pos %d\n",allPos,str,pos);
            printf("%c got, %c expected\n",resChar,idealChar);
            failedTests.push_back(testCount);
        }

        fclose(outFile);
        fclose(idealFile);

        printf("\n");

    }
    failedCount=failedTests.size();
    printf("Passed %d tests of %d\n",testCount-failedCount, testCount);
    printf("Failed %d tests of %d\n",failedCount, testCount);
    if(failedCount)
    {
        printf("Failed test(s): ");
        for(int i=0;i<failedCount;i++)
            printf("%d ",failedTests[i]);
        printf("\n");
    }

	return EXIT_SUCCESS;
}

