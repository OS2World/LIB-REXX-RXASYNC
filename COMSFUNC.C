/*****************************************************************************/
/*                                                                           */
/*  MODULE         COMSFUNC.C                                                */
/*                                                                           */
/*  VERSION        Version 2.0 - 26th Dec 1994                               */
/*                                                                           */
/*  COPYRIGHT      Copyright (c) 1993, 1994 by Crucial Applications          */
/*                             All rights reserved.                          */
/*                                                                           */
/*  DESCRIPTION    Serial Communications Functions                           */
/*                                                                           */
/*  FUNCTIONS                                                                */
/*                                                                           */
/*    CallDevice     - Invoke specified ASYNC_IOCTL call                     */
/*    ActiveSignal   - Return modem input/output signal status               */
/*    FlushInput     - Flush device driver input queue                       */
/*    FlushOutput    - Flush device driver output queue                      */
/*                                                                           */
/*    GetDcbInfo     - Get the device control block info                     */
/*    SetDcbInfo     - Set the device control block info                     */
/*    GetEnhParm     - Get the enhanced parameters                           */
/*    SetEnhParm     - Set the enhanced parameters                           */
/*    GetLnCtrl      - Get the comms port settings                           */
/*    SetLnCtrl      - Set the comms port settings                           */
/*    GetComStatus   - Get COM status                                        */
/*    GetTxStatus    - Get Transmit status                                   */
/*    GetModemOutput - Get modem output signals                              */
/*    GetModemInput  - Get modem input signals                               */
/*    SetBreakOff    - Set break signal off                                  */
/*    SetBreakOn     - Set break signal on                                   */
/*    TxImmediate    - Transmit specified byte immediately                   */
/*    TxStop         - Stop transmission (ala XOFF)                          */
/*    TxStart        - Start transmission (ala XON)                          */
/*    SetMdmOutput   - Set modem output signals                              */
/*    GetRxCount     - Get size and number of chars in RX queue              */
/*    GetTxCount     - Get size and number of chars in TX queue              */
/*    GetComError    - Get the COM Error Word                                */
/*    GetComEvent    - Get the COM Event Word                                */
/*                                                                           */
/*    PrtDcbInfo     - Print DCB information to stdout                       */
/*                                                                           */
/*    WaitCarrier    - Wait/test for carrier signal                          */
/*    WaitInput      - Wait/test for data input                              */
/*    SendBreak      - Send break for specified duration                     */
/*                                                                           */
/*    OpenPort       - Open the specified comms port                         */
/*    ClosePort      - Close the specified comms port                        */
/*    WritePort      - Write the specified data to the comms port            */
/*    ReadPort       - Read data from the specified comms port               */
/*                                                                           */
/*****************************************************************************/

/*********************************************************************/
/* Includes needed by this module                                    */
/*********************************************************************/

#define  _MT
#define  _DLL
#define  INCL_DOS
#define  INCL_ERRORS
#define  INCL_DOSDEVIOCTL

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>

#include "miscfunc.h"
#include "comsfunc.h"

