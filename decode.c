#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    decInfo->dest_image_fname = argv[2];

    if (strstr(decInfo->dest_image_fname, ".bmp") == NULL)
    {
        return e_failure;
    }
    
    if (argv[3])
    {
        decInfo->output_fname = argv[3];
        if (!(strstr(argv[3], ".txt") || strstr(argv[3], ".c") ||strstr(argv[3], ".h")   || strstr(argv[3], ".sh")))
        {
            return e_failure;
        }
    }
    else
    {
        decInfo->output_fname = "decoded_output.txt";
    }
    printf("INFO : Open File is Success\n");
    return e_success;
}
Status open_files_decode(DecodeInfo *decInfo)
{
    decInfo->fptr_dest_image = fopen(decInfo->dest_image_fname, "r");
    if (decInfo->fptr_dest_image == NULL)
    {
        return e_failure;
    }

    decInfo->fptr_output = fopen(decInfo->output_fname, "w");
    if (decInfo->fptr_output == NULL)
    {
        return e_failure;
    }

    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    if (open_files_decode(decInfo) == e_failure)
    {
        printf("INFO : Open file failed\n");
        return e_failure;
    }

    fseek(decInfo->fptr_dest_image, 54, SEEK_SET);

    if (decode_magic_string(decInfo) == e_failure)
    {
        printf("INFO : failed to decode magic string\n");
        return e_failure;
    }

    if (decode_secret_file_extn_size(decInfo) == e_failure)
    {
        printf("INFO : failed to decode secrete file extn size\n");
        return e_failure;
    }

    if (decode_secret_file_extn(decInfo) == e_failure)
    {
        printf("INFO : failed to decode secrete file extn\n");
        return e_failure;
    }

    if (decode_secret_file_size(decInfo) == e_failure)
    {
        printf("INFO : failed to decode file size\n");
        return e_failure;
    }
    if (decode_secret_file_data(decInfo) == e_failure)
    {
        printf("INFO : failed to decode file data\n");
        return e_failure;
    }
    close_all_files_decode(decInfo);

    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
    char buf[3];

    for (int i = 0; i < strlen(MAGIC_STRING); i++)
    {
        fread(decInfo->image_data, 8, 1, decInfo->fptr_dest_image);
        buf[i] = decode_byte_from_lsb(decInfo->image_data);
    }
    buf[2] = '\0';

    if (strcmp(buf, MAGIC_STRING) != 0)
    {
        return e_failure;
    }
    printf("INFO : Decoded magic string succesfully\n");
    return e_success;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    fread(decInfo->image_data, 32, 1, decInfo->fptr_dest_image);
    decInfo->extn_secret_file_size = decode_size_from_lsb(decInfo->image_data);

    if (decInfo->extn_secret_file_size <= 0 || decInfo->extn_secret_file_size > MAX_FILE_SUFFIX)
    {
        return e_failure;
    }
    printf("INFO : Decoded secrete file extrn size succesfully\n");
    return e_success;
}


Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    for (int i = 0; i < decInfo->extn_secret_file_size; i++)
    {
        fread(decInfo->image_data, 8, 1, decInfo->fptr_dest_image);
        decInfo->extn_secret_file_decode[i] =decode_byte_from_lsb(decInfo->image_data);
    }
    decInfo->extn_secret_file_decode[decInfo->extn_secret_file_size] = '\0';
    printf("INFO : Decoded secrete file extrn succesfully\n");
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    fread(decInfo->image_data, 32, 1, decInfo->fptr_dest_image);
    decInfo->size_secret_file_decode = decode_size_from_lsb(decInfo->image_data);

    if (decInfo->size_secret_file_decode <= 0)
    {
        return e_failure;
    }
    printf("INFO : Decoded secrete file size succesfully\n");
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    for (int i = 0; i < decInfo->size_secret_file_decode; i++)
    {
        fread(decInfo->image_data, 8, 1, decInfo->fptr_dest_image);
        char ch = decode_byte_from_lsb(decInfo->image_data);
        fwrite(&ch, 1, 1, decInfo->fptr_output);
    }
    printf("INFO : Decoded secrete file data succesfully\n");
    return e_success;
}

char decode_byte_from_lsb(char *image_buffer)
{
    char ch = 0;

    for (int i = 0; i < 8; i++)
    {
        ch <<= 1;
        ch |= (image_buffer[i] & 1);
    }
    return ch;
}

int decode_size_from_lsb(char *image_buffer)
{
    int value = 0;

    for (int i = 0; i < 32; i++)
    {
        value <<= 1;
        value |= (image_buffer[i] & 1);
    }
    return value;
}

Status close_all_files_decode(DecodeInfo *decInfo)
{
    if (decInfo->fptr_dest_image)
    {
        //printf("Closed destination image file pointer\n");
        fclose(decInfo->fptr_dest_image);
    }

    if (decInfo->fptr_output)
    {
        //printf("Closed output file pointer\n");
        fclose(decInfo->fptr_output);
    }

    return e_success;
}
