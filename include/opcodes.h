/****************************************************************************
 *
 * Filename: opcodes.h
 *
 * Author:   Stefan Pietzonke
 * Project:  nano, virtual machine
 *
 * Purpose:  opcode list
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

/* This is the opcode list, which is MAXOPCODE (0-x) long, see global_d.h for this
 *
 * The first argument is the opcode name, followed by the number of arguments (0 - 3).
 * The argument types are:
 * L_REG = integer register
 * D_REG = double register
 * S_REG = string register
 *
 * variables:
 * BYTE_VAR = byte variable
 * INT_VAR = short integer variable
 * LINT_VAR = long integer variable
 * QINT_VAR = long long integer = 64 bit
 * DOUBLE_VAR = double variable
 * STRING_VAR = string variable
 *
 * variable arrays:
 * BYTE_A_VAR = byte array variable
 * INT_A_VAR = short integer array variable
 * LINT_A_VAR = long integer array variable
 * QINT_A_VAR = long long integer array variable
 * DOUBLE_A_VAR = double array variable
 * STRING_A_VAR = string array variable
 * DYNAMIC_A_VAR = dynamic array variable
 * not needed arguments must be declared as EMPTY
 *
 * this structure is used by assemb/opcode.c as a part of the assembler source parser
 * of the *.na files.
 *
 */


