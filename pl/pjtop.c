/* pjtop.c */
/* Interface to PJL-specific portions parser */

#include "string_.h"
#include "pjtop.h"

/* return the current setting of a pjl environment variable. */
pjl_envvar_t *
pjl_proc_get_envvar(
  pl_interp_instance_t   *pli,
  const char             *pjl_var
)
{
	return ((pjl_implementation_t *)pli->interp->implementation)
	 ->proc_get_envvar(pli, pjl_var);
}

/* compare a pjl environment variable to a string values. */
int
pjl_proc_compare(
  pl_interp_instance_t   *pli,
  const pjl_envvar_t     *s1,
  const char             *s2
)
{
	return ((pjl_implementation_t *)pli->interp->implementation)
	 ->proc_compare(pli, s1, s2);
}

/* map a pjl symbol set name to a pcl integer */
int
pjl_proc_map_pjl_sym_to_pcl_sym(
  pl_interp_instance_t   *pli,
  const pjl_envvar_t     *symname
)
{
	return ((pjl_implementation_t *)pli->interp->implementation)
	 ->proc_map_pjl_sym_to_pcl_sym(pli, symname);
}

/* pjl environment variable to integer. */
int
pjl_proc_vartoi(
  pl_interp_instance_t   *pli,
  const pjl_envvar_t     *s
)
{
	return ((pjl_implementation_t *)pli->interp->implementation)
	 ->proc_vartoi(pli, s);
}

/* pjl envioronment variable to float. */
floatp
pjl_proc_vartof(
  pl_interp_instance_t   *pli,
  const pjl_envvar_t     *s
)
{
	return ((pjl_implementation_t *)pli->interp->implementation)
	 ->proc_vartof(pli, s);
}

/* convert a pjl designated fontsource to a subdirectory pathname. */
char *
pjl_proc_fontsource_to_path(
  pl_interp_instance_t   *pli,
  const pjl_envvar_t     *fontsource
)
{
	return ((pjl_implementation_t *)pli->interp->implementation)
	 ->proc_fontsource_to_path(pli, fontsource);
}

/* get a pjl fontnumber for an internal pcl font. */
int
pjl_proc_get_pcl_internal_font_number(
  pl_interp_instance_t   *pli,
  const char             *filename
)
{
	return ((pjl_implementation_t *)pli->interp->implementation)
	 ->proc_get_pcl_internal_font_number(pli, filename);
}

/* Change to next highest priority font source. */
void
pjl_proc_set_next_fontsource(
  pl_interp_instance_t   *pli
)
{
	((pjl_implementation_t *)pli->interp->implementation)
	 ->proc_set_next_fontsource(pli);
}

/* tell pjl that a soft font is being deleted. */
int
pjl_proc_register_permanent_soft_font_deletion(
  pl_interp_instance_t   *pli,
  int                     font_number
)
{
	return ((pjl_implementation_t *)pli->interp->implementation)
	 ->proc_register_permanent_soft_font_deletion(pli, font_number);
}

/* request that pjl add a soft font and return a pjl font number for the font. */
int
pjl_proc_register_permanent_soft_font_addition(
  pl_interp_instance_t   *pli
)
{
	return ((pjl_implementation_t *)pli->interp->implementation)
	 ->proc_register_permanent_soft_font_addition(pli);
}

