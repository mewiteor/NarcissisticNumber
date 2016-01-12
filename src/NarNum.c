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
#include"NarNum.h"

#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<malloc.h>


void SubSearch(BaseType cur, DigitType remain, const Big* sum, NarNumParam *param);

//************************************
// Method:      Check
// FullName:    Check
// Access:      public 
// Returns:     void
// Qualifier:  
// Parameter:   const Big * sum
// Parameter:   NarNumParam * param
// Description: Check whether the sum
//              matches the count of each
//              number from 0 to base-1.
//************************************
void Check(const Big* sum, NarNumParam *param);

//************************************
// Method:      PreCheck
// FullName:    PreCheck
// Access:      public 
// Returns:     bool
// Qualifier:  
// Parameter:   BaseType cur
// Parameter:   DigitType remain
// Parameter:   const Big * sum
// Parameter:   NarNumParam * param
// Description: Check whether the high
//              digits of the sum which
//              can be ensured match
//              the count of each number
//              from 0 to base-1.
//************************************
bool PreCheck(BaseType cur, DigitType remain, const Big* sum, NarNumParam *param);

//************************************
// Method:      BigAdd
// FullName:    BigAdd
// Access:      public 
// Returns:     void
// Qualifier:  
// Parameter:   Big * r
// Parameter:   const Big * a
// Parameter:   const Big * b
// Parameter:   NarNumParam * param
// Description: r = a + b
//************************************
void BigAdd(Big* r, const Big* a, const Big* b, NarNumParam *param);

//************************************
// Method:      BigSub
// FullName:    BigSub
// Access:      public 
// Returns:     void
// Qualifier:  
// Parameter:   Big * r
// Parameter:   const Big * a
// Parameter:   NarNumParam * param
// Description: r=param->base^param->digit-a
//************************************
void BigSub(Big* r, const Big* a, NarNumParam *param);

//************************************
// Method:      BigSbs
// FullName:    BigSbs
// Access:      public 
// Returns:     void
// Qualifier:  
// Parameter:   Big * r
// Parameter:   const Big * a
// Parameter:   const Big * b
// Parameter:   NarNumParam * param
// Description: r=param->base^param->digit-a-b
//************************************
void BigSbs(Big* r, const Big* a, const Big* b, NarNumParam *param);

//************************************
// Method:      BigPow
// FullName:    BigPow
// Access:      public 
// Returns:     void
// Qualifier:  
// Parameter:   Big * a
// Parameter:   BaseType x
// Parameter:   DigitType n
// Parameter:   NarNumParam * param
// Description: a=x^n
//************************************
void BigPow(Big* a, BaseType x, DigitType n, NarNumParam *param);

//************************************
// Method:      BigCpy
// FullName:    BigCpy
// Access:      public 
// Returns:     void
// Qualifier:  
// Parameter:   Big * a
// Parameter:   const Big * b
// Description: a = b
//************************************
void BigCpy(Big* a, const Big* b);

bool Init(
    const DigitType digit,
    const BaseType base,
    Big** resArr,
    size_t *resCount,
    bool showProgress,
    NarNumParam *param
    )
{
    BaseType i;
    DigitType j, k;

    assert(digit > 0);
    assert(base > 1);
    assert(resArr != NULL);
    assert(resCount != NULL);
    assert(param != NULL);

    param->ok = false;
    param->showProgress = showProgress;
    param->digit = digit;
    param->base = base;
    param->res = resArr;
    param->resSize = resCount;

    param->count = (DigitType*)calloc(base, sizeof(DigitType));

    param->powAndMul = (Big**)calloc(base, sizeof(Big*));
    param->maxSumRem = (Big**)calloc(base, sizeof(Big*));
    param->minSumRem = (Big***)calloc(base - 1, sizeof(Big**));

    if (!(param->count&&param->powAndMul&&param->maxSumRem&&param->minSumRem))
    {
        Release(param);
        return 0;
    }
    for (i = 0; i < base; ++i)
    {
        param->powAndMul[i] = (Big*)calloc(digit + 1, sizeof(Big));
        param->maxSumRem[i] = (Big*)calloc(digit + 1, sizeof(Big));
        param->minSumRem[i] = (Big**)calloc(digit + 1, sizeof(Big*));

        if (!(param->powAndMul[i] && param->maxSumRem[i] && param->minSumRem[i]))
        {
            Release(param);
            return false;
        }

        for (j = 0; j <= digit; ++j)
        {
            // i^n*j
            if (1 == j)
                BigPow(&param->powAndMul[i][j], i, digit, param);
            else if (j)
                BigAdd(&param->powAndMul[i][j],
                    &param->powAndMul[i][j - 1],
                    &param->powAndMul[i][1], param);

            // base^digit-i^digit*j
            BigSub(&param->maxSumRem[i][j], &param->powAndMul[i][j], param);

            --param->digit;
            if (i)
            {
                param->minSumRem[i - 1][j] = (Big*)calloc(j + 1, sizeof(Big));
                if (!param->minSumRem[i - 1][j])
                {
                    Release(param);
                    return false;
                }

                for (k = 0; k <= j; ++k)
                    // base^(digit-1)-i^digit*k-(i-1)^digit*(j-k)
                    BigSbs(&param->minSumRem[i - 1][j][k],
                        &param->powAndMul[i][k],
                        &param->powAndMul[i - 1][j - k], param);
            }
            ++param->digit;
        }
    }
    param->ok = true;
    return true;
}