/*********************************************************************/
/* CallDevice()                                                      */
/*********************************************************************/
APIRET CallDevice( HFILE hcomPort,
                   ULONG ulCategory,
                   ULONG ulFunction,
                   PVOID pParameters,
                   PVOID pDataPacket )
{
   APIRET       rc;                    /* Function return code       */
   PVOID        pParmPkt;              /* DevIOCtl parm packet ptr   */
   PVOID        pDataPkt;              /* DevIOCtl data packet ptr   */
   ULONG        ulParmLen;             /* DevIOCtl parm pack length  */
   ULONG        ulDataLen;             /* DevIOCtl data pack length  */
   PULONG       pulParmLen;            /* DevIOCtl parm pack len ptr */
   PULONG       pulDataLen;            /* DevIOCtl data pack len ptr */
   ULONG        ulSaveLen;             /* Save the data pack length  */
   BYTE         bGenParm;              /* General DevIOCtl parm pack */
   BYTE         bGenData;              /* General DevIOCtl data pack */
   rc = NO_ERROR;
   ulParmLen  = 0;
   ulDataLen  = 0;
   pulParmLen = &ulParmLen;
   pulDataLen = &ulDataLen;
   ulSaveLen  = 0;
   switch( ulCategory )
   {
      case IOCTL_ASYNC:
           pParmPkt = pParameters;
           pDataPkt = pDataPacket;
           switch( ulFunction )
           {
              case ASYNC_GETBAUDRATE:
                   ulDataLen = GETBAUDRATE_SIZE;
                   break;
              case ASYNC_SETBAUDRATE:
                   ulParmLen = SETBAUDRATE_SIZE;
                   break;
              case ASYNC_GETLINECTRL:
                   ulDataLen = GETLINECONTROL_SIZE;
                   break;
              case ASYNC_SETLINECTRL:
                   ulParmLen = SETLINECONTROL_SIZE;
                   break;
              case ASYNC_GETEXTDRATE:
                   ulDataLen = GETEXTDRATE_SIZE;
                   break;
              case ASYNC_SETEXTDRATE:
                   ulParmLen = SETEXTDRATE_SIZE;
                   break;
              case ASYNC_TRANSMITIMM:
                   ulParmLen = TRANSMITIMM_SIZE;
                   break;
              case ASYNC_SETBREAKON:
                   ulDataLen = COMERROR_SIZE;
                   break;
              case ASYNC_SETBREAKOFF:
                   ulDataLen = COMERROR_SIZE;
                   break;
              case ASYNC_SETMODEMCTRL:
                   ulParmLen = MODEMSTATUS_SIZE;
                   ulDataLen = COMERROR_SIZE;
                   break;
              case ASYNC_STARTTRANSMIT:
                   break;
              case ASYNC_STOPTRANSMIT:
                   break;
              case ASYNC_GETDCBINFO:
                   ulDataLen = GETDCBINFO_SIZE;
                   break;
              case ASYNC_SETDCBINFO:
                   ulParmLen = SETDCBINFO_SIZE;
                   break;
              case ASYNC_GETENHPARM:
                   ulDataLen = GETENHPARM_SIZE;
                   break;
              case ASYNC_SETENHPARM:
                   ulParmLen = SETENHPARM_SIZE;
                   break;
              case ASYNC_GETCOMMSTATUS:
                   ulDataLen = GETCOMMSTATUS_SIZE;
                   break;
              case ASYNC_GETLINESTATUS:
                   ulDataLen = GETLINESTATUS_SIZE;
                   break;
              case ASYNC_GETMODEMINPUT:
                   ulDataLen = GETMODEMIO_SIZE;
                   break;
              case ASYNC_GETMODEMOUTPUT:
                   ulDataLen = GETMODEMIO_SIZE;
                   break;
              case ASYNC_GETINQUECOUNT:
                   ulDataLen = GETIOQUEUE_SIZE;
                   break;
              case ASYNC_GETOUTQUECOUNT:
                   ulDataLen = GETIOQUEUE_SIZE;
                   break;
              case ASYNC_GETCOMMERROR:
                   ulDataLen = COMERROR_SIZE;
                   break;
              case ASYNC_GETCOMMEVENT:
                   ulDataLen = GETCOMMEVENT_SIZE;
                   break;
              default:
                   rc = ERROR_INVALID_PARAMETER;
                   break;
           }
           break;
      case IOCTL_GENERAL:
           pParmPkt = &bGenParm;
           pDataPkt = &bGenData;
           switch( ulFunction )
           {
              case DEV_FLUSHINPUT:
                   ulParmLen = DEVFLUSHPRM_SIZE;
                   ulDataLen = DEVFLUSHDAT_SIZE;
                   break;
              case DEV_FLUSHOUTPUT:
                   ulParmLen = DEVFLUSHPRM_SIZE;
                   ulDataLen = DEVFLUSHDAT_SIZE;
                   break;
              default:
                   rc = ERROR_INVALID_PARAMETER;
                   break;
           }
           break;
      default:
           rc = ERROR_INVALID_PARAMETER;
           break;
   }
   if ( hcomPort == NULLHANDLE ) {
      rc = ERROR_INVALID_PARAMETER;
   }
   if ( rc == NO_ERROR ) {
      ulSaveLen = ulDataLen;
      if (ulParmLen == 0) {
         pParmPkt = NULL;
         pulParmLen = NULL;
      }
      if (ulDataLen == 0) {
         pDataPkt = NULL;
         pulDataLen = NULL;
      }
      if (pDataPkt != NULL) {
         memset( pDataPkt, (BYTE)0, ulDataLen );       /* zeroise it */
      }
      rc = DosDevIOCtl( hcomPort,                          /* HFILE  */
                        ulCategory,                        /* ULONG  */
                        ulFunction,                        /* ULONG  */
                        pParmPkt,                          /* PVOID  */
                        ulParmLen,                         /* ULONG  */
                        pulParmLen,                        /* PULONG */
                        pDataPkt,                          /* PVOID  */
                        ulDataLen,                         /* ULONG  */
                        pulDataLen );                      /* PULONG */
      if ( rc == NO_ERROR ) {
         if ( ulDataLen != ulSaveLen ) {
            rc = ERROR_GEN_FAILURE;    /* Didn't return enough data  */
         }
      }
      if ( rc != NO_ERROR ) {
         fprintf( stdout,
                  "DosDevIOCtl(0x%4.4lX,0x%4.4lX) failed with RC=%d.\r\n",
                  ulCategory,
                  ulFunction,
                  rc );
      }
   }
   return( rc );                       /* Return DosDevIOCtl result  */
} /* end CallDevice() */

/*********************************************************************/
/* ActiveSignal()                                                    */
/*********************************************************************/
APIRET ActiveSignal( HFILE hcomPort, BYTE bSignal )
{
   APIRET       rc;                    /* 0 if active, -2 if not     */
   BYTE         bComSignals;           /* Modem Input/Output Signals */
   switch( bSignal )
   {
     case DTR_ON:
     case RTS_ON:
          rc = CallDevice( hcomPort,
                           IOCTL_ASYNC,
                           ASYNC_GETMODEMOUTPUT,
                           NULL,
                           &bComSignals );
          break;
     case CTS_ON:
     case DSR_ON:
     case RI_ON:
     case DCD_ON:
          rc = CallDevice( hcomPort,
                           IOCTL_ASYNC,
                           ASYNC_GETMODEMINPUT,
                           NULL,
                           &bComSignals );
          break;
     default:
          rc = ERROR_INVALID_PARAMETER;
          break;
   }
   if ( rc == NO_ERROR )
      if ( !(bComSignals & bSignal) )
         rc = RC_ASYNC_NO_SIGNAL;
   return( rc );                       /* Return DosDevIOCtl result  */
} /* end ActiveSignal() */

