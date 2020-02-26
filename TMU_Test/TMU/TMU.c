/*
 * TMU.c
 *
 * Created: 2/24/2020 3:03:53 PM
 *  Author: Khaled
 */


 #include "TMU.h"

 /*
 *** Local Macros
 */

 /* BUFFER DUTY VALUES */
 #define USED 1
 #define NOT_USED 0
 /* to determine if the flags are initialized or not*/
 #define NOT_INITIALIZED 0
 #define INITIALIZED 1
 /* initial value for the first empty location in the buffer*/
 #define EMPTY_BUFFER_LOCATION_INITIAL_VALUE 0
 #define TICK_COUNTER_INITAIL_VALUE 1

  /* this struct is used to update the status of TMU */
  typedef struct str_FuncStatus_t
  {
	  uint8_t u8_Init_Flag;
	  uint8_t u8_Start_Flag;
	  uint8_t u8_Stop_Flag;
	  uint8_t u8_Deinit_Flag;
  }str_FuncStatus_t;

  /* this struct is used to determine the characteristics of each task */
  typedef struct str_TMU_Buffer_t
  {

	  uint8_t u8_Duty;                  /* whether the task is used or not */
	  uint16_t u8_Delay_Time;           /* the wanted delay time */
	  uint8_t u8_Repeat;                /* whether the task is repeated or one shot */
	  void(* Ptr_Consumer)(void);       /* pointer to the consumer function */
	  uint8_t u8_Pre_Flag;              /* count the time for consumer function until reach it's periodicity  */
	  uint8_t u8_Task_ID;               /* determine the Id for the consumer task */

  }str_TMU_Buffer_t;

/*********************************************************/
/************ definition of global variables ************/
/*******************************************************/

/* create TMU buffer to serve functions "array of structures"*/
static str_TMU_Buffer_t arrstr_TMU_Buffer [TMU_BUFFER_SIZE];

/* create global instance of str_FuncStatus_t  */
static str_FuncStatus_t  str_FuncStatus;

/* to count elements in the TMU buffer*/
uint8_t gu8_Buffer_Counter = ZERO;

static uint8_t gu8_Ch_ID = ZERO;
static uint8_t	gu8_Tick_Flag = LOW;




/********************************************************/
/************ prototype for local functions ************/
/******************************************************/

static ERROR_STATUS Is_Buffer_Empty(void);
static ERROR_STATUS Is_Buffer_Full(void);
static void Timer_CB (void);

/************************************************************/
/************ implementation of local functions ************/
/**********************************************************/

/* this function is used to check if TMU buffer is empty or not */
static ERROR_STATUS Is_Buffer_Empty(void)
	{
		uint8_t u8_Ret_Status = FALSE;
		if (gu8_Buffer_Counter == 0)
		{
			u8_Ret_Status = TRUE;
		}
		else
		{
			u8_Ret_Status = FALSE;
		}
		return u8_Ret_Status;
	}

/* this function is used to check if TMU buffer is full or not */
static ERROR_STATUS Is_Buffer_Full(void)
	{
		uint8_t u8_Ret_Status = FALSE;
		if (gu8_Buffer_Counter == TMU_BUFFER_SIZE)
		{
			u8_Ret_Status=TRUE;
		}
		else
		{
			u8_Ret_Status=FALSE;
		}
		return u8_Ret_Status;
	}

/* this function is called when timer over flow occur, this function is used to set a specific flag */
 void Timer_CB (void)
	{
		Timer_Set_Value(gu8_Ch_ID, 6);
		gu8_Tick_Flag = HIGH;

	}


	/****************************************************************/
	/************ implementation of interface functions ************/
	/**************************************************************/

