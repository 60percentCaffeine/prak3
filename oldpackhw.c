#include <stdio.h> // printf
#include <stdlib.h> // dynamic memory

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
//    puts("");
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

int main() {
	int *nums; /* буфер для считанных чисел*/
	char *pack, *aux; /*массивы для упакованных данных и дополнительной инф*/
	int nums_len;
	int pack_len, aux_len; /*длины массивов*/

	// считываем числа с клавиатуры, записываем в nums
    nums_len = 0;
    int num;

    nums = malloc(sizeof(int));
    if (nums == NULL)
        return 1;

    int scanned = scanf("%d", &num);
    while (scanned && num != -1)
    {
        nums = realloc(nums, (nums_len + 1) * sizeof(int));
        if (nums == NULL)
            return 1;

        *(nums + nums_len) = num;
        nums_len++;
        scanned = scanf("%d", &num);
    }

/*
    // print source array
    printf("source:\n");
    int i;

    for (i = 0; i < nums_len; i++)
    {
        printf("%d ", *(nums + i));
    }

    printf("\n");

    // print source array binary
    //print_darr_bin_int(nums, nums_len);
    //printf("\n");
*/

	pack_num(nums, nums_len, &pack, &pack_len, &aux, &aux_len); /*упаковка*/
	printf("pack_len = %d, aux_len = %d\n", pack_len, aux_len);
	unpack_num(pack, pack_len, aux, aux_len); /*распечатка распакованных чисел*/

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

int calc_digits(int num)
{
    int digits = 0;

    while (num)
    {
        digits++;
        num >>= 1;
    }

    return digits;
}

// aAAAAA IT'S PACKING THEM IN REVERSE
void pack_num(const int *nums, int nums_len, char **pack, int *pack_len, char **aux, int *aux_len)
{
    *pack = malloc(sizeof(char));
    *aux = malloc(sizeof(char));
    *pack_len = 0;
    *aux_len = 0;

	int i, j;
    char aux_free = 8; // amount of unused bytes in a char
    int pack_taken = 0;
	
	for (i = 0; i < nums_len; i++) {
		int temp = *(nums + i);
		char digits = calc_digits(temp);
        printf("temp: 0x%08x\n", temp);

        // move all digits to the beginning
        while (temp && temp > 0)
        {
            temp <<= 1;
        }
        printf("temp shfted: 0x%08x\n", temp);

//		while (temp) {
        for (j = 0; j < digits; j++)
        {
			if (pack_taken == sizeof(char)*8) {
                (*pack_len)++;
                pack_taken = 0;
                *pack = realloc(*pack, ((*pack_len) + 1) * sizeof(char));
                if (*pack == NULL)
                    exit(1);
			}

			*(*pack + *pack_len) *= 2;
			*(*pack + *pack_len) += (temp < 0);
            pack_taken++;
			temp <<= 1;
			//digits++;
		}

        if (aux_free >= 5) {
            aux_free = aux_free - 5; 
            digits <<= aux_free;
            *(*aux + *aux_len) += digits;
        } else {
            // break "digits" into 2 parts
            char left_part = digits >> (5 - aux_free); // first (aux_free) digits
            char right_part = digits & ((2 ^ (5 - aux_free)) - 1); // last (5 - aux_free) digits

            aux_free += 3;
            *(*aux + *aux_len) += left_part;
            (*aux_len)++;
            *aux = realloc(*aux, (*aux_len + 1) * sizeof(char));
            right_part <<= aux_free;
            *(*aux + *aux_len) += right_part;
        }
	}

    *(*pack + *pack_len) <<= (sizeof(char)*8) - pack_taken;

    (*pack_len)++;
    (*aux_len)++;

    printf("aux:");
    print_darr_bin(*aux, *aux_len);

    // pack_start;
    printf("packed:");
    print_darr_bin(*pack, *pack_len);
}

void unpack_num(char *pack, int pack_len, char *aux, int aux_len)
{
    char aux_free = 0;
    char pack_read = 0;
    int aux_i = 0;
    int pack_i = 0;
    char oddNum = 0;

    printf("unpacked:\n");

//    while (aux_i < aux_len || (((aux_i + 1) < aux_len) && (aux_free != (8*sizeof(char)))))
    while (1)
    {
        int num = 0;
        char j = 0;
        char len = 0;
        char temp = *(aux + aux_i); //>> (8*sizeof(char) - 5);

        // read len from aux
        for (j = 0; j < 5; j++)
        { // rn it reads right-to-left, should be the other way around -- fixd
            if (aux_free == (8*sizeof(char)))
            {
                //printf("next aux at pack_i=%d j=%d\n", pack_i, j);
                aux_i++;

                // reached end of aux
                if (aux_i == aux_len)
                    return;

                temp = *(aux + aux_i);
                temp <<= oddNum * (8 * sizeof(char) - 3);
                aux_free = 0;
            }

            //printf("temp: 0x%08x\n", temp);
            char bin_digit = temp;
            bin_digit = bin_digit < 0; // first bit is used as a sign

            len = len * 2 + bin_digit;
            temp <<= 1;
            aux_free += 1;
        }

        // ptr is at last element of aux
        // which is fully read
        if (len == 0)
            return;

        if (!temp && aux_i == (aux_len - 1))
            return;

        printf("%d ", len);

        // read num
        temp = *(pack + pack_i);
        num = 0;
        for (j = 0; j < len; j++)
        {
            if (pack_read == (8*sizeof(char)))
            {
                pack_i++;
                temp = *(pack + pack_i);
                pack_read = 0;
            }

            num = num * 2 + (temp < 0);
            temp <<= 1;
            pack_read += 1;
        }

        printf("%d\n", num);
        oddNum = !oddNum;
    }

    printf("\n");
}
