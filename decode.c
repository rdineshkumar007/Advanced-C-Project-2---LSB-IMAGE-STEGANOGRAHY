/*
Name  : Dineshkumar R
Date  : 02/09/2025
Title : LSB IMAGE STEGANOGRAPHY : Encoding & Decoding
Input Arguments :
Encoding : ./a.out -e source.bmp secret.txt destination.bmp
Decoding : ./a.out -d destination.bmp final.txt
*/

#include <stdio.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

//Function definitions
char ptr6[100];

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    printf("\n|-------------------------------------------------------------------------------------|\n");
    strcpy(ptr6, " =>Validating Decode Mode Arguments");
    loading1(ptr6);

    // Validate stego image file (must be .bmp)
    char *ext = strrchr(argv[2], '.');
    if(ext == NULL || strcmp(ext, ".bmp") != 0)
    {
        printf("| Error : '.bmp' Extension file not found in argv[2].\n");
        printf("|-------------------------------------------------------------------------------------|\n");
        return e_failure;
    }
    decInfo->stego_image_fname = argv[2];

    // Handle output filename
    if(argv[3] != NULL)
    {
        // Copy the full argument to buffer
        strncpy(decInfo->secret_fname_buf, argv[3], sizeof(decInfo->secret_fname_buf) - 1);
        decInfo->secret_fname_buf[sizeof(decInfo->secret_fname_buf) - 1] = '\0';

        // Remove any existing extension by finding the last dot and truncating
        char *dot_pos = strrchr(decInfo->secret_fname_buf, '.');
        if(dot_pos != NULL)
        {
            *dot_pos = '\0'; // Truncate at the dot
        }
        else
        {
            return e_failure;
        }

        decInfo->secret_fname = decInfo->secret_fname_buf;
    }
    else
    {
        printf("| Note : No argument given for destination file.\n");
        strcpy(decInfo->secret_fname_buf, "secrettt");
        decInfo->secret_fname = decInfo->secret_fname_buf;
        printf("| Created a default file name : %s\n", decInfo->secret_fname);
    }

    printf("| Successfully validated Decode Mode arguments.\n");
    printf("| Stego image file: %s\n", decInfo->stego_image_fname);
    printf("| Output base filename: %s\n", decInfo->secret_fname);
    printf("|-------------------------------------------------------------------------------------|\n");
    return e_success;
}


Status open_decode_files(DecodeInfo *decInfo) // opening decode files function definition
{
    printf("\n|-------------------------------------------------------------------------------------|\n");
    strcpy(ptr6, " =>Opening Files and saving them in structure members to Decode");
    loading1(ptr6);
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");
    if(decInfo->fptr_stego_image == NULL)
    {
        return e_failure;
    }

    printf("| Successfully Saved files to Structure members.\n");
    printf("|-------------------------------------------------------------------------------------|\n");
    return e_success;

}

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo) // decoding magic string function definition
{
    printf("\n|-------------------------------------------------------------------------------------|\n");
    strcpy(ptr6, " =>Decoding Magic Strings");
    loading1(ptr6);
    char imageBuffer[8];
    int magic_len = strlen(magic_string);
    char decoded_magic[magic_len + 1];

    for(int i =0; i<magic_len; i++)
    {
        if (fread(imageBufferq, 8, 1, decInfo->fptr_stego_image) != 1)
        {
            printf("error! magic string.\n");
            return e_failure;
        }

        decoded_magic[i] = decode_byte_from_lsb(imageBuffer);
    }
    decoded_magic[magic_len] = '\0';

    if(strcmp(decoded_magic, magic_string) != 0)
    {
        printf("\nError magic string not matched.\n");
        return e_failure;
    }

    printf("| Successfully Decoded Magic Strings and they Match correctly.\n");
    printf("|-------------------------------------------------------------------------------------|\n");
    return e_success;

}

