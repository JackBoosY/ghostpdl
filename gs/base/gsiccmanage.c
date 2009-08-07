/* Copyright (C) 2001-2009 Artifex Software, Inc.
   All Rights Reserved.
  
   This software is provided AS-IS with no warranty, either express or
   implied.

   This software is distributed under license and may not be copied, modified
   or distributed except as expressly authorized under the terms of that
   license.  Refer to licensing information at http://www.artifex.com/
   or contact Artifex Software, Inc.,  7 Mt. Lassen Drive - Suite A-134,
   San Rafael, CA  94903, U.S.A., +1(415)492-9861, for further information.
*/
/*  GS ICC Manager.  Initial stubbing of functions.  */

#include "std.h"
#include "stdpre.h"
#include "gstypes.h"
#include "gsmemory.h"
#include "gsstruct.h"  
#include "scommon.h"
#include "strmio.h"
#include "gx.h"
#include "gxistate.h"
#include "gxcspace.h"
#include "gscms.h"
#include "gsiccmanage.h"
#include "gsicccache.h"


/* profile data structure */
/* profile_handle should NOT be garbage collected since it is allocated by the external CMS */

gs_private_st_ptrs2(st_gsicc_profile, cmm_profile_t, "gsicc_profile",
		    gsicc_profile_enum_ptrs, gsicc_profile_reloc_ptrs, buffer, name);

gs_private_st_ptrs7(st_gsicc_manager, gsicc_manager_t, "gsicc_manager",
		    gsicc_manager_enum_ptrs, gsicc_manager_profile_reloc_ptrs,
		    device_profile, device_named, default_gray, default_rgb,
                    default_cmyk, proof_profile, output_link); 

static const gs_color_space_type gs_color_space_type_icc = {
    gs_color_space_index_ICC,       /* index */
    true,                           /* can_be_base_space */
    true,                           /* can_be_alt_space */
    NULL                            /* This is going to be outside the norm. struct union*/

}; 



/* Get the size of the ICC profile that is in the buffer */
unsigned int gsicc_getprofilesize(unsigned char *buffer)
{

    return( (buffer[0]<<24) + (buffer[1]<<16) + (buffer[2]<<8) + buffer[3] );

}


/*  Initialize the CMS 
    Prepare the ICC Manager
*/
void 
gsicc_create()
{




}

/*  Shut down the  CMS 
    and the ICC Manager
*/
void 
gsicc_destroy()
{


}

/*  This computes the hash code for the
    ICC data and assigns the code
    and the profile to the appropriate
    member variable in the ICC manager */

int 
gsicc_set_profile(const gs_imager_state * pis, const char* pname, int namelen, gsicc_profile_t defaulttype)
{

    gsicc_manager_t *icc_manager = pis->icc_manager;
    cmm_profile_t *icc_profile;
    cmm_profile_t **manager_default_profile;
    stream *str;
    gs_memory_t *mem_gc = pis->memory; 
    int code;
 
    /* For now only let this be set once. 
       We could have this changed dynamically
       in which case we need to do some 
       deaallocations prior to replacing 
       it */

    switch(defaulttype){

        case DEFAULT_GRAY:

            manager_default_profile = &(icc_manager->default_gray);

            break;

        case DEFAULT_RGB:

            manager_default_profile = &(icc_manager->default_rgb);

            break;

        case DEFAULT_CMYK:
            
             manager_default_profile = &(icc_manager->default_cmyk);

             break;

        case PROOF_TYPE:

             manager_default_profile = &(icc_manager->proof_profile);

             break;

        case NAMED_TYPE:

             manager_default_profile = &(icc_manager->device_named);

             break;

        case LINKED_TYPE:

             manager_default_profile = &(icc_manager->output_link);

             break;

    }

    /* If it is not NULL then it has already been set.
       If it is different than what we already have then
       we will want to free it.  Since other imager states
       could have different default profiles, this is done via
       reference counting.  If it is the same as what we
       already have then we DONT increment, since that is
       done when the imager state is duplicated.  It could
       be the same, due to a resetting of the user params.
       To avoid recreating the profile data, we compare the 
       string names. */

    if ((*manager_default_profile) != NULL){

        /* Check if this is what we already have */

        icc_profile = *manager_default_profile;

        if ( namelen == icc_profile->name_length )
            if( memcmp(pname, icc_profile->name, namelen) == 0) return 0;

        rc_decrement(icc_profile,"gsicc_profile");

    }

    /* We need to do a bit of work here with 
       respect to path names. MJV ToDo.  */

    str = sfopen(pname, "rb", mem_gc);

   if (str != NULL){

        icc_profile = gsicc_profile_new(str, mem_gc, pname, namelen);
        code = sfclose(str);
        *manager_default_profile = icc_profile;

        /* Get the profile handle */

        icc_profile->profile_handle = gsicc_get_profile_handle_buffer(icc_profile->buffer);

        /* Compute the hash code of the profile. Everything in the ICC manager will have 
           it's hash code precomputed */

        gsicc_get_icc_buff_hash(icc_profile->buffer, &(icc_profile->hashcode));
        icc_profile->hash_is_valid = true;
         
        return(0);
       
    }
     
    return(1);

    
}


