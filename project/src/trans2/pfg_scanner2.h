/*==============================================================================
    File: pfg_scanner.h

    Description: 
        Pifagor programming language scanner(lexical analyzer).
        Input: unicode text.
        Output: lexema array, debug information.

    Credits: AUTHORS

    License: GNU General Public License Version 3 or later
             http://www.gnu.org/licenses/gpl.html
==============================================================================*/

#ifndef PFG_SCANNER_H
#define PFG_SCANNER_H

#include <stdio.h>
#include <wchar.h>
#include <string>
#include <vector>
#include "../rig/actor.h"
using namespace std;


#define LEX_BUF_SIZE 10000	//размер буфера лексем
#define TAB_SIZE 7 //размер табуляции (без 1 символа)

/*------------------------------------------------------------------------------
    Enum: lex_type
        Lexema types
------------------------------------------------------------------------------*/
typedef enum lex_type
{
    no_lexema=0,
    end_file,
    spec_add,       // +
    spec_sub,       // -
    spec_mul,       // *
    spec_div,       // /
    spec_idiv,      // %
    spec_brackets,  // ()
    spec_braces,    // []
    spec_cbrackets, // {}
    spec_dot,       // .
    spec_ddot,      // ..
    spec_lower,     // <
    spec_greater,   // >
    spec_len,       // |
    spec_trans,     // #
    spec_le,        // <= / =<
    spec_ge,        // >= / =>
    spec_equal,     // =
    spec_nequal,    // !=
    spec_quest,     // ?
    spec_dup,       // dup
    spec_lassign,   // <<
    spec_rassign,   // >>
    comma,          // ,
    quote,          // '
    ang,            // ^
    dang,           // ^^
    colon,          // :
    dcolon,         // ::
    scolon,         // ;
    cbracket,       // )
    obracket,       // (
    cbrace,         // ]
    obrace,         // [
    ccbracket,      // }
    ocbracket,      // {
    comment,        // commentary
    id,             // identifier
    val_bool,       // bool value
    val_int,        // int value
    val_float,      // float value
    val_char,       // char value
    val_str,        // string value
    kw_block,       // keyword block
    kw_nil,         // keyword nil
    //kw_break,     // keyword break
    kw_datalist,    // keyword datalist
    kw_int,         // keyword int
    kw_float,       // keyword float
    kw_parlist,     // keyword parlist
    kw_type,        // keyword type
    kw_bool,        // keyword bool
    kw_delaylist,   // keyword delaylist
    kw_funcdef,     // keyword funcdef
    kw_prefunc,     // keyword prefunc
    kw_typedef,     // keyword typedef
    kw_char,        // keyword char
    kw_const,       // keyword const
    kw_else,        // keyword else
    kw_value,       // keyword value
    kw_in,          // keyword in
    kw_async,       // keyword async
    kw_import,      // keyword import
    kw_signal,       // keyword signal
    //keywords for error types
    kw_basefuncerror,
    kw_typeerror,
    kw_bounderror,
    kw_interror,
    kw_realerror,
    kw_zerodivide,
    kw_valueerror
} lex;

/*------------------------------------------------------------------------------
  Struct: lexema
    _type 	- lexema's type
    c       - char data
    s		- string data
    i		- long (integer) data
    f		- double (real) data
    b		- bool (logical) data
    str		- string number (in source code)
    pos		- position in string (in source code)
------------------------------------------------------------------------------*/
struct lexema
{
    lex _type;
    QChar c;
    wchar_t* s;
    long i;
    double f;
    bool b;
    Positions pos;
};

/*------------------------------------------------------------------------------
  Type: lexema buffer iterator
------------------------------------------------------------------------------*/
typedef lexema* vl_iterator;


bool is_digit(wchar_t c);		//is character a digit?
bool is_letter(wchar_t c);		//is character a letter?
bool is_skip(wchar_t c);		//is character a skippable?

/*------------------------------------------------------------------------------
   Class: pfg_scanner
     Pifagor language scanner

    wchar_t* s;			- string for names and constant values
    int str_num;		- number of current string of source code
    int str_pos;		- number of position in current string of source code

    lexema* buf;		- lexema buffer
    long buf_len;		- length of lexema buffer
    int cur_i;			- current position in lexema buffer

    err				- error output file
    dbg				- debug information file

    void event(lex e);						- adding new lexema to buffer
    wchar_t next(FILE* in);					- getting new symbol from source code
    void add_char(wchar_t c);					- adding new symbol to "s" array

    pfg_scanner();						- empty constructor
    pfg_scanner(FILE* in, FILE* newErr, FILE* newDbg);		- work constructor
    void read(FILE* in);					- scan of source code
    void print(FILE *out);					- printing of lexema buffer
------------------------------------------------------------------------------*/
class pfg_scanner
{
protected:
    wchar_t* s;
    int start_row;
    int start_col;
    int str_num;
    int str_pos;
    void late_event(lex e);
    void event(lex e);
    wchar_t next(FILE* in);
    void add_char(wchar_t c);

public:
    lexema* buf;
    long buf_len;
    long cur_i;

    FILE* err;
    FILE* dbg;

    pfg_scanner();
    pfg_scanner(FILE* in, FILE* newErr, FILE* newDbg);
    ~pfg_scanner();
    void read(FILE* in);
    void print(FILE *out);
};

#endif