void Search(NarNumParam *param)
{
    Big sum = { 0 };
    assert(param != NULL);
    if (!param->ok) return;
    SubSearch(param->base - 1, param->digit, &sum, param);
    if (sum.data)
        free(sum.data);
}

void SubSearch(BaseType cur, DigitType remain, const Big* sum, NarNumParam *param)
{
    assert(sum != NULL);
    assert(param != NULL);

    if (!param->ok)
        return;

    if (!remain) // select finished.
        Check(sum, param);
    else if (!cur)
    {// select 0 for remain digits.
        param->count[0] = remain;
        Check(sum, param);
        param->count[0] = 0;
    }
    else if (PreCheck(cur, remain, sum, param))
    {
        int nstart, nend;
        Big *start, *tmp;
        DigitType size;
        assert(param->maxSumRem != NULL);
        assert(param->minSumRem != NULL);

        // Binary search. Descending.
        // Find the max nstart:  sum<base^digit-cur^digit*nstart
        start = param->maxSumRem[cur];
        size = remain + 1;
        while (size)
        {
            tmp = start + (size >> 1);
            if (BigCmp(tmp, sum) > 0)
            {
                start = tmp + 1;
                size -= (size >> 1) + 1;
            }
            else
                size >>= 1;
        }
        nstart = (int)(start - 1 - param->maxSumRem[cur]);
        assert(nstart <= (int)remain);

        // Binary search. Descending.
        // Find the min nend: sum>=base^(digit-1)-cur^digit*nend-(cur-1)^digit*(remain-nend)
        start = param->minSumRem[cur - 1][remain];
        size = remain + 1;
        while (size)
        {
            tmp = start + (size >> 1);
            if (BigCmp(tmp, sum) > 0)
            {
                start = tmp + 1;
                size -= (size >> 1) + 1;
            }
            else
                size >>= 1;
        }
        nend = (int)(start - param->minSumRem[cur - 1][remain]);
        assert(nend >= 0);

        while (nstart >= nend)
        {
            param->count[cur] = (DigitType)nstart;
            if (!nstart) // Have not selected the number(cur).
                SubSearch(cur - 1, remain - (DigitType)nstart, sum, param);
            else if (!param->powAndMul)
            {
                Release(param);
                return;
            }
            else // Have selected nstart times of the number(cur).
            {
                Big b = { 0 };
                BigAdd(&b, sum, &param->powAndMul[cur][nstart], param);
                SubSearch(cur - 1, remain - (DigitType)nstart, &b, param);
                if (b.data)
                    free(b.data);
            }
            --nstart;
        }
    }
}

void Check(const Big* sum, NarNumParam *param)
{
    DigitType *tmp;
    DigitType i;

    assert(sum != NULL);
    assert(param != NULL);

    tmp = (DigitType*)calloc(param->base, sizeof(DigitType));
    for (i = sum->size; i-- > 0;)
        ++tmp[sum->data[i]];
    if (!memcmp(param->count, tmp, param->base*sizeof(DigitType)))
#pragma omp critical
    {
        Big *tmp_res = NULL;
        assert(param->res != NULL);
        if (param->showProgress)
        {
            Print(sum, param->base,stderr);
            fputc('\n',stderr);
        }
        tmp_res = (Big*)realloc(*param->res, (*param->resSize + 1)*sizeof(Big));
        if (!tmp_res)
        {
            Release(param);
            return;
        }
        *param->res = tmp_res;
        BigCpy(*param->res + (*param->resSize)++, sum);
    }
    free(tmp);
}

void Print(const Big* sum, const BaseType base,FILE *stream)
{
    DigitType i;

    assert(sum != NULL);

    if (!sum->size)
        fputc('0',stream);
    else
    {
        for (i = sum->size; i-- > 0;)
        {
            if (base < 37)
                fputc(sum->data[i] < 10 ? sum->data[i] + '0' : sum->data[i] + 'A' - 10,stream);
            else
                fprintf(stream,"%u%s", sum->data[i], i ? "," : "");
        }
    }
}

