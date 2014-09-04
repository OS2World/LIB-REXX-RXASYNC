/*****************************************************************************/
/*                                                                           */
/*  MODULE         RXASYNC.cmd                                               */
/*                                                                           */
/*  VERSION        Version 2.0 - 26th Dec 1994                               */
/*                                                                           */
/*  COPYRIGHT      Copyright (c) 1993, 1994 by Crucial Applications          */
/*                              All rights reserved.                         */
/*                                                                           */
/*  AUTHOR         Ian Timms                                                 */
/*                                                                           */
/*  DESCRIPTION                                                              */
/*                                                                           */
/*    Test the essential RXASYNC.DLL functions to make sure they work.       */
/*                                                                           */
/*    NOTE: Only those functions indicated with '+' are tested by this       */
/*          routine.                                                         */
/*                                                                           */
/*  FUNCTIONS                                                                */
/*                                                                           */
/*       House keeping                                                       */
/*                                                                           */
/*  + RxAsyncDropFuncs     -   Drop the functions from REXX                  */
/*  + RxAsyncLoadFuncs     -   Load the functions into REXX                  */
/*                                                                           */
/*       Process control                                                     */
/*                                                                           */
/*  + RxAsyncPriority      -   Set Rexx process priority                     */
/*  + RxAsyncSleep         -   Wait for some defined period                  */
/*                                                                           */
/*       ASYNC IOCTL interface                                               */
/*                                                                           */
/*  + RxAsyncGetDcbInfo    -   Get the device control block info             */
/*  + RxAsyncSetDcbInfo    -   Set the device control block info             */
/*  + RxAsyncGetEnhParm    -   Get the enhanced parameters                   */
/*  + RxAsyncSetEnhParm    -   Set the enhanced parameters                   */
/*  + RxAsyncGetLnCtrl     -   Get the comms port settings                   */
/*  + RxAsyncSetLnCtrl     -   Set the comms port settings                   */
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
/*  + RxAsyncSetMdmOutput  -   Set modem output signals                      */
/*    RxAsyncGetRxCount    -   Get size and number of chars in RX queue      */
/*    RxAsyncGetTxCount    -   Get size and number of chars in TX queue      */
/*    RxAsyncGetComError   -   Get the COM Error Word                        */
/*    RxAsyncGetComEvent   -   Get the COM Event Word                        */
/*                                                                           */
/*       Extensions                                                          */
/*                                                                           */
/*  + RxAsyncOpen          -   Open the specified comms port                 */
/*  + RxAsyncClose         -   Close the specified comms port                */
/*  + RxAsyncWait          -   Wait for receipt of input                     */
/*  + RxAsyncWatch         -   Wait for receipt of specified strings         */
/*  + RxAsyncRead          -   Read data and wait if necessary               */
/*  + RxAsyncWrite         -   Write data to the comms port                  */
/*  + RxAsyncCarrier       -   Test/Wait for Data Carrier Detect (DCD)       */
/*  + RxAsyncBreak         -   Send break for defined period                 */
/*  + RxAsyncDcbInfo       -   Print the device control block info           */
/*  + RxAsyncActive        -   Test for active modem signal                  */
/*  + RxAsyncRaise         -   Raise specified modem signal                  */
/*  + RxAsyncLower         -   Lower specified modem signal                  */
/*                                                                           */
/*       File handling                                                       */
/*                                                                           */
/*  + RxAsyncFileExists    -   Test for file(s) existence                    */
/*  + RxAsyncFileDelete    -   Delete specified file                         */
/*  + RxAsyncFileCopy      -   Copy specified file                           */
/*  + RxAsyncFileMove      -   Move specified file                           */
/*  + RxAsyncFileRename    -   Rename specified file                         */
/*  + RxAsyncFileNameIs    -   Determine file name                           */
/*  + RxAsyncFilePathIs    -   Determine file path                           */
/*  + RxAsyncDirectory     -   Query/Change directory/drive                  */
/*                                                                           */
/*       Miscellaneous                                                       */
/*                                                                           */
/*  + RxAsyncPrint         -   Print string on stdout                        */
/*    RxAsyncMsgBox        -   Display PM message box (PM use only)          */
/*                                                                           */
/*  HISTORY                                                                  */
/*     IT940607 - Added comment re non-16550, thanks to Andre Boisvert for   */
/*                pointing this one out.                                     */
/*     IT941226 - Updated for version 2.0.                                   */
/*                                                                           */
/*****************************************************************************/

