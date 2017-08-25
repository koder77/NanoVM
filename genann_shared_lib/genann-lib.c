/* This is a shared library for genann ANN, artifical neural networks library and Nano VM.
 *
 * Stefan Pietzonke 2017
 */

#include "genann.h"
#include "../include/host.h"

#undef JIT_CPP
#define JIT_CPP  1

#include "../include/global_d.h"
#include "../dll_shared_libs/nanovmlib.h"


void genann_read_ann (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
    
    S8 genann_var_ind;
    U1 genann_name[4095];
    
    FILE *input;
    
    if (stpull_s (threadnum, pthreads, stacksize, genann_name) != ERR_STACK_OK) printf ("genann_read_ann: ERROR on stack genann_name!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &genann_var_ind) != ERR_STACK_OK) printf ("genann_read_ann: ERROR on stack genann_var_ind!\n");
    
    input = fopen (genann_name, "r");
    if (input)
    {
        varlist[genann_var_ind].s_m = (genann *) genann_read (input);
        fclose (input);
        
        if (stpush_l (threadnum, pthreads, stacksize, 0) != ERR_STACK_OK) printf ("genann_read_ann: ERROR on stack return code OK!\n");
    }
    else
    {
        // FAIL
        if (stpush_l (threadnum, pthreads, stacksize, 1) != ERR_STACK_OK) printf ("genann_read_ann: ERROR on stack return code FAIL!\n");
    }
}

void genann_write_ann (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
    
    S8 genann_var_ind;
    U1 genann_name[4095];
    
    FILE *output;
    
    if (stpull_s (threadnum, pthreads, stacksize, genann_name) != ERR_STACK_OK) printf ("genann_write_ann: ERROR on stack genann_name!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &genann_var_ind) != ERR_STACK_OK) printf ("genann_write_ann: ERROR on stack genann_var_ind!\n");
    
    output = fopen (genann_name, "w");
    if (output)
    {
        genann_write (varlist[genann_var_ind].s_m, output);
        fclose (output);
        
        if (stpush_l (threadnum, pthreads, stacksize, 0) != ERR_STACK_OK) printf ("genann_write_ann: ERROR on stack return code OK!\n");
    }
    else
    {
        // FAIL
        if (stpush_l (threadnum, pthreads, stacksize, 1) != ERR_STACK_OK) printf ("genann_write_ann: ERROR on stack return code FAIL!\n");
    }
}
        

void genann_init_ann (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
    
    S8 inputs, hidden_layers, hidden, outputs, genann_var_ind;
    
    if (stpull_l (threadnum, pthreads, stacksize, &outputs) != ERR_STACK_OK) printf ("genann_init_ann: ERROR on stack outputs!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &hidden) != ERR_STACK_OK) printf ("genann_init_ann: ERROR on stack hidden!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &hidden_layers) != ERR_STACK_OK) printf ("genann_init_ann: ERROR on stack hidden_layers!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &inputs) != ERR_STACK_OK) printf ("genann_init_ann: ERROR on stack inputs!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &genann_var_ind) != ERR_STACK_OK) printf ("genann_init_ann: ERROR on stack genann_var_ind!\n");
    
    varlist[genann_var_ind].s_m = (genann *) genann_init (inputs, hidden_layers, hidden, outputs);
    if (varlist[genann_var_ind].s_m != NULL)
    {
             // assign ann memory pointer to VM memory pointer -> store it
        varlist[genann_var_ind].size = 1;       // dummy size
        varlist[genann_var_ind].dims = 0;
        
        // push return code 0 = OK, on stack
        if (stpush_l (threadnum, pthreads, stacksize, 0) != ERR_STACK_OK) printf ("genann_init_ann: ERROR on stack return code FAIL!\n");
    }
    else
    {
        // FAIL
        if (stpush_l (threadnum, pthreads, stacksize, 1) != ERR_STACK_OK) printf ("genann_init_ann: ERROR on stack return code OK!\n");
    }
}




void genann_train_ann (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
    
    genann *ann;
    
    S8 genann_var_ind, inputs_var_ind, desired_outputs_var_ind, learning_rate;
    
    if (stpull_l (threadnum, pthreads, stacksize, &learning_rate) != ERR_STACK_OK) printf ("genann_train_ann: ERROR on stack learning rate!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &desired_outputs_var_ind) != ERR_STACK_OK) printf ("genann_train_ann: ERROR on stack desired_outputs_var_ind!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &inputs_var_ind) != ERR_STACK_OK) printf ("genann_train_ann: ERROR on stack inputs_var_ind!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &genann_var_ind) != ERR_STACK_OK) printf ("genann_train_ann: ERROR on stack genann_var_ind!\n");
    
    // ann = (genann *) varlist[genann_var_ind].s_m;
    // genann_train (ann, varlist[inputs_var_ind].d_m, varlist[desired_outputs_var_ind].d_m, learning_rate);
    genann_train ((genann *) varlist[genann_var_ind].s_m, varlist[inputs_var_ind].d_m, varlist[desired_outputs_var_ind].d_m, learning_rate);
}

void genann_run_ann (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
    
    genann *ann;
    F8 *output;
    
    S8 genann_var_ind, inputs_var_ind, outputs_var_ind;
    
    if (stpull_l (threadnum, pthreads, stacksize, &outputs_var_ind) != ERR_STACK_OK) printf ("genann_run_ann: ERROR on stack outputs_var_ind!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &inputs_var_ind) != ERR_STACK_OK) printf ("genann_run_ann: ERROR on stack inputs_var_ind!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &genann_var_ind) != ERR_STACK_OK) printf ("genann_run_ann: ERROR on stack genann_var_ind!\n");
    
    output = genann_run ((genann *) varlist[genann_var_ind].s_m, varlist[inputs_var_ind].d_m);
    *varlist[outputs_var_ind].d_m = *output;
}