bool PreCheck(BaseType cur, DigitType remain, const Big* sum, NarNumParam *param)
{
    assert(cur > 0);
    assert(remain > 0);
    assert(sum != NULL);
    assert(param != NULL);

    if (!param->powAndMul)
    {
        Release(param);
        return 0;
    }

    if (sum->size < param->digit)
        return true;
    else
    {// several high digits of the final sum can be ensured.
        Big max = { 0 };
        DigitType i,
            lastSameDigit,     // The same digits of sum and max:
                               // param->digit<the digit of sum<=lastSameDigit
            countOfLittle = 0; // The count of the number which is less than
                               // cur in the the same digits of sum and max.

        DigitType *tmp = (DigitType*)calloc(param->base, sizeof(DigitType));

        BigAdd(&max, sum, &param->powAndMul[cur][remain], param);
        if (max.data[param->digit - 1] != sum->data[param->digit - 1])
        {// The digits of max is larger than the digits of sum.
         // None of high digits of the final sum can be ensured yet.
            free(tmp);
            if (max.data)
                free(max.data);
            return true;
        }

        for (lastSameDigit = param->digit - 1; lastSameDigit > 0; --lastSameDigit)
            if (max.data[lastSameDigit - 1] != sum->data[lastSameDigit - 1])
                break;

        for (i = param->digit; i-- > lastSameDigit;)
            ++tmp[max.data[i]];

        for (i = param->base - 1; i > cur; --i)
        {
            if (tmp[i] > param->count[i])
            {
                free(tmp);
                if (max.data)
                    free(max.data);
                return false;
            }
        }

        for (i = 0; i <= cur; ++i)
            countOfLittle += tmp[i];

        free(tmp);
        if (max.data)
            free(max.data);

        return remain >= countOfLittle;
    }
}

void BigAdd(Big* r, const Big* a, const Big* b, NarNumParam *param)
{
    const Big *h = a, *l = b;
    DigitType i = 0;
    unsigned char c = 0;
    assert(a != NULL);
    assert(b != NULL);
    assert(r != NULL);
    if (a->size < b->size) { h = b; l = a; }
    if (!h->size)return;
    r->size = h->size;
    r->data = (BaseType*)calloc(h->size + 1, sizeof(BaseType));
    if (!r->data)
    {
        Release(param);
        return;
    }
    while (i < l->size)
    {
        r->data[i] = h->data[i] + l->data[i] + c;
        if (r->data[i] < param->base)
            c = 0;
        else
        {
            c = 1;
            r->data[i] -= param->base;
        }
        ++i;
    }
    while (i < h->size && c && h->data[i] == param->base - 1)
        ++i;
    if (i < h->size)
    {
        r->data[i] = h->data[i] + c;
        memcpy(&r->data[i + 1], &h->data[i + 1], sizeof(BaseType)*(h->size - i - 1));
    }
    else if (c)
        r->data[r->size++] = 1;
}

void BigSub(Big* r, const Big* a, NarNumParam *param)
{
    assert(r != NULL);
    assert(a != NULL);
    assert(param != NULL);
    if (a->size > param->digit)
    {
        r->size = 0;
        if (a->size != param->digit + 1 || a->data[param->digit] != 1)
            r->neg = true;
        else
        {
            DigitType i = 0;
            while (i < param->digit&&!a->data[i])
                ++i;
            r->neg = i < param->digit;
        }
    }
    else
    {
        DigitType i = 0;
        if (!a->size)
        {
            r->size = param->digit + 1;
            r->data = (BaseType*)calloc(param->digit + 1, sizeof(BaseType));
            r->data[param->digit] = 1;
        }
        else
        {
            r->data = (BaseType*)calloc(param->digit, sizeof(BaseType));
            while (i < a->size&&!a->data[i])
                r->data[i++] = 0;
            r->data[i] = param->base - a->data[i];
            while (++i < a->size)
                r->data[i] = param->base - 1 - a->data[i];
            while (i < param->digit)
                r->data[i++] = param->base - 1;
            r->size = param->digit;
            while (r->size&&!r->data[r->size - 1])
                --r->size;
        }
    }
}

void BigSbs(Big* r, const Big* a, const Big* b, NarNumParam *param)
{
    assert(r != NULL);
    assert(a != NULL);
    assert(b != NULL);
    assert(param != NULL);
    if (a->size >= param->digit || b->size >= param->digit)
        r->neg = true;
    else
    {
        Big t = { 0 };
        BigAdd(&t, a, b, param);
        BigSub(r, &t, param);
        if (t.data)
            free(t.data);
    }
}