/*********************************************************************/
/* FlushInput()                                                      */
/*********************************************************************/
APIRET FlushInput( HFILE hcomPort )
{
   BYTE         bCommandInfo;          /* Device command information */
   BYTE         bCommandAnsw;          /* Device command response    */
   bCommandInfo = GENERAL_CMDINFO_NONE;
   return( CallDevice( hcomPort,
                       IOCTL_GENERAL,
                       DEV_FLUSHINPUT,
                       &bCommandInfo,
                       &bCommandAnsw ) );/* Return DosDevIOCtl result*/
} /* end FlushInput() */

/*********************************************************************/
/* FlushOutput()                                                     */
/*********************************************************************/
APIRET FlushOutput( HFILE hcomPort )
{
   BYTE         bCommandInfo;          /* Device command information */
   BYTE         bCommandAnsw;          /* Device command response    */
   bCommandInfo = GENERAL_CMDINFO_NONE;
   return( CallDevice( hcomPort,
                       IOCTL_GENERAL,
                       DEV_FLUSHOUTPUT,
                       &bCommandInfo,
                       &bCommandAnsw ) );/* Return DosDevIOCtl result*/
} /* end FlushOutput() */

/*********************************************************************/
/* GetDcbInfo()                                                      */
/*********************************************************************/
APIRET GetDcbInfo( HFILE hcomPort, PDCBINFO2 pdcbPortInfo )
{
   return( CallDevice( hcomPort,
                       IOCTL_ASYNC,
                       ASYNC_GETDCBINFO,
                       NULL,
                       pdcbPortInfo ) );/* Return DosDevIOCtl result */
} /* end GetDcbInfo() */

/*********************************************************************/
/* SetDcbInfo()                                                      */
/*********************************************************************/
APIRET SetDcbInfo( HFILE hcomPort, PDCBINFO2 pdcbPortInfo )
{
   return( CallDevice( hcomPort,
                       IOCTL_ASYNC,
                       ASYNC_SETDCBINFO,
                       pdcbPortInfo,
                       NULL ) );       /* Return DosDevIOCtl result  */
} /* end SetDcbInfo() */

/*********************************************************************/
/* GetEnhParm()                                                      */
/*********************************************************************/
APIRET GetEnhParm( HFILE hcomPort, PBYTE pbEnhancedFlags )
{
   APIRET   rc;                        /* Function return code       */
   ENHPARM  eparmParms;                /* Enhanced Mode Parameters   */
   if ( pbEnhancedFlags == NULL ) {
      rc = ERROR_INVALID_PARAMETER;
   } else {
      rc = CallDevice( hcomPort,
                       IOCTL_ASYNC,
                       ASYNC_GETENHPARM,
                       NULL,
                       &eparmParms );
      if ( rc == NO_ERROR ) {
         *pbEnhancedFlags = eparmParms.bEnhancedFlags;
      }
   }
   return( rc );                       /* Return DosDevIOCtl result  */
} /* end GetEnhParm() */

/*********************************************************************/
/* SetEnhParm()                                                      */
/*********************************************************************/
APIRET SetEnhParm( HFILE hcomPort, BYTE bEnhancedFlags )
{
   APIRET   rc;                        /* Function return code       */
   ENHPARM  eparmParms;                /* Enhanced Mode Parameters   */
   rc = CallDevice( hcomPort,
                    IOCTL_ASYNC,
                    ASYNC_GETENHPARM,
                    NULL,
                    &eparmParms );
   if ( rc == NO_ERROR ) {
      if ( eparmParms.bEnhancedFlags & MODE_ENH_SUPPORTED ) {
         eparmParms.bEnhancedFlags = bEnhancedFlags;
         rc = CallDevice( hcomPort,
                          IOCTL_ASYNC,
                          ASYNC_SETENHPARM,
                          &eparmParms,
                          NULL );
      } else {
         /* don't attempt setting, as enhanced mode is not supported */
      }
   }
   return( rc );                       /* Return DosDevIOCtl result  */
} /* end SetEnhParm() */

/*********************************************************************/
/* GetLnCtrl()                                                       */
/*********************************************************************/
APIRET GetLnCtrl( HFILE hcomPort, PEXTDRATE perateBaudRate, PLINECONTROL plnctlLineControl )
{
   APIRET   rc;                        /* Function return code       */
   rc = CallDevice( hcomPort,
                    IOCTL_ASYNC,
                    ASYNC_GETEXTDRATE,
                    NULL,
                    perateBaudRate );
   if ( rc == NO_ERROR ) {
      rc = CallDevice( hcomPort,
                       IOCTL_ASYNC,
                       ASYNC_GETLINECTRL,
                       NULL,
                       plnctlLineControl );
   }
   return( rc );                       /* Return DosDevIOCtl result  */
} /* end GetLnCtrl() */

