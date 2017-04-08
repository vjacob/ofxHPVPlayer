/**********************************************************
* Holo_ToolSet
* http://github.com/HasseltVR/Holo_ToolSet
* http://www.uhasselt.be/edm
*
* Distributed under LGPL v2.1 Licence
* http ://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
**********************************************************/
#pragma once

#include <stdint.h>
#include <fstream>
#include <memory>

#define HPV_MAGIC 0x48505646

#define HPV_VERSION_0_0_0 0     /* The first release; simply wrotes every file concatenated into the output. */
#define HPV_VERSION_0_0_1 1     /* Added a bit more logic and storing the width, height and number of encoded frames. */
#define HPV_VERSION_0_0_2 2     /* Added frame rate field */
#define HPV_VERSION_0_0_3 3     /* Added DXT1 (no alpha) <-> DXT5 (alpha) implementation */
#define HPV_VERSION_0_0_4 4     /* Added some reserved field for later use */
#define HPV_VERSION_0_0_5 5     /* Added DXT5_SCALED_CoCgY for better quality */
#define HPV_VERSION_0_0_6 6     /* Added LZ4 compression/decompression stage */

#define HPV_MAX_SIDE_SIZE 8192
#define HPV_LZ4_COMPRESSION_LEVEL 9

// easy for if-statements
#define HPV_RET_ERROR 0
#define HPV_RET_ERROR_NONE 1

namespace HPV
{
    // This enum defines the different compression types.
    //
    // - RGB pixels can be compressed as
    //      * DXT1:         [RGB(A) input]: ok image quality, no alpha, 0.5 bpp
    //      * scaled DXT5:  [CoCg_Y input]: good image quality, no alpha, 1bpp
    //
    // - RGBA pixels can be compressed as:
    //      * DXT5:         [RGB(A) input]: ok image quality, alpha with good gradients, 1bpp
    enum class HPVCompressionType : std::uint32_t
    {
        HPV_TYPE_DXT1_NO_ALPHA = 0,
        HPV_TYPE_DXT5_ALPHA,
        HPV_TYPE_SCALED_DXT5_CoCg_Y,
        HPV_NUM_TYPES = 3
    };    
    
    // This struct defines the layout of the HPV header that exists in the beginning of any *.hpv video file
    struct HPVHeader
    {
        /* VERSION 1 */
        uint32_t magic;                 /* codename */
        uint32_t version;               /* internal version of API */
        uint32_t video_width;           /* the width of each video frame */
        uint32_t video_height;          /* the height of each video frame */
        uint32_t number_of_frames;      /* the total amount of frames in the file */
        
        /* VERSION 2 */
        uint32_t frame_rate;            /* frame rate at which the file should be played back */
        
        /* VERSION 3 */
        HPVCompressionType compression_type;      /* The used compression type */
        
        /* VERSION 4 - 6 */
        uint32_t crc_frame_sizes;       /* CRC for the frame size table */
        uint32_t reserved_1;
        uint32_t reserved_2;
    };

    // amount of defined header fields
    static const int amount_header_fields = 10;
    
    // swap big <-> little endian
    inline void swap_endian(uint32_t &val)
    {
        val = (val << 24) |
        ((val << 8) & 0x00ff0000) |
        ((val >> 8) & 0x0000ff00) |
        (val >> 24);
    }
} /* End HPV namespace */
