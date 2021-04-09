char in_arr(int *dict, int cnt, int entry)
{
    for (int i = 0; i < cnt; i++)
    {
        if (dict[i] == entry)
            return 1;
    }

    return 0;
}
