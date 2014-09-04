/*****************************************************************************/

call on error

/*--------------------------------------------------------------*/
/* NOTE: Adjust the following as required by your configuration */
/*       If you do NOT have a 16550 UART then set newFlags3     */
/*       to '00000010'. (see the RXASYNC.c & .h for more info.) */
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

/*--------------------------------------------------------------*/

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

/*--------------------------------------------------------------*/

/*-----------------------------*/
/* Setup                       */
/*-----------------------------*/

/* load functions */

call RxFuncAdd 'RxAsyncLoadFuncs', 'RXASYNC', 'RxAsyncLoadFuncs'

Say 'Modem Reset in progress'

rc = RxAsyncLoadFuncs()
if rc <> 0 then do
   say 'RxAsyncLoadFuncs failed with rc=>'rc'<'
   exit
end

/* set session priority */

rc = RxAsyncPriority( Critical, NormalLvl )
if rc <> 0 then say 'RxAsyncPriority failed with rc=>'rc'<'

/* open device */

rc = RxAsyncOpen( PortName, 'PortHandle' )
if rc <> 0 then say 'RxAsyncOpen failed with rc=>'rc'<'

say 'Port >'PortName'<'

/*-----------------------*/
/* Line Control Handling */
/*-----------------------*/

/* save original line control settings */

rc = RxAsyncGetLnCtrl( PortHandle, 'old.Baud', 'old.Data', 'old.Parity', 'old.Stop' )
if rc <> 0 then say 'RxAsyncGetLnCtrl failed with rc=>'rc'<'

/* set new line control settings */

rc = RxAsyncSetLnCtrl( PortHandle, newBaud, newData, newParity, newStop )
if rc <> 0 then say 'RxAsyncSetLnCtrl failed with rc=>'rc'<'

/* query new line control settings */

rc = RxAsyncGetLnCtrl( PortHandle, 'newBaud', 'newData', 'newParity', 'newStop' )
if rc <> 0 then say 'RxAsyncGetLnCtrl failed with rc=>'rc'<'

say 'Baudrate >'newBaud'<'
say 'Databits >'newData'<'
say 'Parity   >'newParity'<'
say 'Stopbits >'newStop'<'

/*-------------------------------*/
/* Device Control Block Handling */
/*-------------------------------*/

/* save original dcb info settings */

rc = RxAsyncGetDcbInfo( PortHandle, 'old.WriteLim', 'old.ReadLim', 'old.Flags1', 'old.Flags2', 'old.Flags3', 'old.ErrChar', 'old.BrkChar', 'old.XonChar', 'old.XoffChar' )
if rc <> 0 then say 'RxAsyncGetDcbInfo failed with rc=>'rc'<'

old.Flags1 = X2B(old.Flags1)
old.Flags2 = X2B(old.Flags2)
old.Flags3 = X2B(old.Flags3)

/* set new dcb info settings */

rc = RxAsyncSetDcbInfo( PortHandle, newWriteLim, newReadLim, newFlags1, newFlags2, newFlags3, newErrChar, newBrkChar, newXonChar, newXoffChar )
if rc <> 0 then say 'RxAsyncSetDcbInfo failed with rc=>'rc'<'

/* query new dcb info settings */

rc = RxAsyncGetDcbInfo( PortHandle, 'newWriteLim', 'newReadLim', 'newFlags1', 'newFlags2', 'newFlags3', 'newErrChar', 'newBrkChar', 'newXonChar', 'newXoffChar' )
if rc <> 0 then say 'RxAsyncGetDcbInfo failed with rc=>'rc'<'

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

/*-----------------------------*/
/* Enhanced Parameter Handling */
/*-----------------------------*/

/* save original enhanced parms settings */

rc = RxAsyncGetEnhParm( PortHandle, 'old.EnhParms' )
if rc <> 0 then say 'RxAsyncGetEnhParm failed with rc=>'rc'<'

old.EnhParms = X2B(old.EnhParms)

/* set new enhanced parms settings */

rc = RxAsyncSetEnhParm( PortHandle, newEnhParms )
if rc <> 0 then say 'RxAsyncSetEnhParm failed with rc=>'rc'<'

/* query new enhanced parms settings */

rc = RxAsyncGetEnhParm( PortHandle, 'newEnhParms' )
if rc <> 0 then say 'RxAsyncGetEnhParm failed with rc=>'rc'<'

newEnhParms = X2B(newEnhParms)

say 'Enhanced Parms >'newEnhParms'<'

/*----------------------*/
/* Read, Write and Wait */
/*----------------------*/

/* write modem string */

rc = RxAsyncWrite( PortHandle, 0, 'ATT'||D2C(13), 'Remaining' )
if rc <> 0
   then say 'Modem not turned on, >'Remaining'< chars unwritten'
   else say 'ATT<cr>'

/* read modem response */

InpStr = ""
GetOk = True
do while GetOk
   rc = RxAsyncRead( PortHandle, 0, 3000, 'InpStr' )
   GetOk = False
   select
     when rc <> 0                 then say 'RxAsyncRead failed with rc=>'rc'<'
     when InpStr == "OK"||crlf    then say '>'InpStr'<'
     when InpStr == "ERROR"||crlf then say '>'InpStr'<'
     otherwise
        say '>'InpStr'<'
        GetOk = True
   end
end

/* write modem string */

rc = RxAsyncWrite( PortHandle, 0, 'ATS0=0'||D2C(13), 'Remaining' )
if rc <> 0
   then say 'Modem not turned on, >'Remaining'< chars unwritten'
   else say 'ATS0=0<cr>'

