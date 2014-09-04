/* RXASYNC.cmd - Test if RXASYNC.dll works   */

call on error

Regular    = 2
Critical   = 3
NormalLvl  = 0
PortName   = 'COM4'
PortHandle = ''
OldBaud    = 0
OldData    = 0
OldParity  = ''
OldStop    = 0
NewBaud    = 0
NewData    = 0
NewParity  = ''
NewStop    = 0
InpStr     = ''
Remaining  = 0
WriteLim   = 0
ReadLim    = 0
Flags1     = ''
Flags2     = ''
Flags3     = ''
ErrChar    = ''
BrkChar    = ''
XonChar    = ''
XoffChar   = ''
EnhParms   = ''
True       = 1
False      = 0
crlf       = D2C(13)''D2C(10)

say '====================================================='

/* setup */

call RxFuncAdd 'RxAsyncLoadFuncs', 'RXASYNC', 'RxAsyncLoadFuncs'
call RxAsyncLoadFuncs
Say 'RxAsyncLoadFuncs is complete.'

/* set session priority */

rc = RxAsyncPriority( Critical, NormalLvl )
if rc <> 0
   then say 'RxAsyncPriority failed with rc=>'rc'<'
   else say 'RxAsyncPriority set, Class=>'Critical'<, Level=>'NormalLvl'<'

/*=====================================================================*/

/* open device */

rc = RxAsyncOpen( PortName, 'PortHandle' )
if rc <> 0
   then say 'RxAsyncOpen failed with rc=>'rc'<'
   else say 'RxAsyncOpen opened the device handle=>'PortHandle'<'

/* get dcb info */

rc = RxAsyncGetDcbInfo( PortHandle, 'WriteLim', 'ReadLim', 'Flags1', 'Flags2', 'Flags3', 'ErrChar', 'BrkChar', 'XonChar', 'XoffChar' )
if rc <> 0
   then say 'RxAsyncGetDcbInfo failed with rc=>'rc'<'
   else say 'RxAsyncGetDcbInfo returned okay.'

say 'Write Timeout  was >'WriteLim'<'
say 'Read  Timeout  was >'ReadLim'<'
say 'F1 HandShake   was >'X2B(Flags1)'<'
say 'F2 FlowReplace was >'X2B(Flags2)'<'
say 'F3 Timeout     was >'X2B(Flags3)'<'
say 'Error Replace  was >'ErrChar'<'
say 'Break Replace  was >'BrkChar'<'
say 'Xon character  was >'XonChar'<'
say 'Xoff character was >'XoffChar'<'

rc = RxAsyncSetDcbInfo( PortHandle, 50,  50, '00001001', '10100000', '11010010', '00', '00', '11', '13' )
/* rc = RxAsyncSetDcbInfo( PortHandle,  ,  , '01001001', '10100000', '11010010', '', '', '11', '13' ) */
if rc <> 0
   then say 'RxAsyncSetDcbInfo failed with rc=>'rc'<'
   else say 'RxAsyncSetDcbInfo returned okay.'

rc = RxAsyncDcbInfo( PortHandle )
if rc <> 0
   then say 'RxAsyncDcbInfo failed with rc=>'rc'<'
   else say 'RxAsyncDcbInfo returned okay.'

/* get enhanced parms */

rc = RxAsyncGetEnhParm( PortHandle, 'EnhParms' )
if rc <> 0
   then say 'RxAsyncGetEnhParm failed with rc=>'rc'<'
   else say 'RxAsyncGetEnhParm returned okay.'

say 'Enhanced Parms was >'X2B(EnhParms)'<'

/* get original settings */

rc = RxAsyncGetLnCtrl( PortHandle, 'OldBaud', 'OldData', 'OldParity', 'OldStop' )
if rc <> 0
   then say 'RxAsyncGetLnCtrl failed with rc=>'rc'<'
   else say 'RxAsyncGetLnCtrl saved the original settings'

say 'Baudrate was >'OldBaud'<'
say 'Databits was >'OldData'<'
say 'Parity   was >'OldParity'<'
say 'Stopbits was >'OldStop'<'

