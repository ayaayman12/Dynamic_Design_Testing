#include <stdio.h>
#include <stdlib.h>
#include "TMU/TMU.h"

#include "TMU_Test_Casses.h"




int main (void)
{
	uint8_t au8_Test_Func_Number = ZERO;
	//line to correct error in IDE related to printf and scanf functions
	setvbuf(stdout,NULL,_IONBF,0);
	//Test_Consumer();
	printf("Please, Enter The number of the desired function to test...\n");
	printf("choose 1 for TMU_Init \n");
	printf("choose 2 for TMU_Start \n");
	printf("choose 3 for TMU_Stop \n");
	printf("choose 4 for TMU_Dinit \n");
    printf("\n\n");
	printf("The number of the desired function to test is ");
	scanf("%d",&au8_Test_Func_Number);
	printf("\n\n");

	switch (au8_Test_Func_Number)
	{
        case 1: TMU_Init_Test(); break;
        case 2: TMU_Start_Test(); break;
        case 3: TMU_Stop_Test(); break;
        case 4: TMU_Dinit_Test(); break;
        default: printf("Invalid function number"); break;
	}
	return 0;
}