struct opcode opcode[] =
{
    { "push_i",             2, { INT_VAR,       L_REG,          EMPTY } },
    { "push_l",             2, { LINT_VAR,      L_REG,          EMPTY } },
    { "push_q",             2, { QINT_VAR,      L_REG,          EMPTY } },
    { "push_d",             2, { DOUBLE_VAR,    D_REG,          EMPTY } },
    { "push_b",             2, { BYTE_VAR,      L_REG,          EMPTY } },
    { "pull_i",             2, { L_REG,         INT_VAR,        EMPTY } },
    { "pull_l",             2, { L_REG,         LINT_VAR,       EMPTY } },
    { "pull_q",             2, { L_REG,         QINT_VAR,       EMPTY } },
    { "pull_d",             2, { D_REG,         DOUBLE_VAR,     EMPTY } },
    { "pull_b",             2, { L_REG,         BYTE_VAR,       EMPTY } },
    { "move_l",             2, { L_REG,         L_REG,          EMPTY } },
    { "move_d",             2, { D_REG,         D_REG,          EMPTY } },
    { "inc_l",              1, { L_REG,         EMPTY,          EMPTY } },
    { "inc_d",              1, { D_REG,         EMPTY,          EMPTY } },
    { "dec_l",              1, { L_REG,         EMPTY,          EMPTY } },
    { "dec_d",              1, { D_REG,         EMPTY,          EMPTY } },
    { "add_l",              3, { L_REG,         L_REG,          L_REG } },
    { "add_d",              3, { D_REG,         D_REG,          D_REG } },
    { "sub_l",              3, { L_REG,         L_REG,          L_REG } },
    { "sub_d",              3, { D_REG,         D_REG,          D_REG } },
    { "mul_l",              3, { L_REG,         L_REG,          L_REG } },
    { "mul_d",              3, { D_REG,         D_REG,          D_REG } },
    { "div_l",              3, { L_REG,         L_REG,          L_REG } },
    { "div_d",              3, { D_REG,         D_REG,          D_REG } },
    { "smul_l",             3, { L_REG,         L_REG,          L_REG } },
    { "sdiv_l",             3, { L_REG,         L_REG,          L_REG } },
    { "and_l",              3, { L_REG,         L_REG,          L_REG } },
    { "or_l",               3, { L_REG,         L_REG,          L_REG } },
    { "band_l",             3, { L_REG,         L_REG,          L_REG } },
    { "bor_l",              3, { L_REG,         L_REG,          L_REG } },
    { "bxor_l",             3, { L_REG,         L_REG,          L_REG } },
    { "mod_l",              3, { L_REG,         L_REG,          L_REG } },
    { "eq_l",               3, { L_REG,         L_REG,          L_REG } },
    { "eq_d",               3, { D_REG,         D_REG,          L_REG } },
    { "neq_l",              3, { L_REG,         L_REG,          L_REG } },
    { "neq_d",              3, { D_REG,         D_REG,          L_REG } },
    { "gr_l",               3, { L_REG,         L_REG,          L_REG } },
    { "gr_d",               3, { D_REG,         D_REG,          L_REG } },
    { "ls_l",               3, { L_REG,         L_REG,          L_REG } },
    { "ls_d",               3, { D_REG,         D_REG,          L_REG } },
    { "greq_l",             3, { L_REG,         L_REG,          L_REG } },
    { "greq_d",             3, { D_REG,         D_REG,          L_REG } },
    { "lseq_l",             3, { L_REG,         L_REG,          L_REG } },
    { "lseq_d",             3, { D_REG,         D_REG,          L_REG } },
    { "move_a_i",           3, { INT_A_VAR,     L_REG,          L_REG } },
    { "move_a_l",           3, { LINT_A_VAR,    L_REG,          L_REG } },
    { "move_a_q",           3, { QINT_A_VAR,    L_REG,          L_REG } },
    { "move_a_d",           3, { DOUBLE_A_VAR,  L_REG,          D_REG } },
    { "move_a_b",           3, { BYTE_A_VAR,    L_REG,          L_REG } },
    { "move_a_s",           3, { STRING_VAR,    L_REG,          S_REG } },
    { "move_i_a",           3, { L_REG,         INT_A_VAR,      L_REG } },
    { "move_l_a",           3, { L_REG,         LINT_A_VAR,     L_REG } },
    { "move_q_a",           3, { L_REG,         QINT_A_VAR,     L_REG } },
    { "move_d_a",           3, { D_REG,         DOUBLE_A_VAR,   L_REG } },
    { "move_b_a",           3, { L_REG,         BYTE_A_VAR,     L_REG } },
    { "move_s_a",           3, { S_REG,         STRING_VAR,     L_REG } },
    { "print_l",            1, { L_REG,         EMPTY,          EMPTY } },
    { "print_d",            1, { D_REG,         EMPTY,          EMPTY } },
    { "print_s",            1, { S_REG,         EMPTY,          EMPTY } },
    { "print_n",            1, { L_REG,         EMPTY,          EMPTY } },
    { "print_sp",           1, { L_REG,         EMPTY,          EMPTY } },
    { "print_c",            0, { EMPTY,         EMPTY,          EMPTY } },
    { "print_a",            1, { L_REG,         EMPTY,          EMPTY } },
    { "print_b",            0, { EMPTY,         EMPTY,          EMPTY } },
    { "print_i",            0, { EMPTY,         EMPTY,          EMPTY } },
    { "print_u",            0, { EMPTY,         EMPTY,          EMPTY } },
    { "print_r",            0, { EMPTY,         EMPTY,          EMPTY } },
    { "print_v",            1, { VAR,           EMPTY,          EMPTY } },
    { "locate",             2, { L_REG,         L_REG,          EMPTY } },
    { "input_l",            1, { L_REG,         EMPTY,          EMPTY } },
    { "input_d",            1, { D_REG,         EMPTY,          EMPTY } },
    { "input_s",            1, { S_REG,         EMPTY,          EMPTY } },
    { "inputch_l",          1, { L_REG,         EMPTY,          EMPTY } },
    { "inputch_d",          1, { D_REG,         EMPTY,          EMPTY } },
    { "inputch_s",          1, { S_REG,         EMPTY,          EMPTY } },
    { "move_s",             2, { S_REG,         S_REG,          EMPTY } },
    { "move_p2s",           3, { S_REG,         L_REG,          S_REG } },
    { "move_s2p",           3, { S_REG,         S_REG,          L_REG } },
    { "add_s",              3, { S_REG,         S_REG,          S_REG } },
    { "strlen",             2, { S_REG,         L_REG,          EMPTY } },
    { "strleft",            3, { S_REG,         L_REG,          S_REG } },
    { "strright",           3, { S_REG,         L_REG,          S_REG } },
    { "ucase",              1, { S_REG,         EMPTY,          EMPTY } },
    { "lcase",              1, { S_REG,         EMPTY,          EMPTY } },
    { "char",               2, { L_REG,         S_REG,          EMPTY } },
    { "asc",                2, { S_REG,         L_REG,          EMPTY } },
    { "eq_s",               3, { S_REG,         S_REG,          L_REG } },
    { "neq_s",              3, { S_REG,         S_REG,          L_REG } },
    { "jmp_l",              2, { L_REG,         LABEL,          EMPTY } },
    { "jsr_l",              2, { L_REG,         LABEL,          EMPTY } },
    { "jmp",                1, { LABEL,         EMPTY,          EMPTY } },
    { "jsr",                1, { LABEL,         EMPTY,          EMPTY } },
    { "rts",                0, { EMPTY,         EMPTY,          EMPTY } },
    { "eq_jmp_l",           3, { L_REG,         L_REG,          LABEL } },
    { "neq_jmp_l",          3, { L_REG,         L_REG,          LABEL } },
    { "gr_jmp_l",           3, { L_REG,         L_REG,          LABEL } },
    { "ls_jmp_l",           3, { L_REG,         L_REG,          LABEL } },
    { "greq_jmp_l",         3, { L_REG,         L_REG,          LABEL } },
    { "lseq_jmp_l",         3, { L_REG,         L_REG,          LABEL } },
    { "eq_jsr_l",           3, { L_REG,         L_REG,          LABEL } },
    { "neq_jsr_l",          3, { L_REG,         L_REG,          LABEL } },
    { "gr_jsr_l",           3, { L_REG,         L_REG,          LABEL } },
    { "ls_jsr_l",           3, { L_REG,         L_REG,          LABEL } },
    { "greq_jsr_l",         3, { L_REG,         L_REG,          LABEL } },
    { "lseq_jsr_l",         3, { L_REG,         L_REG,          LABEL } },
    { "curson",             0, { EMPTY,         EMPTY,          EMPTY } },
    { "cursoff",            0, { EMPTY,         EMPTY,          EMPTY } },
    { "cursleft",           1, { L_REG,         EMPTY,          EMPTY } },
    { "cursright",          1, { L_REG,         EMPTY,          EMPTY } },
    { "cursup",             1, { L_REG,         EMPTY,          EMPTY } },
    { "cursdown",           1, { L_REG,         EMPTY,          EMPTY } },
    { "ton",                0, { EMPTY,         EMPTY,          EMPTY } },
    { "toff",               0, { EMPTY,         EMPTY,          EMPTY } },
    { "time",               0, { EMPTY,         EMPTY,          EMPTY } },
    { "wait_s",             1, { L_REG,         EMPTY,          EMPTY } },
    { "wait_t",             1, { L_REG,         EMPTY,          EMPTY } },
    { "fopen",              3, { L_REG,         S_REG,          S_REG } },
    { "fclose",             1, { L_REG,         EMPTY,          EMPTY } },
    { "fread_b",            2, { L_REG,         L_REG,          EMPTY } },
    { "fread_ab",           3, { L_REG,         BYTE_A_VAR,     L_REG } },
    { "fread_i",            2, { L_REG,         L_REG,          EMPTY } },
    { "fread_l",            2, { L_REG,         L_REG,          EMPTY } },
    { "fread_q",            2, { L_REG,         L_REG,          EMPTY } },
    { "fread_d",            2, { L_REG,         D_REG,          EMPTY } },
    { "fread_s",            3, { L_REG,         S_REG,          L_REG } },
    { "fread_ls",           2, { L_REG,         S_REG,          EMPTY } },
    { "fwrite_b",           2, { L_REG,         L_REG,          EMPTY } },
    { "fwrite_ab",          3, { L_REG,         BYTE_A_VAR,     L_REG } },
    { "fwrite_i",           2, { L_REG,         L_REG,          EMPTY } },
    { "fwrite_l",           2, { L_REG,         L_REG,          EMPTY } },
    { "fwrite_q",           2, { L_REG,         L_REG,          EMPTY } },
    { "fwrite_d",           2, { L_REG,         D_REG,          EMPTY } },
    { "fwrite_s",           2, { L_REG,         S_REG,          EMPTY } },
    { "fwrite_sl",          2, { L_REG,         L_REG,          EMPTY } },
    { "fwrite_sd",          2, { L_REG,         D_REG,          EMPTY } },
    { "fwrite_n",           2, { L_REG,         L_REG,          EMPTY } },
    { "fwrite_sp",          2, { L_REG,         L_REG,          EMPTY } },
    { "fsetpos",            2, { L_REG,         L_REG,          EMPTY } },
    { "fgetpos",            2, { L_REG,         L_REG,          EMPTY } },
    { "frewind",            1, { L_REG,         EMPTY,          EMPTY } },
    { "fsize",              2, { L_REG,         L_REG,          EMPTY } },
    { "fremove",            1, { S_REG,         EMPTY,          EMPTY } },
    { "frename",            2, { S_REG,         S_REG,          EMPTY } },
    { "ssopen",             3, { L_REG,         S_REG,          L_REG } },
    { "ssopenact",          2, { L_REG,         L_REG,          EMPTY } },
    { "scopen",             3, { L_REG,         S_REG,          L_REG } },
    { "ssclose",            1, { L_REG,         EMPTY,          EMPTY } },
    { "sscloseact",         1, { L_REG,         EMPTY,          EMPTY } },
    { "scclose",            1, { L_REG,         EMPTY,          EMPTY } },
    { "clientaddr",         2, { L_REG,         S_REG,          EMPTY } },
    { "hostname",           1, { S_REG,         EMPTY,          EMPTY } },
    { "hostbyname",         2, { S_REG,         S_REG,          EMPTY } },
    { "hostbyaddr",         2, { S_REG,         S_REG,          EMPTY } },
    { "sread_b",            2, { L_REG,         L_REG,          EMPTY } },
    { "sread_ab",           3, { L_REG,         BYTE_A_VAR,     L_REG } },
    { "sread_i",            2, { L_REG,         L_REG,          EMPTY } },
    { "sread_l",            2, { L_REG,         L_REG,          EMPTY } },
    { "sread_q",            2, { L_REG,         L_REG,          EMPTY } },
    { "sread_d",            2, { L_REG,         D_REG,          EMPTY } },
    { "sread_s",            3, { L_REG,         S_REG,          L_REG } },
    { "sread_ls",           2, { L_REG,         S_REG,          EMPTY } },
    { "swrite_b",           2, { L_REG,         L_REG,          EMPTY } },
    { "swrite_ab",          3, { L_REG,         BYTE_A_VAR,     L_REG } },
    { "swrite_i",           2, { L_REG,         L_REG,          EMPTY } },
    { "swrite_l",           2, { L_REG,         L_REG,          EMPTY } },
    { "swrite_q",           2, { L_REG,         L_REG,          EMPTY } },
    { "swrite_d",           2, { L_REG,         D_REG,          EMPTY } },
    { "swrite_s",           2, { L_REG,         S_REG,          EMPTY } },
    { "swrite_sl",          2, { L_REG,         L_REG,          EMPTY } },
    { "swrite_sd",          2, { L_REG,         D_REG,          EMPTY } },
    { "swrite_n",           2, { L_REG,         L_REG,          EMPTY } },
    { "swrite_sp",          2, { L_REG,         L_REG,          EMPTY } },
    { "sizeof",             2, { VAR,           L_REG,          EMPTY } },
    { "vmvar",              2, { VAR,           L_REG,          EMPTY } },
    { "val_l",              2, { S_REG,         L_REG,          EMPTY } },
    { "val_d",              2, { S_REG,         D_REG,          EMPTY } },
    { "str_l",              2, { L_REG,         S_REG,          EMPTY } },
    { "str_d",              2, { D_REG,         S_REG,          EMPTY } },
    { "2int",               2, { D_REG,         L_REG,          EMPTY } },
    { "2double",            2, { L_REG,         D_REG,          EMPTY } },
    { "srand_l",            1, { L_REG,         EMPTY,          EMPTY } },
    { "rand_l",             1, { L_REG,         EMPTY,          EMPTY } },
    { "rand_d",             1, { D_REG,         EMPTY,          EMPTY } },
    { "abs_l",              2, { L_REG,         L_REG,          EMPTY } },
    { "abs_d",              2, { D_REG,         D_REG,          EMPTY } },
    { "ceil_d",             2, { D_REG,         D_REG,          EMPTY } },
    { "floor_d",            2, { D_REG,         D_REG,          EMPTY } },
    { "exp_d",              2, { D_REG,         D_REG,          EMPTY } },
    { "log_d",              2, { D_REG,         D_REG,          EMPTY } },
    { "log10_d",            2, { D_REG,         D_REG,          EMPTY } },
    { "pow_d",              3, { D_REG,         D_REG,          D_REG } },
    { "sqrt_d",             2, { D_REG,         D_REG,          EMPTY } },
    { "cos_d",              2, { D_REG,         D_REG,          EMPTY } },
    { "sin_d",              2, { D_REG,         D_REG,          EMPTY } },
    { "tan_d",              2, { D_REG,         D_REG,          EMPTY } },
    { "acos_d",             2, { D_REG,         D_REG,          EMPTY } },
    { "asin_d",             2, { D_REG,         D_REG,          EMPTY } },
    { "atan_d",             2, { D_REG,         D_REG,          EMPTY } },
    { "cosh_d",             2, { D_REG,         D_REG,          EMPTY } },
    { "sinh_d",             2, { D_REG,         D_REG,          EMPTY } },
    { "tanh_d",             2, { D_REG,         D_REG,          EMPTY } },
    { "runpr",              2, { S_REG,         L_REG,          EMPTY } },
    { "runsh",              2, { S_REG,         L_REG,          EMPTY } },
    { "waitpr",             2, { L_REG,         L_REG,          EMPTY } },
    { "argnum",             1, { L_REG,         EMPTY,          EMPTY } },
    { "argstr",             2, { L_REG,         S_REG,          EMPTY } },
    { "stpush_l",           1, { L_REG,         EMPTY,          EMPTY } },
    { "stpush_d",           1, { D_REG,         EMPTY,          EMPTY } },
    { "stpush_s",           1, { S_REG,         EMPTY,          EMPTY } },
    { "stpush_all_l",       0, { EMPTY,         EMPTY,          EMPTY } },
    { "stpush_all_d",       0, { EMPTY,         EMPTY,          EMPTY } },
    { "stpull_l",           1, { L_REG,         EMPTY,          EMPTY } },
    { "stpull_d",           1, { D_REG,         EMPTY,          EMPTY } },
    { "stpull_s",           1, { S_REG,         EMPTY,          EMPTY } },
    { "stpull_all_l",       0, { EMPTY,         EMPTY,          EMPTY } },
    { "stpull_all_d",       0, { EMPTY,         EMPTY,          EMPTY } },
    { "ston",               0, { EMPTY,         EMPTY,          EMPTY } },
    { "show_stack",         1, { L_REG,         EMPTY,          EMPTY } },
    { "stgettype",          1, { L_REG,         EMPTY,          EMPTY } },
    { "stelements",         1, { L_REG,         EMPTY,          EMPTY } },
    { "gettype",            2, { L_REG,         L_REG,          EMPTY } },
    { "alloc",              1, { VAR,           EMPTY,          EMPTY } },
    { "dealloc",            1, { VAR,           EMPTY,          EMPTY } },
    { "getaddress",         2, { VAR,           L_REG,          EMPTY } },
    { "pointer",            3, { L_REG,         VAR,            LABEL } },
    { "nop",                0, { EMPTY,         EMPTY,          EMPTY } },
    { "exit",               1, { L_REG,         EMPTY,          EMPTY } },
    { "eexit",              0, { EMPTY,         EMPTY,          EMPTY } },
    { "inc_ls_jmp_l",       3, { L_REG,         L_REG,          LABEL } },
    { "inc_lseq_jmp_l",     3, { L_REG,         L_REG,          LABEL } },
    { "dec_gr_jmp_l",       3, { L_REG,         L_REG,          LABEL } },
    { "dec_greq_jmp_l",     3, { L_REG,         L_REG,          LABEL } },
    { "inc_ls_jsr_l",       3, { L_REG,         L_REG,          LABEL } },
    { "inc_lseq_jsr_l",     3, { L_REG,         L_REG,          LABEL } },
    { "dec_gr_jsr_l",       3, { L_REG,         L_REG,          LABEL } },
    { "dec_greq_jsr_l",     3, { L_REG,         L_REG,          LABEL } },
    { "hash32_ab",          3, { BYTE_A_VAR,    L_REG,          L_REG } },
    { "stack_set",          0, { EMPTY,         EMPTY,          EMPTY } },
    { "push_s",             2, { STRING_VAR,    S_REG,          EMPTY } },
    { "pull_s",             2, { S_REG,         STRING_VAR,     EMPTY } },
    { "stpush_all_s",       0, { EMPTY,         EMPTY,          EMPTY } },
    { "stpull_all_s",       0, { EMPTY,         EMPTY,          EMPTY } },
    { "not_l",              2, { L_REG,         L_REG,          EMPTY } },
    { "var_lock",           1, { VAR,           EMPTY,          EMPTY } },
    { "var_unlock",         1, { VAR,           EMPTY,          EMPTY } },
    { "thread_create",      2, { L_REG,         LABEL,          EMPTY } },
    { "thread_join",        0, { EMPTY          EMPTY,          EMPTY } },
    { "thread_state",       2, { L_REG,         L_REG,          EMPTY } },
    { "thread_num",         1, { L_REG,         EMPTY,          EMPTY } },
    { "thread_sync",        1, { L_REG,         EMPTY,          EMPTY } },
    { "thpush_l",           2, { L_REG,         L_REG,          EMPTY } },
    { "thpush_d",           2, { L_REG,         D_REG,          EMPTY } },
    { "thpush_s",           2, { L_REG,         S_REG,          EMPTY } },
    { "thpush_sync",        1, { L_REG,         EMPTY,          EMPTY } },
    { "thpull_sync",        0, { EMPTY,         EMPTY,          EMPTY } },
    { "thexit_code",        2, { L_REG,         L_REG,          EMPTY } },
    { "palloc",             1, { VAR,           EMPTY,          EMPTY } },
    { "pdealloc",           1, { VAR,           EMPTY,          EMPTY } },
    { "ppush_i",            2, { INT_VAR,       L_REG,          EMPTY } },
    { "ppush_l",            2, { LINT_VAR,      L_REG,          EMPTY } },
    { "ppush_q",            2, { QINT_VAR,      L_REG,          EMPTY } },
    { "ppush_d",            2, { DOUBLE_VAR,    D_REG,          EMPTY } },
    { "ppush_b",            2, { BYTE_VAR,      L_REG,          EMPTY } },
    { "ppush_s",            2, { STRING_VAR,    S_REG,          EMPTY } },
    { "ppull_i",            2, { L_REG,         INT_VAR,        EMPTY } },
    { "ppull_l",            2, { L_REG,         LINT_VAR,       EMPTY } },
    { "ppull_q",            2, { L_REG,         QINT_VAR,       EMPTY } },
    { "ppull_d",            2, { D_REG,         DOUBLE_VAR,     EMPTY } },
    { "ppull_b",            2, { L_REG,         BYTE_VAR,       EMPTY } },
    { "ppull_s",            2, { S_REG,         STRING_VAR,     EMPTY } },
    { "pmove_a_i",          3, { INT_A_VAR,     L_REG,          L_REG } },
    { "pmove_a_l",          3, { LINT_A_VAR,    L_REG,          L_REG } },
    { "pmove_a_q",          3, { QINT_A_VAR,    L_REG,          L_REG } },
    { "pmove_a_d",          3, { DOUBLE_A_VAR,  L_REG,          D_REG } },
    { "pmove_a_b",          3, { BYTE_A_VAR,    L_REG,          L_REG } },
    { "pmove_a_s",          3, { STRING_VAR,    L_REG,          S_REG } },
    { "pmove_i_a",          3, { L_REG,         INT_A_VAR,      L_REG } },
    { "pmove_l_a",          3, { L_REG,         LINT_A_VAR,     L_REG } },
    { "pmove_q_a",          3, { L_REG,         QINT_A_VAR,     L_REG } },
    { "pmove_d_a",          3, { D_REG,         DOUBLE_A_VAR,   L_REG } },
    { "pmove_b_a",          3, { L_REG,         BYTE_A_VAR,     L_REG } },
    { "pmove_s_a",          3, { S_REG,         STRING_VAR,     L_REG } },
    { "pfread_ab",          3, { L_REG,         BYTE_A_VAR,     L_REG } },
    { "pfwrite_ab",         3, { L_REG,         BYTE_A_VAR,     L_REG } },
    { "psread_ab",          3, { L_REG,         BYTE_A_VAR,     L_REG } },
    { "pswrite_ab",         3, { L_REG,         BYTE_A_VAR,     L_REG } },
    { "2hex",               2, { L_REG,         S_REG,          EMPTY } },
    { "phash32_ab",         3, { BYTE_A_VAR,    L_REG,          L_REG } },
    { "hash64_ab",          3, { BYTE_A_VAR,    L_REG,          L_REG } },
    { "phash64_ab",         3, { BYTE_A_VAR,    L_REG,          L_REG } },
    
