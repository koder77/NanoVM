/****************************************************************************
*
* Filename: comp_opcode.h
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  N compiler: built in opcodes
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

struct opcode comp_opcode[] =
{
    { "func" },
    { "funcend" },
    { "" },
    { "byte" },
    { "int" },
    { "lint" },
    { "qint" },
    { "double" },
    { "string" },
    { "dynamic" },
    { "print" },
    { "printn" },
    { "exit" },
    { "get" },
    { "" },
    { "if" },
    { "else" },
    { "endif" },
    { "for" },
    { "next" },
    { "lab" },
    { "goto" },
};
