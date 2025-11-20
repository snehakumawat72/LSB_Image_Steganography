#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>
#include "types.h"

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Source image info */
    char *dest_image_fname;
    char *output_fname;

    FILE *fptr_dest_image;
    FILE *fptr_output;

    char image_data[MAX_IMAGE_BUF_SIZE];

    int extn_secret_file_size;
    char extn_secret_file_decode[MAX_FILE_SUFFIX];

    int size_secret_file_decode;

} DecodeInfo;

/* Read & validate arguments */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Open files */
Status open_files_decode(DecodeInfo *decInfo);

/* Main decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Magic string */
Status decode_magic_string(DecodeInfo *decInfo);

/* Secret extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Secret extension */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Secret data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode 1 byte from 8 LSB bits */
char decode_byte_from_lsb(char *image_buffer);

/* Decode integer size from 32 LSB bits */
int decode_size_from_lsb(char *image_buffer);

/* Read data chunk from image */
//Status decode_data_from_image(char *data, int size, DecodeInfo *decInfo);

/* Close files */
Status close_all_files_decode(DecodeInfo *decInfo);

#endif
