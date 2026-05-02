#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>
#include "types.h" 


typedef struct _DecodeInfo
{
    char *stego_image_fname;
    char *output_fname;

    FILE *fptr_stego;
    FILE *fptr_output;

    
    int ext_size;
    int size_secret_file;
    char extn[10];

} DecodeInfo;

Status do_decoding(DecodeInfo *decInfo);
Status open_decode_files(DecodeInfo *decInfo);

/* Decode magic string */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Decode secret file extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Helper functions */
Status decode_byte_from_lsb(char *data, char *image_buffer);


Status decode_int_from_lsb(int *value, char *image_buffer);


#endif