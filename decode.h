/*
Name  : Dineshkumar R
Date  : 02/09/2025
Title : LSB IMAGE STEGANOGRAPHY : Encoding & Decoding
Input Arguments :
Encoding : ./a.out -e source.bmp secret.txt destination.bmp
Decoding : ./a.out -d destination.bmp final.txt
*/

#ifndef DECODE_H_INCLUDED
#define DECODE_H_INCLUDED
#include <stdio.h>
#include "types.h"
#include "common.h"

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[32];
    long size_secret_file;

    // Add this line:
    char secret_fname_buf[256];  // Buffer to store the base filename

} DecodeInfo;

//Function prototypes

void loading1(char *);

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

Status open_decode_files(DecodeInfo *decInfo);

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

Status decode_secret_file_extn_size(int *size, DecodeInfo *decInfo);

Status decode_secret_file_extn(int size, DecodeInfo *decInfo);

Status decode_secret_file_size(long *file_size, DecodeInfo *decInfo);

Status decode_secret_file_data(long file_size, DecodeInfo *decInfo);

char decode_byte_from_lsb(char *image_buffer);

Status decode_size_from_lsb(char *imageBuffer);

#endif
