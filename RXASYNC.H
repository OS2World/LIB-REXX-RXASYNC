/*****************************************************************************/
/*                                                                           */
/*  MODULE         RXASYNC.H                                                 */
/*                                                                           */
/*  VERSION        Version 2.0 - 26th Dec 1994                               */
/*                                                                           */
/*  COPYRIGHT      Copyright (c) 1993, 1994 by Crucial Applications          */
/*                             All rights reserved.                          */
/*                                                                           */
/*  DESCRIPTION    Header file for RXASYNC.C Rexx Functions                  */
/*                                                                           */
/*****************************************************************************/

#ifndef RXASYNC_H_INCLUDED
#define RXASYNC_H_INCLUDED

/*********************************************************************/
/* General definitions                                               */
/*********************************************************************/

#define  RXASYNC_DLL_NAME    "RXASYNC" /* what this DLL is called    */

/*********************************************************************/
/* Function Return Codes to REXX and others.                         */
/*********************************************************************/

// #define  NO_ERROR                   0
// #define  ERROR_NOT_ENOUGH_MEMORY    8
// #define  ERROR_GEN_FAILURE         31
// #define  ERROR_INVALID_PARAMETER   87

/*********************************************************************/
/* Function Prototypes                                               */
/*********************************************************************/

VOID   MyDropFuncs(  LONG entries );
BOOL   MyLoadFuncs(  VOID );

/*********************************************************************/

#endif

/*********************************************************************/
/* END MODULE                                                        */
/*********************************************************************/