/* this function is used to initialize TMU */
 ERROR_STATUS TMU_Init (const str_TMU_InitConfig_t * pstr_ConfigPtr )
	{
		uint8_t Error = E_OK;
		if (pstr_ConfigPtr!= NULL)
		{
			if (str_FuncStatus.u8_Init_Flag == NOT_INITIALIZED)
			{
				/* set TMU status, already initialized */
				str_FuncStatus.u8_Init_Flag= INITIALIZED;

				/*Timer Configurations*/
				Timer_Cfg_ST str_TimerCfg;
				str_TimerCfg.Timer_Ch = (pstr_ConfigPtr->u8_Timer_Id);
				str_TimerCfg.Timer_Mode = TIMER_MODE;
				str_TimerCfg.Timer_PreScaler = PRESCALER_64;
				str_TimerCfg.Timer_Interrupt_Mode = TIMER_MODE_INTERRUPT;
				str_TimerCfg.Ptr_TCB_Function = Timer_CB;

				Timer_Init(&str_TimerCfg);
				/*store timer id in global variable to use in TMU_Start*/
				gu8_Ch_ID = pstr_ConfigPtr->u8_Timer_Id;
			}
			else
			{
				Error_Push (TMU_MODULE, ERROR_ALREADY_INITIALIZED);
				Error = E_NOK;
			}


		}
		else
		{
			Error_Push (TMU_MODULE, ERROR_NULL_POINTER);
			Error = E_NOK;
		}

		return Error;
	}

/* this function is used to check timing for each task*/
 ERROR_STATUS TMU_Dispatch(void)
	{
		uint8_t u8_Buffer_Index=0;
		static uint8_t u8_Flag=0;
		uint8_t Error = E_OK;
		if (HIGH == gu8_Tick_Flag)
		{
		/*Search in TMU buffer if there is function to server this tick */
		for (u8_Buffer_Index=0;u8_Buffer_Index<TMU_BUFFER_SIZE && u8_Flag<gu8_Buffer_Counter;u8_Buffer_Index++)
		{
			if(arrstr_TMU_Buffer[u8_Buffer_Index].u8_Duty == USED)
			{
				arrstr_TMU_Buffer[u8_Buffer_Index].u8_Pre_Flag++;

				/*Check if this task will serve this tick*/
				if  (arrstr_TMU_Buffer[u8_Buffer_Index].u8_Pre_Flag == (arrstr_TMU_Buffer[u8_Buffer_Index].u8_Delay_Time) )
				{

					arrstr_TMU_Buffer[u8_Buffer_Index].Ptr_Consumer();
					arrstr_TMU_Buffer[u8_Buffer_Index].u8_Pre_Flag = 0;

					/*chechk if this task desired for one shot remove it from TMU buffer*/
					if (ONE_SHOT == arrstr_TMU_Buffer[u8_Buffer_Index].u8_Repeat)
					{
					   TMU_Stop((arrstr_TMU_Buffer[u8_Buffer_Index].u8_Task_ID), (arrstr_TMU_Buffer[u8_Buffer_Index].Ptr_Consumer));
					}
				}
			}
		}
		/*clear the tick flag to let the timer call back function set it again and call the dispatch*/
		gu8_Tick_Flag = LOW;
		}

		return Error;
	}

/* this function is used to stop a specific function */
 ERROR_STATUS TMU_Stop(uint8_t Task_Id, void(* Ptr_Func)(void))
	{
		uint8_t au8_Search_Loop_Counter = ZERO;
		uint8_t Error = E_OK;

		if (NULL != Ptr_Func)
		{
		    if (FALSE == Is_Buffer_Empty())
		    {
			    /* Search for about given task*/
			    for (au8_Search_Loop_Counter = ZERO; au8_Search_Loop_Counter < TMU_BUFFER_SIZE; au8_Search_Loop_Counter ++)
			    {

				    /*Check if this task is the desired to stop*/
				    if ((Task_Id == arrstr_TMU_Buffer[au8_Search_Loop_Counter].u8_Task_ID) && (Ptr_Func == arrstr_TMU_Buffer[au8_Search_Loop_Counter].Ptr_Consumer))
				    {
					    /*stop the task*/
					    arrstr_TMU_Buffer[au8_Search_Loop_Counter].u8_Duty = NOT_USED;
					    arrstr_TMU_Buffer[au8_Search_Loop_Counter].Ptr_Consumer = NULL;
					    /*decrease the buffer*/
					    gu8_Buffer_Counter --;
					    break;
				    }
			    }
			    /*if the buffer empty after this decrease stop the timer to stop receiving timer interrupt */
			    if (TRUE == Is_Buffer_Empty())
			    Timer_Stop(Timer_0);
		    }
		    else
		   {
		      Error_Push (TMU_MODULE, ERROR_EMPTY_BUFFER);
				Error = E_NOK;
		   }
		}
		else
		{
		   Error_Push (TMU_MODULE, ERROR_NULL_POINTER);
			Error = E_NOK;
		}

		return Error;
	}