/* read modem response */

InpStr = ""
GetOk = True
do while GetOk
   rc = RxAsyncRead( PortHandle, 0, 3000, 'InpStr' )
   GetOk = False
   select
     when rc <> 0                 then say 'RxAsyncRead failed with rc=>'rc'<'
     when InpStr == "OK"||crlf    then say '>'InpStr'<'
     when InpStr == "ERROR"||crlf then say '>'InpStr'<'
     otherwise
        say '>'InpStr'<'
        GetOk = True
   end
end

/* write modem string */

rc = RxAsyncWrite( PortHandle, 0, 'ATB0E0F1L0M1Q0V1X6Y0'||D2C(13), 'Remaining' )
if rc <> 0
   then say 'Modem not turned on, >'Remaining'< chars unwritten'
   else say 'ATB0E0F1L0M1Q0V1X6Y0<cr>'

/* read modem response */

InpStr = ""
GetOk = True
do while GetOk
   rc = RxAsyncRead( PortHandle, 0, 3000, 'InpStr' )
   GetOk = False
   select
     when rc <> 0                 then say 'RxAsyncRead failed with rc=>'rc'<'
     when InpStr == "OK"||crlf    then say '>'InpStr'<'
     when InpStr == "ERROR"||crlf then say '>'InpStr'<'
     otherwise
        say '>'InpStr'<'
        GetOk = True
   end
end

/* write modem string */

rc = RxAsyncWrite( PortHandle, 0, 'AT&B0&C1&D2&G0&P1&Q1&R1&S1$E1$F3$S8'||D2C(13), 'Remaining' )
if rc <> 0
   then say 'Modem not turned on, >'Remaining'< chars unwritten'
   else say 'AT&B0&C1&D2&G0&P1&Q1&R1&S1$E1$F3$S8<cr>'

/* read modem response */

InpStr = ""
GetOk = True
do while GetOk
   rc = RxAsyncRead( PortHandle, 0, 3000, 'InpStr' )
   GetOk = False
   select
     when rc <> 0                 then say 'RxAsyncRead failed with rc=>'rc'<'
     when InpStr == "OK"||crlf    then say '>'InpStr'<'
     when InpStr == "ERROR"||crlf then say '>'InpStr'<'
     otherwise
        say '>'InpStr'<'
        GetOk = True
   end
end

/* write modem string */

rc = RxAsyncWrite( PortHandle, 0, 'AT&W0'||D2C(13), 'Remaining' )
if rc <> 0
   then say 'Modem not turned on, >'Remaining'< chars unwritten'
   else say 'AT&W0<cr>'

/* read modem response */

InpStr = ""
GetOk = True
do while GetOk
   rc = RxAsyncRead( PortHandle, 0, 3000, 'InpStr' )
   GetOk = False
   select
     when rc <> 0                 then say 'RxAsyncRead failed with rc=>'rc'<'
     when InpStr == "OK"||crlf    then say '>'InpStr'<'
     when InpStr == "ERROR"||crlf then say '>'InpStr'<'
     otherwise
        say '>'InpStr'<'
        GetOk = True
   end
end

/* write modem string */

rc = RxAsyncWrite( PortHandle, 0, 'AT&W1'D2C(13), 'Remaining' )
if rc <> 0
   then say 'Modem may not be turned on, >'Remaining'< chars unwritten'
   else say 'AT&W1<cr>'

/* read modem response */

InpStr = ""
GetOk = True
do while GetOk
   rc = RxAsyncRead( PortHandle, 0, 3000, 'InpStr' )
   GetOk = False
   select
     when rc <> 0                 then say 'RxAsyncRead failed with rc=>'rc'<'
     when InpStr == "OK"||crlf    then say '>'InpStr'<'
     when InpStr == "ERROR"||crlf then say '>'InpStr'<'
     otherwise
        say '>'InpStr'<'
        GetOk = True
   end
end

/*-----------------------------*/
/* Cleanup                     */
/*-----------------------------*/

/* reset to original enhanced parms settings */

rc = RxAsyncSetEnhParm( PortHandle, old.EnhParms )
if rc <> 0 then say 'RxAsyncSetEnhParm failed with rc=>'rc'<'

/* reset to original dcb info settings */

rc = RxAsyncSetDcbInfo( PortHandle, old.WriteLim, old.ReadLim, old.Flags1, old.Flags2, old.Flags3, old.ErrChar, old.BrkChar, old.XonChar, old.XoffChar )
if rc <> 0 then say 'RxAsyncSetDcbInfo failed with rc=>'rc'<'

/* reset to original line control settings */

rc = RxAsyncSetLnCtrl( PortHandle, old.Baud, old.Data, old.Parity, old.Stop )
if rc <> 0 then say 'RxAsyncSetLnCtrl failed with rc=>'rc'<'

/* close the device */

rc = RxAsyncClose( PortHandle )
if rc <> 0 then say 'RxAsyncClose failed with rc=>'rc'<'

/* reset session priority */

rc = RxAsyncPriority( Regular, NormalLvl )
if rc <> 0 then say 'RxAsyncPriority failed with rc=>'rc'<'

/* drop functions */

rc = RxAsyncDropFuncs()
if rc <> 0 then say 'RxAsyncDropFuncs failed with rc=>'rc'<'

Say 'Modem Reset is complete'

exit

/* just in case<g> */

error:
   call RxAsyncClose PortHandle
   exit

