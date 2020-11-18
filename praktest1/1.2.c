// DEC_SAT - decrease n1 by n2, return 0 if result is negative

int DEC_SAT(int n1, int n2)
{
    // even if we don't use hex, we can achieve the same result
    // with "1 << 7".
    char mask_sign = 0x80;
    char sign;

    n1 = n1 - n2;
    sign = n1 & mask_sign;

    if (sign)
        return 0;

    return n1;
}
