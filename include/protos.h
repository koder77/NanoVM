/****************************************************************************
 *
 * Filename: protos.h
 *
 * Author:   Stefan Pietzonke
 * Project:  nano, virtual machine
 *
 * Purpose:  function prototypes
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

/* jumpl.c */

void init_jumplist (void);
U1 jumplist_gonext (void);
S4 getjumpind (U1 *str);

#if DEBUG
    void debug_jumplist (void);
#endif


/* file.c */

char *fgets_uni (char *str, int len, FILE *fptr);


/* loadp.c */

U1 exe_load_prog (U1 *file);
U1 conv_include_name (U1 *str, U1 *file, S2 type);


/* opcode.c */

void get_opcode (U1 *str);
U1 set_opcode (void);


/* regname.c */

void init_regname_l (void);
void init_regname_d (void);
void init_regname_all (void);


/* saveeend.c */

S2 conv_int (S2 num);
S4 conv_lint (S4 num);
F8 conv_double (F8 num);
void write_string (U1 *str, U1 *wstr, S4 pos);
void write_char (U1 *str, U1 wstr, S4 pos);
void write_int (U1 *str, S2 n, S4 pos);
void write_lint (U1 *str, S4 n, S4 pos);
void write_qint (U1 *str, S8 n, S4 pos);
void write_double (U1 *str, F8 n, S4 pos);


/* saveo.c */

U1 exe_save_object (U1 *file, S4 obj_size, U1 save_debug);


/* stringf.c */

S2 checksemicol (U1 *str, S2 start, S2 end);
void convtabs (U1 *str);
void skipspaces (U1 *str, U1 *retstr);
void partstr (U1 *str, U1 *retstr, S2 start, S2 end);
void getstr (U1 *str, U1 *retstr, S2 maxlen, S2 start, S2 end);
S2 searchpart (U1 *str, S2 start, S2 end);
S2 searchstr (U1 *str, U1 *srchstr, S2 start, S2 end, U1 case_sens);


/* var.c */

S4 getvarind (U1 *str);
U1 checkstring (U1 *str);
U1 getvarstr (U1 *argstr, S2 pos_start);
U1 make_val (U1 type, struct varlist *varlist, U1 pprivate);
U1 make_string (void);
U1 get_arg (U1 *str);


/* exe_arr.c */

U1 exe_let_array2_int (struct varlist *varlist, S4 reg1, S8 reg2, S8 *reg3);
U1 exe_let_array2_lint (struct varlist *varlist, S4 reg1, S8 reg2, S8 *reg3);
U1 exe_let_array2_qint (struct varlist *varlist, S4 reg1, S8 reg2, S8 *reg3);
U1 exe_let_array2_double (struct varlist *varlist, S4 reg1, S8 reg2, F8 *reg3);
U1 exe_let_array2_byte (struct varlist *varlist, S4 reg1, S8 reg2, S8 *reg3);
U1 exe_let_array2_string (struct varlist *varlist, S4 reg1, S8 reg2, U1 *s_reg3);
U1 exe_let_2array_int (struct varlist *varlist, S8 reg1, S4 reg2, S8 reg3);
U1 exe_let_2array_lint (struct varlist *varlist, S8 reg1, S4 reg2, S8 reg3);
U1 exe_let_2array_qint (struct varlist *varlist, S8 reg1, S4 reg2, S8 reg3);
U1 exe_let_2array_double (struct varlist *varlist, F8 reg1, S4 reg2, S8 reg3);
U1 exe_let_2array_byte (struct varlist *varlist, S8 reg1, S4 reg2, S8 reg3);
U1 exe_let_2array_string (struct varlist *varlist, U1 *s_reg1, S4 reg2, S8 reg3);


/* exe_file.c */

U1 conv_fname (U1 *new_fname, U1 *fname);
S2 get_ferr (S2 err);

U1 exe_fopen (S8 *flist_ind, U1 *fname, U1 *flag);
U1 exe_fclose (S8 flist_ind);
U1 exe_fread (S4 flist_ind, S4 len);
U1 exe_fwrite (S4 flist_ind, S4 len, U1 binary_write);

U1 exe_fread_byte (S8 flist_ind, S8 *reg);
U1 exe_fread_array_byte (S8 flist_ind, struct varlist *varlist, S4 b_var, S8 len);
U1 exe_fread_int (S8 flist_ind, S8 *reg);
U1 exe_fread_lint (S8 flist_ind, S8 *reg);
U1 exe_fread_qint (S8 flist_ind, S8 *reg);
U1 exe_fread_double (S8 flist_ind, F8 *reg);
U1 exe_fread_string (S8 flist_ind, U1 *s_reg, S8 len);
U1 exe_fread_line_string (S8 flist_ind, U1 *s_reg);

