/****************************************************************************
*
* Filename: exe_file.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  file in/output
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

#if OS_AROS
    #define LOCK();              LockMutex (file_mutex);
    #define UNLOCK();            UnlockMutex (file_mutex);
#else
    #define LOCK();              pthread_mutex_lock (&file_mutex);
    #define UNLOCK();            pthread_mutex_unlock (&file_mutex);
#endif

extern struct varlist *varlist;
extern struct file file[];

extern U1 filename[MAXSTRING_VAR + 1];

#if OS_AROS
    extern void *file_mutex;
#else
    extern pthread_mutex_t file_mutex;
#endif

extern struct vm_mem vm_mem;

#if DEBUG
    extern U1 print_debug;
#endif

extern struct rights rights;



U1 check_path_break (U1 *path)
{
	/* check for forbidden ../ in pathname */ 
	
	S2 slen, i;
	
	slen = strlen (path);
	
	for (i = 0; i < slen - 1; i++)
	{
		if (path[i] == '.')
		{
			if (path[i + 1] == '.')
			{
				return (1);
			}
		}
	}
	
	/* check for forbidden .nr rights file access */
	
	if (path[slen - 3] == '.' && path[slen - 2] == 'n' && path[slen - 1] == 'r')
	{
		return (1);
	}
	
	return (0);  /* path legal, all ok! */
}
			
	

U1 conv_fname (U1 *new_fname, U1 *fname)
{
    /* S2 searchstr (U1 *str, U1 *searchstr, S2 start, S2 end, U1 case_sens) */

    #if PATH_SLASH_CONV
        /* drive:path/file to drive:\path\file */

        U1 str[2];
        S2 i, j, pos, pos_end;

        str[0] = COL_SB; str[1] = BINUL;

        pos_end = strlen (fname) - 1;
        pos = 0;

        pos = searchstr (fname, str, pos, pos_end, TRUE);
        if (pos > -1)
        {
            /* insert backslash */

            if (pos == pos_end)
            {
                strcpy (new_fname, "");
                return (FALSE);
            }

            for (i = 0; i <= pos; i++)
            {
                new_fname[i] = fname[i];
            }
            new_fname[i] = BACKSLASH_SB;
            j = i + 1;
            for (i = pos + 1; i <= pos_end; i++)
            {
                new_fname[j] = fname[i];
                j++;
            }
            new_fname[j] = BINUL;
        }
        else
        {
            strcpy (new_fname, fname);
        }

        str[0] = DIV_SB; str[1] = BINUL;

        pos_end = strlen (new_fname) - 1;
        pos = 0;
        while (pos > -1)
        {
            pos = searchstr (new_fname, str, pos, pos_end, TRUE);
            if (pos > -1)
            {
                /* change slash to backslash */

                new_fname[pos] = BACKSLASH_SB;
            }
        }
    #else
        strcpy (new_fname, fname);
    #endif

    return (TRUE);
}

S2 get_ferr (S2 err)
{
    S2 ferr;

    switch (err)
    {
        case ERR_FILE_OPEN:
            ferr = OPEN;
            break;

        case ERR_FILE_CLOSE:
            ferr = CLOSE;
            break;

        case ERR_FILE_READ:
            ferr = READ;
            break;

        case ERR_FILE_WRITE:
            ferr = WRITE;
            break;

        case ERR_FILE_NUMBER:
            ferr = FNUMBER;
            break;

        case ERR_FILE_EOF:
            ferr = MEOF;
            break;

        case ERR_FILE_FPOS:
            ferr = FPOS;
            break;

        default:
            ferr = err;
            break;
    }
    return (ferr);
}

U1 expand_pathname (U1 *path, U1 *newpath)
{
	if (path[0] == '/')
	{
		/* file root access, check if possible */
		
		if (rights.fileroot == FALSE)
		{
			printf ("ERROR: no rights for root file access!\n");
			UNLOCK();
			return (ERR_FILE_OPEN);
		}
		
		#if OS_ANDROID
			strcpy (newpath, "/sdcard/nanovm/");
		#else
			/* check ENV variable */
		
			if (getenv (NANOVM_ROOT_SB) == NULL)
			{
				/* ENV not set use defaults */
			
				if (OS_AMIGA || OS_AROS)
				{
					strcpy (newpath, "Work:nanovm/");
				}

				if (OS_LINUX)
				{
					strcpy (newpath, "~/nanovm/");
				}
            
				if (OS_WINDOWS)
				{
					strcpy (newpath, "C:/nanovm/");
				}
			}
			else
			{
				/* use ENV variable setting */
				strcpy (newpath, getenv (NANOVM_ROOT_SB));
			}
		#endif
	}
	else
	{
		/* /home directory access */
		
		#if OS_ANDROID
			strcpy (newpath, "/sdcard/nanovm/home/");
		#else
			/* check ENV variable */
		
			if (getenv (NANOVM_ROOT_SB) == NULL)
			{
				/* ENV not set use defaults */
			
				if (OS_AMIGA || OS_AROS)
				{
					strcpy (newpath, "Work:nanovm/home/");
				}

				if (OS_LINUX)
				{
					strcpy (newpath, "~/nanovm/home/");
				}
            
				if (OS_WINDOWS)
				{
					strcpy (newpath, "C:/nanovm/home/");
				}
			}
			else
			{
				/* use ENV variable setting */
				strcpy (newpath, getenv (NANOVM_ROOT_SB));
				strcat (newpath, "/home/");
			}	
		#endif
	}
	
	strcat (newpath, path);
	return (0);
}

