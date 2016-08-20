/****************************************************************************
*
* Filename: global_d.h
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  global definitions
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

#if OS_WINDOWS
    #define WINVER              0x0501      /* = Windows XP, for Windows socket API */
#endif

#if OS_AROS
    #include <proto/exec.h>
    #include <proto/thread.h>
    #include <proto/dos.h>
#else
    #include <pthread.h>            /* for POSIX threads */
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#if OS_WINDOWS
    #include <malloc.h>
#endif

#if OS_LINUX || OS_WINDOWS_CYGWIN || OS_ANDROID
    #include <unistd.h>         /* time functions */
    #include <sys/stat.h>       /* makedir */
	#include <dirent.h>
#endif

#include <math.h>
#include <signal.h>
#include <time.h>



/* for socket->servinfo: */
#if JIT_CPP == 0
#if OS_WINDOWS
    #include <winsock2.h>
    #include <ws2tcpip.h>

    #define CLOSEWINSOCK();             WSACleanup();
    #define SHUT_RDWR                   0x02
#endif
#endif

#if OS_AROS
	#define SocketBase FindTask(NULL)->tc_UserData
	#define __BSDSOCKET_NOLIBBASE__
	#include <proto/bsdsocket.h>
	#include <bsdsocket/socketbasetags.h>
	#define KERNEL
	#include <netdb.h>
	#undef KERNEL
#endif

#if OS_AMIGA || OS_AROS || OS_LINUX || OS_WINDOWS_CYGWIN || OS_ANDROID
    #include <errno.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>

    #if OS_WINDOWS_CYGWIN
        #include <cygwin/socket.h>
    #endif

    #define CLOSEWINSOCK();
#endif

#if OS_AMIGA
    typedef int socklen_t;          /* it's not declared in the includes */
#endif

/* DLL load */
#if OS_WINDOWS || OS_WINDOWS_CYGWIN
#include <direct.h>
#include <windows.h>
#else
#if OS_LINUX
#include <sys/types.h>
#include <dlfcn.h>
#endif
#endif


/* POSIX threads definitions */
#define MAXPTHREADS     256

#define PTHREAD_UNUSED  0
#define PTHREAD_STOPPED 1
#define PTHREAD_RUNNING 2
#define PTHREAD_REQUEST 3



/* JIT COMPILER definitions */

#define JIT_ASSEMBLER 0
#define JIT_COMPILER 1
 

/* virtual memory defs ------------------------------------ */
#define PAGE_MODIFIED	1
#define PAGE_READ		0

/* for get_vmvar, /lib/vm.c */
/* access flags */
#define VM_READ			0
#define VM_WRITE		1

/* -------------------------------------------------------- */
 
 
#define PRIVATE_VAR     1
#define NORMAL_VAR      0

/* type definitions */

#define BINUL           '\0'
#define WARN            5

#define TRUE            1
#define FALSE           0


/* set alignment for Android ARM */
#if M_ARM
	#define ALIGN		__attribute__ ((aligned(8)))
#else
	#define ALIGN
#endif

typedef unsigned char           U1;		/* UBYTE   */
typedef int16_t                 S2;     /* INT     */
typedef uint16_t                U2;     /* UINT    */
typedef int32_t                 S4;     /* LONGINT */


#if OS_ANDROID
	#undef S8
	typedef int64_t				S8;
#else
#if OS_WINDOWS || OS_WINDOWS_CYGWIN
    typedef long long int       S8;     /* LONG LONG INT = 64 bit */
#else    
    typedef int64_t             S8;
#endif
#endif


typedef double                  F8;     /* DOUBLE  */
typedef int                     NINT;


#define NANO_TIMER_TICKS        50


#define MAXLINES            10000    /* maximum line number 32767 */
#define MAXLINELEN          256     /* max line length */
#define MAXVAR              2048    /* max number of variables */
                                    /* areas count as one, (should be enough) */
#define MAXDIMENS           32      /* max dimensions for array */
#define MAXVARNAME          64      /* max variable-name length,  64 chars */
#define MAXJUMPNAME         64      /* max label-name length,     64 chars */
#define MAXCOMMARG          256     /* max arguments of command (3) */
#define BCELIST             4       /* pos in bclist, for plist linenumber (exelist) */
#define INCLUDELIST         5       /* pos in bclist, for include file-index (exelist) */
#define PLINELIST           6       /* pos in bclist, for inline assembler (translate.c) */
#define MAXCCLEN            32000   /* max cclist len (bytecode) */
#define MAXCCOMMARG         7       /* bytecode list arguments */
#define MAXJUMPLEN          10000   /* jumps can be done, (goto ...) */
#define MAXJUMPRECLEN       256     /* gosub calls (dynamic) */
#define MAXSUBARG           32      /* number of subroutine arguments */
#define MAXOPCODE           389     /* number of opcodes, 0 - x */
#define OBJBUFSIZE          10      /* object bufsize (KB), assembler only! */
#define MAXSHELLARG         10      /* shell arguments, vm */

