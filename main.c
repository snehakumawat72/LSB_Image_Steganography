#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

OperationType check_operation_type(char *);

int main(int argc, char *argv[])
{
    if (argc >= 4)
    {
        OperationType optype = check_operation_type(argv[1]);
        EncodeInfo encInfo;
        if (optype == e_encode)
        {

            // Validate input arguments
            if (read_and_validate_encode_args(argv, &encInfo) == e_success)
            {
                printf("Validation successful. Starting encoding process...\n");

                if (do_encoding(&encInfo) == e_success)
                {
                    printf("Encoding completed successfully.\n");
                }
                else
                {
                    printf("Encoding failed.\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Invalid encode arguments or files.\n");
                return e_failure;
            }
        }
    else if (optype == e_decode)
    {
        if (argc >= 3)
        {
            DecodeInfo decInfo;
            if (read_and_validate_decode_args(argv, &decInfo) == e_success)
            {
                printf("Validation successful. Starting decoding process...\n");

                if (do_decoding(&decInfo) == e_success)
                {
                    printf("Decoding completed successfully.\n");
                }
                else
                {
                    printf("Decoding failed.\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Invalid decode arguments or files.\n");
                return e_failure;
            }
        }
    }

    else
    {
        printf("Unsupported operation! Use -e for encoding or -d for decoding.\n");
    }
    }
    else
    {
        printf("Usage:\n");
        printf("For encoding: ./a.out -e <source_image.bmp> <secret.txt> [output.bmp]\n");
    }

    return 0;
}

OperationType check_operation_type(char *symbol)
{
    if (strcmp(symbol, "-e") == 0)
    {
        return e_encode;
    }
    else if (strcmp(symbol, "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}
