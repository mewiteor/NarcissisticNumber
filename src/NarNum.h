/*
 * copyright (c) 2015 Mewiteor
 *
 * This file is part of NarcissisticNumber.
 *
 * NarcissisticNumber is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * NarcissisticNumber is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with NarcissisticNumber; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _NAR_NUM_H_
#define _NAR_NUM_H_ 1

#include <stdbool.h>

typedef unsigned char BaseType;
typedef unsigned short DigitType, DoubleBaseType;
typedef unsigned int DoubleDigitType;

// Big integer type.
typedef struct
{
    bool neg;           // whether negative or not.
    DigitType size;     // Number of digits.
    BaseType* data;     // The data of the big integer.
}Big;

typedef struct
{
    DigitType* count; // The count of each number from 0 to base-1.

    /******************************************************************
     * data:i^digit*j
     * range:0<=i<base,0<=j<=digit
     * example:
     *   base=10,digit=3
     *   +---+-+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     *   |j\i|0|  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |
     *   +---+-+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     *   | 0 |0|  0  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |
     *   +---+-+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     *   | 1 |0| 1^3 | 2^3 | 3^3 | 4^3 | 5^3 | 6^3 | 7^3 | 8^3 | 9^3 |
     *   +---+-+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     *   | 2 |0|1^3*2|2^3*2|3^3*2|4^3*2|5^3*2|6^3*2|7^3*2|8^3*2|9^3*2|
     *   +---+-+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     *   | 3 |0|1^3*3|2^3*3|3^3*3|4^3*3|5^3*3|6^3*3|7^3*3|8^3*3|9^3*3|
     *   +---+-+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     ******************************************************************/
    Big** powAndMul;

    /******************************************************************
     * data:base^digit-i^digit*j
     * range:0<=i<base,0<=j<=digit
     * example:
     *   When base=10,digit=9,i=8,j=3,
     *   it means a number consist of 3 times five (xx888xxxx).And the remain of the
     *       sum is 10^9-8^9*3
     ******************************************************************/
    Big** maxSumRem;

    /******************************************************************
     * data:base^(digit-1)-i^digit*k-(i-1)^digit*(j-k)
     * range:0<i<base,0<=j<=digit,0<=k<=j
     * example:
     *   When base=10,digit=9,i=8,j=5,k=3
     *   it means a number consist of 3 times 8 and twice 7 (77888xxxx).
     *       The remain of the sum is 10^9-8^9*3-7^9*2
     ******************************************************************/
    Big*** minSumRem;

    Big **res;          // A pointer pointing to a array to be used for saving results.
    size_t *resSize;    // A pointer pointing to the size of the result array.
    DigitType digit;
    BaseType base;
    bool ok;            // whether init success or not.
    bool showProgress;  // Whether print the number which is found when check the number or not.
} NarNumParam;

bool Init(
    const DigitType digit,      // The number of digits which will be searched.
    const BaseType base,        // The base of the number.
    Big** resArr,               // A pointer pointing to a array to be used for saving results.
    size_t *resCount,           // The count of results.
    bool showProgress,          // Whether print the number which is found when check the number or not.
    NarNumParam *param
    );
void Search(NarNumParam *param);
void Release(NarNumParam *param);
void Print(const Big* b,const BaseType base);
int  BigCmp(const Big* a, const Big* b);
void BigsFree(Big** nums, size_t size);

//************************************
// Method:      GetMaxDigit
// FullName:    GetMaxDigit
// Access:      public 
// Returns:     DigitType (2<=x<=3486)
// Qualifier:  
// Parameter:   BaseType base (2<=x<=255)
// Description: 
//************************************
DigitType GetMaxDigit(BaseType base);

#endif // _NAR_NUM_H_
