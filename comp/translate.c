/****************************************************************************
*
* Filename: translate.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  N compiler: "automatic" opcode translation to assembly
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2012
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
*
***************************************************************************/

#include "host.h"
#include "global_d.h"
#include "parser.h"

extern struct opcode opcode;
extern struct src_line src_line;

extern struct varlist *varlist;
extern struct varlist *pvarlist_obj;

extern struct varlist_state varlist_state;
extern struct varlist_state pvarlist_state;

extern S2 plist_ind;
extern struct jumplist *jumplist;
extern struct plist *plist;

extern S4 jumplist_ind;

/*
struct translate
{
    U1 opcode[MAXLINELEN + 1];
    U1 argnum;
    S2 args[3];
    S2 trans[2][5];
    S2 transarg;
    S2 target;
};
*/

struct translate translate[] =
{
    { "array2var", 3, { ARRAY_VAR, INTEGER_VAR, ALL },
    {{ INT_VAR, LINT_VAR, QINT_VAR, DOUBLE_VAR, STRING_VAR, BYTE_VAR }, { LETARRAY2_I, LETARRAY2_L, LETARRAY2_Q, LETARRAY2_D, LETARRAY2_S, LETARRAY2_B }},
    3, 3 },
    { "var2array", 3, { ALL, ARRAY_VAR, INTEGER_VAR },
    {{ INT_VAR, LINT_VAR, QINT_VAR, DOUBLE_VAR, STRING_VAR, BYTE_VAR }, { LET2ARRAY_I, LET2ARRAY_L, LET2ARRAY_Q, LET2ARRAY_D, LET2ARRAY_S, LET2ARRAY_B }},
    1, EMPTY },
    { "locate", 2, { INTEGER_VAR, INTEGER_VAR, EMPTY },
    {{ INT_VAR, LINT_VAR, BYTE_VAR, EMPTY, EMPTY, EMPTY }, { LOCATE, LOCATE, LOCATE, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "input", 1, { ALL, EMPTY, EMPTY },
    {{ INT_VAR, LINT_VAR, QINT_VAR, DOUBLE_VAR, STRING_VAR, BYTE_VAR }, { INPUT_L, INPUT_L, INPUT_L, INPUT_D, INPUT_S, INPUT_L }},
    1, 1 },
    { "inputchar", 1, { ALL, EMPTY, EMPTY },
    {{ INT_VAR, LINT_VAR, QINT_VAR, DOUBLE_VAR, STRING_VAR, BYTE_VAR }, { INPUT_L, INPUT_L, INPUT_L, INPUT_D, INPUT_S, INPUT_L }},
    1, 1 },
    { "part2string", 3, { STRING_VAR, INTEGER_VAR, STRING_VAR },
    {{ INT_VAR, LINT_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { MOVEP2STR, MOVEP2STR, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 3 },
    { "string2part", 3, { STRING_VAR, STRING_VAR, INTEGER_VAR },
    {{ INT_VAR, LINT_VAR, EMPTY, EMPTY, EMPTY }, { MOVESTR2P, MOVESTR2P, EMPTY, EMPTY, EMPTY }},
    DIRECT, 3 },
    { "strlen", 2, { STRING_VAR, INTEGER_VAR, EMPTY },
    {{ INT_VAR, LINT_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { STRLEN, STRLEN, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "strleft", 3, { STRING_VAR, INTEGER_VAR, STRING_VAR },
    {{ INT_VAR, LINT_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { STRLEFT, STRLEFT, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 3 },
    { "strright", 3, { STRING_VAR, INTEGER_VAR, STRING_VAR },
    {{ INT_VAR, LINT_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { STRRIGHT, STRRIGHT, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 3 },
    { "ucase", 1, { STRING_VAR, EMPTY, EMPTY },
    {{ STRING_VAR, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { UCASE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 1 },
    { "lcase", 1, { STRING_VAR, EMPTY, EMPTY },
    {{ STRING_VAR, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { UCASE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 1 },
    { "char", 2, { INTEGER_VAR, STRING_VAR, EMPTY },
    {{ INT_VAR, LINT_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { CHAR, CHAR, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "asc", 2, { STRING_VAR, INTEGER_VAR, EMPTY },
    {{ INT_VAR, LINT_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { ASC, ASC, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },

    { "printsp", 1, { INTEGER_VAR, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { PRINT_SPACE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "curson", 0, { EMPTY, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { CURSON, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "cursoff", 0, { EMPTY, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { CURSOFF, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "cursleft", 1, { INTEGER_VAR, EMPTY, EMPTY },
    {{ INT_VAR, LINT_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { CURSLEFT, CURSLEFT, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "cursright", 1, { INTEGER_VAR, EMPTY, EMPTY },
    {{ INT_VAR, LINT_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { CURSRIGHT, CURSRIGHT, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "cursup", 1, { INTEGER_VAR, EMPTY, EMPTY },
    {{ INT_VAR, LINT_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { CURSUP, CURSUP, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "cursdown", 1, { INTEGER_VAR, EMPTY, EMPTY },
    {{ INT_VAR, LINT_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { CURSDOWN, CURSDOWN, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "timeron", 0, { EMPTY, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { TIMERON, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "timeroff", 0, { EMPTY, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { TIMEROFF, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "time", 0, { EMPTY, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { TIME, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "waitsec", 1, { INTEGER_VAR, EMPTY, EMPTY },
    {{ INT_VAR, LINT_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { WAIT_SEC, WAIT_SEC, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "waittick", 1, { INTEGER_VAR, EMPTY, EMPTY },
    {{ INT_VAR, LINT_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { WAIT_TICK, WAIT_TICK, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },

    { "fopen", 3, { INTEGER_VAR, STRING_VAR, STRING_VAR },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { FOPEN, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 1 },
    { "fclose", 1, { INTEGER_VAR, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { FCLOSE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "fread", 2, { INTEGER_VAR, ALL, EMPTY },
    {{ BYTE_VAR, INT_VAR, LINT_VAR, QINT_VAR, DOUBLE_VAR, STRING_VAR }, { FREAD_BYTE, FREAD_INT, FREAD_LINT, FREAD_QINT, FREAD_DOUBLE, FREAD_STRING }},
    2, 2 },
    { "freadab", 3, { INTEGER_VAR, BYTE_VAR, INTEGER_VAR },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { FREAD_ARRAY_BYTE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "freadls", 2, { INTEGER_VAR, STRING_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { FREAD_LINE_STRING, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },

    { "fwrite", 2, { INTEGER_VAR, ALL, EMPTY },
    {{ BYTE_VAR, INT_VAR, LINT_VAR, QINT_VAR, DOUBLE_VAR, STRING_VAR }, { FWRITE_BYTE, FWRITE_INT, FWRITE_LINT, FWRITE_QINT, FWRITE_DOUBLE, FWRITE_STRING }},
    2, EMPTY },
    { "fwriteab", 3, { INTEGER_VAR, BYTE_VAR, INTEGER_VAR },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { FWRITE_ARRAY_BYTE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "fwritesl", 2, { INTEGER_VAR, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { FWRITE_STR_LINT, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "fwritesd", 2, { INTEGER_VAR, DOUBLE_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { FWRITE_STR_DOUBLE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "fwriten", 2, { INTEGER_VAR, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { FWRITE_NEWLINE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "fwritesp", 2, { INTEGER_VAR, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { FWRITE_SPACE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "fsetpos", 2, { INTEGER_VAR, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { FSETPOS, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "fgetpos", 2, { INTEGER_VAR, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { FGETPOS, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "frewind", 1, { INTEGER_VAR, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { FREWIND, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "fsize", 2, { INTEGER_VAR, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { FSIZE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "fremove", 1, { STRING_VAR, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { FREMOVE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "frename", 2, { STRING_VAR, STRING_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { FRENAME, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },

    { "ssopen", 3, { INTEGER_VAR, STRING_VAR, INTEGER_VAR },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SOPEN_SERVER, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 1 },
    { "ssopenact", 2, { INTEGER_VAR, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SOPEN_ACT_SERVER, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "scopen", 3, { INTEGER_VAR, STRING_VAR, INTEGER_VAR },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SOPEN_CLIENT, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 1 },
    { "ssclose", 1, { INTEGER_VAR, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SCLOSE_SERVER, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "sscloseact", 1, { INTEGER_VAR, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SCLOSE_ACT_SERVER, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "scclose", 1, { INTEGER_VAR, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SCLOSE_CLIENT, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "clientaddr", 2, { INTEGER_VAR, STRING_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SGETCLIENTADDR, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "hostname", 1, { STRING_VAR, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SGETHOSTNAME, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 1 },
    { "hostbyname", 2, { STRING_VAR, STRING_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SGETHOSTBYNAME, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "hostbyaddr", 2, { STRING_VAR, STRING_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SGETHOSTBYADDR, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "sread", 2, { INTEGER_VAR, ALL, EMPTY },
    {{ BYTE_VAR, INT_VAR, LINT_VAR, QINT_VAR, DOUBLE_VAR, STRING_VAR }, { SREAD_BYTE, SREAD_INT, SREAD_LINT, SREAD_QINT, SREAD_DOUBLE, SREAD_STRING }},
    2, 2 },
    { "sreadab", 3, { INTEGER_VAR, BYTE_VAR, INTEGER_VAR },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SREAD_ARRAY_BYTE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "sreadls", 2, { INTEGER_VAR, STRING_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SREAD_LINE_STRING, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },

    { "swrite", 2, { INTEGER_VAR, ALL, EMPTY },
    {{ BYTE_VAR, INT_VAR, LINT_VAR, QINT_VAR, DOUBLE_VAR, STRING_VAR }, { SWRITE_BYTE, SWRITE_INT, SWRITE_LINT, SWRITE_QINT, SWRITE_DOUBLE, SWRITE_STRING }},
    2, EMPTY },
    { "swriteab", 3, { INTEGER_VAR, BYTE_VAR, INTEGER_VAR },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SWRITE_ARRAY_BYTE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "swritesl", 2, { INTEGER_VAR, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SWRITE_STR_LINT, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "swritesd", 2, { INTEGER_VAR, DOUBLE_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SWRITE_STR_DOUBLE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "swriten", 2, { INTEGER_VAR, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SWRITE_NEWLINE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "swritesp", 2, { INTEGER_VAR, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SWRITE_SPACE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },

    { "sizeof", 2, { ALL, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SIZEOF, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "vmvar", 2, { ALL, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { VMVAR, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "val", 2, { STRING_VAR, ALL, EMPTY },
    {{ INT_VAR, LINT_VAR, QINT_VAR, DOUBLE_VAR, BYTE_VAR, EMPTY }, { STR2VAL_L, STR2VAL_L, STR2VAL_L, STR2VAL_D, STR2VAL_L, EMPTY }},
    2, 2 },
    { "str", 2, { ALL, STRING_VAR, EMPTY },
    {{ INT_VAR, LINT_VAR, QINT_VAR, DOUBLE_VAR, BYTE_VAR, EMPTY }, { VAL2STR_L, VAL2STR_L, VAL2STR_L, VAL2STR_D, VAL2STR_L, EMPTY }},
    1, 2 },
    { "2int", 2, { DOUBLE_VAR, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { DOUBLE2INT, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "2double", 2, { INTEGER_VAR, DOUBLE_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { INT2DOUBLE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "srand", 1, { INTEGER_VAR, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SRAND_L, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "rand", 1, { ALL, EMPTY, EMPTY },
    {{ INT_VAR, LINT_VAR, QINT_VAR, DOUBLE_VAR, BYTE_VAR, EMPTY }, { RAND_L, RAND_L, RAND_L, RAND_D, RAND_L, EMPTY }},
    1, 1 },
    { "abs", 2, { ALL, ALL, EMPTY },
    {{ INT_VAR, LINT_VAR, QINT_VAR, DOUBLE_VAR, BYTE_VAR, EMPTY }, { ABS_L, ABS_L, ABS_L, ABS_D, ABS_L, EMPTY }},
    1, 2 },
    { "ceil", 2, { DOUBLE_VAR, DOUBLE_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { CEIL_D, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "floor", 2, { DOUBLE_VAR, DOUBLE_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { FLOOR_D, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "exp", 2, { DOUBLE_VAR, DOUBLE_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { EXP_D, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "log", 2, { DOUBLE_VAR, DOUBLE_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { LOG_D, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "log10", 2, { DOUBLE_VAR, DOUBLE_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { LOG10_D, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "pow", 3, { DOUBLE_VAR, DOUBLE_VAR, DOUBLE_VAR },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { POW_D, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 3 },
    { "sqrt", 2, { DOUBLE_VAR, DOUBLE_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SQRT_D, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "cos", 2, { DOUBLE_VAR, DOUBLE_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { COS_D, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "sin", 2, { DOUBLE_VAR, DOUBLE_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SIN_D, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "tan", 2, { DOUBLE_VAR, DOUBLE_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { TAN_D, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "acos", 2, { DOUBLE_VAR, DOUBLE_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { ACOS_D, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "asin", 2, { DOUBLE_VAR, DOUBLE_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { ASIN_D, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "atan", 2, { DOUBLE_VAR, DOUBLE_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { ATAN_D, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "atan2", 3, { DOUBLE_VAR, DOUBLE_VAR, DOUBLE_VAR },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { ATAN2_D, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 3 },
    { "cosh", 2, { DOUBLE_VAR, DOUBLE_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { COSH_D, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "sinh", 2, { DOUBLE_VAR, DOUBLE_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SINH_D, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "tanh", 2, { DOUBLE_VAR, DOUBLE_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { TANH_D, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "runpr", 2, { STRING_VAR, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { RUNPROCESS, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "runsh", 2, { STRING_VAR, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { RUNSHELL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "waitpr", 2, { INTEGER_VAR, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { WAITPROCESS, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "argnum", 1, { INTEGER_VAR, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { ARGNUM, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 1 },
    { "argstr", 2, { INTEGER_VAR, STRING_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { ARGSTR, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "showstack", 1, { INTEGER_VAR, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { SHOW_STACK, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "stgettype", 1, { INTEGER_VAR, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { STGETTYPE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 1 },
    { "stelements", 1, { INTEGER_VAR, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { STELEMENTS, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 1 },
    { "gettype", 2, { INTEGER_VAR, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { GETTYPE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "pgettype", 2, { INTEGER_VAR, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { PGETTYPE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "dealloc", 1, { ALL, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { DEALLOC, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "getaddress", 2, { ALL, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { GETADDRESS, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "pgetaddress", 2, { ALL, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { PGETADDRESS, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "ppointer", 3, { INTEGER_VAR, ALL, LABEL },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { PPOINTER, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "pointer", 3, { INTEGER_VAR, ALL, LABEL },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { POINTER, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "nop", 0, { EMPTY, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { NOP, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "hash32ab", 3, { BYTE_VAR, INTEGER_VAR, INTEGER_VAR },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { HASH32_AB, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 3 },
    { "phash32ab", 3, { BYTE_VAR, INTEGER_VAR, INTEGER_VAR },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { PHASH32_AB, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 3 },
    { "not", 2, { INTEGER_VAR, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { NOT_L, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "varlock", 1, { ALL, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { VAR_LOCK, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "varunlock", 1, { ALL, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { VAR_UNLOCK, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    
    { "threadcreate", 2, { INTEGER_VAR, LABEL, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { THREAD_CREATE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 1 },
    { "threadjoin", 0, { EMPTY, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { THREAD_JOIN, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "threadstate", 2, { INTEGER_VAR, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { THREAD_STATE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    { "threadnum", 1, { INTEGER_VAR, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { THREAD_NUM, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 1 },
    { "threadsync", 1, { INTEGER_VAR, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { THREAD_SYNCRONIZE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "threadpush", 2, { INTEGER_VAR, ALL, EMPTY },
    {{ INT_VAR, LINT_VAR, QINT_VAR, BYTE_VAR, DOUBLE_VAR, STRING_VAR }, { THREAD_PUSH_L, THREAD_PUSH_L, THREAD_PUSH_L, THREAD_PUSH_L, THREAD_PUSH_D, THREAD_PUSH_S }},
    2, EMPTY },
    { "threadpushsync", 1, { INTEGER_VAR, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { THREAD_PUSH_SYNC, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "threadexitcode", 2, { INTEGER_VAR, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { THREAD_EXITCODE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
    
    { "pdealloc", 1, { ALL, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { DEALLOC_PRIVATE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },

    { "pfreadab", 3, { INTEGER_VAR, BYTE_VAR, INTEGER_VAR },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { PFREAD_AB, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "pfwriteab", 3, { INTEGER_VAR, BYTE_VAR, INTEGER_VAR },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { PFWRITE_AB, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "psreadab", 3, { INTEGER_VAR, BYTE_VAR, INTEGER_VAR },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { PSREAD_AB, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
    { "pswriteab", 3, { INTEGER_VAR, BYTE_VAR, INTEGER_VAR },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { PSWRITE_AB, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },

    { "parray2var", 3, { ARRAY_VAR, INTEGER_VAR, ALL },
    {{ INT_VAR, LINT_VAR, QINT_VAR, DOUBLE_VAR, STRING_VAR, BYTE_VAR }, { PMOVE_A_I, PMOVE_A_L, PMOVE_A_Q, PMOVE_A_D, PMOVE_A_S, PMOVE_A_B }},
    3, 3 },
    { "pvar2array", 3, { ALL, ARRAY_VAR, INTEGER_VAR },
    {{ INT_VAR, LINT_VAR, QINT_VAR, DOUBLE_VAR, STRING_VAR, BYTE_VAR }, { PMOVE_I_A, PMOVE_L_A, PMOVE_Q_A, PMOVE_D_A, PMOVE_S_A, PMOVE_B_A }},
    1, EMPTY },
    
    { "hex", 2, { ALL, STRING_VAR, EMPTY },
    {{ INT_VAR, LINT_VAR, QINT_VAR, EMPTY, EMPTY, EMPTY }, { INT2HEX, INT2HEX, INT2HEX, EMPTY, EMPTY, EMPTY }},
    1, 2 },
    
    { "hash64ab", 3, { BYTE_VAR, INTEGER_VAR, INTEGER_VAR },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { HASH64_AB, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 3 },
    { "phash64ab", 3, { BYTE_VAR, INTEGER_VAR, INTEGER_VAR },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { PHASH64_AB, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 3 },
    
    { "stpush", 2, { ALL, ALL, EMPTY },
    {{ INT_VAR, LINT_VAR, QINT_VAR, BYTE_VAR, DOUBLE_VAR, STRING_VAR }, { STPUSH_L, STPUSH_L, STPUSH_L, STPUSH_L, STPUSH_D, STPUSH_S }},
    2, 2 },

	{ "stpull", 2, { ALL, ALL, EMPTY },
    {{ INT_VAR, LINT_VAR, QINT_VAR, BYTE_VAR, DOUBLE_VAR, STRING_VAR }, { STPULL_L, STPULL_L, STPULL_L, STPULL_L, STPULL_D, STPULL_S }},
    2, 2 },
	
    { "dynarray2var", 3, { ARRAY_VAR, INTEGER_VAR, ALL },
    {{ QINT_VAR, DOUBLE_VAR, STRING_VAR, EMPTY, EMPTY, EMPTY }, { MOVE_ADYN_Q, MOVE_ADYN_D, MOVE_ADYN_S, EMPTY, EMPTY, EMPTY }},
    3, 3 },
    { "var2dynarray", 3, { ALL, ARRAY_VAR, INTEGER_VAR },
    {{ QINT_VAR, DOUBLE_VAR, STRING_VAR, EMPTY, EMPTY, EMPTY }, { MOVE_Q_ADYN, MOVE_D_ADYN, MOVE_S_ADYN, EMPTY, EMPTY, EMPTY }},
    1, EMPTY },
    
    { "pdynarray2var", 3, { ARRAY_VAR, INTEGER_VAR, ALL },
    {{ QINT_VAR, DOUBLE_VAR, STRING_VAR, EMPTY, EMPTY, EMPTY }, { PMOVE_ADYN_Q, PMOVE_ADYN_D, PMOVE_ADYN_S, EMPTY, EMPTY, EMPTY }},
    3, 3 },
    { "pvar2dynarray", 3, { ALL, ARRAY_VAR, INTEGER_VAR },
    {{ QINT_VAR, DOUBLE_VAR, STRING_VAR, EMPTY, EMPTY, EMPTY }, { PMOVE_Q_ADYN, PMOVE_D_ADYN, PMOVE_S_ADYN, EMPTY, EMPTY, EMPTY }},
    1, EMPTY },
    
    { "dyngettype", 3, { ARRAY_VAR, INTEGER_VAR, INTEGER_VAR },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { DYNGETTYPE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 3 },
    { "pdyngettype", 3, { ARRAY_VAR, INTEGER_VAR, INTEGER_VAR },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { PDYNGETTYPE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 3 },
    
    
    { "vadd", 3, { ARRAY_VAR, INTEGER_VAR, ARRAY_VAR },
    {{ QINT_VAR, DOUBLE_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { VADD_L, VADD_D, EMPTY, EMPTY, EMPTY, EMPTY }},
    1, EMPTY },
    { "vsub", 3, { ARRAY_VAR, INTEGER_VAR, ARRAY_VAR },
    {{ QINT_VAR, DOUBLE_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { VSUB_L, VSUB_D, EMPTY, EMPTY, EMPTY, EMPTY }},
    1, EMPTY },
    { "vmul", 3, { ARRAY_VAR, INTEGER_VAR, ARRAY_VAR },
    {{ QINT_VAR, DOUBLE_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { VMUL_L, VMUL_D, EMPTY, EMPTY, EMPTY, EMPTY }},
    1, EMPTY },
    { "vdiv", 3, { ARRAY_VAR, INTEGER_VAR, ARRAY_VAR },
    {{ QINT_VAR, DOUBLE_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { VDIV_L, VDIV_D, EMPTY, EMPTY, EMPTY, EMPTY }},
    1, EMPTY },
    
    { "vadd2", 3, { ARRAY_VAR, ARRAY_VAR, ARRAY_VAR },
    {{ QINT_VAR, DOUBLE_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { VADD2_L, VADD2_D, EMPTY, EMPTY, EMPTY, EMPTY }},
    1, EMPTY },
    { "vsub2", 3, { ARRAY_VAR, ARRAY_VAR, ARRAY_VAR },
    {{ QINT_VAR, DOUBLE_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { VSUB2_L, VSUB2_D, EMPTY, EMPTY, EMPTY, EMPTY }},
    1, EMPTY },
    { "vmul2", 3, { ARRAY_VAR, ARRAY_VAR, ARRAY_VAR },
    {{ QINT_VAR, DOUBLE_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { VMUL2_L, VMUL2_D, EMPTY, EMPTY, EMPTY, EMPTY }},
    1, EMPTY },
    { "vdiv2", 3, { ARRAY_VAR, ARRAY_VAR, ARRAY_VAR },
    {{ QINT_VAR, DOUBLE_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { VDIV2_L, VDIV2_D, EMPTY, EMPTY, EMPTY, EMPTY }},
    1, EMPTY },
    
    
    { "pvadd", 3, { ARRAY_VAR, INTEGER_VAR, ARRAY_VAR },
    {{ QINT_VAR, DOUBLE_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { PVADD_L, PVADD_D, EMPTY, EMPTY, EMPTY, EMPTY }},
    1, EMPTY },
    { "pvsub", 3, { ARRAY_VAR, INTEGER_VAR, ARRAY_VAR },
    {{ QINT_VAR, DOUBLE_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { PVSUB_L, PVSUB_D, EMPTY, EMPTY, EMPTY, EMPTY }},
    1, EMPTY },
    { "pvmul", 3, { ARRAY_VAR, INTEGER_VAR, ARRAY_VAR },
    {{ QINT_VAR, DOUBLE_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { PVMUL_L, PVMUL_D, EMPTY, EMPTY, EMPTY, EMPTY }},
    1, EMPTY },
    { "pvdiv", 3, { ARRAY_VAR, INTEGER_VAR, ARRAY_VAR },
    {{ QINT_VAR, DOUBLE_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { PVDIV_L, PVDIV_D, EMPTY, EMPTY, EMPTY, EMPTY }},
    1, EMPTY },
    
    { "pvadd2", 3, { ARRAY_VAR, ARRAY_VAR, ARRAY_VAR },
    {{ QINT_VAR, DOUBLE_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { PVADD2_L, PVADD2_D, EMPTY, EMPTY, EMPTY, EMPTY }},
    1, EMPTY },
    { "pvsub2", 3, { ARRAY_VAR, ARRAY_VAR, ARRAY_VAR },
    {{ QINT_VAR, DOUBLE_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { PVSUB2_L, PVSUB2_D, EMPTY, EMPTY, EMPTY, EMPTY }},
    1, EMPTY },
    { "pvmul2", 3, { ARRAY_VAR, ARRAY_VAR, ARRAY_VAR },
    {{ QINT_VAR, DOUBLE_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { PVMUL2_L, PVMUL2_D, EMPTY, EMPTY, EMPTY, EMPTY }},
    1, EMPTY },
    { "pvdiv2", 3, { ARRAY_VAR, ARRAY_VAR, ARRAY_VAR },
    {{ QINT_VAR, DOUBLE_VAR, EMPTY, EMPTY, EMPTY, EMPTY }, { PVDIV2_L, PVDIV2_D, EMPTY, EMPTY, EMPTY, EMPTY }},
    1, EMPTY },
    
    { "password", 2, { STRING_VAR, INTEGER_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { PASSWORD, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 1 },
    
    { "makedir", 1, { STRING_VAR, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { MAKEDIR, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
	
	{ "hsvar", 2, { ARRAY_VAR, INTEGER_VAR, EMPTY },
	{{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { HSVAR, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
	{ "hsserver", 1, { INTEGER_VAR, EMPTY, EMPTY },
	{{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { HSSERVER, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
	{ "hsload", 2, { ARRAY_VAR, INTEGER_VAR, EMPTY },
	{{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { HSLOAD, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
	{ "hssave", 2, { ARRAY_VAR, INTEGER_VAR, EMPTY },
	{{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { HSSAVE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
	{ "jit", 2, { LABEL, LABEL, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { JIT, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 1 },
    
    { "dobjects", 2, { STRING_VAR, INTEGER_VAR, EMPTY },
	{{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { DOBJECTS, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
	{ "dnames", 2, { STRING_VAR, ARRAY_VAR, EMPTY },
	{{ STRING_VAR, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { DNAMES, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    2, EMPTY },
	{ "pdnames", 2, { STRING_VAR, ARRAY_VAR, EMPTY },
	{{ STRING_VAR, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { PDNAMES, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    2, EMPTY },
	
	{ "lopen", 2, { INTEGER_VAR, STRING_VAR, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { LOPEN, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 1 },
	{ "lclose", 1, { INTEGER_VAR, EMPTY, EMPTY },
    {{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { LCLOSE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
	{ "lfunc", 3, { INTEGER_VAR, INTEGER_VAR, STRING_VAR },
	{{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { LFUNC, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, 2 },
	{ "lcall", 2, { INTEGER_VAR, INTEGER_VAR, EMPTY },
	{{ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }, { LCALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }},
    DIRECT, EMPTY },
};

S2 translate_code (void)
{
    U1 type[3];
    S4 i, opcode = -1, argtypes = -1;
    S4 var, dims, jump;
    S2 reg[3];

    printf ("compile: translate_code ...\n");

    for (i = 0; i <= TRANSLATE_MAX; i++)
    {
        if (strcmp (src_line.arg[0], translate[i].opcode) == 0)
        {
            opcode = i;
            break;
        }
    }

    printf ("compile: opcode: %li\n", opcode);

    if (opcode == -1)
    {
        /* printf ("compile: translate_code: error unknown opcode: %s, line: %li\n", src_line.arg[0], plist_ind); */
        return (-1);
    }

    /* opcode found: do translation */

    /* check arguments */
    if (src_line.args != translate[opcode].argnum)
    {
        printf ("compile: translate_code: error too few or to many arguments line: %li\n", plist_ind);
        printf ("[ %s ]\n", plist[plist_ind].memptr);
        return (FALSE);
    }

    /* check for var_lock OR var_unlock */

    if (translate[opcode].trans[1][0] == VAR_LOCK || translate[opcode].trans[1][0] == VAR_UNLOCK)
    {

        #if DEBUG
            printf ("VAR_LOCK OR VAR_UNLOCK\n");
        #endif        

        var = getvarind_comp (src_line.arg[1]);

        if (! set1 (translate[opcode].trans[1][0], var))
        {
            return (MEMORY);
        }  

        return (TRUE);
    }

    for (i = 0; i < translate[opcode].argnum; i++)
    {
        /* check argument types */

        if (translate[opcode].args[i] == LABEL)
        {
            /* found label -> set LABEL as type */

            type[i] = LABEL;

            printf ("compile: translate_code: LABEL found!\n");
        }
        else
        {
            var = getvarind_comp (src_line.arg[i + 1]);
            if (var == NOTDEF)
            {
                printf ("compile: translate_code: error: variable not defined: %s, line: %li\n", src_line.arg[i + 1], plist_ind);
                return (FALSE);
            }

            if (src_line.arg[i + 1][0] == COMP_PRIVATE_VAR_SB)
            {
                type[i] = pvarlist_obj[var].type;
            }
            else
            {
                type[i] = varlist[var].type;
            }
        }

        
        if (type[i] != LABEL)
		{
        if (src_line.arg[i + 1][0] == COMP_PRIVATE_VAR_SB)
        {
            dims = pvarlist_obj[var].dims;
        }
        else
        {
            dims = varlist[var].dims;
        }

        if (translate[opcode].args[i] == ARRAY_VAR)
        {
            if (src_line.arg[i + 1][0] == COMP_PRIVATE_VAR_SB)
            {
                if (pvarlist_obj[var].dims == NODIMS)
                {
                    printf ("compile: translate_code: error: not an array %s, line: %li\n", src_line.arg[i + 1], plist_ind);
                    return (FALSE);
                }
            }
            else
            {
                if (varlist[var].dims == NODIMS)
                {
                    printf ("compile: translate_code: error: not an array %s, line: %li\n", src_line.arg[i + 1], plist_ind);
                    return (FALSE);
                }
            }
        }
		}
        printf ("compile: var: %s = %li\n", src_line.arg[i + 1], var);
        printf ("compile: type %li\n", type[i]);

        switch (type[i])
        {
            case INT_VAR:
                switch (translate[opcode].args[i])
                {
                    case INT_VAR:
                    case ARRAY_VAR:
                    case ALL:
                    case INTEGER_VAR:
                        break;

                    default:
                        printf ("compile: [INT] translate_code: error: wrong argument type %s, line: %li\n", src_line.arg[i + 1], plist_ind);
                        return (FALSE);
                        break;
                }

                reg[i] = get_vmreg_var_l (var);
                if (reg[i] == EMPTY && dims == NODIMS)
                {
                    /* variable not in registers: push variable into register */

                    reg[i] = get_vmreg_l ();
                    if (reg[i] != FULL)
                    {
                        if (i != translate[opcode].target - 1)
                        {
                            if (src_line.arg[i + 1][0] == COMP_PRIVATE_VAR_SB)
                            {
                                if (! set2 (PPUSH_I, var, reg[i]))
                                {
                                    return (MEMORY);
                                }
                            }
                            else
                            {
                                if (! set2 (PUSH_I, var, reg[i]))
                                {
                                    return (MEMORY);
                                }
                            }
                        }
                        if (src_line.arg[i + 1][0] == COMP_PRIVATE_VAR_SB)
                        {
                            set_vmreg_l (reg[i], var, PRIVATE);
                        }
                        else
                        {
                             set_vmreg_l (reg[i], var, NORMAL);
                        }
                    }
                }
                if (reg[i] == EMPTY && dims != NODIMS)
                {
                    reg[i] = var;
                }
                break;

            case LINT_VAR:
                switch (translate[opcode].args[i])
                {
                    case LINT_VAR:
                    case ARRAY_VAR:
                    case ALL:
                    case INTEGER_VAR:
                        break;

                    default:
                        printf ("compile: [LINT] translate_code: error: wrong argument type %s, line: %li\n", src_line.arg[i + 1], plist_ind);
                        return (FALSE);
                        break;
                }

                reg[i] = get_vmreg_var_l (var);
                if (reg[i] == EMPTY && dims == NODIMS)
                {
                    /* variable not in registers: push variable into register */

                    reg[i] = get_vmreg_l ();
                    if (reg[i] != FULL)
                    {
                        if (i != translate[opcode].target - 1)
                        {
                            if (src_line.arg[i + 1][0] == COMP_PRIVATE_VAR_SB)
                            {
                                if (! set2 (PPUSH_L, var, reg[i]))
                                {
                                    return (MEMORY);
                                }
                            }
                            else
                            {
                                if (! set2 (PUSH_L, var, reg[i]))
                                {
                                    return (MEMORY);
                                }
                            }
                        }
                        if (src_line.arg[i + 1][0] == COMP_PRIVATE_VAR_SB)
                        {
                            set_vmreg_l (reg[i], var, PRIVATE);
                        }
                        else
                        {
                             set_vmreg_l (reg[i], var, NORMAL);
                        }
                    }
                }
                if (reg[i] == EMPTY && dims != NODIMS)
                {
                    reg[i] = var;
                }
                break;

            case QINT_VAR:
                switch (translate[opcode].args[i])
                {
                    case QINT_VAR:
                    case ARRAY_VAR:
                    case ALL:
                    case INTEGER_VAR:
                        break;

                    default:
                        printf ("compile: [QINT] translate_code: error: wrong argument type %s, line: %li\n", src_line.arg[i + 1], plist_ind);
                        return (FALSE);
                        break;
                }

                reg[i] = get_vmreg_var_l (var);
                if (reg[i] == EMPTY && dims == NODIMS)
                {
                    /* variable not in registers: push variable into register */

                    reg[i] = get_vmreg_l ();
                    if (reg[i] != FULL)
                    {
                        if (i != translate[opcode].target - 1)
                        {
                            if (src_line.arg[i + 1][0] == COMP_PRIVATE_VAR_SB)
                            {
                                if (! set2 (PPUSH_Q, var, reg[i]))
                                {
                                    return (MEMORY);
                                }
                            }
                            else
                            {
                                if (! set2 (PUSH_Q, var, reg[i]))
                                {
                                    return (MEMORY);
                                }
                            }
                        }
                        if (src_line.arg[i + 1][0] == COMP_PRIVATE_VAR_SB)
                        {
                            set_vmreg_l (reg[i], var, PRIVATE);
                        }
                        else
                        {
                             set_vmreg_l (reg[i], var, NORMAL);
                        }
                    }
                }
                if (reg[i] == EMPTY && dims != NODIMS)
                {
                    reg[i] = var;
                }
                break;

            case DOUBLE_VAR:
                switch (translate[opcode].args[i])
                {
                    case DOUBLE_VAR:
                    case ARRAY_VAR:
                    case ALL:
                        break;

                    default:
                        printf ("compile: [DOUBLE] translate_code: error: wrong argument type %s, line: %li\n", src_line.arg[i + 1], plist_ind);
                        return (FALSE);
                        break;
                }

                reg[i] = get_vmreg_var_d (var);
                if (reg[i] == EMPTY && dims == NODIMS)
                {
                    /* variable not in registers: push variable into register */

                    reg[i] = get_vmreg_d ();
                    if (reg[i] != FULL)
                    {
                        if (i != translate[opcode].target - 1)
                        {
                            if (src_line.arg[i + 1][0] == COMP_PRIVATE_VAR_SB)
                            {
                                if (! set2 (PPUSH_D, var, reg[i]))
                                {
                                    return (MEMORY);
                                }
                            }
                            else
                            {
                                if (! set2 (PUSH_D, var, reg[i]))
                                {
                                    return (MEMORY);
                                }
                            }
                        }
                        if (src_line.arg[i + 1][0] == COMP_PRIVATE_VAR_SB)
                        {
                            set_vmreg_d (reg[i], var, PRIVATE);
                        }
                        else
                        {
                             set_vmreg_d (reg[i], var, NORMAL);
                        }
                    }
                }
                if (reg[i] == EMPTY && dims != NODIMS)
                {
                    reg[i] = var;
                }
                break;

            case STRING_VAR:
                switch (translate[opcode].args[i])
                {
                    case STRING_VAR:
                    case ARRAY_VAR:
                    case ALL:
                        break;

                    default:
                        printf ("compile: [STRING] translate_code: error: wrong argument type %s, line: %li\n", src_line.arg[i + 1], plist_ind);
                        return (FALSE);
                        break;
                }

                if (translate[opcode].args[i] != ARRAY_VAR)
                {
                    reg[i] = get_vmreg_var_s (var);
                    if (reg[i] == EMPTY)
                    {
                        /* variable not in registers: push variable into register */

                        reg[i] = get_vmreg_s ();
                        if (reg[i] != FULL)
                        {
                            if (i != translate[opcode].target - 1)
                            {
                                if (src_line.arg[i + 1][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPUSH_S, var, reg[i]))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PUSH_S, var, reg[i]))
                                    {
                                        return (MEMORY);
                                    }
                                }
                            }
                            if (src_line.arg[i + 1][0] == COMP_PRIVATE_VAR_SB)
                            {
                                set_vmreg_s (reg[i], var, PRIVATE);
                            }
                            else
                            {
                                set_vmreg_s (reg[i], var, NORMAL);
                            }
                        }
                    }
                }
                else
                {
                    reg[i] = var;
                }
                break;

            case BYTE_VAR:
                switch (translate[opcode].args[i])
                {
                    case BYTE_VAR:
                    case ARRAY_VAR:
                    case ALL:
                    case INTEGER_VAR:
                        break;

                    default:
                        printf ("compile: [BYTE] translate_code: error: wrong argument type %s, line: %li\n", src_line.arg[i + 1], plist_ind);
                        return (FALSE);
                        break;
                }

                reg[i] = get_vmreg_var_l (var); 
                if (reg[i] == EMPTY && dims == NODIMS)
                {
                    /* variable not in registers: push variable into register */

                    reg[i] = get_vmreg_l ();
                    if (reg[i] != FULL)
                    {
                        if (i != translate[opcode].target - 1)
                        {
                            if (src_line.arg[i + 1][0] == COMP_PRIVATE_VAR_SB)
                            {
                                if (! set2 (PPUSH_B, var, reg[i]))
                                {
                                    return (MEMORY);
                                }
                            }
                            else
                            {
                                if (! set2 (PUSH_B, var, reg[i]))
                                {
                                    return (MEMORY);
                                }
                            }
                        }
                        if (src_line.arg[i + 1][0] == COMP_PRIVATE_VAR_SB)
                        {
                            set_vmreg_l (reg[i], var, PRIVATE);
                        }
                        else
                        {
                            set_vmreg_l (reg[i], var, NORMAL);
                        }
                    }
                }
                if (reg[i] == EMPTY && dims != NODIMS)
                {
                    reg[i] = var;
                }
                break;

            case DYNAMIC_VAR:
                switch (translate[opcode].args[i])
                {
                    case DYNAMIC_VAR:
                    case ARRAY_VAR:
                    case ALL:
                        break;

                    default:
                        printf ("compile: [DYNAMIC] translate_code: error: wrong argument type %s, line: %li\n", src_line.arg[i + 1], plist_ind);
                        return (FALSE);
                        break;
                }

                reg[i] = var;
                break;

            case LABEL:
                reg[i] = getjumpind (src_line.arg[i + 1]);
                if (reg[i] == NOTDEF)
                {
                    if (jumplist_gonext () == FALSE)
                    {
                        return (FALSE);
                    }

                    strcpy (jumplist[jumplist_ind].lab, src_line.arg[i + 1]);
                    jumplist[jumplist_ind].pos = NOTDEF;
                    jumplist[jumplist_ind].islabel = FALSE;
                    reg[i] = jumplist_ind;
                }
                
                printf ("compile: [LABEL] found: %s\n", src_line.arg[i + 1]);
                break;
        }
    }

    if (translate[opcode].transarg == DIRECT)
    {
        /* only one emit opcode */

        printf ("compile: translate_code: transarg = DIRECT\n");

        switch (translate[opcode].argnum)
        {
            case 0:
                if (! set0 (translate[opcode].trans[1][0]))
                {
                    return (MEMORY);
                }
                break;

            case 1:
                if (! set1 (translate[opcode].trans[1][0], reg[0]))
                {
                    return (MEMORY);
                }
                break;

            case 2:
                if (! set2 (translate[opcode].trans[1][0], reg[0], reg[1]))
                {
                    return (MEMORY);
                }
                break;

            case 3:
                if (! set3 (translate[opcode].trans[1][0], reg[0], reg[1], reg[2]))
                {
                    return (MEMORY);
                }
                break;
        }
    }
    else
    {
        /* multiple emit opcodes */

        for (i = 0; i <= 5; i++)
        {
            if (translate[opcode].trans[0][i] == type[translate[opcode].transarg - 1])
            {
                break;
            }
        }
        if (translate[opcode].trans[0][i] == EMPTY)
        {
            printf ("compile: translate_code: error: argument type don't match, line: %li\n", plist_ind);
            return (FALSE);
        }

        switch (translate[opcode].argnum)
        {
            case 0:
                if (! set0 (translate[opcode].trans[1][i]))
                {
                    return (MEMORY);
                }
                break;

            case 1:
                if (! set1 (translate[opcode].trans[1][i], reg[0]))
                {
                    return (MEMORY);
                }
                break;

            case 2:
                if (! set2 (translate[opcode].trans[1][i], reg[0], reg[1]))
                {
                    return (MEMORY);
                }
                break;

            case 3:
                if (! set3 (translate[opcode].trans[1][i], reg[0], reg[1], reg[2]))
                {
                    return (MEMORY);
                }
                break;
        }
    }

    if (translate[opcode].target != EMPTY)
    {
        #if DEBUG
            printf ("translate.c: target: %s\n", src_line.arg[translate[opcode].target]);
        #endif

        /* set pull opocde */
        switch (type[translate[opcode].target - 1])
        {
            case INT_VAR:
                var = get_vmreg_val_l (reg[translate[opcode].target - 1]);

                if (src_line.arg[translate[opcode].target][0] == COMP_PRIVATE_VAR_SB)
                {
                    if (! set2 (PPULL_I, reg[translate[opcode].target - 1], var))
                    {
                        return (MEMORY);
                    }
                    set_vmreg_l (reg[translate[opcode].target - 1], var, PRIVATE);
                }
                else
                {
                    if (! set2 (PULL_I, reg[translate[opcode].target - 1], var))
                    {
                        return (MEMORY);
                    }
                    set_vmreg_l (reg[translate[opcode].target - 1], var, NORMAL);
                }
                break;

            case LINT_VAR:
                var = get_vmreg_val_l (reg[translate[opcode].target - 1]);

                if (src_line.arg[translate[opcode].target][0] == COMP_PRIVATE_VAR_SB)
                {
                    if (! set2 (PPULL_L, reg[translate[opcode].target - 1], var))
                    {
                        return (MEMORY);
                    }
                    set_vmreg_l (reg[translate[opcode].target - 1], var, PRIVATE);
                }
                else
                {
                    if (! set2 (PULL_L, reg[translate[opcode].target - 1], var))
                    {
                        return (MEMORY);
                    }    
                    set_vmreg_l (reg[translate[opcode].target - 1], var, NORMAL);
                    
                }
                break;

            case QINT_VAR:
                var = get_vmreg_val_l (reg[translate[opcode].target - 1]);

                if (src_line.arg[translate[opcode].target][0] == COMP_PRIVATE_VAR_SB)
                {
                    if (! set2 (PPULL_Q, reg[translate[opcode].target - 1], var))
                    {
                        return (MEMORY);
                    }
                    set_vmreg_l (reg[translate[opcode].target - 1], var, PRIVATE);
                }
                else
                {
                    if (! set2 (PULL_Q, reg[translate[opcode].target - 1], var))
                    {
                        return (MEMORY);
                    }
                    
                    printf ("translate: PULL_Q: %li\n", var);
                    
                    set_vmreg_l (reg[translate[opcode].target - 1], var, NORMAL);
                }
                break;

            case DOUBLE_VAR:
                var = get_vmreg_val_d (reg[translate[opcode].target - 1]);

                if (src_line.arg[translate[opcode].target][0] == COMP_PRIVATE_VAR_SB)
                {
                    if (! set2 (PPULL_D, reg[translate[opcode].target - 1], var))
                    {
                        return (MEMORY);
                    }
                    set_vmreg_d (reg[translate[opcode].target - 1], var, PRIVATE);
                }
                else
                {
                    if (! set2 (PULL_D, reg[translate[opcode].target - 1], var))
                    {
                        return (MEMORY);
                    }
                    set_vmreg_d (reg[translate[opcode].target - 1], var, NORMAL);
                }
                break;

            case STRING_VAR:
                var = get_vmreg_val_s (reg[translate[opcode].target - 1]);

                if (src_line.arg[translate[opcode].target][0] == COMP_PRIVATE_VAR_SB)
                {
                    if (! set2 (PPULL_S, reg[translate[opcode].target - 1], var))
                    {
                        return (MEMORY);
                    }
                    set_vmreg_s (reg[translate[opcode].target - 1], var, PRIVATE);
                }
                else
                {
                    if (! set2 (PULL_S, reg[translate[opcode].target - 1], var))
                    {
                        return (MEMORY);
                    }
                    set_vmreg_s (reg[translate[opcode].target - 1], var, NORMAL);
                }
                break;

            case BYTE_VAR:
                var = get_vmreg_val_l (reg[translate[opcode].target - 1]);

                if (src_line.arg[translate[opcode].target][0] == COMP_PRIVATE_VAR_SB)
                {
                    if (! set2 (PPULL_B, reg[translate[opcode].target - 1], var))
                    {
                        return (MEMORY);
                    }
                    set_vmreg_l (reg[translate[opcode].target - 1], var, PRIVATE);
                }
                else
                {
                    if (! set2 (PULL_B, reg[translate[opcode].target - 1], var))
                    {
                        return (MEMORY);
                    }
                    set_vmreg_l (reg[translate[opcode].target - 1], var, NORMAL);
                }
                break;
        }
    }

    return (TRUE);
}
