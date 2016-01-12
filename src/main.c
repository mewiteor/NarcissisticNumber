/*
 * copyright (c) 2016 Mewiteor
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <malloc.h>

#include"NarNum.h"

int cmp(const void* a, const void* b)
{
    return BigCmp((const Big*)a, (const Big*)b);
}

int main(int argc, char* argv[])
{
    int i, n, base;
    Big* res = NULL;
    size_t size = 0;
    clock_t c;
    bool showProgress;

    if (argc != 3 ||
        !argv[1][0] ||
        argv[1][1] ||
        ((argv[1][0] | 0x20) != 'n' && (argv[1][0] | 0x20) != 't') ||
        sscanf(argv[2], "%d", &base) != 1)
    {
        fprintf(stderr, "%s <normal mode(n/N)> <base>\n", argv[0]);
        fprintf(stderr, "%s <test mode (t/T)> <base>\n", argv[0]);
        return EXIT_FAILURE;
    }

    argv[1][0] |= 0x20;
    showProgress = argv[1][0] == 'n';

    if (base < 2)
    {
        fputs("Base is too little.", stderr);
        return EXIT_FAILURE;
    }
    if (base > 255)
    {
        fputs("Base is too large.", stderr);
        return EXIT_FAILURE;
    }

    c = clock();
    n = (int)GetMaxDigit((BaseType)base);
#pragma omp parallel for schedule(dynamic)
    for (i = 2; i <= n; ++i)
    {
        NarNumParam param = { 0 };

        if (Init((DigitType)i, (BaseType)base, &res, &size, showProgress, &param))
        {
            Search(&param);
            Release(&param);
        }
    }
    if (showProgress)
        fprintf(stderr,"time:%fs\n", (double)(clock() - c) / CLOCKS_PER_SEC);
    qsort(res, size, sizeof(Big), cmp);
    for (i = 0; i < base; ++i)
    {
        if(argv[1][0] != 't')
            printf("%4d:", i + 1);
        if (i < 37)
            putchar(i < 10 ? i + '0' : i + 'A' - 10);
        else
            printf("%d", i);
        putchar(argv[1][0] != 't'?'\n':' ');
    }
    for (i = 0; i < size; ++i)
    {
        if (argv[1][0] != 't')
            printf("%4d:", i + base + 1);
        Print(res + i, base,stdout);
        putchar(argv[1][0] != 't' ? '\n' : ' ');
    }
    if (argv[1][0] == 't')
        putchar('\n');
    BigsFree(&res, size);
    return EXIT_SUCCESS;
}
