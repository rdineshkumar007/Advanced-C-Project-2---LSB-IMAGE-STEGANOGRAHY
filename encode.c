/*
Name  : Dineshkumar R
Date  : 02/09/2025
Title : LSB IMAGE STEGANOGRAPHY : Encoding & Decoding
Input Arguments :
Encoding : ./a.out -e source.bmp secret.txt destination.bmp
Decoding : ./a.out -d destination.bmp final.txt
*/

#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <string.h>
#include<unistd.h>

/* Function Definitions */
char ptr5[100];
/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);
    printf("| Source file size : \n");
    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("|   1. Width = %u Bytes\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("|   2. Height = %u Bytes\n", height);

    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data
    fseek(fptr, 0, SEEK_END);
    long size1 = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    return size1;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    printf("\n|-------------------------------------------------------------------------------------|\n");
    strcpy(ptr5, " =>Validating Encode Mode Arguments");
    loading(ptr5);
    char *str4[] = {".c", ".txt", ".csv", ".sh"};
    int siz = sizeof(str4)/sizeof(str4[0]);

    //  Step 1 : Check the argv[2] is having the .bmp extension or not
    if(argv[2] == NULL)
    {
        goto a13;
    }
    char *ext = strrchr(argv[2], '.');
    if(ext == NULL || strcmp(ext, ".bmp") != 0)
    {
        a13 :
        printf("| Error : '.bmp' Extension file not found in argv[2].\n");
        printf("|-------------------------------------------------------------------------------------|\n");
        return e_failure;
    }
    // true - > Store into the structure member step 2 , false - > return e_failure
    encInfo->src_image_fname = argv[2];

    // Step 2 : Check the argv[3] is having the .txt extension or not
    if(argv[3] == NULL)
    {
        goto a14;
    }
    ext = strrchr(argv[3], '.');
    if(ext == NULL)
    {
        a14 :
        printf("| Error : Proper Secret file's Extension not found in argv[3].\n");
        printf("|-------------------------------------------------------------------------------------|\n");
        return e_failure;
    }
    int i, flag = 1;
    for(i =0; i<siz; i++)
    {
        if(strcmp(ext, str4[i]) == 0)
        {
            // true - > Store into the structure member step 3 , false - > return e_failure
            encInfo->secret_fname = argv[3];
            flag = 0;
            break;
        }
    }
    if(flag)
    {
        goto a14;
    }
    //printf("%s\n", str4[i]);

    // Step 3 : Check argv[4] != NULL or not true - > Step 4, false -> Step 5
    if(argv[4] != NULL)
    {
        // Step 4 : Check the argv[4] is having the .bmp extension or not
        // true - > Store into the structure member step 6 , false - > return e_failure
        ext = strrchr(argv[4], '.');
        if(ext == NULL || strcmp(ext, ".bmp") != 0)
        {
            printf("| Error : '.bmp' Extension file not found in argv[4].\n");
            printf("|-------------------------------------------------------------------------------------|\n");
            return e_failure;
        }
        encInfo->stego_image_fname = argv[4];
    }
    // Step 5 : Store the default.bmp into the structure member - > Step 6
    else
    {
        printf("| Note : \n| No argument given for destination file.\n");
        encInfo->stego_image_fname = "final_dest.bmp";
        printf("| Created a default file name : %s\n", encInfo->stego_image_fname);
    }

    // Step 6 : return e_success;
    printf("| Successfully validated Encode Mode arguments.\n");
    printf("|-------------------------------------------------------------------------------------|\n");
    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    printf("\n|-------------------------------------------------------------------------------------|\n");
    strcpy(ptr5, " =>Opening Files and saving them in structure members to Encode");
    loading(ptr5);
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "| Error : Unable to open file %s\n", encInfo->src_image_fname);
        printf("|-------------------------------------------------------------------------------------|\n");
        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret =  fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "| Error : Unable to open file %s\n", encInfo->secret_fname);
        printf("|-------------------------------------------------------------------------------------|\n");
        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "| Error : Unable to open file %s\n", encInfo->stego_image_fname);
        printf("|-------------------------------------------------------------------------------------|\n");
        return e_failure;
    }

    // No failure return e_success
    printf("| Successfully Saved files to Structure members.\n");
    printf("|-------------------------------------------------------------------------------------|\n");
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    printf("\n|-------------------------------------------------------------------------------------|\n");
    strcpy(ptr5, " =>Checking Capacity of Source file and Secret file");
    loading(ptr5);
    // Step 1 :  Call get_image_size_for_bmp(encinfo->fptr_image) store into the encinfo->image_capacity
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    // Step 2 : Call get_file_size(encinfo->fptr_secret) store into the encinfo-> size_secret_file;
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    printf("| Secret file size : %ld Bytes\n", encInfo->size_secret_file);
    // Step 3 : encinfo->image_capacity > 16 + 32 + 32 + 32 + (encinfo-> size_secret_file * 8)
    uint capacity1 = 16 + 32 + 32 + 32 + (encInfo->size_secret_file * 8);
    if(encInfo->image_capacity > capacity1)
    {
        printf("| Successfully got the Capacity of Source and Secret file.\n");
        printf("|-------------------------------------------------------------------------------------|\n");
        return e_success;
    }
    else //   true - return e_ succes , false - > e _ failure
    {
        printf("\n| Error : Image capacity size is lower than secret file capacity.\n");
        printf("|-------------------------------------------------------------------------------------|\n");
        return e_failure;
    }

}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    printf("\n|-------------------------------------------------------------------------------------|\n");
    strcpy(ptr5, " =>Copying Source file's header(54 Bytes) to Destination file");
    loading(ptr5);
    char ImageBuffer[54];
    fseek(fptr_src_image, 0 , SEEK_SET);
    fread(ImageBuffer , 54, 1, fptr_src_image);
    fwrite(ImageBuffer, 54, 1,fptr_dest_image);
    printf("| Successfully copied Source file's header to Destination file\n");
    printf("|-------------------------------------------------------------------------------------|\n");
    return e_success;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    printf("\n|-------------------------------------------------------------------------------------|\n");
    strcpy(ptr5, " =>Encoding Magic Strings");
    loading(ptr5);
    char imageBuffer[8];
    int mag_len = strlen(magic_string);
    // Step 1 : Generate the loop upto the magic_string size
    for(int i =0; i<mag_len; i++)
    {
        // Step 2 : Read the 8 bytes from src store into image buffer
        fread(imageBuffer, 8, 1, encInfo->fptr_src_image);

        // Step 3 : Call the encode_byte_to_lsb(magic_string[i],imagebuffer)
        encode_byte_to_lsb(magic_string[i], imageBuffer);

        // Step 4 : Write the imageBuffer into destination file
        fwrite(imageBuffer, 8, 1, encInfo->fptr_stego_image);
    }
    // Repeat the process upto the size
    // Return e_success
    printf("| Successfully Encoded Magic Strings and saved to Destination file.\n");
    printf("|-------------------------------------------------------------------------------------|\n");
    return e_success;
}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    printf("\n|-------------------------------------------------------------------------------------|\n");
    strcpy(ptr5, " =>Encoding Secret file's extension size");
    loading(ptr5);
    // Step 1 : Read the 32 bytes from src store into image buffer
    char imageBuffer[32];

    // Step 2 : Call the encode_size_to_lsb(size,imagebuffer)
    fread(imageBuffer, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(size, imageBuffer);

    // Step 3 :  Write the imageBuffer into destination file
    fwrite(imageBuffer, 32, 1, encInfo->fptr_stego_image);

    // Return e_success
    printf("| Successfully Encoded Secret file's extension size and saved to Destination file.\n");
    printf("|-------------------------------------------------------------------------------------|\n");
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    printf("\n|-------------------------------------------------------------------------------------|\n");
    strcpy(ptr5, " =>Encoding Secret file's extension");
    loading(ptr5);
    char imageBuffer[8];
    int ext_len1 = strlen(file_extn);

    for(int i =0; i<ext_len1; i++)
    {
        //Read 8 bytes from source image
        fread(imageBuffer, 8, 1, encInfo->fptr_src_image);

        //Encode the byte
        encode_byte_to_lsb(file_extn[i], imageBuffer);

        //Write the encoded bytes to destination
        fwrite(imageBuffer, 8, 1, encInfo->fptr_stego_image);
    }
    printf("| Successfully Encoded Secret file's extension and saved to Destination file.\n");
    printf("|-------------------------------------------------------------------------------------|\n");
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    printf("\n|-------------------------------------------------------------------------------------|\n");
    strcpy(ptr5, " =>Encoding Secret file's size");
    loading(ptr5);
    char imageBuffer[32];

    //Read 32 bytes from source file
    fread(imageBuffer, 32, 1, encInfo->fptr_src_image);

    //Encode the file size
    encode_size_to_lsb(file_size, imageBuffer);

    //Write 32 bytes to destination file
    fwrite(imageBuffer, 32, 1, encInfo->fptr_stego_image);
    printf("| Successfully Encoded Secret file's size and saved to Destination file.\n");
    printf("|-------------------------------------------------------------------------------------|\n");
    return e_success;

}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("\n|-------------------------------------------------------------------------------------|\n");
    strcpy(ptr5, " =>Encoding Secret file's data");
    loading(ptr5);
    char imageBuffer[8];
    char secret_byte;

    while(fread(&secret_byte, 1, 1, encInfo->fptr_secret) == 1)
    {
        fread(imageBuffer, 8, 1, encInfo->fptr_src_image);

        encode_byte_to_lsb(secret_byte, imageBuffer);

        fwrite(imageBuffer, 8, 1, encInfo->fptr_stego_image);
    }
    printf("| Successfully Encoded Secret file's data and saved to Destination file.\n");
    printf("|-------------------------------------------------------------------------------------|\n");
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    printf("\n|-------------------------------------------------------------------------------------|\n");
    strcpy(ptr5, " =>Copying remaining data to Destination file");
    loading(ptr5);
    char buffer;
    while(fread(&buffer, 1, 1, fptr_src) == 1)
    {
        fwrite(&buffer, 1, 1, fptr_dest);
    }
    printf("| Successfully Copied remaining data to Destination file.\n");
    printf("|-------------------------------------------------------------------------------------|\n");
    return e_success;

}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    // Step 1 Generate the loop from 0 to 7
    for(int i = 0; i<8; i++)
    {
        // Step 2 : Clear the lsb bit of imageBuffer[i]
        image_buffer[i] &= 0xFE;

        // Step 3 : Get the bit from data
        char bit = (data >> (7 - i)) & 1;

        // Step 4 : Replace the get bit into lsb of imageBuffer[i]
        image_buffer[i] |= bit;
    }
    // Repeat the process up to the size
    // Return e_success;
    return e_success;
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    // Step 1 Generate the loop from 0 to 31
    for(int i = 0; i<32; i++)
    {
        // Step 2 : Clear the lsb bit of imageBuffer[i]
        imageBuffer[i] &= 0xFE;

        // Step 3 : Get the bit from size
        char bit1 = (size >> (31 - i)) & 1;

        // Step 4 : Replace the get bit into  the lsb of imageBuffer[i]
        imageBuffer[i] |= bit1;
    }
    // Repeat the process up to the size
    // Return e_success;
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    printf("\n");
    printf("\n");
    printf("|-------------------------------------------------------------------------------------|\n");
    printf("|*************************---> Starting Encoding Process <---*************************|  ");
    printf("\n|-------------------------------------------------------------------------------------|\n");
    printf("\n");
    // Step 1 : Call the Open files(encinfo) true -> step 2 false - > return e_failure
    if(open_files(encInfo) == e_failure)
    {
        return e_failure;
    }

    if(check_capacity(encInfo) == e_failure)
    {
        return e_failure;
    }

    // Step 2 : Call the copy_bmp_header(); true - > step 3 false - > return e_failure
    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    // Step 3 : Call the  encode_magic_string()  true - > step 4 false - > return e_failure
    if(encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        return e_failure;
    }

    // Step 4 : Extract the extension from secret file name and store into the encinfo - > extn_size
    char *ext = strrchr(encInfo->secret_fname, '.');
    if(ext == NULL)
    {
        printf("\n|-------------------------------------------------------------------------------------|\n");
        printf("| Error : Extracting extension from secret file.\n");
        printf("|-------------------------------------------------------------------------------------|\n");
        return e_failure;
    }

    // Step 5 : Find the secret file extension size
    int ext_len = strlen(ext);

    // Step 6: Call the encode_secret_file_extn_size(); true - step -> 7 ,false - > return e_failure
    if(encode_secret_file_extn_size(ext_len, encInfo) == e_failure)
    {
        return e_failure;
    }

    if(encode_secret_file_extn(ext, encInfo) == e_failure)
    {
        return e_failure;
    }

    if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
        return e_failure;
    }

    if(encode_secret_file_data(encInfo) == e_failure)
    {
        return e_failure;
    }

    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
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

void loading1(char *ptr1) // loading function definition
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
