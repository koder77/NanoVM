/* assemb/ccl.c */
U1 cclist_gonext(void);
/* assemb/jumpl.c */
void init_jumplist(void);
U1 jumplist_gonext(void);
S4 getjumpind(U1 *str);
void debug_jumplist(void);
/* assemb/loadp.c */
U1 exe_load_prog(U1 *file);
void conv_programpath(U1 *outputstr);
U1 conv_include_name(U1 *str, U1 *file, S2 type);
/* assemb/main.c */
U1 set0(S4 comm);
U1 set1(S4 comm, S4 vind1);
U1 set2(S4 comm, S4 vind1, S4 vind2);
U1 set3(S4 comm, S4 vind1, S4 vind2, S4 vind3);
void set_pline(S4 linenumber);
U1 exe_assemble(S4 start, S4 end);
void showinfo(void);
NINT main(NINT ac, char *av[]);
/* assemb/opcode.c */
void get_opcode(U1 *str);
U1 set_opcode(void);
/* assemb/regname.c */
void init_regname_l(void);
void init_regname_d(void);
void init_regname_s(void);
void init_regname_all(void);
/* assemb/saveend.c */
void write_string(U1 *str, U1 *wstr, S4 pos);
void write_char(U1 *str, U1 wstr, S4 pos);
void write_int(U1 *str, S2 n, S4 pos);
void write_lint(U1 *str, S4 n, S4 pos);
void write_qint(U1 *str, S8 n, S4 pos);
void write_double(U1 *str, F8 n, S4 pos);
/* assemb/saveo.c */
U1 exe_save_object(U1 *file, S4 obj_size, U1 save_debug);
/* assemb/stringf.c */
S2 checksemicol(U1 *str, S2 start, S2 end);
S2 checkchars(U1 *str);
void convtabs(U1 *str);
void skipspaces(U1 *str, U1 *retstr);
void partstr(U1 *str, U1 *retstr, S2 start, S2 end);
void getstr(U1 *str, U1 *retstr, S2 maxlen, S2 start, S2 end);
S2 searchpart(U1 *str, S2 start, S2 end);
S2 searchstr(U1 *str, U1 *srchstr, S2 start, S2 end, U1 case_sens);
/* assemb/var.c */
S4 getvarind(U1 *str);
U1 checkstring(U1 *str);
U1 getvarstr(U1 *argstr, S2 pos_start);
U1 make_val(U1 type, struct varlist *varlist, U1 fprivate);
U1 make_string(void);
U1 get_arg(U1 *str);
/* lib/checkd.c */
U1 checkdigit(U1 *str);
/* lib/endian.c */
S2 conv_int(S2 num);
S4 conv_lint(S4 num);
S8 conv_qint(S8 num);
F8 conv_double(F8 num);
/* lib/exe_time.c */
void exe_time(void);
/* lib/file.c */
char *fgets_uni(char *str, int len, FILE *fptr);
/* lib/mem.c */
int init_varlist(struct varlist *varlist, S4 init_maxvarlist);
void init_plist(S4 init_maxplist);
void debug_varlist(struct varlist *varlist, S4 maxind);
void debug_exelist(void);
U1 init_var(struct varlist *varlist, struct varlist_state *varlist_state);
U1 init_int_var(struct varlist *varlist, struct varlist_state *varlist_state);
U1 memblock_gonext(void);
void *get_memblock(S2 msize);
U1 exe_alloc_var(struct varlist *varlist, S4 ind, S4 type);
void dealloc_array_lint(S4 **array, size_t n_rows);
S4 **alloc_array_lint(size_t n_rows, size_t n_columns);
void dealloc_memblocks(void);
U1 exe_dealloc_var(struct varlist *varlist, S4 ind);
void exe_dealloc_varlist(struct varlist *varlist);
void exe_dealloc_pvarlist(struct varlist *pvarlist);
void exe_dealloc_plist(void);
void exe_dealloc_includes(void);
U1 init_files(void);
void exe_dealloc_files(void);
void dealloc_stack(void);
void free_memory(void);
void exe_shutdown(S2 value);
/* lib/msg.c */
void printmsg(S2 msgno, U1 *str);
void printerr(S2 errnum, S4 pos, U1 status, U1 *str);
void printcompilermsg(void);
/* lib/vm.c */
U1 init_vmfile(void);
S8 get_vmblock(S8 msize);
U1 vm_alloc_pages(struct varlist *varlist, S4 vind, S8 vmcachesize, U1 type);
void vm_free_pages(struct varlist *varlist, S4 vind);
U1 save_page(struct varlist *varlist, S4 vind, S8 page);
U1 load_page(struct varlist *varlist, S4 vind, S8 page, S8 index);
void *get_vmvar(struct varlist *varlist, S4 vind, S8 ind, U1 access);
/* comp/compiler.c */
U1 compile_set_init_var(S2 arg, S4 *var, U1 type);
U1 compile_set_label(U1 *name);
U1 compile_set_function_name(U1 *name);
U1 compile(void);
/* comp/emit.c */
U1 emit_code(U1 *filename);
/* comp/for.c */
void init_for(void);
S4 get_for_pos(void);
S4 get_act_for(void);
U1 set_for_label(S4 ind);
S4 get_for_lab(S4 ind);
void set_next(S4 ind);
/* comp/if.c */
void init_if(void);
S4 get_if_pos(void);
S4 get_act_if(void);
U1 set_else_jmp(S4 ind);
U1 set_endif_jmp(S4 ind);
U1 set_else_label(S4 ind);
U1 set_endif_label(S4 ind);
void set_endif_finished(S4 ind);
S4 get_else_lab(S4 ind);
S4 get_endif_lab(S4 ind);
S4 get_else_set(S4 ind);
/* comp/opcode.c */
void init_vmreg(void);
S2 get_vmreg_l(void);
S2 get_vmreg_d(void);
S2 get_vmreg_s(void);
void set_vmreg_l(S2 reg, S4 var, U1 fprivate);
void set_vmreg_d(S2 reg, S4 var, U1 fprivate);
void set_vmreg_s(S2 reg, S4 var, U1 fprivate);
void unset_vmreg_l(S4 var);
void unset_vmreg_d(S4 var);
void unset_vmreg_s(S4 var);
S2 get_vmreg_var_l(S4 var);
S2 get_vmreg_var_d(S4 var);
S2 get_vmreg_var_s(S4 var);
S2 get_vmreg_val_l(S4 i);
S2 get_vmreg_val_d(S4 i);
S2 get_vmreg_val_s(S4 i);
U1 cclist_gonext(void);
U1 set0(S4 comm);
U1 set1(S4 comm, S4 vind1);
U1 set2(S4 comm, S4 vind1, S4 vind2);
U1 set3(S4 comm, S4 vind1, S4 vind2, S4 vind3);
/* comp/optimizer.c */
U1 optimize(void);
U1 optimize_2(void);
U1 optimize_remove_double_pull(void);
U1 optimize_stack(void);
U1 optimize_remove_double_opcode(void);
/* comp/parser.c */
S2 scan_token(U1 *buf);
S2 parse_line(U1 *str);
void set_pline(S4 linenumber);
U1 exe_assemble(S4 start, S4 end);
void showinfo(void);
NINT main(NINT ac, char *av[]);
/* comp/regspill.c */
S2 spill_vmreg_l(void);
S2 spill_vmreg_d(void);
S2 spill_vmreg_s(void);
/* comp/stringf.c */
S2 checksemicol(U1 *str, S2 start, S2 end);
void convtabs(U1 *str);
void skipspaces(U1 *str, U1 *retstr);
void partstr(U1 *str, U1 *retstr, S2 start, S2 end);
void getstr(U1 *str, U1 *retstr, S2 maxlen, S2 start, S2 end);
S2 searchpart(U1 *str, S2 start, S2 end);
S2 searchstr(U1 *str, U1 *srchstr, S2 start, S2 end, U1 case_sens);
/* comp/translate.c */
S2 translate_code(void);
/* comp/var_comp.c */
S4 getvarind_comp(U1 *str);
U1 getvarstr_comp(U1 *argstr, S2 pos_start);
/* vm/main.c */
void showinfo(void);
NINT main(NINT ac, char *av[]);
/* vm/openo.c */
S2 exe_open_object(U1 *file);
/* vm/openend.c */
void read_string(U1 *str, U1 *retstr, S4 pos, S2 len);
S2 read_stringbn(U1 *str, U1 *retstr, S4 pos, S4 size);
void read_char(U1 *str, U1 *retstr, S4 pos);
void write_char(U1 *str, U1 wstr, S4 pos);
S2 read_int(U1 *str, S4 pos);
S4 read_lint(U1 *str, S4 pos);
S8 read_qint(U1 *str, S4 pos);
F8 read_double(U1 *str, S4 pos);
/* vm/exe_arr.c */
U1 exe_let_array2_int(struct varlist *varlist, S4 reg1, S8 reg2, S8 *reg3);
U1 exe_let_array2_lint(struct varlist *varlist, S4 reg1, S8 reg2, S8 *reg3);
U1 exe_let_array2_qint(struct varlist *varlist, S4 reg1, S8 reg2, S8 *reg3);
U1 exe_let_array2_double(struct varlist *varlist, S4 reg1, S8 reg2, F8 *reg3);
U1 exe_let_array2_byte(struct varlist *varlist, S4 reg1, S8 reg2, S8 *reg3);
U1 exe_let_array2_string(struct varlist *varlist, S4 reg1, S8 reg2, U1 *s_reg3);
U1 exe_let_2array_int(struct varlist *varlist, S8 reg1, S4 reg2, S8 reg3);
U1 exe_let_2array_lint(struct varlist *varlist, S8 reg1, S4 reg2, S8 reg3);
U1 exe_let_2array_qint(struct varlist *varlist, S8 reg1, S4 reg2, S8 reg3);
U1 exe_let_2array_double(struct varlist *varlist, F8 reg1, S4 reg2, S8 reg3);
U1 exe_let_2array_byte(struct varlist *varlist, S8 reg1, S4 reg2, S8 reg3);
U1 exe_let_2array_string(struct varlist *varlist, U1 *s_reg1, S4 reg2, S8 reg3);
/* vm/exe_file.c */
U1 check_path_break(U1 *path);
U1 conv_fname(U1 *new_fname, U1 *fname);
S2 get_ferr(S2 err);
U1 expand_pathname(U1 *path, U1 *newpath);
U1 exe_fopen(S8 *flist_ind, U1 *fname, U1 *flag);
U1 exe_fclose(S8 flist_ind);
U1 exe_fread(S4 flist_ind, S4 len);
U1 exe_fwrite(S4 flist_ind, S4 len, U1 binary_write);
U1 exe_fread_byte(S8 flist_ind, S8 *reg);
U1 exe_fread_array_byte(S8 flist_ind, struct varlist *varlist, S4 b_var, S8 len);
U1 exe_fread_int(S8 flist_ind, S8 *reg);
U1 exe_fread_lint(S8 flist_ind, S8 *reg);
U1 exe_fread_qint(S8 flist_ind, S8 *reg);
U1 exe_fread_double(S8 flist_ind, F8 *reg);
U1 exe_fread_string(S8 flist_ind, U1 *s_reg, S8 len);
U1 exe_fread_line_string(S8 flist_ind, U1 *s_reg);
U1 exe_fwrite_byte(S8 flist_ind, S8 reg);
U1 exe_fwrite_array_byte(S8 flist_ind, struct varlist *varlist, S4 b_var, S8 len);
U1 exe_fwrite_int(S8 flist_ind, S8 reg);
U1 exe_fwrite_lint(S8 flist_ind, S8 reg);
U1 exe_fwrite_double(S8 flist_ind, F8 reg);
U1 exe_fwrite_qint(S8 flist_ind, S8 reg);
U1 exe_fwrite_string(S8 flist_ind, U1 *s_reg);
U1 exe_fwrite_str_lint(S8 flist_ind, S8 reg);
U1 exe_fwrite_str_double(S8 flist_ind, F8 reg);
U1 exe_fwrite_newline(S8 flist_ind, S8 reg);
U1 exe_fwrite_space(S8 flist_ind, S8 reg);
U1 exe_set_fpos(S8 flist_ind, S8 pos);
U1 exe_get_fpos(S8 flist_ind, S8 *reg);
U1 exe_frewind(S8 flist_ind);
U1 exe_fsize(S8 flist_ind, S8 *reg);
U1 exe_fremove(U1 *s_reg);
U1 exe_frename(U1 *oldname, U1 *newname);
U1 exe_makedir(U1 *name);
U1 exe_dobjects(U1 *directory, S8 *objects);
U1 exe_dnames(U1 *directory, struct varlist *varlist, S8 sreg);
/* vm/exe_main.c */
void exe_break(void);
struct timespec time_diff(struct timespec start, struct timespec end);
void *exe_elist(S4 threadnum);
/* vm/exe_socket.c */
char *get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen);
void init_sockets(void);
S2 exe_close_socket(S2 sock);
U1 exe_open_server(S8 *slist_ind, U1 *hostname, S8 port, S4 threadnum);
U1 exe_open_accept_server(S8 slist_ind, S8 *new_slist_ind);
U1 exe_open_client(S8 *slist_ind, U1 *hostname, S8 port, S4 threadnum);
U1 exe_close_server(S8 slist_ind);
U1 exe_close_accept_server(S8 slist_ind);
U1 exe_close_client(S8 slist_ind);
U1 exe_getclientaddr(S8 slist_ind, U1 *s_reg);
U1 exe_gethostname(U1 *s_reg);
U1 exe_gethostbyname(U1 *s_reg1, U1 *s_reg2);
U1 exe_gethostbyaddr(U1 *s_reg1, U1 *s_reg2);

