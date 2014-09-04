/*****************************************************************************/
/*                                                                           */
/*  MODULE         RxScript.Cmd                                              */
/*                                                                           */
/*  DESCRIPTION    Perform script processing.                                */
/*                                                                           */
/*  COPYRIGHT      Copyright (C) 1993 - Crucial Applications                 */
/*                            All rights reserved                            */
/*                                                                           */
/*                       Ian Timms - 20th March 1993                         */
/*                                                                           */
/*  NOTES          The following directory layout is used:                   */
/*                                                                           */
/*                 D:\Mail              Home                                 */
/*                 D:\Mail\Bin          DLLs EXEs CMDs                       */
/*                 D:\Mail\Logs         Trace & Log files                    */
/*                 D:\Mail\Scripts      Script files and FREQ lists          */
/*                 D:\Mail\InBox        Inbound mail packets                 */
/*                 D:\Mail\OutBox       Outbound reply packets               */
/*                 D:\Mail\Messages     Mail packets being read              */
/*                 D:\Mail\Replies      Reply packets being built            */
/*                 D:\Mail\SentMail     Processed reply packets              */
/*                                                                           */
/*                 RXASYNC.dll must be located in a directory on the         */
/*                 LIBPATH statement when this utility is invoked or         */
/*                 present in the Bin subdirectory as detailed above.        */
/*                                                                           */
/*  HISTORY                                                                  */
/*                                                                           */
/*    10-Jun-1993  Original release.                                         */
/*                                                                           */
/*  FUTURES        Convert to Exe for faster processing and                  */
/*                 better handling of scripts and coms device.               */
/*                 Add ability to handle mail via CIS. ??? lota work ???     */
/*                                                                           */
/*****************************************************************************/
/* trace('R') */

   parse arg Tparms

   G.   = ""                           /* Globals                            */
   Dev. = ""                           /* Communications device              */
   Mdm. = ""                           /* Modem definition                   */
   Bbs. = ""                           /* Service definition                 */

   G.Parms    = Tparms

   call Startup
   if ComConnect( Bbs.Name, Bbs.Phone, Bbs.RetryLimit, Bbs.RetryWait ) then do
      Bbs.Online = time("E")*1000
      call SayLog "Script "Bbs.Script" commenced on "date('N')" at "time('C')"."G.CrLf

      /* The script routines have to be internal as REXX can't pass variables to */
      /* an external command file so we just check that the name is correct.     */
      select
        when pos( 'BUNMAIL', translate( Bbs.Script ) ) > 0 then call BunScript
        otherwise nop
      end

      call SayLog "Script "Bbs.Script" completed on "date('N')" at "time('C')"."G.CrLf
      call SayLog 'Time online was 'trunc(Bbs.Elapsed/60000)' mins 'trunc((Bbs.Elapsed/10)-(trunc(Bbs.Elapsed/60000)*6000))/100' secs.'G.CrLf
      if ComCarrier() then do
         call ComHangup
      end
   end
   call Cleanup

Exit

/*****************************************************************************/
/* BUNYIP SCRIPTS ########################################################## */
/*****************************************************************************/

BunScript:
   Procedure expose G. Dev. Mdm. Bbs.
   Bbs.Connected = G.True
   if BunLogon() then do
      if \G.FreqOnly then do
         call BunEmail
         /* Get VioDevGrp Next Meeting Notice  */
         if date('W') == 'Monday' then do
            call BunNotice
         end
      end
      if \G.EmailOnly then do
         call BunFiles
      end
      call BunLogoff
   end
Return

BunLogon:
   Procedure expose G. Dev. Mdm. Bbs.
   Tlogon = G.False
   Tagain = G.True
   do while Tagain  &  Bbs.Connected
      Tstr = BbsRead()
      select
        when \Bbs.Connected then nop
        when pos( "Sorry, we're not available right now.",    Tstr ) > 0 then Tagain = G.False
        when pos( "Please press your Escape key to enter",    Tstr ) > 0 then call BbsWrite 0, G.Esc||G.Esc
        when pos( "What is your name:",                       Tstr ) > 0 then call BbsWrite 0, Bbs.Userid||G.CtrlM
        when pos( "Your name was not found",                  Tstr ) > 0 then call BbsWrite 2, 'N'
        when pos( Bbs.Userid" [Y,n]?",                        Tstr ) > 0 then call BbsWrite 0, G.CtrlM
        when pos( "Password:",                                Tstr ) > 0 then call BbsWrite 0, Bbs.Password||G.CtrlM
        when pos( "Select Bulletin to view",                  Tstr ) > 0 then call BbsWrite 0, 'B'
        when pos( "Next bulletin [Y,n]",                      Tstr ) > 0 then call BbsWrite 0, 'N'
        when pos( "Do you wish to check for mail",            Tstr ) > 0 then call BbsWrite 0, 'N'
        when pos( "CHAT: start",                              Tstr ) > 0 then call BbsChat
        when pos( "Select:",                                  Tstr ) > 0 then do
                Tagain = G.False
                Tlogon = G.True
             end
        otherwise nop
      end
   end
Return Tlogon

BunEmail:
   Procedure expose G. Dev. Mdm. Bbs.
   /* Assume we are at the MAIN menu */
   call BbsWrite 0, 'O'                /* Select offline reader              */
   call BbsWaitFor "Select:", 0, 30
   call BunPutMail                     /* Upload any replies from the RepBox */
   call BunGetMail                     /* Download new mail into the PktBox  */
   call BbsWrite 0, 'M'                /* Return to main menu                */
   call BbsWaitFor "Select:", 0, 30
Return

BunPutMail:
   Procedure expose G. Dev. Mdm. Bbs.
   Treplnew = G.RepBox||Bbs.Prefix||".REP"
   Treplold = Bbs.Prefix||".OLD"
   if Bbs.Connected then do
      if \RxAsyncFileExists( Bbs.ReplyCheck ) then do
         if RxAsyncFileExists( Treplnew ) then do
            if RxAsyncFileExists( G.RepBox||Treplold ) then
               call SayLog "Unable to upload replies, untossed packet exists."G.CrLf
            else do
               call BbsWrite 0, 'U'
               if \BbsWaitFor( Mdm.AutoUpLoad, 0, 30 ) then
                  call BbsWrite 0, G.CtrlM
               else do
                  if \SendFile( 'Z', Bbs.Baud, Treplnew ) then
                     call BbsWrite 0, G.CtrlM
                  else do
                     call SayLog "Replies have been posted."G.CrLf
                     if \TossPacket( Bbs.Prefix, "REP", G.RepBox, G.SntBox ) then do
                        call RxAsyncFileRename Treplnew Treplold
                     end
                  end
               end
               call BbsWaitFor "Select:", 0, 30
            end
         end
      end
   end
Return

BunGetMail:
   Procedure expose G. Dev. Mdm. Bbs.
   Tmailpkt = G.InBox||Bbs.Prefix||".QWK"
   if RxAsyncFileExists( Tmailpkt ) then do
      call TossPacket Bbs.Prefix, "QWK", G.InBox, G.PktBox
   end
   if Bbs.Connected then do
      if RxAsyncFileExists( Tmailpkt ) then
         call SayLog "Unable to download new mail, untossed packet exists."G.CrLf
      else do
         call BbsWrite 0, 'D'
         Tmail = G.False
         Tmore = G.True
         do while Tmore  &  Bbs.Connected
            Tmore = G.False
            Tstr = BbsRead()
            select
              when \Bbs.Connected then nop
              when pos( "CHAT: start",                          Tstr ) > 0 then do
                      call BbsChat
                      call BbsWaitFor "Select:", 0, 30
                      call BbsWrite 0, 'D'
                      Tmore = G.True
                   end
              when pos( "Download these in QWK format",         Tstr ) > 0 then do
                      call BbsWrite 0, G.CtrlM
                      Tmore = G.True
                   end
              when pos( "Error compressing messages.",          Tstr ) > 0 then do
                      call BbsWrite 0, G.CtrlM
                      Tmore = G.True
                   end
              when pos( "Select:",                              Tstr ) > 0 then Tmail = G.False
              when pos( "Hit <enter> (or wait 10 seconds)",     Tstr ) > 0 then Tmail = G.True
              otherwise Tmore = G.True
            end
         end
         if Tmail then do
            call BbsWrite 0, G.CtrlM
            if \BbsWaitFor( Mdm.AutoDnLoad, 0, 30 ) then
               call BbsWrite 0, G.CtrlM
            else do
               if \ReceiveFile( 'Z', Bbs.Baud, Tmailpkt ) then
                  call BbsWrite 0, G.CtrlM
               else do
                  call SayLog "Mail has been collected."G.CrLf
                  call TossPacket Bbs.Prefix, "QWK", G.InBox, G.PktBox
               end
            end
            call BbsWaitFor "Select:", 0, 30
         end
      end
   end
Return

BunNotice:
   Procedure expose G. Dev. Mdm. Bbs.
   /* Assume we are at the MAIN menu */
   call BbsWrite 0, 'V'                /* VioDevGrp menu                     */
   call BbsWaitFor "Select:", 0, 30
   call BbsWrite 0, 'N'                /* Next meeting notice                */
   call OpenCap "VioDev.Mtg"
   call BbsWaitFor "Select:", 0, 30
   call CloseCap
   call BbsWrite 0, 'M'                /* Return to main menu                */
   call BbsWaitFor "Select:", 0, 30
Return

BunFiles:
   Procedure expose G. Dev. Mdm. Bbs.
   /* Assume we are at the MAIN menu */
   call BbsWrite 0, 'F'                /* Select file menu                   */
   if \BbsWaitFor( "Select:", 0, 30 ) then do
      call BbsWrite 0, 'V'||G.CtrlM    /* VioDevGrp please                   */
      if \BbsWaitFor( "Select:", 0, 30 ) then do
         call BbsWrite 0, 'DEVGRP'||G.CtrlM    /* DevGrp please              */
         if \BbsWaitFor( "Select:", 0, 30 ) then do
            call BbsWrite 0, 'GBETA'||G.CtrlM  /* Guidelines please          */
            call BbsWaitFor "Select:", 0, 30   /* File menu prompt           */
         end
      end
   end
   call BbsWrite 0, 'N'                /* New files scan                     */
   call BbsWaitFor "Date to search from", 0, 30
   call BbsWrite 0, G.CtrlM            /* Since last connect                 */
   call OpenCap "BunFile.New"
   call BbsWaitFor "Select:", 0, 300   /* Allow 5 minutes tops               */
   call CloseCap
   call BunPutFile                     /* Upload specified files             */
   call BunGetFile                     /* Download specified files           */
Return