Status decode_secret_file_extn_size(int *size, DecodeInfo *decInfo) // decoding secret file extension size function definition
{
    printf("\n|-------------------------------------------------------------------------------------|\n");
    strcpy(ptr6, " =>Decoding Secret file's extension size");
    loading1(ptr6);
    char imageBuffer[32];
    if(fread(imageBuffer, 32, 1, decInfo->fptr_stego_image) != 1)
    {
        printf("\nError reading extension size from image.\n");
        return e_failure;
    }

    *size = decode_size_from_lsb(imageBuffer);
    printf("| Secret file's extension size : %d\n", *size);
    printf("| Successfully Decoded Secret file's extension size.\n");
    printf("|-------------------------------------------------------------------------------------|\n");
    return e_success;
}

Status decode_secret_file_extn(int size, DecodeInfo *decInfo) //decoding secret file extension function definition
{
    printf("\n|-------------------------------------------------------------------------------------|\n");
    strcpy(ptr6, " =>Decoding Secret file's extension");
    loading1(ptr6);
    char imageBuffer[8];
    int i;
    for(i =0; i<size; i++)
    {
        if(fread(imageBuffer, 8, 1, decInfo->fptr_stego_image) != 1)
        {
            printf("\nError reading extension from image.\n");
            return e_failure;
        }

        decInfo->extn_secret_file[i] = decode_byte_from_lsb(imageBuffer);
    }

    decInfo->extn_secret_file[i] = '\0';

    printf("| Secret file's extension : %s\n", decInfo->extn_secret_file);
    printf("| Successfully Decoded Secret file's extension.\n");
    printf("|-------------------------------------------------------------------------------------|\n");
    return e_success;

}

Status decode_secret_file_size(long *file_size, DecodeInfo *decInfo) // decoding secret file size function definition
{
    printf("\n|-------------------------------------------------------------------------------------|\n");
    strcpy(ptr6, " =>Decoding Secret file's size");
    loading1(ptr6);
    char imageBuffer[32];

    if(fread(imageBuffer, 32, 1, decInfo->fptr_stego_image) != 1)
    {
        printf("\nFailed reading file size from image.\n");
        return e_failure;
    }

    *file_size = decode_size_from_lsb(imageBuffer);
    printf("| Secret file size : %ld bytes\n", *file_size);
    printf("| Successfully Decoded Secret file's size.\n");
    printf("|-------------------------------------------------------------------------------------|\n");
    return e_success;
}

Status decode_secret_file_data(long file_size, DecodeInfo *decInfo) // decoding secret file data function definition
{
    printf("\n|-------------------------------------------------------------------------------------|\n");
    strcpy(ptr6, " =>Decoding Secret file's data");
    loading1(ptr6);

    char imageBuffer[8];
    char secret_byte;

    // Create the full filename by combining base name + decoded extension
    char full_filename[256];
    strncpy(full_filename, decInfo->secret_fname, sizeof(full_filename) - 1);
    full_filename[sizeof(full_filename) - 1] = '\0';

    // Append the decoded extension
    strncat(full_filename, decInfo->extn_secret_file,
            sizeof(full_filename) - strlen(full_filename) - 1);

    // Open in binary mode to handle all file types properly
    decInfo->fptr_secret = fopen(full_filename, "wb");
    if(decInfo->fptr_secret == NULL)
    {
        printf("| Error: Failed to create output file '%s'\n", full_filename);
        return e_failure;
    }

    for(int i = 0; i < file_size; i++)
    {
        if(fread(imageBuffer, 8, 1, decInfo->fptr_stego_image) != 1)
        {
            printf("| Error: Failed to read secret data from image at byte %d\n", i);
            fclose(decInfo->fptr_secret);
            return e_failure;
        }

        secret_byte = decode_byte_from_lsb(imageBuffer);

        if(fwrite(&secret_byte, 1, 1, decInfo->fptr_secret) != 1)
        {
            printf("| Error: Failed to write to output file at byte %d\n", i);
            fclose(decInfo->fptr_secret);
            return e_failure;
        }
    }

    // Close the file properly
    fclose(decInfo->fptr_secret);

    printf("| Successfully Decoded %ld bytes of secret data\n", file_size);
    printf("| Output file: %s\n", full_filename);
    printf("|-------------------------------------------------------------------------------------|\n");
    return e_success;
}