call on error

/*--------------------------------------------------------------*/
/* NOTE: Adjust the following as required by your configuration */
/*       If you do NOT have a 16550 UART then set newFlags3     */
/*       to '00000010'. (see the RXASYNC.tip for more info.)    */
/*--------------------------------------------------------------*/

PortName      = 'COM4'
newBaud       = 19200
newData       = 8
newParity     = 'N'
newStop       = 1
newWriteLim   = 50
newReadLim    = 50
newFlags1     = '00001001'
newFlags2     = '10100000'
newFlags3     = '11010010'
newErrChar    = '00'
newBrkChar    = '00'
newXonChar    = '11'
newXoffChar   = '13'
newEnhParms   = '00000010'

PortHandle    = ''
old.Baud      = 0
old.Data      = 0
old.Parity    = ''
old.Stop      = 0
old.WriteLim  = 0
old.ReadLim   = 0
old.Flags1    = ''
old.Flags2    = ''
old.Flags3    = ''
old.ErrChar   = ''
old.BrkChar   = ''
old.XonChar   = ''
old.XoffChar  = ''
old.EnhParms  = ''

True          = 1
False         = 0
Regular       = 2
Critical      = 3
NormalLvl     = 0
InpStr        = ''
Remaining     = 0
crlf          = D2C(13)''D2C(10)
none_on       = '00'
dtr_on        = '01'
rts_on        = '02'
both_on       = '03'
none_off      = 'FF'
dtr_off       = 'FE'
rts_off       = 'FD'
both_off      = 'FC'

/*=====================================================================*/
/* setup                                                               */
/*=====================================================================*/

call RxFuncAdd 'RxAsyncLoadFuncs', 'RXASYNC', 'RxAsyncLoadFuncs'
say 'RxAsyncRc =>'RxAsyncRc'<'
call RxAsyncLoadFuncs
Say 'RxAsyncLoadFuncs is complete'
say 'RxAsyncRc =>'RxAsyncRc'<'

/*=====================================================================*/
/* initialisation                                                      */
/*=====================================================================*/

/* set session priority */

rc = RxAsyncPriority( Critical, NormalLvl )
if rc <> 0
   then say 'RxAsyncPriority failed with rc=>'rc'<'
   else say 'RxAsyncPriority set, Class=>'Critical'<, Level=>'NormalLvl'<'

/* open device */

rc = RxAsyncOpen( PortName, 'PortHandle' )
if rc <> 0
   then say 'RxAsyncOpen failed with rc=>'rc'<'
   else say 'RxAsyncOpen opened the device handle=>'PortHandle'<'

/*=====================================================================*/
/* line control handling                                               */
/*=====================================================================*/

/* save original line control settings */

rc = RxAsyncGetLnCtrl( PortHandle, 'old.Baud', 'old.Data', 'old.Parity', 'old.Stop' )
if rc <> 0
   then say 'RxAsyncGetLnCtrl failed with rc=>'rc'<'
   else say 'RxAsyncGetLnCtrl saved the original settings'

say 'Baudrate >'old.Baud'<'
say 'Databits >'old.Data'<'
say 'Parity   >'old.Parity'<'
say 'Stopbits >'old.Stop'<'

/* set new line control settings */

rc = RxAsyncSetLnCtrl( PortHandle, newBaud, newData, newParity, newStop )
if rc <> 0
   then say 'RxAsyncSetLnCtrl failed with rc=>'rc'<'
   else say 'RxAsyncSetLnCtrl set the new settings'

/* query new line control settings */

