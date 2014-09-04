/*****************************************************************************/
/*                                                                           */
/*  MODULE         MISCFUNC.H                                                */
/*                                                                           */
/*  VERSION        Version 2.0 - 26th Dec 1994                               */
/*                                                                           */
/*  COPYRIGHT      Copyright (c) 1993, 1994 by Crucial Applications          */
/*                             All rights reserved.                          */
/*                                                                           */
/*  DESCRIPTION    Header file for MISCFUNC.C Miscellaneous Functions        */
/*                                                                           */
/*****************************************************************************/

#ifndef MISCFUNC_H_INCLUDED
#define MISCFUNC_H_INCLUDED

/*********************************************************************/
/* General definitions                                               */
/*********************************************************************/

#define  EOSTR_CH        0x00          /* End of string              */
#define  LINEFEED_CH     0x0A          /* Line feed                  */
#define  FORMFEED_CH     0x0C          /* Form feed                  */
#define  CARRIAGE_CH     0x0D          /* Carriage return            */
#define  EOFILE_CH       0x1A          /* End of file                */
#define  EMPTY_STR       ""            /* Empty string               */
#define  NULL_STR        NULL          /* Null string pointer        */

#define  YESNO_OPTIONS   "YyNn"        /* Possible Yes/No values     */
#define  YES_OPTIONS     "Yy"          /* Possible Yes values        */
#define  NO_OPTIONS      "Nn"          /* Possible No values         */

/*********************************************************************/
/* Definitions for Str2Num()                                         */
/*********************************************************************/

#define  MAX_DIGITS          9         /* maximum digits numeric arg */
#define  MAX_BYTE_DEC        255
#define  MAX_SHORT_DEC       32767
#define  MAX_USHORT_DEC      65535
#define  MAX_LONG_DEC        2147483647
#define  MAX_ULONG_DEC       4294967295
#define  DIGIT_CHARS         "0123456789ABCDEFabcdef"

#define  STR2NUM_STR2STR      0
#define  STR2NUM_BIN2BYTE     1
#define  STR2NUM_BIN2SHORT    2
#define  STR2NUM_BIN2USHORT   3
#define  STR2NUM_BIN2LONG     4
#define  STR2NUM_BIN2ULONG    5
#define  STR2NUM_HEX2BYTE     6
#define  STR2NUM_HEX2SHORT    7
#define  STR2NUM_HEX2USHORT   8
#define  STR2NUM_HEX2LONG     9
#define  STR2NUM_HEX2ULONG   10
#define  STR2NUM_OCT2BYTE    11
#define  STR2NUM_OCT2SHORT   12
#define  STR2NUM_OCT2USHORT  13
#define  STR2NUM_OCT2LONG    14
#define  STR2NUM_OCT2ULONG   15
#define  STR2NUM_DEC2BYTE    16
#define  STR2NUM_DEC2SHORT   17
#define  STR2NUM_DEC2USHORT  18
#define  STR2NUM_DEC2LONG    19
#define  STR2NUM_DEC2ULONG   20

/*********************************************************************/
/* Definitions for Val2Str()                                         */
/*********************************************************************/

#define  VAL2STR_STR2STR      0
#define  VAL2STR_HFILE2DEC    1
#define  VAL2STR_BYTE2DEC     2
#define  VAL2STR_SHORT2DEC    3
#define  VAL2STR_USHORT2DEC   4
#define  VAL2STR_LONG2DEC     5
#define  VAL2STR_ULONG2DEC    6
#define  VAL2STR_BYTE2HEX     7
#define  VAL2STR_SHORT2HEX    8
#define  VAL2STR_USHORT2HEX   9
#define  VAL2STR_LONG2HEX    10
#define  VAL2STR_ULONG2HEX   11

/*********************************************************************/
/* Definitions for MsgBox()                                          */
/*********************************************************************/

#define MSG_STACK_SIZE        8192
#define MSG_MAX_LENGTH        255
#define MSG_TITLE_LEN         60

#define MSG_DEFAULT_TITLE     "A message from RxAsync.Dll by Ian Timms"

#define MSG_ICON_STYLES       "NnIiWwQqSs"
#define MSG_BUTTON_STYLES     "VvIiWwQqSsRrAa"

#define MSG_RESPONSE_OK       0
#define MSG_RESPONSE_CANCEL   1
#define MSG_RESPONSE_IGNORE   2
#define MSG_RESPONSE_HELP     3

#pragma pack(1)                        /* pack on 1 byte boundary    */

typedef struct _MYMSG                  /* msg                        */
{
   TID      tidMyMsg;                  /* Thread ID                  */
   HAB      habMyMsg;                  /* Anchor block               */
   HMQ      hmqMyMsg;                  /* Message queue              */
   HWND     hwndMyMsg;                 /* Owned by window            */
   CHAR     szTitle[MSG_TITLE_LEN+1];  /* Message box title          */
   CHAR     chIcon;                    /* Message box icon to use    */
   CHAR     chButtons;                 /* Message box type to use    */
   CHAR     chHelp;                    /* Message box help wanted    */
   USHORT   usNumber;                  /* Message number for help use*/
   CHAR     szText[MSG_MAX_LENGTH+1];  /* Message text itself        */
   USHORT   usReply;                   /* Reply to be returned       */
} MYMSG;
typedef MYMSG *PMYMSG;

/*********************************************************************/
/* Prototypes                                                        */
/*********************************************************************/

BOOL Str2Num( PSZ NumStr, PVOID pNumber, USHORT NumType );

VOID Val2Str( PVOID pValue, PSZ ValStr, USHORT ValType );

LONG MyDiffTime( PDATETIME Tnow, PDATETIME Tthen );

APIRET SetPriority( ULONG ulClass, LONG lLevel );

APIRET MsgBox( PSZ pszTitle, CHAR chIcon, CHAR chButtons, CHAR chHelp, USHORT usNumber, PSZ pszText );

VOID MsgBoxThread( PVOID arg );

VOID StripNulls( PSZ pszBuffer, PULONG pulLength );

VOID ReplaceNulls( PSZ pszBuffer, ULONG ulLength, BYTE bReplace );

/*********************************************************************/

#endif

/*********************************************************************/
/* END MODULE                                                        */
/*********************************************************************/
