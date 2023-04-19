/*==============================================================================
    File: pfg_scanner.cpp

    Description: 
        Pifagor programming language scanner(lexical analyzer).
        Input: unicode text.
        Output: lexema array, debug information.

    Credits: AUTHORS

    License: GNU General Public License Version 3 or later
             http://www.gnu.org/licenses/gpl.html
==============================================================================*/

#ifndef PFG_SCANNER_CPP
#define PFG_SCANNER_CPP

#include "pfg_scanner2.h"
#include <iostream>
#include <wctype.h>
#include <errno.h>
/*----------------------------------------------------------------------------*/
pfg_scanner::pfg_scanner()
{
    s=new wchar_t[1024];
    s[0]=L'\0';
    str_pos=1;
    str_num=1;
    start_row=1;
    start_col=1;
    cur_i=0;
    buf=new lexema[LEX_BUF_SIZE];
    for (int i=0; i<LEX_BUF_SIZE ; i++)
    {
        buf[i].s=new wchar_t[256];
    }
}

/*----------------------------------------------------------------------------*/
pfg_scanner::pfg_scanner(FILE* in, FILE* newErr, FILE* newDbg)
{
    err=newErr;
    dbg=newDbg;
    s=new wchar_t[1024];
    s[0]=L'\0';
    str_pos=0;
    str_num=1;
    start_row=1;
    start_col=0;
    cur_i=0;
    buf=new lexema[LEX_BUF_SIZE];
    for (int i=0; i<LEX_BUF_SIZE; i++)
    {
        buf[i].s=new wchar_t[256];
    }
    read(in);
    buf_len=cur_i;
}
/*----------------------------------------------------------------------------*/
pfg_scanner::~pfg_scanner()
{
    delete[] s;
    for (int i=0; i<LEX_BUF_SIZE; i++)
        delete[] buf[i].s;
    delete[] buf;
}

/*----------------------------------------------------------------------------*/
wchar_t pfg_scanner::next(FILE* in)
{
    wchar_t c;
    errno=0;
    c = fgetwc(in);
    if (errno==EILSEQ)
    {
        fwprintf(err,L"SCANNER wrong character encoding\n");
        fflush(err);
    };
#ifdef SCANNER_DEBUG
    fwprintf(dbg, L"SCANNER next '%lc' \n",c);
    fflush(dbg);
#endif
    str_pos++;
    if(c==WEOF) c=0;
    return c;
}

/*----------------------------------------------------------------------------*/
void pfg_scanner::add_char(wchar_t c)
{
    long len=wcslen(s);
#ifdef SCANNER_DEBUG
    fwprintf(dbg, L"SCANNER add_char len:%ld \n",len);
    fflush(dbg);
#endif
    s[len+1]=L'\0';
    s[len]=c;
}
void pfg_scanner::late_event(lex e)
{
    bool fl=(str_pos>1);
    if(fl)
        str_pos--;
    event(e);
    if(fl)
        str_pos++;
}