    { "move_adyn_q",        3, { DYNAMIC_A_VAR, L_REG,          L_REG } },
    { "move_adyn_d",        3, { DYNAMIC_A_VAR, L_REG,          D_REG } },
    { "move_adyn_s",        3, { DYNAMIC_A_VAR, L_REG,          S_REG } },
    { "move_q_adyn",        3, { L_REG,         DYNAMIC_A_VAR,  L_REG } },
    { "move_d_adyn",        3, { D_REG,         DYNAMIC_A_VAR,  L_REG } },
    { "move_s_adyn",        3, { S_REG,         DYNAMIC_A_VAR,  L_REG } },
    
    { "pmove_adyn_q",       3, { DYNAMIC_A_VAR, L_REG,          L_REG } },
    { "pmove_adyn_d",       3, { DYNAMIC_A_VAR, L_REG,          D_REG } },
    { "pmove_adyn_s",       3, { DYNAMIC_A_VAR, L_REG,          S_REG } },
    { "pmove_q_adyn",       3, { L_REG,         DYNAMIC_A_VAR,  L_REG } },
    { "pmove_d_adyn",       3, { D_REG,         DYNAMIC_A_VAR,  L_REG } },
    { "pmove_s_adyn",       3, { S_REG,         DYNAMIC_A_VAR,  L_REG } },
    
