/* Copyright (C) 1991, 2000 Aladdin Enterprises.  All rights reserved.
  
  This file is part of AFPL Ghostscript.
  
  AFPL Ghostscript is distributed with NO WARRANTY OF ANY KIND.  No author or
  distributor accepts any responsibility for the consequences of using it, or
  for whether it serves any particular purpose or works at all, unless he or
  she says so in writing.  Refer to the Aladdin Free Public License (the
  "License") for full details.
  
  Every copy of AFPL Ghostscript must include a copy of the License, normally
  in a plain ASCII text file named PUBLIC.  The License grants you the right
  to copy, modify and redistribute AFPL Ghostscript, but only under certain
  conditions described in the License.  Among other things, the License
  requires that the copyright notice and this notice be preserved on all
  copies.
*/

/*$Id$ */
/* Font API interface */

#include "iplugin.h"

typedef int FracInt; /* A fractional integer with statically unknown number of fraction bits. 
                        The number of bits depends on plugin and is being specified in
                        FAPI_server::frac_shift.
                     */
typedef int FAPI_retcode;

typedef enum {
    FAPI_FONT_FEATURE_FontMatrix,
    FAPI_FONT_FEATURE_UniqueID,
    FAPI_FONT_FEATURE_BlueScale,
    FAPI_FONT_FEATURE_Weight,
    FAPI_FONT_FEATURE_ItalicAngle,
    FAPI_FONT_FEATURE_IsFixedPitch,
    FAPI_FONT_FEATURE_UnderLinePosition,
    FAPI_FONT_FEATURE_UnderlineThickness,
    FAPI_FONT_FEATURE_FontType,
    FAPI_FONT_FEATURE_FontBBox,
    FAPI_FONT_FEATURE_BlueValues_count, 
    FAPI_FONT_FEATURE_BlueValues,
    FAPI_FONT_FEATURE_OtherBlues_count, 
    FAPI_FONT_FEATURE_OtherBlues,
    FAPI_FONT_FEATURE_FamilyBlues_count, 
    FAPI_FONT_FEATURE_FamilyBlues,
    FAPI_FONT_FEATURE_FamilyOtherBlues_count, 
    FAPI_FONT_FEATURE_FamilyOtherBlues,
    FAPI_FONT_FEATURE_BlueShift,
    FAPI_FONT_FEATURE_BlueFuzz,
    FAPI_FONT_FEATURE_StdHW,
    FAPI_FONT_FEATURE_StdVW,
    FAPI_FONT_FEATURE_StemSnapH_count, 
    FAPI_FONT_FEATURE_StemSnapH,
    FAPI_FONT_FEATURE_StemSnapV_count, 
    FAPI_FONT_FEATURE_StemSnapV,
    FAPI_FONT_FEATURE_ForceBold,
    FAPI_FONT_FEATURE_LanguageGroup,
    FAPI_FONT_FEATURE_lenIV,
    FAPI_FONT_FEATURE_Subrs_count,
    FAPI_FONT_FEATURE_Subrs_total_size,
    FAPI_FONT_FEATURE_TT_size
} fapi_font_feature;

typedef struct {
    int char_code;
    bool is_glyph_index; /* true if char_code contains glyph index */
    const unsigned char *char_name; /* to be used exclusively with char_code. */
    unsigned int char_name_length;
} FAPI_char_ref;

typedef struct FAPI_font_s FAPI_font;
struct FAPI_font_s {
    /* server's data : */
    void *server_font_data;
    bool need_decrypt;
    /* client's data : */
    const char *font_file_path;
    int subfont;
    bool is_type1; /* Only for non-disk fonts; dirty for disk fonts. */
    bool is_cid;
    void *client_font_data;
    void *client_font_data2;
    const void *client_char_data;
    unsigned short (*get_word )(FAPI_font *ff, fapi_font_feature var_id, int index);
    unsigned long  (*get_long )(FAPI_font *ff, fapi_font_feature var_id, int index);
    float          (*get_float)(FAPI_font *ff, fapi_font_feature var_id, int index);
    unsigned short (*get_subr) (FAPI_font *ff, int index,     byte *buf, ushort buf_length);
    unsigned short (*get_glyph)(FAPI_font *ff, int char_code, byte *buf, ushort buf_length);
    unsigned short (*serialize_tt_font)(FAPI_font *ff, void *buf, int buf_size);
};

