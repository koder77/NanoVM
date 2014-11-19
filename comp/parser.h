/****************************************************************************
*
* Filename: parser.h
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  global definitions for n compiler parser
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

#define NORMAL              0
#define PRIVATE             1

#define MAXFUNCTIONS        1024
#define MAXIF               1024
#define MAXFOR              1024

#define IF_FINISHED         2


#define FULL                -1
#define MAXVMREG_L          246

/* tokens: keywords */

#define MAX_TOKENS                      22


#define COMP_FUNC_SB                    "func"
#define COMP_FUNC                       0

#define COMP_FUNC_END_SB                "funcend"
#define COMP_FUNC_END                   1

#define COMP_FUNC_CALL                  2

#define COMP_BYTE_SB                    "byte"
#define COMP_BYTE                       3

#define COMP_INT_SB                     "int"
#define COMP_INT                        4

#define COMP_LONG_INT_SB                "lint"
#define COMP_LONG_INT                   5

#define COMP_QINT_SB                    "qint"
#define COMP_QINT                       6

#define COMP_DOUBLE_SB                  "double"
#define COMP_DOUBLE                     7

#define COMP_STRING_SB                  "string"
#define COMP_STRING                     8

#define COMP_DYNAMIC_SB                 "dynamic"
#define COMP_DYNAMIC                    9

#define COMP_PRINT_SB                   "print"
#define COMP_PRINT                      10

#define COMP_PRINT_NEWLINE_SB           "printn"
#define COMP_PRINT_NEWLINE              11

#define COMP_EXIT_SB                    "exit"
#define COMP_EXIT                       12

#define COMP_GET_SB                     "get"
#define COMP_GET                        13

#define COMP_TOKEN                      14

#define COMP_IF_SB                      "if"
#define COMP_IF                         15

#define COMP_ELSE_SB                    "else"
#define COMP_ELSE                       16

#define COMP_ENDIF_SB                   "endif"
#define COMP_ENDIF                      17

#define COMP_FOR_SB                     "for"
#define COMP_FOR                        18

#define COMP_NEXT_SB                    "next"
#define COMP_NEXT                       19

#define COMP_LAB_SB                     "lab"
#define COMP_LAB                        20

#define COMP_GOTO_SB                    "goto"
#define COMP_GOTO                       21

#define COMP_GET_MULTI_SB               "getmulti"
#define COMP_GET_MULTI                  22

#define COMP_RETURN_MULTI_SB            "returnmulti"
#define COMP_RETURN_MULTI               23

#define COMP_GET_MULTI_END_SB           "getmultiend"
#define COMP_GET_MULTI_END              24

/* inline assembler definition */
#define COMP_ASSEMB_SB                  "#ASSEMB"
#define COMP_ASSEMB                     25

#define COMP_ASSEMB_END_SB              "#ASSEMB_END"
#define COMP_ASSEMB_END                 26


#define COMP_OP_ADD_SB                  "+"
#define COMP_OP_ADD                     27

#define COMP_OP_SUB_SB                  "-"
#define COMP_OP_SUB                     28

#define COMP_OP_MUL_SB                  "*"
#define COMP_OP_MUL                     29

#define COMP_OP_DIV_SB                  "/"
#define COMP_OP_DIV                     30

#define COMP_OP_INC_SB                  "++"
#define COMP_OP_INC                     31

#define COMP_OP_DEC_SB                  "--"
#define COMP_OP_DEC                     32

#define COMP_OP_SMUL_SB                 "<<"
#define COMP_OP_SMUL                    33

#define COMP_OP_SDIV_SB                 ">>"
#define COMP_OP_SDIV                    34

#define COMP_OP_AND_SB                  "&&"
#define COMP_OP_AND                     35

#define COMP_OP_OR_SB                   "||"
#define COMP_OP_OR                      36

#define COMP_OP_BAND_SB                 "&"
#define COMP_OP_BAND                    37

#define COMP_OP_BOR_SB                  "|"
#define COMP_OP_BOR                     38

#define COMP_OP_BXOR_SB                 "^"
#define COMP_OP_BXOR                    39

#define COMP_OP_MOD_SB                  "%"
#define COMP_OP_MOD                     40

#define COMP_OP_GREATER_SB              ">"
#define COMP_OP_GREATER                 41

#define COMP_OP_LESS_SB                 "<"
#define COMP_OP_LESS                    42