#define MAXVMPAGES			10		/* pages for virtual memory */


#define INT_SB              "int"
#define LINT_SB             "lint"
#define QINT_SB             "qint"      /* 64 bit integer */
#define STRING_SB           "string"
#define DOUBLE_SB           "double"

#define UNDEF_VAR           0
#define INT_VAR             1       /* variable types */
#define LINT_VAR            2
#define QINT_VAR            3
#define DOUBLE_VAR          4
#define STRING_VAR          5
#define BYTE_VAR            6
#define DYNAMIC_VAR         7

/* assembler, array vars */
#define INT_A_VAR           8
#define LINT_A_VAR          9
#define QINT_A_VAR          10
#define DOUBLE_A_VAR        11
#define STRING_A_VAR        12
#define BYTE_A_VAR          13
#define DYNAMIC_A_VAR       14
#define L_REG               15
#define D_REG               16
#define S_REG               17
#define LABEL               18
#define LABEL_SET           19
#define VAR                 20
#define CONST_VAR           21


/* THREAD PUSH (thpush_sync / thpull_sync) */
#define THREAD_SYNC         15


/* file definitions */

#define MAXFILES            256     /* max number of files, for in-output */
#define MAXSOCKETS          256     /* max number of sockets */
#define SOCKADDRESSLEN      16      /* max dotted ip len */
#define SOCKBUFSIZE         10240    /* socket data buffer len */
#define FILEBUFSIZE         4096    /* file data buffer len */

#define FILEREAD            0       /* type */
#define FILEWRITE           1
#define FILEAPPEND          2
#define FILEREADWRITE       3

#define SOCKSERVER          0
#define SOCKCLIENT          1

#define FILEOPEN            0       /* status */
#define FILECLOSED          1


#define ERR_FILE_OK         0       /* error codes for '_file' */
#define ERR_FILE_OPEN       1
#define ERR_FILE_CLOSE      2
#define ERR_FILE_READ       3
#define ERR_FILE_WRITE      4
#define ERR_FILE_NUMBER     5
#define ERR_FILE_EOF        6
#define ERR_FILE_FPOS       7


#define ERR_STACK_OK            0       /* stack error codes */
#define ERR_STACK_MEMORY        1
#define ERR_STACK_OVERFLOW      2
#define ERR_STACK_OVERFLOW_VAR  3
#define ERR_STACK_WRONG_TYPE    4


#define MAXSTRING_VAR       4096           /* max string-variable length */
/* 4096 chars */

#define INT_TMP_1           0              /* varlist-indexes for internal */
#define INT_TMP_2           1              /* use only! */
#define LINT_TMP_1          2              /* for build in functions: str2val...*/
#define STRING_TMP_1        3
#define STRING_TMP_2        4
#define DOUBLE_TMP_1        5
#define DOUBLE_TMP_2        6
#define LINT_TMP_2          7              /* array-index (for debugging)*/
#define LINT_TMP_2_SB       "_arrayind"
#define BREAK               8              /* break handling */
#define BREAK_SB            "_break"
#define TIMER               9              /* timer, for ton and toff */
#define TIMER_SB            "_timer"
#define TIMER_TICKS         10             /* clocks per sec */
#define TIMER_TICKS_SB      "_timertck"
#define INTSIZE             11
#define INTSIZE_SB          "_intsize"
#define LINTSIZE            12
#define LINTSIZE_SB         "_lintsize"
#define DOUBLESIZE          13
#define DOUBLESIZE_SB       "_doublesize"
#define MACHINE             14
#define MACHINE_SB          "_machine"
#define ERR_ALLOC           15
#define ERR_ALLOC_SB        "_alloc"
#define ERR_FILE            16
#define ERR_FILE_SB         "_err_file"
#define TIME_HOUR           17
#define TIME_HOUR_SB        "_hour"
#define TIME_MIN            18
#define TIME_MIN_SB         "_min"
#define TIME_SEC            19
#define TIME_SEC_SB         "_sec"
#define DATE_YEAR           20
#define DATE_YEAR_SB        "_year"
#define DATE_MONTH          21
#define DATE_MONTH_SB       "_month"
#define DATE_DAY            22
#define DATE_DAY_SB         "_day"
#define DATE_WDAY           23
#define DATE_WDAY_SB        "_wday"
#define DATE_YDAY           24
#define DATE_YDAY_SB        "_yday"
#define VERSION             25
#define VERSION_SB          "_version"
#define VMREGS              26
#define VMREGS_SB           "_vmregs"

#define LANGUAGE            27
#define LANGUAGE_SB         "_language"
#define LANGUAGE_LEN        30

