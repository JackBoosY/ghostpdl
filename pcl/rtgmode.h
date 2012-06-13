/* Copyright (C) 2001-2012 Artifex Software, Inc.
   All Rights Reserved.

   This software is provided AS-IS with no warranty, either express or
   implied.

   This software is distributed under license and may not be copied,
   modified or distributed except as expressly authorized under the terms
   of the license contained in the file LICENSE in this distribution.

   Refer to licensing information at http://www.artifex.com or contact
   Artifex Software, Inc.,  7 Mt. Lassen Drive - Suite A-134, San Rafael,
   CA  94903, U.S.A., +1(415)492-9861, for further information.
*/


/* rtgmode.h - interface to PCL graphics (raster) mode */

#ifndef rtgmode_INCLUDED
#define rtgmode_INCLUDED

#include "rtrstst.h"
#include "pcstate.h"
#include "pcommand.h"

/*
 * Types of entry into graphics mode. Note that implicit entry is distinct
 * from any of the explicit modes.
 */
typedef enum {
    NO_SCALE_LEFT_MARG = 0,
    NO_SCALE_CUR_PT = 1,
    SCALE_LEFT_MARG = 2,
    SCALE_CUR_PTR = 3,
    IMPLICIT = 100
} pcl_gmode_entry_t;

/* enter raster graphics mode */
int     pcl_enter_graphics_mode(
    pcl_state_t *       pcs,
    pcl_gmode_entry_t   mode
);

/* exit raster graphics mode */
int pcl_end_graphics_mode(pcl_state_t * pcs);

extern  const pcl_init_t    rtgmode_init;
extern  const pcl_init_t    rtlrastr_init;

#endif			/* rtgmode_INCLUDED */