U1 exe_fopen (S8 *flist_ind, U1 *fname, U1 *flag)
{
    U1 new_fname[MAXSTRING_VAR + 1], path[MAXSTRING_VAR + 1], convname[MAXSTRING_VAR + 1], open_flag[4];
    S2 type = -1;
    S4 i, filehandle = -1;

	if (rights.file == FALSE)
	{
		printf ("ERROR: no rights for file access!\n");
		return (ERR_FILE_OPEN);
	}
	
    LOCK();
    for (i = 0; i < MAXFILES; i++)
    {
        if (file[i].status == FILECLOSED)
        {
            filehandle = i;
            break;
        }
    }

    if (filehandle == -1)
    {
        /* error no free file */
        UNLOCK();
        return (ERR_FILE_OPEN);
    }

    *flist_ind = filehandle;

    /* type */

    if (strcmp (flag, FILEREAD_SB) == 0)
    {
        strcpy (open_flag, "rb");
        type = FILEREAD;
    }
    if (strcmp (flag, FILEWRITE_SB) == 0)
    {
        strcpy (open_flag, "wb");
        type = FILEWRITE;
    }
    if (strcmp (flag, FILEAPPEND_SB) == 0)
    {
        strcpy (open_flag, "ab");
        type = FILEAPPEND;
    }
    if (strcmp (flag, FILEREADWRITE_SB) == 0)
    {
        strcpy (open_flag, "r+b");
        type = FILEREADWRITE;
    }
    if (strcmp (flag, FILEWRITEREAD_SB) == 0)
    {
        strcpy (open_flag, "w+b");
        type = FILEREADWRITE;
    }
    if (strcmp (flag, FILEAPPENDREAD_SB) == 0)
    {
        strcpy (open_flag, "a+b");
        type = FILEREADWRITE;
    }

    if (type == -1)
    {
        /* bad flag */
        UNLOCK();
        return (SYNTAX);
    }

    file[*flist_ind].type = type;

    /* set endianess */

    file[*flist_ind].endian = (U1) *varlist[FILE_ENDIAN].i_m;

    /* name */

    strcpy (new_fname, "");
    strcpy (file[*flist_ind].name, fname);
    strcpy (new_fname, fname);

    #if DEBUG
        if (print_debug)
        {
            printf ("debug: exe_fopen  new filename: '%s'\n", new_fname);
        }
    #endif

    if (expand_pathname (new_fname, path) != 0)
	{
		UNLOCK();
		return (ERR_FILE_OPEN);
	}
	
	strcpy (new_fname, path);
    
	if (! conv_fname (convname, new_fname))
    {
        UNLOCK();
        return (ERR_FILE_OPEN);
    }
	
	if (check_path_break (convname) != 0)
	{
		printf ("ERROR: illegal filename!\n");
		
		UNLOCK();
		return (ERR_FILE_OPEN);
	}
	
    if ((file[*flist_ind].fptr = fopen (convname, open_flag)) != NULL)
    {
        file[*flist_ind].status = FILEOPEN;
        UNLOCK();
        return (ERR_FILE_OK);
    }
    else
    {
        UNLOCK();
        return (ERR_FILE_OPEN);
    }
}

