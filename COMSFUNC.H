/*****************************************************************************/
/*                                                                           */
/*  MODULE         COMSFUNC.H                                                */
/*                                                                           */
/*  VERSION        Version 2.0 - 26th Dec 1994                               */
/*                                                                           */
/*  COPYRIGHT      Copyright (c) 1993, 1994 by Crucial Applications          */
/*                             All rights reserved.                          */
/*                                                                           */
/*  DESCRIPTION    Header file for COMSFUNC.C Serial Communications Functions*/
/*                                                                           */
/*****************************************************************************/

#ifndef COMSFUNC_H_INCLUDED
#define COMSFUNC_H_INCLUDED

/*********************************************************************/
/* General definitions                                               */
/*********************************************************************/

#define  MAX_SLEEP            10       /* maximum time wait/read poll*/

#define  RC_ASYNC_TIMED_OUT   -1       /* Function timed out         */
#define  RC_ASYNC_NO_SIGNAL   -2       /* Function lost signal ie.DCD*/
#define  RC_ASYNC_NO_INPUT    -3       /* Function didn't detect data*/

/*********************************************************************/
/* Required ASYNC definitions.                                       */
/*                                                                   */
/* NOTE: Items which are commented out are those that ARE defined in */
/*       the BSEDEV.h include file supplied with OS/2 2.x Toolkit.   */
/*********************************************************************/

/*********************************************************************/
/* General Device Control                                            */
/*********************************************************************/

//#define IOCTL_GENERAL                0x000B

//#define DEV_FLUSHINPUT               0x0001
//#define DEV_FLUSHOUTPUT              0x0002

/*********************************************************************/
/* DEV_FLUSHINPUT, DEV_FLUSHOUTPUT                                   */
/*********************************************************************/

// Uses a single BYTE no struct required.

#define DEVFLUSHPRM_SIZE               1                               /* not defined in BSEDEV.h */
#define DEVFLUSHDAT_SIZE               1                               /* not defined in BSEDEV.h */

/* bCommandInfo ------------------------------------------ 7654 3210 */
#define   GENERAL_CMDINFO_NONE         0x00             /* .... .... *//* not defined in BSEDEV.h */

/*********************************************************************/
/* Async Device Control                                              */
/*********************************************************************/

//#define IOCTL_ASYNC                  0x0001

//#define ASYNC_SETBAUDRATE            0x0041
//#define ASYNC_SETLINECTRL            0x0042
#define   ASYNC_SETEXTDRATE            0x0043                          /* not defined in BSEDEV.h */
//#define ASYNC_TRANSMITIMM            0x0044
//#define ASYNC_SETBREAKOFF            0x0045
//#define ASYNC_SETMODEMCTRL           0x0046
//#define ASYNC_STOPTRANSMIT           0x0047
//#define ASYNC_STARTTRANSMIT          0x0048
//#define ASYNC_SETBREAKON             0x004B
//#define ASYNC_SETDCBINFO             0x0053
#define   ASYNC_SETENHPARM             0x0054                          /* not defined in BSEDEV.h */
//#define ASYNC_GETBAUDRATE            0x0061
//#define ASYNC_GETLINECTRL            0x0062
#define   ASYNC_GETEXTDRATE            0x0063                          /* not defined in BSEDEV.h */
//#define ASYNC_GETCOMMSTATUS          0x0064
//#define ASYNC_GETLINESTATUS          0x0065
//#define ASYNC_GETMODEMOUTPUT         0x0066
//#define ASYNC_GETMODEMINPUT          0x0067
//#define ASYNC_GETINQUECOUNT          0x0068
//#define ASYNC_GETOUTQUECOUNT         0x0069
//#define ASYNC_GETCOMMERROR           0x006D
//#define ASYNC_GETCOMMEVENT           0x0072
//#define ASYNC_GETDCBINFO             0x0073
#define   ASYNC_GETENHPARM             0x0074                          /* not defined in BSEDEV.h */