/*  This computes the hash code for the
    device profile and assigns the code
    and the profile to the DeviceProfile
    member variable in the ICC Manager
    This should really occur only one time.
    This is different than gs_set_device_profile
    which sets the profile on the output
    device */

void 
gsicc_set_device_profile(gs_imager_state *pis, gx_device * pdev, gs_memory_t * mem)
{
    gsicc_manager_t *icc_manager = pis->icc_manager;
    cmm_profile_t *icc_profile;
    stream *str;
    const char *profile = &(pdev->color_info.icc_profile[0]);
    int code;

    if (icc_manager->device_profile == NULL){

        /* Device profile in icc manager has not 
           yet been set. Lets do it. */

        /* Check if device has a profile.  This 
           should always be true, since if one was
           not set, we should have set it to the default. 
        */

        if (profile != '\0'){

            /* We need to do a bit of work here with 
               respect to path names. MJV ToDo.  */

            str = sfopen(profile, "rb", mem);

            if (str != NULL){
                icc_profile = gsicc_profile_new(str, mem, profile, strlen(profile));
                code = sfclose(str);
                icc_manager->device_profile = icc_profile;

                /* Get the profile handle */

                icc_profile->profile_handle = gsicc_get_profile_handle_buffer(icc_profile->buffer);

                /* Compute the hash code of the profile. Everything in the ICC manager will have 
                   it's hash code precomputed */

                gsicc_get_icc_buff_hash(icc_profile->buffer, &(icc_profile->hashcode));
                icc_profile->hash_is_valid = true;

            }
  

        }


    }
    
}



/*  This will occur only if the device did not
    return a profie when asked for one.  This
    will set DeviceProfile member variable to the
    appropriate default device.  If output device
    has more than 4 channels you need to use
    gsicc_set_device_profile with the appropriate
    ICC profile.  */

void 
gsicc_load_default_device_profile(int numchannels)
{



}


/* This is a special case of the functions gsicc_set_gray_profile
    gsicc_set_rgb_profile and gsicc_set_cmyk_profile. In
    this function the number of channels is specified and the 
    appropriate profile contained in Resources/Color/InputProfiles is used */

void 
gsicc_load_default_input_profile(int numchannels)
{


}




void 
gsicc_setbuffer_desc(gsicc_bufferdesc_t *buffer_desc,unsigned char num_chan,
    unsigned char bytes_per_chan,
    bool has_alpha,
    bool alpha_first,
    bool little_endian,
    bool is_planar,
    gsicc_colorbuffer_t buffercolor)
{
    buffer_desc->num_chan = num_chan;
    buffer_desc->bytes_per_chan = bytes_per_chan;
    buffer_desc->has_alpha = has_alpha;
    buffer_desc->alpha_first = alpha_first;
    buffer_desc->little_endian = little_endian;
    buffer_desc->is_planar = is_planar;
    buffer_desc->buffercolor = buffercolor;

}