#define FILE_NEWLINE        28
#define FILE_NEWLINE_SB     "_fnewline"
#define FILE_NEWLINE_LEN    2

#define FILE_ENDIAN         29
#define FILE_ENDIAN_SB      "_fendian"


#define OS                  30
#define OS_SB               "_os"

#define QINTSIZE            31
#define QINTSIZE_SB         "_qintsize"

#define ENDIAN              32
#define ENDIAN_SB           "_endian"

#define NANOVM_PATH			33
#define NANOVM_PATH_SB		"_nanopath"

#define THREADING			34
#define THREADING_SB		"_threading"

#define VERSION_NUM         3430            /* version number */
#define VERSION_OBJECT      3101            /* object file version, required */

#define BLOCK_TMP_1         0              /* TMP block handling */
#define BLOCK_TMP_2         1


/* memory, for no array-variables */

#define MAXMEMBLOCK         64
#define MAXMEMBSIZE         4096

/* virtual memory */

#define MAXVMBSIZE          1048576        /* 1 MB (in bytes) */
#define MAXVMCACHESIZE      1024           /* variables */
#define VMINITWRITECACHE    8192           /* write cache for swapfile */
                                           /* creation (bytes) */

#define VARLIST_START       7              /* first public-var = 6 */
#define PROG_VARLIST_START  35             /* first var index in program */


/* virtual machine */

#define MAXVMREG            256            /* number of registers (0 - 255) + 1 */

#define NODIMS              -1             /* no dimensions (no array) */
#define NOTDEF              -1             /* not defined */

/* special registers */
#define MEMBSIZE            247             /* mem-block size */
#define VMBSIZE             248             /* vm-block size */
#define VMCACHESIZE         249             /* vm-cache size */
#define VMUSE               250             /* vm use: 1 on, 0 off */
#define STSIZE              251             /* stack size */
#define ERR_ALLOC_R         252
#define ERR_PROCESS_R       253
#define ERR_FILE_R          254
#define ERR_SOCK_R          255


/* stack memory */
#define MAXSTSIZE           1024000           /* 1000 KB (in bytes) */

/* includes (assembler), debug info */
#define MAXINCLUDES         32



/* symbol definitions */


#define SEMICOL_SB          ';'
#define COMMA_SB            ','
#define SPACE_SB            ' '
#define QUOTE_SB            '"'
#define SQUOTE_SB           '\''
#define BACKSLASH_SB        '\\'
#define GREATER_SB          '>'
#define LESS_SB             '<'
#define AROPEN_SB           '['
#define ARCLOSE_SB          ']'
#define COL_SB              ':'
#define DIV_SB              '/'

#define SEMICOL             0
#define COMMA               1
#define SPACE               2
#define QUOTE               3
#define SQUOTE              4
#define BACKSLASH           5
#define GREATER             6
#define LESS                7
#define AROPEN              8
#define ARCLOSE             9
#define COL                 10
#define DIV                 11


/* vm defs opcodes ---------------------------------------------------------------------------------- */

#define PUSH_I              0
#define PUSH_L              1
#define PUSH_Q              2
#define PUSH_D              3
#define PUSH_B              4

#define PULL_I              5
#define PULL_L              6
#define PULL_Q              7
#define PULL_D              8
#define PULL_B              9

#define MOVE_L              10
#define MOVE_D              11

#define INC_L               12
#define INC_D               13

#define DEC_L               14
#define DEC_D               15

#define ADD_L               16
#define ADD_D               17

#define SUB_L               18
#define SUB_D               19

#define MUL_L               20
#define MUL_D               21

#define DIV_L               22
#define DIV_D               23

#define SMUL_L              24
#define SDIV_L              25

#define AND_L               26
#define OR_L                27
#define BAND_L              28
#define BOR_L               29
#define BXOR_L              30
#define MOD_L               31
#define EQUAL_L             32
#define EQUAL_D             33
#define NOT_EQUAL_L         34
#define NOT_EQUAL_D         35
#define GREATER_L           36
#define GREATER_D           37
#define LESS_L              38
#define LESS_D              39
#define GREATER_OR_EQ_L     40
#define GREATER_OR_EQ_D     41
#define LESS_OR_EQ_L        42
#define LESS_OR_EQ_D        43

#define LETARRAY2_I         44
#define LETARRAY2_L         45
#define LETARRAY2_Q         46
#define LETARRAY2_D         47
#define LETARRAY2_B         48
#define LETARRAY2_S         49

#define LET2ARRAY_I         50
#define LET2ARRAY_L         51
#define LET2ARRAY_Q         52
#define LET2ARRAY_D         53
#define LET2ARRAY_B         54
#define LET2ARRAY_S         55

#define PRINT_L             56
#define PRINT_D             57
#define PRINT_S             58