/*----------------------------------------------------------------------------*/
void pfg_scanner::event(lex e)
{
    //fwprintf(dbg,L"event\n");fflush(dbg);
    buf[cur_i].pos.Set(start_row,start_col,str_num,str_pos);
    start_row=str_num;
    start_col=str_pos;
    buf[cur_i]._type=e;
    if (e==id)
    {
        wcscpy(buf[cur_i].s,s);
    };
    if (e==val_str)
    {
        wcscpy(buf[cur_i].s,s);
    };
    if (e==val_char)
    {
        buf[cur_i].c=QChar(s[0]);
    };
    if (e==val_bool)
    {
        if (wcscmp(s,L"true")==0) buf[cur_i].b=true;
        else buf[cur_i].b=false;
    }
    if (e==val_int)
    {
        errno=0;
        buf[cur_i].i=wcstol(s,NULL,0);
        if (errno==ERANGE||errno==EINVAL)
        {
            fwprintf(err,L"SCANNER integer conversion error\n");
            fflush(err);
        };
    };
    if (e==val_float)
    {
        buf[cur_i].f=wcstod(s,NULL);
        if (errno==ERANGE||errno==EINVAL)
        {
            fwprintf(err,L"SCANNER float conversion error\n");
            fflush(err);
        };
    };

#ifdef SCANNER_DEBUG
 if (e==id)              { fwprintf(dbg,L"id %ls %d\n",s,cur_i);  };
    if (e==val_str)         { fwprintf(dbg,L"val_str %ls\n",s);      };
    if (e==val_bool)        { fwprintf(dbg,L"val_bool %ls\n",s);     };
    if (e==val_int)         { fwprintf(dbg,L"val_int %ls\n",s);      };
    if (e==val_float)       { fwprintf(dbg,L"val_float %ls\n",s);    };
    if (e==no_lexema)       { fwprintf(dbg,L"no lexema\n");          };
    if (e==end_file)        { fwprintf(dbg,L"end_file\n");           };
    if (e==spec_add)        { fwprintf(dbg,L"spec_add\n");           };
    if (e==spec_sub)        { fwprintf(dbg,L"spec_sub\n");           };
    if (e==spec_mul)        { fwprintf(dbg,L"spec_mul\n");           };
    if (e==spec_div)        { fwprintf(dbg,L"spec_div\n");           };
    if (e==spec_idiv)       { fwprintf(dbg,L"spec_idiv\n");          };
    if (e==spec_brackets)   { fwprintf(dbg,L"spec_brackets\n");      };
    if (e==spec_braces)     { fwprintf(dbg,L"spec_braces\n");        };
    if (e==spec_cbrackets)  { fwprintf(dbg,L"spec_cbrackets\n");     };
    if (e==spec_dot)        { fwprintf(dbg,L"spec_dot\n");           };
    if (e==spec_ddot)       { fwprintf(dbg,L"spec_ddot\n");          };
    if (e==spec_lower)      { fwprintf(dbg,L"spec_lower\n");         };
    if (e==spec_greater)    { fwprintf(dbg,L"spec_greater\n");       };
    if (e==spec_len)        { fwprintf(dbg,L"spec_len\n");           };
    if (e==spec_trans)      { fwprintf(dbg,L"spec_trans\n");         };
    if (e==spec_le)         { fwprintf(dbg,L"spec_le\n");            };
    if (e==spec_ge)         { fwprintf(dbg,L"spec_ge\n");            };
    if (e==spec_equal)      { fwprintf(dbg,L"spec_equal\n");         };
    if (e==spec_nequal)     { fwprintf(dbg,L"spec_nequal\n");        };
    if (e==spec_quest)      { fwprintf(dbg,L"spec_quest\n");         };
    if (e==spec_dup)        { fwprintf(dbg,L"spec_dup\n");           };
    if (e==spec_lassign)    { fwprintf(dbg,L"spec_lassign\n");       };
    if (e==spec_rassign)    { fwprintf(dbg,L"spec_rassign\n");       };
    if (e==comma)           { fwprintf(dbg,L"comma\n");              };
    if (e==quote)           { fwprintf(dbg,L"quote\n");              };
    if (e==ang)             { fwprintf(dbg,L"ang\n");                };
    if (e==dang)            { fwprintf(dbg,L"dang\n");                };
    if (e==colon)           { fwprintf(dbg,L"colon\n");              };
    if (e==dcolon)          { fwprintf(dbg,L"dcolon\n");             };
    if (e==scolon)          { fwprintf(dbg,L"scolon\n");             };
    if (e==cbracket)        { fwprintf(dbg,L"cbracket\n");           };
    if (e==obracket)        { fwprintf(dbg,L"obracket\n");           };
    if (e==cbrace)          { fwprintf(dbg,L"cbrace\n");             };
    if (e==obrace)          { fwprintf(dbg,L"obrace\n");             };
    if (e==ccbracket)       { fwprintf(dbg,L"ccbracket\n");          };
    if (e==ocbracket)       { fwprintf(dbg,L"ocbracket\n");          };
    if (e==comment)         { fwprintf(dbg,L"comment\n");            };
    if (e==kw_block)        { fwprintf(dbg,L"kw_block\n");           };
    if (e==kw_nil)          { fwprintf(dbg,L"kw_nil\n");             };
    //if (e==kw_break)        { fwprintf(dbg,L"kw_break\n");           };
    if (e==kw_datalist)     { fwprintf(dbg,L"kw_datalist\n");        };
    if (e==kw_parlist)      { fwprintf(dbg,L"kw_parlist\n");         };
    if (e==kw_type)         { fwprintf(dbg,L"kw_type\n");            };
    if (e==kw_delaylist)    { fwprintf(dbg,L"kw_delaylist\n");       };
    if (e==kw_funcdef)      { fwprintf(dbg,L"kw_funcdef\n");         };
    if (e==kw_prefunc)      { fwprintf(dbg,L"kw_prefunc\n");         };
    if (e==kw_typedef)      { fwprintf(dbg,L"kw_typedef\n");         };
    if (e==kw_const)        { fwprintf(dbg,L"kw_const\n");           };
    if (e==kw_else)         { fwprintf(dbg,L"kw_else\n");            };
    if (e==kw_value)        { fwprintf(dbg,L"kw_value\n");           };
    if (e==kw_in)           { fwprintf(dbg,L"kw_in\n");              };
    if (e==kw_async)        { fwprintf(dbg,L"kw_async\n");           };
    if (e==kw_import)       { fwprintf(dbg,L"kw_import\n");          };

    if (e==kw_signal)       { fwprintf(dbg,L"kw_signal\n");          };
    if (e==kw_int)          { fwprintf(dbg,L"kw_int\n");            };
    if (e==kw_float)        { fwprintf(dbg,L"kw_float\n");           };
    if (e==kw_bool)         { fwprintf(dbg,L"kw_bool\n");            };
    if (e==kw_char)         { fwprintf(dbg,L"kw_char\n");            };

    if (e==kw_basefuncerror)    { fwprintf(dbg,L"kw_basefuncerror\n");          };
    if (e==kw_typeerror)        { fwprintf(dbg,L"kw_typeerror\n");            };
    if (e==kw_bounderror)       { fwprintf(dbg,L"kw_bounderror\n");           };
    if (e==kw_interror)         { fwprintf(dbg,L"kw_interror\n");            };
    if (e==kw_realerror)        { fwprintf(dbg,L"kw_realerror\n");            };
    if (e==kw_zerodivide)       { fwprintf(dbg,L"kw_zerodivide\n");            };
    if (e==kw_valueerror)       { fwprintf(dbg,L"kw_valueerror\n");            };

    fflush(dbg);
#endif

    s[0]=L'\0';
    cur_i++;
};

