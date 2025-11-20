#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size from BMP */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    fseek(fptr_image, 18, SEEK_SET);
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    long size = ftell(fptr);
    rewind(fptr);
    printf("File size = %ld bytes\n",size);
    return size;
}
/* 
* Get File pointers for i/p and o/p files 
* Inputs: Src Image file, Secret file and 
* Stego Image file * Output: FILE pointer for above files 
* Return Value: e_success or e_failure, on file errors
*/

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if (strcmp(strstr(argv[2], ".bmp"), ".bmp") == 0)
    {
        printf("INFO : BMP file Validated Successfully\n");
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }

    if (strcmp(strstr(argv[3], ".txt"), ".txt") == 0)
    {
        printf("INFO : Secret file Validated Successfully\n");
        encInfo->secret_fname = argv[3];
    }
    else
    {
        return e_failure;
    }

    if (argv[4] != NULL)
    {
        encInfo->dest_image_fname = argv[4];
    }
    else
    {
        encInfo->dest_image_fname = "default.bmp";
    }
    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open %s\n", encInfo->src_image_fname);
        return e_failure;
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open %s\n", encInfo->secret_fname);
        return e_failure;
    }

    encInfo->fptr_dest_image = fopen(encInfo->dest_image_fname, "w");
    if (encInfo->fptr_dest_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open %s\n", encInfo->dest_image_fname);
        return e_failure;
    }

    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    if (encInfo->image_capacity > (16 + 32 + 32 + 32 + encInfo->size_secret_file * 8))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char image_buffer[54];
    fseek(fptr_src_image, 0, SEEK_SET);
    fread(image_buffer, 54, 1, fptr_src_image);
    fwrite(image_buffer, 54, 1, fptr_dest_image);
    long src_offset = ftell(fptr_src_image);
    long dest_offset = ftell(fptr_dest_image);

    if (src_offset != dest_offset)
    {
        printf("BMP header failed to copy.\n");
        return e_failure;
    }
    printf("BMP header copied successfully.\n");
    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char image_buffer[8];
    int size = strlen(magic_string);
    int i;

    for (i = 0; i < size; i++)
    {
        fread(image_buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i], image_buffer);
        fwrite(image_buffer, 8, 1, encInfo->fptr_dest_image);
    }

    long src_offset = ftell(encInfo->fptr_src_image);
    long dest_offset = ftell(encInfo->fptr_dest_image);

    if (src_offset != dest_offset)
    {
        printf("Magic string failed to encode.\n");
        return e_failure;
    }
    printf("Magic string encoded successfully.\n");
    return e_success;
}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char image_buffer[32];

    fread(image_buffer, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(size, image_buffer);
    fwrite(image_buffer, 32, 1, encInfo->fptr_dest_image);

    //checking offset
    long src_offset = ftell(encInfo->fptr_src_image);
    long dest_offset = ftell(encInfo->fptr_dest_image);

    if (src_offset != dest_offset)
    {
        printf("Secret file extension size failed to encode.\n");
        return e_failure;
    }
    printf("Secret file extension size encoded successfully.\n");
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char image_buffer[8];
    int size = strlen(file_extn);
    int i;

    for (i = 0; i < size; i++)
    {
        fread(image_buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], image_buffer);
        fwrite(image_buffer, 8, 1, encInfo->fptr_dest_image);
    }

    //checking offset
    long src_offset = ftell(encInfo->fptr_src_image);
    long dest_offset = ftell(encInfo->fptr_dest_image);

    if (src_offset != dest_offset)
    {
        printf("Secret file extension failed to encode.\n");
        return e_failure;
    }
    printf("Secret file extension encoded successfully.\n");
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char image_buffer[32];
    fread(image_buffer, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, image_buffer);
    fwrite(image_buffer, 32, 1, encInfo->fptr_dest_image);

    long src_offset = ftell(encInfo->fptr_src_image);
    long dest_offset = ftell(encInfo->fptr_dest_image);

    if (src_offset != dest_offset)
    {
        printf("Secret file size failed to encode.\n");
        return e_failure;
    }
    printf("Secret file size encoded successfully.\n");
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    rewind(encInfo->fptr_secret);
    char secret_data[encInfo->size_secret_file];
    char image_buffer[8];
    fread(secret_data, 1, encInfo->size_secret_file, encInfo->fptr_secret);

    for (long i = 0; i < encInfo->size_secret_file; i++)
    {
        fread(image_buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(secret_data[i], image_buffer);
        fwrite(image_buffer, 8, 1, encInfo->fptr_dest_image);
    }

    long src_offset = ftell(encInfo->fptr_src_image);
    long dest_offset = ftell(encInfo->fptr_dest_image);
    if (src_offset != dest_offset)
    {
        printf("Secret file data failed to encode.\n");
        return e_failure;
    }
    printf("Secret file data encoded successfully.\n");
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), fptr_src)) != 0)
    {
        fwrite(buffer, 1, bytesRead, fptr_dest);
    }
    //printf("Remaining data copied succesfully\n");
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    int i, bit;
    for (i = 0; i < 8; i++)
    {
        image_buffer[i] = image_buffer[i] & 0xFE; //clear the lsb of image_buffer
        bit = (data >> (7 - i)) & 1;  //get the one from one data
        image_buffer[i] = image_buffer[i] | bit; //set the bit
    }
    return e_success;
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    int i, bit;
    for (i = 0; i < 32; i++)
    {
        imageBuffer[i] = imageBuffer[i] & 0xFE;   //clear
        bit = (size >> (31 - i)) & 1;             //get
        imageBuffer[i] = imageBuffer[i] | bit;    //set
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_success)
    {
        printf("INFO : Open File is Success\n");

        if (check_capacity(encInfo) == e_success)
        {
            printf("INFO : Check capacity is Success\n");

            if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_dest_image) == e_success)
            {
                printf("INFO : BMP header copied successfully\n");

                if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                {
                    printf("INFO : Encoded Magic string successfully\n");
                    strcpy(encInfo->extn_secret_file, strstr(encInfo->secret_fname, "."));

                    if (encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_success)
                    {
                        printf("INFO : Encoded extension size successfully\n");

                        if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
                        {
                            printf("INFO : Encoded secret file extension successfully\n");

                            if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
                            {
                                printf("INFO : Encoded secret file size successfully\n");

                                if (encode_secret_file_data(encInfo) == e_success)
                                {
                                    printf("INFO : Encoded secret file data successfully\n");

                                    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_dest_image) == e_success)
                                    {
                                        printf("INFO : Remaining image data copied successfully\n");
                                    }
                                    else
                                    {
                                        printf("INFO : Failed to copy image data\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf("INFO : Failed to encode secret file data\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("INFO : Failed to encode secret file size\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("INFO : Failed to encode secret file extension\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("INFO : Encoding extension size failed\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("INFO : Encoding Magic string failed\n");
                    return e_failure;
                }
            }
            else
            {
                printf("INFO : BMP header copy failed\n");
                return e_failure;
            }
        }
        else
        {
            printf("INFO : Check capacity failed\n");
            return e_failure;
        }
    }
    else
    {
        printf("INFO : Open file failed\n");
        return e_failure;
    }
    return e_success;
}
