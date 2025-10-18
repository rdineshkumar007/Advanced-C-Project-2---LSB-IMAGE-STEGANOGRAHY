/*
Name  : Dineshkumar R
Date  : 02/09/2025
Title : STEGANOGRAPHY : Encoding & Decoding
Input Arguments :
Encoding : ./a.out -e source.bmp secret.txt destination.bmp
Decoding : ./a.out -d destination.bmp final.txt
*/

#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include<unistd.h>
#include<string.h>

OperationType check_operation_type(char *);

int main(int argc, char *argv[])
{
    // Step 1 : Check for argument counts
    if(argc < 3)
    {
        printf("--------------------------------\n");
        printf("\nArguments not enough.\n");
        printf("--------------------------------\n");
        return 1;
    }

    printf("\n---------------------------------------------------------------------------------------");
    printf("\n|                                    STEGANOGRAPHY                                    |");
    printf("\n---------------------------------------------------------------------------------------\n");

    // Step 2 : Call the check_operation_type(argv[1]) == e_encode )) true - > step 3
    OperationType op_type = check_operation_type(argv[1]);
    if(op_type == e_encode)
    {
        printf("\n|------------------------------------Encoding Mode------------------------------------|\n");
        // Step 3 : Declare structure variable EncodeInfo enc_info
        EncodeInfo encInfo;

        if(read_and_validate_encode_args(argv, &encInfo) == e_success)// Step 4 : Call the read_and_validate_encode_args(argv,&enc_info)== e_success)
        {//  true -> Step 5 , false - > terminate the program
            if(do_encoding(&encInfo) == e_success)// Step 5 : Call the do_encoding (&encInfo);
            {
                fclose(encInfo.fptr_src_image);
                fclose(encInfo.fptr_stego_image);
                fclose(encInfo.fptr_secret);
                printf("\n|---------------------------------Existing Encoding Mode------------------------------|\n");
                return 1;
            }
        }
        else
        {
            printf("\n|------------------------------------Encoding Failed----------------------------------|\n");
            printf("\n|---------------------------------Existing Encoding Mode------------------------------|\n");
        }

    }
    else if(op_type == e_decode)// Step 2 : Call the check_operation_type(argv[1]) == d_encode )) true - > step 3
    {

        printf("\n|------------------------------------Decoding Mode------------------------------------|\n");
        DecodeInfo decInfo;// Step 3 : Declare structure variable DecodeInfo decInfo

        if(read_and_validate_decode_args(argv, &decInfo) == e_success)// Step 4 : Call the read_and_validate_decode_args(argv,&decInfo)== e_success)
        {//  true -> Step 5 , false - > terminate the program
            if(do_decoding(&decInfo) == e_success)// Step 5 : Call the do_decoding(&decInfo);
            {
                fclose(decInfo.fptr_stego_image);
                fclose(decInfo.fptr_secret);
                printf("\n|---------------------------------Existing Decoding Mode------------------------------|\n");
                return e_failure;
            }

        }
        else
        {
            printf("\n|------------------------------------Decoding Failed----------------------------------|\n");
            printf("\n|---------------------------------Existing Decoding Mode------------------------------|\n");
        }


    }
    else
    {
        printf("Unsupported format.\n");
        return e_failure;
    }



}

OperationType check_operation_type(char *symbol)
{
    // Step 1 : Check whether the symbol is -e or not true - > return e_encode false -> Step 2
    if(strcmp(symbol, "-e") == 0)
    {
        return e_encode;
    }
    // Step 2 : Check whether the symbol is -d or not true - > return e_decode
    else if(strcmp(symbol, "-d") == 0)
    {
        return e_decode;
    }
    // Step 3 : Step 2 false -> return e_unsupported
    else
    {
        return e_unsupported;
    }

}