/*********************************************************************/
/* ASYNC_GETCOMMERROR, ASYNC_SETBREAKOFF, ASYNC_SETBREAKON,          */
/* ASYNC_SETMODEMCTRL                                                */
/*********************************************************************/

// Returns COMERR a single WORD (16bit==USHORT) no struct required.

typedef USHORT    COMERR;

#define COMERROR_SIZE                  2                               /* not defined in BSEDEV.h */

/* --------------------------------------------- FEDC BA98 7654 3210 */
//#define RX_QUE_OVERRUN               0x0001 /* .... .... .... ...1 */
//#define RX_HARDWARE_OVERRUN          0x0002 /* .... .... .... ..1. */
//#define PARITY_ERROR                 0x0004 /* .... .... .... .1.. */
//#define FRAMING_ERROR                0x0008 /* .... .... .... 1... */

/*********************************************************************/
/* ASYNC_GETBAUDRATE, ASYNC_SETBAUDRATE                              */
/*********************************************************************/

// Uses a single WORD (16bit==USHORT) no struct required.

#define GETBAUDRATE_SIZE               2                               /* not defined in BSEDEV.h */
#define SETBAUDRATE_SIZE               2                               /* not defined in BSEDEV.h */

/*********************************************************************/
/* ASYNC_GETLINECTRL, ASYNC_SETLINECTRL                              */
/*********************************************************************/

//#pragma pack(1)                      /* pack on 1 byte boundary    */

//typedef struct _LINECONTROL          /* lnctl */
//        {
//        BYTE bDataBits;              /* BYTE  */
//        BYTE bParity;                /* BYTE  */
//        BYTE bStopBits;              /* BYTE  */
//        BYTE fTransBreak;            /* BYTE  */
//        } LINECONTROL;
//typedef LINECONTROL *PLINECONTROL;

#define GETLINECONTROL_SIZE            4                               /* not defined in BSEDEV.h */
#define SETLINECONTROL_SIZE            3                               /* not defined in BSEDEV.h */

#define DATA_BITS_5                    0x05                            /* not defined in BSEDEV.h */
#define DATA_BITS_6                    0x06                            /* not defined in BSEDEV.h */
#define DATA_BITS_7                    0x07                            /* not defined in BSEDEV.h */
#define DATA_BITS_8                    0x08                            /* not defined in BSEDEV.h */

#define PARITY_NONE                    0x00                            /* not defined in BSEDEV.h */
#define PARITY_ODD                     0x01                            /* not defined in BSEDEV.h */
#define PARITY_EVEN                    0x02                            /* not defined in BSEDEV.h */
#define PARITY_MARK                    0x03                            /* not defined in BSEDEV.h */
#define PARITY_SPACE                   0x04                            /* not defined in BSEDEV.h */

#define STOP_BITS_1                    0x00                            /* not defined in BSEDEV.h */
#define STOP_BITS_15                   0x01                            /* not defined in BSEDEV.h */
#define STOP_BITS_2                    0x02                            /* not defined in BSEDEV.h */

#define DATA_BITS_5_STR                "5"
#define DATA_BITS_6_STR                "6"
#define DATA_BITS_7_STR                "7"
#define DATA_BITS_8_STR                "8"
#define DATA_BITS_UNKNOWN_STR          "0"

#define PARITY_NONE_STR                "N"
#define PARITY_ODD_STR                 "O"
#define PARITY_EVEN_STR                "E"
#define PARITY_MARK_STR                "M"
#define PARITY_SPACE_STR               "S"
#define PARITY_UNKNOWN_STR             "?"

#define STOP_BITS_1_STR                "1"
#define STOP_BITS_15_STR               "1.5"
#define STOP_BITS_2_STR                "2"
#define STOP_BITS_UNKNOWN_STR          "0"

/*********************************************************************/
/* ASYNC_GETEXTDRATE, ASYNC_SETEXTDRATE                              */
/*********************************************************************/

#pragma pack(1)                        /* pack on 1 byte boundary    */