    { "dyngettype",         3, { DYNAMIC_A_VAR, L_REG,          L_REG } },
    { "pdyngettype",        3, { DYNAMIC_A_VAR, L_REG,          L_REG } },
    
    { "vadd_l",             3, { QINT_A_VAR,    L_REG,          QINT_A_VAR } }, 
    { "vadd_d",             3, { DOUBLE_A_VAR,  D_REG,          DOUBLE_A_VAR } },
    
    { "vsub_l",             3, { QINT_A_VAR,    L_REG,          QINT_A_VAR } }, 
    { "vsub_d",             3, { DOUBLE_A_VAR,  D_REG,          DOUBLE_A_VAR } },
    
    { "vmul_l",             3, { QINT_A_VAR,    L_REG,          QINT_A_VAR } }, 
    { "vmul_d",             3, { DOUBLE_A_VAR,  D_REG,          DOUBLE_A_VAR } },
    
    { "vdiv_l",             3, { QINT_A_VAR,    L_REG,          QINT_A_VAR } }, 
    { "vdiv_d",             3, { DOUBLE_A_VAR,  D_REG,          DOUBLE_A_VAR } },
    
    { "vadd2_l",            3, { QINT_A_VAR,    QINT_A_VAR,     QINT_A_VAR } }, 
    { "vadd2_d",            3, { DOUBLE_A_VAR,  DOUBLE_A_VAR,   DOUBLE_A_VAR } },
    