rc = RxAsyncGetLnCtrl( PortHandle, 'newBaud', 'newData', 'newParity', 'newStop' )
if rc <> 0
   then say 'RxAsyncGetLnCtrl failed with rc=>'rc'<'
   else say 'RxAsyncGetLnCtrl queried the new settings'

say 'Baudrate >'newBaud'<'
say 'Databits >'newData'<'
say 'Parity   >'newParity'<'
say 'Stopbits >'newStop'<'

/*=====================================================================*/
/* device control block handling                                       */
/*=====================================================================*/

/* save original dcb info settings */

rc = RxAsyncGetDcbInfo( PortHandle, 'old.WriteLim', 'old.ReadLim', 'old.Flags1', 'old.Flags2', 'old.Flags3', 'old.ErrChar', 'old.BrkChar', 'old.XonChar', 'old.XoffChar' )
if rc <> 0
   then say 'RxAsyncGetDcbInfo failed with rc=>'rc'<'
   else say 'RxAsyncGetDcbInfo saved the original settings'

old.Flags1 = X2B(old.Flags1)
old.Flags2 = X2B(old.Flags2)
old.Flags3 = X2B(old.Flags3)

say 'Write Timeout  >'old.WriteLim'<'
say 'Read  Timeout  >'old.ReadLim'<'
say 'F1 HandShake   >'old.Flags1'<'
say 'F2 FlowReplace >'old.Flags2'<'
say 'F3 Timeout     >'old.Flags3'<'
say 'Error Replace  >'old.ErrChar'<'
say 'Break Replace  >'old.BrkChar'<'
say 'Xon character  >'old.XonChar'<'
say 'Xoff character >'old.XoffChar'<'

/* set new dcb info settings */

rc = RxAsyncSetDcbInfo( PortHandle, newWriteLim, newReadLim, newFlags1, newFlags2, newFlags3, newErrChar, newBrkChar, newXonChar, newXoffChar )
if rc <> 0
   then say 'RxAsyncSetDcbInfo failed with rc=>'rc'<'
   else say 'RxAsyncSetDcbInfo set the new settings'

/* query new dcb info settings */

rc = RxAsyncGetDcbInfo( PortHandle, 'newWriteLim', 'newReadLim', 'newFlags1', 'newFlags2', 'newFlags3', 'newErrChar', 'newBrkChar', 'newXonChar', 'newXoffChar' )
if rc <> 0
   then say 'RxAsyncGetDcbInfo failed with rc=>'rc'<'
   else say 'RxAsyncGetDcbInfo queried the new settings'

newFlags1 = X2B(newFlags1)
newFlags2 = X2B(newFlags2)
newFlags3 = X2B(newFlags3)

say 'Write Timeout  >'newWriteLim'<'
say 'Read  Timeout  >'newReadLim'<'
say 'F1 HandShake   >'newFlags1'<'
say 'F2 FlowReplace >'newFlags2'<'
say 'F3 Timeout     >'newFlags3'<'
say 'Error Replace  >'newErrChar'<'
say 'Break Replace  >'newBrkChar'<'
say 'Xon character  >'newXonChar'<'
say 'Xoff character >'newXoffChar'<'

/*=====================================================================*/
/* enhanced parameter handling                                         */
/*=====================================================================*/

/* save original enhanced parms settings */

rc = RxAsyncGetEnhParm( PortHandle, 'old.EnhParms' )
if rc <> 0
   then say 'RxAsyncGetEnhParm failed with rc=>'rc'<'
   else say 'RxAsyncGetEnhParm saved the original settings'

old.EnhParms = X2B(old.EnhParms)

say 'Enhanced Parms >'old.EnhParms'<'

/* set new enhanced parms settings */

rc = RxAsyncSetEnhParm( PortHandle, newEnhParms )
if rc <> 0
   then say 'RxAsyncSetEnhParm failed with rc=>'rc'<'
   else say 'RxAsyncSetEnhParm set the new settings'

/* query new enhanced parms settings */

rc = RxAsyncGetEnhParm( PortHandle, 'newEnhParms' )
if rc <> 0
   then say 'RxAsyncGetEnhParm failed with rc=>'rc'<'
   else say 'RxAsyncGetEnhParm queried the new settings'