typedef struct _EXTDRATE               /* erate */
        {
        ULONG  ulCurrentBaudRate;      /* DWORD 4-bytes */
        BYTE   bCurrentFraction;       /* BYTE  */
        ULONG  ulMinBaudRate;          /* DWORD */
        BYTE   bMinFraction;           /* BYTE  */
        ULONG  ulMaxBaudRate;          /* DWORD */
        BYTE   bMaxFraction;           /* BYTE  */
        } EXTDRATE;
typedef EXTDRATE *PEXTDRATE;

#define GETEXTDRATE_SIZE               15                              /* not defined in BSEDEV.h */
#define SETEXTDRATE_SIZE               5                               /* not defined in BSEDEV.h */

/*********************************************************************/
/* ASYNC_TRANSMITIMM                                                 */
/*********************************************************************/

// Sends a single BYTE no struct required.

#define TRANSMITIMM_SIZE               1                               /* not defined in BSEDEV.h */

/*********************************************************************/
/* ASYNC_SETMODEMCTRL                                                */
/*********************************************************************/

//#pragma pack(1)                      /* pack on 1 byte boundary    */

//typedef struct _MODEMSTATUS          /* mdmst */
//        {
//        BYTE fbModemOn;              /* BYTE  */
//        BYTE fbModemOff;             /* BYTE  */
//        } MODEMSTATUS;
//typedef MODEMSTATUS *PMODEMSTATUS;

#define MODEMSTATUS_SIZE               2                               /* not defined in BSEDEV.h */

/* MODEMSTATUS.fbModemOn --------------------------------- 7654 3210 */
#define NONE_ON                        0x00             /* .... .... *//* not defined in BSEDEV.h */
//#define DTR_ON                       0x01             /* .... ...1 */
//#define RTS_ON                       0x02             /* .... ..1. */
#define DTR_RTS_ON                     0x03             /* .... ..11 *//* not defined in BSEDEV.h */

/* MODEMSTATUS.fbModemOff -------------------------------- 7654 3210 */
#define NONE_OFF                       0xFF             /* 1111 1111 *//* not defined in BSEDEV.h */
//#define DTR_OFF                      0xFE             /* 1111 111. */
//#define RTS_OFF                      0xFD             /* 1111 11.1 */
#define DTR_RTS_OFF                    0xFC             /* 1111 11.. *//* not defined in BSEDEV.h */

/*********************************************************************/
/* ASYNC_GETDCBINFO, ASYNC_SETDCBINFO                                */
/*********************************************************************/

#pragma pack(1)                        /* pack on 1 byte boundary    */

typedef struct _DCBINFO2               /* dcbinf */
        {
        USHORT usWriteTimeout;         /* WORD */
        USHORT usReadTimeout;          /* WORD */
        BYTE   fbCtlHndShake;          /* BYTE */
        BYTE   fbFlowReplace;          /* BYTE */
        BYTE   fbTimeout;              /* BYTE */
        BYTE   bErrorReplacementChar;  /* BYTE */
        BYTE   bBreakReplacementChar;  /* BYTE */
        BYTE   bXONChar;               /* BYTE */
        BYTE   bXOFFChar;              /* BYTE */
        } DCBINFO2;                                                    /* incorrectly defined in BSEDEV.h */
typedef DCBINFO2 *PDCBINFO2;                                           /* incorrectly defined in BSEDEV.h */

#define GETDCBINFO_SIZE                11                              /* not defined in BSEDEV.h */
#define SETDCBINFO_SIZE                11                              /* not defined in BSEDEV.h */

/* fbCtlHndShake ----------------------------------------- 7654 3210 */
//#define MODE_DTR_CONTROL             0x01             /* .... ...1 */
//#define MODE_DTR_HANDSHAKE           0x02             /* .... ..1. */
//#define MODE_CTS_HANDSHAKE           0x08             /* .... 1... */
//#define MODE_DSR_HANDSHAKE           0x10             /* ...1 .... */
//#define MODE_DCD_HANDSHAKE           0x20             /* ..1. .... */
//#define MODE_DSR_SENSITIVITY         0x40             /* .1.. .... */