#define PRINT_NEWLINE       59
#define PRINT_SPACE         60
#define PRINT_CLS           61
#define PRINT_ASCII         62
#define PRINT_BOLD          63
#define PRINT_ITALIC        64
#define PRINT_UNDERLINE     65
#define PRINT_NORMAL        66
#define PRINT_VARNAME       67

#define LOCATE              68

#define INPUT_L             69
#define INPUT_D             70
#define INPUT_S             71

#define INPUTCH_L           72
#define INPUTCH_D           73
#define INPUTCH_S           74

#define MOVESTR             75
#define MOVEP2STR           76
#define MOVESTR2P           77
#define ADD_S               78

#define STRLEN              79
#define STRLEFT             80
#define STRRIGHT            81
#define UCASE               82
#define LCASE               83
#define CHAR                84
#define ASC                 85

#define EQUALSTR_L          86
#define NOT_EQUALSTR_L      87

#define JMP_L               88
#define JSR_L               89
#define JMP                 90
#define JSR                 91
#define RTS                 92

#define EQUAL_JMP_L         93
#define NOT_EQUAL_JMP_L     94
#define GREATER_JMP_L       95
#define LESS_JMP_L          96
#define GREATER_OR_EQ_JMP_L 97
#define LESS_OR_EQ_JMP_L    98

#define EQUAL_JSR_L         99
#define NOT_EQUAL_JSR_L     100
#define GREATER_JSR_L       101
#define LESS_JSR_L          102
#define GREATER_OR_EQ_JSR_L 103
#define LESS_OR_EQ_JSR_L    104

#define CURSON              105
#define CURSOFF             106
#define CURSLEFT            107
#define CURSRIGHT           108
#define CURSUP              109
#define CURSDOWN            110

#define TIMERON             111
#define TIMEROFF            112
#define TIME                113

#define WAIT_SEC            114
#define WAIT_TICK           115

#define FOPEN               116
#define FCLOSE              117

#define FREAD_BYTE          118
#define FREAD_ARRAY_BYTE    119
#define FREAD_INT           120
#define FREAD_LINT          121
#define FREAD_QINT          122
#define FREAD_DOUBLE        123
#define FREAD_STRING        124
#define FREAD_LINE_STRING   125

#define FWRITE_BYTE         126
#define FWRITE_ARRAY_BYTE   127
#define FWRITE_INT          128
#define FWRITE_LINT         129
#define FWRITE_QINT         130
#define FWRITE_DOUBLE       131
#define FWRITE_STRING       132

#define FWRITE_STR_LINT     133
#define FWRITE_STR_DOUBLE   134
#define FWRITE_NEWLINE      135
#define FWRITE_SPACE        136

#define FSETPOS             137
#define FGETPOS             138
#define FREWIND             139
#define FSIZE               140
#define FREMOVE             141
#define FRENAME             142


/* socket functions */

#define SOPEN_SERVER        143
#define SOPEN_ACT_SERVER    144
#define SOPEN_CLIENT        145
#define SCLOSE_SERVER       146
#define SCLOSE_ACT_SERVER   147
#define SCLOSE_CLIENT       148
#define SGETCLIENTADDR      149
#define SGETHOSTNAME        150
#define SGETHOSTBYNAME      151
#define SGETHOSTBYADDR      152

#define SREAD_BYTE          153
#define SREAD_ARRAY_BYTE    154
#define SREAD_INT           155
#define SREAD_LINT          156
#define SREAD_QINT          157
#define SREAD_DOUBLE        158
#define SREAD_STRING        159
#define SREAD_LINE_STRING   160

#define SWRITE_BYTE         161
#define SWRITE_ARRAY_BYTE   162
#define SWRITE_INT          163
#define SWRITE_LINT         164
#define SWRITE_QINT         165
#define SWRITE_DOUBLE       166
#define SWRITE_STRING       167
#define SWRITE_STR_LINT     168
#define SWRITE_STR_DOUBLE   169
#define SWRITE_NEWLINE      170
#define SWRITE_SPACE        171


#define SIZEOF              172
#define VMVAR               173

#define STR2VAL_L           174
#define STR2VAL_D           175

#define VAL2STR_L           176
#define VAL2STR_D           177

#define DOUBLE2INT          178
#define INT2DOUBLE          179

#define SRAND_L             180
#define RAND_L              181
#define RAND_D              182

#define ABS_L               183
#define ABS_D               184
#define CEIL_D              185
#define FLOOR_D             186
#define EXP_D               187
#define LOG_D               188
#define LOG10_D             189
#define POW_D               190
#define SQRT_D              191
#define COS_D               192
#define SIN_D               193
#define TAN_D               194
#define ACOS_D              195
#define ASIN_D              196
#define ATAN_D              197
#define COSH_D              198
#define SINH_D              199
#define TANH_D              200