BunPutFile:
   Procedure expose G. Dev. Mdm. Bbs.
   if Bbs.Connected then do
      Told = "SendFile.Old"
      Tnew = "SendFile.New"
      if \RxAsyncFileRename( G.ScrPath||G.SndFile, Told ) then
         call SayLog "Unable to rename '"G.ScrPath||G.SndFile"' to '"Told"'."G.CrLf
      else do
         Tcount = 0
         do while lines( G.ScrPath||Told ) > 0
            Tspec = linein( G.ScrPath||Told )
            select
               when \Bbs.Connected then nop
               when substr(Tspec,1,1) == '#' then nop
               when Tcount >= Bbs.MaxUpload then nop
               when (pos( Bbs.Prefix, Tspec ) > 0) | (substr(Tspec,1,1) == '*') then do
                       if BunSndFile( Tspec ) then do
                          if substr(Tspec,1,1) == '*' then
                             call lineout G.ScrPath||Tnew, "# Uploaded on "date('N')" at "time('C')": "Bbs.Prefix" "substr(Tspec,3)
                          else do
                             Tspec = "# Uploaded on "date('N')" at "time('C')": "Tspec
                          end
                          Tcount = Tcount + 1
                       end
                    end
               otherwise nop
            end
            call lineout G.ScrPath||Tnew, Tspec
         end
         call lineout G.ScrPath||Tnew
         call lineout G.ScrPath||Told
         if RxAsyncFileRename( G.ScrPath||Tnew, G.SndFile ) then
            call RxAsyncFileDelete G.ScrPath||Told
         else do
            call SayLog "Unable to rename '"G.ScrPath||Tnew"' to '"G.SndFile"'."G.CrLf
         end
      end
   end
Return

BunSndFile:
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg Tfspec
   parse var Tfspec Tbbs Tarea Tfname Tdesc
   Tfile = G.False
   if length( Tfname ) > 0 then do
      /* default file path if not specified */
      Tdir  = RxAsyncFilePathIs( Tfname )
      Tname = RxAsyncFileNameIs( Tfname )
      if length( Tdir ) = 0 then do
         Tdir = G.OutBox
      end
      Tfname = Tdir||Tname
      if \RxAsyncFileExists( Tfname ) then
         call SayLog "Unable to upload '"Tfname"', file not found."G.CrLf
      else do
         /* change file area if required */
         if substr(Tarea,1,1) \= '*' then nop    /* not implemented yet */
         /* check for file on BBS */
         call BbsWrite 0, 'L'
         call BbsWaitFor "Enter the text to find:", 0, 30
         call BbsWrite 0, Tname||G.CtrlM
         Tfind = G.True
         Tmore = G.True
         do while Tmore  &  Bbs.Connected
            Tstr = BbsRead()
            select
              when \Bbs.Connected then nop
              when pos( "CHAT: start",             Tstr ) > 0 then do
                      call BbsChat
                      call BbsWaitFor "Select:", 0, 30
                      call BbsWrite 0, 'L'
                      call BbsWaitFor "Enter the text to find:", 0, 30
                      call BbsWrite 0, Tname||G.CtrlM
                   end
              when pos( "Located 0 matches.",      Tstr ) > 0 then Tfind = G.False
              when pos( "Select:",                 Tstr ) > 0 then Tmore = G.False
              otherwise nop
            end
         end
         if Bbs.Connected  &  \Tfind then do
            /* upload file */
            call BbsWrite 0, 'U'
            if \BbsWaitFor( Mdm.AutoUpLoad, 0, 30 ) then
               call BbsWrite 0, G.CtrlM
            else do
               if \SendFile( 'Z', Bbs.Baud, Tfname ) then
                  call BbsWrite 0, G.CtrlM
               else do
                  Tfile = G.True
                  /* fill in the description */
                  Tlin = 1
                  call BbsWaitFor D2C(Tlin+48)||'>', 0, 30
                  Tdsc = ""
                  Tnum = 0
                  Tcnt = words( Tdesc )
                  do while Tnum < Tcnt  &  Tlin < 4  &  Bbs.Connected
                     Tnum = Tnum + 1
                     Twrd = word( Tdesc, Tnum )
                     if length( Tdsc' 'Twrd ) < 45 then
                        Tdsc = Tdsc' 'Twrd
                     else do
                        Tlin = Tlin + 1
                        call BbsWrite 0, substr(Tdsc,2)||G.CtrlM
                        call BbsWaitFor D2C(Tlin+48)||'>', 0, 30
                        Tdsc = ' 'Twrd
                     end
                     if Tnum = Tcnt then do
                        Tlin = Tlin + 1
                        call BbsWrite 0, substr(Tdsc,2)||G.CtrlM
                        call BbsWaitFor D2C(Tlin+48)||'>', 0, 30
                     end
                  end
                  call BbsWrite 0, G.CtrlM
               end
            end
            call BbsWaitFor "Select:", 0, 30
         end
      end
   end
Return Tfile

BunGetFile:
   Procedure expose G. Dev. Mdm. Bbs.
   if Bbs.Connected then do
      Told = "FreqFile.Old"
      Tnew = "FreqFile.New"
      if \RxAsyncFileRename( G.ScrPath||G.FrqFile, Told ) then
         call SayLog "Unable to rename '"G.ScrPath||G.FrqFile"' to '"Told"'."G.CrLf
      else do
         Tcount = 0
         do while lines( G.ScrPath||Told ) > 0
            Tspec = linein( G.ScrPath||Told )
            select
               when \Bbs.Connected then nop
               when substr(Tspec,1,1) == '#' then nop
               when Tcount >= Bbs.MaxDnload then nop
               when (pos( Bbs.Prefix, Tspec ) > 0) | (substr(Tspec,1,1) == '*') then do
                       if BunRcvFile( Tspec ) then do
                          if substr(Tspec,1,1) == '*' then
                             Tspec = "# Downloaded on "date('N')" at "time('C')": "Bbs.Prefix" "substr(Tspec,3)
                          else do
                             Tspec = "# Downloaded on "date('N')" at "time('C')": "Tspec
                          end
                          Tcount = Tcount + 1
                       end
                    end
               otherwise nop
            end
            call lineout G.ScrPath||Tnew, Tspec
         end
         call lineout G.ScrPath||Tnew
         call lineout G.ScrPath||Told
         if RxAsyncFileRename( G.ScrPath||Tnew, G.FrqFile ) then
            call RxAsyncFileDelete G.ScrPath||Told
         else do
            call SayLog "Unable to rename '"G.ScrPath||Tnew"' to '"G.FrqFile"'."G.CrLf
         end
      end
   end
Return

BunRcvFile:
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg Tfspec
   parse var Tfspec Tbbs Tarea Tfname Tfdest
   Tfile = G.False
   if length( Tfname ) > 0 then do
      /* source file spec */
      Tname = RxAsyncFileNameIs( Tfname )
      /* destination file spec */
      Tfdestdir  = ''
      Tfdestname = ''
      if length( Tfdest ) > 0 then do
         Tfdestdir  = RxAsyncFilePathIs( Tfdest )
         Tfdestname = RxAsyncFileNameIs( Tfdest )
      end
      /* default to source name if no target name specified */
      if length( Tfdestname ) = 0 then do
         Tfdestname = Tname
      end
      /* default to inbox if no target directory specified */
      if length( Tfdestdir ) = 0 then do
         Tfdestdir = G.InBox
      end
      Tfdest = Tfdestdir||Tfdestname
      /* check existence */
      if RxAsyncFileExists( Tfdest ) then do
         call SayLog "Unable to download to '"Tfdest"', file already exists."G.CrLf
         Tfile = G.True
      end; else do
         /* change file area if required */
         if substr(Tarea,1,1) \= '*' then nop /* not implemented yet */
         /* check for file on BBS */
         call BbsWrite 0, 'L'
         call BbsWaitFor "Enter the text to find:", 0, 30
         call BbsWrite 0, Tname||G.CtrlM
         Tfind = G.True
         Tmore = G.True
         do while Tmore  &  Bbs.Connected
            Tstr = BbsRead()
            select
              when \Bbs.Connected then nop
              when pos( "CHAT: start",             Tstr ) > 0 then do
                      call BbsChat
                      call BbsWaitFor "Select:", 0, 30
                      call BbsWrite 0, 'L'
                      call BbsWaitFor "Enter the text to find:", 0, 30
                      call BbsWrite 0, Tname||G.CtrlM
                   end
              when pos( "Located 0 matches.",      Tstr ) > 0 then Tfind = G.False
              when pos( "Select:",                 Tstr ) > 0 then Tmore = G.False
              otherwise nop
            end
         end
         if Bbs.Connected  &  Tfind then do
            /* download file */
            call BbsWrite 0, 'D'
            call BbsWaitFor "File(s) to download ", 0, 30
            call BbsWrite 0, Tname||G.CtrlM
            if \BbsWaitFor( "File(s) to download", 0, 30 ) then
               call BbsWrite 0, G.CtrlM
            else do
               call BbsWrite 0, G.CtrlM
               if \BbsWaitFor( Mdm.AutoDnLoad, 0, 30 ) then
                  call BbsWrite 0, G.CtrlM
               else do
                  if \ReceiveFile( 'Z', Bbs.Baud, Tfdest ) then
                     call BbsWrite 0, G.CtrlM
                  else do
                     Tfile = G.True
                  end
               end
            end
            call BbsWaitFor "Select:", 0, 30
         end
      end
   end
Return Tfile

BunLogoff:
   Procedure expose G. Dev. Mdm. Bbs.
   call BbsWrite 0, 'G'
   call BbsWaitFor "Disconnect [Y,n,?=help]", 0, 30
   call BbsWrite 0, G.CtrlM
   call BbsWaitFor "Leave a message to", 0, 30
   call BbsWrite 0, G.CtrlM
   call BbsWaitFor "You have uploaded", 0, 30
Return

/*****************************************************************************/
/* SCRIPT SUPPORT ROUTINES ################################################# */
/*****************************************************************************/

BbsWrite:                              /* write string to BBS                */
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg Twait, Tstring
   if Bbs.Connected then do
      Twait = Twait * 1000             /* convert to milliseconds */
      call ComWrite Twait, Tstring
   end
Return

BbsChat:                               /* handle chat interruption by sysop  */
   Procedure expose G. Dev. Mdm. Bbs.
   /* found a "CHAT: start" so tell the sysop to POQ */
   call OpenCap "BbsChat.Cap"
   call BbsWrite 0, "WARNING: You are attempting to chat with an automated service!"||G.CtrlM
   call BbsWrite 0, G.CtrlM
   call BbsWrite 0, "  Your comments will be logged but cannot be responded to at"||G.CtrlM
   call BbsWrite 0, "  this time.  Execution of this service should resume as soon"||G.CtrlM
   call BbsWrite 0, "  as chat mode is terminated."||G.CtrlM
   call BbsWrite 0, G.CtrlM
   call BbsWrite 0, "Thanks for dropping in.<g>"||G.CtrlM
   call BbsWrite 0, G.CtrlM
   call BbsWaitFor "CHAT: end", 0, 120
   call CloseCap
   /* found a "CHAT: end" so return to doing what we came here to do */
Return

BbsRead:                               /* read string from BBS               */
   Procedure expose G. Dev. Mdm. Bbs.
   Tstr = ""
   Tagain = G.True
   do while Tagain & Bbs.Connected
      Tstr = ComRead()
      Bbs.Connected = ComCarrier()
      if Bbs.Connected then do
         Bbs.Elapsed = (time("E")*1000) - Bbs.Online
         select
           when Bbs.Elapsed >= Bbs.OnTimeout then do
                   call SayLog "Closing connection, time limit exceeded."G.CrLf
                   call ComHangup         /* Terminate connection               */
                   Bbs.Connected = G.False
                   Tagain = G.False
                end
           when pos( "--More--",                Tstr ) > 0 then call BbsWrite 0, ' '
           when pos( "Press ENTER to continue", Tstr ) > 0 then call BbsWrite 0, G.CtrlM
           when pos( "More [Y,n,=]?",           Tstr ) > 0 then call BbsWrite 0, G.CtrlM
           when pos( "More [Y,n,t,=]?",         Tstr ) > 0 then call BbsWrite 0, G.CtrlM
           when length( Tstr ) > 0 then Tagain = G.False
           otherwise nop
         end
      end
   end