void BigPow(Big* a, BaseType x, DigitType n, NarNumParam *param)
{
    DigitType i;
    DoubleBaseType r;
    assert(a != NULL);
    a->neg = false;
    if (!x)
        a->size = 0;
    else if (1 == x)
    {
        a->data = (BaseType*)malloc(sizeof(BaseType));
        a->size = 1;
        a->data[0] = 1;
    }
    else
    {
        a->data = (BaseType*)malloc(sizeof(BaseType)*param->digit);
        a->size = 1;
        a->data[0] = x;
        while (--n)
        {
            r = 0;
            for (i = 0; i < a->size; ++i)
            {
                r = (DoubleBaseType)a->data[i] * x + r;
                a->data[i] = (BaseType)(r % param->base);
                r /= param->base;
            }
            if (r)
                a->data[a->size++] = (BaseType)r;
        }
    }
}

int  BigCmp(const Big* a, const Big* b)
{
    DigitType i;
    if (a->size > b->size)
        return 1;
    else if (a->size < b->size)
        return -1;
    for (i = a->size; i-- > 0;)
        if (a->data[i] > b->data[i])
            return 1;
        else if (a->data[i] < b->data[i])
            return -1;
    return 0;
}

void BigCpy(Big* a, const Big* b)
{
    a->neg = b->neg;
    a->size = b->size;
    a->data = (BaseType*)malloc(sizeof(BaseType)*a->size);
    memcpy(a->data, b->data, a->size*sizeof(BaseType));
}

DigitType GetMaxDigit(BaseType base)
{
    // ((base-1)^n*n).size>=n
    DigitType n = 1, i, j;
    DoubleDigitType r;
    NarNumParam param;
    param.base = base;

    assert(base >= 2);
    for (n = 1; n; n <<= 1)
    {
        Big t = { 0 };
        DigitType sz = 0;

        param.digit = n;
        BigPow(&t, base - 1, n, &param);
        for (i = 0, r = 0; i < t.size; ++i)
            r = ((DoubleDigitType)t.data[i] * n + r) / base;
        while (r) { ++sz; r /= base; }
        free(t.data);
        if (t.size + sz < n)
            break;
    }
    n >>= 1;
    for (j = n >> 1; j; j >>= 1)
    {
        Big t = { 0 };
        DigitType sz = 0;

        param.digit = n |= j;
        BigPow(&t, base - 1, n, &param);
        for (i = 0, r = 0; i < t.size; ++i)
            r = ((unsigned long long)t.data[i] * n + r) / base;
        while (r) { ++sz; r /= base; }
        free(t.data);
        if (t.size + sz < n)
            n &= ~j;
    }
    return n;
}

void Release(NarNumParam *param)
{
    assert(param != NULL);
    if (!(param->count&&param->powAndMul&&param->maxSumRem&&param->minSumRem))
    {
        BaseType i;
        DigitType j, k;
        if (param->count)
            free(param->count);
        if (param->powAndMul)
        {
            for (i = 0; i < param->base; ++i)
                if (param->powAndMul[i])
                {
                    for (j = 0; j <= param->digit; ++j)
                        if (param->powAndMul[i][j].data)
                            free(param->powAndMul[i][j].data);
                    free(param->powAndMul[i]);
                }
            free(param->powAndMul);
        }
        if (param->maxSumRem)
        {
            for (i = 0; i < param->base; ++i)
                if (param->maxSumRem[i])
                {
                    for (j = 0; j <= param->digit; ++j)
                        if (param->powAndMul[i][j].data)
                            free(param->powAndMul[i][j].data);
                    free(param->maxSumRem[i]);
                }
            free(param->maxSumRem);
        }
        if (param->minSumRem)
        {
            for (i = 0; i < param->base - 1; ++i)
                if (param->minSumRem[i])
                {
                    for (j = 0; j <= param->digit; ++j)
                        if (param->minSumRem[i][j])
                        {
                            for (k = 0; k <= j; ++k)
                                if (param->minSumRem[i][j][k].data)
                                    free(param->minSumRem[i][j][k].data);
                            free(param->minSumRem[i][j]);
                        }
                    free(param->minSumRem[i]);
                }
            free(param->minSumRem);
        }
        param->count = NULL;
        param->powAndMul = param->maxSumRem = NULL;
        param->minSumRem = NULL;
    }
    param->ok = false;
}

void BigsFree(Big** nums, size_t size)
{
    assert(nums);
    if (*nums)
    {
        while (size--)
            if ((*nums)[size].data)
                free((*nums)[size].data);
        free(*nums);
        *nums = NULL;
    }
}
