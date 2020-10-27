#include <stdio.h>

//
// Strings
//

char str[80] = "abcdef";

// a b c d e f \0 (garbage)
// \0 != '0', '0' == 48

// 'x' != "x"
// "x" == 'x', '\0'

char str2[] = "ghjkl";

char *p_str = "xywz";

scanf("%79s", str); // читай первые 79 символов
// без этого scanf может выйти за границы str

int strlen(char *str)
{
    char *p = str;

    while (*str)
        str++;

    return str - p;
}

int strlen2(char *str)
{
    int i;

    for (i = 0; *(str + i); i++);

    return i;
}

// lib
// #include <string.h>

int strlen(char *str); // len w/o '\0'
int strcmp(char *str1. char *str2); // сравнение строк в лексикографическом порядке
// возвращает 0 - строки равны
// больше 0 - первая строка больше второй
// меньше 0 - первая строка меньше второй
// лексикографический порядок - "z" > "abcdef"

strcat(str1, str2, n); // concantenation

strcat(str1 + 1, str2 + 2);

// sprintf - printf but to string instead of stdout
char s[100];

sprintf(s, "%d", int);

// WARNING: might exceed the bounds of "s"

snprintf(s, n, "%d %d x=%f", int, int, float);

// scanf from str
sscanf(str, "%d %d", &i, &j);

// считывает строку из входного потока
char* gets(char* str);
gets(str)
scanf("%["\n"]s", str); // equivalent

char* strchr(char *str, int ch); // адрес первого вхождения символа char (??? type mismatch)
char* strrchr(char* str, int ch); // адрес последнего

// второе вхождение
char c = 'b';
char str[15] = "abcdebf";
strchr(strchr(str, c) + 1, c);

// strstr - начало вхождения строки в строку
char* strstr(char* str_A, char* str_B);
