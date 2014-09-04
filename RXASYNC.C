/*****************************************************************************/
/*                                                                           */
/*  MODULE         RXASYNC.C                                                 */
/*                                                                           */
/*  VERSION        Version 2.0 - 26th Dec 1994                               */
/*                                                                           */
/*  COPYRIGHT      Copyright (c) 1993, 1994 by Crucial Applications          */
/*                              All rights reserved.                         */
/*                                                                           */
/*  AUTHOR         Ian Timms                                                 */
/*                                                                           */
/*  DESCRIPTION    Rexx Asynchronous Communications Functions                */
/*                                                                           */
/*    The enclosed REXX DLL comprises a set of functions which are used to   */
/*    to extend the REXX language by providing FULL exposure of the ASYNC    */
/*    communications API. In addition a number of higher level functions     */
/*    are also provided to simplify the task of enabling REXX to be used     */
/*    as a scripting language for serial communications control.             */
/*                                                                           */
/*    They are by no means perfect but do function sufficiently well for the */
/*    author who uses them to provide a means of automating the collection   */
/*    of mail from the BBS.  If you have any comments or suggestions for     */
/*    improvement then I'd be glad to hear from you.                         */
/*                                                                           */
/*    The author can be contacted (in order of preference) at:               */
/*                                                                           */
/*         Bunyips Cave BBS    3:633/379                                     */
/*                                                                           */
/*         Internet            itimms@ariel.ucs.unimelb.edu.au               */
/*                                                                           */
/*         Compuserve          100236,1404                                   */
/*                                                                           */
/*         Snail mail          Ian Timms,                                    */
/*                             13 Cowper Street                              */
/*                             Footscray, Victoria,                          */
/*                             Australia, 3011                               */
/*                                                                           */
/*  FUNCTIONS                                                                */
/*                                                                           */
/*       House keeping                                                       */
/*                                                                           */
/*    RxAsyncDropFuncs     - + Drop the functions from REXX                  */
/*    RxAsyncLoadFuncs     -   Load the functions into REXX                  */
/*                                                                           */
/*       Process control                                                     */
/*                                                                           */
/*    RxAsyncPriority      -   Set Rexx process priority                     */
/*                                                                           */
/*       ASYNC IOCTL interface                                               */
/*                                                                           */
/*    RxAsyncGetDcbInfo    -   Get the device control block info             */
/*    RxAsyncSetDcbInfo    -   Set the device control block info             */
/*    RxAsyncGetEnhParm    -   Get the enhanced parameters                   */
/*    RxAsyncSetEnhParm    -   Set the enhanced parameters                   */
/*    RxAsyncGetLnCtrl     -   Get the comms port settings                   */
/*    RxAsyncSetLnCtrl     -   Set the comms port settings                   */
/*                                                                           */
/*    RxAsyncSetBreakOff   -   Turn break off                                */
/*    RxAsyncSetBreakOn    -   Turn break on                                 */
/*    RxAsyncTxImmediate   -   Transmit byte immediately                     */
/*    RxAsyncTxStop        -   Stop transmission (ala XOFF)                  */
/*    RxAsyncTxStart       -   Start transmission (ala XON)                  */
/*    RxAsyncGetComStatus  -   Get COM status                                */
/*    RxAsyncGetTxStatus   -   Get Transmit status                           */
/*    RxAsyncGetMdmOutput  -   Get modem output signals                      */
/*    RxAsyncGetMdmInput   -   Get modem input signals                       */
/*    RxAsyncSetMdmOutput  -   Set modem output signals                      */
/*    RxAsyncGetRxCount    -   Get size and number of chars in RX queue      */
/*    RxAsyncGetTxCount    -   Get size and number of chars in TX queue      */
/*    RxAsyncGetComError   -   Get the COM Error Word                        */
/*    RxAsyncGetComEvent   -   Get the COM Event Word                        */
/*                                                                           */
/*       Extensions                                                          */
/*                                                                           */
/*    RxAsyncOpen          -   Open the specified comms port                 */
/*    RxAsyncClose         -   Close the specified comms port                */
/*    RxAsyncWait          -   Wait for receipt of input                     */
/*    RxAsyncWatch         -   Wait for receipt of specified strings         */
/*    RxAsyncRead          -   Read data and wait if necessary               */
/*    RxAsyncWrite         -   Write data to the comms port                  */
/*    RxAsyncCarrier       -   Test/Wait for Data Carrier Detect (DCD)       */
/*    RxAsyncBreak         -   Send break for defined period                 */
/*    RxAsyncDcbInfo       -   Print the device control block info           */
/*    RxAsyncActive        -   Test for active modem signal                  */
/*    RxAsyncRaise         -   Raise specified modem signal                  */
/*    RxAsyncLower         -   Lower specified modem signal                  */
/*                                                                           */
/*       File handling                                                       */
/*                                                                           */
/*    RxAsyncFileExists    - + Test for file(s) existence                    */
/*    RxAsyncFileDelete    - + Delete specified file                         */
/*    RxAsyncFileCopy      - + Copy specified file                           */
/*    RxAsyncFileMove      - + Move specified file                           */
/*    RxAsyncFileRename    - + Rename specified file                         */
/*    RxAsyncFileNameIs    - + Determine file name                           */
/*    RxAsyncFilePathIs    - + Determine file path                           */
/*    RxAsyncDirectory     - + Query/Change directory/drive                  */
/*                                                                           */
/*       Miscellaneous                                                       */
/*                                                                           */
/*    RxAsyncSleep         - + Wait for some defined period                  */
/*    RxAsyncPrint         - + Print string on stdout                        */
/*    RxAsyncMsgBox        - + Display PM message box (PM use only)          */
/*                                                                           */
/*  FILES                                                                    */
/*                                                                           */
/*    READ.ME      -  Documentation (such as it is)                          */
/*    RXASYNC.C    -  Rexx async comms functions DLL source                  */
/*    RXASYNC.H    -  Header file                                            */
/*    RXVPFUNC.C   -  Rexx variable pool functions                           */
/*    RXVPFUNC.H   -  Header file                                            */
/*    COMSFUNC.C   -  Communications functions                               */
/*    COMSFUNC.H   -  Header file                                            */
/*    FILEFUNC.C   -  File system functions                                  */
/*    FILEFUNC.H   -  Header file                                            */
/*    MISCFUNC.C   -  Miscellaneous functions                                */
/*    MISCFUNC.H   -  Header file                                            */
/*    RXASYNC.DEF  -  Definitions file for linker                            */
/*    RXASYNC.DEP  -  Dependancy file make                                   */
/*    RXASYNC.MAK  -  Make file                                              */
/*    RXASYNC.DLL  -  The final product                                      */
/*    RXASYNC.CMD  -  Rexx script showing basic usage of DLL                 */
/*    RXMSG.CMD    -  Rexx script showing usage with message boxes           */
/*                                                                           */
/*  LICENSE                                                                  */
/*                                                                           */
/*    This Program is released to the public, you may use this Program in    */
/*    any way you see fit, pursuant to the following restrictions:           */
/*                                                                           */
/*    o  You may not sell the Program or use this code _in_ a commercial     */
/*       application nor may you distribute this Program as a part of any    */
/*       such application without the express permission of the author;      */
/*                                                                           */
/*    o  The Program may be freely used for non commercial and educational   */
/*       purposes, privately and by registered charities, non-profit         */
/*       organisations and educational institutions;                         */
/*                                                                           */
/*    o  Distribution on Shareware and Developer CD-ROMs is also permitted;  */
/*                                                                           */
/*    o  You may not charge a fee for distribution of the Program;           */
/*                                                                           */
/*    o  The Program shall be supplied in its original, unmodified form, as  */
/*       distributed by the author, which includes all source code;          */
/*                                                                           */
/*    o  Share/Freeware authors wishing to incorporate this program in the   */
/*       distribution package of their own product may do so freely provided */
/*       the FULL RxAsync archive is included intact;                        */
/*                                                                           */
/*    o  Special dispensation to distribute the RXASYNC.DLL on its own may   */
/*       be given upon request.                                              */
/*                                                                           */
/*  DISCLAIMER                                                               */
/*                                                                           */
/*    Use of this Program and/or documentation for any purpose whatsoever    */
/*    constitutes your unqualified acceptance of the following statements.   */
/*                                                                           */
/*    The author, Ian Timms, makes no warranty or representation that the    */
/*    Program or Documentation will be error free.  The author disclaims     */
/*    any warranties, either express or implied, including but not limited   */
/*    to any implied warranty of merchantability or fitness for any          */
/*    particular purpose.                                                    */
/*                                                                           */
/*    The author has no obligation to provide service, defect correction, or */
/*    any maintenance for the Program.  The author has no obligation to      */
/*    supply any Program updates or enhancements to you even if such are or  */
/*    later become available.                                                */
/*                                                                           */
/*    In no event will the author be liable for any damages whatsoever       */
/*    (including without limitation damages for loss of business profits,    */
/*    business interruption, loss of business information or other           */
/*    incidental or consequential damages) arising out of the use of,        */
/*    interruption in the use of, or inability to use the Program, even if   */
/*    the author has been advised of any possibility or likelihood of such   */
/*    damages.                                                               */
/*                                                                           */
/*    The user agrees to take full responsibility for the selection of and   */
/*    any use whatsoever made of the Program and Documentation such as it    */
/*    exists.                                                                */
/*                                                                           */
/*  NOTES                                                                    */
/*                                                                           */
/*    Online timeout etc. is checked in hundredths of a second as this is    */
/*    the maximum granularity available with standard OS/2 calls.  Use of a  */
/*    high resolution timer might enhance this aspect significantly but I    */
/*    have not yet found a need for this. The MyDiffTime routine which is    */
/*    used for handling timeout calculation can only calulate a maximum      */
/*    period difference to within 1 week which should be sufficient.         */
/*                                                                           */
/*    The routines themselves do NOT check for the loss of carrier.  Such    */
/*    checking as exists in the communications drivers is relied upon for    */
/*    this. I use the NO CARRIER response from the modem and this does the   */
/*    trick reasonably well. There is now an RxAsyncCarrier function which   */
/*    will tell you the carrier status, ideal for loop control.              */
/*                                                                           */
/*    Nulls are NOT stripped from any input read from the device. I had      */
/*    initially intended doing this but have so far not implemented this     */
/*    facility, even though the majority of the code is already there. I     */
/*    should also note that it would only strip nulls from the scripted      */
/*    side of things, that is to say I would NOT tell the device driver to   */
/*    do it, that would be an unwise move for reasons which should become    */
/*    obvious when you think about the effect it would have on an external   */
/*    protocol. The facility to strip nulls has now been implemented in the  */
/*    RxAsyncRead routine, you can optionally specify a replacement value.   */
/*                                                                           */
/*    Please use the RxAsyncPrint function to display all "stdout" style     */
/*    output, in this way you will correctly see the error results returned  */
/*    by the routines should a failure occur. It also avoids the problem of  */
/*    REXX adding <cr><lf> to everything.                                    */
/*                                                                           */
/*    In order to make sure that output containing ANSI escape sequences is  */
/*    correctly handled you will need to load the ANSI.sys driver in your    */
/*    CONFIG.sys file.                                                       */
/*                                                                           */
/*    The file handling functions which I have added are NOT replacements    */
/*    for the standard OS/2 command line functions. They do NOT handle       */
/*    wildcard nameing (except for FileExists) and unpredictable results     */
/*    may occur if you attempt to do so.  What the functions WILL do is      */
/*    enable you to easily move/copy/delete/rename and find a file without   */
/*    having to shell out to the CMD.EXE command processor.  The FileMove    */
/*    function has also been written so that it is possible to move a file   */
/*    across drives, something the standard MOVE command will not allow.     */
/*                                                                           */
/*    I have now implemented propper device control block handling which     */
/*    permits setting of more reasonable timeouts using the standard OS/2    */
/*    COM.SYS driver. The default values of 1 minute can now be overridden   */
/*    after the communications port is opened by using the RxAsyncSetDcbInfo */
/*    function.                                                              */
/*                                                                           */
/*    If you use the COM.SYS driver supplied with OS/2 2.x GA and elect NOT  */
/*    to set read and write timeouts using the RxAsyncSetDcbInfo function    */
/*    then you'll find the functionality of these routines a little lacking. */
/*                                                                           */
/*    To control handshaking, extended buffering etc. etc. I recommend that  */
/*    you use the MODE command to set up your port appropriately before      */
/*    using these routines. You can also do this using the RxAsyncSetDcbInfo */
/*    and RxAsyncSetLnCtrl functions but generally it is much simpler to use */
/*    the MODE command. Locking your pc<->modem port speed is highly         */
/*    recommended.                                                           */
/*                                                                           */
/*    The following MODE setting is recommended to be placed in your         */
/*    STARTUP.CMD file for best results:                                     */
/*                                                                           */
/*       MODE COM4:19200,N,8,1,TO=OFF,XON=OFF,        full duplex            */
/*                             OCTS=ON,RTS=HS,        hardware CTS/RTS       */
/*                             DTR=ON,                DTR control mode       */
/*                             IDSR=OFF,ODSR=OFF,     disable DSR checking   */
/*                             BUFFER=ON              bufferred              */
/*                                                                           */
/*    Note that I have DSR checking disabled, my modem does not appear to    */
/*    handle the DSR signal correctly, if you are not getting any response   */
/*    from yours then the same may well be true. Also having this enabled    */
/*    may cause unpredictable responses when using the SIO drivers.          */
/*    (Ray may have fixed this now, however I do not use SIO these days      */
/*    except for testing so your mileage may vary.)                          */
/*                                                                           */
/*    You should also use the following Hayes or equivalent modem settings:  */
/*                                                                           */
/*        E0, Q0, V1, X4 or X6, &C1, &D2                                     */
/*                                                                           */
/*    If you do NOT have a 16550 UART then you will need to make sure that   */
/*    the DCB information supplied to the RxAsyncSetDcbInfo routine is       */
/*    appropriate. Usually this means using '00000010' for Flags3 and        */
/*    setting BUFFER=OFF in the MODE command above.                          */
/*                                                                           */
/*    If you need a more in depth discussion of handling DCB parameters      */
/*    etc. then the best initial source is the SIOREF document provided      */
/*    with the SIO drivers. Second best although more comprehensive is       */
/*    the OS/2 Physical Device Driver reference, it's heavier reading.<g>    */
/*                                                                           */
/*  HISTORY                                                                  */
/*                                                                           */
/*    14-Aug-1993  Original release.                                         */
/*    24-Aug-1993  Added carrier test function. Thanks to M.N.Stephenson.    */
/*    04-Sep-1993  Added file handling functions.                            */
/*    02-Nov-1993  Added RxAsyncGetDcbInfo function.                         */
/*    06-Nov-1993  Added RxAsyncBreak function.                              */
/*    18-Dec-1993  Changed RxAsyncGetDcbInfo to RxAsyncDcbInfo               */
/*    18-Dec-1993  Improved documentation                                    */
/*    11-Mar-1994  Permit passing empty string to RxAsyncWrite.              */
/*    11-Mar-1994  Changed both read and write timeouts to 1 second.         */
/*    11-Mar-1994  Added RxAsyncGetDcbInfo function.                         */
/*    11-Mar-1994  Added RxAsyncGetEnhParm function.                         */
/*    03-May-1994  Added generic Str2Num function.                           */
/*    03-May-1994  Added RxAsyncSetDcbInfo function.                         */
/*    03-May-1994  Added RxAsyncSetEnhParm function.                         */
/*    03-May-1994  Removed forcing of full duplex and read/write timeouts.   */
/*    03-May-1994  Changed name to RXASYNC.                                  */
/*    03-May-1994  Cleaned up parameter validations.                         */
/*    20-May-1994  Added extra modem control and signal query functions.     */
/*    22-May-1994  Prepared for distribution as version 1.5.                 */
/*                                                                           */
/*    22-Oct-1994  Added RxAsyncMsgBox function for PM usage.                */
/*    13-Nov-1994  Added ability to optionally specify a null replace char   */
/*                 in the RxAsyncRead function. And corrected to return ALL  */
/*                 data read by default, regardless of nulls. Also fixed a   */
/*                 minor performance bug when waiting for data.              */
/*    13-Nov-1994  Expanded RxAsyncFileNameIs and RxAsyncFilePathIs funcs    */
/*                 to permit an empty string as a parameter. Corrected bug   */
/*                 in FilePathIs where it was modifying the arguement.       */
/*    17-Nov-1994  Added ability to optionally specify a timeout for the     */
/*                 RxAsyncCarrier function to enable wait_for_carrier usage. */
/*    26-Nov-1994  Enhanced RxAsyncLoadFuncs so that it caters for any       */
/*                 load failures and unloads functions if unable to          */
/*                 complete successfully.                                    */
/*    26-Nov-1994  Added/enhanced rexx variable pool miscellaneous           */
/*                 functions to cater for fetch only and stem handling.      */
/*    27-Nov-1994  Added stub for RxAsyncWatch function.                     */
/*    28-Nov-1994  Split RXASYNC module into pieces, resulting in modules    */
/*                 COMSFUNC, FILEFUNC, RXVPFUNC, MISCFUNC and RXASYNC.       */
/*    02-Dec-1994  Added RxAsyncActive, RxAsyncRaise and RxAsyncLower.       */
/*    07-Dec-1994  Corrected bug in RxAsyncSetEnhParm function.              */
/*    08-Dec-1994  Added optional parameter to RxAsyncRead to allow user to  */
/*                 specify an additional line termination character to those */
/*                 currently used. (defaults are LF, FF and EOF).            */
/*    24-Dec-1994  Completed work on RxAsyncWatch function.                  */
/*    26-Dec-1994  Added RxAsyncRC return code variable. RxAsyncRc = 0 if    */
/*                 all okay, non zero if error, where the value is the error */
/*                 return code of the DosCall made, normally an IOCTL.       */
/*                 For obvious reasons the File Handling and Miscellaneous   */
/*                 functions do not set this variable, nor does DropFuncs.   */
/*    26-Dec-1994  Prepared for distribution as version 2.0.                 */
/*                                                                           */
/*  KNOWN PROBLEMS                                                           */
/*                                                                           */
/*    There is a bug in OS/2 2.1 GA which causes output printed to stdout by */
/*    a DLL function to appear on stderr when stdout output from a command   */
/*    file is redirected.  It is not know whether this problem has been      */
/*    resolved in later versions, if this poses a problem for you then       */
/*    please raise it with IBM.                                              */
/*                                                                           */
/*  FUTURES                                                                  */
/*                                                                           */
/*    Add internal functions for ASCII file download/upload.                 */
/*                                                                           */
/*  FORTUNE TELLING                                                          */
/*                                                                           */
/*    Create a wraparound for the RexxStart command so that a script can     */
/*     be run under PM using message boxes etc. and get its name registered  */
/*     in the OS/2 task list. The beginnings of a propper scripting tool.    */
/*    Add RxAsyncReadBuffer variable so that reading can be handled better.  */
/*    Add internal functions for Zmodem download/upload.                     */
/*    Add ability to use system messages in RxAsyncMsgBox function.          */
/*    Add DBCS enablement where required.                                    */
/*    Internalise the whole shebang into an EXE scripting tool. Eventually!  */
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
#define  INCL_DOSDEVIOCTL

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <rexxsaa.h>