#define COMP_OP_GREATER_OR_EQ_SB        ">="
#define COMP_OP_GREATER_OR_EQ           43

#define COMP_OP_LESS_OR_EQ_SB           "<="
#define COMP_OP_LESS_OR_EQ              44

#define NBRACKET_OPEN_SB                "("
#define NBRACKET_OPEN                   45

#define NBRACKET_CLOSE_SB               ")"
#define NBRACKET_CLOSE                  46

#define EQUAL_SB                        "="
#define EQUAL                           47

#define NOT_EQUAL_SB                    "!="
#define NOT_EQUAL                       48

#define EQUAL_LOG_SB                    "=="
#define EQUAL_LOG                       49

#define NOT_SB                          "!"
#define NOT                             50


#define ASSEMB_INLINE                   MAXOPCODE + 1

#define SPACE_SB                        ' '

#define REGISTER_L_SB                   "L"
#define REGISTER_D_SB                   "D"
#define REGISTER_S_SB                   "S"

#define COMP_MAIN_SB                    "main"
#define COMP_SYNC_SB                    "#sync"

#define COMP_RETURN_SB                  "return"


#define COMP_NESTED_CODE_OFF_SB         "#NESTED_CODE_OFF"
#define COMP_NESTED_CODE_OFF            MAXOPCODE + 2

#define COMP_NESTED_CODE_ON_SB          "#NESTED_CODE_ON"
#define COMP_NESTED_CODE_ON             MAXOPCODE + 3

#define COMP_NESTED_CODE_GLOBAL_OFF_SB  "#NESTED_CODE_GLOBAL_OFF"
#define COMP_NESTED_CODE_GLOBAL_OFF     MAXOPCODE + 4

#define COMP_PULL_SB					"pull"
#define COMP_PULL 						MAXOPCODE + 5

#define PGETADDRESS                     MAXOPCODE + 6

#define COMP_ATOMIC_START_SB			"#ATOMIC"
#define COMP_ATOMIC_START 				MAXOPCODE + 7

#define COMP_ATOMIC_END_SB				"#ATOMIC_END"
#define COMP_ATOMIC_END 				MAXOPCODE + 8


#define COMP_PRIVATE_VAR_SB             'P'
#define AROPEN_SB                       '['
#define ARCLOSE_SB                      ']'

#define AT_SB                           "@"
#define NOTREVERSE_SB	 				'>'

/* for translate.c (auto translater) */
#define ARRAY_VAR                       19
#define ALL                             20
#define INTEGER_VAR                     21
#define DIRECT                          -1
#define TRANSLATE_MAX                   161     /* opcodes */

struct comp_opcode
{
    U1 op[16];
};

struct function
{
    U1 name[MAXLINELEN + 1];
    S2 start;                   /* linenumbers */
    S2 end;
};

struct vmreg_comp
{
    S2 l[MAXVMREG];                         /* the vm-registers */
    S2 d[MAXVMREG];
    S2 s[MAXVMREG];
    
    U1 lp[MAXVMREG];                        /* set to true, if private variable */
    U1 dp[MAXVMREG];
    U1 sp[MAXVMREG];
};

struct vmreg_comp_info
{
    S2 l[MAXVMREG];                         /* the vm-registers spilling info */
    S2 d[MAXVMREG];
    S2 s[MAXVMREG];
};   

struct if_comp
{
    U1 used;
    S4 else_pos;
    S4 endif_pos;
    U1 else_name[MAXJUMPNAME + 1];
    U1 endif_name[MAXJUMPNAME + 1];
};

struct for_comp
{
    U1 used;
    S4 for_pos;
    U1 next;
    U1 for_name[MAXJUMPNAME + 1];
};

struct translate
{
    U1 opcode[MAXLINELEN + 1];
    U1 argnum;
    S2 args[3];
    S2 trans[2][6];
    S2 transarg;
    S2 target;
};

/* message definitions */

#define COMP_START_TXT      "nanoc 64 bit 1.1.0  (c) 2014 by Stefan Pietzonke jay-t@gmx.net\n-== free software: GPL/MPL ==-\n"
#define VERSION_NANOC_TXT   "$VER: nanoc 1.1.0 (08.11.14)"


#define STATUS_OK_TXT       "ok"
#define LOAD_PROG_TXT       "loading program:"
#define SAVE_PROG_TXT       "saving program:"


#include "protos.h"