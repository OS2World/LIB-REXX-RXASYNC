/*****************************************************************************/
/*                                                                           */
/*  MODULE         RXVPFUNC.H                                                */
/*                                                                           */
/*  VERSION        Version 2.0 - 26th Dec 1994                               */
/*                                                                           */
/*  COPYRIGHT      Copyright (c) 1993, 1994 by Crucial Applications          */
/*                             All rights reserved.                          */
/*                                                                           */
/*  DESCRIPTION    Header file for RXVPFUNC.C Rexx Variable Pool Functions   */
/*                                                                           */
/*****************************************************************************/

#ifndef RXVPFUNC_H_INCLUDED
#define RXVPFUNC_H_INCLUDED

/*********************************************************************/
/* General definitions                                               */
/*********************************************************************/

#define  ROUTINE_VALID             0   /* Successful completion      */
#define  ROUTINE_INVALID          40   /* Raise Rexx error           */

#define  RC_REXX_FALSE             0   /* Rexx result for false      */
#define  RC_REXX_TRUE              1   /* Rexx result for true       */

/*********************************************************************/
/* Some useful macros.                                               */
/*********************************************************************/

#define BUILDRXSTRING(t,s) { \
   strcpy((t)->strptr,(s)); \
   (t)->strlength = strlen((t)->strptr); \
}

#define BUILDRXDOSRET(t,i) { \
   sprintf((t)->strptr, "%d", i); \
   (t)->strlength = strlen((t)->strptr); \
}

/*********************************************************************/
/* Prototypes                                                        */
/*********************************************************************/

VOID   InitVar(   PSHVBLOCK pRexxVar, PSHVBLOCK pNextPtr, PSZ VarName );
APIRET FetchVar(  PSHVBLOCK pRexxVar );
VOID   EditVar(   PSHVBLOCK pRexxVar, USHORT VarType, PVOID VarValue );
APIRET UpdateVar( PSHVBLOCK pRexxVar );
VOID   FreeVar(   PSHVBLOCK pRexxVar );

APIRET NewRexxValue( PSZ VarName, USHORT VarType, PVOID VarValue );
APIRET GetRexxValue( PSZ VarName, USHORT VarType, PVOID VarValue );
APIRET SetRexxValue( PSZ VarName, USHORT VarType, PVOID VarValue );

APIRET NewStemValue( PSZ VarStem, USHORT VarNumb, USHORT VarType, PVOID VarValue );
APIRET GetStemValue( PSZ VarStem, USHORT VarNumb, USHORT VarType, PVOID VarValue );
APIRET SetStemValue( PSZ VarStem, USHORT VarNumb, USHORT VarType, PVOID VarValue );

/*********************************************************************/

#endif

/*********************************************************************/
/* END MODULE                                                        */
/*********************************************************************/
