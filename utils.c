#include "utils.h"

void LongArrSum(const unsigned long *arr, size_t capacity, unsigned long *ptr)
{
    unsigned long res = 0;
    for (size_t i = 0; i < capacity; i++)
    {
        res = res + arr[i];
    }
    *ptr = res;
}