#include "miscfunc.h"
#include "rxvpfunc.h"
#include "filefunc.h"
#include "comsfunc.h"
#include "rxasync.h"

/*********************************************************************/
/* Declare exported functions as REXX functions.                     */
/*********************************************************************/

RexxFunctionHandler RxAsyncDropFuncs;
RexxFunctionHandler RxAsyncLoadFuncs;

RexxFunctionHandler RxAsyncPriority;
RexxFunctionHandler RxAsyncSleep;

RexxFunctionHandler RxAsyncGetDcbInfo;
RexxFunctionHandler RxAsyncSetDcbInfo;
RexxFunctionHandler RxAsyncGetEnhParm;
RexxFunctionHandler RxAsyncSetEnhParm;
RexxFunctionHandler RxAsyncGetLnCtrl;
RexxFunctionHandler RxAsyncSetLnCtrl;
RexxFunctionHandler RxAsyncSetBreakOff;
RexxFunctionHandler RxAsyncSetBreakOn;
RexxFunctionHandler RxAsyncTxImmediate;
RexxFunctionHandler RxAsyncTxStop;
RexxFunctionHandler RxAsyncTxStart;
RexxFunctionHandler RxAsyncGetComStatus;
RexxFunctionHandler RxAsyncGetTxStatus;
RexxFunctionHandler RxAsyncGetMdmOutput;
RexxFunctionHandler RxAsyncSetMdmOutput;
RexxFunctionHandler RxAsyncGetMdmInput;
RexxFunctionHandler RxAsyncGetRxCount;
RexxFunctionHandler RxAsyncGetTxCount;
RexxFunctionHandler RxAsyncGetComError;
RexxFunctionHandler RxAsyncGetComEvent;

RexxFunctionHandler RxAsyncOpen;
RexxFunctionHandler RxAsyncClose;
RexxFunctionHandler RxAsyncWait;
RexxFunctionHandler RxAsyncRead;
RexxFunctionHandler RxAsyncWrite;
RexxFunctionHandler RxAsyncPrint;
RexxFunctionHandler RxAsyncCarrier;
RexxFunctionHandler RxAsyncBreak;
RexxFunctionHandler RxAsyncDcbInfo;

RexxFunctionHandler RxAsyncFileExists;
RexxFunctionHandler RxAsyncFileDelete;
RexxFunctionHandler RxAsyncFileCopy;
RexxFunctionHandler RxAsyncFileMove;
RexxFunctionHandler RxAsyncFileRename;
RexxFunctionHandler RxAsyncFileNameIs;
RexxFunctionHandler RxAsyncFilePathIs;
RexxFunctionHandler RxAsyncDirectory;

RexxFunctionHandler RxAsyncMsgBox;
RexxFunctionHandler RxAsyncWatch;

RexxFunctionHandler RxAsyncFlushInput;
RexxFunctionHandler RxAsyncFlushOutput;

RexxFunctionHandler RxAsyncActive;
RexxFunctionHandler RxAsyncRaise;
RexxFunctionHandler RxAsyncLower;

/*********************************************************************/
/* RxFncTable                                                        */
/*   Array of names of the RXASYNC functions.                        */
/*   This list is used for registration and deregistration.          */
/*********************************************************************/

#pragma pack(1)                        /* pack on 1 byte boundary    */

static PSZ  RxFncTable[] =
   {
      "RxAsyncDropFuncs"
   ,  "RxAsyncLoadFuncs"
   ,  "RxAsyncPriority"
   ,  "RxAsyncSleep"
   ,  "RxAsyncGetDcbInfo"
   ,  "RxAsyncSetDcbInfo"
   ,  "RxAsyncGetEnhParm"
   ,  "RxAsyncSetEnhParm"
   ,  "RxAsyncGetLnCtrl"
   ,  "RxAsyncSetLnCtrl"
   ,  "RxAsyncSetBreakOff"
   ,  "RxAsyncSetBreakOn"
   ,  "RxAsyncTxImmediate"
   ,  "RxAsyncTxStop"
   ,  "RxAsyncTxStart"
   ,  "RxAsyncGetComStatus"
   ,  "RxAsyncGetTxStatus"
   ,  "RxAsyncGetMdmOutput"
   ,  "RxAsyncSetMdmOutput"
   ,  "RxAsyncGetMdmInput"
   ,  "RxAsyncGetRxCount"
   ,  "RxAsyncGetTxCount"
   ,  "RxAsyncGetComError"
   ,  "RxAsyncGetComEvent"
   ,  "RxAsyncOpen"
   ,  "RxAsyncClose"
   ,  "RxAsyncWait"
   ,  "RxAsyncRead"
   ,  "RxAsyncWrite"
   ,  "RxAsyncPrint"
   ,  "RxAsyncCarrier"
   ,  "RxAsyncBreak"
   ,  "RxAsyncDcbInfo"
   ,  "RxAsyncFileExists"
   ,  "RxAsyncFileDelete"
   ,  "RxAsyncFileCopy"
   ,  "RxAsyncFileMove"
   ,  "RxAsyncFileRename"
   ,  "RxAsyncFileNameIs"
   ,  "RxAsyncFilePathIs"
   ,  "RxAsyncDirectory"
   ,  "RxAsyncMsgBox"
   ,  "RxAsyncWatch"
   ,  "RxAsyncFlushInput"
   ,  "RxAsyncFlushOutput"
   ,  "RxAsyncActive"
   ,  "RxAsyncRaise"
   ,  "RxAsyncLower"
   };

static CHAR RxAsyncRcVarName[10] = "RxAsyncRc";

/*********************************************************************/
/* Function:  DropRxFuncs( entries )                                 */
/* Purpose:   Deregister the specified number of function entries.   */
/* Return:    none.                                                  */
/*********************************************************************/
VOID DropRxFuncs( LONG entries )
{
   LONG i;
   i = entries;
   while (i > 0) {
      i = i - 1;
      RexxDeregisterFunction( RxFncTable[i] );
   }
   return;
}

/*********************************************************************/
/* Function:  LoadRxFuncs                                            */
/* Purpose:   Register the functions in the DLL to REXX.             */
/* Return:    True if successful, False if not.                      */
/*********************************************************************/
BOOL LoadRxFuncs( VOID )
{
   APIRET rxrc;                        /* Rexx function return       */
   LONG   entries;                     /* Num of entries             */
   LONG   i;                           /* Counter                    */
   entries = sizeof( RxFncTable ) / sizeof( PSZ );
   i = 0;
   while (i < entries) {
      rxrc = RexxRegisterFunctionDll( RxFncTable[i],
                                      RXASYNC_DLL_NAME,
                                      RxFncTable[i] );
      if ( rxrc == RXFUNC_NOMEM ) {    /* Registration failed        */
         DropRxFuncs( i );
         return( FALSE );
      }
      i = i + 1;
   }
   return( TRUE );
}