/*********************************************************************/
/* SetLnCtrl()                                                       */
/*********************************************************************/
APIRET SetLnCtrl( HFILE hcomPort, PEXTDRATE perateBaudRate, PLINECONTROL plnctlLineControl )
{
   APIRET   rc;                        /* Function return code       */
   rc = CallDevice( hcomPort,
                    IOCTL_ASYNC,
                    ASYNC_SETEXTDRATE,
                    perateBaudRate,
                    NULL );
   if ( rc == NO_ERROR ) {
      rc = CallDevice( hcomPort,
                       IOCTL_ASYNC,
                       ASYNC_SETLINECTRL,
                       plnctlLineControl,
                       NULL );
   }
   return( rc );                       /* Return DosDevIOCtl result  */
} /* end SetLnCtrl() */

/*********************************************************************/
/* GetComStatus()                                                    */
/*********************************************************************/
APIRET GetComStatus( HFILE hcomPort, PBYTE pbComStatus )
{
   return( CallDevice( hcomPort,
                       IOCTL_ASYNC,
                       ASYNC_GETCOMMSTATUS,
                       NULL,
                       pbComStatus ) );/* Return DosDevIOCtl result  */
} /* end GetComStatus() */

/*********************************************************************/
/* GetTxStatus()                                                     */
/*********************************************************************/
APIRET GetTxStatus( HFILE hcomPort, PBYTE pbTxStatus )
{
   return( CallDevice( hcomPort,
                       IOCTL_ASYNC,
                       ASYNC_GETLINESTATUS,
                       NULL,
                       pbTxStatus ) ); /* Return DosDevIOCtl result  */
} /* end GetTxStatus() */

/*********************************************************************/
/* GetModemOutput()                                                  */
/*********************************************************************/
APIRET GetModemOutput( HFILE hcomPort, PBYTE pbMdmOutput )
{
   return( CallDevice( hcomPort,
                       IOCTL_ASYNC,
                       ASYNC_GETMODEMOUTPUT,
                       NULL,
                       pbMdmOutput ) );/* Return DosDevIOCtl result  */
} /* end GetModemOutput() */

/*********************************************************************/
/* GetModemInput()                                                   */
/*********************************************************************/
APIRET GetModemInput( HFILE hcomPort, PBYTE pbMdmInput )
{
   return( CallDevice( hcomPort,
                       IOCTL_ASYNC,
                       ASYNC_GETMODEMINPUT,
                       NULL,
                       pbMdmInput ) ); /* Return DosDevIOCtl result  */
} /* end GetModemInput() */

/*********************************************************************/
/* SetBreakOff()                                                     */
/*********************************************************************/
APIRET SetBreakOff( HFILE hcomPort )
{
   COMERR   ComError;                  /* Communication Error Status */
   return( CallDevice( hcomPort,
                       IOCTL_ASYNC,
                       ASYNC_SETBREAKOFF,
                       NULL,
                       &ComError ) );  /* Return DosDevIOCtl result  */
} /* end SetBreakOff() */

/*********************************************************************/
/* SetBreakOn()                                                      */
/*********************************************************************/
APIRET SetBreakOn( HFILE hcomPort )
{
   COMERR   ComError;                  /* Communication Error Status */
   return( CallDevice( hcomPort,
                       IOCTL_ASYNC,
                       ASYNC_SETBREAKON,
                       NULL,
                       &ComError ) );  /* Return DosDevIOCtl result  */
} /* end SetBreakOn() */

/*********************************************************************/
/* TxImmediate()                                                     */
/*********************************************************************/
APIRET TxImmediate( HFILE hcomPort, BYTE bAnyByte )
{
   return( CallDevice( hcomPort,
                       IOCTL_ASYNC,
                       ASYNC_TRANSMITIMM,
                       &bAnyByte,
                       NULL ) );       /* Return DosDevIOCtl result  */
} /* end TxImmediate() */

/*********************************************************************/
/* TxStop()                                                          */
/*********************************************************************/
APIRET TxStop( HFILE hcomPort )
{
   return( CallDevice( hcomPort,
                       IOCTL_ASYNC,
                       ASYNC_STOPTRANSMIT,
                       NULL,
                       NULL ) );       /* Return DosDevIOCtl result  */
} /* end TxStop() */

/*********************************************************************/
/* TxStart()                                                         */
/*********************************************************************/
APIRET TxStart( HFILE hcomPort )
{
   return( CallDevice( hcomPort,
                       IOCTL_ASYNC,
                       ASYNC_STARTTRANSMIT,
                       NULL,
                       NULL ) );       /* Return DosDevIOCtl result  */
} /* end TxStart() */

