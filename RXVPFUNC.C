/*****************************************************************************/
/*                                                                           */
/*  MODULE         RXVPFUNC.C                                                */
/*                                                                           */
/*  VERSION        Version 2.0 - 26th Dec 1994                               */
/*                                                                           */
/*  COPYRIGHT      Copyright (c) 1993, 1994 by Crucial Applications          */
/*                             All rights reserved.                          */
/*                                                                           */
/*  DESCRIPTION    Rexx Variable Pool Functions                              */
/*                                                                           */
/*  FUNCTIONS                                                                */
/*                                                                           */
/*    InitVar      - Initialise rexx variable structure for fetch            */
/*    FetchVar     - Fetches the specified variables from the variable pool  */
/*    EditVar      - Modify value in specified rexx variable for update      */
/*    UpdateVar    - Updates the specified variables into the variable pool  */
/*    FreeVar      - Free rexx variable memory allocated by rexx             */
/*                                                                           */
/*    NewRexxValue - Create the specified rexx variable                      */
/*    GetRexxValue - Get the value of a specified rexx variable              */
/*    SetRexxValue - Set the value of a specified rexx variable              */
/*                                                                           */
/*    NewStemValue - Create the specified rexx stem variable                 */
/*    GetStemValue - Get the value of a specified rexx stem variable         */
/*    SetStemValue - Set the value of a specified rexx stem variable         */
/*                                                                           */
/*****************************************************************************/

/*********************************************************************/
/* Includes needed by this module                                    */
/*********************************************************************/

#define  _MT
#define  _DLL
#define  INCL_DOS
#define  INCL_ERRORS
#define  INCL_REXXSAA

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <rexxsaa.h>

#include "miscfunc.h"
#include "rxvpfunc.h"

/*********************************************************************/
/* Function:  InitVar( &rexxvar, &nextvar, varname )                 */
/*                                                                   */
/* Purpose:   Initialises Rexx variable with the specified name in   */
/*            preparation for querying the variable pool.            */
/*********************************************************************/
VOID InitVar(
        PSHVBLOCK  pRexxVar,
        PSHVBLOCK  pNextPtr,
        PSZ        VarName )
{
   pRexxVar->shvnext            = pNextPtr;
   pRexxVar->shvname.strptr     = VarName;
   pRexxVar->shvname.strlength  = strlen(VarName);
   pRexxVar->shvvalue.strptr    = NULL;
   pRexxVar->shvvalue.strlength = 0;
   return;
}

/*********************************************************************/
/* Function:  FetchVar( &rexxvar )                                   */
/*                                                                   */
/* Purpose:   Fetches the Rexx variable(s) from the variable pool.   */
/*            Rexx allocates the returned values, we free it later.  */
/*********************************************************************/
APIRET FetchVar( PSHVBLOCK  pRexxVar )
{
   APIRET    rc;                       /* Function return code       */
   APIRET    rxrc;                     /* RexxVariablePool return    */
   PSHVBLOCK pNextPtr;
   pNextPtr = pRexxVar;
   while( pNextPtr != NULL ) {
      pNextPtr->shvcode     = RXSHV_SYFET;
      pNextPtr->shvret      = (UCHAR) 0;
      pNextPtr->shvnamelen  = pNextPtr->shvname.strlength;
      pNextPtr->shvvaluelen = pNextPtr->shvvalue.strlength;
      pNextPtr = pNextPtr->shvnext;
   }
   rxrc = RexxVariablePool( pRexxVar ); /* GET the rexx variable     */
   if ( rxrc == RXSHV_OK    ) {
      rc = NO_ERROR;                   /* all okay                   */
   } else if ( rxrc & RXSHV_MEMFL ) {
      rc = ERROR_NOT_ENOUGH_MEMORY;    /* no memory                  */
   } else {
      rc = ERROR_INVALID_PARAMETER;    /* other failure              */
   }
   return( rc );
}

/*********************************************************************/
/* Function:  EditVar( &rexxvar, type, &value )                      */
/*                                                                   */
/* Purpose:   Modifies the Rexx variable with the specified value in */
/*            preparation for return to the variable pool.           */
/*********************************************************************/
VOID EditVar(
        PSHVBLOCK  pRexxVar,
        USHORT     VarType,
        PVOID      VarValue )
{
   Val2Str( VarValue, pRexxVar->shvvalue.strptr, VarType );
   pRexxVar->shvvalue.strlength = strlen( pRexxVar->shvvalue.strptr );
   return;
}

