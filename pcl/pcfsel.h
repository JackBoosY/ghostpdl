/* Copyright (C) 1997 Aladdin Enterprises.  All rights reserved.
 * Unauthorized use, copying, and/or distribution prohibited.
 */

/* pcfsel.h */
/* Interface to PCL5 / HP-GL/2 font selection */

#ifndef pcfsel_INCLUDED
#  define pcfsel_INCLUDED

/* Recompute the font from the parameters if necessary. */
/* This is used by both PCL and HP-GL/2. */
int pcl_reselect_font(P2(pcl_font_selection_t *pfs, const pcl_state_t *pcls));

/*
 * Select a font by ID, updating the selection parameters.  Return 0
 * normally, 1 if no font was found, or an error code.  The pcl_state_t is
 * used only for the font and symbol set dictionaries.
 */
int pcl_select_font_by_id(P3(pcl_font_selection_t *pfs, uint id,
			     const pcl_state_t *pcls));

#endif				/* pcfsel_INCLUDED */
