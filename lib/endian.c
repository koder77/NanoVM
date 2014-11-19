/****************************************************************************
*
* Filename: endian.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  convert endianess
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2003
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

S2 conv_int (S2 num)
{
    U1 *num_ptr, *new_ptr;
    S2 new;

    num_ptr = (U1 *) &num;
    new_ptr = (U1 *) &new;

    new_ptr[0] = num_ptr[1];
    new_ptr[1] = num_ptr[0];

    return (new);
}

S4 conv_lint (S4 num)
{
    U1 *num_ptr, *new_ptr;
    S4 new;

    num_ptr = (U1 *) &num;
    new_ptr = (U1 *) &new;

    new_ptr[0] = num_ptr[3];
    new_ptr[1] = num_ptr[2];
    new_ptr[2] = num_ptr[1];
    new_ptr[3] = num_ptr[0];

    return (new);
}

S8 conv_qint (S8 num)
{
    U1 *num_ptr, *new_ptr;
    S8 new;

    num_ptr = (U1 *) &num;
    new_ptr = (U1 *) &new;

    new_ptr[0] = num_ptr[7];
    new_ptr[1] = num_ptr[6];
    new_ptr[2] = num_ptr[5];
    new_ptr[3] = num_ptr[4];
    new_ptr[4] = num_ptr[3];
    new_ptr[5] = num_ptr[2];
    new_ptr[6] = num_ptr[1];
    new_ptr[7] = num_ptr[0];

    return (new);
}

F8 conv_double (F8 num)
{
    U1 *num_ptr, *new_ptr;
    F8 new;

    num_ptr = (U1 *) &num;
    new_ptr = (U1 *) &new;

    new_ptr[0] = num_ptr[7];
    new_ptr[1] = num_ptr[6];
    new_ptr[2] = num_ptr[5];
    new_ptr[3] = num_ptr[4];
    new_ptr[4] = num_ptr[3];
    new_ptr[5] = num_ptr[2];
    new_ptr[6] = num_ptr[1];
    new_ptr[7] = num_ptr[0];

    return (new);
}