    { "vsub2_l",            3, { QINT_A_VAR,    QINT_A_VAR,     QINT_A_VAR } }, 
    { "vsub2_d",            3, { DOUBLE_A_VAR,  DOUBLE_A_VAR,   DOUBLE_A_VAR } },
    
    { "vmul2_l",            3, { QINT_A_VAR,    QINT_A_VAR,     QINT_A_VAR } }, 
    { "vmul2_d",            3, { DOUBLE_A_VAR,  DOUBLE_A_VAR,   DOUBLE_A_VAR } },
    
    { "vdiv2_l",            3, { QINT_A_VAR,    QINT_A_VAR,     QINT_A_VAR } }, 
    { "vdiv2_d",            3, { DOUBLE_A_VAR,  DOUBLE_A_VAR,   DOUBLE_A_VAR } },
    
    { "pvadd_l",             3, { QINT_A_VAR,    L_REG,          QINT_A_VAR } }, 
    { "pvadd_d",             3, { DOUBLE_A_VAR,  D_REG,          DOUBLE_A_VAR } },
    
    { "pvsub_l",             3, { QINT_A_VAR,    L_REG,          QINT_A_VAR } }, 
    { "pvsub_d",             3, { DOUBLE_A_VAR,  D_REG,          DOUBLE_A_VAR } },
    