char decode_byte_from_lsb(char *image_buffer) // decoding byte from lsb function definition
{
    char data = 0;
    for(int i =0; i<8; i++)
    {
        char bit = image_buffer[i] & 1;
        data = (data << 1) | bit;
    }

    return data;
}

Status decode_size_from_lsb(char *imageBuffer) // decoding size from lsb function definition
{
    int size = 0;
    for(int i =0; i<32; i++)
    {
        char bit = imageBuffer[i] & 1;
        size = (size << 1) | bit;
    }
    return size;
}

Status do_decoding(DecodeInfo *decInfo) // Encoding function definition
{
    printf("\n");
    printf("\n");
    printf("|-------------------------------------------------------------------------------------|\n");
    printf("|*************************---> Starting Decoding Process <---*************************|  ");
    printf("\n|-------------------------------------------------------------------------------------|\n");
    printf("\n");

    if(open_decode_files(decInfo) == e_failure)
    {
        printf("|-------------------------------------------------------------------------------------|\n");
        printf("\n| Error : Failed to open files.\n");
        printf("|-------------------------------------------------------------------------------------|\n");
        return e_failure;
    }

    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    if(decode_magic_string(MAGIC_STRING, decInfo) == e_failure)
    {
        printf("|-------------------------------------------------------------------------------------|\n");
        printf("\n| Error : Magic string verification failed.\n");
        printf("|-------------------------------------------------------------------------------------|\n");
        return e_failure;
    }

    int extn_size;
    if(decode_secret_file_extn_size(&extn_size, decInfo) == e_failure)
    {
        printf("|-------------------------------------------------------------------------------------|\n");
        printf("\n| Error : Decoding extension file failed.\n");
        printf("|-------------------------------------------------------------------------------------|\n");
        return e_failure;
    }

    if(decode_secret_file_extn(extn_size, decInfo) == e_failure)
    {
        printf("|-------------------------------------------------------------------------------------|\n");
        printf("\n| Error : Failed to decode extension.\n");
        printf("|-------------------------------------------------------------------------------------|\n");
        return e_failure;
    }

    long file_size;
    if(decode_secret_file_size(&file_size, decInfo) == e_failure)
    {
        printf("|-------------------------------------------------------------------------------------|\n");
        printf("\n| Error : Failed to decode file size.\n");
        printf("|-------------------------------------------------------------------------------------|\n");
        return e_failure;
    }

    if(decode_secret_file_data(file_size, decInfo) == e_failure)
    {
        printf("|-------------------------------------------------------------------------------------|\n");
        printf("\n| Error : Failed to decode secret data.\n");
        printf("|-------------------------------------------------------------------------------------|\n");
        return e_failure;
    }
     printf("\n");
    printf("\n");
    printf("|-------------------------------------------------------------------------------------|\n");
    printf("|*******************---> Successfully Completed Decoding Data <---********************|");
    printf("\n|-------------------------------------------------------------------------------------|\n");
    printf("\n");
    return e_success;
}

void loading(char *ptr1) // loading function definition
{
    char local_ptr[100];
    strcpy(local_ptr, ptr1);

    printf("%s [          ] 0%%", local_ptr);
    fflush(stdout);

    for(int i = 0; i <= 100; i += 5)
    {
        usleep(50000);
        printf("\r|%s [", local_ptr);
        for(int j = 0; j < 10; j++)
        {
            if(j < i/10)
            {
                printf("=");
            }
            else if(j == i/10)
            {
                printf(">");
            }
            else
            {
                printf(" ");
            }
        }
        printf("] %d%%", i);
        fflush(stdout);
    }
    printf("\n");
}

