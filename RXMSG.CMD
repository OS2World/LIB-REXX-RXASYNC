/*=====================================================================*/
/* RXMSG.cmd - Test if RXASYNC.dll message boxes work                  */
/*                                                                     */
/* Note: You will need to use the following OS/2 START command         */
/*                                                                     */
/*       START "RxMsg.Cmd Test" /PGM /PM "cmd.exe" /C RxMsg.Cmd        */
/*                                                                     */
/*=====================================================================*/

call on error

/*=====================================================================*/
/* setup                                                               */
/*=====================================================================*/

call RxFuncAdd 'RxAsyncLoadFuncs', 'RXASYNC', 'RxAsyncLoadFuncs'
call RxAsyncLoadFuncs
Say 'RxAsyncLoadFuncs is complete.'

/*=====================================================================*/
/* display pm message boxes                                            */
/*=====================================================================*/

rc = RxAsyncMsgBox('Testing 1 2 3','I','I',,,'Information with title. Vanilla returned '||rc )
if rc <> 0
   then say 'RxAsyncMsgBox failed with rc=>'rc'<'
   else say 'RxAsyncMsgBox completed okay.'

rc = RxAsyncMsgBox('','Q','Q','Y',,'Continue with no title. Information returned '||rc )
if rc <> 0
   then say 'RxAsyncMsgBox failed with rc=>'rc'<'
   else say 'RxAsyncMsgBox completed okay.'

rc = RxAsyncMsgBox('Testing 1 2 3','W','W',,,'Warning with title. Continue returned '||rc )
if rc <> 0
   then say 'RxAsyncMsgBox failed with rc=>'rc'<'
   else say 'RxAsyncMsgBox completed okay.'

rc = RxAsyncMsgBox('Testing 1 2 3','S','S',,,'Error with title. Warning returned '||rc )
if rc <> 0
   then say 'RxAsyncMsgBox failed with rc=>'rc'<'
   else say 'RxAsyncMsgBox completed okay.'

rc = RxAsyncMsgBox('Testing 1 2 3','W','R',,,'Retry with title. Error returned '||rc )
if rc <> 0
   then say 'RxAsyncMsgBox failed with rc=>'rc'<'
   else say 'RxAsyncMsgBox completed okay.'

rc = RxAsyncMsgBox('Testing 1 2 3','S','A',,,'Abort with title. Retry returned '||rc )
if rc <> 0
   then say 'RxAsyncMsgBox failed with rc=>'rc'<'
   else say 'RxAsyncMsgBox completed okay.'

rc = RxAsyncMsgBox('Testing 1 2 3','I','I',,,'Information with title. Abort returned '||rc )
if rc <> 0
   then say 'RxAsyncMsgBox failed with rc=>'rc'<'
   else say 'RxAsyncMsgBox completed okay.'

/*=====================================================================*/
/* cleanup                                                             */
/*=====================================================================*/

call RxAsyncDropFuncs
Say 'RxAsyncDropFuncs is complete.'
exit

/*=====================================================================*/
/* just in case                                                        */
/*=====================================================================*/

error:
   call RxAsyncDropFuncs
   exit

