/* plplatf.c   Platform-related utils */

#include "string_.h"
#include "gdebug.h"
#include "gsio.h"
#include "gstypes.h"
#include "gsmemory.h"
#include "gsstruct.h"
#include "gp.h"
#include "gslib.h"
#include "plplatf.h"


/* ------------- Platform de/init --------- */
void
pl_platform_init(FILE *debug_out)
{
    gp_init();
    gs_lib_init(gs_stderr);
}

void
pl_platform_dnit(int exit_status)
{
    gs_lib_finit(exit_status, 0);
}


/* ---------------- Stubs ---------------- */

/* Stubs for GC */
const gs_ptr_procs_t ptr_struct_procs = { NULL, NULL, NULL };
const gs_ptr_procs_t ptr_string_procs = { NULL, NULL, NULL };
const gs_ptr_procs_t ptr_const_string_procs = { NULL, NULL, NULL };
void * /* obj_header_t * */
gs_reloc_struct_ptr(const void * /* obj_header_t * */ obj, gc_state_t *gcst)
{	return (void *)obj;
}
void
gs_reloc_string(gs_string *sptr, gc_state_t *gcst)
{
}
void
gs_reloc_const_string(gs_const_string *sptr, gc_state_t *gcst)
{
}

/* Stub for abnormal termination */
void
gs_exit(int exit_status)
{	pl_platform_dnit(exit_status);
	pl_exit(exit_status);	/* must be implemeted by caller */
}
