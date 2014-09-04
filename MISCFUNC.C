//*****************************************************************************/
/*                                                                           */
/*  MODULE         MISCFUNC.C                                                */
/*                                                                           */
/*  VERSION        Version 2.0 - 26th Dec 1994                               */
/*                                                                           */
/*  COPYRIGHT      Copyright (c) 1993, 1994 by Crucial Applications          */
/*                             All rights reserved.                          */
/*                                                                           */
/*  DESCRIPTION    Miscellaneous Support Functions                           */
/*                                                                           */
/*  FUNCTIONS                                                                */
/*                                                                           */
/*    Str2Num      - Convert specified string to a number                    */
/*    Val2Str      - Convert specified value to a string                     */
/*    MyDiffTime   - Return difference between two times                     */
/*    SetPriority  - Set current thread priority                             */
/*    MsgBox       - Generic PM message box function                         */
/*    MsgBoxThread - Thread function for MsgBox routine                      */
/*                                                                           */
/*****************************************************************************/

/*********************************************************************/
/* Includes                                                          */
/*********************************************************************/

#define  _MT
#define  _DLL
#define  INCL_DOS
#define  INCL_ERRORS

#include <os2.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "miscfunc.h"

/*********************************************************************/
/* Function:  Str2Num( numstr, numptr, numtype )                     */
/*                                                                   */
/* Purpose:   Validates and converts an ASCII-Z string from string   */
/*            form to designated value.  Returns FALSE if the string */
/*            is not valid, TRUE if the string was successfully      */
/*            converted.                                             */
/*                                                                   */
/*            Source types             Target types                  */
/*            ----------------------   ----------------------        */
/*            BINARY    '01001100'     BYTE                          */
/*            OCTAL     '765'          SHORT                         */
/*            DECIMAL   '12345'        USHORT                        */
/*            HEX       '0A'           LONG                          */
/*                                     ULONG                         */
/*                                     PSZ                           */
/*                                                                   */
/* Return:    TRUE - Good string converted                           */
/*            FALSE - Invalid string supplied.                       */
/*********************************************************************/
BOOL Str2Num( PSZ NumStr, PVOID pNumber, USHORT NumType )
{
   ULONG  Tnumber;
   ULONG  Tmax;
   USHORT Trad;
   USHORT Tlen;
   USHORT Tind;
   USHORT Tpos;
   SHORT  Tsgn;
   CHAR   Tdig[2];

   Tnumber = 0;
   Tmax    = 0;
   Trad    = 0;
   Tlen    = 0;
   Tind    = 0;
   Tpos    = 0;
   Tsgn    = 0;
   Tdig[0] = EOSTR_CH;
   Tdig[1] = EOSTR_CH;

   switch( NumType )
   {
      case STR2NUM_STR2STR:    /* string  to string */ Tlen =  0; Trad =  0; Tmax = 0;              break;
      case STR2NUM_BIN2BYTE:   /* binary  to byte   */ Tlen =  8; Trad =  2; Tmax = MAX_BYTE_DEC;   break;
      case STR2NUM_BIN2SHORT:  /* binary  to short  */ Tlen = 16; Trad =  2; Tmax = MAX_SHORT_DEC;  break;
      case STR2NUM_BIN2USHORT: /* binary  to ushort */ Tlen = 16; Trad =  2; Tmax = MAX_USHORT_DEC; break;
      case STR2NUM_BIN2LONG:   /* binary  to long   */ Tlen = 32; Trad =  2; Tmax = MAX_LONG_DEC;   break;
      case STR2NUM_BIN2ULONG:  /* binary  to ulong  */ Tlen = 32; Trad =  2; Tmax = MAX_ULONG_DEC;  break;
      case STR2NUM_OCT2BYTE:   /* octal   to byte   */ Tlen =  3; Trad =  8; Tmax = MAX_BYTE_DEC;   break;
      case STR2NUM_OCT2SHORT:  /* octal   to short  */ Tlen =  6; Trad =  8; Tmax = MAX_SHORT_DEC;  break;
      case STR2NUM_OCT2USHORT: /* octal   to ushort */ Tlen =  6; Trad =  8; Tmax = MAX_USHORT_DEC; break;
      case STR2NUM_OCT2LONG:   /* octal   to long   */ Tlen =  9; Trad =  8; Tmax = MAX_LONG_DEC;   break;
      case STR2NUM_OCT2ULONG:  /* octal   to ulong  */ Tlen =  9; Trad =  8; Tmax = MAX_ULONG_DEC;  break;
      case STR2NUM_DEC2BYTE:   /* decimal to byte   */ Tlen =  3; Trad = 10; Tmax = MAX_BYTE_DEC;   break;
      case STR2NUM_DEC2SHORT:  /* decimal to short  */ Tlen =  5; Trad = 10; Tmax = MAX_SHORT_DEC;  break;
      case STR2NUM_DEC2USHORT: /* decimal to ushort */ Tlen =  5; Trad = 10; Tmax = MAX_USHORT_DEC; break;
      case STR2NUM_DEC2LONG:   /* decimal to long   */ Tlen = 10; Trad = 10; Tmax = MAX_LONG_DEC;   break;
      case STR2NUM_DEC2ULONG:  /* decimal to ulong  */ Tlen = 10; Trad = 10; Tmax = MAX_ULONG_DEC;  break;
      case STR2NUM_HEX2BYTE:   /* hex     to byte   */ Tlen =  2; Trad = 16; Tmax = MAX_BYTE_DEC;   break;
      case STR2NUM_HEX2SHORT:  /* hex     to short  */ Tlen =  4; Trad = 16; Tmax = MAX_SHORT_DEC;  break;
      case STR2NUM_HEX2USHORT: /* hex     to ushort */ Tlen =  4; Trad = 16; Tmax = MAX_USHORT_DEC; break;
      case STR2NUM_HEX2LONG:   /* hex     to long   */ Tlen =  8; Trad = 16; Tmax = MAX_LONG_DEC;   break;
      case STR2NUM_HEX2ULONG:  /* hex     to ulong  */ Tlen =  8; Trad = 16; Tmax = MAX_ULONG_DEC;  break;
      default:
           return( FALSE );
   }

   if ( Tlen > 0 ) {

      while (isspace( *NumStr )) { NumStr++; } /* skip leading blanks */

      if (*NumStr == '+') { NumStr++; } /* skip positive sign */

      if (*NumStr == '-') {
         switch( NumType )
         {
            case STR2NUM_DEC2SHORT:
            case STR2NUM_DEC2LONG:
                 /* to short or long */
                 break;
            default:
                 return( FALSE );
         }
         Tsgn = -1;
         NumStr++;
      } /* set negative and skip sign */

      if (strlen( NumStr ) == 0) { return( FALSE ); } /* not a number */

      Tind = 0;
      Tpos = 0;

      do {
         Tdig[0] = NumStr[0];
         Tind = strcspn( DIGIT_CHARS, Tdig );

         if ((Trad == 16) && (Tind > 15)) { Tind = Tind - 6; } /* lowercase hex adjust */

         if (Tind < Trad)
         {
            Tnumber = Tnumber*Trad + Tind;

            if (Tnumber > (Tmax - Tsgn)) { return( FALSE ); } /* exceeds bounds */

            Tpos = Tpos + 1;
            NumStr++;

         } /* else not a valid digit or end of string */

      } while ((Tpos < Tlen) && (Tind < Trad));

      while (isspace( *NumStr )) { NumStr++; }     /* skip trailing blanks */

      if (*NumStr != EOSTR_CH) { return( FALSE ); } /* not a number */
   }

   switch( NumType )
   {
      case STR2NUM_STR2STR:
           /* to string */
           strcpy( pNumber, NumStr );
           break;
      case STR2NUM_BIN2BYTE:
      case STR2NUM_OCT2BYTE:
      case STR2NUM_DEC2BYTE:
      case STR2NUM_HEX2BYTE:
           /* to byte   */
           *((PBYTE) pNumber)   = (BYTE) Tnumber;
           break;
      case STR2NUM_BIN2SHORT:
      case STR2NUM_OCT2SHORT:
      case STR2NUM_DEC2SHORT:
      case STR2NUM_HEX2SHORT:
           /* to short  */
           if (Tsgn < 0) {
              *((PSHORT) pNumber)  = (SHORT) 0 - (SHORT) Tnumber;
           } else {
              *((PSHORT) pNumber)  = (SHORT) Tnumber;
           }
           break;
      case STR2NUM_BIN2USHORT:
      case STR2NUM_OCT2USHORT:
      case STR2NUM_DEC2USHORT:
      case STR2NUM_HEX2USHORT:
           /* to ushort */
           *((PUSHORT) pNumber) = (USHORT) Tnumber;
           break;
      case STR2NUM_BIN2LONG:
      case STR2NUM_OCT2LONG:
      case STR2NUM_DEC2LONG:
      case STR2NUM_HEX2LONG:
           /* to long   */
           if (Tsgn < 0) {
              *((PLONG) pNumber)   = (LONG) 0 - (LONG) Tnumber;
           } else {
              *((PLONG) pNumber)   = (LONG) Tnumber;
           }
           break;
      case STR2NUM_BIN2ULONG:
      case STR2NUM_OCT2ULONG:
      case STR2NUM_DEC2ULONG:
      case STR2NUM_HEX2ULONG:
           /* to ulong  */
           *((PULONG) pNumber)  = (ULONG) Tnumber;
           break;
      default:
           return( FALSE );
   }

   return( TRUE );
}