    { "pvmul_l",             3, { QINT_A_VAR,    L_REG,          QINT_A_VAR } }, 
    { "pvmul_d",             3, { DOUBLE_A_VAR,  D_REG,          DOUBLE_A_VAR } },
    
    { "pvdiv_l",             3, { QINT_A_VAR,    L_REG,          QINT_A_VAR } }, 
    { "pvdiv_d",             3, { DOUBLE_A_VAR,  D_REG,          DOUBLE_A_VAR } },
    
    { "pvadd2_l",            3, { QINT_A_VAR,    QINT_A_VAR,     QINT_A_VAR } }, 
    { "pvadd2_d",            3, { DOUBLE_A_VAR,  DOUBLE_A_VAR,   DOUBLE_A_VAR } },
    
    { "pvsub2_l",            3, { QINT_A_VAR,    QINT_A_VAR,     QINT_A_VAR } }, 
    { "pvsub2_d",            3, { DOUBLE_A_VAR,  DOUBLE_A_VAR,   DOUBLE_A_VAR } },
    
    { "pvmul2_l",            3, { QINT_A_VAR,    QINT_A_VAR,     QINT_A_VAR } }, 
    { "pvmul2_d",            3, { DOUBLE_A_VAR,  DOUBLE_A_VAR,   DOUBLE_A_VAR } },
    
