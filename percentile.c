#include "percentile.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

static int cmp(const int64_t *a, const int64_t *b)
{
    return (int) (*a - *b);
}

int64_t percentile(int64_t *a, double which, size_t size)
{
    qsort(a, size, sizeof(int64_t), (int (*)(const void *, const void *)) cmp);
    size_t array_position = (size_t)((double) size * (double) which);
    assert(array_position < size);
    return a[array_position];
}