/* process handling */
#define RUNPROCESS          201
#define RUNSHELL            202
#define WAITPROCESS         203

/* shell arguments */
#define ARGNUM              204
#define ARGSTR              205


/* stack handling */
#define STPUSH_L            206
#define STPUSH_D            207
#define STPUSH_S            208
#define STPUSH_ALL_L        209
#define STPUSH_ALL_D        210

#define STPULL_L            211
#define STPULL_D            212
#define STPULL_S            213
#define STPULL_ALL_L        214
#define STPULL_ALL_D        215

#define STON                216
#define SHOW_STACK          217
#define STGETTYPE           218
#define STELEMENTS          219

#define GETTYPE             220

#define ALLOC               221
#define DEALLOC             222

#define GETADDRESS          223
#define POINTER             224

#define NOP                 225

#define PEXIT               226
#define EEXIT               227

#define INC_LESS_JMP_L          228
#define INC_LESS_OR_EQ_JMP_L    229

#define DEC_GREATER_JMP_L       230
#define DEC_GREATER_OR_EQ_JMP_L 231

#define INC_LESS_JSR_L          232
#define INC_LESS_OR_EQ_JSR_L    233

#define DEC_GREATER_JSR_L       234
#define DEC_GREATER_OR_EQ_JSR_L 235

#define HASH32_AB               236
#define STACK_SET               237

#define PUSH_S                  238
#define PULL_S                  239
#define STPUSH_ALL_S            240
#define STPULL_ALL_S            241

#define NOT_L                   242

/* POSIX THREAD FUNCTIONS */

#define VAR_LOCK                243
#define VAR_UNLOCK              244
#define THREAD_CREATE           245
#define THREAD_JOIN             246
#define THREAD_STATE            247
#define THREAD_NUM              248
#define THREAD_SYNCRONIZE       249
#define THREAD_PUSH_L           250
#define THREAD_PUSH_D           251
#define THREAD_PUSH_S           252
#define THREAD_PUSH_SYNC        253
#define THREAD_PULL_SYNC        254
#define THREAD_EXITCODE         255

/* thread private varlist */
#define ALLOC_PRIVATE           256
#define DEALLOC_PRIVATE         257

#define PPUSH_I                 258
#define PPUSH_L                 259
#define PPUSH_Q                 260
#define PPUSH_D                 261
#define PPUSH_B                 262
#define PPUSH_S                 263

#define PPULL_I                 264
#define PPULL_L                 265
#define PPULL_Q                 266
#define PPULL_D                 267
#define PPULL_B                 268
#define PPULL_S                 269

#define PMOVE_A_I               270
#define PMOVE_A_L               271
#define PMOVE_A_Q               272
#define PMOVE_A_D               273
#define PMOVE_A_B               274
#define PMOVE_A_S               275

#define PMOVE_I_A               276
#define PMOVE_L_A               277
#define PMOVE_Q_A               278
#define PMOVE_D_A               279
#define PMOVE_B_A               280
#define PMOVE_S_A               281

#define PFREAD_AB               282
#define PFWRITE_AB              283
#define PSREAD_AB               284
#define PSWRITE_AB              285

#define INT2HEX                 286

#define PHASH32_AB              287
#define HASH64_AB               288
#define PHASH64_AB              289

#define MOVE_ADYN_Q             290
#define MOVE_ADYN_D             291
#define MOVE_ADYN_S             292

#define MOVE_Q_ADYN             293
#define MOVE_D_ADYN             294
#define MOVE_S_ADYN             295

#define PMOVE_ADYN_Q            296
#define PMOVE_ADYN_D            297
#define PMOVE_ADYN_S            298

#define PMOVE_Q_ADYN            299
#define PMOVE_D_ADYN            300
#define PMOVE_S_ADYN            301

#define DYNGETTYPE              302
#define PDYNGETTYPE             303

#define VADD_L                  304
#define VADD_D                  305

#define VSUB_L                  306
#define VSUB_D                  307

#define VMUL_L                  308
#define VMUL_D                  309

#define VDIV_L                  310
#define VDIV_D                  311


#define VADD2_L                 312 
#define VADD2_D                 313

#define VSUB2_L                 314
#define VSUB2_D                 315

#define VMUL2_L                 316
#define VMUL2_D                 317

#define VDIV2_L                 318
#define VDIV2_D                 319



#define PVADD_L                 320
#define PVADD_D                 321

#define PVSUB_L                 322
#define PVSUB_D                 323

#define PVMUL_L                 324
#define PVMUL_D                 325

#define PVDIV_L                 326
#define PVDIV_D                 327


#define PVADD2_L                328
#define PVADD2_D                329

#define PVSUB2_L                330
#define PVSUB2_D                331

#define PVMUL2_L                332
#define PVMUL2_D                333

#define PVDIV2_L                334
#define PVDIV2_D                335