/*********************************************************************/
/* Function:  Val2Str( valptr, valstr, valtype )                     */
/*                                                                   */
/* Purpose:   Converts value of specified type to an ASCII-Z string. */
/*                                                                   */
/* Return:    none                                                   */
/*********************************************************************/
VOID Val2Str( PVOID pValue, PSZ ValStr, USHORT ValType )
{
   ValStr[0] = EOSTR_CH;

   switch( ValType )
   {
      case VAL2STR_STR2STR:    strcpy(  ValStr,          (PSZ)pValue        ); break;
      case VAL2STR_HFILE2DEC:  sprintf( ValStr,    "%d", *((PHFILE)pValue)  ); break;

      case VAL2STR_BYTE2DEC:   sprintf( ValStr,    "%d", *((PBYTE)pValue)   ); break;
      case VAL2STR_SHORT2DEC:  sprintf( ValStr,    "%d", *((PSHORT)pValue)  ); break;
      case VAL2STR_USHORT2DEC: sprintf( ValStr,    "%d", *((PUSHORT)pValue) ); break;
      case VAL2STR_LONG2DEC:   sprintf( ValStr,    "%d", *((PLONG)pValue)   ); break;
      case VAL2STR_ULONG2DEC:  sprintf( ValStr,    "%d", *((PULONG)pValue)  ); break;

      case VAL2STR_BYTE2HEX:   sprintf( ValStr, "%2.2X", *((PBYTE)pValue)   ); break;
      case VAL2STR_SHORT2HEX:  sprintf( ValStr, "%4.4X", *((PSHORT)pValue)  ); break;
      case VAL2STR_USHORT2HEX: sprintf( ValStr, "%4.4X", *((PUSHORT)pValue) ); break;
      case VAL2STR_LONG2HEX:   sprintf( ValStr, "%8.8X", *((PLONG)pValue)   ); break;
      case VAL2STR_ULONG2HEX:  sprintf( ValStr, "%8.8X", *((PULONG)pValue)  ); break;

      default:
           strcpy( ValStr, (PSZ)pValue );
           break;
   }

   return;
}