newEnhParms = X2B(newEnhParms)

say 'Enhanced Parms >'newEnhParms'<'

/*=====================================================================*/
/* test read, write and wait functions                                 */
/*=====================================================================*/

/* write modem reset string */

rc = RxAsyncWrite( PortHandle, 0, 'ATZ'||D2C(13), 'Remaining' )
if rc <> 0
   then say 'Modem not turned on, >'Remaining'< chars unwritten'
   else say 'RxAsyncWrite output ATZ<cr>'

/* read modem response */

InpStr = ""
GetOk = True
Tries = 10
do while GetOk
   rc = RxAsyncRead( PortHandle, 0, 3000, 'InpStr' )
   GetOk = False
   select
     when Tries <= 0 then nop
     when rc <> 0                 then say 'RxAsyncRead failed with rc=>'rc'<'
     when InpStr == "OK"||crlf    then say 'RxAsyncRead returned >'InpStr'<'
     when InpStr == "ERROR"||crlf then say 'RxAsyncRead returned >'InpStr'<'
     otherwise
        say 'RxAsyncRead returned >'InpStr'<'
        GetOk = True
        Tries = Tries - 1
   end
end

/* write modem display settings string */

call RxAsyncWrite PortHandle, 0, 'AT&V0'||D2C(13), 'Remaining'
if RxAsyncRc <> 0
   then say 'Modem not turned on, >'Remaining'< chars unwritten'
   else say 'RxAsyncWrite output AT&V0<cr>'
say 'RxAsyncRc =>'RxAsyncRc'<'

/* read the rest */

InpStr = ""
GetOk = True
Tries = 60
do while GetOk
   call RxAsyncRead PortHandle, 0, 3000, 'InpStr'
   GetOk = False
   select
     when Tries <= 0 then nop
     when RxAsyncRc <> 0             then say 'RxAsyncRead failed with rc=>'RxAsyncRc'<'
     when InpStr == "OK"||crlf       then say 'RxAsyncRead returned >'InpStr'<'
     when InpStr == "ERROR"||crlf    then say 'RxAsyncRead returned >'InpStr'<'
     otherwise
        say 'RxAsyncRead returned >'InpStr'<'
        GetOk = True
        Tries = Tries - 1
   end
end
say 'RxAsyncRc =>'RxAsyncRc'<'

/* write modem display settings string */

rc = RxAsyncWrite( PortHandle, 0, 'AT&V0'||D2C(13), 'Remaining' )
if rc <> 0
   then say 'Modem not turned on, >'Remaining'< chars unwritten'
   else say 'RxAsyncWrite output AT&V0<cr>'

/* watch for 'Auto','&L','Fixed' modem response */

MatchStr.0 = 3
MatchStr.1 = '&l'
MatchStr.2 = 'FiXeD'
MatchStr.3 = 'AuTo'
InpStr = ''
NumFound = 0
rc =  RxAsyncWatch( PortHandle, 'MatchStr', 'Y', 3000, 'InpStr', 'NumFound' )
if rc = 0
   then do
          say 'RxAsyncWatch returned >'InpStr'<'
          say 'Index of string found was >'NumFound'<'
        end
   else if rc = -1
           then say 'RxAsyncWatch timed out'
           else say 'RxAsyncWatch failed rc=>'rc'<'

/* read the rest */

InpStr = ""
GetOk = True
Tries = 60
do while GetOk
   rc = RxAsyncRead( PortHandle, 0, 3000, 'InpStr' )
   GetOk = False
   select
     when Tries <= 0 then nop
     when rc <> 0                    then say 'RxAsyncRead failed with rc=>'rc'<'
     when InpStr == "OK"||crlf       then say 'RxAsyncRead returned >'InpStr'<'
     when InpStr == "ERROR"||crlf    then say 'RxAsyncRead returned >'InpStr'<'
     otherwise
        say 'RxAsyncRead returned >'InpStr'<'
        GetOk = True
        Tries = Tries - 1
   end
