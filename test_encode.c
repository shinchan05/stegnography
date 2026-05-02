#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;  

    if(argc < 3)
    {
        printf("Not enough arguments passed after ./a.out <.bmp> file is required \n");
        return 0;
    }
    if(argc < 4)
    {
        printf(" NOT ENOUGH ARGUMENTS PASSED AFTER ./a.out  <secret .txt> is required  \n");
        return 0;
    }
    if(argv[4]==NULL)
    {
     encInfo.stego_image_fname = "stego_img.bmp";   
    }
    else 
    {
        encInfo.stego_image_fname = argv[4];
    }
    if(check_operation_type(argv) == e_encode)
    {
        encInfo.src_image_fname = argv[2];
        encInfo.secret_fname = argv[3];

        if(read_and_validate_encode_args(argv) == e_success)
        {
            int ret = do_encoding(&encInfo);
            if(ret == e_success)
                printf("------Encoding successful------\n");
            else
                printf("------Encoding failed--------\n");
        }

    }
    else if(check_operation_type(argv) == e_decode)
    {
        decInfo.stego_image_fname = argv[2];
        decInfo.output_fname = argv[3];
         if(read_and_validate_encode_args(argv) == e_success)
        {
             int ret = do_decoding(&decInfo);  
        if(ret == e_success)
            printf("-------Decoding successful-------\n");
        else
            printf("-------Decoding failed-------\n");
        }
    }
    return 0;
}

Status read_and_validate_encode_args(char *argv[]) // Validates that input files have correct .bmp and .txt extensions.
{
    char str[10]=".bmp";
    char *ptr;
    ptr=strstr(argv[2],str);
    if((ptr != NULL && strcmp(ptr,str)==0)&&strlen(ptr) == strlen(str))
    {
       printf("---------validation for bmp file complete--------\n");
       
    }
    else
    {
    printf("Enter the .bmp file");
    return e_failure;
    }
    char *ptr1=strstr(argv[3],".txt");
    if((ptr1 != NULL && strcmp(ptr1,".txt")==0)&&strlen(ptr1) == strlen(".txt"))
    {
       printf("---------validation complete--------\n");
       return e_success;
    }
    else 
    printf("Enter the .txt file\n");
    return e_failure;
}
OperationType check_operation_type(char *argv[])// Checks whether user selected -e (encode) or -d (decode).
{
    if(strcmp(argv[1],"-e")==0)
    {
        printf("----------Selected Encoding------------\n");
        return  e_encode;
    }
    else if(strcmp(argv[1],"-d")==0)
    {
        //printf("------------Decoding-------------------\n");
        return e_decode;
    }
    else
    {
        printf("---Eroor : enter valid operation after ./a.out Ex -e or -d\n");
        return e_unsupported;
    }
}
Status do_encoding(EncodeInfo *encInfo)// Main  function that performs all encoding steps.
{
 
  if(open_files(encInfo)==e_success)// Opens source image, secret file, and stego image file.
  {
    printf("Open file is successful\n");
  }
  else
  {
    printf("Open File is failure\n");
    return e_failure;
  }
   if(check_capacity(encInfo)==e_success)// Checks if BMP has enough capacity to embed secret data.
   {
    printf("capicity in range\n");
    //return e_success;
   }
   else 
   {
    return e_failure;
   }
   if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
   {
    
       printf("\nCopying the header file to destination image\n");
      // return e_success;
   }
   else 
   {
    printf("\nCopying not successful\n");
    return e_failure;
   }
    if(encode_magic_string( MAGIC_STRING, encInfo)==e_success)
    {
        printf(" magic string is succesfully matched\n");
    }
    else 
    {
        printf("magic string is not matched\n");
        return e_failure;
    }
    if(encode_secret_file_extn(".txt",encInfo)==e_success)
    {
        printf("Encoding secret flile extension\n");
    }
    else
    {
        printf("Encoding secret file extension failed");
        return e_failure;
    }
    if( encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)
    {
        printf("Encoding secret file size\n");
    }
    else
    {
            printf("Encoding secret file size not done");
            return e_failure;
    }
    if(encode_secret_file_data(encInfo)==e_success)
    {
        printf("Started encoding secret file data\n");

    }
    else 
    {
        printf("encoding secret fiel data failed ");
        return e_failure;
    }
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success) 
    {
        printf("copying image files\n");
    }
    else
    {
        printf("copying image not done");
    }
    