Return Tstr

BbsWaitFor:                            /* wait for specified string          */
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg Twaitstr, Tstrpos, Tmaxwait
   Tstr = ""
   Tbegun = time("E")*1000
   Tfound = G.False
   Tagain = G.True
   do while Tagain & Bbs.Connected
      Tstr = ComRead()
      Bbs.Connected = ComCarrier()
      if Bbs.Connected then do
         Bbs.Elapsed = (time("E")*1000) - Bbs.Online
         Twaited = (time("E")*1000) - Tbegun
         select
           when Bbs.Elapsed >= Bbs.OnTimeout then do
                   call SayLog "Closing connection, time limit exceeded."G.CrLf
                   call ComHangup         /* Terminate connection               */
                   Bbs.Connected = G.False
                   Tagain = G.False
                end
           when (Tstrpos = 0) & (pos( Twaitstr, Tstr ) > 0) then do
                   Tfound = G.True
                   Tagain = G.False
                end
           when (Tstrpos > 0) & (pos( Twaitstr, Tstr ) = Tstrpos) then do
                   Tfound = G.True
                   Tagain = G.False
                end
           when pos( "--More--",                Tstr ) > 0 then call BbsWrite 0, ' '
           when pos( "Press ENTER to continue", Tstr ) > 0 then call BbsWrite 0, G.CtrlM
           when pos( "More [Y,n,=]?",           Tstr ) > 0 then call BbsWrite 0, G.CtrlM
           when pos( "More [Y,n,t,=]?",         Tstr ) > 0 then call BbsWrite 0, G.CtrlM
           when (Tmaxwait >= 0)  &  (Twaited >= (Tmaxwait*1000)) then Tagain = G.False
           otherwise nop
         end
      end
   end
Return Tfound

/*****************************************************************************/
/* EMAIL SUPPORT ROUTINES ################################################## */
/*****************************************************************************/

TossPacket:
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg PktPfx, PktType, PktFrom, PktTo
   Tossed = G.False
   Ttmp = G.HomeDir||"RxScript.Tmp"
   Tnum = 0
   "@FF "PktTo" "PktPfx"."substr(PktType,1,1)"?? | SORT >"Ttmp
   if RC = 0 then do
      do while lines( Ttmp ) > 0
         Tstr = linein( Ttmp )
         Tval = X2D( right( Tstr, 2, '0' ) )
         if Tval > Tnum then Tnum = Tval
      end
      call lineout Ttmp
   end
   call RxAsyncFileDelete Ttmp
   Tnum = Tnum + 1
   if Tnum > 255 then do     /* (ie. FF) */
      Tnum = 1               /* (ie. 01) */
   end
   OldPkt = PktFrom||PktPfx||"."||PktType
   NewPkt = PktTo||PktPfx||"."||substr(PktType,1,1)||D2X(Tnum,2)
   Tossed = RxAsyncFileMove( OldPkt, NewPkt )
   if \Tossed then
      call SayLog "Unable to toss '"OldPkt"' to '"NewPkt"'. Archive NOW!"G.CrLf
   else do
      if PktType == "REP" then do     /* only for reply packets */
         "@pkunzip -o "||NewPkt||" "||PktTo||" "||PktPfx||".MSG"
         "@"||G.PrgPath||"rep2txt "||PktTo||PktPfx||".MSG "||PktTo||"SentMail.Txt"
         "@del "||PktTo||PktPfx||".MSG"
      end
   end
Return Tossed

/*****************************************************************************/
/* INITIALISATION AND SHUTDOWN ROUTINES #################################### */
/*****************************************************************************/

Startup:                               /* Perform initialisation routines    */
   Procedure expose G. Dev. Mdm. Bbs.
   call FncLoad
   call InitGlb
   call GetParm
   call OpenTrc
   call OpenLog
   call InitProt
   call PrepDev
   call PrepMdm
   call PrepBbs
   call ComOpen
   call ComInitialise Bbs.Baud, Bbs.Parity, Bbs.Data, Bbs.Stop
Return

Cleanup:                               /* Cleanup prior to exit              */
   Procedure expose G. Dev. Mdm. Bbs.
   call ComClose
   call FiniProt
   call CloseBbs
   call CloseMdm
   call CloseDev
   call CloseCap
   call CloseLog
   call CloseTrc
   call FreeGlb
   call FncDrop
Return

FncLoad:                               /* Register functions with rexx       */
   Procedure expose G. Dev. Mdm. Bbs.
   Dev.Load = G.True
   call RxFuncAdd 'RxAsyncLoadFuncs', 'RXASYNC', 'RxAsyncLoadFuncs'
   zx = RxAsyncLoadFuncs()             /* Load RXASYNC.dll functions         */
   say 'RxAsyncLoadFuncs returned rc='zx
   signal on error name FncFatal
Return

FncDrop:                               /* Deregister externalised functions  */
   Procedure expose G. Dev. Mdm. Bbs.
   if Dev.Load then do
      Dev.Load = G.False               /* Prevent repeated call              */
      zx = RxAsyncDropFuncs()          /* Drop RXASYNC.dll functions         */
      say 'RxAsyncDropFuncs returned rc='zx
   end
Return

FncFatal:                              /* Handle serious errors              */
   Procedure expose G. Dev. Mdm. Bbs.
   if Dev.Hdl > 0 then do
      call RxAsyncClose Dev.Hdl
   end
   exit
Return

InitGlb:                               /* Initialise global variables        */
   Procedure expose G. Dev. Mdm. Bbs.

   G.SaveDir  = RxAsyncDirectory()     /* Save so that we can return to it   */

   /* Install should place an entry in OS2.INI which tells RXSCRIPT where it */
   /* has been installed, thus this routine can then query and set HomeDir.  */
   /* This requires install script to prompt for destination directory with  */
   /* default of "C:\Mail\" and insert an entry into OS2.INI then copy the   */
   /* files and creating directories and desktop objects required.           */
   /* An uninstall script to delete the OS2.INI entry then prompt for        */
   /* removal of all files, directories and desktop objects.                 */

   /* In the meantime we hard code the home directory here.                  */
   G.HomeDir  = "D:\Mail\"             /* Home, home on the range.......     */

   /* General */
   G.True      = 1
   G.False     = 0
   G.Esc       = D2C(27)
   G.CtrlM     = D2C(13)
   G.CtrlX     = D2C(24)
   G.CrLf      = D2C(13)||D2C(10)
   G.NoEcho    = G.True
   G.Quietly   = G.False
   G.EmailOnly = G.False
   G.FreqOnly  = G.False
   G.Abort     = G.False

   /* Default paths */
   G.PrgPath  = G.HomeDir||"Bin\"      /* Programs and configuration files   */
   G.LogPath  = G.HomeDir||"Logs\"     /* log and trace files                */
   G.ScrPath  = G.HomeDir||"Scripts\"  /* script files                       */
   G.InBox    = G.HomeDir||"InBox\"    /* inbound file and mail area         */
   G.OutBox   = G.HomeDir||"OutBox\"   /* outbound file and mail area        */
   G.PktBox   = G.HomeDir||"Messages\" /* message packet reading area        */
   G.RepBox   = G.HomeDir||"Replies\"  /* reply packet creation area         */
   G.SntBox   = G.HomeDir||"SentMail\" /* processed replies                  */

   /* Default files names */
   G.SndFile  = "RxScript.Put"         /* default upload list file name      */
   G.FrqFile  = "RxScript.Get"         /* default download list file name    */
   G.TrcFile  = "RxScript.Trc"         /* default trace file name            */
   G.LogFile  = "RxScript.Log"         /* default log file name              */
   G.XfrFile  = "RxScript.Xfr"         /* default up/down load log name      */
   G.DevFile  = "RxScript.Dev"         /* default device file name           */
   G.MdmFile  = "RxScript.Mdm"         /* default modem file name            */
   G.BbsFile  = ""                     /* default bbs file name              */
   G.ScrFile  = ""                     /* default script file name           */
   G.CapFile  = ""                     /* default capture file name          */

   /* Used by trace routines */
   G.WantTrc  = G.False
   G.TrcOpen  = G.False

   /* Used by logging routines */
   G.WantLog  = G.False
   G.LogOpen  = G.False

   /* Used by capture routines */
   G.CapOpen  = G.False

   /* Used by external protocol routines */
   G.ProtIni  = G.False
   G.OldPath  = ""
   G.OldDpath = ""
   G.NewPath  = ""
   G.NewDpath = ""

   /* Status of definition files */
   Dev.Open   = G.False
   Mdm.Open   = G.False
   Bbs.Open   = G.False

   call RxAsyncDirectory G.PrgPath     /* Relocate to program directory      */
Return

FreeGlb:                               /* Release global variables           */
   Procedure expose G. Dev. Mdm. Bbs.
   call RxAsyncDirectory G.SaveDir     /* return to original location        */
Return