/* fbFlowReplace ----------------------------------------- 7654 3210 */
//#define MODE_AUTO_TRANSMIT           0x01             /* .... ...1 */
//#define MODE_AUTO_RECEIVE            0x02             /* .... ..1. */
//#define MODE_ERROR_CHAR              0x04             /* .... .1.. */
//#define MODE_NULL_STRIPPING          0x08             /* .... 1... */
//#define MODE_BREAK_CHAR              0x10             /* ...1 .... */
#define   MODE_AUTO_FULL_DUPLEX        0x20             /* ..1. .... *//* not defined in BSEDEV.h */
//#define MODE_RTS_CONTROL             0x40             /* 01.. .... */
//#define MODE_RTS_HANDSHAKE           0x80             /* 10.. .... */
//#define MODE_TRANSMIT_TOGGLE         0xC0             /* 11.. .... */

/* fbTimeout --------------------------------------------- 7654 3210 */
//#define MODE_NO_WRITE_TIMEOUT        0x01             /* .... ...1 */
//#define MODE_READ_TIMEOUT            0x02             /* .... .01. */
//#define MODE_WAIT_READ_TIMEOUT       0x04             /* .... .10. */
//#define MODE_NOWAIT_READ_TIMEOUT     0x06             /* .... .11. */
#define   MODE_EXTD_BUFF_DISABLED      0x08             /* ...0 1... *//* not defined in BSEDEV.h */
#define   MODE_EXTD_BUFF_ENABLED       0x10             /* ...1 0... *//* not defined in BSEDEV.h */
#define   MODE_EXTD_BUFF_AUTO          0x18             /* ...1 1... *//* not defined in BSEDEV.h */
//         RX trigger = 1                               /* .00. .... */
#define   MODE_RX_TRIGGER_4            0x20             /* .01. .... *//* not defined in BSEDEV.h */
#define   MODE_RX_TRIGGER_8            0x40             /* .10. .... *//* not defined in BSEDEV.h */
#define   MODE_RX_TRIGGER_14           0x60             /* .11. .... *//* not defined in BSEDEV.h */
//         TX trigger = 1                               /* 0... .... */
#define   MODE_TX_BUFFER_16            0x80             /* 1... .... *//* not defined in BSEDEV.h */

/*********************************************************************/
/* ASYNC_GETENHPARM, ASYNC_SETENHPARM                                */
/*********************************************************************/

#pragma pack(1)                        /* pack on 1 byte boundary    */

typedef struct _ENHPARM                /* eparm */
        {
        BYTE   bEnhancedFlags;         /* BYTE  */
        ULONG  ulReserved;             /* DWORD 4-bytes */
        } ENHPARM;
typedef ENHPARM *PENHPARM;

#define GETENHPARM_SIZE                5                               /* not defined in BSEDEV.h */
#define SETENHPARM_SIZE                5                               /* not defined in BSEDEV.h */

/* bEnhancedFlags ---------------------------------------- 7654 3210 */
#define   MODE_ENH_SUPPORTED           0x01             /* .... ...1 *//* not defined in BSEDEV.h */
#define   MODE_ENH_ENABLED             0x02             /* .... ..1. *//* not defined in BSEDEV.h */
//        Disable DMA Receive                           /* .... 00.. */
#define   MODE_ENH_RXDMA_ENABLE        0x04             /* .... 01.. *//* not defined in BSEDEV.h */
#define   MODE_ENH_RXDMA_DED_CHAN      0x08             /* .... 10.. *//* not defined in BSEDEV.h */
//        Disable DMA Transmit                          /* ..00 .... */
#define   MODE_ENH_TXDMA_ENABLE        0x10             /* ..01 .... *//* not defined in BSEDEV.h */
#define   MODE_ENH_TXDMA_DED_CHAN      0x20             /* ..10 .... *//* not defined in BSEDEV.h */
#define   MODE_ENH_RX_IN_DMA           0x40             /* .1.. .... *//* not defined in BSEDEV.h */
#define   MODE_ENH_TX_IN_DMA           0x80             /* 1... .... *//* not defined in BSEDEV.h */