/*----------------------------------------------------------------------------*/
void pfg_scanner::read(FILE* in){
    int cnt=0;
    wchar_t c=0;
    c=next(in);
_0:
    cnt++;
    if (c==0)           { event(no_lexema);                 goto _end;      };
    if (c==L'+')        { event(spec_add);      c=next(in); goto _0;        };
    if (c==L'*')        { event(spec_mul);      c=next(in); goto _0;        };
    if (c==L'%')        { event(spec_idiv);     c=next(in); goto _0;        };
    if (c==L'#')        { event(spec_trans);    c=next(in); goto _0;        };
    if (c==L'?')        { event(spec_quest);    c=next(in); goto _0;        };
    if (c==L'|')        { event(spec_len);      c=next(in); goto _0;        };
    if (c==L',')        { event(comma);         c=next(in); goto _0;        };
    if (c==L'^')        {                       c=next(in); goto _ang;      };
    if (c==L':')        {                       c=next(in); goto _colon;    };
    if (c==L';')        { event(scolon);        c=next(in); goto _0;        };
    if (c==L')')        { event(cbracket);      c=next(in); goto _0;        };
    if (c==L']')        { event(cbrace);        c=next(in); goto _0;        };
    if (c==L'}')        { event(ccbracket);     c=next(in); goto _0;        };
    if (c==L'/')        {                       c=next(in); goto _slash;    };
    if (c==L'-')        { add_char(c);          c=next(in); goto _minus;    };
    if (c==L'(')        {                       c=next(in); goto _s;        };
    if (c==L'[')        {                       c=next(in); goto _ks;       };
    if (c==L'{')        {                       c=next(in); goto _fs;       };
    if (c==L'.')        {                       c=next(in); goto _dot;      };
    if (c==L'<')        {                       c=next(in); goto _l;        };
    if (c==L'>')        {                       c=next(in); goto _g;        };
    if (c==L'!')        {                       c=next(in); goto _excl;     };
    if (c==L'=')        {                       c=next(in); goto _e;        };
    if (c==L'"')        {                       c=next(in); goto _str;      };
    if (c==L'\'')       {                       c=next(in); goto _char;      };
    if (is_digit(c))    { add_char(c);          c=next(in); goto _num;      };
    if (is_letter(c))   { add_char(c);          c=next(in); goto _id;       };
    if (is_skip(c))
    {
        if(c!=L'\n'){
            start_col++;
            if(c==L'\t') start_col+=TAB_SIZE;
            str_pos=start_col;
        }
        else{
            start_row++;
            start_col=0;
            str_num=start_row;
            str_pos=start_col;
        }
        c=next(in);
        goto _0;
    };
    c=next(in);         goto _0;
_colon:
    if (c==L':')        { event(dcolon);        c=next(in); goto _0;        };
    late_event(colon);       goto _0;
_ang:
    if (c==L'^')        { event(dang);        	c=next(in); goto _0;        };
    late_event(ang);       goto _0;
_slash:
    if (c==L'/')        {                       c=next(in); goto _dslash;   };
    if (c==L'*')        {                       c=next(in); goto _comm_open;};
    late_event(spec_div);    goto _0;
_dslash:
    //if (c==L'\n')     {                       c=next(in); goto _0;        };
    if (c==0)           {                       goto _0;        };
    if (c==L'\n')       {                       goto _0;        };
    c=next(in);         goto _dslash;
_comm_open:
    if (c==L'\n')
    {
        start_row++;
        start_col=0;
        str_num=start_row;
        str_pos=start_col;
    };
    if (c==L'*')        {                       c=next(in); goto _comm_star;};
    c=next(in);         goto _comm_open;
_comm_star:
    if (c==L'/')        {                       c=next(in); goto _0;        };
    goto _comm_open;
_minus:
    if (is_digit(c))    { add_char(c);          c=next(in); goto _num;      };
    late_event(spec_sub);    goto _0;
_num:
    if (is_digit(c))    { add_char(c);          c=next(in); goto _num;      };
    if (c==L'.')        { add_char(c);          c=next(in); goto _num_float;};
    if (c==L'e')        { add_char(c);          c=next(in); goto _num_float_power;};
    if (c==L'E')        { add_char(c);          c=next(in); goto _num_float_power;};
    late_event(val_int);     goto _0;
_num_float:
    if (is_digit(c))    { add_char(c);          c=next(in); goto _num_float; };
    if (c==L'e')        { add_char(c);          c=next(in); goto _num_float_power; };
    if (c==L'E')        { add_char(c);          c=next(in); goto _num_float_power; };
    late_event(val_float);   goto _0;
_num_float_power:
    if (c==L'-')        { add_char(c);          c=next(in); goto _num_float_power_signed; };
    if (c==L'+')        { add_char(c);          c=next(in); goto _num_float_power_signed; };
    if (is_digit(c))    { add_char(c);          c=next(in); goto _num_float_power; };
    late_event(val_float);   goto _0;
_num_float_power_signed:
    if (is_digit(c))    { add_char(c);          c=next(in); goto _num_float_power; };
    late_event(val_float);   goto _0;
_id:
    if (is_letter(c))   { add_char(c);          c=next(in); goto _id;       }
    if (c==L'.')        { add_char(c);          c=next(in); goto _id;       }
    if (is_digit(c))    { add_char(c);          c=next(in); goto _id;       }
    else
    {
        if (wcscmp(s,L"dup")        == 0)   { late_event(spec_dup);      goto _0; };
        if (wcscmp(s,L"block")      == 0)   { late_event(kw_block);      goto _0; };
        if (wcscmp(s,L"nil")        == 0)   { late_event(kw_nil);        goto _0; };
        if (wcscmp(s,L"datalist")   == 0)   { late_event(kw_datalist);   goto _0; };

        if (wcscmp(s,L"parlist")    == 0)   { late_event(kw_parlist);    goto _0; };
        if (wcscmp(s,L"type")       == 0)   { late_event(kw_type);       goto _0; };

        if (wcscmp(s,L"delaylist")  == 0)   { late_event(kw_delaylist);  goto _0; };
        if (wcscmp(s,L"funcdef")    == 0)   { late_event(kw_funcdef);    goto _0; };
        if (wcscmp(s,L"prefunc")    == 0)   { late_event(kw_prefunc);    goto _0; };
        if (wcscmp(s,L"typedef")    == 0)   { late_event(kw_typedef);    goto _0; };

        if (wcscmp(s,L"const")      == 0)   { late_event(kw_const);      goto _0; };
        if (wcscmp(s,L"else")       == 0)   { late_event(kw_else);       goto _0; };
        if (wcscmp(s,L"true")       == 0)   { late_event(val_bool);      goto _0; };
        if (wcscmp(s,L"false")      == 0)   { late_event(val_bool);      goto _0; };
        if (wcscmp(s,L"value")      == 0)   { late_event(kw_value);      goto _0; };
        if (wcscmp(s,L"in")         == 0)   { late_event(kw_in);         goto _0; };
        if (wcscmp(s,L"async")      == 0)   { late_event(kw_async);      goto _0; };
        if (wcscmp(s,L"import")     == 0)   { late_event(kw_import);     goto _0; };
        if (wcscmp(s,L"signal")     == 0)   { late_event(kw_signal);     goto _0; };
        if (wcscmp(s,L"int")        == 0)   { late_event(kw_int);       goto _0; };
        if (wcscmp(s,L"float")      == 0)   { late_event(kw_float);      goto _0; };
        if (wcscmp(s,L"bool")       == 0)   { late_event(kw_bool);       goto _0; };
        if (wcscmp(s,L"char")       == 0)   { late_event(kw_char);       goto _0; };

        if (wcscmp(s,L"BASEFUNCERROR") == 0)    { late_event(kw_basefuncerror);       goto _0; };
        if (wcscmp(s,L"TYPEERROR") == 0)        { late_event(kw_typeerror);       goto _0; };
        if (wcscmp(s,L"BOUNDERROR") == 0)       { late_event(kw_bounderror);       goto _0; };
        if (wcscmp(s,L"INTERROR") == 0)         { late_event(kw_interror);       goto _0; };
        if (wcscmp(s,L"REALERROR") == 0)        { late_event(kw_realerror);       goto _0; };
        if (wcscmp(s,L"ZERODIVIDE") == 0)       { late_event(kw_zerodivide);       goto _0; };
        if (wcscmp(s,L"VALUEERROR") == 0)       { late_event(kw_valueerror);       goto _0; };

    };
    late_event(id);          goto _0;
_s:
    if (c==L')')        { event(spec_brackets); c=next(in);     goto _0;    };
    late_event(obracket);    goto _0;
_ks:
    if (c==L']')        { event(spec_braces);   c=next(in);     goto _0;    };
    late_event(obrace);      goto _0;
_fs:
    if (c==L'}')        { event(spec_cbrackets);c=next(in);     goto _0;    };
    late_event(ocbracket);   goto _0;
_dot:
    if (c==L'.')        { event(spec_ddot);     c=next(in);     goto _0;    };
    late_event(spec_dot);    goto _0;
_l:
    if (c==L'<')        { event(spec_lassign);  c=next(in);     goto _0;    };
    if (c==L'=')        { event(spec_le);       c=next(in);     goto _0;    };
    late_event(spec_lower);  goto _0;
_g:
    if (c==L'>')        { event(spec_rassign);  c=next(in);     goto _0;    };
    if (c==L'=')        { event(spec_ge);       c=next(in);     goto _0;    };
    late_event(spec_greater);goto _0;
_excl:
    if (c==L'=')        { event(spec_nequal);   c=next(in);     goto _0;    };
    goto _error;
_e:
    if (c==L'>')        { event(spec_ge);       c=next(in);     goto _0;    };
    if (c==L'<')        { event(spec_le);       c=next(in);     goto _0;    };
    late_event(spec_equal);  goto _0;
_char:
    add_char(c); c=next(in); ;
    if (c==L'\'')        { event(val_char);       c=next(in);     goto _0;    };
    goto _error;
_str:
    if (c==L'"')        { event(val_str);       c=next(in);     goto _0;    };
    add_char(c); c=next(in); goto _str;
_error:;
_end:;
}

