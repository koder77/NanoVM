/****************************************************************************
*
* Filename: protos.h
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  N compiler: protos
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

S4** alloc_array_lint (size_t n_rows, size_t n_columns);
U1 exe_alloc_var (struct varlist *varlist, S4 ind, S4 type);
U1 make_val (U1 type, struct varlist *varlist, U1 private);
void convtabs (U1 *str);
void skipspaces (U1 *str, U1 *retstr);
void partstr (U1 *str, U1 *retstr, S2 start, S2 end);
void getstr (U1 *str, U1 *retstr, S2 maxlen, S2 start, S2 end);
S2 searchstr (U1 *str, U1 *srchstr, S2 start, S2 end, U1 case_sens);