/*********************************************************************/
/* SetMdmOutput()                                                    */
/*********************************************************************/
APIRET SetMdmOutput( HFILE hcomPort, BYTE bModemOn, BYTE bModemOff )
{
   MODEMSTATUS mdmstControl;           /* Modem control              */
   COMERR      ComError;               /* Communication Error Status */
   mdmstControl.fbModemOn  = bModemOn;
   mdmstControl.fbModemOff = bModemOff;
   return( CallDevice( hcomPort,
                       IOCTL_ASYNC,
                       ASYNC_SETMODEMCTRL,
                       &mdmstControl,
                       &ComError ) );  /* Return DosDevIOCtl result  */
} /* end SetMdmOutput() */

/*********************************************************************/
/* GetRxCount()                                                      */
/*********************************************************************/
APIRET GetRxCount( HFILE hcomPort, PUSHORT pusRxCount, PUSHORT pusRxSize )
{
   APIRET   rc;                        /* Function return code       */
   IOQUEUE  ioqueStatus;               /* IO Queue details returned  */
   if ( (pusRxCount == NULL) || (pusRxSize  == NULL) ) {
      rc = ERROR_INVALID_PARAMETER;
   } else {
      rc = CallDevice( hcomPort,
                       IOCTL_ASYNC,
                       ASYNC_GETINQUECOUNT,
                       NULL,
                       &ioqueStatus );
      if ( rc == NO_ERROR ) {
         *pusRxCount = ioqueStatus.usQueueCount;
         *pusRxSize  = ioqueStatus.usQueueSize;
      }
   }
   return( rc );                       /* Return DosDevIOCtl result  */
} /* end GetRxCount() */

/*********************************************************************/
/* GetTxCount()                                                      */
/*********************************************************************/
APIRET GetTxCount( HFILE hcomPort, PUSHORT pusTxCount, PUSHORT pusTxSize )
{
   APIRET   rc;                        /* Function return code       */
   IOQUEUE  ioqueStatus;               /* IO Queue details returned  */
   if ( (pusTxCount == NULL) || (pusTxSize  == NULL) ) {
      rc = ERROR_INVALID_PARAMETER;
   } else {
      rc = CallDevice( hcomPort,
                       IOCTL_ASYNC,
                       ASYNC_GETOUTQUECOUNT,
                       NULL,
                       &ioqueStatus );
      if ( rc == NO_ERROR ) {
         *pusTxCount = ioqueStatus.usQueueCount;
         *pusTxSize  = ioqueStatus.usQueueSize;
      }
   }
   return( rc );                       /* Return DosDevIOCtl result  */
} /* end GetTxCount() */

/*********************************************************************/
/* GetComError()                                                     */
/*********************************************************************/
APIRET GetComError( HFILE hcomPort, PUSHORT pusComError )
{
   return( CallDevice( hcomPort,
                       IOCTL_ASYNC,
                       ASYNC_GETCOMMERROR,
                       NULL,
                       pusComError ) );/* Return DosDevIOCtl result  */
} /* end GetComError() */

/*********************************************************************/
/* GetComEvent()                                                     */
/*********************************************************************/
APIRET GetComEvent( HFILE hcomPort, PUSHORT pusComEvent )
{
   return( CallDevice( hcomPort,
                       IOCTL_ASYNC,
                       ASYNC_GETCOMMEVENT,
                       NULL,
                       pusComEvent ) );/* Return DosDevIOCtl result  */
} /* end GetComEvent() */

/*********************************************************************/
/* PrtDcbInfo()                                                      */
/*********************************************************************/
APIRET PrtDcbInfo( HFILE hcomPort )
{
   APIRET   rc;                        /* Function return code       */
   DCBINFO2 dcbPortInfo;               /* Device control block       */
   rc = CallDevice( hcomPort, IOCTL_ASYNC, ASYNC_GETDCBINFO, NULL, &dcbPortInfo );
   fprintf( stdout, "Current DCBINFO settings:\r\n" );
   fprintf( stdout, "\r\n" );
   fprintf( stdout, "Write Timeout  =>%u<\r\n",      dcbPortInfo.usWriteTimeout );
   fprintf( stdout, "Read Timeout   =>%u<\r\n",      dcbPortInfo.usReadTimeout );
   fprintf( stdout, "Hand Shake     =>0x%2.2X<\r\n", dcbPortInfo.fbCtlHndShake );
   fprintf( stdout, "Flow Replace   =>0x%2.2X<\r\n", dcbPortInfo.fbFlowReplace );
   fprintf( stdout, "Timeout        =>0x%2.2X<\r\n", dcbPortInfo.fbTimeout );
   fprintf( stdout, "Error Replace  =>0x%2.2X<\r\n", dcbPortInfo.bErrorReplacementChar );
   fprintf( stdout, "Break Replace  =>0x%2.2X<\r\n", dcbPortInfo.bBreakReplacementChar );
   fprintf( stdout, "XON Character  =>0x%2.2X<\r\n", dcbPortInfo.bXONChar );
   fprintf( stdout, "XOFF Character =>0x%2.2X<\r\n", dcbPortInfo.bXOFFChar );
   fprintf( stdout, "\r\n" );
   return( rc );
} /* end PrtDcbInfo() */