end

/* write AT<cr> string */

rc = RxAsyncWrite( PortHandle, 0, 'AT'D2C(13), 'Remaining' )
if rc <> 0
   then say 'Modem not turned on, >'Remaining'< chars unwritten'
   else say 'RxAsyncWrite output AT<cr>'

/* read modem response */

InpStr = ""
GetOk = True
Tries = 10
do while GetOk
   rc = RxAsyncRead( PortHandle, 0, 3000, 'InpStr', , d2c(13) )
   GetOk = False
   select
     when Tries <= 0 then nop
     when rc <> 0                    then say 'RxAsyncRead failed with rc=>'rc'<'
     when InpStr == "OK"||d2c(13)    then say 'RxAsyncRead returned >'InpStr'<'
     when InpStr == "ERROR"||d2c(13) then say 'RxAsyncRead returned >'InpStr'<'
     otherwise
        say 'RxAsyncRead returned >'InpStr'<'
        GetOk = True
        Tries = Tries - 1
   end
end

/* consume following linefeed */

InpStr = ""
GetOk = True
Tries = 10
do while GetOk
   rc = RxAsyncRead( PortHandle, 0, 3000, 'InpStr' )
   GetOk = False
   select
     when Tries <= 0 then nop
     when rc <> 0              then say 'RxAsyncRead failed with rc=>'rc'<'
     when InpStr == d2c(10)    then say 'RxAsyncRead returned >'InpStr'<'
     otherwise
        say 'RxAsyncRead returned >'InpStr'<'
        GetOk = True
        Tries = Tries - 1
   end
end

/* write bad string */

rc = RxAsyncWrite( PortHandle, 0, 'AT CAUSE AN ERROR'||D2C(13), 'Remaining' )
if rc <> 0
   then say 'Modem not turned on, >'Remaining'< chars unwritten'
   else say 'RxAsyncWrite output AT CAUSE AN ERROR<cr>'

/* read modem response */

InpStr = ""
GetOk = True
Tries = 10
do while GetOk
   rc = RxAsyncRead( PortHandle, 0, 3000, 'InpStr' )
   GetOk = False
   select
     when Tries <= 0 then nop
     when rc <> 0                 then say 'RxAsyncRead failed with rc=>'rc'<'
     when InpStr == "OK"||crlf    then say 'RxAsyncRead returned >'InpStr'<'
     when InpStr == "ERROR"||crlf then say 'RxAsyncRead returned >'InpStr'<'
     otherwise
        say 'RxAsyncRead returned >'InpStr'<'
        GetOk = True
        Tries = Tries - 1
   end
end

/* write a query string */

rc = RxAsyncWrite( PortHandle, 0, 'ATS7?'||D2C(13), 'Remaining' )
if rc <> 0
   then say 'Modem not turned on, >'Remaining'< chars unwritten'
   else say 'RxAsyncWrite output ATS7?<cr>'

/* read modem response */

InpStr = ""
GetOk = True
Tries = 10
do while GetOk
   rc = RxAsyncRead( PortHandle, 0, 3000, 'InpStr' )
   GetOk = False
   select
     when Tries <= 0 then nop
     when rc <> 0                 then say 'RxAsyncRead failed with rc=>'rc'<'
     when InpStr == "OK"||crlf    then say 'RxAsyncRead returned >'InpStr'<'
     when InpStr == "ERROR"||crlf then say 'RxAsyncRead returned >'InpStr'<'
     otherwise
        say 'RxAsyncRead returned >'InpStr'<'
        GetOk = True
        Tries = Tries - 1
   end
end

/* write modem attention string */

rc = RxAsyncWrite( PortHandle, 0, 'AT'D2C(13), 'Remaining' )
if rc <> 0
   then say 'Modem may not be turned on, >'Remaining'< chars unwritten'
   else say 'RxAsyncWrite output AT<cr>'

/* wait three seconds or until there is data to be read */