    { "pvdiv2_l",            3, { QINT_A_VAR,    QINT_A_VAR,     QINT_A_VAR } }, 
    { "pvdiv2_d",            3, { DOUBLE_A_VAR,  DOUBLE_A_VAR,   DOUBLE_A_VAR } },
    
    { "atan2_d",            3, { D_REG,         D_REG,          D_REG } },
    
    { "ppointer",           3, { L_REG,         VAR,            LABEL } },
    { "pgettype",           2, { L_REG,         L_REG,          EMPTY } },
    
    
    { "move_ip_b",          3, { INT_VAR,       L_REG,          L_REG } },
    { "move_lp_b",          3, { LINT_VAR,      L_REG,          L_REG } },
    { "move_qp_b",          3, { QINT_VAR,      L_REG,          L_REG } },
    { "move_dp_b",          3, { DOUBLE_VAR,    L_REG,          L_REG } },
    
    { "move_b_ip",          3, { L_REG,         INT_VAR,        L_REG } },
    { "move_b_lp",          3, { L_REG,         LINT_VAR,       L_REG } },
    { "move_b_qp",          3, { L_REG,         QINT_VAR,       L_REG } },
    { "move_b_dp",          3, { L_REG,         DOUBLE_VAR,     L_REG } },
    
    { "pmove_ip_b",         3, { INT_VAR,       L_REG,          L_REG } },
    { "pmove_lp_b",         3, { LINT_VAR,      L_REG,          L_REG } },
    { "pmove_qp_b",         3, { QINT_VAR,      L_REG,          L_REG } },
    { "pmove_dp_b",         3, { DOUBLE_VAR,    L_REG,          L_REG } },
    
