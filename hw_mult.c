#include <stdlib.h> // rand, srand
#include <time.h> // time
#include <stdio.h> // printf, scanf

// returns a random digit from 2 to 9
int iDigit(void)
{
    return rand() % 8 + 2;
}

void clearBuffer(void)
{
	char c = 'a';

	while (c != '\n' && c != EOF)
		scanf("%c", &c);

	printf("ERROR: enter an integer.\n");
}

int main(void)
{
    int aDatabase[8][8] = {{0}};
    int i;
    int iCorrectAnswers = 0;

    srand(time(NULL));

    for (i = 0; i < 5; i++)
    {
        int iFirst, iSecond, iResult;
	char cInput;
        iFirst = iDigit();
        iSecond = iDigit();

        // check the database
        if (aDatabase[iFirst - 2][iSecond - 2])
        {
            i--;
            continue;
        }

        aDatabase[iFirst - 2][iSecond - 2] = 1;

        // ask the question 
        printf("%d * %d? ", iFirst, iSecond);
        iResult = 0;
	cInput = '0';

	while (cInput != '\n' && cInput != EOF)
	{
		// input incorrect
		if (cInput < '0' || cInput > '9')
		{
			clearBuffer();
			iResult = 0;
			cInput = '0';
			continue;
		}

		// input correct
		iResult = (iResult * 10) + (cInput - '0');
        	scanf("%c", &cInput);
	}

        if (iResult == (iFirst * iSecond))
        {
            iCorrectAnswers += 1;
            printf("Success!! ^_^\n");
        }
        else
            printf("Wroong! :P\n");
    }

    printf("Correct answers: %d out of 5\n", iCorrectAnswers);
    return 0;
}