U1 exe_fwrite_byte (S8 flist_ind, S8 reg);
U1 exe_fwrite_array_byte (S8 flist_ind, struct varlist *varlist, S4 b_var, S8 len);
U1 exe_fwrite_int (S8 flist_ind, S8 reg);
U1 exe_fwrite_lint (S8 flist_ind, S8 reg);
U1 exe_fwrite_double (S8 flist_ind, F8 reg);
U1 exe_fwrite_qint (S8 flist_ind, S8 reg);
U1 exe_fwrite_string (S8 flist_ind, U1 *s_reg);
U1 exe_fwrite_str_lint (S8 flist_ind, S8 reg);
U1 exe_fwrite_str_double (S8 flist_ind, F8 reg);
U1 exe_fwrite_newline (S8 flist_ind, S8 reg);
U1 exe_fwrite_space (S8 flist_ind, S8 reg);
U1 exe_set_fpos (S8 flist_ind, S8 pos);
U1 exe_get_fpos (S8 flist_ind, S8 *reg);

U1 exe_frewind (S8 flist_ind);
U1 exe_fsize (S8 flist_ind, S8 *reg);
U1 exe_fremove (U1 *s_reg);
U1 exe_frename (U1 *oldname, U1 *newname);

U1 exe_dobjects (U1 *directory, S8 *objects);
U1 exe_dnames (U1 *directory, struct varlist *varlist, S8 sreg);

/* exe_socket.c */

void init_sockets (void);

#if OS_WINDOWS
    S2 init_winsock (void);
#endif

S2 exe_close_socket (S2 sock);
U1 exe_open_server (S8 *slist_ind, U1 *hostname, S8 port, S4 threadnum);
U1 exe_open_accept_server (S8 slist_ind, S8 *new_slist_ind);
U1 exe_open_client (S8 *slist_ind, U1 *hostname, S8 port, S4 threadnum);
U1 exe_close_server (S8 slist_ind);
U1 exe_close_accept_server (S8 slist_ind);
U1 exe_close_client (S8 slist_ind);
U1 exe_getclientaddr (S8 slist_ind, U1 *s_reg);
U1 exe_gethostname (U1 *s_reg);
U1 exe_gethostbyname (U1 *s_reg1, U1 *s_reg2);
U1 exe_gethostbyaddr (U1 *s_reg1, U1 *s_reg2);
//S8 htonq (S8 hostd);
//S8 ntohq (S8 netd);
F8 htond (F8 hostd);
F8 ntohd (F8 netd);

U1 exe_sread (S4 slist_ind, S4 len);
U1 exe_swrite (S4 slist_ind, S4 len);

U1 exe_sread_byte (S8 slist_ind, S8 *reg);
U1 exe_sread_array_byte (S8 slist_ind, struct varlist *varlist, S4 b_var, S8 len);
U1 exe_sread_int (S8 slist_ind, S8 *reg);
U1 exe_sread_lint (S8 slist_ind, S8 *reg);
U1 exe_sread_qint (S8 slist_ind, S8 *reg);
U1 exe_sread_double (S8 slist_ind, F8 *reg);
U1 exe_sread_string (S8 slist_ind, U1 *s_reg, S8 len);
U1 exe_sread_line_string (S8 slist_ind, U1 *s_reg);

U1 exe_swrite_byte (S8 slist_ind, S8 reg);
U1 exe_swrite_array_byte (S8 slist_ind, struct varlist *varlist, S4 b_var, S8 len);
U1 exe_swrite_int (S8 slist_ind, S8 reg);
U1 exe_swrite_lint (S8 slist_ind, S8 reg);
U1 exe_swrite_qint (S8 slist_ind, S8 reg);
U1 exe_swrite_double (S8 slist_ind, F8 reg);
U1 exe_swrite_string (S8 slist_ind, U1 *s_reg);
U1 exe_swrite_str_lint (S8 slist_ind, S8 reg);
U1 exe_swrite_str_double (S8 slist_ind, F8 reg);
U1 exe_swrite_newline (S8 slist_ind, S8 reg);
U1 exe_swrite_space (S8 slist_ind, S8 reg);




/* exe_main.c */

void exe_break (void);
void *exe_elist (S4 start);

#if OS_AROS
    S2 exe_elist_trampoline (S4 start);
#endif

/* openend.c */

S2 conv_int (S2 num);
S4 conv_lint (S4 num);
S8 conv_qint (S8 num);
F8 conv_double (F8 num);

void read_string (U1 *str, U1 *retstr, S4 pos, S2 len);
S2 read_stringbn (U1 *str, U1 *retstr, S4 pos, S4 size);
void read_char (U1 *str, U1 *retstr, S4 pos);
S2 read_int (U1 *str, S4 pos);
S4 read_lint (U1 *str, S4 pos);
S8 read_qint (U1 *str, S4 pos);
F8 read_double (U1 *str, S4 pos);


/* openo.c */

S2 exe_open_object (U1 *file);


/* verifyo.c */

U1 exe_verify_object ();


/* ccl.c */

U1 cclist_gonext (void);


/* checkd.c */

U1 checkdigit (U1 *str);


/* exe_time.c */

void exe_time (void);


/* mem.c */

int init_varlist (struct varlist *varlist, S4 init_maxvarlist);
void init_plist (S4 init_maxplist);

#if DEBUG
    void debug_varlist (struct varlist *varlist, S4 maxind);
    void debug_exelist (void);
