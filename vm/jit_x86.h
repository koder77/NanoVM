/****************************************************************************
*
* Filename: jit_x86.h
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  "byte storm" - AsmJit JIT compiler for x86
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2013
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

/* N code must be compiled with -O2 and NESTED_CODE preprocessor
 * flag set as needed.
 * 
 * JIT compiler is usable now, should be stable.
 * 
 * Now for use with new AsmJit 1.1 on github.
 */

#define OFFSET(x) x * 8

/* its so ugly!!!! - but works!!! */
	#if JIT_X86_64
		#define RSI 	rsi
		#define RDI 	rdi
		#define R8 		r8
		#define R9 		r9
		#define R10		r10
		#define R11 	r11
		#define RBX 	rbx
		#define RCX 	rcx
		#define EAX 	eax
		#define EDX		edx
	#endif

	#if JIT_X86_32
		#define RSI 	esi
		#define RDI 	edi
		#define R8		ebp
		#define R9		esp
		#define R10		ebx
		#define R11 	ecx
		#define RBX		ebx
		#define RCX 	ecx
		#define EAX 	eax
		#define EDX 	edx
	#endif


/* AsmJit JIT compiler functions */

using namespace asmjit;
using namespace asmjit::host;

// This is type of function we will generate
typedef uint32_t (*Func)(void);

extern struct jumplist *jumplist;
extern S4 maxjumplist;

extern S4 JIT_code_ind;

struct JIT_code
{
	Func fn;
};

struct JIT_code JIT_code[MAXJITCODE];


struct JIT_label
{
	asmjit::Label lab;
	S4 pos;
	S4 if_;
	S4 endif;
};

struct JIT_label JIT_label[MAXJUMPLEN];

extern S4 JIT_label_ind;


void partstr (U1 *str, U1 *retstr, S2 start, S2 end)
{
    S2 i, j = 0;
    for (i = start; i <= end; i++)
    {
        retstr[j] = str[i];
        j++;
    }
    retstr[j] = BINUL;
}


