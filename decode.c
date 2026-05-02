#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

Status do_decoding(DecodeInfo *decInfo)
{
    if(open_decode_files(decInfo) != e_success) // opening files to decode
    {
        printf("ERROR: Failed to open files\n");
        return e_failure;
    } else {
        printf("SUCCESS: Opened files successfully\n");
    }

    fseek(decInfo->fptr_stego, 54, SEEK_SET);//skipping header filess

    if(decode_magic_string(MAGIC_STRING, decInfo) != e_success) // decode the magic string
    {
        printf("ERROR: Magic string mismatch\n");
        return e_failure;
    } else {
        printf("SUCCESS: Magic string decoded correctly\n");
    }

    if(decode_secret_file_extn_size(decInfo) != e_success) // decoding secret files extenmsion size 
    {
        printf("ERROR: Failed to decode secret file extension size\n");
        return e_failure;
    } else {
        printf("SUCCESS: Decoded secret file extension size\n");
    }
     if(decode_secret_file_extn(decInfo) != e_success) // decoding secret file extension
     {
        printf("ERROR: Failed to decode secret file extension\n");
        return e_failure;
    } 
    else 
    {
        printf("SUCCESS: Decoded secret file extension\n");
    }
    if(decode_secret_file_size(decInfo) != e_success) //decoding secret file
    {
        printf("ERROR: Failed to decode secret file size\n");
        return e_failure;
    } else {
        printf("SUCCESS: Decoded secret file size\n");
    }
   
    if(decode_secret_file_data(decInfo) != e_success) {
        printf("ERROR: Failed to decode secret file data\n");
        return e_failure;
    } else {
        printf("SUCCESS: Secret file data decoded successfully\n");
    }

   // printf("\n*** DECODING COMPLETED SUCCESSFULLY ***\n");
    return e_success;
}
    
Status open_decode_files(DecodeInfo *decInfo)
{
              
    decInfo->fptr_stego = fopen(decInfo->stego_image_fname, "rb");//reads in binary
    // Do Error handling
    if (decInfo->fptr_stego== NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

    	return e_failure;
    }
    decInfo->fptr_output = fopen(decInfo->output_fname, "wb");//writes in binary 
    // Do Error handling
    if (decInfo->fptr_output == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_fname);

    	return e_failure;
    }
    return e_success;
}
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)//decoding magic string
{
    char buffer[8];
    char decod_char;
    int len = strlen(MAGIC_STRING);

    for (int i = 0; i < len; i++)
    {
        size_t bytesRead = fread(buffer, 1, 8, decInfo->fptr_stego); 
        if (bytesRead != 8)
        {
            printf("ERROR: Not enough bytes to decode magic string\n");
            return e_failure;
        }

        decode_byte_from_lsb(&decod_char, buffer);

        if (decod_char != magic_string[i])//checking the magic string
            return e_failure;
    }
    return e_success;
}
Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    *data = 0;
    for (int i = 0; i < 8; i++)
    {
    *data = (*data << 1) | (image_buffer[i] & 1);// decoding lgoic 
    }
    return e_success;
}
Status decode_int_from_lsb(int *value, char *image_buffer)
{
    *value = 0;
    for (int i = 0; i < 32; i++)
    {
    *value = (*value << 1) | (image_buffer[i] & 1);// for int same did 32 bits because of integer
    }
    return e_success;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char buffer[32]; 
    fread(buffer, 32, 1, decInfo->fptr_stego);
    int ext_size;
    decode_int_from_lsb(&ext_size, buffer);
    decInfo->ext_size = ext_size;
    return e_success;
}
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    decInfo->extn[decInfo->ext_size] = '\0';  
    char buffer[8];
    for (int i = 0; i < decInfo->ext_size; i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_stego);
        decode_byte_from_lsb(&decInfo->extn[i], buffer);
    }
    return e_success;
}
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char buffer[32];  
    fread(buffer, 32, 1, decInfo->fptr_stego);
    int secret_file_size;
    decode_int_from_lsb(&secret_file_size, buffer);
    decInfo->size_secret_file = secret_file_size;
    return e_success;
}
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buffer[8];  
    for (int i = 0; i < decInfo->size_secret_file; i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_stego);
        char decoded_char;
        decode_byte_from_lsb(&decoded_char, buffer);
        fputc(decoded_char, decInfo->fptr_output);
    }
    return e_success;
}