#define ATAN2_D                 336

#define PPOINTER                337
#define PGETTYPE                338

/* byte = x[y] */

#define MOVE_IP_B               339
#define MOVE_LP_B               340
#define MOVE_QP_B               341
#define MOVE_DP_B               342

#define MOVE_B_IP               343
#define MOVE_B_LP               344
#define MOVE_B_QP               345
#define MOVE_B_DP               346

#define PMOVE_IP_B              347
#define PMOVE_LP_B              348 
#define PMOVE_QP_B              349
#define PMOVE_DP_B              350

#define PMOVE_B_IP              351
#define PMOVE_B_LP              352
#define PMOVE_B_QP              353
#define PMOVE_B_DP              354

#define PASSWORD                355
#define MAKEDIR                 356
#define MOD_D                   357

/* hyperspace functions */
#define HSVAR					358
#define HSSERVER				359
#define HSLOAD					360
#define HSSAVE					361

#define STPUSH_ALL_ALL			362
#define STPULL_ALL_ALL			363

#define JIT						364
#define RUN_JIT_CODE			365

#define DOBJECTS				366
#define DNAMES					367
#define PDNAMES					368

/* DLL / so library opcodes */
#define LOPEN					369
#define LCLOSE					370
#define LFUNC					371
#define LCALL					372

/* assembler */
#define VM_INT                  373
#define VM_LINT                 374
#define VM_QINT                 375
#define VM_DOUBLE               376
#define VM_STRING               377
#define VM_BYTE                 378
#define VM_DYNAMIC              379
#define VM_LAB                  380
#define SETREG_L                381 
#define SETREG_D                382
#define SETREG_S                383
#define UNSETREG_ALL_L          384
#define UNSETREG_ALL_D          385
#define UNSETREG_ALL_S          386
#define SETQUOTE                387 
#define SETSEPAR                388
#define SETSEMICOL              389  /* 389 !!! */

#if JIT_CPP == 0
#define EMPTY               -1
#endif

#define SETDEFAULT_SB       "default"

#define INCLUDE_SB          "#include"         /* C-like */

#define NANOVM_ROOT_SB		"NANOVM_ROOT"	   /* root path for Nano VM */

#define VM_FILE_SB          "NANOTEMP"         /* vm-memory file */

#define CLI_PREFIX          '-'
#define CLI_NO_MSG_SB       'q'                /* no msg */

#if DEBUG
#define CLI_DEBUG_SB        'd'
#endif

#define CLI_STRIP_DEBUG_SB  's'

#define REM_SB              "//"              /* comment at beginning of */
                                              /* a line */

#define PRIVATE_VAR_SB      'P'               /* assembler private variable start */

#define NORM_OBJECTHD_SB    "nanovm  "
#define DEBUG_OBJECTHD_SB   "nanovm d"

#define OBJECTHD_LEN        8


#define FILEREAD_SB         "r"
#define FILEWRITE_SB        "w"
#define FILEAPPEND_SB       "a"
#define FILEREADWRITE_SB    "rw"
#define FILEWRITEREAD_SB    "wr"
#define FILEAPPENDREAD_SB   "ar"


#define ST_PRE              0   /* printerr, status: preprocessing */
#define ST_EXE              1   /*                   running       */


#define HS_READ             0   /* hyperspace - multicomputer shared memory */
#define HS_WRITE            1


/* the funny part ;-)  error definitions */

#define OVERFLOW_VAR_TXT    "variable overflow"
#define OVERFLOW_STR_TXT    "string overflow"
#define OVERFLOW_IND_TXT    "index overflow"
#define WRONG_VARTYPE_TXT   "wrong variable type"
#define NOTDEF_VAR_TXT      "variable not defined"
#define DOUBDEF_VAR_TXT     "variable already defined"
#define OPEN_TXT            "couldn't open file"
#define SAVE_TXT            "couldn't save file"
#define MEOF_TXT            "end of file"
#define BRNOT_OK_TXT        "brackets error"
#define SYNTAX_TXT          "syntax error"
#define MEMORY_TXT          "out of memory"
#define VARLIST_TXT         "varlist full"
#define PLIST_TXT           "plist full"
#define JUMPLIST_TXT        "jumplist full"
#define JUMPRECLIST_TXT     "jumpreclist full"
#define NOTDEF_LAB_TXT      "no label defined"
#define DOUBDEF_LAB_TXT     "label already defined"
#define PREP_FAIL_TXT       "preprocessor error"
#define DIV_BY_ZERO_TXT     "division by zero"
#define VM_FILE_TXT         "couldn't remove file"
#define OPERATOR_TXT        "illegal operator"
#define READ_TXT            "can't read file"
#define WRITE_TXT           "can't write file"
#define CLOSE_TXT           "can't close file"
#define FNUMBER_TXT         "file-number out of index"
#define FPOS_TXT            "can't set file-pos"
#define CCLIST_TXT          "cclist full"
#define PEXIT_FAIL_TXT      "exit not set"
#define UNKN_OPCODE_TXT     "unknown opcode"
#define ENV_NOT_SET_TXT     "env variable not set"
#define WRONG_VERSION_TXT   "wrong version"
#define BYTECODE_VERIFY_TXT "bytecode verify failed"
#define STACK_OVERFLOW_TXT  "stack overflow"
#define LINE_END_NOTSET_TXT "line end not set"
#define ERR_POINTER_TXT      "pointer: wrong variable"