extern "C" int jit_compiler (S4 ***clist, struct vmreg *vmreg, S4 start, S4 end)
{

// ==========================================================================

	/* debug: */
	S8 deb;
	
	S4 i, j, l, k;
	S8 r1, r2, r3;
	S8 r1v, r2v, r3v;
	
	S2 opcode;
	
	U1 found_if = 0, found_else = 0, label_created;
	S4 if_check;
	S4 label;
	
	int run_jit = 0;
	
	U1 buf[256];
	
	#if DEBUG
		printf ("byte storm - START\n");
		printf ("start: %i\n", start);
		printf ("end: %i\n", end);
	#endif				
	// Create assembler.
	
	FileLogger logger(stdout);
	logger.setOption(kLoggerOptionBinaryForm, true);

	JitRuntime runtime;
	X86Assembler a(&runtime);
	
	/* register bases used for calculating register offset (OFFSET()) */
	
	a.mov (RSI, imm ((intptr_t)(void *) &vmreg->l[0])); /* long registers base: rsi */
	
	a.mov (RDI, imm ((intptr_t)(void *) &vmreg->d[0])); /* double registers base: rdi */
	
	
	
	#if DEBUG
		printf ("JUMPLIST\n");
		for (i = 0; i <= maxjumplist; i++)
		{
			printf ("labelname: %s\n", jumplist[i].lab);
			printf ("position: %i\n", jumplist[i].pos);
			printf ("islabel: %i\n", jumplist[i].islabel);
		}
		printf ("JUMPLIST END\n\n");
	#endif	
	/* initialize label pos */
	
	for (i = 0; i < MAXJUMPLEN; i++)
	{
		JIT_label[i].pos = -1;
	}
		
	for (i = start; i <= end; i++)
	{
		#if DEBUG 
			printf ("opcode: %i\n", (*clist)[i][0]);
		#endif
		
		/* check if current opcode is on label */
		label_created = 0;
		
		for (j = 0; j <= maxjumplist; j++)
		{
			if (i == jumplist[j].pos)
			{
				/* create label */
				
				if (jumplist[j].lab[0] == 'i' && jumplist[j].lab[1] == 'f')
				{
					break;
				}
				
				if (jumplist[j].lab[0] == 'e' && jumplist[j].lab[1] == 'n' && jumplist[j].lab[2] == 'd' && jumplist[j].lab[3] == 'i' && jumplist[j].lab[4] == 'f')
				{
					break;
				}
				
				for (l = 0; l < MAXJUMPLEN; l++)
				{
					if (JIT_label[l].pos == i)
					{
						label_created = 1;
						label = l;
						break;
					}
				}
					
				if (label_created == 0 && JIT_label_ind < MAXJUMPLEN)
				{
					JIT_label_ind++;
				}
				else
				{
					if (JIT_label_ind == MAXJUMPLEN)
					{
						printf ("JIT compiler: error label list full!\n");
						return (1);
					}
				}
				
				if (jumplist[j].lab[0] == 'f' && jumplist[j].lab[1] == 'o' && jumplist[j].lab[2] == 'r')
				{
					/* search INC JMP OR DEC JMP: and load counter register */
					for (l = i; l <= end; l++)
					{
						if ((*clist)[l][0] == INC_LESS_OR_EQ_JMP_L || (*clist)[l][0] == DEC_GREATER_OR_EQ_JMP_L)
						{
							r1 = (*clist)[l][1]; r2 = (*clist)[l][2];
							a.mov (R8, qword_ptr (RSI, OFFSET(r1)));
							a.mov (R9, qword_ptr (RSI, OFFSET(r2)));
						}
					}
				}
				#if DEBUG	
					printf ("labelname: %s\n", jumplist[j].lab);
					printf ("position: %i\n", jumplist[j].pos);
					printf ("islabel: %i\n", jumplist[j].islabel);
				#endif
					
				if (label_created == 0)
				{
					JIT_label[JIT_label_ind].lab = a.newLabel ();
					JIT_label[JIT_label_ind].pos = jumplist[j].pos;
					JIT_label[JIT_label_ind].if_ = -1; 
					JIT_label[JIT_label_ind].endif = -1;
					a.bind (JIT_label[JIT_label_ind].lab);
				}
				else
				{
					a.bind (JIT_label[label].lab);
				
				#if DEBUG
					printf ("LABEL binded!\n");
				#endif
				}
			}
		}
		
		
		switch ((*clist)[i][0])
		{
			case PUSH_I:
			case PUSH_L:
			case PUSH_Q:
			case PUSH_D:
			case PUSH_B:
				break;
			
			case PULL_I:
			case PULL_L:
			case PULL_Q:
			case PULL_D:
			case PULL_B:
				break;
			
			case MOVE_L:
				PRINTD ("MOVE_L");
				
				r1 = (*clist)[i][1]; r2 = (*clist)[i][2];
				
				a.mov (RBX, qword_ptr (RSI, OFFSET(r1)));
				a.mov (qword_ptr (RSI, OFFSET(r2)), RBX);
				
				run_jit = 1;
				break;
				
			case MOVE_D:
				PRINTD ("MOVE_D");
				
				r1 = (*clist)[i][1]; r2 = (*clist)[i][2];
				
				a.mov (RBX, qword_ptr (RDI, OFFSET(r1)));
				a.mov (qword_ptr (RDI, OFFSET(r2)), RBX);
				
				run_jit = 1;
				break;
				
			case INC_L:
				PRINTD ("INC_L");
				
				r1 = (*clist)[i][1];
				
				a.mov (RBX, qword_ptr (RSI, OFFSET(r1)));
				a.inc (RBX);
			    a.mov (qword_ptr (RSI, OFFSET(r1)), RBX);
				
				run_jit = 1;
				break;
				
			case DEC_L:
				PRINTD ("DEC_L");
				
				r1 = (*clist)[i][1];
				
				a.mov (RBX, qword_ptr (RSI, OFFSET(r1)));
				a.dec (RBX);
				a.mov (qword_ptr (RSI, OFFSET(r1)), RBX);
				
				run_jit = 1;
				break;
				
			case ADD_L:
			case SUB_L:
			case MUL_L:
			case DIV_L:
				r1 = (*clist)[i][1]; r2 = (*clist)[i][2]; r3 = (*clist)[i][3];
				
				a.mov (RBX, qword_ptr (RSI, OFFSET(r1))); /* r1v */
				a.mov (RCX, qword_ptr (RSI, OFFSET(r2))); /* r2v */
				
				switch ((*clist)[i][0])
				{
					case ADD_L:						
						a.add (RBX, RCX);
						break;
					
					case SUB_L:
						a.sub (RBX, RCX);
						break;
						
					case MUL_L:
						a.imul (RBX, RCX);
						break;
						
					case DIV_L:
						a.mov (EAX, RBX);
						a.xor_ (EDX, EDX);		/* clear the upper 64 bit */
						a.idiv (RCX);
						a.mov (RBX, EAX);
						break;
				}
					
				j = i + 1;
	
				opcode = (*clist)[i][0];
				
				while ((*clist)[j][0] == opcode && j <= end)
				{
					/* check if its a x = a + b + c chained ADD_L */
					
					r1 = (*clist)[j][1]; r2 = (*clist)[j][2]; r3 = (*clist)[j][3];
					
					if ((*clist)[j - 1][3] == r1 && (*clist)[j - 1][3] == r3)
					{
						switch ((*clist)[i][0])
						{
							case ADD_L:	
								a.mov (RCX, qword_ptr (RSI, OFFSET(r2)));
								a.add (RBX, RCX);
								break;
						
							case SUB_L:
								a.mov (RCX, qword_ptr (RSI, OFFSET(r2)));
								a.sub (RBX, RCX);
								break;
								
							case MUL_L:
								a.mov (RCX, qword_ptr (RSI, OFFSET(r2)));
								a.imul (RBX, RCX);
								break;
								
							case DIV_L:
								a.mov (EAX, RBX);
								a.xor_ (EDX, EDX);		/* clear the upper 64 bit */
								a.idiv (RCX);
								a.mov (RBX, EAX);
								break;
						}
					}
					j++;
				} 
				
				a.mov (qword_ptr (RSI, OFFSET(r3)), RBX);

				i = j - 1;
				
				run_jit = 1;
				break;
			
			case ADD_D:
			case SUB_D:
			case MUL_D:
			case DIV_D:
				r1 = (*clist)[i][1]; r2 = (*clist)[i][2]; r3 = (*clist)[i][3];
				
				a.fld (qword_ptr (RDI, OFFSET(r1)));
				a.fld (qword_ptr (RDI, OFFSET(r2)));
				
				switch ((*clist)[i][0])
				{
					case ADD_D:						
						a.faddp ();
						break;
					
					case SUB_D:
						a.fsubp ();
						break;
						
					case MUL_D:
						a.fmulp ();
						break;
						
					case DIV_D:
						a.fdivp ();
						break;
				}
				
				j = i + 1;
				
				opcode = (*clist)[i][0];
				
				while ((*clist)[j][0] == opcode && j <= end)
				{
					/* check if its a x = a + b + c chained ADD_L */
					
				    r2 = (*clist)[j][2]; r3 = (*clist)[j][3];
					
					a.fld (qword_ptr (RDI, OFFSET(r2)));
					
					if ((*clist)[j - 1][3] == r1 && (*clist)[j - 1][3] == r3)
					{
						switch ((*clist)[i][0])
						{
							case ADD_D:						
								a.faddp ();
								break;
					
							case SUB_D:
								a.fsubp ();
								break;
						
							case MUL_D:
								a.fmulp ();
								break;
						
							case DIV_D:
								a.fdivp ();
								break;
						}
					}
					j++;
				} 
				
				a.fstp (qword_ptr (RDI, OFFSET(r3)));

				i = j - 1;
				
				run_jit = 1;
				break;
				
			case AND_L:
				PRINTD ("AND_L");
				
				r1 = (*clist)[i][1]; r2 = (*clist)[i][2]; r3 = (*clist)[i][3];
				
				a.mov (RBX, qword_ptr (RSI, OFFSET(r1))); /* r1v */
				a.mov (RCX, qword_ptr (RSI, OFFSET(r2))); /* r2v */
				
				a.and_ (RBX, RCX);
				
				a.mov (qword_ptr (RSI, OFFSET(r3)), RBX);
				
				run_jit = 1;
				break;
				
			case OR_L:
				PRINTD ("OR_L");
				
				r1 = (*clist)[i][1]; r2 = (*clist)[i][2]; r3 = (*clist)[i][3];
				
				a.mov (RBX, qword_ptr (RSI, OFFSET(r1))); /* r1v */
				a.mov (RCX, qword_ptr (RSI, OFFSET(r2))); /* r2v */
				
				a.or_ (RBX, RCX);
				
				a.mov (qword_ptr (RSI, OFFSET(r3)), RBX);
				
				run_jit = 1;
				break;	
			
			case BAND_L:
				PRINTD ("BAND_L");
				
				r1 = (*clist)[i][1]; r2 = (*clist)[i][2]; r3 = (*clist)[i][3];
				
				a.movq (mm0, qword_ptr (RSI, OFFSET(r1))); /* r1v */
				a.movq (mm1, qword_ptr (RSI, OFFSET(r2))); /* r2v */
				
				a.pand (mm0, mm1);
				
				a.movq (qword_ptr (RSI, OFFSET(r3)), mm0);
				
				run_jit = 1;
				break;
			
			case BOR_L:
				PRINTD ("BOR_L");
				
				r1 = (*clist)[i][1]; r2 = (*clist)[i][2]; r3 = (*clist)[i][3];
				
				a.movq (mm0, qword_ptr (RSI, OFFSET(r1))); /* r1v */
				a.movq (mm1, qword_ptr (RSI, OFFSET(r2))); /* r2v */
				
				a.por (mm0, mm1);
				
				a.movq (qword_ptr (RSI, OFFSET(r3)), mm0);
				
				run_jit = 1;
				break;	
				
			case BXOR_L:
				PRINTD ("BXOR_L");
				
				r1 = (*clist)[i][1]; r2 = (*clist)[i][2]; r3 = (*clist)[i][3];
				
				a.movq (mm0, qword_ptr (RSI, OFFSET(r1))); /* r1v */
				a.movq (mm1, qword_ptr (RSI, OFFSET(r2))); /* r2v */
				
				a.pxor (mm0, mm1);
				
				a.movq (qword_ptr (RSI, OFFSET(r3)), mm0);
				
				run_jit = 1;
				break;
			
			case MOD_L:
				PRINTD ("MOD_L");
				
				r1 = (*clist)[i][1]; r2 = (*clist)[i][2]; r3 = (*clist)[i][3];
				
				a.mov (RBX, qword_ptr (RSI, OFFSET(r1))); /* r1v */
				a.mov (RCX, qword_ptr (RSI, OFFSET(r2))); /* r2v */
				
				a.mov (EAX, RBX);
				a.xor_ (EDX, EDX);		/* clear the upper 64 bit */
				a.idiv (RCX);
				a.mov (RBX, EDX);
						
				a.mov (qword_ptr (RSI, OFFSET(r3)), RBX);
				break;
				
				
			case EQUAL_L:
			case NOT_EQUAL_L:
			case GREATER_L:
			case LESS_L:
			case GREATER_OR_EQ_L:
			case LESS_OR_EQ_L:
				PRINTD ("COMPARE_L");
				
				r1 = (*clist)[i][1]; r2 = (*clist)[i][2]; r3 = (*clist)[i][3];
				
				/* check following if label */
				
				found_if = 0;
				
				for (j = 0; j <= maxjumplist; j++)
				{
					if (jumplist[j].pos > i)
					{
						if (jumplist[j].lab[0] == 'i' && jumplist[j].lab[1] == 'f')
						{
							/* found if label */
							
							found_if = 1;
							break;
						}
					}
				}
				
				if (found_if == 1)
				{
					a.mov (RBX, qword_ptr (RSI, OFFSET(r1)));
					a.mov (RCX, qword_ptr (RSI, OFFSET(r2)));
					
					a.cmp (RBX, RCX);
					
					/* insert label else */
					
					if (JIT_label_ind < MAXJUMPLEN)
					{
						JIT_label_ind++;
					}
					else
					{
						printf ("JIT compiler: error label list full!\n");
						return (1);
					}
				
					JIT_label[JIT_label_ind].lab = a.newLabel ();
					JIT_label[JIT_label_ind].pos = i;
					
					partstr ((U1 *) jumplist[j].lab, (U1 *) buf, 3, strlen ((const char *) jumplist[j].lab) - 1);
					JIT_label[JIT_label_ind].if_ = strtoll ((const char *) buf, NULL, 10);
					
					printf ("IF: %i\n", JIT_label[JIT_label_ind].if_);
					
					JIT_label[JIT_label_ind].endif = -1;
					
					switch ((*clist)[i][0])
					{
						case EQUAL_L:
							a.jne (JIT_label[JIT_label_ind].lab);
							break;
							
						case NOT_EQUAL_L:
							a.je (JIT_label[JIT_label_ind].lab);
							break;	
					
						case GREATER_L:
							a.jle (JIT_label[JIT_label_ind].lab);
							break;
							
						case LESS_L:
							a.jng (JIT_label[JIT_label_ind].lab);
							break;
							
						case GREATER_OR_EQ_L:
							a.jge (JIT_label[JIT_label_ind].lab);
							break;	
					
						case LESS_OR_EQ_L:
							a.jnge (JIT_label[JIT_label_ind].lab);
							break;
					}
					i = i + 4;
				}
				else
				{
					/* do compare */
					
					a.mov (RBX, qword_ptr (RSI, OFFSET(r1)));
					a.mov (RCX, qword_ptr (RSI, OFFSET(r2)));
					a.mov (qword_ptr (RSI, OFFSET(r3)), 0);				/* move zero to r3 */
					
					a.cmp (RBX, RCX);
					
					/* move one to r3 if condition is true: */
					
					switch ((*clist)[i][0])
					{
						case EQUAL_L:
							a.mov (RBX, 1);
							a.cmove (RCX, RBX);
							a.mov (qword_ptr (RSI, OFFSET(r3)), RCX);
							break;
							
						case NOT_EQUAL_L:
							a.mov (RBX, 1);
							a.cmovne (RCX, RBX);
							a.mov (qword_ptr (RSI, OFFSET(r3)), RCX);
							break;
							
						case GREATER_L:
							a.mov (RBX, 1);
							a.cmovg (RCX, RBX);
							a.mov (qword_ptr (RSI, OFFSET(r3)), RCX);
							break;
							
						case LESS_L:
							a.mov (RBX, 1);
							a.cmovl (RCX, RBX);
							a.mov (qword_ptr (RSI, OFFSET(r3)), RCX);
							break;
							
						case GREATER_OR_EQ_L:
							a.mov (RBX, 1);
							a.cmovge (RCX, RBX);
							a.mov (qword_ptr (RSI, OFFSET(r3)), RCX);
							break;
						
						case LESS_OR_EQ_L:
							a.mov (RBX, 1);
							a.cmovle (RCX, RBX);
							a.mov (qword_ptr (RSI, OFFSET(r3)), RCX);
							break;
					}
				}	
							
				run_jit = 1;
				break;
			
			
			case EQUAL_D:
			case NOT_EQUAL_D:
			case GREATER_D:
			case LESS_D:
			case GREATER_OR_EQ_D:
			case LESS_OR_EQ_D:
				PRINTD ("COMPARE_D");
				
				r1 = (*clist)[i][1]; r2 = (*clist)[i][2]; r3 = (*clist)[i][3];
				
				/* check following if label */
				
				found_if = 0;
				
				for (j = 0; j <= maxjumplist; j++)
				{
					if (jumplist[j].pos > i)
					{
						if (jumplist[j].lab[0] == 'i' && jumplist[j].lab[1] == 'f')
						{
							/* found if label */
							
							found_if = 1;
							break;
						}
					}
				}
				
				if (found_if == 1)
				{
					a.fld (qword_ptr (RDI, OFFSET(r1)));
					a.fld (qword_ptr (RDI, OFFSET(r2)));
				
					a.fcomip (fp(1));
					
					/* insert label else */
					
					if (JIT_label_ind < MAXJUMPLEN)
					{
						JIT_label_ind++;
					}
					else
					{
						printf ("JIT compiler: error label list full!\n");
						return (1);
					}
				
					JIT_label[JIT_label_ind].lab = a.newLabel ();
					JIT_label[JIT_label_ind].pos = i;
					
					partstr ((U1 *) jumplist[j].lab, (U1 *) buf, 3, strlen ((const char *) jumplist[j].lab) - 1);
					JIT_label[JIT_label_ind].if_ = strtoll ((const char *) buf, NULL, 10);
					
					printf ("IF: %i\n", JIT_label[JIT_label_ind].if_);
					
					JIT_label[JIT_label_ind].endif = -1;
					
					switch ((*clist)[i][0])
					{
						case EQUAL_D:
							a.jne (JIT_label[JIT_label_ind].lab);
							break;
							
						case NOT_EQUAL_D:
							a.je (JIT_label[JIT_label_ind].lab);
							break;	
					
						case GREATER_D:
							a.jle (JIT_label[JIT_label_ind].lab);
							break;
							
						case LESS_D:
							a.jng (JIT_label[JIT_label_ind].lab);
							break;
							
						case GREATER_OR_EQ_D:
							a.jge (JIT_label[JIT_label_ind].lab);
							break;	
					
						case LESS_OR_EQ_D:
							a.jnge (JIT_label[JIT_label_ind].lab);
							break;
					}
					i = i + 4;
				}
				else
				{
					a.fld (qword_ptr (RDI, OFFSET(r1)));
					a.fld (qword_ptr (RDI, OFFSET(r2)));
				
					a.fcomip (fp(1));
					
					switch ((*clist)[i][0])
					{
						case EQUAL_D:
							a.setz (RCX);
							a.mov (qword_ptr (RSI, OFFSET(r3)), RCX);
							break;
							
						case NOT_EQUAL_D:
							a.setnz (RCX);
							a.mov (qword_ptr (RSI, OFFSET(r3)), RCX);
							break;
							
						case GREATER_D:
							a.seta (RCX);
							a.mov (qword_ptr (RSI, OFFSET(r3)), RCX);
							break;
						
						case LESS_D:
							a.setc (RCX);
							a.mov (qword_ptr (RSI, OFFSET(r3)), RCX);
							break;
							
						case GREATER_OR_EQ_D:
							a.setae (RCX);
							a.mov (qword_ptr (RSI, OFFSET(r3)), RCX);
							break;
							
						case LESS_OR_EQ_D:
							a.setbe (RCX);
							a.mov (qword_ptr (RSI, OFFSET(r3)), RCX);
							break;
					}
				}
				
				run_jit = 1;
				break;
				
			case JMP_L:
				PRINTD ("JMP_L");
				
				r1 = (*clist)[i][1]; r2 = (*clist)[i][2];
				
				a.mov (RBX, qword_ptr (RSI, OFFSET(r1)));
				a.mov (RCX, 1);
				
				if (r1 < i)
				{
					/* label was created, check */
					
					for (j = 0; j <= JIT_label_ind; j++)
					{
						if (JIT_label[j].pos == r2)
						{
							a.cmp (RBX, RCX);
							a.jmp (JIT_label[j].lab);
						}
					}
				}
				else
				{
					/* create label */
				
					if (JIT_label_ind < MAXJUMPLEN)
					{
						JIT_label_ind++;
					}
					else
					{
						printf ("JIT compiler: error label list full!\n");
						return (1);
					}
				
					JIT_label[JIT_label_ind].lab = a.newLabel ();
					JIT_label[JIT_label_ind].pos = r2;
					
					a.cmp (RBX, RCX);
					a.jmp (JIT_label[JIT_label_ind].lab);
				}
				run_jit = 1;
				break;
				
			case JMP:
				PRINTD ("JMP");
				
				r1 = (*clist)[i][1];
				
				if (found_if == 1)
				{
					#if DEBUG
						printf ("JMP: if\n");
					#endif
						
					found_else = 0;
					for (j = 0; j <= maxjumplist; j++)
					{
						if (jumplist[j].pos > i)
						{
							if (jumplist[j].lab[0] == 'e' && jumplist[j].lab[1] == 'l' && jumplist[j].lab[2] == 's' && jumplist[j].lab[3] == 'e')
							{
								/* found else label */
							
								found_else = 1;
								break;
							}
						}
					}
					
					if (found_else == 1)
					{
						partstr ((U1 *) jumplist[j].lab, (U1 *) buf, 5, strlen ((const char *) jumplist[j].lab) - 1);
						if_check = strtoll ((const char *) buf, NULL, 10);
						
						for (j = 0; j <= JIT_label_ind; j++)
						{
							if (JIT_label[j].if_ == if_check)
							{
								/* create endif label */
								
								if (JIT_label_ind < MAXJUMPLEN)
								{
									JIT_label_ind++;
								}
								else
								{
									printf ("JIT compiler: error label list full!\n");
									return (1);
								}
				
								JIT_label[JIT_label_ind].lab = a.newLabel ();
								JIT_label[JIT_label_ind].pos = r1;
								JIT_label[JIT_label_ind].if_ = if_check;
								JIT_label[JIT_label_ind].endif = if_check;
								
								a.jmp (JIT_label[JIT_label_ind].lab);
								
								a.bind (JIT_label[j].lab);
								
								#if DEBUG
									printf ("FOUND ELSE\n");
								#endif
								break;
							}
						}
					}
				}
				else
				{
					if (r1 < i)
					{
						/* label was created, check */
					
						for (j = 0; j <= JIT_label_ind; j++)
						{
							if (JIT_label[j].pos == r1)
							{
								a.jmp (JIT_label[j].lab);
							}
						}
					}
					else
					{
						/* create label */
				
						if (JIT_label_ind < MAXJUMPLEN)
						{
							JIT_label_ind++;
						}
						else
						{
							printf ("JIT compiler: error label list full!\n");
							return (1);
						}
				
						JIT_label[JIT_label_ind].lab = a.newLabel ();
						JIT_label[JIT_label_ind].pos = r1;
					
						a.jmp (JIT_label[JIT_label_ind].lab);
					}
				}
					
				run_jit = 1;
				break;
								
			
						
							
			case EQUAL_JMP_L:
			case NOT_EQUAL_JMP_L:
			case GREATER_JMP_L:
			case LESS_JMP_L:
			case GREATER_OR_EQ_JMP_L:
			case LESS_OR_EQ_JMP_L:
				PRINTD ("x_JMP_L");
				
				r1 = (*clist)[i][1]; r2 = (*clist)[i][2]; r3 = (*clist)[i][3];
				
				a.mov (qword_ptr (RSI, OFFSET(r1)), R10);
				a.mov (qword_ptr (RSI, OFFSET(r2)), R11);
				
				a.cmp (R10, R11);
				
				if (r3 < i)
				{
					/* label was created, check */
					
					for (j = 0; j <= JIT_label_ind; j++)
					{
						if (JIT_label[j].pos == r3)
						{
							switch ((*clist)[i][0])
							{
								case EQUAL_JMP_L:
									a.je (JIT_label[j].lab);
									break;
									
								case NOT_EQUAL_JMP_L:
									a.jne (JIT_label[j].lab);
									break;
									
								case GREATER_JMP_L:
									a.jg (JIT_label[j].lab);
									break;
									
								case LESS_JMP_L:
									a.jl (JIT_label[j].lab);
									break;
									
								case GREATER_OR_EQ_JMP_L:
									a.jge (JIT_label[j].lab);
									break;
									
								case LESS_OR_EQ_JMP_L:
									a.jle (JIT_label[j].lab);
									break;
							}
						}
					}
				}
				else
				{
					/* create label */
				
					if (JIT_label_ind < MAXJUMPLEN)
					{
						JIT_label_ind++;
					}
					else
					{
						printf ("JIT compiler: error label list full!\n");
						return (1);
					}
				
					JIT_label[JIT_label_ind].lab = a.newLabel ();
					JIT_label[JIT_label_ind].pos = r3;
				
					switch ((*clist)[i][0])
					{
						case EQUAL_JMP_L:
							a.je (JIT_label[JIT_label_ind].lab);
							break;
							
						case NOT_EQUAL_JMP_L:
							a.jne (JIT_label[JIT_label_ind].lab);
							break;
							
						case GREATER_JMP_L:
							a.jg (JIT_label[JIT_label_ind].lab);
							break;
							
						case LESS_JMP_L:
							a.jl (JIT_label[JIT_label_ind].lab);
							break;
							
						case GREATER_OR_EQ_JMP_L:
							a.jge (JIT_label[JIT_label_ind].lab);
							break;
							
						case LESS_OR_EQ_JMP_L:
							a.jle (JIT_label[JIT_label_ind].lab);
							break;
					}
				}
				run_jit = 1;
				break;
			

			case COS_D:
				PRINTD ("COS_D");
				
				r1 = (*clist)[i][1]; r2 = (*clist)[i][2];
				
				a.fld (qword_ptr (RDI, OFFSET(r1)));
				
				a.fcos ();
				
				a.fstp (qword_ptr (RDI, OFFSET(r2)));
				
				run_jit = 1;
				break;
				
			case SIN_D:
				PRINTD ("SIN_D");
				
				r1 = (*clist)[i][1]; r2 = (*clist)[i][2];
				
				a.fld (qword_ptr (RDI, OFFSET(r1)));
				
				a.fsin ();
				
				a.fstp (qword_ptr (RDI, OFFSET(r2)));			
						
				run_jit = 1;
				break;
					
				
			case NOP:
				a.nop ();		/* just do nothing, really! */
				
				run_jit = 1;
				break;
				
			case INC_LESS_OR_EQ_JMP_L:
				PRINTD ("INC_LESS_OR_EQ_JMP_L");
				
				r1 = (*clist)[i][1]; r2 = (*clist)[i][2]; r3 = (*clist)[i][3];
				
				a.inc (R8);
				a.mov (qword_ptr (RSI, OFFSET(r1)), R8);
					
				if (r3 < i)
				{
					/* label was created, check */
					
					for (j = 0; j <= JIT_label_ind; j++)
					{
						if (JIT_label[j].pos == r3)
						{
							a.cmp (R8, R9);
							a.jle (JIT_label[j].lab);
						}
					}
				}
				else
				{
					/* create label */
				
					if (JIT_label_ind < MAXJUMPLEN)
					{
						JIT_label_ind++;
					}
					else
					{
						printf ("JIT compiler: error label list full!\n");
						return (1);
					}
				
					JIT_label[JIT_label_ind].lab = a.newLabel ();
					JIT_label[JIT_label_ind].pos = r3;
				
					a.cmp (R8, R9);
					a.jle (JIT_label[JIT_label_ind].lab);
				}
				
				run_jit = 1;
				break;
				
			case DEC_GREATER_OR_EQ_JMP_L:
				PRINTD ("DEC_GREATER_OR_EQ_JMP_L");
				
				r1 = (*clist)[i][1]; r2 = (*clist)[i][2]; r3 = (*clist)[i][3];
				
				a.dec (R8);
				a.mov (qword_ptr (RSI, OFFSET(r1)), R8);
					
				if (r3 < i)
				{
					/* label was created, check */
					
					for (j = 0; j <= JIT_label_ind; j++)
					{
						if (JIT_label[j].pos == r3)
						{
							a.cmp (R8, R9);
							a.jne (JIT_label[j].lab);
						}
					}
				}
				else
				{
					/* create label */
				
					if (JIT_label_ind < MAXJUMPLEN)
					{
						JIT_label_ind++;
					}
					else
					{
						printf ("JIT compiler: error label list full!\n");
						return (1);
					}
				
					JIT_label[JIT_label_ind].lab = a.newLabel ();
					JIT_label[JIT_label_ind].pos = r3;
				
					a.cmp (R8, R9);
					a.jne (JIT_label[JIT_label_ind].lab);
				}
				
				run_jit = 1;
				break;
			
			case NOT_L:
				break;
				
			default:
				printf ("JIT compiler: UNKNOWN opcode: %i - exiting!\n", (*clist)[i][0]);
				return (1);
		}
	}
	
	if (run_jit)
	{
		a.ret ();		/* return to main program code */
		
		if (JIT_code_ind < MAXJITCODE)
		{
			JIT_code_ind++;
			JIT_code[JIT_code_ind].fn = asmjit_cast<Func>(a.make());		/* create JIT code function */ 
			
			#if DEBUG
				printf ("JIT compiler: function saved.\n");
			#endif
			
			return (0);
		}
		else
		{
			printf ("JIT compiler: error jit code list full!\n");
			return (1);
		}
	}
	return (1);
}


extern "C" int run_jit (S8 code, struct vmreg *vmreg)
{
	#if	 DEBUG
		printf ("run_jit: code: %li\n", code);
	#endif
	
	Func func = JIT_code[code].fn;
		
	/* call JIT code function, stored in JIT_code[] */
	func ();
	return (0);
}

extern "C" int free_jit_code ()
{
	/* free all JIT code functions from memory */

	using namespace asmjit;
	using namespace asmjit::host;
	
	JitRuntime runtime;
	
	S4 i;

	if (JIT_code_ind > -1)
	{
		for (i = 0; i <= JIT_code_ind; i++)
		{
			runtime.release((Func *) JIT_code[i].fn);
		}
	}

	return (0);
}

				