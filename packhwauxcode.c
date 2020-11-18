#include <stdio.h> // printf
#include <stdlib.h> // dynamic memory
//#include <math.h> // pow

// throws a memory allocation error
void memalloc_error()
{
    printf("memory allocation error\n");
    exit(1);
}

void pack_num(const int *,int,  char **, int *, char **, int *);
void unpack_num(char *, int , char *, int);

void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
}

void print_darr_bin_int(int *arr, int arr_len)
{
    int i;

    for (i = 0; i < arr_len; i++) {
        int temp = *(arr + i);
        printBits(sizeof(temp), &temp);
        printf(" ");
    }

    printf("\n");
}

int main()
{
	int *nums; /* буфер для считанных чисел*/
	char *pack, *aux; /*массивы для упакованных данных и дополнительной инф*/
	int nums_len;
	int pack_len, aux_len; /*длины массивов*/

	// считываем числа с клавиатуры, записываем в nums
    nums_len = 0;
    int num;

    nums = (int* ) malloc(sizeof(int));
    if (nums == NULL)
        memalloc_error();

    char c = 0;
    num = 0;
    char sign = 1;

    scanf("%c", &c);

    while (1)
    {
        if (c == ' ' || c == '\n') // end of number
        {
            num *= sign;
            sign = 1;

            nums = (int* ) realloc(nums, (nums_len + 1) * sizeof(int));
            if (nums == NULL)
                memalloc_error();

            *(nums + nums_len) = num;
            nums_len++;
            num = 0;

            // loop condition
            if (c == '\n')
                break;

            scanf("%c", &c);
            continue;
        }

        if (c == '-')
            sign = -1;
        else if (c >= '0' && c <= '9')
        {
            num *= 10;
            num += (c - '0');
        }
        else
        {
            printf("ERROR: Enter an integer.\n");
            return 1;
        }

        scanf("%c", &c);
    }

	pack_num(nums, nums_len, &pack, &pack_len, &aux, &aux_len); /*упаковка*/
	printf("pack_len = %d, aux_len = %d\n", pack_len, aux_len);
	unpack_num(pack, pack_len, aux, aux_len); /*распечатка распакованных чисел*/

  //  for (i = 0; i < (nums_len - 1); i++)
        free(nums);

	return 0;
}

void print_darr_bin(char *arr, int arr_len)
{
    int i;

    for (i = 0; i < arr_len; i++) {
        char temp = *(arr + i);
        printBits(sizeof(temp), &temp);
        printf(" ");
    }

    printf("\n");
}

char calc_digits(unsigned int num, int sign)
{
    char digits = 0;

    do
    {
        digits++;
        num >>= 1;
    } while (num);

    return digits;
}

void pack_num(const int *nums, int nums_len, char **pack, int *pack_len, char **aux, int *aux_len)
{
    *pack = (char* ) malloc(sizeof(char));
    if (*pack == NULL)
        memalloc_error();

    *aux = (char* ) malloc(sizeof(char));
    if (*aux == NULL)
        memalloc_error();

    *pack_len = 0;
    *aux_len = 0;

	int i, j;
    char aux_free = 8; // amount of unused bytes in a char
    int pack_taken = 0;
	
	for (i = 0; i < nums_len; i++) {
		int temp = *(nums + i);
        int sign = temp < 0;
		char digits;
        //printf("digits of %d: %d\n", temp, digits);
        
        if (sign)
        {
            temp *= -1;
        }

		digits = calc_digits(temp, sign);

        // move all digits to the beginning
        while (temp && temp > 0)
        {
            temp <<= 1;
        }

        for (j = 0; j < digits; j++)
        {
			if (pack_taken == sizeof(char)*8) {
                (*pack_len)++;
                pack_taken = 0;
                *pack = (char* ) realloc(*pack, ((*pack_len) + 1) * sizeof(char));
                if (*pack == NULL)
                    memalloc_error();
			}

			*(*pack + *pack_len) <<= 1;
			*(*pack + *pack_len) += (temp < 0);
            pack_taken++;
			temp <<= 1;
		}

        digits <<= 1;
        digits += sign;

        if (aux_free >= 7) {
            aux_free = aux_free - 7; 
            digits <<= aux_free;
            *(*aux + *aux_len) += digits;
        } else {
            // break "digits" into 2 parts
            char left_part = digits >> (7 - aux_free); // first (aux_free) digits

            char right_mask = (1 << (7 - aux_free)) - 1; // 1 <<= 6
            char right_part = digits & right_mask; // last (5 - aux_free) digits

            aux_free += 1;
            *(*aux + *aux_len) += left_part;
            (*aux_len)++;
            *aux = (char* ) realloc(*aux, (*aux_len + 1) * sizeof(char));
            if (*aux == NULL)
                memalloc_error();
            right_part <<= aux_free;
            *(*aux + *aux_len) += right_part;
        }
	}

    *(*pack + *pack_len) <<= (sizeof(char)*8) - pack_taken;

    (*pack_len)++;
    (*aux_len)++;

    //printf("aux:");
    //print_darr_bin(*aux, *aux_len);

    // pack_start;
    printf("packed:");
    print_darr_bin(*pack, *pack_len);
}

char read_bin_digit(char* c, char* read)
{
    char digit = (*c < 0);
    *c <<= 1;
    (*read)++;
    return digit;
}

int read_bin_digits(char** c, char* read, char amt, char* limit)
{
    int num = 0;
    int i = 0;

    for (i = 0; i < amt; i++)
    {
        if (*read == 8)
        {
            *read = 0;
            (*c)++;

            if (*c > limit)
                return 0;
        }

        num <<= 1;
        num += read_bin_digit(*c, read);
    }

    return num;
}

void unpack_num(char* pack, int pack_len, char* aux, int aux_len)
{
    char* aux_ptr = aux;
    char aux_read = 0;
    char* aux_limit = aux + aux_len - 1;

    char* pack_ptr = pack;
    char pack_read = 0;
    char* pack_limit = pack + pack_len - 1;

    printf("unpacked:\n");

    // first condition: out of bounds w/ less than 5 unread bits remaining
    while (aux_ptr <= aux_limit)
    {
        char digits = read_bin_digits(&aux_ptr, &aux_read, 6, aux_limit);
        if (digits == 0) // second condition: read_bin_digits failed or read 0
            break;

        char sign = read_bin_digits(&aux_ptr, &aux_read, 1, aux_limit);

        //printf("%d ", digits);
        int num = read_bin_digits(&pack_ptr, &pack_read, digits, pack_limit);

        if (sign)
            num *= -1;

        printf("%d\n", num);
    }

    free(pack);
    pack = NULL;
    free(aux);
    aux = NULL;
}