#if JIT_CPP == 0

#define OVERFLOW_VAR        0
#define OVERFLOW_STR        1
#define OVERFLOW_IND        2
#define WRONG_VARTYPE       3
#define NOTDEF_VAR          4
#define DOUBDEF_VAR         5
#define OPEN                6
#define SAVE                7
#define MEOF                8
#define BRNOT_OK            9
#define SYNTAX              10
#define MEMORY              11
#define VARLIST             12
#define PLIST               13
#define JUMPLIST            14
#define JUMPRECLIST         15
#define NOTDEF_LAB          16
#define DOUBDEF_LAB         17
#define PREP_FAIL           18
#define DIV_BY_ZERO         19
#define VM_FILE             20
#define OPERATOR            21
#define READ                22
#define WRITE               23
#define CLOSE               24
#define FNUMBER             25
#define FPOS                26
#define CCLIST              27
#define PEXIT_FAIL          28
#define UNKN_OPCODE         29
#define ENV_NOT_SET         30
#define WRONG_VERSION       31
#define BYTECODE_VERIFY     32
#define STACK_OVERFLOW      33
#define LINE_END_NOTSET     34
#define ERR_POINTER         35

#endif

/* message definitions */

#define VM_START_TXT        "nano vm 64 bit 3.4.3-3 (c) 2016 by Stefan Pietzonke jay-t@gmx.net\n-== free software: GPL/MPL ==-\n     -==    for R.    ==-\n        ==============\n"
#define A_START_TXT         "nano assembler 64 bit 3.4.3-3  (c) 2016 by Stefan Pietzonke jay-t@gmx.net\n-== free software: GPL/MPL ==-\n"
#define VERSION_TXT         "$VER: nano 3.4.3-3 (01.07.16)"


#define STATUS_OK_TXT       "ok"
#define LOAD_PROG_TXT       "loading program:"
#define SAVE_PROG_TXT       "saving program:"
#define VM_INIT_TXT         "creating virtual memory..."

#define START               0
#define STATUS_OK           1
#define LOAD_PROG           2
#define SAVE_PROG           3
#define VM_INIT             4



/* structures */

struct plist
{
    U1 *memptr;
    S2 line_len;
    S4 srcline;
    U2 include;
};

struct includes
{
    U1 *name;
};

struct jumplist
{
    U1 islabel;
    U1 lab[MAXJUMPNAME + 1];
    S4 pos;                                 /* position in exelist */
    S4 source_pos;                          /* position in sourcecode */
};

struct jumpreclist
{
    S4 pos;
};

struct t_var
{
    U1 type;
    S4 dims;
    S4 dimens[MAXDIMENS];
    U1 varname[MAXVARNAME + 1];
    U1 digitstr[MAXSTRING_VAR + 1];
    U1 digitstr_type;
    U1 str[MAXSTRING_VAR + 1];
    S4 args;
    S4 arguments[MAXSUBARG];
    S4 varlist_ind;
	U1 base;					/* runtime conversion: exe_main */
};

struct t_op
{
    S2 type;
    S2 pos;
};

struct temp
{
    U1 s[MAXSTRING_VAR + 1];
    S2 i;
    S4 l;
    F8 d ALIGN;
};

struct memblock
{
    U1 *memptr;
    S4 size;
    S4 nextpos;
};

struct file
{
    U1 name[MAXSTRING_VAR + 1];             /* filename */
    U1 type;                                /* type: "r" = read, "w" = write, ... */
    U1 status;                              /* open, closed */
    FILE *fptr;                             /* file handle */
    U1 *buf;                                /* file data buffer */
    S4 bufsize;                             /* file data buffer size */
    U1 endian;                              /* endianess for binary data */
};

struct sock
{
    S2 sock;                                /* socket handle */
    S2 serv_conn;                           /* server connection */
    struct addrinfo *servinfo;
    U1 type;                                /* server / client */
    U1 status;                              /* open, closed */
    U1 client_ip[SOCKADDRESSLEN];           /* client ip */
    U1 buf[SOCKBUFSIZE];                    /* socket data buffer */
};