/* Set the icc profile in the gs_color_space object */

int
gsicc_cs_profile(gs_color_space *pcs, cmm_profile_t *icc_profile, gs_memory_t * mem)
{

    if (pcs != NULL){

        if (pcs->cmm_icc_profile_data != NULL) {

            /* There is already a profile set there */
            /* free it and then set to the new one.  */
            /* should we check the hash code and retain if the same
               or place this job on the caller?  */

            rc_free_icc_profile(mem, (void*) pcs->cmm_icc_profile_data, "gsicc_cs_profile");
            pcs->cmm_icc_profile_data = icc_profile;

            return(0);

        } else {

            pcs->cmm_icc_profile_data = icc_profile;
            return(0);

        }
    }

    return(-1);

}



cmm_profile_t *
gsicc_profile_new(stream *s, gs_memory_t *memory, const char* pname, int namelen)
{


    cmm_profile_t *result;
    int code;
    char *nameptr;

    result = gs_alloc_struct(memory, cmm_profile_t, &st_gsicc_profile,
			     "gsicc_profile_new");

    if (result == NULL) return result; 

    if (namelen > 0){

        nameptr = gs_alloc_bytes(memory, namelen,
	   		     "gsicc_profile_new");
        memcpy(nameptr, pname, namelen);
        result->name = nameptr;

    } else {
    
        result->name = NULL;

    }

    result->name_length = namelen;

    code = gsicc_load_profile_buffer(result, s, memory);
    if (code < 0) {

        gs_free_object(memory, result, "gsicc_profile_new");
        return NULL;

    }

    rc_init_free(result, memory, 1, rc_free_icc_profile);

    result->profile_handle = NULL;
    result->hash_is_valid = false;

    return(result);

}



static void
rc_free_icc_profile(gs_memory_t * mem, void *ptr_in, client_name_t cname)
{
    cmm_profile_t *profile = (cmm_profile_t *)ptr_in;

    if (profile->rc.ref_count <= 1 ){

        /* Clear out the buffer if it is full */

        if(profile->buffer != NULL){

            gs_free_object(mem, profile->buffer, cname);
            profile->buffer = NULL;

        }

        /* Release this handle if it has been set */

        if(profile->profile_handle != NULL){

            gscms_release_profile(profile->profile_handle);
            profile->profile_handle = NULL;

        }

        /* Release the name if it has been set */

        if(profile->name != NULL){

            gs_free_object(mem,profile->name,cname);
            profile->name = NULL;
            profile->name_length = 0;

        }

        profile->hash_is_valid = 0;

	gs_free_object(mem, profile, cname);
        profile = NULL;

    }
}



gsicc_manager_t *
gsicc_manager_new(gs_memory_t *memory)
{

    gsicc_manager_t *result;

    /* Allocated in gc memory */
      
    result = gs_alloc_struct(memory, gsicc_manager_t, &st_gsicc_manager,
			     "gsicc_manager_new");

    if (result == NULL)
	return result;  

   result->default_cmyk = NULL;
   result->default_gray = NULL;
   result->default_rgb = NULL;
   result->device_named = NULL;
   result->output_link = NULL;
   result->device_profile = NULL;
   result->proof_profile = NULL;
   result->memory = memory;

   return(result);

}