U1 exe_fclose (S8 flist_ind)
{
    strcpy (filename, file[flist_ind].name);

    if (file[flist_ind].status == FILECLOSED)
    {
        return (ERR_FILE_CLOSE);
    }

    if (flist_ind < 0 || flist_ind > MAXFILES - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    fflush (file[flist_ind].fptr); 
    
    if (fclose (file[flist_ind].fptr) != EOF)
    {
        LOCK();
        file[flist_ind].status = FILECLOSED;
        UNLOCK();
        return (ERR_FILE_OK);
    }
    else
    {
        return (ERR_FILE_CLOSE);
    }
}

/* read/write ------------------------------------------------------------- */

U1 exe_fread (S4 flist_ind, S4 len)
{
    S2 pos;
    S4 read_size;

    strcpy (filename, file[flist_ind].name);

    if (file[flist_ind].type == FILEWRITE || file[flist_ind].type == FILEAPPEND || file[flist_ind].status == FILECLOSED)
    {
        return (ERR_FILE_READ);
    }

    if (flist_ind < 0 || flist_ind > MAXFILES - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    file[flist_ind].buf[0] = BINUL;         /* clear read buffer */

    if (len < 0)
    {
        /* read string */

        if (fgets_uni (file[flist_ind].buf, FILEBUFSIZE, file[flist_ind].fptr) == NULL)
        {
            return (ERR_FILE_EOF);
        }
        else
        {
            /* strip '\n' away */

            pos = strlen (file[flist_ind].buf) - 1;
            file[flist_ind].buf[pos] = BINUL;

            return (ERR_FILE_OK);
        }
    }
    else
    {
        /* binary read */

        if (len > file[flist_ind].bufsize)
        {
            /* try to allocate bigger buffer */
            free (file[flist_ind].buf);
            file[flist_ind].buf = NULL;

            file[flist_ind].bufsize = len;
            file[flist_ind].buf = (U1 *) malloc (file[flist_ind].bufsize * sizeof (U1));
            if (file[flist_ind].buf == NULL)
            {
                return (ERR_FILE_READ);
            }
        }

        read_size = fread (file[flist_ind].buf, sizeof (U1), len, file[flist_ind].fptr);
        if (read_size != len)
        {
			if (feof (file[flist_ind].fptr) < 0)
			{
				return (ERR_FILE_EOF);
			}
			else
			{
				return (ERR_FILE_READ);
			}
        }
        else
        {
            return (ERR_FILE_OK);
        }
    }
}

U1 exe_fwrite (S4 flist_ind, S4 len, U1 binary_write)
{
    S4 write_size;

    strcpy (filename, file[flist_ind].name);

    if (file[flist_ind].type == FILEREAD || file[flist_ind].status == FILECLOSED)
    {
        return (ERR_FILE_WRITE);
    }

    if (flist_ind < 0 || flist_ind > MAXFILES - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    /* binary_write FLAG set by exe_fwrite_array_byte */
    if (len < 0 || (len > FILEBUFSIZE && binary_write == FALSE))
    {
        return (ERR_FILE_WRITE);
    }

    write_size = fwrite (file[flist_ind].buf, sizeof (U1), len, file[flist_ind].fptr);
    if (write_size != len)
    {
        return (ERR_FILE_WRITE);
    }
    else
    {
        return (ERR_FILE_OK);
    }
}


/* read ------------------------------------------------------------- */

U1 exe_fread_byte (S8 flist_ind, S8 *reg)
{
    U1 ret;

    ret = exe_fread (flist_ind, sizeof (U1));
    if (ret != ERR_FILE_OK)
    {
        return (ret);
    }

    *reg = (S4) file[flist_ind].buf[0];

    return (ret);
}

U1 exe_fread_array_byte (S8 flist_ind, struct varlist *varlist, S4 b_var, S8 len)
{
    U1 ret;
    S4 i;

    if (len < 0)
    {
        return (ERR_FILE_READ);
    }

    ret = exe_fread (flist_ind, len);
    if (ret == ERR_FILE_READ || ret == ERR_FILE_NUMBER)
    {
        return (ret);
    }

    if (varlist[b_var].size >= len)
    {
        for (i = len - 1; i >= 0; i--)
        {
            varlist[b_var].s_m[i] = file[flist_ind].buf[i];
        }
    }
    else
    {
        /* array overflow */

        return (ERR_FILE_READ);
    }

    return (ERR_FILE_OK);
}

U1 exe_fread_int (S8 flist_ind, S8 *reg)
{
    U1 ret, *ptr;
    S2 i;
    S2 n;

    ret = exe_fread (flist_ind, sizeof (S2));
    if (ret != ERR_FILE_OK)
    {
        return (ret);
    }

    ptr = (U1 *) &n;

    for (i = 0; i <= sizeof (S2) - 1; i++)
    {
        *ptr++ = file[flist_ind].buf[i];
    }

    #if BS_LITTLE_ENDIAN
        /* host = little endian */

        if (file[flist_ind].endian)
        {
            *reg = (S4) n;              /* little endian */
        }
        else
        {
            *reg = (S4) conv_int (n);   /* big endian */
        }
    #else
        /* host = big endian */

        if (file[flist_ind].endian)
        {
            *reg = (S4) conv_int (n);   /* little endian */
        }
        else
        {
            *reg = (S4) n;              /* big endian */
        }
    #endif

    return (ret);
}

U1 exe_fread_lint (S8 flist_ind, S8 *reg)
{
    U1 ret, *ptr;
    S2 i;
    S4 n;

    ret = exe_fread (flist_ind, sizeof (S4));
    if (ret != ERR_FILE_OK)
    {
        return (ret);
    }

    ptr = (U1 *) &n;

    for (i = 0; i <= sizeof (S4) - 1; i++)
    {
        *ptr++ = file[flist_ind].buf[i];
    }

    #if BS_LITTLE_ENDIAN
        /* host = little endian */

        if (file[flist_ind].endian)
        {
            *reg = (S4) n;              /* little endian */
        }
        else
        {
            *reg = (S4) conv_lint (n);  /* big endian */
        }
    #else
        /* host = big endian */

        if (file[flist_ind].endian)
        {
            *reg = (S4) conv_lint (n);  /* little endian */
        }
        else
        {
            *reg = (S4) n;              /* big endian */
        }
    #endif

    return (ret);
}

U1 exe_fread_qint (S8 flist_ind, S8 *reg)
{
    U1 ret, *ptr;
    S2 i;
    S8 n;

    ret = exe_fread (flist_ind, sizeof (S8));
    if (ret != ERR_FILE_OK)
    {
        return (ret);
    }

    ptr = (U1 *) &n;

    for (i = 0; i <= sizeof (S8) - 1; i++)
    {
        *ptr++ = file[flist_ind].buf[i];
    }

    #if BS_LITTLE_ENDIAN
        /* host = little endian */

        if (file[flist_ind].endian)
        {
            *reg = (S8) n;              /* little endian */
        }
        else
        {
            *reg = (S8) conv_qint (n);  /* big endian */
        }
    #else
        /* host = big endian */

        if (file[flist_ind].endian)
        {
            *reg = (S8) conv_qint (n);  /* little endian */
        }
        else
        {
            *reg = (S8) n;              /* big endian */
        }
    #endif

    return (ret);
}

U1 exe_fread_double (S8 flist_ind, F8 *reg)
{
    U1 ret, *ptr;
    S2 i;
    F8 n;

    ret = exe_fread (flist_ind, sizeof (F8));
    if (ret != ERR_FILE_OK)
    {
        return (ret);
    }

    ptr = (U1 *) &n;

    for (i = 0; i <= sizeof (F8) - 1; i++)
    {
        *ptr++ = file[flist_ind].buf[i];
    }

    #if BS_LITTLE_ENDIAN
        /* host = little endian */

        if (file[flist_ind].endian)
        {
            *reg = (F8) n;                 /* little endian */
        }
        else
        {
            *reg = (F8) conv_double (n);   /* big endian */
        }
    #else
        /* host = big endian */

        if (file[flist_ind].endian)
        {
            *reg = (F8) conv_double (n);   /* little endian */
        }
        else
        {
            *reg = (F8) n;                 /* big endian */
        }
    #endif

    return (ret);
}

U1 exe_fread_string (S8 flist_ind, U1 *s_reg, S8 len)
{
    U1 ret;

    if (len < 0)
    {
        return (ERR_FILE_READ);
    }

    ret = exe_fread (flist_ind, len);
    if (ret == ERR_FILE_READ || ret == ERR_FILE_NUMBER)
    {
        return (ret);
    }

    file[flist_ind].buf[len] = BINUL;

    if (MAXSTRING_VAR + 1  >= len + 1)
    {
        strcpy (s_reg, file[flist_ind].buf);
    }
    else
    {
        /* string overflow */

        return (ERR_FILE_READ);
    }

    return (ERR_FILE_OK);
}

U1 exe_fread_line_string (S8 flist_ind, U1 *s_reg)
{
    /* read line terminated by (CR, LF, CRLF, LFCR) */

    U1 ret;

    ret = exe_fread (flist_ind, -1);
    if (ret == ERR_FILE_READ || ret == ERR_FILE_NUMBER)
    {
        return (ret);
    }

    if (MAXSTRING_VAR + 1 >= strlen (file[flist_ind].buf) + 1)
    {
        strcpy (s_reg, file[flist_ind].buf);

        return (ret);
    }
    else
    {
        /* string overflow */

        return (ERR_FILE_READ);
    }
}


/* write ------------------------------------------------------------ */

U1 exe_fwrite_byte (S8 flist_ind, S8 reg)
{
    U1 ret;

    file[flist_ind].buf[0] = (U1) reg;

    ret = exe_fwrite (flist_ind, sizeof (U1), FALSE);

    return (ret);
}

U1 exe_fwrite_array_byte (S8 flist_ind, struct varlist *varlist, S4 b_var, S8 len)
{
    U1 ret;
    S4 i;

    if (len < 0 || len > file[flist_ind].bufsize)
    {
        if (len > file[flist_ind].bufsize)
        {
            /* try to allocate bigger buffer */
            free (file[flist_ind].buf);
            file[flist_ind].buf = NULL;

            file[flist_ind].bufsize = len;
            file[flist_ind].buf = (U1 *) malloc (file[flist_ind].bufsize * sizeof (U1));
            if (file[flist_ind].buf == NULL)
            {
                return (ERR_FILE_WRITE);
            }
        }
    }

    for (i = len - 1; i >= 0; i--)
    {
        file[flist_ind].buf[i] = varlist[b_var].s_m[i];
    }

    ret = exe_fwrite (flist_ind, len, TRUE);

    return (ret);
}

U1 exe_fwrite_int (S8 flist_ind, S8 reg)
{
    U1 ret, *ptr;
    S2 i;
    S2 n, regv;

    regv = (S2) reg;

    #if BS_LITTLE_ENDIAN
        /* host = little endian */

        if (file[flist_ind].endian)
        {
            n = (S2) regv;              /* little endian */
        }
        else
        {
            n = (S2) conv_int (regv);   /* big endian */
        }
    #else
        /* host = big endian */

        if (file[flist_ind].endian)
        {
            n = (S2) conv_int (regv);   /* little endian */
        }
        else
        {
            n = (S2) regv;              /* big endian */
        }
    #endif

    ptr = (U1 *) &n;

    for (i = 0; i <= sizeof (S2) - 1; i++)
    {
        file[flist_ind].buf[i] = *ptr++;
    }

    ret = exe_fwrite (flist_ind, sizeof (S2), FALSE);

    return (ret);
}

U1 exe_fwrite_lint (S8 flist_ind, S8 reg)
{
    U1 ret, *ptr;
    S2 i;
    S4 n;

    #if BS_LITTLE_ENDIAN
        /* host = little endian */

        if (file[flist_ind].endian)
        {
            n = (S4) reg;              /* little endian */
        }
        else
        {
            n = (S4) conv_lint (reg);  /* big endian */
        }
    #else
        /* host = big endian */

        if (file[flist_ind].endian)
        {
            n = (S4) conv_lint (reg);  /* little endian */
        }
        else
        {
            n = (S4) reg;              /* big endian */
        }
    #endif

    ptr = (U1 *) &n;

    for (i = 0; i <= sizeof (S4) - 1; i++)
    {
        file[flist_ind].buf[i] = *ptr++;
    }

    ret = exe_fwrite (flist_ind, sizeof (S4), FALSE);

    return (ret);
}

U1 exe_fwrite_double (S8 flist_ind, F8 reg)
{
    U1 ret, *ptr;
    S2 i;
    F8 n;

    #if BS_LITTLE_ENDIAN
        /* host = little endian */

        if (file[flist_ind].endian)
        {
            n = (F8) reg;                 /* little endian */
        }
        else
        {
            n = (F8) conv_double (reg);   /* big endian */
        }
    #else
        /* host = big endian */

        if (file[flist_ind].endian)
        {
            n = (F8) conv_double (reg);   /* little endian */
        }
        else
        {
            n = (F8) reg;                 /* big endian */
        }
    #endif

    ptr = (U1 *) &n;

    for (i = 0; i <= sizeof (F8) - 1; i++)
    {
        file[flist_ind].buf[i] = *ptr++;
    }

    ret = exe_fwrite (flist_ind, sizeof (F8), FALSE);

    return (ret);
}

U1 exe_fwrite_qint (S8 flist_ind, S8 reg)
{
    U1 ret, *ptr;
    S2 i;
    S8 n;

    #if BS_LITTLE_ENDIAN
        /* host = little endian */

        if (file[flist_ind].endian)
        {
            n = (S8) reg;                 /* little endian */
        }
        else
        {
            n = (S8) conv_qint (reg);   /* big endian */
        }
    #else
        /* host = big endian */

        if (file[flist_ind].endian)
        {
            n = (S8) conv_qint (reg);   /* little endian */
        }
        else
        {
            n = (S8) reg;                 /* big endian */
        }
    #endif

    ptr = (U1 *) &n;

    for (i = 0; i <= sizeof (S8) - 1; i++)
    {
        file[flist_ind].buf[i] = *ptr++;
    }

    ret = exe_fwrite (flist_ind, sizeof (S8), FALSE);

    return (ret);
}

U1 exe_fwrite_string (S8 flist_ind, U1 *s_reg)
{
    U1 ret, buf_end = FALSE;
    S4 i, j, buf_ind = -1, len, nlch_len;

    if (strlen (s_reg) > FILEBUFSIZE)
    {
        return (ERR_FILE_WRITE);
    }

    nlch_len = strlen (varlist[FILE_NEWLINE].s_m) - 1;
    len = strlen (s_reg) - 1;

    for (i = 0; i <= len; i++)
    {
        if (s_reg[i] == '\n')
        {
            /* write newline string to buffer */

            for (j = 0; j <= nlch_len; j++)
            {
                if (buf_ind < FILEBUFSIZE - 2)
                {
                    buf_ind++;
                    file[flist_ind].buf[buf_ind] = varlist[FILE_NEWLINE].s_m[j];
                }
                else
                {
                    buf_end = TRUE;
                    break;
                }
            }
        }
        else
        {
            if (buf_ind < FILEBUFSIZE - 2)
            {
                buf_ind++;
                file[flist_ind].buf[buf_ind] = s_reg[i];
            }
            else
            {
                buf_end = TRUE;
            }
        }

        if (buf_end)
        {
            break;
        }
    }

    if (buf_end)
    {
        return (ERR_FILE_WRITE);
    }

    buf_ind++;
    file[flist_ind].buf[buf_ind] = BINUL;

    ret = exe_fwrite (flist_ind, strlen (file[flist_ind].buf), FALSE);

    return (ret);
}

U1 exe_fwrite_str_lint (S8 flist_ind, S8 reg)
{
    U1 ret;

    sprintf (file[flist_ind].buf, "%li", reg);

    ret = exe_fwrite (flist_ind, strlen (file[flist_ind].buf), FALSE);

    return (ret);
}

U1 exe_fwrite_str_double (S8 flist_ind, F8 reg)
{
    U1 ret;

    sprintf (file[flist_ind].buf, "%.10lf", reg);

    ret = exe_fwrite (flist_ind, strlen (file[flist_ind].buf), FALSE);

    return (ret);
}

U1 exe_fwrite_newline (S8 flist_ind, S8 reg)
{
    U1 ret;
    S4 i;
    S4 len;

    len = reg * strlen (varlist[FILE_NEWLINE].s_m);
    if (len > FILEBUFSIZE)
    {
        return (ERR_FILE_WRITE);
    }

    strcpy (file[flist_ind].buf, "");

    for (i = reg; i > 0; i--)
    {
        strcat (file[flist_ind].buf, varlist[FILE_NEWLINE].s_m);
    }

    ret = exe_fwrite (flist_ind, strlen (file[flist_ind].buf), FALSE);

    return (ret);
}

U1 exe_fwrite_space (S8 flist_ind, S8 reg)
{
    U1 ret;
    S4 i;

    if (reg > FILEBUFSIZE)
    {
        return (ERR_FILE_WRITE);
    }

    for (i = reg - 1; i >= 0; i--)
    {
        file[flist_ind].buf[i] = ' ';
    }
    file[flist_ind].buf[reg] = BINUL;

    ret = exe_fwrite (flist_ind, strlen (file[flist_ind].buf), FALSE);

    return (ret);
}


/* misc ------------------------------------------------------------- */

U1 exe_set_fpos (S8 flist_ind, S8 pos)
{
    strcpy (filename, file[flist_ind].name);

    if (file[flist_ind].status == FILECLOSED)
    {
        return (ERR_FILE_FPOS);
    }

    if (flist_ind < 0 || flist_ind > MAXFILES - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    if (fseek (file[flist_ind].fptr, pos, SEEK_SET) == 0)
    {
        clearerr (file[flist_ind].fptr);        /* clear error flags: eof */
        return (ERR_FILE_OK);
    }
    else
    {
        return (ERR_FILE_FPOS);
    }
}

U1 exe_get_fpos (S8 flist_ind, S8 *reg)
{
    strcpy (filename, file[flist_ind].name);

    if (file[flist_ind].status == FILECLOSED)
    {
        return (ERR_FILE_FPOS);
    }

    if (flist_ind < 0 || flist_ind > MAXFILES - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    *reg = (S4) ftell (file[flist_ind].fptr);
    if (reg >= 0)
    {
        clearerr (file[flist_ind].fptr);        /* clear error flags: eof */
        return (ERR_FILE_OK);
    }
    else
    {
        return (ERR_FILE_FPOS);
    }
}

U1 exe_frewind (S8 flist_ind)
{
    U1 ret;

    ret = exe_set_fpos (flist_ind, 0);

    return (ret);
}

U1 exe_fsize (S8 flist_ind, S8 *reg)
{
    S4 curr_pos, end_pos;

    strcpy (filename, file[flist_ind].name);

    if (file[flist_ind].status == FILECLOSED)
    {
        return (ERR_FILE_FPOS);
    }

    if (flist_ind < 0 || flist_ind > MAXFILES - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    curr_pos = ftell (file[flist_ind].fptr);                    /* save current position */

    fseek (file[flist_ind].fptr, (long) NULL, SEEK_END);
    end_pos = ftell (file[flist_ind].fptr);                     /* get filesize */

    fseek (file[flist_ind].fptr, curr_pos, SEEK_SET);           /* restore position */

    clearerr (file[flist_ind].fptr);                            /* clear error flags: eof */

    *reg = (S4) end_pos;

    return (ERR_FILE_OK);
}

U1 exe_fremove (U1 *s_reg)
{
	U1 path[MAXSTRING_VAR + 1];
	
	if (rights.file == FALSE)
	{
		printf ("ERROR: no rights for file access!\n");
		return (ERR_FILE_WRITE);
	}
	
	if (expand_pathname (s_reg, path) != 0)
	{
		return (ERR_FILE_WRITE);
	}
	
	if (check_path_break (path) != 0)
	{
		printf ("ERROR: illegal filename!\n");
	
		return (ERR_FILE_WRITE);
	}
	
    if (remove (path) == 0)
    {
        return (ERR_FILE_OK);
    }
    else
    {
        return (ERR_FILE_WRITE);
    }
}

U1 exe_frename (U1 *oldname, U1 *newname)
{
	U1 oldnamepath[MAXSTRING_VAR + 1], newnamepath[MAXSTRING_VAR + 1];
	
	if (rights.file == FALSE)
	{
		printf ("ERROR: no rights for file access!\n");
		return (ERR_FILE_WRITE);
	}
	
	if (expand_pathname (oldname, oldnamepath) != 0)
	{
		return (ERR_FILE_WRITE);
	}
	
	if (expand_pathname (newname, newnamepath) != 0)
	{
		return (ERR_FILE_WRITE);
	}
	
	if (check_path_break (oldnamepath) != 0)
	{
		printf ("ERROR: illegal filename!\n");
	
		return (ERR_FILE_WRITE);
	}
	
	if (check_path_break (newnamepath) != 0)
	{
		printf ("ERROR: illegal filename!\n");
	
		return (ERR_FILE_WRITE);
	}
	
    if (rename (oldnamepath, newnamepath) == 0)
    {
        return (ERR_FILE_OK);
    }
    else
    {
        return (ERR_FILE_WRITE);
    }
}

U1 exe_makedir (U1 *name)
{
	U1 path[MAXSTRING_VAR + 1];
	
	if (rights.file == FALSE)
	{
		printf ("ERROR: no rights for file access!\n");
		return (ERR_FILE_WRITE);
	}
	
	if (expand_pathname (name, path) != 0)
	{
		return (ERR_FILE_WRITE);
	}
	
	if (check_path_break (path) != 0)
	{
		printf ("ERROR: illegal filename!\n");
	
		return (ERR_FILE_WRITE);
	}
	
#if OS_LINUX
    if (mkdir (path, S_IRWXU) == 0)
    {
        return (ERR_FILE_OK);
    }
    else
    {
        return (ERR_FILE_WRITE);
    }
#endif

#if OS_WINDOWS
    if (CreateDirectory (path, NULL) != 0)
    {
        return (ERR_FILE_OK);
    }
    else
    {
        return (ERR_FILE_WRITE);
    }
#endif
}

/* exe_dobjects () ---------------------------------------- */
/* get number of directory objects: files and directorys    */

/* source taken from: http://stackoverflow.com/questions/5734802/reading-directory-content-in-linux */
/* and modified to fit */

#if OS_LINUX || OS_ANDROID
U1 exe_dobjects (U1 *directory, S8 *objects)
{
	U1 new_fname[MAXSTRING_VAR + 1], convname[MAXSTRING_VAR + 1], path[MAXSTRING_VAR + 1];
    
    DIR *dp;
    struct dirent *files;
	
	S8 dir_obj = 0 ALIGN;
	
    /*structure for storing inode numbers and files in dir
    struct dirent
    {
        ino_t d_ino;
        char d_name[NAME_MAX+1]
    }
    */
	
	if (rights.file == FALSE)
	{
		printf ("ERROR: no rights for file access!\n");
		return (ERR_FILE_OPEN);
	}
	
	strcpy (new_fname, directory);
	
	if (expand_pathname (new_fname, path) != 0)
	{
		return (ERR_FILE_OPEN);
	}
	
	strcpy (new_fname, path);
    
	if (! conv_fname (convname, new_fname))
    {
        return (ERR_FILE_OPEN);
    }
	
	if (check_path_break (convname) != 0)
	{
		printf ("ERROR: illegal filename!\n");
		
		return (ERR_FILE_OPEN);
	}
	
	strcpy (path, convname);
	
    if((dp=opendir(path))==NULL)
        return (ERR_FILE_OPEN);
	
    U1 newp[1000];
    struct stat buf;
    while((files=readdir(dp))!=NULL)
    {
              if(!strcmp(files->d_name,".") || !strcmp(files->d_name,".."))
                continue;

        strcpy(newp,path);
        strcat(newp,"/");
        strcat(newp,files->d_name); 
            // printf("%s\n",newp);
			dir_obj++;

            //stat function return a structure of information about the file    
        if(stat(newp,&buf)==-1)
        perror("stat");
        if(S_ISDIR(buf.st_mode))// if directory, then add a "/" to current path
        {
            strcat (newp, "/");
			
            // read(path);
            // strcpy(path,convname);
        }
        // printf ("%s\n", path);
        // printf ("%s\n", newp);
		strcpy(path,convname);
    }
    *objects = dir_obj;
    return (ERR_FILE_OK);
}
#endif

#if OS_WINDOWS
U1 exe_dobjects (U1 *directory, S8 *objects)
{
	printf ("dobjects ERROR: function not implemented on Windows!\n"
	return (ERR_FILE_OPEN);
}
#endif

/* -------------------------------------------------------- */

/* exe_dnames () ------------------------------------------ */
/* get directory names: file names and directory names      */

#if OS_LINUX || OS_ANDROID
U1 exe_dnames (U1 *directory, struct varlist *varlist, S8 sreg)
{
	U1 new_fname[MAXSTRING_VAR + 1], convname[MAXSTRING_VAR + 1], path[MAXSTRING_VAR + 1];
	U1 rel_fname[MAXSTRING_VAR + 1];
    
    DIR *dp;
    struct dirent *files;
	
	S8 index = 0 ALIGN;
	S8 path_len ALIGN;
	S8 len ALIGN;
	S8 i ALIGN;
	S8 j ALIGN;
	S8 max ALIGN;
	
    /*structure for storing inode numbers and files in dir
    struct dirent
    {
        ino_t d_ino;
        char d_name[NAME_MAX+1]
    }
    */
	
	if (rights.file == FALSE)
	{
		printf ("ERROR: no rights for file access!\n");
		return (ERR_FILE_OPEN);
	}
	
	path_len = strlen (directory);
	
	strcpy (new_fname, directory);
	
	if (expand_pathname (new_fname, path) != 0)
	{
		return (ERR_FILE_OPEN);
	}
	
	strcpy (new_fname, path);
    
	if (! conv_fname (convname, new_fname))
    {
        return (ERR_FILE_OPEN);
    }
	
	if (check_path_break (convname) != 0)
	{
		printf ("ERROR: illegal filename!\n");
		
		return (ERR_FILE_OPEN);
	}
	
	strcpy (path, convname);
	
    if((dp=opendir(path))==NULL)
        return (ERR_FILE_OPEN);
	
    U1 newp[1000];
    struct stat buf;
    while((files=readdir(dp))!=NULL)
    {
              if(!strcmp(files->d_name,".") || !strcmp(files->d_name,".."))
                continue;

        strcpy(newp,path);
        strcat(newp,"/");
        strcat(newp,files->d_name); 
            // printf("%s\n",newp);
			
			// U1 exe_let_2array_string (struct varlist *varlist, U1 *s_reg1, S4 reg2, S8 reg3)
			
            //stat function return a structure of information about the file    
        if(stat(newp,&buf)==-1)
        perror("stat");
        if(S_ISDIR(buf.st_mode))// if directory, then add a "/" to current path
        {
            strcat (newp, "/");
        }
        
        len = strlen (newp);
        
        if (exe_let_2array_string (varlist, newp, sreg, index) != TRUE)
		{
			return (ERR_FILE_READ);
		}
		index++;
    }
    return (ERR_FILE_OK);
}
#endif

#if OS_WINDOWS
U1 exe_dnames (U1 *directory, struct varlist *varlist, S8 sreg)
{
	printf ("dnames ERROR: function not implemented on Windows!\n"
	return (ERR_FILE_OPEN);
}
#endif