/*********************************************************************/
/* ASYNC_GETCOMMSTATUS                                               */
/*********************************************************************/

// Returns a single BYTE no struct required.

#define GETCOMMSTATUS_SIZE             1                               /* not defined in BSEDEV.h */

/* ------------------------------------------------------- 7654 3210 */
//#define TX_WAITING_FOR_CTS           0x01             /* .... ...1 */
//#define TX_WAITING_FOR_DSR           0x02             /* .... ..1. */
//#define TX_WAITING_FOR_DCD           0x04             /* .... .1.. */
//#define TX_WAITING_FOR_XON           0x08             /* .... 1... */
//#define TX_WAITING_TO_SEND_XON       0x10             /* ...1 .... */
//#define TX_WAITING_WHILE_BREAK_ON    0x20             /* ..1. .... */
//#define TX_WAITING_TO_SEND_IMM       0x40             /* .1.. .... */
//#define RX_WAITING_FOR_DSR           0x80             /* 1... .... */

/*********************************************************************/
/* ASYNC_GETLINESTATUS                                               */
/*********************************************************************/

// Returns a single BYTE no struct required.

#define GETLINESTATUS_SIZE             1                               /* not defined in BSEDEV.h */

/* ------------------------------------------------------- 7654 3210 */
//#define WRITE_REQUEST_QUEUED         0x01             /* .... ...1 */
//#define DATA_IN_TX_QUE               0x02             /* .... ..1. */
//#define HARDWARE_TRANSMITTING        0x04             /* .... .1.. */
//#define CHAR_READY_TO_SEND_IMM       0x08             /* .... 1... */
//#define WAITING_TO_SEND_XON          0x10             /* ...1 .... */
//#define WAITING_TO_SEND_XOFF         0x20             /* ..1. .... */

/*********************************************************************/
/* ASYNC_GETMODEMINPUT, ASYNC_GETMODEMOUTPUT                         */
/*********************************************************************/

// Returns a single BYTE no struct required.

#define GETMODEMIO_SIZE                1                               /* not defined in BSEDEV.h */

/* ASYNC_GETMODEMINPUT ----------------------------------- 7654 3210 */
//#define CTS_ON                       0x10             /* ...1 .... */
//#define DSR_ON                       0x20             /* ..1. .... */
//#define RI_ON                        0x40             /* .1.. .... */
//#define DCD_ON                       0x80             /* 1... .... */

/* ASYNC_GETMODEMOUTPUT ---------------------------------- 7654 3210 */
//#define DTR_ON                       0x01             /* .... ...1 */
//#define RTS_ON                       0x02             /* .... ..1. */

/*********************************************************************/
/* ASYNC_GETINQUECOUNT, ASYNC_GETOUTQUECOUNT                         */
/*********************************************************************/

#pragma pack(1)                        /* pack on 1 byte boundary    */

typedef struct _IOQUEUE                /* ioque */
        {
        USHORT usQueueCount;           /* WORD 2-bytes */
        USHORT usQueueSize;            /* WORD  */
        } IOQUEUE;
typedef IOQUEUE *PIOQUEUE;

#define GETIOQUEUE_SIZE                4                               /* not defined in BSEDEV.h */

/*********************************************************************/
/* ASYNC_GETCOMMEVENT                                                */
/*********************************************************************/

// Returns a single WORD (16bit==USHORT) no struct required.

#define GETCOMMEVENT_SIZE              2                               /* not defined in BSEDEV.h */