/*********************************************************************/
/* Function:  UpdateVar( &rexxvar )                                  */
/*                                                                   */
/* Purpose:   Updates the Rexx variable(s) into the variable pool.   */
/*********************************************************************/
APIRET UpdateVar( PSHVBLOCK  pRexxVar )
{
   APIRET    rc;                       /* Function return code       */
   APIRET    rxrc;                     /* RexxVariablePool return    */
   PSHVBLOCK pNextPtr;
   pNextPtr = pRexxVar;
   while( pNextPtr != NULL ) {
      pNextPtr->shvcode     = RXSHV_SYSET;
      pNextPtr->shvret      = (UCHAR) 0;
      pNextPtr->shvvaluelen = pNextPtr->shvvalue.strlength;
      pNextPtr = pNextPtr->shvnext;
   }
   rxrc = RexxVariablePool( pRexxVar ); /* GET the rexx variable     */
   if ( rxrc == RXSHV_OK    ) {
      rc = NO_ERROR;                   /* all okay                   */
   } else if ( rxrc == RXSHV_NEWV ) {
      rc = NO_ERROR;                   /* all new variables          */
   } else if ( rxrc & RXSHV_MEMFL ) {
      rc = ERROR_NOT_ENOUGH_MEMORY;    /* no memory                  */
   } else {
      rc = ERROR_INVALID_PARAMETER;    /* other failure              */
   }
   return( rc );
}

/*********************************************************************/
/* Function:  FreeVar( &rexxvar )                                    */
/*                                                                   */
/* Purpose:   Frees the memory allocated to the Rexx variable value  */
/*            which is allocated when fetching the variable(s) from  */
/*            the variable pool.                                     */
/*********************************************************************/
VOID FreeVar( PSHVBLOCK  pRexxVar )
{
   PSHVBLOCK pNextPtr;
   pNextPtr = pRexxVar;
   while( pNextPtr != NULL ) {
      if ( pNextPtr->shvvalue.strptr != NULL ) {
         DosFreeMem( pNextPtr->shvvalue.strptr );
         pNextPtr->shvvalue.strlength = 0;
         pNextPtr->shvvaluelen = 0;
      }
      pNextPtr = pNextPtr->shvnext;
   }
   return;
}

/*********************************************************************/
/* Function:  rc = NewRexxValue( name, type, &value )                */
/*                                                                   */
/* Purpose:   Initialises rexx variable with the specified name then */
/*            updates the rexx variable with the specified value and */
/*            creates it in the variable pool.                       */
/*                                                                   */
/* Notes:     Currently only handles normal (256byte) values.        */
/*********************************************************************/
APIRET NewRexxValue(
          PSZ       VarName,
          USHORT    VarType,
          PVOID     VarValue )
{
   APIRET   rc;                        /* Function return code       */
   SHVBLOCK RexxVar;                   /* Rexx variable structure    */
   CHAR     VarBuff[RXAUTOBUFLEN];     /* Holder for value           */
   VarBuff[0] = EOSTR_CH;
   InitVar( &RexxVar, NULL, VarName ); /* Initialise for fetch       */
   RexxVar.shvvalue.strptr    = VarBuff;
   RexxVar.shvvalue.strlength = RXAUTOBUFLEN;
   EditVar( &RexxVar, VarType, VarValue );
   rc = UpdateVar( &RexxVar );         /* Create variable            */
   return( rc );
}

/*********************************************************************/
/* Function:  rc = GetRexxValue( name, type, &value )                */
/*                                                                   */
/* Purpose:   Initialises rexx variable with the specified name and  */
/*            fetches its contents from the rexx variable pool then  */
/*            frees the memory which was allocated by the variable   */
/*            pool interface when fetching the variable from it.     */
/*                                                                   */
/*            The value returned is copied into the variable pointed */
/*            to by the VarValue parameter, this memory must be      */
/*            allocated by the caller and be of sufficient length to */
/*            hold the returned value.                               */
/*                                                                   */
/*            The value type returned is determined by the VarType   */
/*            parameter and must be a valid type for the Str2Num     */
/*            function call.                                         */
/*********************************************************************/
APIRET GetRexxValue(
          PSZ       VarName,
          USHORT    VarType,
          PVOID     VarValue )
{
   APIRET   rc;                        /* Function return code       */
   SHVBLOCK RexxVar;                   /* Rexx variable structure    */
   InitVar( &RexxVar, NULL, VarName ); /* Setup variable for fetch   */
   rc = FetchVar( &RexxVar );          /* Fetch variable             */
   if ( rc == NO_ERROR ) {
      if ( !Str2Num( RexxVar.shvvalue.strptr, VarValue, VarType ) ) {
         rc = ERROR_INVALID_PARAMETER;
      }
   }
   FreeVar( &RexxVar );                      /* Free variable        */
   return( rc );
}