/*********************************************************************/
/* Function:  MyDiffTime( &now, &then )                              */
/*                                                                   */
/* Purpose:   Calculates the difference in hundredths of a second    */
/*            between two specified times. The maximum difference    */
/*            permitted is 23hrs 59min 99/100ths.                    */
/*                                                                   */
/* Return:    difference in milliseconds.                            */
/*********************************************************************/
LONG MyDiffTime(
        PDATETIME Tnow,
        PDATETIME Tthen )
{
   LONG  Tdiff;
   SHORT Tdays, Thours, Tmins, Tsecs, Thunds;

   Tdays   = Tnow->weekday    - Tthen->weekday;
   Thours  = Tnow->hours      - Tthen->hours;
   Tmins   = Tnow->minutes    - Tthen->minutes;
   Tsecs   = Tnow->seconds    - Tthen->seconds;
   Thunds  = Tnow->hundredths - Tthen->hundredths;

   if (Thunds  < 0) { Thunds  = Thunds  + 100; Tsecs   = Tsecs   - 1; }
   if (Tsecs   < 0) { Tsecs   = Tsecs   + 60;  Tmins   = Tmins   - 1; }
   if (Tmins   < 0) { Tmins   = Tmins   + 60;  Thours  = Thours  - 1; }
   if (Thours  < 0) { Thours  = Thours  + 24;  Tdays   = Tdays   - 1; }
   if (Tdays   < 0) { Tdays   = Tdays   + 7; }

   if (Tdays != 0)
      Tdiff = -1;    /* do not handle, too long a period */
   else
      Tdiff = Thunds
            + (100*Tsecs)
            + (100*60*Tmins)
            + (100*60*60*Thours);

   return( Tdiff*10 );                 /* return as milliseconds     */
}

