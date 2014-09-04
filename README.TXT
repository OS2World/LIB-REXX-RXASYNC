
                                 RXASYNC

                       Version 2.0 - 26th Dec 1994


              Copyright (c) 1993, 1994 by Crucial Applications
                           All rights reserved.


    The enclosed REXX DLL comprises a set of functions which are used to to
    extend the REXX language by providing FULL exposure of the ASYNC
    communications API.  In addition a number of higher level functions are
    also provided to simplify the task of enabling REXX to be used as a
    scripting language for serial communications control.

    They are by no means perfect but do function sufficiently well for the
    author who uses them to provide a means of automating the collection of
    mail from the BBS.  If you have any comments or suggestions for
    improvement then I'd be glad to hear from you.

    Documentation has never been one of my strong points, so in order to
    alleviate some grief on my part and disk space on yours I have elected
    not to include a seperate set of documentation.

    The reason for this will become obvious when you examine the source
    code file (RXASYNC.c) which contains all the necessary information on
    every function in the package including syntax and examples.

    Further details on the settings and parameter values returned from the
    calls to the ASYNC API are present in the header files which include
    everything the standard OS/2 header file provides on the subject plus
    all the missing (not to mention essential) definitions needed to
    complete the picture.

    I do not expect novice users to be delving into this area too quickly,
    if you are new to OS/2 or REXX then I'd like to suggest trying one or
    two of the shareware OS/2 communications packages that provide
    scripting facilities as these may well be of greater value to you.

                          --------------------

    If anyone is using the RXASYNC package and has been unable to get the
    RxAsync.Cmd script to work with a COM port that has an 8250/16540 UART
    then please alter the 'newFlags3' variable to '00000010'.

    This correctly adjusts the setting of buffering options in the
    RxAsyncSetDcbInfo call for the non-buffered UART.

    (Many thanks to Andre Boisvert for pointing this out to me.)

    BTW. If you are unfamiliar with the DcbInfo options and are unable
    to decipher the information in the (.h) header file, and don't have
    the OS/2 Physical Device Driver reference,  then a good souce of
    information is the SIOREF document supplied by Ray Gwinn with his
    SIO drivers.

                          --------------------

    The DLL is fully upward compatible with the previous versions and
    should function correctly with all existing scripts (bugs and
    bug fixes not withstanding).

                          --------------------

    Enclosed is a zip file called RXSCRIPT.zip which contains sample
    files from the BBS scripts that I currently use to access Bunyips
    Cave BBS.  Bunyips run Maximus for OS/2 and my scripts currently
    rely heavily on the content of prompts received and also utilise
    the M2ZMODEM package among other things.

    Please _do_not_ expect these scripts to work for you (not without
    some modification<g>), they have been included in order to show
    how RxAsync is currently being used by the author and to hopefully
    give some ideas to those of you using the RxAsync package.

    The scripts that should work are RXASYNC.cmd, RXMSG.cmd and
    MDMRESET.cmd (asuming you use the correct modem parameters).

                          --------------------

    If you find RXASYNC useful then please send me a short message
    telling me what you think and detailing what you are using it
    for. This will help me in determining priorities of future
    enhancements and hopefully give me a few more ideas for additions.

                          --------------------

    The author can be contacted (in order of preference) at:

         Bunyips Cave BBS    3:633/379

         Internet            itimms@ariel.ucs.unimelb.edu.au

         Compuserve          100236,1404

         Snail mail          Ian Timms,
                             13 Cowper Street
                             Footscray, Victoria,
                             Australia, 3011

                          --------------------

