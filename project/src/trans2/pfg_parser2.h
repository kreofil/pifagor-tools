/*==============================================================================
	File: pfg_parser.h

	Description: 
		Pifagor programming language parser(syntax analyzer).
		Input: array of lexema, debug information.
		Output: information graph, debug information.

	Credits: AUTHORS

	License: GNU General Public License Version 3 or later
			 http://www.gnu.org/licenses/gpl.html
==============================================================================*/

#ifndef PFG_PARSER_H
#define PFG_PARSER_H

#include "pfg_scanner2.h"
#include "../rig/rig.h"
#include <iostream>
#include <map>
#include <cstring>
#include <vector>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <stack>


using namespace std;

#define PARSER_DEBUG	

/*------------------------------------------------------------------------------
  Class: pfg_parser
	PIFAGOR programming language parser 
------------------------------------------------------------------------------*/
class pfg_parser
{
protected:
	FILE* err;					//file for error messages
	FILE* dbg;					//file for debug information
public:
    vector<Rig*> funcRigs;      //function rigs
    Rig *curRig;                //current rig
    MemLocation mlLastObj;      //last created object's MemLoc
    Value*      vLastObj;       //last created object (for constant parsing)
    vector< map<wstring,MemLocation> > mlVars;      //table of variables
    vector< vector<MemLocation> > mlBlockBreaks;    //list of block roots
    long curDelay;
    long maxDelay;
    void set_var_id(wstring name, MemLocation id);

	bool get_imports		();		//parsing "import" section
	bool check_imports	  	();		//reading imported modules' nametables
	
	bool is_designation_l   	();		//left designation check
	bool is_designation_r   	();		//right designation check
	bool is_element			();		//element check
	bool is_object			();		//object check
	bool is_func			();		//internal function check
	bool is_atom			();		//atomary element check
    bool is_error           ();		//error element check
	bool is_complex		 	();		//complex element check
	bool is_datalist		();		//data list check
	bool is_parlist		 	();		//parallel list check
	bool is_delaylist	   	();		//delayed list check
	bool is_asynclist	   	();		//asyncronous list check

	bool is_function		();		//function check
	bool is_block		   	();		//block check
	bool is_expression	  	();		//expression check
	bool is_term			();		//term check

	bool is_program		 	();		//program check
	bool is_described	   	();		//described expression check
	bool is_description	 	();		//description check

	bool is_const		   	();		//constant check
    bool is_const_datalist  ();		//constant data list check
    bool is_const_parlist   ();     //constant parallel list check

	bool is_expr_design	 	();		//expression designation check
	bool is_expr_l			();		//left expression designation check
	bool is_expr_r			();		//right expression designation check

	lexema* buf;					//lexema buffer
	lexema* pos;					//current position in lexema buffer
	long len;					//length of lexema buffer

	vector<wstring> imports;			//list of imported modules
	vector<wstring> ntFuncs;			//external function nametable

	vector<wstring> mbFuncs;			//potential functions nametable

    vector<Positions> funcPos;      //functions' positions

	long cur_block;					//block level

	bool	 error_flag;				//is any error found?
	lexema*  error_begin;				//where error begins?
	lexema*  error_end;				//where error ends?
	wchar_t* error_message;				//error message

	pfg_parser();					//constructor (empty)
    ~pfg_parser();
	bool parse(pfg_scanner*);			//parsing program
	bool parse_const(pfg_scanner*);			//parsing constant (argument)
};

#endif