/*********************************************************************/
/* WaitCarrier()                                                     */
/*********************************************************************/
APIRET WaitCarrier(
          HFILE    hcomPort,           /* Communications port handle */
          LONG     lTimeLimit          /* Wait timeout limit         */
       )
{
   APIRET       rc;                    /* 0 DCD, -1 timeout, -2 noDCD*/
   BYTE         bComInput;             /* Modem Input Signals        */
   BOOL         fTimedOut;             /* Function has timed out     */
   LONG         lElapsed;              /* Time elapsed in millisecs  */
   DATETIME     tTimeStart;            /* Timer start time           */
   DATETIME     tTimeNow;              /* Time now                   */
   fTimedOut = FALSE;
   DosGetDateTime( &tTimeStart );
   do {
      rc = GetModemInput( hcomPort, &bComInput ); /* check mdm input */
      if ( rc != NO_ERROR ) {               /* get modeminput failed */
         fTimedOut = TRUE;                  /* implicit timeout      */
      } else if ( bComInput & DCD_ON ) {    /* carrier was detected  */
         fTimedOut = TRUE;                  /* implicit timeout      */
      } else if ( lTimeLimit == 0 ) {       /* no waiting            */
         rc = RC_ASYNC_NO_SIGNAL;           /* force no_signal return*/
         fTimedOut = TRUE;                  /* implicit timeout      */
      } else if ( lTimeLimit < 0 ) {        /* wait forever          */
         DosSleep( MAX_SLEEP );             /* Wait 1/100th second   */
      } else {                              /* check for timeout     */
         DosGetDateTime( &tTimeNow );
         lElapsed = MyDiffTime( &tTimeNow, &tTimeStart );
         if ( lElapsed > lTimeLimit ) {     /* we really did         */
            rc = RC_ASYNC_TIMED_OUT;        /* force timed_out return*/
            fTimedOut = TRUE;               /* explicit timeout      */
         } else {                           /* keep waiting          */
            DosSleep( MAX_SLEEP );          /* Wait 1/100th second   */
         }
      }
   } while( !fTimedOut );
   return( rc );                       /* Return DosDevIOCtl result  */
} /* end WaitCarrier() */

/*********************************************************************/
/* WaitInput()                                                       */
/*********************************************************************/
APIRET WaitInput(
          HFILE    hcomPort,           /* Communications port handle */
          LONG     lTimeLimit          /* Wait timeout limit         */
       )
{
   APIRET       rc;                    /* 0 data, -1 timeout, -3 none*/
   USHORT       usComEvent;            /* COM Event Information      */
   BOOL         fTimedOut;             /* Function has timed out     */
   LONG         lElapsed;              /* Time elapsed in millisecs  */
   DATETIME     tTimeStart;            /* Timer start time           */
   DATETIME     tTimeNow;              /* Time now                   */
   fTimedOut = FALSE;
   DosGetDateTime( &tTimeStart );
   do {
      rc = GetComEvent( hcomPort, &usComEvent ); /* check com event  */
      if ( rc != NO_ERROR ) {               /* get commevent failed  */
         fTimedOut = TRUE;                  /* implicit timeout      */
      } else if ( usComEvent & CHAR_RECEIVED ) { /* got some data ok */
         fTimedOut = TRUE;                  /* implicit timeout      */
      } else if ( lTimeLimit == 0 ) {       /* no waiting            */
         rc = RC_ASYNC_NO_INPUT;            /* force no_input return */
         fTimedOut = TRUE;                  /* implicit timeout      */
      } else if ( lTimeLimit < 0 ) {        /* wait forever          */
         DosSleep( MAX_SLEEP );             /* Wait 1/100th second   */
      } else {                              /* check for timeout     */
         DosGetDateTime( &tTimeNow );
         lElapsed = MyDiffTime( &tTimeNow, &tTimeStart );
         if ( lElapsed > lTimeLimit ) {     /* we really did         */
            rc = RC_ASYNC_TIMED_OUT;        /* force timed_out return*/
            fTimedOut = TRUE;               /* explicit timeout      */
         } else {                           /* keep waiting          */
            DosSleep( MAX_SLEEP );          /* Wait 1/100th second   */
         }
      }
   } while( !fTimedOut );
   return( rc );                       /* Return DosDevIOCtl result  */
} /* end WaitInput() */

/*********************************************************************/
/* SendBreak()                                                       */
/*********************************************************************/
APIRET SendBreak(
          HFILE    hcomPort,           /* Communications port handle */
          ULONG    ulDuration          /* Break duration             */
       )
{
   APIRET   rc1;                       /* Function return code       */
   APIRET   rc2;                       /* Function return code       */
   rc1 = SetBreakOn( hcomPort );
   if ( rc1 == NO_ERROR ) {
      if ( ulDuration > 0 ) {          /* Only if its worthwile      */
         DosSleep( ulDuration );
      }
   }
   rc2 = SetBreakOff( hcomPort );
   if ( rc2 != NO_ERROR ) {
      if ( rc1 == NO_ERROR ) {         /* Not already in error       */
         rc1 = rc2;
      }
   }
   return( rc1 );                      /* Return DosDevIOCtl result  */
} /* end SendBreak() */