void pfg_scanner::print(FILE *out)
{
    int i=0;
    fprintf(out,"%ld lexems:\n",cur_i);
    char *lexems[]= {
        (char *)"no_lexema",        // 0
        (char *)"end_file",         // 1
        (char *)"spec_add",         // 2 +
        (char *)"spec_sub",         // 3 -
        (char *)"spec_mul",         // 4 *
        (char *)"spec_div",         // 5 /
        (char *)"spec_idiv",        // 6 %
        (char *)"spec_brackets",    // 7 ()
        (char *)"spec_braces",      // 8 []
        (char *)"spec_cbrackets",   // 9 {}
        (char *)"spec_dot",         // 10 .
        (char *)"spec_ddot",        // 11 ..
        (char *)"spec_lower",       // 12 <
        (char *)"spec_greater",     // 13 >
        (char *)"spec_len",         // 14 |
        (char *)"spec_trans",       // 15 #
        (char *)"spec_le",          // 16 <= / =<
        (char *)"spec_ge",          // 17 >= / =>
        (char *)"spec_equal",       // 18 =
        (char *)"spec_nequal",      // 19 !=
        (char *)"spec_quest",       // 20 ?
        (char *)"spec_dup",         // 21 dup
        (char *)"spec_lassign",     // 22 <<
        (char *)"spec_rassign",     // 23 >>
        (char *)"comma",            // 24 ,
        (char *)"quote",            // 25 '
        (char *)"ang",              // 26 ^
        (char *)"dang",             // 27 ^^
        (char *)"colon",            // 28 :
        (char *)"dcolon",           // 29 ::
        (char *)"scolon",           // 30 ;
        (char *)"cbracket",         // 31 )
        (char *)"obracket",         // 32 (
        (char *)"cbrace",           // 33 ]
        (char *)"obrace",           // 34 [
        (char *)"ccbracket",        // 35 }
        (char *)"ocbracket",        // 36 {
        (char *)"comment",          // 37 commentary
        (char *)"id",               // 38 identifier
        (char *)"val_bool",         // 39 bool value
        (char *)"val_int",          // 40 int value
        (char *)"val_float",        // 41 float value
        (char *)"val_str",          // 42 string value
        (char *)"kw_block",         // 43 keyword block
        (char *)"kw_nil",           // 44 keyword nil
        (char *)"kw_datalist",      // 45 keyword datalist
        (char *)"kw_int",           // 46 keyword int
        (char *)"kw_float",         // 47 keyword float
        (char *)"kw_parlist",       // 48 keyword parlist
        (char *)"kw_type",          // 49 keyword type
        (char *)"kw_bool",          // 50 keyword bool
        (char *)"kw_delaylist",     // 51 keyword delaylist
        (char *)"kw_funcdef",       // 52 keyword funcdef
        (char *)"kw_prefunc",       // 53 keyword prefunc
        (char *)"kw_typedef",       // 54 keyword typedef
        (char *)"kw_char",          // 55 keyword char
        (char *)"kw_const",         // 56 keyword const
        (char *)"kw_else",          // 57 keyword else
        (char *)"kw_value",         // 58 keyword value
        (char *)"kw_in",            // 59 keyword in
        (char *)"kw_async",         // 60 keyword async
        (char *)"kw_import",        // 61 keyword import
        (char *)"kw_signal",        // 62 keyword signal
        //errors
        (char *)"kw_basefuncerror",
        (char *)"kw_typeerror",
        (char *)"kw_bounderror",
        (char *)"kw_interror",
        (char *)"kw_realerror",
        (char *)"kw_zerodivide",
        (char *)"kw_valueerror"
    };
    for(i=0;i<cur_i;i++)
    {
        fprintf(out,"%d: %s",i+1, lexems[buf[i]._type]);
        if(buf[i]._type==id)
            fprintf(out," %ls",buf[i].s);
        fprintf(out," from %d:%d to %d:%d",buf[i].pos.GetBeginRow(),buf[i].pos.GetBeginCol(),buf[i].pos.GetEndRow(),buf[i].pos.GetEndCol());
        fprintf(out,"\n");
        fflush(dbg);
    }
}

/*----------------------------------------------------------------------------*/
inline bool is_digit(wchar_t c)
{
    return iswdigit(c)!=0;
}

/*----------------------------------------------------------------------------*/
inline bool is_letter(wchar_t c)
{
    return ((iswalpha(c)!=0)||(c==L'_'));
}

/*----------------------------------------------------------------------------*/
inline bool is_skip(wchar_t c)
{
    return (c==L'\r'||c==L'\n'||c==L'\t'||c==L' ');
}

#endif