return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
Status check_capacity(EncodeInfo *encInfo)
{
   encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
   encInfo->size_secret_file= get_file_size(encInfo->fptr_secret);
    if(encInfo->image_capacity>16+32+32+32+(encInfo->size_secret_file*8))
    {
        return  e_success;
    }
    else 
    return  e_failure;

}
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width of bmp file = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height of bmp file  = %u\n", height);

    // Return image capacity
    return width * height * 3;
}
uint get_file_size(FILE *fptr)// Returns the size of the given file using ftell().

{
  fseek(fptr,0,SEEK_END);
  printf("Secret file size is %lu\n",ftell(fptr));
  return ftell(fptr);
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)// Copies 54-byte BMP header from source to destination.
{
    char arr[54];
    fseek(fptr_src_image,0,SEEK_SET);
    fread(arr,54,1,fptr_src_image);
    fwrite(arr,54,1,fptr_dest_image);
    printf("\n copying the header file is successful\n");
    return e_success;
}
Status encode_magic_string( char *magic_string, EncodeInfo *encInfo)// Encodes MAGIC_STRING into image to verify during decoding
{
     
    if(encode_data_to_image(MAGIC_STRING,strlen(MAGIC_STRING),encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
    {
          printf("Encoding data to image\n");
          return e_success;
    }
    else 
    {
        printf("Encoding data to image failed \n");
        return e_failure;
    }
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)// Encodes the length and characters of file extension.
{ 
    int ext_len = strlen(file_extn);
    if (encode_secret_file_size(ext_len, encInfo) != e_success)// Encodes size of the secret file as 4 bytes.
        return e_failure;

    /* Now encode extension characters */
    if (encode_data_to_image(file_extn, ext_len,
            encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
        return e_failure;

    return e_success;

}
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    unsigned char size_buffer[4];

    size_buffer[0] = (file_size >> 24) & 0xFF;
    size_buffer[1] = (file_size >> 16) & 0xFF;
    size_buffer[2] = (file_size >> 8) & 0xFF;
    size_buffer[3] = file_size & 0xFF;

   if(encode_data_to_image((char*)size_buffer, 4, encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
    {
          printf("Encoding data to image\n");
          return e_success;
    }
    else 
    {
        printf("Encoding data to image failed \n");
        return e_failure;
    }
}
Status encode_secret_file_data(EncodeInfo *encInfo)// Encodes actual secret file contents byte by byte.
{
    char arr[8];
    int ch;
    fseek(encInfo->fptr_secret,0,SEEK_SET);
    while ((ch = fgetc(encInfo->fptr_secret)) != EOF)
    {
        fread(arr, 8, 1, encInfo->fptr_src_image);
        if (encode_byte_to_lsb((char)ch, arr) != e_success)
            return e_failure;
        fwrite(arr, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)// Encodes each byte of data into LSBs of image bytes.
{
    char str[8];
    for(int i=0;i<size;i++)
    {
        fread(str,8,1,fptr_src_image);
        if(encode_byte_to_lsb(data[i],str)!=e_success)
        {
          return e_failure;    
        }
        fwrite(str,8,1,fptr_stego_image);
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)// Replaces LSB of image bytes with bits of given data byte.
{
    //first we clear the lsb bits 
    for(int i=0;i<8;i++)
    {
        char bit = (data >> (7 - i)) & 1;  
        image_buffer[i]=(image_buffer[i] & 0xFE) | bit;
    }
    return e_success;
}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest) // Copies remaining BMP pixel data unchanged to stego image.
{
   char arr[512];
   uint var;
    // Continue copying until end of file
    while ((var = fread(arr, 1, sizeof(arr), fptr_src)) > 0)
    {
        fwrite(arr, 1, var, fptr_dest);
    }
    return e_success;
}