say 'Wait for data 'time()
rc = RxAsyncWait( PortHandle, 3000 )
say 'Waited enough 'time()
if rc = 0
   then say 'RxAsyncWait detected input'
   else if rc = -1
           then say 'RxAsyncWait timed out'
           else say 'RxAsyncWait returned rc=>'rc'<'

/* wait for carrier signal */

say 'Wait for carrier'
rc = RxAsyncCarrier( PortHandle, 3000 )
say 'Waited enough'
if rc = 0
   then say 'RxAsyncCarrier detected DCD'
   else if rc = -1
           then say 'RxAsyncCarrier timed out'
           else say 'RxAsyncCarrier returned rc=>'rc'<'

/* test for carrier signal */

say 'Test for carrier'
rc = RxAsyncCarrier( PortHandle )
if rc = 0
   then say 'RxAsyncCarrier detected DCD'
   else if rc = -2
           then say 'RxAsyncCarrier did not detect DCD'
           else say 'RxAsyncCarrier returned rc=>'rc'<'

/* read modem response */

InpStr = ""
GetOk = True
Tries = 10
do while GetOk
   rc = RxAsyncRead( PortHandle, 0, 3000, 'InpStr' )
   GetOk = False
   select
     when Tries <= 0 then nop
     when rc <> 0                 then say 'RxAsyncRead failed with rc=>'rc'<'
     when InpStr == "OK"||crlf    then say 'RxAsyncRead returned >'InpStr'<'
     when InpStr == "ERROR"||crlf then say 'RxAsyncRead returned >'InpStr'<'
     otherwise
        say 'RxAsyncRead returned >'InpStr'<'
        GetOk = True
        Tries = Tries - 1
   end
end

/* write query string */

rc = RxAsyncWrite( PortHandle, 0, 'ATS1?'||D2C(13), 'Remaining' )
if rc <> 0
   then say 'Modem not turned on, >'Remaining'< chars unwritten'
   else say 'RxAsyncWrite output ATS1?<cr>'

/* read modem response */

InpStr = ""
GetOk = True
Tries = 10
do while GetOk
   rc = RxAsyncRead( PortHandle, 0, 3000, 'InpStr' )
   GetOk = False
   select
     when Tries <= 0 then nop
     when rc <> 0                 then say 'RxAsyncRead failed with rc=>'rc'<'
     when InpStr == "OK"||crlf    then say 'RxAsyncRead returned >'InpStr'<'
     when InpStr == "ERROR"||crlf then say 'RxAsyncRead returned >'InpStr'<'
     otherwise
        say 'RxAsyncRead returned >'InpStr'<'
        GetOk = True
        Tries = Tries - 1
   end
end

/*=====================================================================*/
/* miscellaneous                                                       */
/*=====================================================================*/

/* send break for half a second */

BreakTime = 500
rc = RxAsyncBreak( PortHandle, BreakTime )
if rc <> 0
   then say 'RxAsyncBreak failed with rc=>'rc'<'
   else say 'RxAsyncBreak sent break for 'BreakTime' milliseconds'

/* print dcb info */

rc = RxAsyncDcbInfo( PortHandle )
if rc <> 0
   then say 'RxAsyncDcbInfo failed with rc=>'rc'<'
   else say 'RxAsyncDcbInfo returned okay'

/* print something on stdout */

call RxAsyncPrint crlf||'Well, how do you like it so far.<g>'||crlf

/* turn DTR off (DTR is normally on once port is open) */

rc = RxAsyncSetMdmOutput( PortHandle, none_on, dtr_off )
if rc <> 0
   then say 'RxAsyncSetMdmOutput failed with rc=>'rc'<'
   else say 'RxAsyncSetMdmOutput disabled DTR'

/* sleep 2 seconds for modem to recognise DTR toggle */

call RxAsyncSleep 2000

/* turn DTR on */

rc = RxAsyncSetMdmOutput( PortHandle, dtr_on, none_off )
if rc <> 0
   then say 'RxAsyncSetMdmOutput failed with rc=>'rc'<'
   else say 'RxAsyncSetMdmOutput enabled DTR'

/* Test if CTS active */

