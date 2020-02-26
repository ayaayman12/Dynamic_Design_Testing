/*
 * Errors.h
 *
 * Created: 2/25/2020 10:40:12 AM
 *  Author: ahmed
 */ 


#ifndef ERRORS_H_
#define ERRORS_H_

/***********************************************/
/***************** MODULE NUBERS *****************/
/***********************************************/

#define TMU_MODULE 1


/***********************************************/
/***************** ERRORS NUBERS *****************/
/***********************************************/

#define NO_ERROR                          0
#define ERROR_NULL_POINTER					   NO_ERROR + 1
#define ERROR_INVALID_PARAMETER				ERROR_NULL_POINTER + 1		   
#define ERROR_NOT_INITIALIZED		         ERROR_INVALID_PARAMETER + 1					   
#define ERROR_ALREADY_INITIALIZED	      ERROR_NOT_INITIALIZED + 1						   
#define ERROR_STOP_WITHOUT_START			   ERROR_ALREADY_INITIALIZED + 1				   
#define ERROR_MULTIPLE_START				   ERROR_STOP_WITHOUT_START + 1			   
#define ERROR_FULL_BUFFER					   ERROR_MULTIPLE_START + 1
#define ERROR_NOT_FULL_BUFFER				   ERROR_FULL_BUFFER + 1
#define ERROR_EMPTY_BUFFER					   ERROR_NOT_FULL_BUFFER + 1		   
#define ERROR_NOT_EMPTY_BUFFER			   ERROR_EMPTY_BUFFER + 1							   
							   
							   
							   
#endif /* ERRORS_H_ */