GetParm:                               /* Get and validate parameters        */
   Procedure expose G. Dev. Mdm. Bbs.
   Tparm = G.Parms
   do while length(Tparm) > 0
      parse var Tparm Tparm Trest
      Tparm = strip( Tparm, 'B', ' ' )
      Toptn = translate( substr(Tparm,1,2) )
      Tvalu = substr(Tparm,3)
      Tvallen = length( Tvalu )
      Tnamlen = 0
      if Tvallen > 0 then do
         Tnamlen = length( RxAsyncFileNameIs( Tvalu ) )
      end
      select
        when Toptn == "-B" then do
                select
                  when Tvallen = 0 then
                       call Abort "GetParm: Option -b specified without BBS definition file name."
                  when Tnamlen = 0 then
                       call Abort "GetParm: Option -b specified invalid BBS definition file name."
                  otherwise
                       G.BbsFile = Tvalu
                end
             end
        when Toptn == "-D" then do
                select
                  when Tvallen = 0 then
                       call Abort "GetParm: Option -d specified without device definition file name."
                  when Tnamlen = 0 then
                       call Abort "GetParm: Option -d specified invalid device definition file name."
                  otherwise
                       G.DevFile = Tvalu
                end
             end
        when Toptn == "-E" then do
                G.EmailOnly = G.True
             end
        when Toptn == "-F" then do
                G.FreqOnly = G.True
             end
        when Toptn == "-L" then do
                G.WantLog = G.True
                select
                  when Tvallen = 0 then
                       nop
                  when Tnamlen = 0 then
                       call Abort "GetParm: Option -l specified invalid log file name."
                  otherwise
                       G.LogFile = Tvalu
                end
             end
        when Toptn == "-M" then do
                select
                  when Tvallen = 0 then
                       call Abort "GetParm: Option -m specified without modem definition file name."
                  when Tnamlen = 0 then
                       call Abort "GetParm: Option -m specified invalid modem definition file name."
                  otherwise
                       G.MdmFile = Tvalu
                end
             end
        when Toptn == "-Q" then do
                G.Quietly = G.True
             end
        when Toptn == "-S" then do
                select
                  when Tvallen = 0 then
                       call Abort "GetParm: Option -s specified without script file name."
                  when Tnamlen = 0 then
                       call Abort "GetParm: Option -s specified invalid script file name."
                  otherwise
                       G.ScrFile = Tvalu
                end
             end
        when Toptn == "-T" then do
                G.WantTrc = G.True
                select
                  when Tvallen = 0 then
                       nop
                  when Tnamlen = 0 then
                       call Abort "GetParm: Option -t specified invalid trace file name."
                  otherwise
                       G.TrcFile = Tvalu
                end
             end
        when Toptn == "-X" then do
                select
                  when Tvallen = 0 then
                       call Abort "GetParm: Option -x specified without transfer log file name."
                  when Tnamlen = 0 then
                       call Abort "GetParm: Option -x specified invalid transfer log file name."
                  otherwise
                       G.XfrFile = Tvalu
                end
             end
        otherwise nop
      end
      Tparm = Trest
   end
   /* Default to looking in program path for DEV file if not specified */
   if RxAsyncFilePathIs( G.DevFile ) == "" then do
      G.DevFile = G.PrgPath||G.DevFile
   end
   /* Default to looking in log path for LOG file if not specified */
   if RxAsyncFilePathIs( G.LogFile ) == "" then do
      G.LogFile = G.LogPath||G.LogFile
   end
   /* Default to looking in program path for MDM file if not specified */
   if RxAsyncFilePathIs( G.MdmFile ) == "" then do
      G.MdmFile = G.PrgPath||G.MdmFile
   end
   /* Default to looking in script path for BBS file if not specified */
   if RxAsyncFilePathIs( G.BbsFile ) == "" then do
      G.BbsFile = G.ScrPath||G.BbsFile
   end
   /* Default to looking in log path for TRC file if not specified */
   if RxAsyncFilePathIs( G.TrcFile ) == "" then do
      G.TrcFile = G.LogPath||G.TrcFile
   end
   /* Default to looking in log path for XFR file if not specified */
   if RxAsyncFilePathIs( G.XfrFile ) == "" then do
      G.XfrFile = G.LogPath||G.XfrFile
   end
Return

/*****************************************************************************/
/* BBS DEFINITION ROUTINES ################################################# */
/*****************************************************************************/

PrepBbs:                               /* Initialise bbs definitions         */
   Procedure expose G. Dev. Mdm. Bbs.
   call InitBbsVar
   call OpenBbs
   call ReadBbs
   call CloseBbs
   call ChkBbsVar
Return

InitBbsVar:                            /* Initialise global bbs variables    */
   Procedure expose G. Dev. Mdm. Bbs.
   Bbs.Name       = ""
   Bbs.Prefix     = ""
   Bbs.Sysop      = ""
   Bbs.Phone      = ""
   Bbs.HostId     = ""
   Bbs.Script     = ""
   Bbs.Userid     = ""
   Bbs.Password   = ""
   Bbs.Baud       = Dev.Baud
   Bbs.Parity     = Dev.Parity
   Bbs.Data       = Dev.Data
   Bbs.Stop       = Dev.Stop
   Bbs.OnTimeout  = 3600*1000          /* 60 minutes */
   Bbs.RetryWait  = 300*1000           /* 5 minutes */
   Bbs.RetryLimit = 5                  /* 5 times */
   Bbs.FileArea   = "1"
   Bbs.FtpStatus  = "Yes"
   Bbs.MaxUpload  = 10
   Bbs.MaxDnload  = 20
   Bbs.ReplyCheck = ""
   Bbs.Online     = 0
   Bbs.Elapsed    = 0
   Bbs.Connected  = G.False
Return

OpenBbs:                               /* Open bbs definition file           */
   Procedure expose G. Dev. Mdm. Bbs.
   Bbs.Open = G.False
   if RxAsyncFileExists( G.BbsFile ) then
      call SayMsg "Loading BBS definition from file '"G.BbsFile"'."G.CrLf
   else do
      call Abort "OpenBbs: Could not open BBS definition file '"G.BbsFile"'."
   end
   Tstr = linein( G.BbsFile, 1, 0 )
   if Tstr \== "" then do
      call Abort "OpenBbs: Could not open BBS definition file '"G.BbsFile"'."
   end
   Bbs.Open = G.True
Return

ReadBbs:                               /* Process bbs definition file        */
   Procedure expose G. Dev. Mdm. Bbs.
   if Bbs.Open then do
      do while lines( G.BbsFile ) > 0
         Tstr = linein( G.BbsFile )
         Tstr = strip( Tstr, 'B', ' ' )
         if length( Tstr ) > 0 then do
            if substr( Tstr, 1, 1 ) <> '#' then do
               parse var Tstr Tkey Tval
               call SetBbsVar Tkey, Tval
            end
         end
      end
   end
Return

CloseBbs:                              /* Close bbs defintion file           */
   Procedure expose G. Dev. Mdm. Bbs.
   if Bbs.Open then do
      Bbs.Open = G.False               /* Prevent repeated call              */
      Trxc = lineout( G.BbsFile )
      if Trxc <> 0 then do
         call Abort "CloseBbs: Could not close BBS definition file '"G.BbsFile"'."
      end
   end
Return

ChkBbsVar:                             /* Validate bbs definitions           */
   Procedure expose G. Dev. Mdm. Bbs.
   if length( G.ScrFile ) > 0 then do
      Bbs.Script = G.ScrFile
   end
   Tscrpath = RxAsyncFilePathIs( Bbs.Script )
   Tscrname = RxAsyncFileNameIs( Bbs.Script )
   if Tscrpath == "" then do
      Bbs.Script = G.ScrPath||Bbs.Script
   end
   if Bbs.Prefix     == "" then call Abort "ChkBbsVar: BBS definition 'Prefix:' not specified."
   if Bbs.Phone      == "" then call Abort "ChkBbsVar: BBS definition 'PhoneNumber:' not specified."
   if Bbs.Userid     == "" then call Abort "ChkBbsVar: BBS definition 'Userid:' not specified."
   if Bbs.Password   == "" then call Abort "ChkBbsVar: BBS definition 'Password:' not specified."
   if Tscrname       == "" then call Abort "ChkBbsVar: BBS definition 'Script:' not specified."
   if Bbs.ReplyCheck == "" then call Abort "ChkBbsVar: BBS definition 'ReplyCheck:' not specified."

   /* if \RxAsyncFileExists( Bbs.Script ) then do                             */
   /*    call Abort "ChkBbsVar: Could not find script file '"Bbs.Script"'."   */
   /* end                                                                     */
   /*                                                                         */
   /* The script routines have to be internal as REXX can't pass variables to */
   /* an external command file so we'll just assume that the name is correct. */
Return

SetBbsVar:                             /* Set global bbs variables           */
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg Tkey, Tval
   Tkey = strip( Tkey, 'B', ' ' )
   Tval = strip( Tval, 'B', ' ' )
   Tchk = translate( Tkey )            /* Convert to uppercase for testing   */
   Tval = ChkDefVal( Tkey, Tval )      /* Resolve any ^M sequences           */
   call TrcMsg Tkey||' '||Tval
   select
     when Tchk == "NAME:"                         then Bbs.Name       = Tval
     when Tchk == "PREFIX:"         & Tval \== "" then Bbs.Prefix     = Tval
     when Tchk == "SYSOP:"                        then Bbs.Sysop      = Tval
     when Tchk == "PHONENUMBER:"    & Tval \== "" then Bbs.Phone      = Tval
     when Tchk == "HOSTNETID:"      & Tval \== "" then Bbs.HostId     = Tval
     when Tchk == "SCRIPT:"         & Tval \== "" then Bbs.Script     = Tval
     when Tchk == "USERID:"         & Tval \== "" then Bbs.Userid     = Tval
     when Tchk == "PASSWORD:"       & Tval \== "" then Bbs.Password   = Tval
     when Tchk == "BAUDRATE:"       & Tval \== "" then Bbs.Baud       = Tval
     when Tchk == "PARITY:"         & Tval \== "" then Bbs.Parity     = Tval
     when Tchk == "DATABITS:"       & Tval \== "" then Bbs.Data       = Tval
     when Tchk == "STOPBITS:"       & Tval \== "" then Bbs.Stop       = Tval
     when Tchk == "ONLINETIMEOUT:"  & Tval \== "" then Bbs.OnTimeout  = Tval*1000
     when Tchk == "RETRYWAIT:"      & Tval \== "" then Bbs.RetryWait  = Tval*1000
     when Tchk == "RETRYLIMIT:"     & Tval \== "" then Bbs.RetryLimit = Tval
     when Tchk == "DEFFILEAREA:"                  then Bbs.FileArea   = Tval
     when Tchk == "SHOWFTPSTATUS:"  & Tval \== "" then Bbs.FtpStatus  = Tval
     when Tchk == "MAXUPLOAD:"      & Tval \== "" then Bbs.MaxUpload  = Tval
     when Tchk == "MAXDOWNLOAD:"    & Tval \== "" then Bbs.MaxDnload  = Tval
     when Tchk == "REPLYCHECK:"     & Tval \== "" then Bbs.ReplyCheck = Tval
     otherwise
          call Abort "SetBbsVar: Bad BBS definition entry. Key='"Tkey"', Value='"Tval"'."
   end
Return

/*****************************************************************************/
/* MODEM DEFINITION ROUTINES ############################################### */
/*****************************************************************************/

PrepMdm:                               /* Initialise modem definitions       */
   Procedure expose G. Dev. Mdm. Bbs.
   call InitMdmVar
   call OpenMdm
   call ReadMdm
   call CloseMdm
   call ChkMdmVar
Return

InitMdmVar:                            /* Initialise global modem variables  */
   Procedure expose G. Dev. Mdm. Bbs.
   Mdm.Name       = "Generic"
   Mdm.ConTimeout = 45*1000              /* 45 seconds */
   Mdm.AutoDnLoad = G.CtrlX||"B00"
   Mdm.AutoUpLoad = G.CtrlX||"B01"
   Mdm.Enter      = G.CtrlM
   Mdm.Reset      = "ATZ"
   Mdm.Break      = "+++~~~+++"
   Mdm.DialPrefix = "ATDT"
   Mdm.DialSuffix = G.CtrlM
   Mdm.Online     = "ATO"
   Mdm.Hangup     = "ATH0"
   Mdm.Okay       = "OK"
   Mdm.Error      = "ERROR"
   Mdm.NoCarrier  = "NO CARRIER"
   Mdm.InitStr0   = "ATE0Q0V1X1&C1&D2"
   Mdm.InitStr1   = ""
   Mdm.InitStr2   = ""
   Mdm.InitStr3   = ""
   Mdm.InitStr4   = ""
   Mdm.InitStr5   = ""
   Mdm.InitStr6   = ""
   Mdm.InitStr7   = ""
   Mdm.InitStr8   = ""
   Mdm.InitStr9   = ""
   Mdm.Connect0   = "CONNECT"
   Mdm.Connect1   = ""
   Mdm.Connect2   = ""
   Mdm.Connect3   = ""
   Mdm.Connect4   = ""
   Mdm.Connect5   = ""
   Mdm.Connect6   = ""
   Mdm.Connect7   = ""
   Mdm.Connect8   = ""
   Mdm.Connect9   = ""
   Mdm.NoConnect0 = "NO CARRIER"
   Mdm.NoConnect1 = ""
   Mdm.NoConnect2 = ""
   Mdm.NoConnect3 = ""
   Mdm.NoConnect4 = ""
   Mdm.NoConnect5 = ""
   Mdm.NoConnect6 = ""
   Mdm.NoConnect7 = ""
   Mdm.NoConnect8 = ""
   Mdm.NoConnect9 = ""