#if ! __ANDROID__
S8 htonq(S8 num);
S8 ntohq(S8 num);
#endif

F8 htond(F8 hostd);
F8 ntohd(F8 netd);
U1 exe_sread(S4 slist_ind, S4 len);
U1 exe_swrite(S4 slist_ind, S4 len);
U1 exe_sread_byte(S8 slist_ind, S8 *reg);
U1 exe_sread_array_byte(S8 slist_ind, struct varlist *varlist, S4 b_var, S8 len);
U1 exe_sread_int(S8 slist_ind, S8 *reg);
U1 exe_sread_lint(S8 slist_ind, S8 *reg);
U1 exe_sread_qint(S8 slist_ind, S8 *reg);
U1 exe_sread_double(S8 slist_ind, F8 *reg);
U1 exe_sread_string(S8 slist_ind, U1 *s_reg, S8 len);
U1 exe_sread_line_string(S8 slist_ind, U1 *s_reg);
U1 exe_swrite_byte(S8 slist_ind, S8 reg);
U1 exe_swrite_array_byte(S8 slist_ind, struct varlist *varlist, S4 b_var, S8 len);
U1 exe_swrite_int(S8 slist_ind, S8 reg);
U1 exe_swrite_lint(S8 slist_ind, S8 reg);
U1 exe_swrite_qint(S8 slist_ind, S8 reg);
U1 exe_swrite_double(S8 slist_ind, F8 reg);
U1 exe_swrite_string(S8 slist_ind, U1 *s_reg);
U1 exe_swrite_str_lint(S8 slist_ind, S8 reg);
U1 exe_swrite_str_double(S8 slist_ind, F8 reg);
U1 exe_swrite_newline(S8 slist_ind, S8 reg);
U1 exe_swrite_space(S8 slist_ind, S8 reg);
/* vm/stringf.c */
S2 checksemicol(U1 *str, S2 start, S2 end);
void skipspaces(U1 *str, U1 *retstr);
void partstr(U1 *str, U1 *retstr, S2 start, S2 end);
void getstr(U1 *str, U1 *retstr, S2 maxlen, S2 start, S2 end);
S2 searchpart(U1 *str, S2 start, S2 end);
S2 searchstr(U1 *str, U1 *srchstr, S2 start, S2 end, U1 case_sens);
/* vm/mt19937ar.c */
void init_genrand(unsigned long s);
void init_by_array(unsigned long init_key[], int key_length);
unsigned long genrand_int32(void);
long genrand_int31(void);
double genrand_real1(void);
double genrand_real2(void);
double genrand_real3(void);
double genrand_res53(void);
/* vm/exe_process.c */
U1 exe_run_process(U1 *name, S8 *process);
U1 exe_run_shell(U1 *command, S8 *retval);
U1 exe_wait_process(S8 process, S8 *retval);
/* vm/verifyo.c */
U1 exe_verify_object(void);
/* vm/exe_stack.c */
S2 get_sterr(S2 err);
int init_stack(void);
U1 alloc_stack(void);
U1 check_stack(S4 threadnum);
U1 exe_stpush_l(S4 threadnum, S8 reg);
U1 exe_stpush_d(S4 threadnum, F8 reg);
U1 exe_stpush_s(S4 threadnum, U1 *s_reg);
U1 exe_stpull_l(S4 threadnum, S8 *reg);
U1 exe_stpull_d(S4 threadnum, F8 *reg);
U1 exe_stpull_s(S4 threadnum, U1 *s_reg);
U1 exe_show_stack(S4 threadnum);
U1 exe_stgettype(S4 threadnum, S8 *reg);
U1 exe_stelements(S4 threadnum, S8 *reg);
U1 exe_stpush_sync(S4 threadnum);
U1 exe_stpull_sync(S4 threadnum);
/* vm/rinzler.c */
U1 check_code(S4 threadnum, S4 maxcclist, S4 **pcclist);
U1 rinzler(void);
/* vm/hash32.c */
U1 exe_hash32_array_byte(struct varlist *varlist, S4 b_var, S4 len, S4 *reg);
/* vm/pthread.c */
int init_pthreads(void);
S4 get_new_thread(void);
void set_thread_running(S4 threadnum);
void set_thread_stopped(S4 threadnum);
U1 get_thread_state(S4 threadnum);
void join_threads(S4 threadnum);
void wait_thread_running(S4 threadnum);
S8 create_new_thread(S4 startpos);
S4 get_new_thread_startpos(void);
/* vm/snprintf.c */
/* vm/dynamic_array.c */
U1 exe_let_dyn_array2_double(struct varlist *varlist, S4 reg1, S8 reg2, F8 *reg3);
U1 exe_let_dyn_array2_string(struct varlist *varlist, S4 reg1, S8 reg2, U1 *s_reg3);
U1 exe_let_2_dyn_array_qint(struct varlist *varlist, S8 reg1, S4 reg2, S8 reg3);
U1 exe_let_2_dyn_array_double(struct varlist *varlist, S8 reg1, S4 reg2, S8 reg3);
U1 exe_let_2_dyn_array_string(struct varlist *varlist, U1 *s_reg1, S4 reg2, S8 reg3);
U1 exe_dyngettype(struct varlist *varlist, S4 reg1, S8 reg2, S8 *reg3);
/* vm/strtoll.c */
long long int strtoll(const char *nptr, char **endptr, int base);
/* vm/exe_vector_arr.c */
U1 exe_vadd_l(struct varlist *varlist, struct varlist *global, S4 reg1, S8 reg2, S4 reg3, S4 threadnum);
U1 exe_vadd_d(struct varlist *varlist, struct varlist *global, S4 reg1, F8 reg2, S4 reg3, S4 threadnum);
U1 exe_vsub_l(struct varlist *varlist, struct varlist *global, S4 reg1, S8 reg2, S4 reg3, S4 threadnum);
U1 exe_vsub_d(struct varlist *varlist, struct varlist *global, S4 reg1, F8 reg2, S4 reg3, S4 threadnum);
U1 exe_vmul_l(struct varlist *varlist, struct varlist *global, S4 reg1, S8 reg2, S4 reg3, S4 threadnum);
U1 exe_vmul_d(struct varlist *varlist, struct varlist *global, S4 reg1, F8 reg2, S4 reg3, S4 threadnum);
U1 exe_vdiv_l(struct varlist *varlist, struct varlist *global, S4 reg1, S8 reg2, S4 reg3, S4 threadnum);
U1 exe_vdiv_d(struct varlist *varlist, struct varlist *global, S4 reg1, F8 reg2, S4 reg3, S4 threadnum);
U1 exe_vadd2_l(struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum);
U1 exe_vadd2_d(struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum);
U1 exe_vsub2_l(struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum);
U1 exe_vsub2_d(struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum);
U1 exe_vmul2_l(struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum);
U1 exe_vmul2_d(struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum);
U1 exe_vdiv2_l(struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum);
U1 exe_vdiv2_d(struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum);
/* vm/exe_passw_input.c */
/* vm/hyperspace_server.c */
S4 getvarind(U1 *str);
U1 hs_read(S4 slist_ind, S4 len);
S2 hs_sread_byte(S8 slist_ind, S8 *reg);
S2 hs_sread_qint(S8 slist_ind, S8 *reg);
S2 hs_sread_double(S8 slist_ind, F8 *reg);
S2 hs_swrite_byte(S8 slist_ind, S8 reg);
S2 hs_swrite_qint(S8 slist_ind, S8 reg);
S2 hs_swrite_double(S8 slist_ind, F8 reg);
S2 hyperspace_server(S8 socket);
/* vm/hyperspace_client.c */
S2 hs_cread_byte(S8 slist_ind, S8 *reg);
S2 hs_cread_qint(S8 slist_ind, S8 *reg);
S2 hs_cread_double(S8 slist_ind, F8 *reg);
S2 hs_cwrite_byte(S8 slist_ind, S8 reg);
S2 hs_cwrite_qint(S8 slist_ind, S8 reg);
S2 hs_cwrite_double(S8 slist_ind, F8 reg);
S2 hs_send(S8 socket, S4 varind, S8 index, U1 access);
S2 hs_load(struct varlist *varlist, S4 reg1, S8 reg2);
S2 hs_save(struct varlist *varlist, S4 reg1, S8 reg2);
/* vm/jit.cpp */
/* vm/rights.c */
U1 load_rights(U1 *filename);
/* vm/dlload.c */
U1 expand_dll_pathname(U1 *path, U1 *newpath);
void init_dlls(void);
S4 get_dll_handle(void);
S4 get_dll_func_handle(S8 index);
U1 exe_lopen(S8 *llist_ind, U1 *lname);
U1 exe_lclose(S8 llist_ind);
U1 exe_lfunc(S8 llist_ind, S8 *flist_ind, U1 *fname);
U1 exe_lcall(S8 llist_ind, S8 flist_ind, U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize);
S4 close_dll_handles(void);
