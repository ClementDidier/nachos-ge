/**
*
* @file    IziAssert.h
*
* @authors Floran NARENJI-SHESHKALANI
*
* @date    24/11/2014
*
* @version V1.0
*
* @brief  IZI_ASSERT
*
**/
#pragma once
#define true 1
#define false 0
#define bool int
#include <stdio.h>
#include <string.h>
#define IZI_ASSERT(condition) iziAssert(condition, #condition, __FILE__, __LINE__, __func__);

void iziAssert(bool success, const char* condition, const int lineNumber, const char* functionName)
    {

      char str[200];
      strcpy(str,(success ? "\033[32mASSERTION SUCCESS: \n" : "\033[31mASSERTION FAILED: \n"));
      strcat(str,"\tFunction: ");
      strcat(str, functionName);
      strcat(str, "\n\tCondition: ");
      strcat(str, condition);
      strcat(str, "\n\tLine: ");
      char tmp[5];
      sprintf(tmp,"%d",lineNumber);
      strcat(str, tmp);
      strcat(str, "\n\033[0m");
      strcat(str, "\n");
      printf("\n%s",str);

    }