/*********************************************************************/
/* SetPriority()                                                     */
/*********************************************************************/
APIRET SetPriority( ULONG ulClass, LONG lLevel )
{
   APIRET    rc;                        /* creation return code       */

   rc = DosSetPrty( PRTYS_THREAD,
                    ulClass,
                    lLevel,
                    0 );               /* current process/thread     */

   if ( rc != NO_ERROR ) {
      fprintf( stdout, "DosSetPrty(%d,%d) failed with RC=%d.\r\n", ulClass, lLevel, rc );
   }

   return( rc );                       /* Return DosSetPriority rc   */

} /* end SetPriority() */

/*********************************************************************/
/* MsgBox()                                                          */
/*********************************************************************/
APIRET MsgBox(
          PSZ    pszTitle,
          CHAR   chIcon,
          CHAR   chButtons,
          CHAR   chHelp,
          USHORT usNumber,
          PSZ    pszText )
{
   USHORT  usReply;
   MYMSG   msgMyMsg;
   PMYMSG  pMsg;

   pMsg = &msgMyMsg;

   /* Set message struct with parameters */

   pMsg->tidMyMsg   = NULLHANDLE;
   pMsg->habMyMsg   = NULLHANDLE;
   pMsg->hmqMyMsg   = NULLHANDLE;
   pMsg->hwndMyMsg  = NULLHANDLE;

   if ( pszTitle == NULL ) {
      strcpy( pMsg->szTitle, MSG_DEFAULT_TITLE );
   } else {
      strncpy( pMsg->szTitle, pszTitle, MSG_TITLE_LEN );
      (pMsg->szTitle)[MSG_TITLE_LEN] = EOSTR_CH;
   }

   pMsg->chIcon    = chIcon;
   pMsg->chButtons = chButtons;
   pMsg->chHelp    = chHelp;
   pMsg->usNumber  = usNumber;

   if ( pszText == NULL ) {
      strcpy( pMsg->szText, EMPTY_STR );
   } else {
      strcpy( pMsg->szText, pszText );
   }

   pMsg->usReply = MSG_RESPONSE_IGNORE;

   // if msgno not zero then build msg string from msgno
   // using msgtext for parameters (linefeed delimited),
   // any excess parameters are to be ignored, any
   // missing parameters need to be replaced with '<?>'

   pMsg->usNumber = 0;                  /* no help just yet */

   pMsg->tidMyMsg = _beginthread( MsgBoxThread, NULL, MSG_STACK_SIZE, (PVOID) pMsg );

   if ( pMsg->tidMyMsg == -1 ) {
      DosBeep(750,500);
   } else {
      DosWaitThread( &(pMsg->tidMyMsg), DCWW_WAIT );
   }

   usReply = pMsg->usReply; /* now we can extract the reply */

   return( usReply );

} /* end MsgBox() */