/* --------------------------------------------- FEDC BA98 7654 3210 */
//#define CHAR_RECEIVED                0x0001 /* .... .... .... ...1 */
#define RX_TIMEOUT_INT                 0x0002 /* .... .... .... ..1. *//* not defined in BSEDEV.h */
//#define LAST_CHAR_SENT               0x0004 /* .... .... .... .1.. */
//#define CTS_CHANGED                  0x0008 /* .... .... .... 1... */
//#define DSR_CHANGED                  0x0010 /* .... .... ...1 .... */
//#define DCD_CHANGED                  0x0020 /* .... .... ..1. .... */
//#define BREAK_DETECTED               0x0040 /* .... .... .1.. .... */
//#define ERROR_OCCURRED               0x0080 /* .... .... 1... .... */
//#define RI_DETECTED                  0x0100 /* .... ...1 .... .... */

/*********************************************************************/
/* Prototypes                                                        */
/*********************************************************************/

APIRET CallDevice(     HFILE hcomPort, ULONG     ulCategory, ULONG ulFunction, PVOID pParameters, PVOID pDataPacket );
APIRET ActiveSignal(   HFILE hcomPort, BYTE      bSignal );
APIRET FlushInput(     HFILE hcomPort );
APIRET FlushOutput(    HFILE hcomPort );

APIRET GetDcbInfo(     HFILE hcomPort, PDCBINFO2 pdcbPortInfo );
APIRET SetDcbInfo(     HFILE hcomPort, PDCBINFO2 pdcbPortInfo );
APIRET GetEnhParm(     HFILE hcomPort, PBYTE     pbEnhancedFlags );
APIRET SetEnhParm(     HFILE hcomPort, BYTE      bEnhancedFlags );
APIRET GetLnCtrl(      HFILE hcomPort, PEXTDRATE perateBaudRate, PLINECONTROL plnctlLineControl );
APIRET SetLnCtrl(      HFILE hcomPort, PEXTDRATE perateBaudRate, PLINECONTROL plnctlLineControl );
APIRET GetComStatus(   HFILE hcomPort, PBYTE     pbComStatus );
APIRET GetTxStatus(    HFILE hcomPort, PBYTE     pbTxStatus );
APIRET GetModemOutput( HFILE hcomPort, PBYTE     pbMdmOutput );
APIRET GetModemInput(  HFILE hcomPort, PBYTE     pbMdmInput );
APIRET SetBreakOff(    HFILE hcomPort );
APIRET SetBreakOn(     HFILE hcomPort );
APIRET TxImmediate(    HFILE hcomPort, BYTE      bAnyByte );
APIRET TxStop(         HFILE hcomPort );
APIRET TxStart(        HFILE hcomPort );
APIRET SetMdmOutput(   HFILE hcomPort, BYTE      bModemOn,   BYTE bModemOff );
APIRET GetRxCount(     HFILE hcomPort, PUSHORT   pusRxCount, PUSHORT pusRxSize );
APIRET GetTxCount(     HFILE hcomPort, PUSHORT   pusTxCount, PUSHORT pusTxSize );
APIRET GetComError(    HFILE hcomPort, PUSHORT   pusComError );
APIRET GetComEvent(    HFILE hcomPort, PUSHORT   pusComEvent );

APIRET PrtDcbInfo(     HFILE hcomPort );
APIRET WaitCarrier(    HFILE hcomPort, LONG      lTimeLimit );
APIRET WaitInput(      HFILE hcomPort, LONG      lTimeLimit );
APIRET SendBreak(      HFILE hcomPort, ULONG     ulDuration );

APIRET OpenPort(       PSZ pszName,    PHFILE    phcomPort );
APIRET ClosePort(      PHFILE phcomPort );
APIRET WritePort(      HFILE hcomPort, PSZ       pszBuffer, ULONG ulBuffLen, PULONG pulRemains );
APIRET ReadPort(       HFILE hcomPort, PBYTE    pbReadBuff, ULONG ulReadSize, PBYTE pbBuffer, PULONG pulBuffLen, LONG lTimeLimit, PSZ pszNullReplace, PSZ pszLineEnd );

/*********************************************************************/

#endif

/*********************************************************************/
/* END MODULE                                                        */
/*********************************************************************/