/* set required settings */

rc = RxAsyncSetLnCtrl( PortHandle, 19200, 8, 'N', 1 )
if rc <> 0
   then say 'RxAsyncSetLnCtrl failed with rc=>'rc'<'
   else say 'RxAsyncSetLnCtrl has set the new settings'

/* get new settings */

rc = RxAsyncGetLnCtrl( PortHandle, 'NewBaud', 'NewData', 'NewParity', 'NewStop' )
if rc <> 0
   then say 'RxAsyncGetLnCtrl failed with rc=>'rc'<'
   else say 'RxAsyncGetLnCtrl queried the new settings'

say 'Baudrate was >'NewBaud'<'
say 'Databits was >'NewData'<'
say 'Parity   was >'NewParity'<'
say 'Stopbits was >'NewStop'<'

/* write modem reset string */

rc = RxAsyncWrite( PortHandle, 0, 'ATZ'||D2C(13), 'Remaining' )
if rc <> 0
   then say 'Modem not turned on, >'Remaining'< chars unwritten'
   else say 'RxAsyncWrite output ATZ<cr>'

/* read modem response */

InpStr = ""
GetOk = True
do while GetOk
   rc = RxAsyncRead( PortHandle, 0, 3000, 'InpStr' )
   GetOk = False
   select
     when rc <> 0                 then say 'RxAsyncRead failed with rc=>'rc'<'
     when InpStr == "OK"||crlf    then say 'RxAsyncRead returned >'InpStr'<'
     when InpStr == "ERROR"||crlf then say 'RxAsyncRead returned >'InpStr'<'
     otherwise
        say 'RxAsyncRead returned >'InpStr'<'
        GetOk = True
   end
end

/* write modem reset string */

rc = RxAsyncWrite( PortHandle, 0, 'AT'D2C(13), 'Remaining' )
if rc <> 0
   then say 'Modem not turned on, >'Remaining'< chars unwritten'
   else say 'RxAsyncWrite output AT<cr>'

/* read modem response */

InpStr = ""
GetOk = True
do while GetOk
   rc = RxAsyncRead( PortHandle, 0, 3000, 'InpStr' )
   GetOk = False
   select
     when rc <> 0                 then say 'RxAsyncRead failed with rc=>'rc'<'
     when InpStr == "OK"||crlf    then say 'RxAsyncRead returned >'InpStr'<'
     when InpStr == "ERROR"||crlf then say 'RxAsyncRead returned >'InpStr'<'
     otherwise
        say 'RxAsyncRead returned >'InpStr'<'
        GetOk = True
   end
end

/* write modem reset string */

rc = RxAsyncWrite( PortHandle, 0, 'AT CAUSE AN ERROR'||D2C(13), 'Remaining' )
if rc <> 0
   then say 'Modem not turned on, >'Remaining'< chars unwritten'
   else say 'RxAsyncWrite output AT CAUSE AN ERROR<cr>'

/* read modem response */

InpStr = ""
GetOk = True
do while GetOk
   rc = RxAsyncRead( PortHandle, 0, 3000, 'InpStr' )
   GetOk = False
   select
     when rc <> 0                 then say 'RxAsyncRead failed with rc=>'rc'<'
     when InpStr == "OK"||crlf    then say 'RxAsyncRead returned >'InpStr'<'
     when InpStr == "ERROR"||crlf then say 'RxAsyncRead returned >'InpStr'<'
     otherwise
        say 'RxAsyncRead returned >'InpStr'<'
        GetOk = True
   end
end

/* write modem reset string */

rc = RxAsyncWrite( PortHandle, 0, 'ATS7?'||D2C(13), 'Remaining' )
if rc <> 0
   then say 'Modem not turned on, >'Remaining'< chars unwritten'
   else say 'RxAsyncWrite output ATS7?<cr>'

/* read modem response */