if RxAsyncActive( PortHandle, "CTS" )
   then say 'RxAsyncActive said CTS was active'
   else say 'RxAsyncActive said CTS was NOT active'

/* DTR toggle a different way */

rc = RxAsyncLower( PortHandle, "DTR" )
if rc <> 0
   then say 'RxAsyncLower failed with rc=>'rc'<'
   else say 'RxAsyncLower dropped DTR'

rc = RxAsyncRaise( PortHandle, "DTR" )
if rc <> 0
   then say 'RxAsyncRaise failed with rc=>'rc'<'
   else say 'RxAsyncRaise enabled DTR'

/*=====================================================================*/
/* test file and directory handling                                    */
/*=====================================================================*/

fdir = RxAsyncDirectory('.')
fnam = fdir||'RXASYNC'

if RxAsyncFileExists( fnam'.cmd' )
   then say 'File 'fnam'.cmd was found'
   else say 'File 'fnam'.cmd was not found'

if RxAsyncFileCopy( fnam'.cmd', fnam'.xxx' )
   then say 'File 'fnam'.cmd copied to 'fnam'.xxx'
   else say 'File 'fnam'.cmd not copied to 'fnam'.xxx'

if RxAsyncFileMove( fnam'.xxx', fnam'.yyy' )
   then say 'File 'fnam'.xxx was moved to 'fnam'.yyy'
   else say 'File 'fnam'.xxx was not moved to 'fnam'.yyy'

if RxAsyncFileRename( fnam'.yyy', fnam'.xxx' )
   then say 'File 'fnam'.yyy was renamed to 'fnam'.xxx'
   else say 'File 'fnam'.yyy was not renamed to 'fnam'.xxx'

if RxAsyncFileDelete( fnam'.xxx' )
   then say 'File 'fnam'.xxx was deleted'
   else say 'File 'fnam'.xxx was not deleted'

say 'Path is 'RxAsyncFilePathIs( fnam'.cmd' )

say 'Name is 'RxAsyncFileNameIs( fnam'.cmd' )

/*=====================================================================*/
/* reset changes made to port settings                                 */
/*=====================================================================*/

/* reset to original enhanced parms settings */

rc = RxAsyncSetEnhParm( PortHandle, old.EnhParms )
if rc <> 0
   then say 'RxAsyncSetEnhParm failed with rc=>'rc'<'
   else say 'RxAsyncSetEnhParm restored the original settings'

/* reset to original dcb info settings */

rc = RxAsyncSetDcbInfo( PortHandle, old.WriteLim, old.ReadLim, old.Flags1, old.Flags2, old.Flags3, old.ErrChar, old.BrkChar, old.XonChar, old.XoffChar )
if rc <> 0
   then say 'RxAsyncSetDcbInfo failed with rc=>'rc'<'
   else say 'RxAsyncSetDcbInfo restored the original settings'

/* reset to original line control settings */

rc = RxAsyncSetLnCtrl( PortHandle, old.Baud, old.Data, old.Parity, old.Stop )
if rc <> 0
   then say 'RxAsyncSetLnCtrl failed with rc=>'rc'<'
   else say 'RxAsyncSetLnCtrl restored the original settings'

/*=====================================================================*/
/* termination                                                         */
/*=====================================================================*/

/* close the device */

rc = RxAsyncClose( PortHandle )
if rc <> 0
   then say 'RxAsyncClose failed with rc=>'rc'<'
   else say 'RxAsyncClose closed the device'

/* reset session priority */

rc = RxAsyncPriority( Regular, NormalLvl )
if rc <> 0
   then say 'RxAsyncPriority failed with rc=>'rc'<'
   else say 'RxAsyncPriority set, Class=>'Regular'<, Level=>'NormalLvl'<'

/*=====================================================================*/
/* cleanup                                                             */
/*=====================================================================*/

call RxAsyncDropFuncs
Say 'RxAsyncDropFuncs is complete'
exit

/*=====================================================================*/
/* just in case                                                        */
/*=====================================================================*/

error:
   call RxAsyncClose PortHandle
   call RxAsyncDropFuncs
   exit