    { "pmove_b_ip",         3, { L_REG,         INT_VAR,        L_REG } },
    { "pmove_b_lp",         3, { L_REG,         LINT_VAR,       L_REG } },
    { "pmove_b_qp",         3, { L_REG,         QINT_VAR,       L_REG } },
    { "pmove_b_dp",         3, { L_REG,         DOUBLE_VAR,     L_REG } },
    
    { "password",           2, { S_REG,         L_REG,          EMPTY } },
    { "makedir",            1, { S_REG,         EMPTY,          EMPTY } },
    
    { "mod_d",              3, { D_REG,         D_REG,          D_REG } },
	
	{ "hsvar",				2, { VAR,			L_REG,			EMPTY } },
	{ "hsserver",			1, { L_REG,			EMPTY,			EMPTY } },
	{ "hsload",				2, { VAR,			L_REG,			EMPTY } },
	{ "hssave",				2, { VAR,			L_REG,			EMPTY } },

	{ "stpush_all_all",		0, { EMPTY,         EMPTY,          EMPTY } },
	{ "stpull_all_all",		0, { EMPTY,         EMPTY,          EMPTY } }, 
	
	{ "jit",				2, { LABEL,			LABEL,			EMPTY } },
	{ "runjitcode",			2, { L_REG,			LABEL, 			EMPTY } },
	
	{ "dobjects",			2, { S_REG,			L_REG,			EMPTY } },
	{ "dnames", 			2, { S_REG,			STRING_VAR,		EMPTY } },
	{ "pdnames",			2, { S_REG,			STRING_VAR,		EMPTY } },
	
	{ "lopen",				2, { L_REG,			S_REG,			EMPTY }, },
	{ "lclose",				1, { L_REG,			EMPTY,			EMPTY }, },
	{ "lfunc",				3, { L_REG,			L_REG,			S_REG }, },
	{ "lcall", 				2, { L_REG,			L_REG,			EMPTY }, },
	
    { "int",                1, { VAR,           EMPTY,          EMPTY } },
    { "lint",               1, { VAR,           EMPTY,          EMPTY } },
    { "qint",               1, { VAR,           EMPTY,          EMPTY } },
    { "double",             1, { VAR,           EMPTY,          EMPTY } },
    { "string",             1, { VAR,           EMPTY,          EMPTY } },
    { "byte",               1, { VAR,           EMPTY,          EMPTY } },
    { "dynamic",            1, { VAR,           EMPTY,          EMPTY } },
    { "lab",                1, { LABEL_SET,     EMPTY,          EMPTY } },
    { "#setreg_l",          2, { L_REG,         STRING_VAR,     EMPTY } },
    { "#setreg_d",          2, { D_REG,         STRING_VAR,     EMPTY } },
    { "#setreg_s",          2, { S_REG,         STRING_VAR,     EMPTY } },
    { "#unsetreg_all_l",    0, { EMPTY,         EMPTY,          EMPTY } },
    { "#unsetreg_all_d",    0, { EMPTY,         EMPTY,          EMPTY } },
    { "#unsetreg_all_s",    0, { EMPTY,         EMPTY,          EMPTY } },
    { "#setquote",          1, { VAR,           EMPTY,          EMPTY } },
    { "#setsepar",          1, { VAR,           EMPTY,          EMPTY } },
    { "#setsemicol",        1, { VAR,           EMPTY,          EMPTY } }
};