InpStr = ""
GetOk = True
do while GetOk
   rc = RxAsyncRead( PortHandle, 0, 3000, 'InpStr' )
   GetOk = False
   select
     when rc <> 0                 then say 'RxAsyncRead failed with rc=>'rc'<'
     when InpStr == "OK"||crlf    then say 'RxAsyncRead returned >'InpStr'<'
     when InpStr == "ERROR"||crlf then say 'RxAsyncRead returned >'InpStr'<'
     otherwise
        say 'RxAsyncRead returned >'InpStr'<'
        GetOk = True
   end
end

/* write modem attention string */

rc = RxAsyncWrite( PortHandle, 0, 'AT'D2C(13), 'Remaining' )
if rc <> 0
   then say 'Modem may not be turned on, >'Remaining'< chars unwritten'
   else say 'RxAsyncWrite output AT<cr>'

/* wait a second or until there is data to be read */

say 'Wait for data 'time()
rc = RxAsyncWait( PortHandle, 3000 )
say 'Waited enough 'time()
if rc = 0
   then say 'RxAsyncWait detected input'
   else if rc = -1
           then say 'RxAsyncWait timed out'
           else say 'RxAsyncWait returned rc=>'rc'<'

say 'Wait for carrier'
rc = RxAsyncCarrier( PortHandle, 3000 )
say 'Waited enough'
if rc = 0
   then say 'RxAsyncCarrier detected DCD'
   else if rc = -1
           then say 'RxAsyncCarrier timed out'
           else say 'RxAsyncCarrier returned rc=>'rc'<'

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
do while GetOk
   rc = RxAsyncRead( PortHandle, 0, 3000, 'InpStr' )
   GetOk = False
   select
     when rc <> 0                 then say 'RxAsyncRead failed with rc=>'rc'<'
     when InpStr == "OK"||crlf    then say 'RxAsyncRead returned >'InpStr'<'
     when InpStr == "ERROR"||crlf then say 'RxAsyncRead returned >'InpStr'<'
     otherwise
        say 'RxAsyncRead returned >'InpStr'<'
        GetOk = True
   end
end

/* write modem reset string */

rc = RxAsyncWrite( PortHandle, 0, 'ATS1?'||D2C(13), 'Remaining' )
if rc <> 0
   then say 'Modem not turned on, >'Remaining'< chars unwritten'
   else say 'RxAsyncWrite output ATS1?<cr>'

/* read modem response */

InpStr = ""
GetOk = True
do while GetOk
   rc = RxAsyncRead( PortHandle, 0, 3000, 'InpStr' )
   GetOk = False
   select
     when rc <> 0                 then say 'RxAsyncRead failed with rc=>'rc'<'
     when InpStr == "OK"||crlf    then say 'RxAsyncRead returned >'InpStr'<'
     when InpStr == "ERROR"||crlf then say 'RxAsyncRead returned >'InpStr'<'
     otherwise
        say 'RxAsyncRead returned >'InpStr'<'
        GetOk = True
   end
end

/* send a break */

BreakTime = 500
rc = RxAsyncBreak( PortHandle, BreakTime )
if rc <> 0
   then say 'RxAsyncBreak failed with rc=>'rc'<'
   else say 'RxAsyncBreak sent break for 'BreakTime' milliseconds'

/* reset to original settings */

rc = RxAsyncSetLnCtrl( PortHandle, OldBaud, OldData, OldParity, OldStop )
if rc <> 0
   then say 'RxAsyncSetLnCtrl failed with rc=>'rc'<'
   else say 'RxAsyncSetLnCtrl restored the original settings'

/* close the device */

rc = RxAsyncClose( PortHandle )
if rc <> 0
   then say 'RxAsyncClose failed with rc=>'rc'<'
   else say 'RxAsyncClose closed the device'

/*=====================================================================*/

/* reset session priority */

rc = RxAsyncPriority( Regular, NormalLvl )
if rc <> 0
   then say 'RxAsyncPriority failed with rc=>'rc'<'
   else say 'RxAsyncPriority set, Class=>'Regular'<, Level=>'NormalLvl'<'

/* cleanup */

call RxAsyncDropFuncs
Say 'RxAsyncDropFuncs is complete.'

say '====================================================='

exit

error:
   call RxAsyncClose PortHandle
   exit

