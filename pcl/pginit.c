/* Copyright (C) 1996, 1997 Aladdin Enterprises.  All rights reserved.
   Unauthorized use, copying, and/or distribution prohibited.
 */

/* pginit.c */
/* Initialization and resetting for HP-GL/2. */
#include "std.h"
#include "pgmand.h"
#include "pginit.h"
#include "pgdraw.h"

/* called when there is a graphics error.  Keep a breakpoint on this function */
#ifdef DEBUG
void
hpgl_error()
{
	return;
}
#endif
/* ------ Internal procedures ------ */

/* Reset a set of font parameters to their default values. */
void
hpgl_default_font_params(pcl_font_selection_t *pfs)
{	pfs->params.symbol_set = 277;	/* Roman-8 */
	pfs->params.proportional_spacing = false;
	pfs->params.pitch_100ths = 9*100;
	pfs->params.height_4ths = (int)(11.5*4);
	pfs->params.style = 0;
	pfs->params.stroke_weight = 0;
	pfs->params.typeface_family = 48;	/* stick font */
	pfs->font = 0;		/* not looked up yet */
}

/* Reset a pen to its default color. */
void
hpgl_default_pen_color(hpgl_state_t *pgls, int pen)
{	static const byte default_colors[8][3] = {
	  {1,1,1} /*white*/, {0,0,0} /*black*/,
	  {1,0,0} /*red*/, {0,1,0} /*green*/,
	  {1,1,0} /*yellow*/, {0,0,1} /*blue*/,
	  {1,0,1} /*magenta*/, {0,1,1} /*cyan*/
	};
	int c;

	for ( c = 0; c < 3; ++c )
	  { pgls->g.pen_color[pen].rgb[c] =
	      (default_colors[pen][c] ? pgls->g.color_range[c].cmin :
	       pgls->g.color_range[c].cmax);
	  }
}

/* the following is not consistant with the general model as we
   usually depend upon calling the commands directly to update
   appropriate state variables, unfortunately we must guarantee a
   reasonable picture frame, anchor point, and plot size for the rest
   of the hpgl/2 code to function properly, and they must be provided
   all at once.  For example documented side effects of changing the
   vertical picture frame height are IP;IW;PM;PM2, but these commands
   do not make sense if the horizontal picture frame has never been
   set. */

private void
hpgl_default_coordinate_system(hpgl_state_t *pcls)
{
	pcls->g.plot_width = pcls->g.picture_frame.width = 
	  pcls->logical_page_width;
	pcls->g.plot_height = pcls->g.picture_frame.height = 
	  pcls->text_length * ((float)pcls->vmi / 48.0);
	pcls->g.picture_frame.anchor_point.x = pcls->left_margin;
	pcls->g.picture_frame.anchor_point.y = pcls->top_margin;
	pcls->g.plot_width = pcls->g.picture_frame.width;
}

	
/* Reset a fill pattern to solid fill.  The index is 1-origin, */
/* as for the RF command.  free = true means free the old pattern if any. */
void
hpgl_default_fill_pattern(hpgl_state_t *pgls, int index, bool free)
{	int index0 = index - 1;
	if ( free )
	  gs_free_object(pgls->memory, pgls->g.fill_pattern[index0].data,
			 "fill pattern(reset)");
	pgls->g.fill_pattern[index0].data = 0;
}

/* fill the state with a bogus value -- debug only.  HAS fixme -- GL/2
   interpreter actually depends on 0's being here.  It should not. */
private void
hpgl_clear_state(pcl_state_t *pcls)
{

#ifdef DEBUG
	memset(&pcls->g, 0, sizeof(pcls->g));
#endif
	return;
}

void
hpgl_do_reset(pcl_state_t *pcls, pcl_reset_type_t type)
{		/* pgframe.c (Chapter 18) */
	/* HAS we expect the pcl logical page size to be calculated in
           advance of an hpgl reset.  Alot of repetition here that can
           be done away with */
	hpgl_args_t hpgl_args;
	pcl_args_t pcl_args;

	if ( type & (pcl_reset_initial | pcl_reset_printer | pcl_reset_cold ))
	  {
	    hpgl_clear_state(pcls);
	    /* provide default anchor point, plot size and picture
               frame size */
	    hpgl_default_coordinate_system(pcls);
	    /* execute IN */
	    hpgl_args_setup(&hpgl_args);
	    hpgl_IN(&hpgl_args, pcls);

	  }   
	    
	if ( type & (pcl_reset_page_params) )
	  {
	    hpgl_default_coordinate_system(pcls);

	    hpgl_args_setup(&hpgl_args);
	    hpgl_IW(&hpgl_args, pcls);
	    
	    hpgl_args_set_int(&hpgl_args,0);
	    hpgl_PM(&hpgl_args, pcls);

	    hpgl_args_set_int(&hpgl_args,2);
	    hpgl_PM(&hpgl_args, pcls);

	    /* update the cursor position ?? */
	  }
	
	if ( type & (pcl_reset_picture_frame) )
	  {
	    hpgl_args_setup(&hpgl_args);
	    hpgl_IP(&hpgl_args, pcls);
	    
	    hpgl_args_setup(&hpgl_args);
	    hpgl_IW(&hpgl_args, pcls);

	    hpgl_args_set_int(&hpgl_args,0);
	    hpgl_PM(&hpgl_args, pcls);

	    hpgl_args_set_int(&hpgl_args,2);
	    hpgl_PM(&hpgl_args, pcls);

	    /* HAS pen to P1 */
	  }

	if (type & (pcl_reset_plot_size))
	  {
	    /* HAS reset picture frame scaling factors */
	  }
}

/* ------ Copy the HP-GL/2 state for macro call/overlay/exit. */

private int
hpgl_do_copy(pcl_state_t *psaved, const pcl_state_t *pcls,
  pcl_copy_operation_t operation)
{	if ( operation & pcl_copy_after )
	  { /* Don't restore the polygon buffer. */
	    /* (Copy from pcls to psaved.) */
	  }
	return 0;
}

/* Initialization */
const pcl_init_t pginit_init = {
  0, hpgl_do_reset, hpgl_do_copy
};