#endif

U1 init_var (struct varlist *varlist, struct varlist_state *varlist_state);
U1 init_int_var (struct varlist *varlist, struct varlist_state *varlist_state);
U1 memblock_gonext (void);
void *get_memblock (S2 msize);
U1 exe_alloc_var (struct varlist *varlist, S4 ind, S4 type);
S4 **alloc_array_lint (size_t n_rows, size_t n_columns);
void dealloc_array_lint (S4** array, size_t n_rows);
void dealloc_memblocks (void);
U1 exe_dealloc_var (struct varlist *varlist, S4 ind);
void exe_dealloc_varlist (struct varlist *varlist);
void exe_dealloc_pvarlist (struct varlist *varlist);
void exe_dealloc_plist (void);
void free_memory (void);
void exe_shutdown (S2 value);
U1 init_files (void);

/* msg.c */

void printmsg (S2 msgno, U1 *str);
void printerr (S2 errnum, S4 pos, U1 status, U1 *str);
void printcompilermsg (void);


/* vm.c */

U1 init_vmfile (void);
S8 get_vmblock (S8 msize);
U1 vm_alloc_pages (struct varlist *varlist, S4 vind, S8 vmcachesize, U1 type);
U1 save_page (struct varlist *varlist, S4 vind, S8 page);
U1 load_page (struct varlist *varlist, S4 vind, S8 page, S8 index);
void *get_vmvar (struct varlist *varlist, S4 vind, S8 ind, U1 access);


/* mt19937ar.c */
void init_genrand(unsigned long s);
long genrand_int31(void);
double genrand_real1(void);


/* process.c */

U1 exe_run_process (U1 *name, S8 *process);
U1 exe_run_shell (U1 *command, S8 *retval);
U1 exe_wait_process (S8 process, S8 *retval);


/* exe_stack.c */

S2 get_sterr (S2 err);
U1 alloc_stack (void);
U1 check_stack (S4 threadnum);

U1 exe_stpush_l (S4 threadnum, S8 reg);
U1 exe_stpush_d (S4 threadnum, F8 reg);
U1 exe_stpush_s (S4 threadnum, U1 *s_reg);

U1 exe_stpull_l (S4 threadnum, S8 *reg);
U1 exe_stpull_d (S4 threadnum, F8 *reg);
U1 exe_stpull_s (S4 threadnum, U1 *s_reg);

U1 exe_show_stack (S4 threadnum);

U1 exe_stgettype (S4 threadnum, S8 *reg);
U1 exe_stelements (S4 threadnum, S8 *reg);

U1 exe_stpush_sync (S4 threadnum);
U1 exe_stpull_sync (S4 threadnum);


/* main.c (assembler) */

U1 set0 (S4 comm);
U1 set1 (S4 comm, S4 vind1);
U1 set2 (S4 comm, S4 vind1, S4 vind2);
U1 set3 (S4 comm, S4 vind1, S4 vind2, S4 vind3);

/* pthreads */

int init_pthreads (void);
S4 get_new_thread (void);
void set_thread_running (S4 threadnum);
void set_thread_stopped (S4 threadnum);
U1 get_thread_state (S4 threadnum);
void join_threads (S4 threadnum);
void wait_thread_running (S4 threadnum);
S8 create_new_thread (S4 startpos);
S4 get_new_thread_startpos (void);


/* exe_vector_arr.c */

U1 exe_vadd_l (struct varlist *varlist, struct varlist *global, S4 reg1, S8 reg2, S4 reg3, S4 threadnum);
U1 exe_vadd_d (struct varlist *varlist, struct varlist *global, S4 reg1, F8 reg2, S4 reg3, S4 threadnum);
U1 exe_vsub_l (struct varlist *varlist, struct varlist *global, S4 reg1, S8 reg2, S4 reg3, S4 threadnum);
U1 exe_vsub_d (struct varlist *varlist, struct varlist *global, S4 reg1, F8 reg2, S4 reg3, S4 threadnum);
U1 exe_vmul_l (struct varlist *varlist, struct varlist *global, S4 reg1, S8 reg2, S4 reg3, S4 threadnum);
U1 exe_vmul_d (struct varlist *varlist, struct varlist *global, S4 reg1, F8 reg2, S4 reg3, S4 threadnum);
U1 exe_vdiv_l (struct varlist *varlist, struct varlist *global, S4 reg1, S8 reg2, S4 reg3, S4 threadnum);
U1 exe_vdiv_d (struct varlist *varlist, struct varlist *global, S4 reg1, F8 reg2, S4 reg3, S4 threadnum);

U1 exe_vadd2_l (struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum);
U1 exe_vadd2_d (struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum);
U1 exe_vsub2_l (struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum);
U1 exe_vsub2_d (struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum);
U1 exe_vmul2_l (struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum);
U1 exe_vmul2_d (struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum);
U1 exe_vdiv2_l (struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum);
U1 exe_vdiv2_d (struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum);


/* exe_passw_input */

void get_password (U1 *buffer, S8 size);