/*********************************************************************/
/* OpenPort()                                                        */
/*********************************************************************/
APIRET OpenPort(
          PSZ      pszName,            /* Communications port name   */
          PHFILE   phcomPort           /* Communications port handle */
       )
{
   APIRET   rc;                        /* Function return code       */
   ULONG    ulOpenAction;              /* Open action                */
   *phcomPort = NULLHANDLE;
   rc = DosOpen( pszName,
                 phcomPort,
                 &ulOpenAction,
                 0,
                 FILE_NORMAL,
                 FILE_OPEN,
                 OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYNONE,
                 (PEAOP2) NULL );
   if ( rc != NO_ERROR ) {             /* Couldn't open port         */
      fprintf( stdout, "DosOpen(%s) failed with RC=%d.\r\n", pszName, rc );
      *phcomPort = NULLHANDLE;
   }
   return( rc );                       /* Return DosOpen result      */
} /* end OpenPort() */

/*********************************************************************/
/* ClosePort()                                                       */
/*********************************************************************/
APIRET ClosePort(
          PHFILE   phcomPort           /* Communications port handle */
       )
{
   APIRET   rc;                        /* Function return code       */
   rc = DosClose( *phcomPort );
   if ( rc != NO_ERROR ) {
      fprintf( stdout, "DosClose(%d) failed with RC=%d.\r\n", *phcomPort, rc );
   }
   *phcomPort = NULLHANDLE;
   return( rc );                       /* Return DosClose result     */
} /* end ClosePort() */

/*********************************************************************/
/* WritePort()                                                       */
/*********************************************************************/
APIRET WritePort(
          HFILE    hcomPort,           /* Communications port handle */
          PSZ      pszBuffer,          /* Buffer of data to write    */
          ULONG    ulBuffLen,          /* Length of data in buffer   */
          PULONG   pulRemains          /* Amount unwritten           */
       )
{
   APIRET   rc;                        /* Function return code       */
   ULONG    ulWritten;                 /* Number of chars written    */
   ulWritten = 0;
   if ( ulBuffLen > 0 ) {              /* Not empty buffer           */
      rc = DosWrite( hcomPort, pszBuffer, ulBuffLen, &ulWritten );
      if ( rc != NO_ERROR ) {
         fprintf( stdout, "DosWrite(%s) failed with RC=%d.\r\n", pszBuffer, rc );
      }
   }
   *pulRemains = ulBuffLen - ulWritten;
   return( rc );                       /* Return DosWrite result     */
} /* end WritePort() */