/*********************************************************************/
/* Function:  rc = SetRexxValue( name, type, &value )                */
/*                                                                   */
/* Purpose:   Initialises rexx variable with the specified name and  */
/*            fetches its contents from the rexx variable pool then  */
/*            updates the rexx variable with the specified value and */
/*            returns it to the variable pool and finally frees the  */
/*            memory which was allocated by the rexx variable pool   */
/*            interface when fetching the variable from the pool.    */
/*********************************************************************/
APIRET SetRexxValue(
          PSZ       VarName,
          USHORT    VarType,
          PVOID     VarValue )
{
   APIRET   rc;                        /* Function return code       */
   SHVBLOCK RexxVar;                   /* Rexx variable structure    */
   InitVar( &RexxVar, NULL, VarName ); /* Initialise for fetch       */
   rc = FetchVar( &RexxVar );          /* Fetch variable             */
   if ( rc == NO_ERROR ) {
      EditVar( &RexxVar, VarType, VarValue );/* Alter var contents   */
      rc = UpdateVar( &RexxVar );      /* Update variable            */
   }
   FreeVar( &RexxVar );                      /* Free variable        */
   return( rc );
}

/*********************************************************************/
/* Function:  rc = NewStemValue( stem, numb, type, &value )          */
/*                                                                   */
/* Purpose:   Constructs the variable name from the stem and number  */
/*            specified then invokes NewRexxVar to create the value  */
/*            of the variable in the variable pool.                  */
/*                                                                   */
/* Notes:     Currently only handles normal (256byte) names & values.*/
/*********************************************************************/
APIRET NewStemValue(
          PSZ       VarStem,
          USHORT    VarNumb,
          USHORT    VarType,
          PVOID     VarValue )
{
   CHAR     VarName[RXAUTOBUFLEN];     /* Buffer to hold stem name   */
   VarName[0] = EOSTR_CH;
   if ( VarStem[strlen(VarStem)-1] == '.' ) {
      sprintf( VarName,  "%s%d", VarStem, VarNumb );
   } else {
      sprintf( VarName, "%s.%d", VarStem, VarNumb );
   }
   return( NewRexxValue( VarName, VarType, VarValue ) );
}

/*********************************************************************/
/* Function:  rc = GetStemValue( stem, numb, type, &value )          */
/*                                                                   */
/* Purpose:   Constructs the variable name from the stem and number  */
/*            specified then invokes GetRexxVar to retrieve the      */
/*            value of the variable from the variable pool.          */
/*                                                                   */
/* Notes:     Currently only handles normal (256byte) names.         */
/*********************************************************************/
APIRET GetStemValue(
          PSZ       VarStem,
          USHORT    VarNumb,
          USHORT    VarType,
          PVOID     VarValue )
{
   CHAR     VarName[RXAUTOBUFLEN];     /* Buffer to hold stem name   */
   VarName[0] = EOSTR_CH;
   if ( VarStem[strlen(VarStem)-1] == '.' ) {
      sprintf( VarName,  "%s%d", VarStem, VarNumb );
   } else {
      sprintf( VarName, "%s.%d", VarStem, VarNumb );
   }
   return( GetRexxValue( VarName, VarType, VarValue ) );
}

/*********************************************************************/
/* Function:  rc = SetStemValue( stem, numb, type, &value )          */
/*                                                                   */
/* Purpose:   Constructs the variable name from the stem and number  */
/*            specified then invokes SetRexxVar to update the value  */
/*            of the variable in the variable pool.                  */
/*                                                                   */
/* Notes:     Currently only handles normal (256byte) names.         */
/*********************************************************************/
APIRET SetStemValue(
          PSZ       VarStem,
          USHORT    VarNumb,
          USHORT    VarType,
          PVOID     VarValue )
{
   CHAR     VarName[RXAUTOBUFLEN];     /* Buffer to hold stem name   */
   VarName[0] = EOSTR_CH;
   if ( VarStem[strlen(VarStem)-1] == '.' ) {
      sprintf( VarName,  "%s%d", VarStem, VarNumb );
   } else {
      sprintf( VarName, "%s.%d", VarStem, VarNumb );
   }
   return( SetRexxValue( VarName, VarType, VarValue ) );
}

/*********************************************************************/
/* END MODULE                                                        */
/*********************************************************************/