/*********************************************************************/
/* RXASYNC functions.                                                */
/*********************************************************************/

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncDropFuncs                                       */
/*                                                                   */
/* Purpose:   Unload the functions                                   */
/*                                                                   */
/* Syntax:    call RxAsyncDropFuncs                                  */
/*                                                                   */
/* Parms:     none                                                   */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   call RxAsyncDropFuncs                                           */
/*   Say 'RxAsyncDropFuncs is complete.'                             */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncDropFuncs(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   if ( numargs != 0 )                 /* Must have no arguments     */
      return( ROUTINE_INVALID );

   DropRxFuncs( (LONG) (sizeof( RxFncTable ) / sizeof( PSZ )) );

   BUILDRXDOSRET( retstr, NO_ERROR );  /* Initialise return value    */

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncLoadFuncs                                       */
/*                                                                   */
/* Purpose:   Load the functions                                     */
/*                                                                   */
/* Syntax:    call RxAsyncLoadFuncs                                  */
/*                                                                   */
/* Params:    none                                                   */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - ERROR_NOT_ENOUGH_MEMORY (8)                   */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   call RxFuncAdd 'RxAsyncLoadFuncs','RXASYNC','RxAsyncLoadFuncs'  */
/*   call RxAsyncLoadFuncs                                           */
/*   Say 'RxAsyncLoadFuncs is complete.'                             */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncLoadFuncs(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET    rc;                       /* Function return code       */

   if ( numargs != 0 )                 /* Must have no arguments     */
      return( ROUTINE_INVALID );

   if ( LoadRxFuncs() ) {
      rc = NO_ERROR;                   /* Load successful            */
   } else {
      rc = ERROR_NOT_ENOUGH_MEMORY;    /* Load failed                */
   }

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );

   return( ROUTINE_VALID );            /* All ok                     */
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncPriority                                        */
/*                                                                   */
/* Purpose:   Set Rexx process priority                              */
/*                                                                   */
/* Syntax:    rc = RxAsyncPriority( class, level )                   */
/*                                                                   */
/* Params:    class - Priority class 0..4                            */
/*                       0 = NOCHANGE                                */
/*                       1 = IDLETIME                                */
/*                       2 = REGULAR                                 */
/*                       3 = TIMECRITICAL                            */
/*                       4 = FOREGROUNDSERVER                        */
/*            level - Change current level -31..+31                  */
/*                       (normally zero)                             */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosSetPriority                        */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   Crit    = 3                                                     */
/*   NormLvl = 0                                                     */
/*   rc = RxAsyncPriority( Crit, NormLvl )                           */
/*   if rc <> 0                                                      */
/*   then say 'RxAsyncPriority failed with rc=>'rc'<'                */
/*   else say 'RxAsyncPriority, Class=>'Crit'<, Level=>'NormLvl'<'   */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncPriority(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET    rc;                       /* Function return code       */
   ULONG     ulClass;                  /* Priority class             */
   LONG      lLevel;                   /* Priority level             */

   if ( numargs != 2 )                 /* Must have two arguments    */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify class         */
       || !RXVALIDSTRING(args[1]) )    /* and priority level         */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &ulClass, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert class failed       */

   if ( !Str2Num(args[1].strptr, &lLevel, STR2NUM_DEC2LONG) )
      return( ROUTINE_INVALID );       /* Convert level failed       */

   if (   (ulClass < PRTYC_NOCHANGE)
       || (ulClass > PRTYC_FOREGROUNDSERVER) )
      return( ROUTINE_INVALID );       /* Invalid class              */

   if (   (lLevel < PRTYD_MINIMUM)
       || (lLevel > PRTYD_MAXIMUM) )
      return( ROUTINE_INVALID );       /* Invalid priority           */

   rc = SetPriority( ulClass, lLevel );

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncSleep                                           */
/*                                                                   */
/* Purpose:   Sleep the specified length of time                     */
/*                                                                   */
/* Syntax:    call RxAsyncSleep milliseconds                         */
/*                                                                   */
/* Params:    millisecs - length of time to sleep                    */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   say 'Sleep for a bit 'time()                                    */
/*   call RxAsyncSleep 2000                                          */
/*   say 'Slept enough 'time()                                       */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncSleep(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   LONG  millisecs;                    /* Time to sleep              */

   if ( numargs != 1 )                 /* Must have one argument     */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0]) )    /* Must specify time limit    */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &millisecs, STR2NUM_DEC2LONG) )
      return( ROUTINE_INVALID );       /* Convert millisecs failed   */

   if ( millisecs < 0 )                /* Can not wait negative time */
      return( ROUTINE_INVALID );

   if ( millisecs > 0 )                /* Only if its worthwile      */
      DosSleep( millisecs );

   BUILDRXDOSRET( retstr, NO_ERROR );  /* Initialise return value    */

   return( ROUTINE_VALID );            /* All ok                     */
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncGetDcbInfo                                      */
/*                                                                   */
/* Purpose:   Return current port settings                           */
/*                                                                   */
/*            Utilises IOCTL 73H ASYNC_GETDCBINFO.                   */
/*                                                                   */
/* Syntax:    call RxAsyncGetDcbInfo handlevar, write_timeout_var,   */
/*                                              read_timeout_var,    */
/*                                              handshake_var,       */
/*                                              flow_replace_var,    */
/*                                              timeout_var,         */
/*                                              error_replace_var,   */
/*                                              break_replace_var,   */
/*                                              xon_var,             */
/*                                              xoff_var             */
/*                                                                   */
/* Params:    handlevar  - rexx variable with com handle             */
/*            writevar   - rexx variable to store write timeout      */
/*            readvar    - rexx variable to store read timeout       */
/*            handflgvar - rexx variable to store handshake flags 1  */
/*            flowflgvar - rexx variable to store flow-repl flags 2  */
/*            timeflgvar - rexx variable to store timeout flags 3    */
/*            errreplvar - rexx variable to store error-repl char    */
/*            brkreplvar - rexx variable to store break-repl char    */
/*            xonvar     - rexx variable to store xon char           */
/*            xoffvar    - rexx variable to store xoff char          */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - ERROR_NOT_ENOUGH_MEMORY (8)                   */
/*          "        - ERROR_INVALID_PARAMETER (87)                  */
/*          "        - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   rc = RxAsyncGetDcbInfo( PortHandle, 'wlim','rlim','flags1',     */
/*                           'flags2','flags3','errch','brkch',      */
/*                           'xonch','xoffch' )                      */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncGetDcbInfo failed with rc=>'rc'<'           */
/*      else say 'RxAsyncGetDcbInfo returned okay.'                  */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncGetDcbInfo(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET   rc;                        /* Function return code       */
   APIRET   rxrc;                      /* RexxVariablePool return    */
   HFILE    hcomPort;                  /* Communications port handle */
   DCBINFO2 dcbPortInfo;               /* Device control block       */
   SHVBLOCK WlimVar;                   /* Rexx var for write timeout */
   SHVBLOCK RlimVar;                   /* Rexx var for read timeout  */
   SHVBLOCK Flg1Var;                   /* Rexx var for flags 1       */
   SHVBLOCK Flg2Var;                   /* Rexx var for flags 2       */
   SHVBLOCK Flg3Var;                   /* Rexx var for flags 3       */
   SHVBLOCK ErrcVar;                   /* Rexx var for err repl char */
   SHVBLOCK BrkcVar;                   /* Rexx var for brk repl char */
   SHVBLOCK XoncVar;                   /* Rexx var for xon char      */
   SHVBLOCK XoffVar;                   /* Rexx var for xoff char     */

   if ( numargs != 10 )                /* Must have ten arguments    */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port          */
       || !RXVALIDSTRING(args[1])      /* and write timeout var name */
       || !RXVALIDSTRING(args[2])      /* and read timeout var name  */
       || !RXVALIDSTRING(args[3])      /* and flags 1 var name       */
       || !RXVALIDSTRING(args[4])      /* and flags 2 var name       */
       || !RXVALIDSTRING(args[5])      /* and flags 3 var name       */
       || !RXVALIDSTRING(args[6])      /* and error replace var name */
       || !RXVALIDSTRING(args[7])      /* and break replace var name */
       || !RXVALIDSTRING(args[8])      /* and xon character var name */
       || !RXVALIDSTRING(args[9]) )    /* and xoff character var name*/
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   rc = GetDcbInfo( hcomPort, &dcbPortInfo );

   if ( rc == NO_ERROR ) {

      /* fetch rexx variables details (rexx mallocs, we free) */

      InitVar( &WlimVar, &RlimVar, args[1].strptr );
      InitVar( &RlimVar, &Flg1Var, args[2].strptr );
      InitVar( &Flg1Var, &Flg2Var, args[3].strptr );
      InitVar( &Flg2Var, &Flg3Var, args[4].strptr );
      InitVar( &Flg3Var, &ErrcVar, args[5].strptr );
      InitVar( &ErrcVar, &BrkcVar, args[6].strptr );
      InitVar( &BrkcVar, &XoncVar, args[7].strptr );
      InitVar( &XoncVar, &XoffVar, args[8].strptr );
      InitVar( &XoffVar, NULL,     args[9].strptr );

      rc = FetchVar( &WlimVar );       /* GET the rexx variables     */

      if ( rc == NO_ERROR ) {

         /* set rexx variable details with retrieved values */

         EditVar( &WlimVar, VAL2STR_USHORT2DEC, &(dcbPortInfo.usWriteTimeout) );
         EditVar( &RlimVar, VAL2STR_USHORT2DEC, &(dcbPortInfo.usReadTimeout) );
         EditVar( &Flg1Var, VAL2STR_BYTE2HEX,   &(dcbPortInfo.fbCtlHndShake) );
         EditVar( &Flg2Var, VAL2STR_BYTE2HEX,   &(dcbPortInfo.fbFlowReplace) );
         EditVar( &Flg3Var, VAL2STR_BYTE2HEX,   &(dcbPortInfo.fbTimeout) );
         EditVar( &ErrcVar, VAL2STR_BYTE2HEX,   &(dcbPortInfo.bErrorReplacementChar) );
         EditVar( &BrkcVar, VAL2STR_BYTE2HEX,   &(dcbPortInfo.bBreakReplacementChar) );
         EditVar( &XoncVar, VAL2STR_BYTE2HEX,   &(dcbPortInfo.bXONChar) );
         EditVar( &XoffVar, VAL2STR_BYTE2HEX,   &(dcbPortInfo.bXOFFChar) );

         rc = UpdateVar( &WlimVar );   /* SET the rexx variables     */
      }

      FreeVar( &WlimVar );             /* Cleanup                    */
   }

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosDevIOCtl result  */

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncSetDcbInfo                                      */
/*                                                                   */
/* Purpose:   Update current port settings                           */
/*                                                                   */
/*            Utilises IOCTLs 53H ASYNC_SETDCBINFO                   */
/*                        and 73H ASYNC_GETDCBINFO.                  */
/*                                                                   */
/* Syntax:    call RxAsyncSetDcbInfo port,                           */
/*                                   write_timeout,                  */
/*                                   read_timeout,                   */
/*                                   handshake,                      */
/*                                   flow_replace,                   */
/*                                   timeout,                        */
/*                                   error_replace,                  */
/*                                   break_replace,                  */
/*                                   xon,                            */
/*                                   xoff                            */
/*                                                                   */
/* Params:    port       - com port handle                           */
/*            wlimit     - write timeout                             */
/*            rlimit     - read timeout                              */
/*            flags1     - handshake flags 1                         */
/*            flags2     - flow-replace flags 2                      */
/*            flags3     - timeout flags 3                           */
/*            err_repl   - error-replace char                        */
/*            brk_repl   - break-replace char                        */
/*            xon        - xon char                                  */
/*            xoff       - xoff char                                 */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Notes:                                                            */
/*   Parameters other than the PortHandle are optional in that if    */
/*   specified as null items they will not cause the existing        */
/*   settings to be alterred.                                        */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   rc = RxAsyncSetDcbInfo( PortHandle,                             */
/*                           100,                                    */
/*                           50,                                     */
/*                           '01001001',                             */
/*                           '10100000',                             */
/*                           '11010010',                             */
/*                           '00',                                   */
/*                           '00',                                   */
/*                           '11',                                   */
/*                           '13' )                                  */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncSetDcbInfo failed with rc=>'rc'<'           */
/*      else say 'RxAsyncSetDcbInfo returned okay.'                  */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncSetDcbInfo(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET   rc;                        /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */
   DCBINFO2 dcbPortInfo;               /* Device control block       */

   if ( numargs != 10 )                /* Must have ten arguments    */
      return( ROUTINE_INVALID );

   if ( !RXVALIDSTRING(args[0]) )      /* Must specify port          */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   rc = GetDcbInfo( hcomPort, &dcbPortInfo );

   if ( rc == NO_ERROR ) {

      if ( RXVALIDSTRING(args[1]) )    /* If specify write timeout   */
         if ( !Str2Num(args[1].strptr, &(dcbPortInfo.usWriteTimeout), STR2NUM_DEC2USHORT) )
            return( ROUTINE_INVALID );

      if ( RXVALIDSTRING(args[2]) )    /* If specify read timeout    */
         if ( !Str2Num(args[2].strptr, &(dcbPortInfo.usReadTimeout), STR2NUM_DEC2USHORT) )
            return( ROUTINE_INVALID );

      if ( RXVALIDSTRING(args[3]) )    /* If specify flags 1         */
         if ( !Str2Num(args[3].strptr, &(dcbPortInfo.fbCtlHndShake), STR2NUM_BIN2BYTE) )
            return( ROUTINE_INVALID );

      if ( RXVALIDSTRING(args[4]) )    /* If specify flags 2         */
         if ( !Str2Num(args[4].strptr, &(dcbPortInfo.fbFlowReplace), STR2NUM_BIN2BYTE) )
            return( ROUTINE_INVALID );

      if ( RXVALIDSTRING(args[5]) )    /* If specify flags 3         */
         if ( !Str2Num(args[5].strptr, &(dcbPortInfo.fbTimeout), STR2NUM_BIN2BYTE) )
            return( ROUTINE_INVALID );

      if ( RXVALIDSTRING(args[6]) )    /* If specify error replace   */
         if ( !Str2Num(args[6].strptr, &(dcbPortInfo.bErrorReplacementChar), STR2NUM_HEX2BYTE) )
            return( ROUTINE_INVALID );

      if ( RXVALIDSTRING(args[7]) )    /* If specify break replace   */
         if ( !Str2Num(args[7].strptr, &(dcbPortInfo.bBreakReplacementChar), STR2NUM_HEX2BYTE) )
            return( ROUTINE_INVALID );

      if ( RXVALIDSTRING(args[8]) )    /* If specify xon character   */
         if ( !Str2Num(args[8].strptr, &(dcbPortInfo.bXONChar), STR2NUM_HEX2BYTE) )
            return( ROUTINE_INVALID );

      if ( RXVALIDSTRING(args[9]) )    /* If specify xoff character  */
         if ( !Str2Num(args[9].strptr, &(dcbPortInfo.bXOFFChar), STR2NUM_HEX2BYTE) )
            return( ROUTINE_INVALID );

      rc = SetDcbInfo( hcomPort, &dcbPortInfo );
   }

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosDevIOCtl result  */

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncGetEnhParm                                      */
/*                                                                   */
/* Purpose:   Return current settings                                */
/*                                                                   */
/*            Utilises IOCTL 74H ASYNC_GETENHPARM.                   */
/*                                                                   */
/* Syntax:    call RxAsyncGetEnhParm handlevar, enhparmvar           */
/*                                                                   */
/* Params:    handlevar  - rexx variable with com handle             */
/*            enhparmvar - rexx variable to store parameter value    */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   EnhParm = '00'                                                  */
/*   rc = RxAsyncGetEnhParm( PortHandle, 'EnhParm' )                 */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncGetEnhParm failed with rc=>'rc'<'           */
/*      else do                                                      */
/*             say 'RxAsyncGetEnhParm returned okay.'                */
/*             if substr( X2B(EnhParm), 1, 1 ) = '1'                 */
/*                then say 'Enhanced mode DMA transmit is ON.'       */
/*             if substr( X2B(EnhParm), 2, 1 ) = '1'                 */
/*                then say 'Enhanced mode DMA receive is ON.'        */
/*             if substr( X2B(EnhParm), 3, 2 ) = '10'                */
/*                then say 'DMA transmit on with dedicated channel.' */
/*             if substr( X2B(EnhParm), 3, 2 ) = '01'                */
/*                then say 'DMA transmit is enabled.'                */
/*             if substr( X2B(EnhParm), 3, 2 ) = '00'                */
/*                then say 'DMA transmit is disabled.'               */
/*             if substr( X2B(EnhParm), 5, 2 ) = '10'                */
/*                then say 'DMA receive on with dedicated channel.'  */
/*             if substr( X2B(EnhParm), 5, 2 ) = '01'                */
/*                then say 'DMA receive is enabled.'                 */
/*             if substr( X2B(EnhParm), 5, 2 ) = '00'                */
/*                then say 'DMA receive is disabled.'                */
/*             if substr( X2B(EnhParm), 7, 1 ) = '1'                 */
/*                then say 'Enhanced mode is enabled.'               */
/*             if substr( X2B(EnhParm), 8, 1 ) = '1'                 */
/*                then say 'Enhanced mode is supported.'             */
/*           end                                                     */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncGetEnhParm(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET   rc;                        /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */
   BYTE     bEnhancedFlags;            /* Enhanced Mode Parameters   */

   if ( numargs != 2 )                 /* Must have two arguments    */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port          */
       || !RXVALIDSTRING(args[1]) )    /* and enh parameters var name*/
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   rc = GetEnhParm( hcomPort, &bEnhancedFlags );

   if ( rc == NO_ERROR ) {
      rc = SetRexxValue( args[1].strptr, VAL2STR_BYTE2HEX, &bEnhancedFlags );
   }

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosDevIOCtl result  */

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncSetEnhParm                                      */
/*                                                                   */
/* Purpose:   Update current settings                                */
/*                                                                   */
/*            Utilises IOCTLs 54H ASYNC_SETENHPARM                   */
/*                        and 74H ASYNC_GETENHPARM.                  */
/*                                                                   */
/* Syntax:    call RxAsyncSetEnhParm handlevar, enhparmvar           */
/*                                                                   */
/* Params:    handlevar  - rexx variable with com handle             */
/*            enhparmvar - rexx variable with parameter settings     */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   EnhParm = '00000010'                                            */
/*   rc = RxAsyncSetEnhParm( PortHandle, EnhParm )                   */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncSetEnhParm failed with rc=>'rc'<'           */
/*      else say 'RxAsyncSetEnhParm returned okay.'                  */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncSetEnhParm(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET   rc;                        /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */
   BYTE     bEnhancedFlags;            /* Enhanced Mode Parameters   */

   if ( numargs != 2 )                 /* Must have two arguments    */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port          */
       || !RXVALIDSTRING(args[1]) )    /* and enh parameters         */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   if ( !Str2Num(args[1].strptr, &bEnhancedFlags, STR2NUM_BIN2BYTE) )
      return( ROUTINE_INVALID );       /* Convert parameters failed  */

   rc = SetEnhParm( hcomPort, bEnhancedFlags );

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosDevIOCtl result  */

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncGetLnCtrl                                       */
/*                                                                   */
/* Purpose:   Get status of specified port                           */
/*                                                                   */
/*            Utilises IOCTLs 62H ASYNC_GETLINECTRL                  */
/*                        and 63H ASYNC_GETEXTDRATE.                 */
/*                                                                   */
/* Syntax:    call RxAsyncGetLnCtrl port                             */
/*                                  'baud' 'data' 'parity' 'stop'    */
/*                                                                   */
/* Params:    port   - handle of open port                           */
/*            baud   - rexx variable to store baudrate in            */
/*            data   - rexx variable to store databits in            */
/*            parity - rexx variable to store parity in              */
/*            stop   - rexx variable to store stopbits in            */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - ERROR_NOT_ENOUGH_MEMORY (8)                   */
/*          "        - ERROR_INVALID_PARAMETER (87)                  */
/*          "        - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   OldBaud    = 0                                                  */
/*   OldData    = 0                                                  */
/*   OldParity  = ''                                                 */
/*   OldStop    = 0                                                  */
/*   rc = RxAsyncGetLnCtrl( PortHandle, 'OldBaud',                   */
/*                          'OldData', 'OldParity', 'OldStop' )      */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncGetLnCtrl failed with rc=>'rc'<'            */
/*      else say 'RxAsyncGetLnCtrl saved the original settings'      */
/*                                                                   */
/*   say 'Baudrate was >'OldBaud'<'                                  */
/*   say 'Databits was >'OldData'<'                                  */
/*   say 'Parity   was >'OldParity'<'                                */
/*   say 'Stopbits was >'OldStop'<'                                  */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncGetLnCtrl(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET       rc;                    /* Function return code       */
   APIRET       rxrc;                  /* RexxVariablePool return    */
   HFILE        hcomPort;              /* Communications port handle */
   EXTDRATE     erateBaudRate;         /* Extended bitrate params    */
   LINECONTROL  lnctlLineControl;      /* Line control parameters    */
   SHVBLOCK     BaudVar;               /* Rexx variable for baudrate */
   SHVBLOCK     DataVar;               /* Rexx variable for databits */
   SHVBLOCK     PariVar;               /* Rexx variable for parity   */
   SHVBLOCK     StopVar;               /* Rexx variable for stopbits */
   PSZ          pszDataBits;
   PSZ          pszStopBits;
   PSZ          pszParity;

   if ( numargs != 5 )                 /* Must have five arguments   */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port handle   */
       || !RXVALIDSTRING(args[1])      /* and baudrate var name      */
       || !RXVALIDSTRING(args[2])      /* and databits var name      */
       || !RXVALIDSTRING(args[3])      /* and parity var name        */
       || !RXVALIDSTRING(args[4]) )    /* and stopbits var name      */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   rc = GetLnCtrl( hcomPort, &erateBaudRate, &lnctlLineControl );

   if ( rc == NO_ERROR ) {

      switch( lnctlLineControl.bDataBits )
      {
         case DATA_BITS_5:
              pszDataBits = DATA_BITS_5_STR;
              break;
         case DATA_BITS_6:
              pszDataBits = DATA_BITS_6_STR;
              break;
         case DATA_BITS_7:
              pszDataBits = DATA_BITS_7_STR;
              break;
         case DATA_BITS_8:
              pszDataBits = DATA_BITS_8_STR;
              break;
         default:
              pszDataBits = DATA_BITS_UNKNOWN_STR;
              break;
      }

      switch( lnctlLineControl.bParity )
      {
         case PARITY_NONE:
              pszParity = PARITY_NONE_STR;
              break;
         case PARITY_ODD:
              pszParity = PARITY_ODD_STR;
              break;
         case PARITY_EVEN:
              pszParity = PARITY_EVEN_STR;
              break;
         case PARITY_MARK:
              pszParity = PARITY_MARK_STR;
              break;
         case PARITY_SPACE:
              pszParity = PARITY_SPACE_STR;
              break;
         default:
              pszParity = PARITY_UNKNOWN_STR;
              break;
      }

      switch( lnctlLineControl.bStopBits )
      {
         case STOP_BITS_1:
              pszStopBits = STOP_BITS_1_STR;
              break;
         case STOP_BITS_15:
              pszStopBits = STOP_BITS_15_STR;
              break;
         case STOP_BITS_2:
              pszStopBits = STOP_BITS_2_STR;
              break;
         default:
              pszStopBits = STOP_BITS_UNKNOWN_STR;
              break;
      }

      /* fetch rexx variables details (rexx mallocs, we free) */

      InitVar( &BaudVar, &DataVar, args[1].strptr );
      InitVar( &DataVar, &PariVar, args[2].strptr );
      InitVar( &PariVar, &StopVar, args[3].strptr );
      InitVar( &StopVar, NULL,     args[4].strptr );

      rc = FetchVar( &BaudVar );       /* GET the rexx variables     */

      if ( rc == NO_ERROR ) {

         /* set rexx variable details with retrieved values */

         EditVar( &BaudVar, VAL2STR_ULONG2DEC, &(erateBaudRate.ulCurrentBaudRate) );
         EditVar( &DataVar, VAL2STR_STR2STR,   pszDataBits );
         EditVar( &PariVar, VAL2STR_STR2STR,   pszParity );
         EditVar( &StopVar, VAL2STR_STR2STR,   pszStopBits );

         rc = UpdateVar( &BaudVar );   /* SET the rexx variables     */
      }

      FreeVar( &BaudVar );             /* Cleanup                    */
   }

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosDevIOCtl result  */

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncSetLnCtrl                                       */
/*                                                                   */
/* Purpose:   Set status of specified port                           */
/*                                                                   */
/*            Utilises IOCTLs 62H ASYNC_GETLINECTRL                  */
/*                        and 63H ASYNC_GETEXTDRATE                  */
/*                        and 42H ASYNC_SETLINECTRL                  */
/*                        and 43H ASYNC_SETEXTDRATE.                 */
/*                                                                   */
/* Syntax:    call RxAsyncSetLnCtrl port 19200 8 'N' 1               */
/*                                                                   */
/* Params:    port   - handle of open port                           */
/*            baud   - baudrate to set                               */
/*            data   - data bits to set                              */
/*            parity - parity to set                                 */
/*            stop   - stop bits to set                              */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - ERROR_INVALID_PARAMETER (87)                  */
/*          "        - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   NewBaud    = 19200                                              */
/*   NewData    = 8                                                  */
/*   NewParity  = 'N'                                                */
/*   NewStop    = 1                                                  */
/*   rc = RxAsyncSetLnCtrl( PortHandle, NewBaud,                     */
/*                          NewData, NewParity, NewStop )            */
/*   if rc <> 0                                                      */
/*     then                                                          */
/*       say 'RxAsyncSetLnCtrl failed with rc=>'rc'<'                */
/*     else                                                          */
/*       say 'RxAsyncSetLnCtrl has set the new settings'             */
/*                                                                   */
/*   say 'Baudrate now >'NewBaud'<'                                  */
/*   say 'Databits now >'NewData'<'                                  */
/*   say 'Parity   now >'NewParity'<'                                */
/*   say 'Stopbits now >'NewStop'<'                                  */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncSetLnCtrl(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET       rc;                    /* Function return code       */
   HFILE        hcomPort;              /* Communications port handle */
   EXTDRATE     erateBaudRate;         /* Extended bitrate params    */
   LINECONTROL  lnctlLineControl;      /* Line control parameters    */

   if ( numargs != 5 )                 /* Must have five arguments   */
      return( ROUTINE_INVALID );

   if ( !RXVALIDSTRING(args[0]) )      /* Must specify port handle   */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   rc = GetLnCtrl( hcomPort, &erateBaudRate, &lnctlLineControl );

   if ( rc == NO_ERROR ) {

      if ( RXVALIDSTRING(args[1]) ) /* If specify baud rate       */
      {
         if ( !Str2Num(args[1].strptr, &(erateBaudRate.ulCurrentBaudRate), STR2NUM_DEC2ULONG) )
            return( ROUTINE_INVALID );       /* Convert baudrate failed    */

         erateBaudRate.bCurrentFraction  = (BYTE)0;
      }

      if ( RXVALIDSTRING(args[2]) ) /* If specify data bits       */
      {
         if        ( strcmp(args[2].strptr,  DATA_BITS_5_STR) == 0 ) {
            lnctlLineControl.bDataBits  = DATA_BITS_5;
         } else if ( strcmp(args[2].strptr,  DATA_BITS_6_STR) == 0 ) {
            lnctlLineControl.bDataBits  = DATA_BITS_6;
         } else if ( strcmp(args[2].strptr,  DATA_BITS_7_STR) == 0 ) {
            lnctlLineControl.bDataBits  = DATA_BITS_7;
         } else if ( strcmp(args[2].strptr,  DATA_BITS_8_STR) == 0 ) {
            lnctlLineControl.bDataBits  = DATA_BITS_8;
         } else {
            rc = ERROR_INVALID_PARAMETER;
         }
      }

      if ( RXVALIDSTRING(args[3]) ) /* If specify parity          */
      {
         if        ( stricmp(args[3].strptr, PARITY_NONE_STR ) == 0 ) {
            lnctlLineControl.bParity    = PARITY_NONE;
         } else if ( stricmp(args[3].strptr, PARITY_ODD_STR  ) == 0 ) {
            lnctlLineControl.bParity    = PARITY_ODD;
         } else if ( stricmp(args[3].strptr, PARITY_EVEN_STR ) == 0 ) {
            lnctlLineControl.bParity    = PARITY_EVEN;
         } else if ( stricmp(args[3].strptr, PARITY_MARK_STR ) == 0 ) {
            lnctlLineControl.bParity    = PARITY_MARK;
         } else if ( stricmp(args[3].strptr, PARITY_SPACE_STR) == 0 ) {
            lnctlLineControl.bParity    = PARITY_SPACE;
         } else {
            rc = ERROR_INVALID_PARAMETER;
         }
      }

      if ( RXVALIDSTRING(args[4]) ) /* If specify stop bits       */
      {
         if        ( strcmp(args[4].strptr,  STOP_BITS_1_STR ) == 0 ) {
            lnctlLineControl.bStopBits  = STOP_BITS_1;
         } else if ( strcmp(args[4].strptr,  STOP_BITS_15_STR) == 0 ) {
            lnctlLineControl.bStopBits  = STOP_BITS_15;
         } else if ( strcmp(args[4].strptr,  STOP_BITS_2_STR ) == 0 ) {
            lnctlLineControl.bStopBits  = STOP_BITS_2;
         } else {
            rc = ERROR_INVALID_PARAMETER;
         }
      }

      if ( rc == NO_ERROR ) {

         rc = SetLnCtrl( hcomPort, &erateBaudRate, &lnctlLineControl );

      }
   }

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosDevIOCtl result  */

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncSetBreakOff                                     */
/*                                                                   */
/* Purpose:   Set break off                                          */
/*                                                                   */
/*            Utilises IOCTL 45H ASYNC_SETBREAKOFF.                  */
/*                                                                   */
/* Syntax:    call RxAsyncSetBreakOff handlevar                      */
/*                                                                   */
/* Params:    handlevar - rexx variable with com handle              */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   rc = RxAsyncSetBreakOff( PortHandle )                           */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncSetBreakOff failed with rc=>'rc'<'          */
/*      else say 'RxAsyncSetBreakOff returned okay'                  */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncSetBreakOff(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET    rc;                       /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */

   if ( numargs != 1 )                 /* Must have one argument     */
      return( ROUTINE_INVALID );

   if ( !RXVALIDSTRING(args[0]) )      /* Must specify port handle   */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   rc = SetBreakOff( hcomPort );

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncSetBreakOn                                      */
/*                                                                   */
/* Purpose:   Set break on                                           */
/*                                                                   */
/*            Utilises IOCTL 4BH ASYNC_SETBREAKON.                   */
/*                                                                   */
/* Syntax:    call RxAsyncSetBreakOn handlevar                       */
/*                                                                   */
/* Params:    handlevar - rexx variable with com handle              */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   rc = RxAsyncSetBreakOn( PortHandle )                            */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncSetBreakOn failed with rc=>'rc'<'           */
/*      else say 'RxAsyncSetBreakOn returned okay'                   */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncSetBreakOn(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET    rc;                       /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */

   if ( numargs != 1 )                 /* Must have one argument     */
      return( ROUTINE_INVALID );

   if ( !RXVALIDSTRING(args[0]) )      /* Must specify port handle   */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   rc = SetBreakOn( hcomPort );

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncTxImmediate                                     */
/*                                                                   */
/* Purpose:   Transmit specified byte immediately                    */
/*                                                                   */
/*            Utilises IOCTL 44H ASYNC_TRANSMITIMM.                  */
/*                                                                   */
/* Syntax:    call RxAsyncTxImmediate handlevar byte                 */
/*                                                                   */
/* Params:    handlevar - rexx variable with com handle              */
/*            byte      - byte to be transmitted                     */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   escape_char = '1B'                                              */
/*   rc = RxAsyncTxImmediate( PortHandle, escape_char )              */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncTxImmediate failed with rc=>'rc'<'          */
/*      else say 'RxAsyncTxImmediate returned okay'                  */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncTxImmediate(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET    rc;                       /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */
   BYTE     bAnyByte;                  /* Byte to be sent            */

   if ( numargs != 2 )                 /* Must have two arguments    */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port handle   */
       || !RXVALIDSTRING(args[1]) )    /* and byte to send           */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   if ( !Str2Num(args[1].strptr, &bAnyByte, STR2NUM_HEX2BYTE) )
      return( ROUTINE_INVALID );       /* Convert byte failed        */

   rc = TxImmediate( hcomPort, bAnyByte );

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncTxStop                                          */
/*                                                                   */
/* Purpose:   Stop transmission (XOFF)                               */
/*                                                                   */
/*            Utilises IOCTL 47H ASYNC_STOPTRANSMIT.                 */
/*                                                                   */
/* Syntax:    call RxAsyncTxStop handlevar                           */
/*                                                                   */
/* Params:    handlevar  - rexx variable with com handle             */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   rc = RxAsyncTxStop( PortHandle )                                */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncTxStop failed with rc=>'rc'<'               */
/*      else say 'RxAsyncTxStop returned okay.'                      */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncTxStop(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET   rc;                        /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */

   if ( numargs != 1 )                 /* Must have one argument     */
      return( ROUTINE_INVALID );

   if ( !RXVALIDSTRING(args[0]) )      /* Must specify port          */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   rc = TxStop( hcomPort );

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncTxStart                                         */
/*                                                                   */
/* Purpose:   Start transmission (XON)                               */
/*                                                                   */
/*            Utilises IOCTL 48H ASYNC_STARTTRANSMIT.                */
/*                                                                   */
/* Syntax:    call RxAsyncTxStart handlevar                          */
/*                                                                   */
/* Params:    handlevar  - rexx variable with com handle             */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   rc = RxAsyncTxStart( PortHandle )                               */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncTxStart failed with rc=>'rc'<'              */
/*      else say 'RxAsyncTxStart returned okay.'                     */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncTxStart(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET    rc;                       /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */

   if ( numargs != 1 )                 /* Must have one argument     */
      return( ROUTINE_INVALID );

   if ( !RXVALIDSTRING(args[0]) )      /* Must specify port          */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   rc = TxStart( hcomPort );

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncGetComStatus                                    */
/*                                                                   */
/* Purpose:   Return current COM status                              */
/*                                                                   */
/*            Utilises IOCTL 64H ASYNC_GETCOMMSTATUS.                */
/*                                                                   */
/* Syntax:    call RxAsyncGetComStatus handlevar, statusvar          */
/*                                                                   */
/* Params:    handlevar  - rexx variable with com handle             */
/*            statusvar  - rexx variable to store status value       */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   ComStatus = '00'                                                */
/*   rc = RxAsyncGetComStatus( PortHandle, 'ComStatus' )             */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncGetComStatus failed with rc=>'rc'<'         */
/*      else do                                                      */
/*             say 'RxAsyncGetComStatus returned okay.'              */
/*             if substr( X2B(ComStatus), 1, 1 ) = '1'               */
/*                then say 'Receive waiting for DSR.'                */
/*             if substr( X2B(ComStatus), 2, 1 ) = '1'               */
/*                then say 'Transmit waiting to send immediate.'     */
/*             if substr( X2B(ComStatus), 3, 1 ) = '1'               */
/*                then say 'Transmit waiting while break on.'        */
/*             if substr( X2B(ComStatus), 4, 1 ) = '1'               */
/*                then say 'Transmit waiting to send XON.'           */
/*             if substr( X2B(ComStatus), 5, 1 ) = '1'               */
/*                then say 'Transmit waiting for XON.'               */
/*             if substr( X2B(ComStatus), 6, 1 ) = '1'               */
/*                then say 'Transmit waiting for DCD.'               */
/*             if substr( X2B(ComStatus), 7, 1 ) = '1'               */
/*                then say 'Transmit waiting for DSR.'               */
/*             if substr( X2B(ComStatus), 8, 1 ) = '1'               */
/*                then say 'Transmit waiting for CTS.'               */
/*           end                                                     */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncGetComStatus(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET   rc;                        /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */
   BYTE     bComStatus;                /* COM status returned        */

   if ( numargs != 2 )                 /* Must have two arguments    */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port          */
       || !RXVALIDSTRING(args[1]) )    /* and COM status var name    */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   rc = GetComStatus( hcomPort, &bComStatus );

   if ( rc == NO_ERROR ) {
      rc = SetRexxValue( args[1].strptr, VAL2STR_BYTE2HEX, &(bComStatus) );
   }

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosDevIOCtl result  */

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncGetTxStatus                                     */
/*                                                                   */
/* Purpose:   Return current Transmit status                         */
/*                                                                   */
/*            Utilises IOCTL 65H ASYNC_GETLINESTATUS.                */
/*                                                                   */
/* Syntax:    call RxAsyncGetTxStatus  handlevar, statusvar          */
/*                                                                   */
/* Params:    handlevar  - rexx variable with com handle             */
/*            statusvar  - rexx variable to store status value       */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   TxStatus = '00'                                                 */
/*   rc = RxAsyncGetTxStatus( PortHandle, 'TxStatus' )               */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncGetTxStatus failed with rc=>'rc'<'          */
/*      else do                                                      */
/*             say 'RxAsyncGetTxStatus returned okay.'               */
/*             if substr( X2B(TxStatus), 3, 1 ) = '1'                */
/*                then say 'Waiting to send XOFF.'                   */
/*             if substr( X2B(TxStatus), 4, 1 ) = '1'                */
/*                then say 'Waiting to send XON.'                    */
/*             if substr( X2B(TxStatus), 5, 1 ) = '1'                */
/*                then say 'Char ready to send immediate.'           */
/*             if substr( X2B(TxStatus), 6, 1 ) = '1'                */
/*                then say 'Hardware is transmitting.'               */
/*             if substr( X2B(TxStatus), 7, 1 ) = '1'                */
/*                then say 'Data in transmit queue.'                 */
/*             if substr( X2B(TxStatus), 8, 1 ) = '1'                */
/*                then say 'Write request queued.'                   */
/*           end                                                     */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncGetTxStatus(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET   rc;                        /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */
   BYTE     bTxStatus;                 /* Tx status returned         */

   if ( numargs != 2 )                 /* Must have two arguments    */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port          */
       || !RXVALIDSTRING(args[1]) )    /* and tx status var name     */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   rc = GetTxStatus( hcomPort, &bTxStatus );

   if ( rc == NO_ERROR ) {
      rc = SetRexxValue( args[1].strptr, VAL2STR_BYTE2HEX, &(bTxStatus) );
   }

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosDevIOCtl result  */

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncGetMdmOutput                                    */
/*                                                                   */
/* Purpose:   Return current Modem Output signals                    */
/*                                                                   */
/*            Utilises IOCTL 66H ASYNC_GETMODEMOUTPUT.               */
/*                                                                   */
/* Syntax:    call RxAsyncGetMdmOutput handlevar, signalvar          */
/*                                                                   */
/* Params:    handlevar  - rexx variable with com handle             */
/*            signalvar  - rexx variable to store signals value      */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   MdmOutput = '00'                                                */
/*   rc = RxAsyncGetMdmOutput( PortHandle, 'MdmOutput' )             */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncGetMdmOutput failed with rc=>'rc'<'         */
/*      else do                                                      */
/*             say 'RxAsyncGetMdmOutput returned okay.'              */
/*             if substr( X2B(MdmOutput), 7, 1 ) = '1'               */
/*                then say 'RTS signal is ON.'                       */
/*             if substr( X2B(MdmOutput), 8, 1 ) = '1'               */
/*                then say 'DTR signal is ON.'                       */
/*           end                                                     */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncGetMdmOutput(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET   rc;                        /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */
   BYTE     bMdmOutput;                /* Modem Output returned      */

   if ( numargs != 2 )                 /* Must have two arguments    */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port          */
       || !RXVALIDSTRING(args[1]) )    /* and modem output var name  */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   rc = GetModemOutput( hcomPort, &bMdmOutput );

   if ( rc == NO_ERROR ) {
      rc = SetRexxValue( args[1].strptr, VAL2STR_BYTE2HEX, &(bMdmOutput) );
   }

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosDevIOCtl result  */

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncSetMdmOutput                                    */
/*                                                                   */
/* Purpose:   Alters current Modem Output signals                    */
/*                                                                   */
/*            Utilises IOCTL 46H ASYNC_SETMODEMCTRL.                 */
/*                                                                   */
/* Syntax:    call RxAsyncSetMdmOutput handlevar, onmask, offmask    */
/*                                                                   */
/* Params:    handlevar  - rexx variable with com handle             */
/*            onmask     - on mask setting                           */
/*            offmask    - off mask setting                          */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   none_on  = '00'                                                 */
/*   dtr_on   = '01'                                                 */
/*   rts_on   = '02'                                                 */
/*   both_on  = '03'                                                 */
/*   none_off = 'FF'                                                 */
/*   dtr_off  = 'FE'                                                 */
/*   rts_off  = 'FD'                                                 */
/*   both_off = 'FC'                                                 */
/*   rc = RxAsyncSetMdmOutput( PortHandle, none_on, dtr_off )        */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncSetMdmOutput failed with rc=>'rc'<'         */
/*      else say 'RxAsyncSetMdmOutput returned okay.'                */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncSetMdmOutput(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET    rc;                       /* Function return code       */
   HFILE     hcomPort;                 /* Communications port handle */
   BYTE      bModemOn;                 /* Modem control ON mask      */
   BYTE      bModemOff;                /* Modem control OFF mask     */

   if ( numargs != 3 )                 /* Must have three arguments  */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port          */
       || !RXVALIDSTRING(args[1])      /* and signal on mask         */
       || !RXVALIDSTRING(args[2]) )    /* and signal off mask        */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   if ( !Str2Num(args[1].strptr, &bModemOn, STR2NUM_HEX2BYTE) )
      return( ROUTINE_INVALID );       /* Convert ON mask failed     */

   if ( !Str2Num(args[2].strptr, &bModemOff, STR2NUM_HEX2BYTE) )
      return( ROUTINE_INVALID );       /* Convert OFF mask failed    */

   rc = SetMdmOutput( hcomPort, bModemOn, bModemOff );

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncGetMdmInput                                     */
/*                                                                   */
/* Purpose:   Return current Modem Input signals                     */
/*                                                                   */
/*            Utilises IOCTL 67H ASYNC_GETMODEMINPUT.                */
/*                                                                   */
/* Syntax:    call RxAsyncGetMdmInput handlevar, signalvar           */
/*                                                                   */
/* Params:    handlevar  - rexx variable with com handle             */
/*            signalvar  - rexx variable to store signals value      */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   MdmInput = '00'                                                 */
/*   rc = RxAsyncGetMdmInput( PortHandle, 'MdmInput' )               */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncGetMdmInput failed with rc=>'rc'<'          */
/*      else do                                                      */
/*             say 'RxAsyncGetMdmInput returned okay.'               */
/*             if substr( X2B(MdmInput), 1, 1 ) = '1'                */
/*                then say 'DCD signal is ON.'                       */
/*             if substr( X2B(MdmInput), 2, 1 ) = '1'                */
/*                then say 'RI signal is ON.'                        */
/*             if substr( X2B(MdmInput), 3, 1 ) = '1'                */
/*                then say 'DSR signal is ON.'                       */
/*             if substr( X2B(MdmInput), 4, 1 ) = '1'                */
/*                then say 'CTS signal is ON.'                       */
/*           end                                                     */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncGetMdmInput(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET   rc;                        /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */
   BYTE     bMdmInput;                 /* Modem Input returned       */

   if ( numargs != 2 )                 /* Must have two arguments    */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port          */
       || !RXVALIDSTRING(args[1]) )    /* and modem input var name   */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   rc = GetModemInput( hcomPort, &bMdmInput );

   if ( rc == NO_ERROR ) {
      rc = SetRexxValue( args[1].strptr, VAL2STR_BYTE2HEX, &(bMdmInput) );
   }

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosDevIOCtl result  */

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncGetRxCount                                      */
/*                                                                   */
/* Purpose:   Return current RX queue size and count                 */
/*                                                                   */
/*            Utilises IOCTL 68H ASYNC_GETINQUECOUNT.                */
/*                                                                   */
/* Syntax:    call RxAsyncGetRxCount  handlevar, countvar, sizevar   */
/*                                                                   */
/* Params:    handlevar  - rexx variable with com handle             */
/*            countvar   - rexx variable to store count value        */
/*            sizevar    - rexx variable to store size  value        */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   RxCount = 0                                                     */
/*   RxSize  = 0                                                     */
/*   rc = RxAsyncGetRxCount( PortHandle, 'RxCount', 'RxSize' )       */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncGetRxCount failed with rc=>'rc'<'           */
/*      else do                                                      */
/*             say 'RxAsyncGetRxCount returned okay.'                */
/*             say 'There are 'RxCount' bytes in the RX queue.'      */
/*             say 'The RX queue is 'RxSize' bytes long.'            */
/*           end                                                     */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncGetRxCount(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET   rc;                        /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */
   USHORT   usRxCount;                 /* Number bytes in RX Queue   */
   USHORT   usRxSize;                  /* Size of RX Queue           */

   if ( numargs != 3 )                 /* Must have three arguments  */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port          */
       || !RXVALIDSTRING(args[1])      /* and queue count var name   */
       || !RXVALIDSTRING(args[2]) )    /* and queue size var name    */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   rc = GetRxCount( hcomPort, &usRxCount, &usRxSize );

   if ( rc == NO_ERROR ) {
      rc = SetRexxValue( args[1].strptr, VAL2STR_USHORT2DEC, &usRxCount );

      if ( rc == NO_ERROR ) {
         rc = SetRexxValue( args[2].strptr, VAL2STR_USHORT2DEC, &usRxSize );
      }
   }

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosDevIOCtl result  */

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncGetTxCount                                      */
/*                                                                   */
/* Purpose:   Return current TX queue size and count                 */
/*                                                                   */
/*            Utilises IOCTL 69H ASYNC_GETOUTQUECOUNT.               */
/*                                                                   */
/* Syntax:    call RxAsyncGetTxCount  handlevar, countvar, sizevar   */
/*                                                                   */
/* Params:    handlevar  - rexx variable with com handle             */
/*            countvar   - rexx variable to store count value        */
/*            sizevar    - rexx variable to store size  value        */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   TxCount = 0                                                     */
/*   TxSize  = 0                                                     */
/*   rc = RxAsyncGetTxCount( PortHandle, 'TxCount', 'TxSize' )       */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncGetTxCount failed with rc=>'rc'<'           */
/*      else do                                                      */
/*             say 'RxAsyncGetTxCount returned okay.'                */
/*             say 'There are 'TxCount' bytes in the TX queue.'      */
/*             say 'The TX queue is 'TxSize' bytes long.'            */
/*           end                                                     */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncGetTxCount(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET   rc;                        /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */
   USHORT   usTxCount;                 /* Number bytes in TX Queue   */
   USHORT   usTxSize;                  /* Size of TX Queue           */

   if ( numargs != 3 )                 /* Must have three arguments  */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port          */
       || !RXVALIDSTRING(args[1])      /* and queue count var name   */
       || !RXVALIDSTRING(args[2]) )    /* and queue size var name    */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   rc = GetTxCount( hcomPort, &usTxCount, &usTxSize );

   if ( rc == NO_ERROR ) {
      rc = SetRexxValue( args[1].strptr, VAL2STR_USHORT2DEC, &usTxCount );

      if ( rc == NO_ERROR ) {
         rc = SetRexxValue( args[2].strptr, VAL2STR_USHORT2DEC, &usTxSize );
      }
   }

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosDevIOCtl result  */

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncGetComError                                     */
/*                                                                   */
/* Purpose:   Return current COM Error setting                       */
/*                                                                   */
/*            Utilises IOCTL 6DH ASYNC_GETCOMMERROR.                 */
/*                                                                   */
/* Syntax:    call RxAsyncGetComError handlevar, errorvar            */
/*                                                                   */
/* Params:    handlevar  - rexx variable with com handle             */
/*            errorvar   - rexx variable to store error value        */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   ComError = '0000'                                               */
/*   rc = RxAsyncGetComError( PortHandle, 'ComError' )               */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncGetComError failed with rc=>'rc'<'          */
/*      else do                                                      */
/*             say 'RxAsyncGetComError returned okay.'               */
/*             if substr( X2B(ComError), 13, 1 ) = '1'               */
/*                then say 'Framing Error detected.'                 */
/*             if substr( X2B(ComError), 14, 1 ) = '1'               */
/*                then say 'Parity Error detected.'                  */
/*             if substr( X2B(ComError), 15, 1 ) = '1'               */
/*                then say 'Receive Hardware Overrun detected.'      */
/*             if substr( X2B(ComError), 16, 1 ) = '1'               */
/*                then say 'Receive Queue Overrun detected.'         */
/*           end                                                     */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncGetComError(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET   rc;                        /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */
   USHORT   usComError;                /* COM Error returned         */

   if ( numargs != 2 )                 /* Must have two arguments    */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port          */
       || !RXVALIDSTRING(args[1]) )    /* and com error var name     */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   rc = GetComError( hcomPort, &usComError );

   if ( rc == NO_ERROR ) {
      rc = SetRexxValue( args[1].strptr, VAL2STR_USHORT2HEX, &(usComError) );
   }

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosDevIOCtl result  */

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncGetComEvent                                     */
/*                                                                   */
/* Purpose:   Return current COM Event setting                       */
/*                                                                   */
/*            Utilises IOCTL 72H ASYNC_GETCOMMEVENT.                 */
/*                                                                   */
/* Syntax:    call RxAsyncGetComEvent handlevar, eventvar            */
/*                                                                   */
/* Params:    handlevar  - rexx variable with com handle             */
/*            eventvar   - rexx variable to store event value        */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   ComEvent = '0000'                                               */
/*   rc = RxAsyncGetComEvent( PortHandle, 'ComEvent' )               */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncGetComEvent failed with rc=>'rc'<'          */
/*      else do                                                      */
/*             say 'RxAsyncGetComEvent returned okay.'               */
/*             if substr( X2B(ComEvent),  8, 1 ) = '1'               */
/*                then say 'Ring Indicator detected.'                */
/*             if substr( X2B(ComEvent),  9, 1 ) = '1'               */
/*                then say 'Error occurred.'                         */
/*             if substr( X2B(ComEvent), 10, 1 ) = '1'               */
/*                then say 'Break detected.'                         */
/*             if substr( X2B(ComEvent), 11, 1 ) = '1'               */
/*                then say 'DCD changed state.'                      */
/*             if substr( X2B(ComEvent), 12, 1 ) = '1'               */
/*                then say 'DSR changed state.'                      */
/*             if substr( X2B(ComEvent), 13, 1 ) = '1'               */
/*                then say 'CTS changed state.'                      */
/*             if substr( X2B(ComEvent), 14, 1 ) = '1'               */
/*                then say 'Last character sent.'                    */
/*             if substr( X2B(ComEvent), 15, 1 ) = '1'               */
/*                then say 'Receive timeout interrupt.'              */
/*             if substr( X2B(ComEvent), 16, 1 ) = '1'               */
/*                then say 'Character received.'                     */
/*           end                                                     */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncGetComEvent(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET   rc;                        /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */
   USHORT   usComEvent;                /* COM Event returned         */

   if ( numargs != 2 )                 /* Must have two arguments    */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port          */
       || !RXVALIDSTRING(args[1]) )    /* and com event var name     */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   rc = GetComEvent( hcomPort, &usComEvent );

   if ( rc == NO_ERROR ) {
      rc = SetRexxValue( args[1].strptr, VAL2STR_USHORT2HEX, &(usComEvent) );
   }

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosDevIOCtl result  */

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncOpen                                            */
/*                                                                   */
/* Purpose:   Open the specified port                                */
/*                                                                   */
/* Syntax:    call RxAsyncOpen name 'handlevar'                      */
/*                                                                   */
/* Params:    name      - name of com port or named pipe             */
/*            handlevar - rexx variable to store handle in           */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosOpen                               */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   PortName   = 'COM4'                                             */
/*   PortHandle = ''                                                 */
/*   rc = RxAsyncOpen( PortName, 'PortHandle' )                      */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncOpen failed with rc=>'rc'<'                 */
/*      else say 'RxAsyncOpen opened device handle=>'PortHandle'<'   */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncOpen(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET   rc;                        /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */

   if ( numargs != 2 )                 /* Must have two arguments    */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port name     */
       || !RXVALIDSTRING(args[1]) )    /* and handle var name        */
      return( ROUTINE_INVALID );

   rc = OpenPort( args[0].strptr, &hcomPort );
   if ( rc == NO_ERROR ) {             /* Port opened okay           */

      rc = SetRexxValue( args[1].strptr, VAL2STR_HFILE2DEC, &hcomPort );
      if ( rc != NO_ERROR ) {          /* Couldn't continue          */

         ClosePort( &hcomPort );
      }
   }

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosOpen result      */

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncClose                                           */
/*                                                                   */
/* Purpose:   Close the specified port                               */
/*                                                                   */
/* Syntax:    call RxAsyncClose handlevar                            */
/*                                                                   */
/* Params:    handlevar - rexx variable with com handle              */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosClose                              */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   rc = RxAsyncClose( PortHandle )                                 */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncClose failed with rc=>'rc'<'                */
/*      else say 'RxAsyncClose closed the device.'                   */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncClose(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET   rc;                        /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */

   if ( numargs != 1 )                 /* Must have one argument     */
      return( ROUTINE_INVALID );

   if ( !RXVALIDSTRING(args[0]) )      /* Must specify port          */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   rc = ClosePort( &hcomPort );

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosClose result     */

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncWait                                            */
/*                                                                   */
/* Purpose:   Wait for input from the specified port                 */
/*                                                                   */
/*            Utilises IOCTL 72H ASYNC_GETCOMMEVENT.                 */
/*                                                                   */
/* Syntax:    call RxAsyncWait port timeout                          */
/*                                                                   */
/* Params:    port    - handle of open port                          */
/*            timeout - maximum period to wait in milliseconds       */
/*                                                                   */
/*                      If -1 is specified then the routine waits    */
/*                      forever.                                     */
/*                                                                   */
/*                      If 0 is specified then the routine will      */
/*                      return 0 if data was detected and -3 if no   */
/*                      data was detected.                           */
/*                                                                   */
/* Return:                                                           */
/*   Data detected    - NO_ERROR (0)                                 */
/*   Wait timed out   - RC_ASYNC_TIMED_OUT (-1)                      */
/*   No data detected - RC_ASYNC_NO_INPUT  (-3)                      */
/*   Error occurred   - rc from DosDevIOCtl                          */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   say 'Wait for data 'time()                                      */
/*   rc = RxAsyncWait( PortHandle, 3000 )                            */
/*   say 'Waited enough 'time()                                      */
/*   if rc = 0                                                       */
/*      then say 'RxAsyncWait detected input.'                       */
/*      else if rc = -1                                              */
/*              then say 'RxAsyncWait timed out.'                    */
/*              else say 'RxAsyncWait failed with rc=>'rc'<'         */
/*                                                                   */
/* Note: RC_ASYNC_NO_INPUT is only returned if the timeout specified */
/*       is 0 (zero) and there is no data detected.                  */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncWait(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET       rc;                    /* Function return code       */
   HFILE        hcomPort;              /* Communications port handle */
   LONG         lTimeLimit;            /* Time in hundredths         */

   if ( numargs != 2 )                 /* Must have two arguments    */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port handle   */
       || !RXVALIDSTRING(args[1]) )    /* and timeout                */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   if ( !Str2Num(args[1].strptr, &lTimeLimit, STR2NUM_DEC2LONG) )
      return( ROUTINE_INVALID );       /* Convert timeout failed     */

   rc = WaitInput( hcomPort, lTimeLimit );

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosDevIOCtl result  */

   return( ROUTINE_VALID );            /* All ok                     */
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncRead                                            */
/*                                                                   */
/* Purpose:   Read from the specified port                           */
/*                                                                   */
/*            Utilises IOCTL 72H ASYNC_GETCOMMEVENT.                 */
/*                                                                   */
/* Syntax:    rc = RxAsyncRead( port, length, timeout, 'varname' )   */
/*                                                                   */
/*      OR    rc = RxAsyncRead( port,                                */
/*                              length,                              */
/*                              timeout,                             */
/*                              'varname',                           */
/*                              nullrep )                            */
/*                                                                   */
/*      OR    rc = RxAsyncRead( port,                                */
/*                              length,                              */
/*                              timeout,                             */
/*                              'varname',                           */
/*                              nullrep,                             */
/*                              lineend )                            */
/*                                                                   */
/* Params:    port    - handle of open port                          */
/*                                                                   */
/*            length  - number of bytes to read, 0=a line, n=chars   */
/*                      Note: A line constitutes everything upto     */
/*                      and including a terminating linefeed char    */
/*                      if it can be read in the time specified.     */
/*                      A maximum line size of 255 bytes is read,    */
/*                      if this limit is reached then the portion    */
/*                      read upto this point will be returned.       */
/*                      If the read times out then whatever was      */
/*                      read upto that point is returned.            */
/*                                                                   */
/*            timeout - maximum period to wait in milliseconds       */
/*                      -1 = no timeout                              */
/*                                                                   */
/*            varname - name of rexx variable to return data in      */
/*                                                                   */
/*            nullrep - optional fifth parameter which when          */
/*                      specified permits null charatacers in the    */
/*                      input stream to be either discarded ('')     */
/*                      or replaced ('?'). An empty string tells     */
/*                      the function to discard nulls, otherwise     */
/*                      all nulls are replaced with the character    */
/*                      specified. If this parameter is not          */
/*                      specified then all nulls will be returned    */
/*                      in the string passed back to the caller.     */
/*                                                                   */
/*            lineend - optional sixth parameter which when          */
/*                      specified indicates an additional character  */
/*                      to be used in detecting end_of_line.         */
/*                      The defaults if not specified are line_feed, */
/*                      form_feed or end_of_file. When specified     */
/*                      the character acts in addition to the        */
/*                      defaults.                                    */
/*                                                                   */
/* Return:                                                           */
/*   Call successful  - NO_ERROR (0)                                 */
/*   Read timed out   - RC_ASYNC_TIMED_OUT (-1)                      */
/*   Call failed      - ERROR_NOT_ENOUGH_MEMORY (8)                  */
/*          "         - ERROR_INVALID_PARAMETER (87)                 */
/*          "         - rc from DosRead                              */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   InpStr = ''                                                     */
/*   rc =  RxAsyncRead( PortHandle, 0, 3000, 'InpStr' )              */
/*   if rc = 0                                                       */
/*      then say 'RxAsyncRead returned >'InpStr'<'                   */
/*      else if rc = -1                                              */
/*              then say 'RxAsyncRead timed out, returned >'InpStr'<'*/
/*              else say 'RxAsyncRead failed with rc=>'rc'<'         */
/*                                                                   */
/*   -- to replace nulls with question marks use                     */
/*                                                                   */
/*   rc =  RxAsyncRead( PortHandle, 0, 3000, 'InpStr', '?' )         */
/*                                                                   */
/*   -- to use <CR> as line termination and discard nulls use        */
/*                                                                   */
/*   cret = d2c(13)                                                  */
/*   rc =  RxAsyncRead( PortHandle, 0, 3000, 'InpStr', '', cret )    */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncRead(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET       rc;                    /* Function return code       */
   APIRET       rc2;                   /* RexxVariablePool return    */
   HFILE        hcomPort;              /* Communications port handle */
   BYTE         ReadBuff[RXAUTOBUFLEN];/* Input buffer               */
   ULONG        ulNumRequested;        /* Read length (0=line mode)  */
   SHVBLOCK     ReadVar;               /* Rexx variable for input    */
   LONG         lTimeLimit;            /* Timeout in milliseconds    */
   PSZ          pszNullReplace;        /* Null replace character     */
   PSZ          pszLineEnd;            /* Line end character         */

   if (   (numargs < 4)                /* May have either 4,5 or 6   */
       || (numargs > 6) )
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port handle   */
       || !RXVALIDSTRING(args[1])      /* and read length            */
       || !RXVALIDSTRING(args[2])      /* and timeout                */
       || !RXVALIDSTRING(args[3]) )    /* and string to return in    */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   if ( !Str2Num(args[1].strptr, &ulNumRequested, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert length failed      */

   if ( ulNumRequested > RXAUTOBUFLEN-1 )
      return( ROUTINE_INVALID );       /* Invalid length specified   */

   if ( !Str2Num(args[2].strptr, &lTimeLimit, STR2NUM_DEC2LONG) )
      return( ROUTINE_INVALID );       /* Convert timeout failed     */

   pszNullReplace = NULL;
   if ( numargs >= 5 ) {               /* If five or more arguements */
      if ( RXVALIDSTRING(args[4]) )    /* If specified               */
         if ( args[4].strlength > 1 )  /* Must only be one or zero   */
            return( ROUTINE_INVALID );

      pszNullReplace = args[4].strptr;
   }

   pszLineEnd = NULL;
   if ( numargs >= 6 ) {               /* If six or more arguements  */
      if ( RXVALIDSTRING(args[5]) )    /* If specified               */
         if ( args[5].strlength > 1 )  /* Must only be one or zero   */
            return( ROUTINE_INVALID );

      pszLineEnd = args[5].strptr;
   }

   /* fetch rexx variable details  (rexx mallocs, we free) */

   InitVar( &ReadVar, NULL, args[3].strptr );

   rc = FetchVar( &ReadVar );

   if ( rc == NO_ERROR ) {

      ReadVar.shvvalue.strptr[0] = EOSTR_CH;

      if ( ulNumRequested > 0 ) {
         ReadVar.shvvalue.strlength = ulNumRequested;
      } else {
         ReadVar.shvvalue.strlength = RXAUTOBUFLEN;
      }

      rc = ReadPort( hcomPort,
                     ReadBuff,
                     ulNumRequested,
                     ReadVar.shvvalue.strptr,
                     &(ReadVar.shvvalue.strlength),
                     lTimeLimit,
                     pszNullReplace,
                     pszLineEnd );

      /* set rexx variable details with retrieved values */

      rc2 = UpdateVar( &ReadVar );     /* Set the rexx variable      */

      if ( rc == NO_ERROR ) {          /* Only if not already error  */
         rc = rc2;
      }
   }

   FreeVar( &ReadVar );                /* Cleanup                    */

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosDevIOCtl result  */

   return( ROUTINE_VALID );            /* All ok                     */
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncWrite                                           */
/*                                                                   */
/* Purpose:   Write to the specified port                            */
/*                                                                   */
/* Syntax:    rc = RxAsyncWrite( port, delay, outvar, 'remvar' )     */
/*                                                                   */
/* Params:    port   - handle of open port                           */
/*            delay  - delay before writing (millisecs)              */
/*            outvar - string to be written                          */
/*            remvar - name of variable to return unwritten count in */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosWrite                              */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   crlf = D2C(13)''D2C(10)                                         */
/*   Remaining = 0                                                   */
/*   rc = RxAsyncWrite( PortHandle, 0, 'ATZ'crlf, 'Remaining' )      */
/*   if rc <> 0                                                      */
/*      then say 'Modem not turned on, >'Remaining'< chars unwritten'*/
/*      else say 'RxAsyncWrite output ATZ<crlf>'                     */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncWrite(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET       rc;                    /* Function return code       */
   APIRET       rc2;                   /* RexxVariablePool return    */
   HFILE        hcomPort;              /* Communications port handle */
   ULONG        ulRemains;             /* Number of chars not written*/
   LONG         millisecs;             /* Time to sleep              */

   if ( numargs != 4 )                 /* Must have four arguments   */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port handle   */
       || !RXVALIDSTRING(args[1])      /* and write delay            */
       ||   RXNULLSTRING(args[2])      /* and var to write           */
       || !RXVALIDSTRING(args[3]) )    /* and var to return num left */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   if ( !Str2Num(args[1].strptr, &millisecs, STR2NUM_DEC2LONG) )
      return( ROUTINE_INVALID );       /* Convert millisecs failed   */

   if ( millisecs < 0 )                /* Can not wait negative time */
      return( ROUTINE_INVALID );

   if ( millisecs > 0 )                /* Only if its worthwile      */
      DosSleep( millisecs );

   rc = WritePort( hcomPort, args[2].strptr, args[2].strlength, &ulRemains );

   rc2 = SetRexxValue( args[3].strptr, VAL2STR_ULONG2DEC, &ulRemains );
   if ( rc2 != NO_ERROR ) {            /* Couldn't set rexx variable */

      if ( rc == NO_ERROR ) {          /* Not already in error       */
         rc = rc2;
      }
   }

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosWrite result     */

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncPrint                                           */
/*                                                                   */
/* Purpose:   Write to stdout                                        */
/*                                                                   */
/* Syntax:    call RxAsyncPrint string                               */
/*                                                                   */
/* Params:    string  - text to be written                           */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   call RxAsyncPrint 'This is printed on stdout'                   */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncPrint(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr )
{
   if ( numargs != 1 )                 /* Must have one argument     */
      return( ROUTINE_INVALID );

   if (     RXNULLSTRING(args[0]) )    /* Must be a string (empty ok)*/
      return( ROUTINE_INVALID );

   fprintf( stdout, args[0].strptr );

   BUILDRXDOSRET( retstr, NO_ERROR );  /* Return result              */

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncCarrier                                         */
/*                                                                   */
/* Purpose:   Test for data carrier detect on specified port         */
/*                                                                   */
/*            Utilises IOCTL 67H ASYNC_GETMODEMINPUT.                */
/*                                                                   */
/* Syntax:    rc = RxAsyncCarrier( port )                            */
/*                                                                   */
/*      OR    rc = RxAsyncCarrier( port, timeout )                   */
/*                                                                   */
/* Params:    port    - handle of open port                          */
/*                                                                   */
/*            timeout - optional second parameter (milliseconds)     */
/*                      which when specified and > 0 causes this     */
/*                      routine to wait for an active DCD signal.    */
/*                                                                   */
/*                      If -1 is specified then the routine waits    */
/*                      forever.                                     */
/*                                                                   */
/*                      If 0 is specified or the parameter is        */
/*                      omitted then the current carrier status      */
/*                      is returned.                                 */
/*                                                                   */
/* Return:                                                           */
/*   Carrier active   - NO_ERROR (0)                                 */
/*   Wait timed out   - RC_ASYNC_TIMED_OUT (-1)                      */
/*   Carrier inactive - RC_ASYNC_NO_SIGNAL (-2)                      */
/*   Error occurred   - rc from DosDevIOCtl                          */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   rc = RxAsyncCarrier( PortHandle )                               */
/*   if rc = 0                                                       */
/*      then say 'RxAsyncCarrier detected DCD'                       */
/*      else if rc = -2                                              */
/*              then say 'RxAsyncCarrier did not detect DCD'         */
/*              else say 'RxAsyncCarrier failed with rc=>'rc'<'      */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncCarrier(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr )
{
   APIRET    rc;                       /* Function return code       */
   HFILE     hcomPort;                 /* Communications port handle */
   LONG      lTimeLimit;               /* Timeout in milliseconds    */

   if (   (numargs < 1)                /* May have either one        */
       || (numargs > 2) )              /* or two arguments           */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0]) )    /* Must specify port handle   */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   lTimeLimit = 0;                     /* Default to return immediate*/

   if ( numargs == 2 ) {               /* If two arguments then      */

      if ( !RXVALIDSTRING(args[1]) )   /* Must specify timeout       */
         return( ROUTINE_INVALID );

      if ( !Str2Num(args[1].strptr, &lTimeLimit, STR2NUM_DEC2LONG) )
         return( ROUTINE_INVALID );    /* Convert timeout failed     */
   }

   rc = WaitCarrier( hcomPort, lTimeLimit );

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncBreak                                           */
/*                                                                   */
/* Purpose:   Send break for specified length of time                */
/*                                                                   */
/*            Utilises IOCTLs 45H ASYNC_SETBREAKOFF                  */
/*                        and 4BH ASYNC_SETBREAKON.                  */
/*                                                                   */
/* Syntax:    call RxAsyncBreak handlevar milliseconds               */
/*                                                                   */
/* Params:    handlevar - rexx variable with com handle              */
/*            millisecs - duration of break to send                  */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   BreakTime = 500                                                 */
/*   rc = RxAsyncBreak( PortHandle, BreakTime )                      */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncBreak failed with rc=>'rc'<'                */
/*      else say 'RxAsyncBreak sent break for 'BreakTime' millisecs' */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncBreak(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr )
{
   APIRET    rc;                       /* Function return code       */
   HFILE     hcomPort;                 /* Communications port handle */
   ULONG     ulDuration;               /* Break duration             */

   if ( numargs != 2 )                 /* Must have two arguments    */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port handle   */
       || !RXVALIDSTRING(args[1]) )    /* and break duration         */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   if ( !Str2Num(args[1].strptr, &ulDuration, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert duration failed    */

   rc = SendBreak( hcomPort, ulDuration );

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncDcbInfo                                         */
/*                                                                   */
/* Purpose:   Print current port settings                            */
/*                                                                   */
/*            Utilises IOCTL 73H ASYNC_GETDCBINFO.                   */
/*                                                                   */
/* Syntax:    call RxAsyncDcbInfo handlevar                          */
/*                                                                   */
/* Params:    handlevar - rexx variable with com handle              */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   rc = RxAsyncDcbInfo( PortHandle )                               */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncDcbInfo failed with rc=>'rc'<'              */
/*      else say 'RxAsyncDcbInfo returned okay.'                     */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncDcbInfo(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr )
{
   APIRET   rc;                        /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */

   if ( numargs != 1 )                 /* Must have one argument     */
      return( ROUTINE_INVALID );

   if ( !RXVALIDSTRING(args[0]) )      /* Must specify port          */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   rc = PrtDcbInfo( hcomPort );

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosDevIOCtl result  */

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncFileExists                                      */
/*                                                                   */
/* Purpose:   Test for existence of specified file                   */
/*                                                                   */
/* Syntax:    if RxAsyncFileExists( FileName )                       */
/*                                                                   */
/* Params:    FileName - Name of file to test for (WILDCARDS OK)     */
/*                                                                   */
/* Return:                                                           */
/*   File was found   - RC_REXX_TRUE (1)                             */
/*   File not found   - RC_REXX_FALSE (0)                            */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   if RxAsyncFileExists( 'C:\*.BAT' )                              */
/*      then say 'RxAsyncFileExists found a batch file.'             */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncFileExists(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr )
{
   if ( numargs != 1 )                 /* Must have one argument     */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0]) )    /* Must specify file name     */
      return( ROUTINE_INVALID );

   if ( fFileExists( args[0].strptr ) ) {
      BUILDRXDOSRET( retstr, RC_REXX_TRUE );
   } else {
      BUILDRXDOSRET( retstr, RC_REXX_FALSE );
   }

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncFileDelete                                      */
/*                                                                   */
/* Purpose:   Delete specified file                                  */
/*                                                                   */
/* Syntax:    if RxAsyncFileDelete( FileName )                       */
/*                                                                   */
/* Params:    FileName - Name of file to delete (NO WILDCARDS)       */
/*                                                                   */
/* Return:                                                           */
/*   File was deleted - RC_REXX_TRUE (1)                             */
/*   File not deleted - RC_REXX_FALSE (0)                            */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   if RxAsyncFileDelete( 'C:\FRED.TXT' )                           */
/*      then say 'RxAsyncFileDelete deleted file Fred.Txt'           */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncFileDelete(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr )
{
   if ( numargs != 1 )                 /* Must have one argument     */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0]) )    /* Must specify file name     */
      return( ROUTINE_INVALID );

   if ( fFileDelete( args[0].strptr ) ) {
      BUILDRXDOSRET( retstr, RC_REXX_TRUE );
   } else {
      BUILDRXDOSRET( retstr, RC_REXX_FALSE );
   }

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncFileCopy                                        */
/*                                                                   */
/* Purpose:   Copy a specified file                                  */
/*                                                                   */
/* Syntax:    if RxAsyncFileCopy( FromName, ToName )                 */
/*                                                                   */
/* Params:    FromName - Name of file to copy (NO WILDCARDS)         */
/*            ToName   - Name to copy to      (NO WILDCARDS)         */
/*                                                                   */
/* Return:                                                           */
/*   File was copied  - RC_REXX_TRUE (1)                             */
/*   File not copied  - RC_REXX_FALSE (0)                            */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   if RxAsyncFileCopy( 'C:\FRED.TXT', 'D:\FRED.BAK' )              */
/*      then say 'RxAsyncFileCopy copied Fred.Txt to Fred.Bak'       */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncFileCopy(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr )
{
   if ( numargs != 2 )                 /* Must have two arguments    */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify file name src */
       || !RXVALIDSTRING(args[1]) )    /* Must specify file name trg */
      return( ROUTINE_INVALID );

   if ( fFileCopy( args[0].strptr, args[1].strptr ) ) {
      BUILDRXDOSRET( retstr, RC_REXX_TRUE );
   } else {
      BUILDRXDOSRET( retstr, RC_REXX_FALSE );
   }

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncFileMove                                        */
/*                                                                   */
/* Purpose:   Move a specified file                                  */
/*                                                                   */
/* Syntax:    if RxAsyncFileMove( FromName, ToName )                 */
/*                                                                   */
/* Params:    FromName - Name of file to move (NO WILDCARDS)         */
/*            ToName   - Name to move to      (NO WILDCARDS)         */
/*                                                                   */
/* Return:                                                           */
/*   File was moved   - RC_REXX_TRUE (1)                             */
/*   File not moved   - RC_REXX_FALSE (0)                            */
/*                                                                   */
/* Notes:                                                            */
/*   Permits the movement of files accross drives.                   */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   if RxAsyncFileMove( 'C:\FRED.TXT', 'D:\FRED.TXT' )              */
/*      then say 'RxAsyncFileMove moved C:\Fred.Txt to D:\Fred.Txt'  */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncFileMove(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr )
{
   if ( numargs != 2 )                 /* Must have two arguments    */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify file name src */
       || !RXVALIDSTRING(args[1]) )    /* Must specify file name trg */
      return( ROUTINE_INVALID );

   if ( fFileMove( args[0].strptr, args[1].strptr ) ) {
      BUILDRXDOSRET( retstr, RC_REXX_TRUE );
   } else {
      BUILDRXDOSRET( retstr, RC_REXX_FALSE );
   }

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncFileRename                                      */
/*                                                                   */
/* Purpose:   Rename specified file                                  */
/*                                                                   */
/* Syntax:    if RxAsyncFileRename( FromName, ToName )               */
/*                                                                   */
/* Params:    FromName - Name of file to rename  (NO WILDCARDS)      */
/*            ToName   - Name to rename to       (NO WILDCARDS)      */
/*                                                                   */
/* Return:                                                           */
/*   File was renamed - RC_REXX_TRUE (1)                             */
/*   File not renamed - RC_REXX_FALSE (0)                            */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   if RxAsyncFileRename( 'C:\FRED.TXT', 'C:\FRED.OLD' )            */
/*      then say 'RxAsyncFileRename renamed Fred.Txt to Fred.Old'    */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncFileRename(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr )
{
   if ( numargs != 2 )                 /* Must have two arguments    */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify file name old */
       || !RXVALIDSTRING(args[1]) )    /* Must specify file name new */
      return( ROUTINE_INVALID );

   if ( fFileRename( args[0].strptr, args[1].strptr ) ) {
      BUILDRXDOSRET( retstr, RC_REXX_TRUE );
   } else {
      BUILDRXDOSRET( retstr, RC_REXX_FALSE );
   }

   return( ROUTINE_VALID );            /* All ok                     */
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncFileNameIs                                      */
/*                                                                   */
/* Purpose:   Determine file name                                    */
/*                                                                   */
/* Syntax:    Tname = RxAsyncFileNameIs( FileName )                  */
/*                                                                   */
/* Params:    FileName - File spec to extract name from              */
/*                                                                   */
/* Return:                                                           */
/*   File name extracted if any                                      */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   Fname = RxAsyncFileNameIs( 'C:\FRED.TXT' )                      */
/*   say 'RxAsyncFileNameIs returned 'Fname                          */
/*                                                                   */
/*   RxAsyncFileNameIs returned FRED.TXT                             */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncFileNameIs(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr )
{
   if ( numargs != 1 )                 /* Must have one argument     */
      return( ROUTINE_INVALID );

   if ( RXNULLSTRING(args[0]) )        /* Must specify file name     */
      return( ROUTINE_INVALID );

   FileNameIs( args[0].strptr, retstr->strptr );

   retstr->strlength = strlen(retstr->strptr);

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncFilePathIs                                      */
/*                                                                   */
/* Purpose:   Determine path name                                    */
/*                                                                   */
/* Syntax:    Tpath = RxAsyncFilePathIs( FileName )                  */
/*                                                                   */
/* Params:    FileName - File spec to extract path from              */
/*                                                                   */
/* Return:                                                           */
/*   File path extracted if any                                      */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   Fpath = RxAsyncFilePathIs( 'C:\FRED.TXT' )                      */
/*   say 'RxAsyncFilePathIs returned 'Fpath                          */
/*                                                                   */
/*   RxAsyncFilePathIs returned C:\                                  */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncFilePathIs(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr )
{
   if ( numargs != 1 )                 /* Must have one argument     */
      return( ROUTINE_INVALID );

   if ( RXNULLSTRING(args[0]) )        /* Must specify file name     */
      return( ROUTINE_INVALID );

   FilePathIs( args[0].strptr, retstr->strptr );

   retstr->strlength = strlen(retstr->strptr);

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncDirectory                                       */
/*                                                                   */
/* Purpose:   Query/Change directory                                 */
/*                                                                   */
/* Syntax:    Tdir = RxAsyncDirectory( NewDir )                      */
/*                                                                   */
/* Params:    NewDir - Name of directory to change to if specified   */
/*                                                                   */
/* Return:                                                           */
/*   Current/New directory name                                      */
/*                                                                   */
/* Notes:                                                            */
/*   Differs from the standard directory() function by always        */
/*   appending a trailing backslash to the returned path spec.       */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   OldDir = RxAsyncDirectory()                                     */
/*   NewDir = 'C:\OS2\'                                              */
/*   CurDir = RxAsyncDirectory( NewDir )                             */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncDirectory(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr )
{
   if ( numargs > 1 )                  /* May have one argument/none */
      return( ROUTINE_INVALID );

   if ( numargs == 1 ) {               /* If argument specified      */

      if ( RXNULLSTRING(args[0]) )     /* Must be valid string       */
         return( ROUTINE_INVALID );

      FileChgDir( args[0].strptr );
   }

   FileGetDir( retstr->strptr );

   retstr->strlength = strlen(retstr->strptr);

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncMsgBox                                          */
/*                                                                   */
/* Purpose:   Display a PM message box                               */
/*                                                                   */
/* Syntax:    answ = RxAsyncMsgBox( MsgTitle,                        */
/*                                  MsgIcon,                         */
/*                                  MsgButtons,                      */
/*                                  MsgHelp,                         */
/*                                  MsgNumber,                       */
/*                                  MsgText )                        */
/*                                                                   */
/* Params:    MsgTitle    - Text to use as the title if specified    */
/*            MsgIcon     - Type of icon to display in box           */
/*                                                                   */
/*              N-one             NO_ICON                            */
/*              I-nformation      ICONASTERISK                       */
/*              W-arning          ICONEXCLAMATION                    */
/*              Q-uery            ICONQUESTION                       */
/*              S-top             ICONHAND                           */
/*                                                                   */
/*              Note: Only the first character is significant.       */
/*                                                                   */
/*            MsgButtons  - Type of message box to use               */
/*                                                                   */
/*              V-anilla          ENTER                              */
/*              I-nformation      OK                                 */
/*              W-arning          OK CANCEL                          */
/*              Q-uery            YES NO                             */
/*              S-top             CANCEL                             */
/*              R-etry            RETRY CANCEL                       */
/*              A-bort            ABORT RETRY IGNORE                 */
/*                                                                   */
/*              Note: Only the first character is significant.       */
/*                                                                   */
/*            MsgHelp     - Whether to add help button "Y"/"N"       */
/*                                                                   */
/*              Note: Only the first character is significant.       */
/*                                                                   */
/*            MsgNumber   - System message to retrieve if specified  */
/*            MsgText     - Text of message or parameters for Number */
/*                                                                   */
/*              Note: If using a system message (Number not 0) then  */
/*                    the parameters are supplied in the MsgText as  */
/*                    a linefeed (0x10) delimited list. Excess       */
/*                    parameters will be discarded and missing       */
/*                    parameters will have '<?>' substituted into    */
/*                    the constructed message text.                  */
/*                                                                   */
/* Return:                                                           */
/*   Value indicating response chosen.                               */
/*     0 - OK, YES, RETRY, ENTER                                     */
/*     1 - CANCEL, NO, ABORT, ERROR                                  */
/*     2 - IGNORE                                                    */
/*     3 - HELP                                                      */
/*                                                                   */
/* Notes:                                                            */
/*   At present the use of system messages is not implemented.       */
/*                                                                   */
/*   In order to use RxAsyncMsgBox your rexx script must be invoked  */
/*   under a CMD.EXE session which has been started as a PM session. */
/*                                                                   */
/*   To do this you use the following command:-                      */
/*                                                                   */
/*     START "Test script" /PM /PGM "Cmd.Exe" /C "Your.cmd"          */
/*                                                                   */
/*     (see the START command in the OS/2 Command Reference for a    */
/*      full explanation of the parameters.)                         */
/*                                                                   */
/*   WARNING: When running your script under PM in this way you will */
/*            NOT have a window session, hence NO stdin, stdout or   */
/*            stderr, unless you redirect them.                      */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   Tans = RxAsyncMsgBox( "A message from RxAsync.",                */
/*                         "Question mark icon",                     */
/*                         "Query buttons",                          */
/*                         "No help button please",                  */
/*                         0,                                        */
/*                         "Hi there dudes! Want more?" )            */
/*   if Tans = 0                                                     */
/*      then call UserPressedYES                                     */
/*      else call UserPressedNO                                      */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncMsgBox(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr )
{
   APIRET    rc;                       /* Function return code       */
   CHAR      chIcon;                   /* Code for icon type         */
   CHAR      chButtons;                /* Code for button type       */
   CHAR      chHelp;                   /* Want help button           */
   USHORT    usNumber;                 /* Message number if any      */

   if ( numargs != 6 )                 /* Must have five arguments   */
      return( ROUTINE_INVALID );

   if ( RXVALIDSTRING(args[1]) ) {
      chIcon = (args[1].strptr)[0];    /* Only want first character  */
   } else {
      chIcon = 'N';                    /* Default to No icon         */
   }

   if ( RXVALIDSTRING(args[2]) ) {
      chButtons = (args[2].strptr)[0]; /* Only want first character  */
   } else {
      chButtons = 'V';                 /* Default to Vanilla button  */
   }

   if ( RXVALIDSTRING(args[3]) ) {
      chHelp = (args[3].strptr)[0];    /* Only want first character  */
   } else {
      chHelp = 'N';                    /* Default to No help button  */
   }

   if ( strchr(MSG_ICON_STYLES, chIcon) == NULL )
      return( ROUTINE_INVALID );       /* Invalid icon style         */

   if ( strchr(MSG_BUTTON_STYLES, chButtons) == NULL )
      return( ROUTINE_INVALID );       /* Invalid button style       */

   if ( strchr(YESNO_OPTIONS, chHelp) == NULL )
      return( ROUTINE_INVALID );       /* Invalid help style         */

   if ( RXVALIDSTRING(args[4]) ) {
      if ( !Str2Num(args[4].strptr, &(usNumber), STR2NUM_DEC2USHORT) )
         return( ROUTINE_INVALID );    /* Invalid message number     */
   } else {
      usNumber = 0;
   }

   rc = MsgBox( args[0].strptr,
                chIcon,
                chButtons,
                chHelp,
                usNumber,
                args[5].strptr );

   BUILDRXDOSRET( retstr, rc );

   return( ROUTINE_VALID );            /* All ok                     */
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncWatch                                           */
/*                                                                   */
/* Purpose:   Wait for one or more strings to be read.               */
/*                                                                   */
/*            Utilises IOCTL 72H ASYNC_GETCOMMEVENT.                 */
/*                                                                   */
/* Syntax:    rc = RxAsyncWatch( port,                               */
/*                               'match',                            */
/*                               nocase,                             */
/*                               timeout,                            */
/*                               'varname',                          */
/*                               'findndx',                          */
/*                               nullrep,                            */
/*                               lineend )                           */
/*                                                                   */
/* Params:    port    - Handle of open port.                         */
/*                                                                   */
/*            match   - Name of stem variable which specifies the    */
/*                      strings to search for.                       */
/*                                                                   */
/*            nocase  - Whether to ignore case "Y"/"N".              */
/*                                                                   */
/*                      Note: Only first character is significant.   */
/*                                                                   */
/*            timeout - Maximum period to wait in milliseconds.      */
/*                                                                   */
/*                       0 = no waiting                              */
/*                      -1 = no timeout                              */
/*                                                                   */
/*            varname - Name of rexx variable to return data in.     */
/*                                                                   */
/*            findndx - Name of rexx variable to return ndx found.   */
/*                                                                   */
/*            nullrep - Optional parameter which when specified      */
/*                      permits null charatacers in the input stream */
/*                      to be either discarded ('') or replaced      */
/*                      ('?').  An empty string tells the function   */
/*                      to discard nulls, otherwise all nulls are    */
/*                      replaced with the character specified.  If   */
/*                      this parameter is not specified then all     */
/*                      nulls will be returned in the string passed  */
/*                      back to the caller.                          */
/*                                                                   */
/*            lineend - Optional parameter which when specified      */
/*                      indicates an additional character to be used */
/*                      in detecting end_of_line.  The defaults if   */
/*                      not specified are line_feed, form_feed or    */
/*                      end_of_file.  When specified the character   */
/*                      acts in addition to the defaults.            */
/*                                                                   */
/* Notes:     RxAsyncWatch is a line mode only routine, as each line */
/*            is read it is checked to see if one of the specified   */
/*            strings is present.  If found then the line containing */
/*            the string is returned in the variable specified, and  */
/*            the number of the string found is returned in the find */
/*            index.  If not found then the line is printed on       */
/*            stdout and the next line is read.                      */
/*                                                                   */
/* Return:                                                           */
/*   Call successful  - NO_ERROR (0)                                 */
/*   Read timed out   - RC_ASYNC_TIMED_OUT (-1)                      */
/*   Call failed      - ERROR_NOT_ENOUGH_MEMORY (8)                  */
/*          "         - ERROR_INVALID_PARAMETER (87)                 */
/*          "         - rc from DosRead                              */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   lnfd = d2c(10)                                                  */
/*   MatchStr.0 = 2                                                  */
/*   MatchStr.1 = 'Hello'                                            */
/*   MatchStr.2 = 'Goodbye'                                          */
/*   InpStr = ''                                                     */
/*   NumFound = 0                                                    */
/*   rc =  RxAsyncWatch( PortHandle, 'MatchStr', 'Y', 3000,          */
/*                       'InpStr', 'NumFound', '', lnfd )            */
/*   if rc = 0                                                       */
/*      then do                                                      */
/*             say 'RxAsyncWatch returned >'InpStr'<'                */
/*             say 'Index of string found was >'NumFound'<'          */
/*             switch                                                */
/*                when NumFound = 1 then say 'Hi dude'               */
/*                when NumFound = 2 then say 'Later dude'            */
/*                otherwise say 'Nothing found'                      */
/*             end                                                   */
/*           end                                                     */
/*      else if rc = -1                                              */
/*              then say 'RxAsyncWatch timed out'                    */
/*              else say 'RxAsyncWatch failed rc=>'rc'<'             */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncWatch(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr )
{
   APIRET       rc;                    /* Function return code       */
   APIRET       rc2;                   /* RexxVariablePool return    */
   HFILE        hcomPort;              /* Communications port handle */
   BYTE         ReadBuff[RXAUTOBUFLEN];/* Input buffer               */
   LONG         lTimeLimit;            /* Timeout in milliseconds    */
   SHVBLOCK     ReadVar;               /* Rexx variable for input    */
   PSZ          pszNullReplace;        /* Null replace character     */
   PSZ          pszLineEnd;            /* Line end character         */
   ULONG        ulMaxStrings;          /* Number of strings to check */
   ULONG        ulCurString;           /* Number of current string   */
   CHAR         StemBuff[RXAUTOBUFLEN];/* Stem content buffer        */
   BOOL         bStringFound;          /* Found required string      */
   BOOL         bIgnoreCase;           /* Whether to ignore case     */

   if (   (numargs < 6)                /* May have either 6,7 or 8   */
       || (numargs > 8) )
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port handle   */
       || !RXVALIDSTRING(args[1])      /* and match stem var name    */
       || !RXVALIDSTRING(args[2])      /* and case option            */
       || !RXVALIDSTRING(args[3])      /* and timeout                */
       || !RXVALIDSTRING(args[4])      /* and string to return data  */
       || !RXVALIDSTRING(args[5]) )    /* and string to return index */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   if ( (args[1].strptr)[args[1].strlength-1] == FILE_DELIM_CH )
      return( ROUTINE_INVALID );       /* Invalid stem variable name */

   if ( strchr(YESNO_OPTIONS, (CHAR) (args[2].strptr)[0]) == NULL )
      return( ROUTINE_INVALID );       /* Invalid ignore case option */

   if ( !Str2Num(args[3].strptr, &lTimeLimit, STR2NUM_DEC2LONG) )
      return( ROUTINE_INVALID );       /* Convert timeout failed     */

   pszNullReplace = NULL;
   if ( numargs >= 7 ) {               /* If seven or more arguements*/
      if ( RXVALIDSTRING(args[6]) )    /* If specified               */
         if ( args[6].strlength > 1 )  /* Must only be one or zero   */
            return( ROUTINE_INVALID );

      pszNullReplace = args[6].strptr;
   }

   pszLineEnd = NULL;
   if ( numargs >= 8 ) {               /* If eight or more arguements*/
      if ( RXVALIDSTRING(args[7]) )    /* If specified               */
         if ( args[7].strlength > 1 )  /* Must only be one or zero   */
            return( ROUTINE_INVALID );

      pszLineEnd = args[7].strptr;
   }

   /* set ignore case flag */

   bIgnoreCase = ( strchr(YES_OPTIONS, (CHAR) (args[2].strptr)[0]) != NULL );

   /* initialise returned index */

   ulCurString = 0;
   rc = SetRexxValue( args[5].strptr, VAL2STR_ULONG2DEC, &ulCurString );
   if ( rc != NO_ERROR ) {
      NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );
      BUILDRXDOSRET( retstr, rc );
      return( ROUTINE_VALID );
   }

   /* fetch stem.0 count of elements */

   ulMaxStrings = 0;
   rc = GetStemValue( args[1].strptr, 0, STR2NUM_DEC2ULONG, &ulMaxStrings );
   if ( rc != NO_ERROR ) {
      NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );
      BUILDRXDOSRET( retstr, rc );
      return( ROUTINE_VALID );         /* Fetch element count failed */
   }

   if ( ulMaxStrings == 0 ) {
      NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );
      BUILDRXDOSRET( retstr, ERROR_INVALID_PARAMETER );
      return( ROUTINE_VALID );         /* Must be non zero           */
   }

   /* fetch rexx variable details  (rexx mallocs, we free) */

   InitVar( &ReadVar, NULL, args[4].strptr );

   rc = FetchVar( &ReadVar );

   if ( rc == NO_ERROR ) {

      rc2 = NO_ERROR;

      bStringFound = FALSE;

      do {

         /* read a line */

         ReadVar.shvvalue.strptr[0] = EOSTR_CH; /* Initialise return var   */

         ReadVar.shvvalue.strlength = RXAUTOBUFLEN;

         rc = ReadPort( hcomPort,
                        ReadBuff,
                        0,
                        ReadVar.shvvalue.strptr,
                        &(ReadVar.shvvalue.strlength),
                        lTimeLimit,
                        pszNullReplace,
                        pszLineEnd );

         /* check if line contains a required string */

         strcpy(ReadBuff, ReadVar.shvvalue.strptr); /* save copy for testing */
         if ( bIgnoreCase ) {
            strupr(ReadBuff);
         }

         ulCurString = 0;
         do {
            ulCurString = ulCurString + 1;
            rc2 = GetStemValue( args[1].strptr, ulCurString, STR2NUM_STR2STR, StemBuff );
            if ( rc2 == NO_ERROR ) {
               /* check to see if required string is present */
               if ( bIgnoreCase ) {
                  strupr(StemBuff);
               }
               bStringFound = (strstr(ReadBuff, StemBuff) != NULL);
            }
         } while( (!bStringFound) && (ulCurString < ulMaxStrings) && (rc2 == NO_ERROR) );

         if ( !bStringFound ) {
            fprintf( stdout, ReadVar.shvvalue.strptr );
         }

      } while( (!bStringFound) && (rc == NO_ERROR) && (rc2 == NO_ERROR) );

      if ( bStringFound ) {
         rc = SetRexxValue( args[5].strptr, VAL2STR_ULONG2DEC, &ulCurString );
      } else if ( rc == NO_ERROR ) {
         rc = rc2;
      }

      /* set rexx variable details with retrieved values */

      rc2 = UpdateVar( &ReadVar );     /* Set the rexx variable      */

      if ( rc == NO_ERROR ) {          /* Only if not already error  */
         rc = rc2;
      }
   }

   FreeVar( &ReadVar );                /* Cleanup                    */

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosDevIOCtl result  */

   return( ROUTINE_VALID );            /* All ok                     */
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncFlushInput                                      */
/*                                                                   */
/* Purpose:   Flush device driver input queue                        */
/*                                                                   */
/*            Utilises IOCTL 01H DEV_FLUSHINPUT.                     */
/*                                                                   */
/* Syntax:    call RxAsyncFlushInput port                            */
/*                                                                   */
/* Params:    port   - handle of open port                           */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   rc = RxAsyncFlushInput( PortHandle )                            */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncFlushInput failed with rc=>'rc'<'           */
/*      else say 'RxAsyncFlushInput returned okay.'                  */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncFlushInput(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET   rc;                        /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */

   if ( numargs != 1 )                 /* Must have one argument     */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0]) )    /* Must specify port          */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   rc = FlushInput( hcomPort );

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosDevIOCtl result  */

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncFlushOutput                                     */
/*                                                                   */
/* Purpose:   Flush device driver output queue                       */
/*                                                                   */
/*            Utilises IOCTL 02H DEV_FLUSHOUTPUT.                    */
/*                                                                   */
/* Syntax:    call RxAsyncFlushOutput port                           */
/*                                                                   */
/* Params:    port   - handle of open port                           */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   rc = RxAsyncFlushOutput( PortHandle )                           */
/*   if rc <> 0                                                      */
/*      then say 'RxAsyncFlushOutput failed with rc=>'rc'<'          */
/*      else say 'RxAsyncFlushOutput returned okay.'                 */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncFlushOutput(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr
       )
{
   APIRET   rc;                        /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */

   if ( numargs != 1 )                 /* Must have one argument     */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0]) )    /* Must specify port          */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   rc = FlushOutput( hcomPort );

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );        /* Return DosDevIOCtl result  */

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncActive                                          */
/*                                                                   */
/* Purpose:   Test if specified signal is active                     */
/*                                                                   */
/* Syntax:    if RxAsyncActive( port, signal )                       */
/*                                                                   */
/* Params:    port   - handle of open port                           */
/*            signal - modem signal to test                          */
/*                     DTR, RTS, CTS, DSR, RI, DCD                   */
/*                                                                   */
/* Return:                                                           */
/*   Signal active    - RC_REXX_TRUE (1)                             */
/*   Signal inactive  - RC_REXX_FALSE (0)                            */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   if RxAsyncActive( PortHandle, "CTS" )                           */
/*      then say 'RxAsyncActive detected CTS signal.'                */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncActive(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr )
{
   APIRET   rc;                        /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */
   BYTE     bMdmSignal;                /* Modem signal to check      */

   if ( numargs != 2 )                 /* Must have two arguments    */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port handle   */
       || !RXVALIDSTRING(args[1]) )    /* and signal type            */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   if        ( stricmp(args[1].strptr, "CTS") == 0 ) {
      bMdmSignal = CTS_ON;
   } else if ( stricmp(args[1].strptr, "DSR") == 0 ) {
      bMdmSignal = DSR_ON;
   } else if ( stricmp(args[1].strptr, "RI" ) == 0 ) {
      bMdmSignal = RI_ON;
   } else if ( stricmp(args[1].strptr, "DCD") == 0 ) {
      bMdmSignal = DCD_ON;
   } else if ( stricmp(args[1].strptr, "DTR") == 0 ) {
      bMdmSignal = DTR_ON;
   } else if ( stricmp(args[1].strptr, "RTS") == 0 ) {
      bMdmSignal = RTS_ON;
   } else {
      return( ROUTINE_INVALID );       /* Invalid signal string      */
   }

   rc = ActiveSignal( hcomPort, bMdmSignal );

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   if ( rc == NO_ERROR ) {
      BUILDRXDOSRET( retstr, RC_REXX_TRUE );
   } else {
      BUILDRXDOSRET( retstr, RC_REXX_FALSE );
   }

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncRaise                                           */
/*                                                                   */
/* Purpose:   Activate specified signal                              */
/*                                                                   */
/* Syntax:    if RxAsyncRaise( port, signal )                        */
/*                                                                   */
/* Params:    port   - handle of open port                           */
/*            signal - modem signal to enable (either DTR or RTS)    */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   if RxAsyncRaise( PortHandle, "DTR" ) = 0                        */
/*      then say 'RxAsyncRaise enabled DTR.'                         */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncRaise(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr )
{
   APIRET   rc;                        /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */
   BYTE     bModemOn;                  /* Modem signal to activate   */

   if ( numargs != 2 )                 /* Must have two arguments    */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port handle   */
       || !RXVALIDSTRING(args[1]) )    /* and signal type            */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   if        ( stricmp(args[1].strptr, "DTR") == 0 ) {
      bModemOn = DTR_ON;
   } else if ( stricmp(args[1].strptr, "RTS") == 0 ) {
      bModemOn = RTS_ON;
   } else {
      return( ROUTINE_INVALID );       /* Invalid signal string      */
   }

   rc = SetMdmOutput( hcomPort, bModemOn, NONE_OFF );

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );

   return( ROUTINE_VALID );
}

/*********************************************************************/
/*                                                                   */
/* Function:  RxAsyncLower                                           */
/*                                                                   */
/* Purpose:   De-activate specified signal                           */
/*                                                                   */
/* Syntax:    if RxAsyncLower( port, signal )                        */
/*                                                                   */
/* Params:    port   - handle of open port                           */
/*            signal - modem signal to drop (either DTR or RTS)      */
/*                                                                   */
/* Return:                                                           */
/*   Call successful - NO_ERROR (0)                                  */
/*   Call failed     - rc from DosDevIOCtl                           */
/*                                                                   */
/* Example:                                                          */
/*                                                                   */
/*   if RxAsyncLower( PortHandle, "DTR" ) = 0                        */
/*      then say 'RxAsyncLower dropped DTR.'                         */
/*                                                                   */
/*********************************************************************/

APIRET RxAsyncLower(
          CHAR     *name,
          ULONG    numargs,
          RXSTRING args[],
          CHAR     *queuename,
          RXSTRING *retstr )
{
   APIRET   rc;                        /* Function return code       */
   HFILE    hcomPort;                  /* Communications port handle */
   BYTE     bModemOff;                 /* Modem signal to activate   */

   if ( numargs != 2 )                 /* Must have two arguments    */
      return( ROUTINE_INVALID );

   if (   !RXVALIDSTRING(args[0])      /* Must specify port handle   */
       || !RXVALIDSTRING(args[1]) )    /* and signal type            */
      return( ROUTINE_INVALID );

   if ( !Str2Num(args[0].strptr, &hcomPort, STR2NUM_DEC2ULONG) )
      return( ROUTINE_INVALID );       /* Convert handle failed      */

   if        ( stricmp(args[1].strptr, "DTR") == 0 ) {
      bModemOff = DTR_OFF;
   } else if ( stricmp(args[1].strptr, "RTS") == 0 ) {
      bModemOff = RTS_OFF;
   } else {
      return( ROUTINE_INVALID );       /* Invalid signal string      */
   }

   rc = SetMdmOutput( hcomPort, NONE_ON, bModemOff );

   NewRexxValue( RxAsyncRcVarName, VAL2STR_ULONG2DEC, &rc );

   BUILDRXDOSRET( retstr, rc );

   return( ROUTINE_VALID );
}

/*********************************************************************/
/* END MODULE                                                        */
/*********************************************************************/