struct vm_pages
{
	U1 *(pages[MAXVMPAGES]);				/* number of virtual memory pages, pages */
	S8 size ALIGN;							/* page size (bytes) */
	S8 page_start_addr[MAXVMPAGES];			/* start address of page */
	S8 page_end_addr[MAXVMPAGES];			/* end address of page */
	S8 page_pos_file[MAXVMPAGES];			/* real position in swapfile */
	U1 page_mod[MAXVMPAGES];				/* cache modify flag: (0 = not modified, 1 = modified) */
	S8 page_hits[MAXVMPAGES];				/* cache hit counter */
	S8 vm_ind ALIGN;						/* real position in swapfile */
	S8 last_hit ALIGN;						/* last hit was in this page */
};

struct varlist
{
    U1 type;                                /* type of variable */
    U1 constant;                            /* BOOL: constant if set to TRUE (CONST_VAR) */
    U1 name[MAXVARNAME + 1];                /* name */
    U1 vm;									/* virtual memory (BOOL) */
    struct vm_pages vm_pages;
    S4 dims;                                /* -1 = NODIMS => no array */
    S4 dimens[MAXDIMENS];

    S2 *i_m;
    S4 *li_m;
    S8 *q_m ALIGN;    /* 64 bit longint */
    F8 *d_m ALIGN;
    U1 *s_m;
    S8 size ALIGN;

    S2 *i_vm;
    S4 *li_vm;
    S8 *q_vm ALIGN;    /* 64 bit longint */
    F8 *d_vm ALIGN;
    U1 *s_vm;
    S8 vmcachesize ALIGN;					/* virtual memory cachesize in indexes */
	
    #if OS_AROS
        void *mutex;
    #else
        pthread_mutex_t mutex;
    #endif
		
	S2 hs_sock;							/* hyperspace socket handle: client */
};

struct varlist_state
{
    S4 varlist_ind ALIGN;
    S4 maxvarlist;
    S4 varlist_size;
};

struct dynamic_array
{
    void *memptr;
    U1 type;
};


struct vmreg
{
    S8 l[MAXVMREG];                         /* the vm-registers */
    F8 d[MAXVMREG];
    U1 s[MAXVMREG][MAXSTRING_VAR + 1];
};

struct vmreg_name
{
    U1 l[MAXVMREG][MAXVARNAME + 1];
    U1 d[MAXVMREG][MAXVARNAME + 1];
    U1 s[MAXVMREG][MAXVARNAME + 1];
};


/* memory */

struct vm_mem
{
    U1 error;           /* error flag: 0 = no error, 1 = error */
    U1 init;            /* 0 = no swapfile, 1 = swapfile created */
    S8 ind ALIGN;             /* global vmfile index */
    U1 filename[256];   /* swapfile name */
    FILE *file;         /* swapfile handle */
    S8 mblock_ind ALIGN;      /* memory block index, normal variables */
    S4 obj_maxvarlist;  /* max variable */
    S4 obj_maxpvarlist; /* max thread private varlist */
};


struct opcode
{
    U1 op[16];
    S2 args;
    S2 type[3];
};

struct src_line
{
    U1 opcode[MAXLINELEN + 1];
    S2 opcode_n;                            /* opcode number (nanoc) */
    U1 arg[MAXCOMMARG][MAXLINELEN + 1];
    S2 args;
};

struct arg
{
    S2 type;
    S4 reg;
};

struct shell_args
{
    S2 args;
    U1 str[MAXSHELLARG][MAXLINELEN + 1];
};

struct assemb_set
{
    U1 quote;
    U1 separ;
    U1 semicol;
};

/* POSIX threads */

struct pthreads
{
    U1 state;
    S4 startpos;
    S8 ret_val;					/* new: changed to 64 bit value */
    
    #if OS_AROS
        uint32_t thread;
        U1 socket_lib_open;
    #else
        pthread_t thread;
    #endif

    S4 jumpreclist_ind;

    struct jumpreclist jumpreclist[MAXJUMPRECLEN];
    struct t_var t_var;

    U1 *stack;
    S4 stack_ind;
    S4 stack_elements;
    
    #if OS_AROS
        void *stack_mutex;
    #else
        pthread_mutex_t stack_mutex;
    #endif
};


#define MAXJITCODE	100

/* execution rights of a program */
struct rights
{
	U1 network;
	U1 file;
	U1 fileroot;
	U1 process;
};


/* .dll / .so shared library functions ----------------------------------------- */

#define MAXDLLS			32
#define MAXDLLFUNC		64

typedef S8 (*dll_func)(U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize);

struct dlls
{
	U1 name[MAXSTRING_VAR + 1];  		/* dll name */   
	
	#if OS_WINDOWS						/* dll handle */
		HINSTANCE lptr;
	#else
		void *lptr;
	#endif
		
	dll_func func[MAXDLLFUNC];
};

#include "protos.h"

/* END */