/*********************************************************************/
/* MsgBoxThread()                                                    */
/*********************************************************************/
VOID MsgBoxThread( PVOID arg )
{
   /*****************************************************/
   /* There is never an owner for the message box,      */
   /* we don't even know if there is message queue etc. */
   /* so we create a thread and allocate a hab and hmq. */
   /*****************************************************/

   USHORT usRC;
   USHORT usAttrs;
   PMYMSG pMsg;

   pMsg = (PMYMSG) arg;

   pMsg->habMyMsg = WinInitialize( NULLHANDLE );
   if ( pMsg->habMyMsg != NULLHANDLE )
   {
      pMsg->hmqMyMsg = WinCreateMsgQueue( pMsg->habMyMsg, NULLHANDLE );
      if ( pMsg->hmqMyMsg != NULLHANDLE )
      {

         switch( pMsg->chIcon )
         {
            case 'N':
            case 'n':
                 usAttrs = MB_NOICON;
                 break;
            case 'I':
            case 'i':
                 usAttrs = MB_ICONASTERISK;
                 break;
            case 'W':
            case 'w':
                 usAttrs = MB_ICONEXCLAMATION;
                 break;
            case 'Q':
            case 'q':
                 usAttrs = MB_ICONQUESTION;
                 break;
            case 'S':
            case 's':
                 usAttrs = MB_ICONHAND;
                 break;
            default:
                 usAttrs = MB_ICONHAND;
                 break;
         }

         switch( pMsg->chButtons )
         {
            case 'V':
            case 'v':
                 usAttrs |= MB_ENTER;
                 break;
            case 'I':
            case 'i':
                 usAttrs |= MB_OK;
                 break;
            case 'W':
            case 'w':
                 usAttrs |= MB_OKCANCEL;
                 break;
            case 'Q':
            case 'q':
                 usAttrs |= MB_YESNO;
                 break;
            case 'S':
            case 's':
                 usAttrs |= MB_CANCEL;
                 break;
            case 'R':
            case 'r':
                 usAttrs |= MB_RETRYCANCEL;
                 break;
            case 'A':
            case 'a':
                 usAttrs |= MB_ABORTRETRYIGNORE;
                 break;
            default:
                 usAttrs |= MB_CANCEL;
                 break;
         }

         if (   (pMsg->usNumber != 0)
             || (strchr(YES_OPTIONS, pMsg->chHelp) != NULL) )
         {
            usAttrs |= MB_HELP;
         }

         usRC = WinMessageBox( HWND_DESKTOP,
                               pMsg->hwndMyMsg,
                               (PSZ) pMsg->szText,
                               (PSZ) pMsg->szTitle,
                               pMsg->usNumber,
                               usAttrs |
                               MB_MOVEABLE |
                               MB_CUANOTIFICATION );
         switch( usRC )
         {
            case MBID_OK:
            case MBID_YES:
            case MBID_ENTER:
            case MBID_RETRY:
                 pMsg->usReply = MSG_RESPONSE_OK;
                 break;

            case MBID_CANCEL:
            case MBID_NO:
            case MBID_ERROR:
            case MBID_ABORT:
                 pMsg->usReply = MSG_RESPONSE_CANCEL;
                 break;

            case MBID_HELP:
                 pMsg->usReply = MSG_RESPONSE_HELP;
                 break;

            default: /* MBID_IGNORE */
                 pMsg->usReply = MSG_RESPONSE_IGNORE;
                 break;
         }

         WinDestroyMsgQueue( pMsg->hmqMyMsg );
         pMsg->hmqMyMsg = NULLHANDLE;
      }
      WinTerminate( pMsg->habMyMsg );
      pMsg->habMyMsg = NULLHANDLE;
   }

   _endthread();

} /* end MsgBoxThread() */

/*********************************************************************/
/* StripNulls()                                                      */
/* Note: This function requires addition of DBCS handling so that    */
/*       DBCS trail bytes are not stripped.                          */
/*********************************************************************/
VOID StripNulls( PSZ pszBuffer, PULONG pulLength )
{
   ULONG i, j;
   i = 0;
   for ( j=0; j < *pulLength; j++ ) {  /* for each character in buff */
      if (pszBuffer[j] != EOSTR_CH) {  /* if it is not a null        */
         pszBuffer[i] = pszBuffer[j];  /* then keep it               */
         i++;                          /* and skip to next position  */
      }
   }
   pszBuffer[i] = EOSTR_CH;            /* set the new end of string  */
   *pulLength = i;                     /* set the new length         */
   return;
}

/*********************************************************************/
/* ReplaceNulls()                                                    */
/* Note: This function requires addition of DBCS handling so that    */
/*       DBCS trail bytes are not replaced.                          */
/*********************************************************************/
VOID ReplaceNulls( PSZ pszBuffer, ULONG ulLength, BYTE bReplace )
{
   ULONG i;
   for ( i=0; i < ulLength; i++ ) {    /* for each character in buff */
      if (pszBuffer[i] == EOSTR_CH) {  /* if it is a null            */
         pszBuffer[i] = bReplace;      /* then replace it            */
      }
   }
   return;
}

/*********************************************************************/
/* END MODULE                                                        */
/*********************************************************************/