/* Allocates and loads the icc buffer from the stream. */
static int
gsicc_load_profile_buffer(cmm_profile_t *profile, stream *s, gs_memory_t *memory)
{
       
    int                     num_bytes,profile_size;
    unsigned char           buffer_size[4];
    unsigned char           *buffer_ptr;


    buffer_ptr = &(buffer_size[0]);
    num_bytes = sfread(buffer_ptr,sizeof(unsigned char),4,s);
    profile_size = gsicc_getprofilesize(buffer_ptr);

    /* Allocate the buffer, stuff with the profile */

   buffer_ptr = gs_alloc_bytes(memory, profile_size,
					"gsicc_load_profile");

   if (buffer_ptr == NULL){
        return(-1);
   }

   srewind(s);
   num_bytes = sfread(buffer_ptr,sizeof(unsigned char),profile_size,s);

   if( num_bytes != profile_size){
    
       gs_free_object(memory, buffer_ptr, "gsicc_load_profile");
       return(-1);

   }

   profile->buffer = buffer_ptr;

   return(0);

}


gcmmhprofile_t
gsicc_get_profile_handle_buffer(unsigned char *buffer){

    gcmmhprofile_t profile_handle = NULL;
    unsigned int profile_size;

     if( buffer != NULL){
         profile_size = gsicc_getprofilesize(buffer);
         profile_handle = gscms_get_profile_handle_mem(buffer, profile_size);
         return(profile_handle);
     }

     return(0);

}
                                


 /*  If we have a profile handle in the color space already, then we use that.  
     If we do not have one, then we check if there is data in the buffer.  A
     handle is created from that data and also stored in the gs color space.
     If we do not have a handle nor any ICC data, then we will use data from 
     the ICC manager that is based upon the current color space. */


 gcmmhprofile_t
 gsicc_get_profile_handle(gs_color_space *gs_colorspace, gsicc_manager_t *icc_manager)
 {

     gcmmhprofile_t profilehandle = gs_colorspace->cmm_icc_profile_data->profile_handle;
     unsigned char *buffer = gs_colorspace->cmm_icc_profile_data->buffer;
     gs_color_space_index color_space_index = gs_color_space_get_index(gs_colorspace);

     if( profilehandle != NULL ){
        return(profilehandle);
     }

     if( buffer != NULL){
         return(gsicc_get_profile_handle_buffer(buffer));
     }

     /* Now get a colorspace handle based upon the colorspace type */

     switch( color_space_index ){

	case gs_color_space_index_DeviceGray:

            gs_colorspace->cmm_icc_profile_data = icc_manager->default_gray;
            return(gs_colorspace->cmm_icc_profile_data->profile_handle);

            break;

	case gs_color_space_index_DeviceRGB:

            gs_colorspace->cmm_icc_profile_data = icc_manager->default_rgb;
            return(gs_colorspace->cmm_icc_profile_data->profile_handle);

            break;

	case gs_color_space_index_DeviceCMYK:

            gs_colorspace->cmm_icc_profile_data = icc_manager->default_cmyk;
            return(gs_colorspace->cmm_icc_profile_data->profile_handle);

            break;

	case gs_color_space_index_DevicePixel:

            /* Not sure yet what our response to 
               this should be */

            return(0);

            break;

       case gs_color_space_index_DeviceN:

            /* If we made it to here, then we will need to use the 
               alternate colorspace */

            return(0);

            break;

       case gs_color_space_index_CIEDEFG:

           /* Need to convert to an ICC form */

            break;

        case gs_color_space_index_CIEDEF:

           /* Need to convert to an ICC form */

            break;

        case gs_color_space_index_CIEABC:

           /* Need to convert to an ICC form */

            break;

        case gs_color_space_index_CIEA:

           /* Need to convert to an ICC form */

            break;

        case gs_color_space_index_Separation:

            /* Caller should use named color path */

            return(0);

            break;

        case gs_color_space_index_Pattern:
        case gs_color_space_index_Indexed:

            /* Caller should use the base space for these */
            return(0);

            break;


        case gs_color_space_index_ICC:

            /* This should not occur, as the space
               should have had a populated profile handle */

            return(0);

            break;

     } 


    return(0);


 }