/*********************************************************************/
/* ReadPort()                                                        */
/*                                                                   */
/* Note:                                                             */
/*                                                                   */
/*   This routine requires DBCS handling so that DBCS trail bytes    */
/*   are not inadvertantly stripped if the ReadSize should cause     */
/*   a break within the DBCS pair and the trail byte of the DBCS     */
/*   character just happens to be 0x00.                              */
/*                                                                   */
/*   pulBuffLen indicates the total amount requested to be read in   */
/*   ulReadSize chunks, the total amount actually read is returned   */
/*   in pulBuffLen.                                                  */
/*                                                                   */
/*   pszNullReplace and pszLineEnd must at this time be single       */
/*   character strings.                                              */
/*                                                                   */
/*********************************************************************/
APIRET ReadPort( HFILE  hcomPort,      /* Communications port handle */
                 PBYTE  pbReadBuff,    /* Buffer to read data with   */
                 ULONG  ulReadSize,    /* Block size to use          */
                 PBYTE  pbBuffer,      /* Buffer to return data in   */
                 PULONG pulBuffLen,    /* Length of data to read     */
                 LONG   lTimeLimit,    /* Time limit to use, if any  */
                 PSZ    pszNullReplace,/* Null replace char, if any  */
                 PSZ    pszLineEnd )   /* Line end char, if any      */
{
   APIRET       rc;                    /* Function return code       */
   USHORT       usComEvent;            /* COM Event Information      */
   BOOL         fWaitForMore;          /* Poll for more input        */
   BOOL         fTimedOut;             /* Function has timed out     */
   BOOL         fStopReading;          /* Got all we wanted flag     */
   BOOL         fByteFound;            /* Found some data to read    */
   ULONG        ulNumToRead;           /* Number to read this time   */
   ULONG        ulActuallyRead;        /* Number of chars we did get */
   ULONG        ulTotalRead;           /* Total number of chars read */
   LONG         lElapsed;              /* Time elapsed in millisecs  */
   DATETIME     tTimeStart;            /* Timer start time           */
   DATETIME     tTimeNow;              /* Time now                   */
   BOOL         fWantNulls;            /* Whether to keep nulls      */
   BYTE         bNullReplace;          /* Byte to use inplace of null*/
   BYTE         bLineEnd;              /* Byte to use in line end    */
   DosGetDateTime( &tTimeStart );
   ulTotalRead  = 0;
   fTimedOut    = FALSE;
   fStopReading = FALSE;
   fWantNulls   = TRUE;
   bNullReplace = EOSTR_CH;
   if ( pszNullReplace != NULL ) {
      fWantNulls   = FALSE;
      bNullReplace = pszNullReplace[0];
   }
   bLineEnd = LINEFEED_CH;
   if ( pszLineEnd != NULL ) {
      if ( pszLineEnd[0] != EOSTR_CH ) {
         bLineEnd = pszLineEnd[0];
      }
   }
   if ( ulReadSize > 0 ) {             /* block mode */
      ulNumToRead = ulReadSize;
   } else  {                           /* line mode  */
      ulNumToRead = 1;
   }
   if ( ulReadSize > *pulBuffLen ) {
      rc = ERROR_INVALID_PARAMETER;
   } else {
      rc = GetComEvent( hcomPort, &usComEvent ); /* clear comm event word */
      if ( rc == NO_ERROR ) {
         do {
            fWaitForMore = FALSE;
            do {
               ulActuallyRead = 0;
               memset( pbReadBuff, (BYTE)0, ulNumToRead ); /* zeroise read buffer */
               rc = DosRead( hcomPort, pbReadBuff, (ULONG) ulNumToRead, &ulActuallyRead );
               if ( (rc == NO_ERROR) || (rc == ERROR_MORE_DATA) ) {
                  if ( ulActuallyRead != ulNumToRead ) {
                     fWaitForMore = TRUE;
                  }
                  if ( ulActuallyRead > 0 ) {
                     if ( !fWantNulls ) {
                        if ( bNullReplace != EOSTR_CH ) {
                           ReplaceNulls( pbReadBuff, ulActuallyRead, bNullReplace );
                        } else {
                           StripNulls( pbReadBuff, &ulActuallyRead );
                        }
                     }
                     if ( ulActuallyRead > 0 ) {
                        memcpy( &(pbBuffer[ulTotalRead]), pbReadBuff, ulActuallyRead );
                        ulTotalRead = ulTotalRead + ulActuallyRead;
                        if ( ulReadSize > 0 ) {           /* block mode */
                           /* number to read is the amount left unread  */
                           /* or the block size if that is the lesser.  */
                           ulNumToRead = *pulBuffLen - ulTotalRead;
                           if ( ulReadSize < ulNumToRead ) {
                              ulNumToRead = ulReadSize;
                           }
                           if ( ulNumToRead <= 0 ) {
                              fStopReading = TRUE;
                           }
                        } else {                          /* line mode  */
                           pbBuffer[ulTotalRead] = EOSTR_CH; /* terminate string */
                           /* handle line termination detection */
                           if (   (pbReadBuff[0] == LINEFEED_CH)    /* LINEFEED */
                               || (pbReadBuff[0] == FORMFEED_CH)    /* FORMFEED */
                               || (pbReadBuff[0] == EOFILE_CH)      /* EOF      */
                               || (pbReadBuff[0] == bLineEnd)   ) { /* user spec*/
                              fStopReading = TRUE;
                           } else if ( ulTotalRead >= *pulBuffLen - 1 ) {
                              /* stop one byte short to maintain string terminator */
                              fStopReading = TRUE;
                           }
                        }
                     } else if ( fWaitForMore ) {
                        /* didn't receive enough data, wait for more */
                     } else if ( lTimeLimit < 0) {
                        /* read again, don't care about timeout */
                     } else {
                        DosGetDateTime( &tTimeNow );
                        lElapsed = MyDiffTime( &tTimeNow, &tTimeStart );
                        if ( lElapsed > lTimeLimit ) {       /* explicit timeout */
                           rc = RC_ASYNC_TIMED_OUT;          /* so return timeout*/
                           fTimedOut = TRUE;
                        }
                     }
                  } /* some if not all data WAS read */
               } else {
                  fprintf( stdout, "DosRead() failed with RC=%d.\r\n", rc );
                  fTimedOut = TRUE;                          /* implicit timeout */
               }
            } while( !fStopReading && !fTimedOut && !fWaitForMore );
            if ( fWaitForMore ) {
               fByteFound = FALSE;
               do {
                  rc = GetComEvent( hcomPort, &usComEvent ); /* check com event  */
                  if ( rc != NO_ERROR ) {                    /* call failed      */
                     fTimedOut = TRUE;                       /* implicit timeout */
                  } else if ( usComEvent & CHAR_RECEIVED ) { /* got some data    */
                     fByteFound = TRUE;                      /* exit wait loop   */
                  } else if ( lTimeLimit < 0 ) {             /* waiting forever  */
                     DosSleep( MAX_SLEEP );                  /* wait 1/100th sec */
                  } else {                                   /* check timeout    */
                     DosGetDateTime( &tTimeNow );
                     lElapsed = MyDiffTime( &tTimeNow, &tTimeStart );
                     if ( lElapsed > lTimeLimit ) {          /* limit exceeded   */
                        rc = RC_ASYNC_TIMED_OUT;             /* so return timeout*/
                        fTimedOut = TRUE;                    /* explicit timeout */
                     } else {
                        DosSleep( MAX_SLEEP );               /* wait 1/100th sec */
                     }
                  }
               } while( !fByteFound && !fTimedOut );
            } /* endif wait for more data to arrive */
         } while( !fStopReading && !fTimedOut );
      } /* endif get comm event okay */
   } /* endif read size okay */
   *pulBuffLen = ulTotalRead;                                /* return true length */
   return( rc );
} /* end ReadPort() */

/*********************************************************************/
/* END MODULE                                                        */
/*********************************************************************/
