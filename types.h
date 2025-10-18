/*
Name  : Dineshkumar R
Date  : 02/09/2025
Title : LSB IMAGE STEGANOGRAPHY : Encoding & Decoding
Input Arguments :
Encoding : ./a.out -e source.bmp secret.txt destination.bmp
Decoding : ./a.out -d destination.bmp final.txt
*/

#ifndef TYPES_H
#define TYPES_H

/* User defined types */
typedef unsigned int uint;

/* Status will be used in fn. return type */
typedef enum
{
    e_failure,
    e_success
} Status;

typedef enum
{
    e_encode,
    e_decode,
    e_unsupported
} OperationType;

#endif