Return

OpenMdm:                               /* Open modem definition file         */
   Procedure expose G. Dev. Mdm. Bbs.
   Mdm.Open = G.False
   if RxAsyncFileExists( G.MdmFile ) then
      call SayMsg "Loading modem definition from file '"G.MdmFile"'."G.CrLf
   else do
      call Abort "OpenMdm: Could not open modem definition file '"G.MdmFile"'."
   end
   Tstr = linein( G.MdmFile, 1, 0 )
   if Tstr \== "" then do
      call Abort "OpenMdm: Could not open modem definition file '"G.MdmFile"'."
   end
   Mdm.Open = G.True
Return

ReadMdm:                               /* Process modem definition file      */
   Procedure expose G. Dev. Mdm. Bbs.
   if Mdm.Open then do
      do while lines( G.MdmFile ) > 0
         Tstr = linein( G.MdmFile )
         Tstr = strip( Tstr, 'B', ' ' )
         if length( Tstr ) > 0 then do
            if substr( Tstr, 1, 1 ) <> '#' then do
               parse var Tstr Tkey Tval
               call SetMdmVar Tkey, Tval
            end
         end
      end
   end
Return

CloseMdm:                              /* Close modem defintion file         */
   Procedure expose G. Dev. Mdm. Bbs.
   if Mdm.Open then do
      Mdm.Open = G.False               /* Prevent repeated call              */
      Trxc = lineout( G.MdmFile )
      if Trxc <> 0 then do
         call Abort "CloseMdm: Could not close modem definition file '"G.MdmFile"'."
      end
   end
Return

ChkMdmVar:                             /* Validate modem definitions         */
   Procedure expose G. Dev. Mdm. Bbs.
   /* do nothing stub */
   nop
Return

SetMdmVar:                             /* Set global modem variables         */
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg Tkey, Tval
   Tkey = strip( Tkey, 'B', ' ' )
   Tval = strip( Tval, 'B', ' ' )
   Tchk = translate( Tkey )            /* Convert to uppercase for testing   */
   Tval = ChkDefVal( Tkey, Tval )      /* Resolve any ^M sequences           */
   call TrcMsg Tkey||' '||Tval
   select
     when Tchk == "NAME:"                         then Mdm.Name       = Tval
     when Tchk == "CONNECTTIMEOUT:" & Tval \== "" then Mdm.ConTimeout = Tval*1000
     when Tchk == "AUTODNLOAD:"     & Tval \== "" then Mdm.AutoDnLoad = Tval
     when Tchk == "AUTOUPLOAD:"     & Tval \== "" then Mdm.AutoUpLoad = Tval
     when Tchk == "ENTER:"          & Tval \== "" then Mdm.Enter      = Tval
     when Tchk == "RESET:"          & Tval \== "" then Mdm.Reset      = Tval
     when Tchk == "BREAK:"                        then Mdm.Break      = Tval
     when Tchk == "DIALPREFIX:"     & Tval \== "" then Mdm.DialPrefix = Tval
     when Tchk == "DIALSUFFIX:"     & Tval \== "" then Mdm.DialSuffix = Tval
     when Tchk == "ONLINE:"                       then Mdm.Online     = Tval
     when Tchk == "HANGUP:"                       then Mdm.Hangup     = Tval
     when Tchk == "OKAY:"           & Tval \== "" then Mdm.Okay       = Tval
     when Tchk == "ERROR:"          & Tval \== "" then Mdm.Error      = Tval
     when Tchk == "NOCARRIER:"      & Tval \== "" then Mdm.NoCarrier  = Tval
     when Tchk == "INITSTR0:"                     then Mdm.InitStr0   = Tval
     when Tchk == "INITSTR1:"                     then Mdm.InitStr1   = Tval
     when Tchk == "INITSTR2:"                     then Mdm.InitStr2   = Tval
     when Tchk == "INITSTR3:"                     then Mdm.InitStr3   = Tval
     when Tchk == "INITSTR4:"                     then Mdm.InitStr4   = Tval
     when Tchk == "INITSTR5:"                     then Mdm.InitStr5   = Tval
     when Tchk == "INITSTR6:"                     then Mdm.InitStr6   = Tval
     when Tchk == "INITSTR7:"                     then Mdm.InitStr7   = Tval
     when Tchk == "INITSTR8:"                     then Mdm.InitStr8   = Tval
     when Tchk == "INITSTR9:"                     then Mdm.InitStr9   = Tval
     when Tchk == "CONNECT0:"       & Tval \== "" then Mdm.Connect0   = Tval
     when Tchk == "CONNECT1:"                     then Mdm.Connect1   = Tval
     when Tchk == "CONNECT2:"                     then Mdm.Connect2   = Tval
     when Tchk == "CONNECT3:"                     then Mdm.Connect3   = Tval
     when Tchk == "CONNECT4:"                     then Mdm.Connect4   = Tval
     when Tchk == "CONNECT5:"                     then Mdm.Connect5   = Tval
     when Tchk == "CONNECT6:"                     then Mdm.Connect6   = Tval
     when Tchk == "CONNECT7:"                     then Mdm.Connect7   = Tval
     when Tchk == "CONNECT8:"                     then Mdm.Connect8   = Tval
     when Tchk == "CONNECT9:"                     then Mdm.Connect9   = Tval
     when Tchk == "NOCONNECT0:"     & Tval \== "" then Mdm.NoConnect0 = Tval
     when Tchk == "NOCONNECT1:"                   then Mdm.NoConnect1 = Tval
     when Tchk == "NOCONNECT2:"                   then Mdm.NoConnect2 = Tval
     when Tchk == "NOCONNECT3:"                   then Mdm.NoConnect3 = Tval
     when Tchk == "NOCONNECT4:"                   then Mdm.NoConnect4 = Tval
     when Tchk == "NOCONNECT5:"                   then Mdm.NoConnect5 = Tval
     when Tchk == "NOCONNECT6:"                   then Mdm.NoConnect6 = Tval
     when Tchk == "NOCONNECT7:"                   then Mdm.NoConnect7 = Tval
     when Tchk == "NOCONNECT8:"                   then Mdm.NoConnect8 = Tval
     when Tchk == "NOCONNECT9:"                   then Mdm.NoConnect9 = Tval
     otherwise
          call Abort "SetMdmVar: Bad modem definition entry. Key='"Tkey"', Value='"Tval"'."
   end
Return

/*****************************************************************************/
/* COMMUNICATIONS DEFINITION ROUTINES ###################################### */
/*****************************************************************************/

PrepDev:                               /* Initialise comms port definitions  */
   Procedure expose G. Dev. Mdm. Bbs.
   call InitDevVar
   call OpenDev
   call ReadDev
   call CloseDev
   call ChkDevVar
Return

InitDevVar:                            /* Initialise global comms variables  */
   Procedure expose G. Dev. Mdm. Bbs.
   Dev.Load      = G.False
   Dev.Carrier   = G.False
   Dev.Hdl       = 0
   Dev.PollTime  = 2000                /* milliseconds */
   Dev.Port      = "COM2"
   Dev.Baud      = 1200
   Dev.Parity    = 'N'
   Dev.Data      = 8
   Dev.Stop      = 1
   Dev.WTimeout  = 50                  /* hundredths */
   Dev.RTimeout  = 50                  /* hundredths */
   Dev.DcbFlags1 = "00001001"
   Dev.DcbFlags2 = "10100000"
   Dev.DcbFlags3 = "11010010"
   Dev.ErrorChar = "00"
   Dev.BreakChar = "00"
   Dev.XonChar   = "11"
   Dev.XoffChar  = "13"
   Dev.EnhParms  = "00000010"
   Dev.OldBaud   = 0
   Dev.OldData   = 0
   Dev.OldParity = ''
   Dev.OldStop   = 0
   Dev.OldWtime  = 0
   Dev.OldRtime  = 0
   Dev.OldFlag1  = ""
   Dev.OldFlag2  = ""
   Dev.OldFlag3  = ""
   Dev.OldErrCh  = ""
   Dev.OldBrkCh  = ""
   Dev.OldXonCh  = ""
   Dev.OldXofCh  = ""
   Dev.OldEnPrm  = ""
Return

OpenDev:                               /* Open com definition file           */
   Procedure expose G. Dev. Mdm. Bbs.
   Dev.Open = G.False
   if RxAsyncFileExists( G.DevFile ) then
      call SayMsg "Loading device definition from file '"G.DevFile"'."G.CrLf
   else do
      call Abort "OpenDev: Could not open device definition file '"G.DevFile"'."
   end
   Tstr = linein( G.DevFile, 1, 0 )
   if Tstr \== "" then do
      call Abort "OpenDev: Could not open device definition file '"G.DevFile"'."
   end
   Dev.Open = G.True
Return

ReadDev:                               /* Process com definition file        */
   Procedure expose G. Dev. Mdm. Bbs.
   if Dev.Open then do
      do while lines( G.DevFile ) > 0
         Tstr = linein( G.DevFile )
         Tstr = strip( Tstr, 'B', ' ' )
         if length( Tstr ) > 0 then do
            if substr( Tstr, 1, 1 ) <> '#' then do
               parse var Tstr Tkey Tval
               call SetDevVar Tkey, Tval
            end
         end
      end
   end
Return

CloseDev:                              /* Close com defintion file           */
   Procedure expose G. Dev. Mdm. Bbs.
   if Dev.Open then do
      Dev.Open = G.False               /* Prevent repeated call              */
      Trxc = lineout( G.DevFile )
      if Trxc <> 0 then do
         call Abort "CloseDev: Could not close device definition file '"G.DevFile"'."
      end
   end
Return

ChkDevVar:                             /* Validate device definitions        */
   Procedure expose G. Dev. Mdm. Bbs.
   /* do nothing stub */
   nop
Return