/* this function is used to start a specific function */
 ERROR_STATUS TMU_Start (uint16_t Time_Delay,uint8_t Task_Id, uint8_t Repeat, void(* Ptr_Func)(void))
  {
     /*variable for linear search algorithm*/
	  uint8_t au8_Search_Loop_Counter = ZERO;
	  sint8_t aS8_Empty_Buffer_Location = EMPTY_BUFFER_LOCATION_INITIAL_VALUE;
	  uint8_t au8_Already_Started = NOT_INITIALIZED;
	  uint8_t Error = E_OK;

	  if (INITIALIZED == str_FuncStatus.u8_Init_Flag)
	  {
	     if (FALSE == Is_Buffer_Full())
		  {
		      if (NULL != Ptr_Func)
		      {

			      /* Search for empty location and if the consumer is already started in the TMU buffer*/
			      for (au8_Search_Loop_Counter = ZERO; au8_Search_Loop_Counter < TMU_BUFFER_SIZE; au8_Search_Loop_Counter ++)
			      {
				      /* save the first empty location in TMU buffer*/
				      if ((NOT_USED == arrstr_TMU_Buffer[au8_Search_Loop_Counter].u8_Duty) && (aS8_Empty_Buffer_Location == EMPTY_BUFFER_LOCATION_INITIAL_VALUE))
				      {
					      aS8_Empty_Buffer_Location = au8_Search_Loop_Counter;
				      }
				      /*Check if the consumer is pre started with the same ID*/
				      if ((Task_Id == arrstr_TMU_Buffer[au8_Search_Loop_Counter].u8_Task_ID) && (Ptr_Func == arrstr_TMU_Buffer[au8_Search_Loop_Counter].Ptr_Consumer))
				      {
					      au8_Already_Started = INITIALIZED;
					      break;
				      }
			      }


			      if (au8_Already_Started == NOT_INITIALIZED)
			      {
				      /*Create struct for the new creator*/
				      str_TMU_Buffer_t str_Buffer;
				      str_Buffer.u8_Duty = USED;
				      str_Buffer.u8_Delay_Time = Time_Delay;
				      str_Buffer.u8_Repeat = Repeat;
				      str_Buffer.Ptr_Consumer =  Ptr_Func;
				      str_Buffer.u8_Pre_Flag = 1;
				      str_Buffer.u8_Task_ID = Task_Id;

				      /*Insert the created struct for the new creator in the TMU buffer*/
				      arrstr_TMU_Buffer[aS8_Empty_Buffer_Location] = str_Buffer;

						/*if the buffer empty start the timer */
						if (TRUE == Is_Buffer_Empty())
				      Timer_Start(Timer_0,250);

						/*increment buffer counter before adding new task*/
						gu8_Buffer_Counter++;




			      }
					//else for restart same task
			      else if (au8_Already_Started == INITIALIZED)
			      {
				      aS8_Empty_Buffer_Location = EMPTY_BUFFER_LOCATION_INITIAL_VALUE;
				      SET_BIT(PORTA_DATA,BIT6);
				      //return error
			      }

				}


				else
				{
				   Error_Push (TMU_MODULE, ERROR_NULL_POINTER);
					Error = E_NOK;
				}
		  }

		  else
		  {
		     Error_Push (TMU_MODULE, ERROR_FULL_BUFFER);
			  Error = E_NOK;
		  }

	  }

	  else
	  {
	     Error_Push (TMU_MODULE, ERROR_NOT_INITIALIZED);
		  Error = E_NOK;
	  }



	  return Error;
  }

/* this function is used to deinitialize TMU */
 ERROR_STATUS TMU_DeInit (void)
{
	uint8_t Error = E_OK;

	/*check if the module was initialized early*/
	if (INITIALIZED == str_FuncStatus.u8_Init_Flag)
	{
		/*check if the TMU buffer is empty to keep the tasks correct*/
		if (TRUE == Is_Buffer_Empty())
		{
			str_FuncStatus.u8_Init_Flag = NOT_INITIALIZED;
			str_FuncStatus.u8_Start_Flag = NOT_INITIALIZED;
			str_FuncStatus.u8_Stop_Flag = NOT_INITIALIZED;

		}
		else
		{
		   Error_Push (TMU_MODULE, ERROR_NOT_EMPTY_BUFFER);
		   Error = E_NOK;
		}
	}
	else
	{
	   Error_Push (TMU_MODULE, ERROR_NOT_INITIALIZED);
	   Error = E_NOK;
	}
	return Error;
}