typedef struct FAPI_path_s FAPI_path;
struct FAPI_path_s {
    void *olh; /* Client's data. */
    int shift;
    int (*moveto   )(FAPI_path *, FracInt, FracInt);
    int (*lineto   )(FAPI_path *, FracInt, FracInt);
    int (*curveto  )(FAPI_path *, FracInt, FracInt, FracInt, FracInt, FracInt, FracInt);
    int (*closepath)(FAPI_path *);
};

typedef struct { /* 1bit/pixel only, rows are byte-aligned. */
    void *p;
    int width, height, line_step;
} FAPI_raster;

typedef struct FAPI_metrics_s {
    int bbox_x0, bbox_y0, bbox_x1, bbox_y1; /* design units */
    int escapement; /* design units */
    int orig_x, orig_y; /* origin, 1/16s pixel */
    int em_x, em_y; /* design units */
    FracInt esc_x, esc_y; /* escapement, device units */
} FAPI_metrics;

#ifndef FAPI_server_DEFINED
#define FAPI_server_DEFINED
typedef struct FAPI_server_s FAPI_server;
#endif

struct FAPI_server_s {
    i_plugin_instance ig;
    int frac_shift; /* The number of fractional bits in coordinates. */
    FAPI_retcode (*ensure_open)(FAPI_server *server);
    FAPI_retcode (*get_scaled_font)(FAPI_server *server, FAPI_font *ff, int subfont, const FracInt matrix[6], const FracInt HWResolution[2], const char *xlatmap);
    FAPI_retcode (*get_decodingID)(FAPI_server *server, FAPI_font *ff, const char **decodingID);
    FAPI_retcode (*get_font_bbox)(FAPI_server *server, FAPI_font *ff, int BBox[4]);
    FAPI_retcode (*can_retrieve_char_by_name)(FAPI_server *server, FAPI_font *ff, int *result);
    FAPI_retcode (*outline_char)(FAPI_server *server, FAPI_font *ff, FAPI_char_ref *c, FAPI_path *p, FAPI_metrics *metrics);
    FAPI_retcode (*get_char_width)(FAPI_server *server, FAPI_font *ff, FAPI_char_ref *c, FracInt *wx, FracInt *wy);
    FAPI_retcode (*get_char_raster_metrics)(FAPI_server *server, FAPI_font *ff, FAPI_char_ref *c, FAPI_metrics *metrics);
    FAPI_retcode (*get_char_raster)(FAPI_server *server, FAPI_font *ff, FAPI_char_ref *c, FAPI_raster *r);
    FAPI_retcode (*release_char_raster)(FAPI_server *server, FAPI_font *ff, FAPI_char_ref *c);
    FAPI_retcode (*release_typeface)(FAPI_server *server, void *server_font_data);
    /*  Some people get confused with terms "font cache" and "character cache".
        "font cache" means a cache for scaled font objects, which mainly
        keep the font header information and rules for adjusting it to specific raster.
        "character cahce" is a cache for specific character outlines and/or character rasters.
    */
    /*  get_scaled_font opens a typeface with a server and scales it according to CTM and HWResolution.
        This creates a server's scaled font object.
        Since UFST doesn't provide a handle to this object,
        we need to build the data for it and call this function whenever scaled font data may change.
        The server must cache scaled fonts internally.
        Note that FreeType doesn't provide internal font cache,
        so the bridge must do.
    */
    /*  GS cannot provide information when a scaled font to be closed.
        Therefore we don't provide close_scaled_font function in this interface.
        The server must cache scaled fonts, and close ones which were
        not in use during a long time.
    */
    /*  With current version of UFST server, 
        get_char_raster_metrics leaves some data kept by the server,
	so get_char_raster uses them and release_char_raster releases them.
        Therefore calls from GS to these functions must not 
        interfer with different characters.
    */
};