SetDevVar:                             /* Set global com variables           */
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg Tkey, Tval
   Tkey = strip( Tkey, 'B', ' ' )
   Tval = strip( Tval, 'B', ' ' )
   Tchk = translate( Tkey )            /* Convert to uppercase for testing   */
   Tval = ChkDefVal( Tkey, Tval )      /* Resolve any ^M sequences           */
   call TrcMsg Tkey||' '||Tval
   select
     when Tchk == "PORT:"           & Tval \== "" then Dev.Port       = Tval
     when Tchk == "POLLTIMEOUT:"    & Tval \== "" then Dev.PollTime   = Tval
     when Tchk == "BAUDRATE:"       & Tval \== "" then Dev.Baud       = Tval
     when Tchk == "PARITY:"         & Tval \== "" then Dev.Parity     = Tval
     when Tchk == "DATABITS:"       & Tval \== "" then Dev.Data       = Tval
     when Tchk == "STOPBITS:"       & Tval \== "" then Dev.Stop       = Tval
     when Tchk == "WRITETIMEOUT:"   & Tval \== "" then Dev.WTimeout   = Tval
     when Tchk == "READTIMEOUT:"    & Tval \== "" then Dev.RTimeout   = Tval
     when Tchk == "DCBFLAGS1:"      & Tval \== "" then Dev.DcbFlags1  = Tval
     when Tchk == "DCBFLAGS2:"      & Tval \== "" then Dev.DcbFlags2  = Tval
     when Tchk == "DCBFLAGS3:"      & Tval \== "" then Dev.DcbFlags3  = Tval
     when Tchk == "ERRORCHAR:"      & Tval \== "" then Dev.ErrorChar  = Tval
     when Tchk == "BREAKCHAR:"      & Tval \== "" then Dev.BreakChar  = Tval
     when Tchk == "XONCHAR:"        & Tval \== "" then Dev.XonChar    = Tval
     when Tchk == "XOFFCHAR:"       & Tval \== "" then Dev.XoffChar   = Tval
     when Tchk == "ENHANCEDPARMS:"  & Tval \== "" then Dev.EnhParms   = Tval
     otherwise
          call Abort "SetDevVar: Bad device definition entry. Key='"Tkey"', Value='"Tval"'."
   end
Return

/*****************************************************************************/
/* DEFINITION FILE ROUTINES ################################################ */
/*****************************************************************************/

ChkDefVal:                             /* Resolve control sequences          */
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg ChkKey, ChkVal
   ChkFin = G.False
   ChkNew = ""
   ChkRem = ChkVal
   do until ChkFin
      ChkPos = pos( '^', ChkRem )
      ChkLen = length( ChkRem )
      if ChkPos > 0 then do
         if ChkPos = ChkLen then
            call Abort "ChkDefVal: Unexpected end of definition entry, Key='"ChkKey"', Value='"ChkVal"'."
         else do
            if ChkPos > 1 then do
               ChkNew = ChkNew||substr( ChkRem, 1, ChkPos-1 )
            end
            CtlChr = substr( ChkRem, ChkPos+1, 1 )
            if CtlChr == '^' then
               ChkNew = ChkNew||CtlChr
            else do
               CtlChr = translate( CtlChr )
               CtlInd = pos( CtlChr, "ABCDEFGHIJKLMNOPQRSTUVWXYZ[" )
               if CtlInd > 0 then
                  ChkNew = ChkNew||D2C(CtlInd)
               else do
                  call Abort "ChkDefVal: Invalid definition entry, Key='"ChkKey"', Value='"ChkVal"'."
               end
            end
            if ChkPos+2 > ChkLen then
               ChkFin = G.True
            else do
               ChkRem = substr( ChkRem, ChkPos+2, ChkLen-(ChkPos+1) )
            end
         end
      end; else do
         ChkFin = G.True
         ChkNew = ChkNew||ChkRem
      end
   end
Return ChkNew

/*****************************************************************************/
/* COMMUNICATIONS DEVICE ROUTINES ########################################## */
/*****************************************************************************/

ComOpen:                               /* Open communications device         */
   Procedure expose G. Dev. Mdm. Bbs.
   Dev.Hdl = 0
   Tdev = Dev.Hdl
   Trxc = RxAsyncOpen( Dev.Port, "Tdev" )
   if Trxc <> 0 then do
      call Abort "ComOpen: RxAsyncOpen failed, Rc='"Trxc"'."
   end
   Dev.Hdl = Tdev
   call TrcMsg "RxAsyncOpen opened port '"Dev.Port"', Handle='"Dev.Hdl"'."
   call ComPriority 3, 0
   call ComSave
Return

ComClose:                              /* Close communications device        */
   Procedure expose G. Dev. Mdm. Bbs.
   if Dev.Hdl > 0 then do
      call ComRestore
      call ComPriority 2, 0
      Tdev = Dev.Hdl
      Dev.Hdl = 0                      /* Prevent repeated call              */
      Trxc = RxAsyncClose( Tdev )
      if Trxc <> 0 then do
         call Abort "ComClose: RxAsyncClose failed, Rc='"Trxc"'."
      end
      call TrcMsg "RxAsyncClose closed the device."
   end
Return

ComSave:                               /* Save original settings             */
   Procedure expose G. Dev. Mdm. Bbs.
   /* line control */
   Tbaud   = 0
   Tdata   = 0
   Tparity = ''
   Tstop   = 0
   Trxc = RxAsyncGetLnCtrl( Dev.Hdl, 'Tbaud', 'Tdata', 'Tparity', 'Tstop' )
   if Trxc <> 0 then do
      call Abort "ComSave: RxAsyncGetLnCtrl failed, Rc='"Trxc"'."
   end
   Dev.OldBaud   = Tbaud
   Dev.OldData   = Tdata
   Dev.OldParity = Tparity
   Dev.OldStop   = Tstop
   call TrcMsg "RxAsyncGetLnCtrl has saved the original settings."
   call TrcMsg "  Baudrate was >"Dev.OldBaud"<"
   call TrcMsg "  Parity   was >"Dev.OldParity"<"
   call TrcMsg "  Databits was >"Dev.OldData"<"
   call TrcMsg "  Stopbits was >"Dev.OldStop"<"
   /* dcb info */
   TWtime  = 0
   TRtime  = 0
   TFlag1  = ""
   TFlag2  = ""
   TFlag3  = ""
   TErrCh  = ""
   TBrkCh  = ""
   TXonCh  = ""
   TXofCh  = ""
   Trxc = RxAsyncGetDcbInfo( Dev.Hdl, 'TWtime', 'TRtime', 'TFlag1', 'TFlag2', 'TFlag3', 'TErrCh', 'TBrkCh', 'TXonCh', 'TXofCh' )
   if Trxc <> 0 then do
      call Abort "ComSave: RxAsyncGetDcbInfo failed, Rc='"Trxc"'."
   end
   Dev.OldWtime  = TWtime
   Dev.OldRtime  = TRtime
   Dev.OldFlag1  = X2B(TFlag1)
   Dev.OldFlag2  = X2B(TFlag2)
   Dev.OldFlag3  = X2B(TFlag3)
   Dev.OldErrCh  = TErrCh
   Dev.OldBrkCh  = TBrkCh
   Dev.OldXonCh  = TXonCh
   Dev.OldXofCh  = TXofCh
   call TrcMsg "RxAsyncGetDcbInfo has saved the original settings."
   call TrcMsg "  Write Timeout  was >"Dev.OldWtime"<"
   call TrcMsg "  Read  Timeout  was >"Dev.OldRtime"<"
   call TrcMsg "  F1 HandShake   was >"Dev.OldFlag1"<"
   call TrcMsg "  F2 FlowReplace was >"Dev.OldFlag2"<"
   call TrcMsg "  F3 Timeout     was >"Dev.OldFlag3"<"
   call TrcMsg "  Error Replace  was >"Dev.OldErrCh"<"
   call TrcMsg "  Break Replace  was >"Dev.OldBrkCh"<"
   call TrcMsg "  Xon character  was >"Dev.OldXonCh"<"
   call TrcMsg "  Xoff character was >"Dev.OldXofCh"<"
   /* enhanced parms */
   TEnPrm  = ""
   Trxc = RxAsyncGetEnhParm( Dev.Hdl, 'TEnPrm' )
   if Trxc <> 0 then do
      call Abort "ComSave: RxAsyncGetEnhParm failed, Rc='"Trxc"'."
   end
   Dev.OldEnPrm  = X2B(TEnPrm)
   call TrcMsg "RxAsyncGetEnhParm has saved the original settings."
   call TrcMsg "  Enhanced Parms was >"Dev.OldEnPrm"<"
Return

ComRestore:                            /* Restore original settings          */
   Procedure expose G. Dev. Mdm. Bbs.
   /* enhanced parms */
   Trxc = RxAsyncSetEnhParm( Dev.Hdl, Dev.OldEnPrm )
   if Trxc <> 0 then do
      call Abort "ComRestore: RxAsyncSetEnhParm failed, Rc='"Trxc"'."
   end
   call TrcMsg "RxAsyncSetEnhParm has restored the original settings."
   /* dcb info */
   Trxc = RxAsyncSetDcbInfo( Dev.Hdl, Dev.OldWtime, Dev.OldRtime, Dev.OldFlag1, Dev.OldFlag2, Dev.OldFlag3, Dev.OldErrCh, Dev.OldBrkCh, Dev.OldXonCh, Dev.OldXofCh )
   if Trxc <> 0 then do
      call Abort "ComRestore: RxAsyncSetDcbInfo failed, Rc='"Trxc"'."
   end
   call TrcMsg "RxAsyncSetDcbInfo has restored the original settings."
   /* line control */
   Trxc = RxAsyncSetLnCtrl( Dev.Hdl, Dev.OldBaud, Dev.OldData, Dev.OldParity, Dev.OldStop )
   if Trxc <> 0 then do
      call Abort "ComRestore: RxAsyncSetLnCtrl failed, Rc='"Trxc"'."
   end
   call TrcMsg "RxAsyncSetLnCtrl has restored the original settings."
Return

