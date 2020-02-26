/*
 * TMU_Test.c
 *
 *  Created on: Feb 26, 2020
 *      Author: Khaled
 */


#include "TMU/TMU.h"


uint8_t gu8_Timer_Init_Return;
uint8_t gu8_Timer_Start_Return;
uint8_t gu8_Timer_Stop_Return;
uint8_t gu8_Timer_Get_Value_Return;
uint8_t gu8_Timer_Set_Value_Return;
uint8_t gu8_Timer_Get_Status_Return;

int main (void)
{
	uint8_t au8_Test_Func_Number = ZERO;
	//line to correct error in IDE related to printf and scanf functions
	setvbuf(stdout,NULL,_IONBF,0);

	printf("choose 1 for Motor_Init \n");
	printf("choose 2 for Motor_Direction \n");
	printf("choose 3 for Motor_Start \n");

	printf("The number of the desired function to test is ");
	scanf("%d",&au8_Test_Func_Number);

	switch (au8_Test_Func_Number)
	{
	case 1:
		;
		break;

	case 2:
		;
		break;

	case 3:
		;
		break;
	default:
		printf("Invalid function number");
		break;
	}



	return 0;
}