ComInitialise:                         /* Set communications device parms    */
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg Tbaud, Tparity, Tdata, Tstop
   /* line control */
   Trxc = RxAsyncSetLnCtrl( Dev.Hdl, Tbaud, Tdata, Tparity, Tstop )
   if Trxc <> 0 then do
      call Abort "ComInitialise: RxAsyncSetLnCtrl failed, Rc='"Trxc"'."
   end
   call TrcMsg "RxAsyncSetLnCtrl has set the required settings."
   call TrcMsg "  Baudrate now >"Tbaud"<"
   call TrcMsg "  Parity   now >"Tparity"<"
   call TrcMsg "  Databits now >"Tdata"<"
   call TrcMsg "  Stopbits now >"Tstop"<"
   /* dcb info */
   Trxc = RxAsyncSetDcbInfo( Dev.Hdl, Dev.WTimeout, Dev.RTimeout, Dev.DcbFlags1, Dev.DcbFlags2, Dev.DcbFlags3, Dev.ErrorChar, Dev.BreakChar, Dev.XonChar, Dev.XoffChar )
   if Trxc <> 0 then do
      call Abort "ComInitialise: RxAsyncSetDcbInfo failed, Rc='"Trxc"'."
   end
   call TrcMsg "RxAsyncSetDcbInfo has set the required settings."
   call TrcMsg "  Write Timeout  now >"Dev.WTimeout"<"
   call TrcMsg "  Read  Timeout  now >"Dev.RTimeout"<"
   call TrcMsg "  F1 HandShake   now >"Dev.DcbFlags1"<"
   call TrcMsg "  F2 FlowReplace now >"Dev.DcbFlags2"<"
   call TrcMsg "  F3 Timeout     now >"Dev.DcbFlags3"<"
   call TrcMsg "  Error Replace  now >"Dev.ErrorChar"<"
   call TrcMsg "  Break Replace  now >"Dev.BreakChar"<"
   call TrcMsg "  Xon character  now >"Dev.XonChar"<"
   call TrcMsg "  Xoff character now >"Dev.XoffChar"<"
   /* enhanced parms */
   Trxc = RxAsyncSetEnhParm( Dev.Hdl, Dev.EnhParms )
   if Trxc <> 0 then do
      call Abort "ComInitialise: RxAsyncSetEnhParms failed, Rc='"Trxc"'."
   end
   call TrcMsg "RxAsyncSetEnhParms has set the required settings."
   call TrcMsg "  Enhanced Parms now >"Dev.EnhParms"<"
   /* modem */
   Tstr = ComSuck()
   call TrcMsg "Resetting"
   call ComWrite 0, Mdm.Reset||Mdm.Enter
   Tstr = ComSuck()
   if pos( Mdm.Okay, Tstr ) = 0 then do
      call Abort "ComInitialise: Unable to reset modem, possibly switched off."
   end
   call TrcMsg "Initialising"
   if length( Mdm.InitStr0 ) > 0 then do
      call ComWrite 0, Mdm.InitStr0||Mdm.Enter
      Tstr = ComSuck()
      if pos( Mdm.Okay, Tstr ) = 0 then do
         call Abort "ComInitialise: Unable to init0 modem, possibly switched off."
      end
   end
   if length( Mdm.InitStr1 ) > 0 then do
      call ComWrite 0, Mdm.InitStr1||Mdm.Enter
      Tstr = ComSuck()
      if pos( Mdm.Okay, Tstr ) = 0 then do
         call Abort "ComInitialise: Unable to init1 modem, possibly switched off."
      end
   end
   if length( Mdm.InitStr2 ) > 0 then do
      call ComWrite 0, Mdm.InitStr2||Mdm.Enter
      Tstr = ComSuck()
      if pos( Mdm.Okay, Tstr ) = 0 then do
         call Abort "ComInitialise: Unable to init2 modem, possibly switched off."
      end
   end
   if length( Mdm.InitStr3 ) > 0 then do
      call ComWrite 0, Mdm.InitStr3||Mdm.Enter
      Tstr = ComSuck()
      if pos( Mdm.Okay, Tstr ) = 0 then do
         call Abort "ComInitialise: Unable to init3 modem, possibly switched off."
      end
   end
   if length( Mdm.InitStr4 ) > 0 then do
      call ComWrite 0, Mdm.InitStr4||Mdm.Enter
      Tstr = ComSuck()
      if pos( Mdm.Okay, Tstr ) = 0 then do
         call Abort "ComInitialise: Unable to init4 modem, possibly switched off."
      end
   end
   if length( Mdm.InitStr5 ) > 0 then do
      call ComWrite 0, Mdm.InitStr5||Mdm.Enter
      Tstr = ComSuck()
      if pos( Mdm.Okay, Tstr ) = 0 then do
         call Abort "ComInitialise: Unable to init5 modem, possibly switched off."
      end
   end
   if length( Mdm.InitStr6 ) > 0 then do
      call ComWrite 0, Mdm.InitStr6||Mdm.Enter
      Tstr = ComSuck()
      if pos( Mdm.Okay, Tstr ) = 0 then do
         call Abort "ComInitialise: Unable to init6 modem, possibly switched off."
      end
   end
   if length( Mdm.InitStr7 ) > 0 then do
      call ComWrite 0, Mdm.InitStr7||Mdm.Enter
      Tstr = ComSuck()
      if pos( Mdm.Okay, Tstr ) = 0 then do
         call Abort "ComInitialise: Unable to init7 modem, possibly switched off."
      end
   end
   if length( Mdm.InitStr8 ) > 0 then do
      call ComWrite 0, Mdm.InitStr8||Mdm.Enter
      Tstr = ComSuck()
      if pos( Mdm.Okay, Tstr ) = 0 then do
         call Abort "ComInitialise: Unable to init8 modem, possibly switched off."
      end
   end
   if length( Mdm.InitStr9 ) > 0 then do
      call ComWrite 0, Mdm.InitStr9||Mdm.Enter
      Tstr = ComSuck()
      if pos( Mdm.Okay, Tstr ) = 0 then do
         call Abort "ComInitialise: Unable to init9 modem, possibly switched off."
      end
   end
Return

ComRead:                               /* Read from communications device    */
   Procedure expose G. Dev. Mdm. Bbs.
   Tstr = ""
   Trxc = RxAsyncRead( Dev.Hdl, 0, Dev.PollTime, "Tstr", "" )
   if Trxc > 0 then do
      call Abort "ComRead: RxAsyncRead failed, Rc='"Trxc"', Str='"Tstr"'."
   end
   call SayMsg Tstr
   if pos( Mdm.NoCarrier, Tstr ) = 1 then do
      Dev.Carrier = G.False
      call TrcMsg "RxAsyncRead detected a '"Mdm.NoCarrier"' response."
   end
Return Tstr

ComSuck:                               /* Readall from communications device */
   Procedure expose G. Dev. Mdm. Bbs.
   Tstr = ""
   Ttmp = ""
   Trxc = 0
   do while Trxc = 0
      Trxc = RxAsyncRead( Dev.Hdl, 0, Dev.PollTime, "Ttmp", "" )
      if Trxc > 0 then do
         call Abort "ComSuck: RxAsyncRead failed, Rc='"Trxc"', Str='"Ttmp"'."
      end
      if length(Ttmp) > 0 then do
         call SayMsg Ttmp
         Tstr = Ttmp
      end
   end
Return Tstr

ComWrite:                              /* Write to communications device     */
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg Tdelay, Tstr
   Trem = 0
   Trxc = RxAsyncWrite( Dev.Hdl, Tdelay, Tstr, "Trem" )
   if Trxc > 0 then do
      call Abort "ComWrite: RxAsyncWrite failed, Rc='"Trxc"', Rem='"Trem"'."
   end
   if G.NoEcho then
      call TrcMsg Tstr
   else do
      call SayMsg Tstr
   end
Return

ComPriority:                           /* Set process priority               */
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg Tclass, Tlevel
   Trxc = RxAsyncPriority( Tclass, Tlevel )
   if Trxc <> 0 then do
      call Abort "ComPriority: RxAsyncPriority failed, Rc='"Trxc"'."
   end
   call TrcMsg "RxAsyncPriority set, Class='"Tclass"', Level='"Tlevel"'."
Return

ComCarrier:                            /* Determine carrier status           */
   Procedure expose G. Dev. Mdm. Bbs.
   Dev.Carrier = G.False
   Trxc = RxAsyncCarrier( Dev.Hdl, 0 )
   if Trxc > 0 then do
      call Abort "ComCarrier: RxAsyncCarrier failed, Rc='"Trxc"'."
   end
   if Trxc = 0 then
      Dev.Carrier = G.True
   else do
      call TrcMsg "RxAsyncCarrier indicates carrier was dropped."
   end
Return Dev.Carrier

ComConnect:                            /* Connect to specified service       */
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg Tservice, Tphone, Tretries, Tretrywait
   call SayLog "Dialing '"Tservice"' on '"Tphone"'."G.CrLf
   Tcount = 0
   Tagain = G.True
   do while Tagain
      Tcount = Tcount + 1
      call ComWrite 0, Mdm.DialPrefix||Tphone||Mdm.DialSuffix
      call RxAsyncCarrier Dev.Hdl, Mdm.ConTimeout
      Tstr = ComRead()
      if length(Tstr) = 0 then do
         Tstr = ComRead()              /* In case we missed it first time    */
      end
      Tconnected = G.False
      select
        when length(Tstr) = 0                 then Tagain = ComHangup()
        when pos( Mdm.Okay       , Tstr ) = 1 then Tagain = G.False
        when pos( Mdm.Error      , Tstr ) = 1 then Tagain = G.False
        when pos( Mdm.NoCarrier  , Tstr ) = 1 then Tconnected = G.False
        when pos( Mdm.NoConnect0 , Tstr ) = 1 then Tconnected = G.False
        when pos( Mdm.NoConnect1 , Tstr ) = 1 then Tconnected = G.False
        when pos( Mdm.NoConnect2 , Tstr ) = 1 then Tconnected = G.False
        when pos( Mdm.NoConnect3 , Tstr ) = 1 then Tconnected = G.False
        when pos( Mdm.NoConnect4 , Tstr ) = 1 then Tconnected = G.False
        when pos( Mdm.NoConnect5 , Tstr ) = 1 then Tconnected = G.False
        when pos( Mdm.NoConnect6 , Tstr ) = 1 then Tconnected = G.False
        when pos( Mdm.NoConnect7 , Tstr ) = 1 then Tconnected = G.False
        when pos( Mdm.NoConnect8 , Tstr ) = 1 then Tconnected = G.False
        when pos( Mdm.NoConnect9 , Tstr ) = 1 then Tconnected = G.False
        when pos( Mdm.Connect0   , Tstr ) = 1 then Tconnected = G.True
        when pos( Mdm.Connect1   , Tstr ) = 1 then Tconnected = G.True
        when pos( Mdm.Connect2   , Tstr ) = 1 then Tconnected = G.True
        when pos( Mdm.Connect3   , Tstr ) = 1 then Tconnected = G.True
        when pos( Mdm.Connect4   , Tstr ) = 1 then Tconnected = G.True
        when pos( Mdm.Connect5   , Tstr ) = 1 then Tconnected = G.True
        when pos( Mdm.Connect6   , Tstr ) = 1 then Tconnected = G.True
        when pos( Mdm.Connect7   , Tstr ) = 1 then Tconnected = G.True
        when pos( Mdm.Connect8   , Tstr ) = 1 then Tconnected = G.True
        when pos( Mdm.Connect9   , Tstr ) = 1 then Tconnected = G.True
        otherwise Tagain = ComHangup()
      end
      if Tconnected then do
         Tagain = G.False
         if length(Mdm.Online) > 0 then do
            call ComWrite 0, Mdm.Online||Mdm.Enter
         end
         if \ComCarrier() then do
            Tconnected = G.False
         end
      end; else do
         call SayLog "Unable to make connection, Reason='"Tstr"'."G.CrLf
         if Tagain then do
            if Tcount >= Tretries then
               Tagain = G.False
            else do
               call RxAsyncSleep Tretrywait
               Tstr = ComSuck()
            end
         end
      end
   end
Return Tconnected

ComHangup:                             /* Hangup via close and open          */
   Procedure expose G. Dev. Mdm. Bbs.
   Tok = G.False
   if Dev.Hdl > 0 then do
      Tdev = Dev.Hdl
      Dev.Hdl = 0                      /* Prevent repeated call              */
      Trxc = RxAsyncClose( Tdev )
      if Trxc <> 0 then do
         call Abort "ComHangup: RxAsyncClose failed, Rc='"Trxc"'."
      end
      call TrcMsg "RxAsyncClose closed the device."
      Dev.Carrier = G.False            /* Should no longer have it           */
      call RxAsyncSleep 2000           /* Sleep 2 seconds                    */
      Tdev = 0
      Trxc = RxAsyncOpen( Dev.Port, "Tdev" )
      if Trxc <> 0 then do
         call Abort "ComHangup: RxAsyncOpen failed, Rc='"Trxc"'."
      end
      Dev.Hdl = Tdev
      call TrcMsg "RxAsyncOpen reopened port "Dev.Port", Handle='"Dev.Hdl"'."
      Tok = G.True
   end
Return Tok

/*****************************************************************************/
/* EXTERNAL PROTOCOL ROUTINES ############################################## */
/*****************************************************************************/

InitProt:                              /* Set required environment           */
   Procedure expose G. Dev. Mdm. Bbs.
   /* Ensure path and dpath set correctly for the M2Zmodem external protocol */
   G.OldPath  = value("PATH", ,"OS2ENVIRONMENT")
   G.OldDpath = value("DPATH",,"OS2ENVIRONMENT")
   G.NewPath  = value("PATH",  G.OldPath||"E:\PROGRAMS\OS2\M2ZMODEM;","OS2ENVIRONMENT")
   G.NewDPath = value("DPATH",G.OldDPath||"E:\PROGRAMS\OS2\M2ZMODEM;","OS2ENVIRONMENT")
   G.ProtIni  = G.True
Return

FiniProt:                              /* Reset environment                  */
   Procedure expose G. Dev. Mdm. Bbs.
   if G.ProtIni then do
      G.ProtIni  = G.False             /* Prevent repeated call              */
      G.NewPath  = value("PATH",  G.OldPath,"OS2ENVIRONMENT")
      G.NewDPath = value("DPATH",G.OldDPath,"OS2ENVIRONMENT")
   end
Return

SendFile:                              /* Upload the specified file          */
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg Tprot, Tbaud, Tfname
   Tprot = translate( Tprot )          /* Convert to uppercase               */
   if substr(translate(Bbs.FtpStatus),1,1) == "Y" then
      Tpm = "-pm "
   else do
      Tpm = ""
   end
   /* default file path only if necessary */
   Tdir  = RxAsyncFilePathIs( Tfname )
   Tname = RxAsyncFileNameIs( Tfname )
   if length( Tdir ) = 0 then do
      Tdir = G.OutBox
   end
   Tfname = Tdir||Tname
   call SayLog "Uploading file '"Tfname"' using protocol '"Tprot"'."G.CrLf
   Tbegin = time("E")*1000
   select
     when Tprot = 'X' then
          '@M2ZMODEM -u 'Dev.Hdl' -b 'Tbaud' -h -o 'G.XfrFile' 'Tpm'-prty 2 -q -prot XMODEM -s 'Tfname
     when Tprot = '1' then
          '@M2ZMODEM -u 'Dev.Hdl' -b 'Tbaud' -h -o 'G.XfrFile' 'Tpm'-prty 2 -q -prot XMODEM1K -s 'Tfname
     when Tprot = 'Z' then
          '@M2ZMODEM -u 'Dev.Hdl' -b 'Tbaud' -h -o 'G.XfrFile' 'Tpm'-prty 2 -q -s 'Tfname
     otherwise
          RC = 'Protocol unknown'
   end
   Tok = (RC = 0)
   if \Tok then do
      call SayLog "Upload failed, Rc='"RC"'."G.CrLf
      call ComWrite 0, G.CtrlX
      call ComWrite 0, G.CtrlX
      call ComWrite 0, G.CtrlX
      call ComWrite 0, G.CtrlX
      call ComWrite 0, G.CtrlX
      call ComWrite 0, G.CtrlX
   end
   Ttook = (time("E")*1000) - Tbegin
   call SayLog 'Upload time was 'trunc(Ttook/60000)' mins 'trunc((Ttook/10)-(trunc(Ttook/60000)*6000))/100' secs.'G.CrLf
Return Tok

ReceiveFile:                           /* Download to the specified path     */
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg Tprot, Tbaud, Tfname
   Tprot = translate( Tprot )          /* Convert to uppercase               */
   if substr(translate(Bbs.FtpStatus),1,1) == "Y" then
      Tpm = "-pm "
   else do
      Tpm = ""
   end
   /* default file path only if necessary */
   Tdir  = RxAsyncFilePathIs( Tfname )
   Tname = RxAsyncFileNameIs( Tfname )
   if length( Tdir ) = 0 then do
      Tdir = G.InBox
   end
   Tfname = Tdir||Tname
   call SayLog "Downloading '"Tname"' to '"Tdir"' using protocol '"Tprot"'."G.CrLf
   Tbegin = time("E")*1000
   select
     when Tprot = 'X' then
          '@M2ZMODEM -u 'Dev.Hdl' -b 'Tbaud' -h -o 'G.XfrFile' 'Tpm'-prty 2 -q -ren -prot XMODEM -r 'Tfname
     when Tprot = '1' then
          '@M2ZMODEM -u 'Dev.Hdl' -b 'Tbaud' -h -o 'G.XfrFile' 'Tpm'-prty 2 -q -ren -prot XMODEM1K -r 'Tfname
     when Tprot = 'Z' then
          '@M2ZMODEM -u 'Dev.Hdl' -b 'Tbaud' -h -o 'G.XfrFile' 'Tpm'-prty 2 -q -ren -r 'Tdir
     otherwise
          RC = 'Protocol unknown'
   end
   Tok = (RC = 0)
   if \Tok then do
      call SayLog "Download failed, Rc='"RC"'."G.CrLf
      call ComWrite 0, G.CtrlX
      call ComWrite 0, G.CtrlX
      call ComWrite 0, G.CtrlX
      call ComWrite 0, G.CtrlX
      call ComWrite 0, G.CtrlX
      call ComWrite 0, G.CtrlX
   end
   Ttook = (time("E")*1000) - Tbegin
   call SayLog 'Download time was 'trunc(Ttook/60000)' mins 'trunc((Ttook/10)-(trunc(Ttook/60000)*6000))/100' secs.'G.CrLf
Return Tok

/*****************************************************************************/
/* ABORT AND MESSAGE HANDLING ############################################## */
/*****************************************************************************/

SayMsg:                                /* Write output without logging       */
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg Tstr
   if length( Tstr ) > 0 then do
      if \G.Quietly then call RxAsyncPrint Tstr
      call WriteTrc Tstr
      call WriteCap Tstr
   end
Return

SayLog:                                /* Write output with logging          */
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg Tstr
   if length( Tstr ) > 0 then do
      if \G.Quietly then call RxAsyncPrint Tstr
      call WriteTrc Tstr
      call WriteCap Tstr
      call WriteLog Tstr
   end
Return

TrcMsg:                                /* Write output to trace only         */
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg Tstr
   if length( Tstr ) > 0 then do
      call WriteTrc Tstr
   end
Return

Abort:                                 /* Display error, cleanup and exit    */
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg Tstr
   if \G.Abort then do
      call SayLog date('N')||' '||time('C')||" => Error: "||Tstr||G.CrLf
      G.Abort = G.True                 /* Prevent repeated messages          */
   end
   call Cleanup
   exit
Return

/*****************************************************************************/
/* TRACE FILE ROUTINES ##################################################### */
/*****************************************************************************/

OpenTrc:                               /* Open session trace file            */
   Procedure expose G. Dev. Mdm. Bbs.
   if G.WantTrc then do
      G.TrcOpen = G.False
      /* Clear out trace file, only ever want to trace current session */
      if RxAsyncFileExists( G.TrcFile ) then do
         if \RxAsyncFileDelete( G.TrcFile ) then do
            call Abort "OpenTrc: Unable to clear trace file '"G.TrcFile"'."
         end
      end
      Trxc = lineout( G.TrcFile, "RxScript: Tracing commenced on "date('N')" at "time('C')"." )
      if Trxc <> 0 then do
         call Abort "OpenTrc: Could not open trace file '"G.TrcFile"', Rc='"Trxc"'."
      end
      G.TrcOpen = G.True
   end
Return

WriteTrc:                              /* Write to session trace file        */
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg Tstr
   if G.TrcOpen then do
      Tstr = strip( Tstr, 'T', D2C(10) )
      Tstr = strip( Tstr, 'T', D2C(13) )
      Trxc = lineout( G.TrcFile, Tstr )
      if Trxc <> 0 then do
         call Abort "WriteTrc: Could not write to trace file '"G.TrcFile"', Rc='"Trxc"'."
      end
   end
Return

CloseTrc:                              /* Close session trace file           */
   Procedure expose G. Dev. Mdm. Bbs.
   if G.TrcOpen then do
      G.TrcOpen = G.False              /* Prevent repeated call              */
      Trxc = lineout( G.TrcFile )
      if Trxc <> 0 then do
         call Abort "CloseTrc: Could not close trace file '"G.TrcFile"', Rc='"Trxc"'."
      end
   end
Return

/*****************************************************************************/
/* LOG FILE ROUTINES ####################################################### */
/*****************************************************************************/

OpenLog:                               /* Open session log file              */
   Procedure expose G. Dev. Mdm. Bbs.
   if G.WantLog then do
      G.LogOpen = G.False
      Trxc = lineout( G.LogFile, "--------------------------------------------------------------" )
      if Trxc <> 0 then do
         call Abort "OpenLog: Could not open log file '"G.LogFile"', Rc='"Trxc"'."
      end
      G.LogOpen = G.True
   end
Return

WriteLog:                              /* Write to session log file          */
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg Tstr
   if G.LogOpen then do
      Tstr = strip( Tstr, 'T', D2C(10) )
      Tstr = strip( Tstr, 'T', D2C(13) )
      Trxc = lineout( G.LogFile, Tstr )
      if Trxc <> 0 then do
         call Abort "WriteLog: Could not write to log file '"G.LogFile"', Rc='"Trxc"'."
      end
   end
Return

CloseLog:                              /* Close session log file             */
   Procedure expose G. Dev. Mdm. Bbs.
   if G.LogOpen then do
      G.LogOpen = G.False              /* Prevent repeated call              */
      Trxc = lineout( G.LogFile )
      if Trxc <> 0 then do
         call Abort "CloseLog: Could not close log file '"G.LogFile"', Rc='"Trxc"'."
      end
   end
Return

/*****************************************************************************/
/* CAPTURE FILE ROUTINES ################################################### */
/*****************************************************************************/

OpenCap:                               /* Open capture file                  */
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg Tfile
   Tname = RxAsyncFileNameIs( Tfile )
   if Tname \== "" then do
      G.CapOpen = G.False
      Tdir  = RxAsyncFilePathIs( Tfile )
      if length( Tdir ) = 0 then do
         Tdir = G.PktBox
      end
      Trxc = lineout( Tdir||Tname, "---"date('N')"---"time('C')"----------------------------------------" )
      if Trxc <> 0 then
         call Abort "OpenCap: Could not open capture file '"Tdir||Tname"', Rc='"Trxc"'."
      else do
         G.CapFile = Tdir||Tname
         G.CapOpen = G.True
      end
   end
Return

WriteCap:                              /* Write to capture file              */
   Procedure expose G. Dev. Mdm. Bbs.
   parse arg Tstr
   if G.CapOpen then do
      Tstr = strip( Tstr, 'T', D2C(10) )
      Tstr = strip( Tstr, 'T', D2C(13) )
      Trxc = lineout( G.CapFile, Tstr )
      if Trxc <> 0 then do
         call Abort "WriteCap: Could not write to capture file '"G.CapFile"', Rc='"Trxc"'."
      end
   end
Return

CloseCap:                              /* Close capture file                 */
   Procedure expose G. Dev. Mdm. Bbs.
   if G.CapOpen then do
      G.CapOpen = G.False              /* Prevent repeated call              */
      Trxc = lineout( G.CapFile )
      if Trxc <> 0 then do
         call Abort "CloseCap: Could not close capture file '"G.CapFile"', Rc='"Trxc"'."
      end
      G.CapFile = ""
   end
Return

/*****************************************************************************/
/* END MODULE ############################################################## */
/*****************************************************************************/
