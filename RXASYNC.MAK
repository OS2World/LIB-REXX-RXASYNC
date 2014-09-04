# IBM Developer's Workframe/2 Make File Creation run at 14:45:08 on 12/26/94

# Make File Creation run in directory:
#   D:\USR\OS2\RXASYNC;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx

RXASYNC.DLL:  \
  COMSFUNC.OBJ \
  FILEFUNC.OBJ \
  MISCFUNC.OBJ \
  RXASYNC.OBJ \
  RXVPFUNC.OBJ \
  RXASYNC.MAK
   ICC.EXE @<<
 /Fe"RXASYNC.DLL" REXX.LIB RXASYNC.DEF 
COMSFUNC.OBJ 
FILEFUNC.OBJ 
MISCFUNC.OBJ 
RXASYNC.OBJ 
RXVPFUNC.OBJ
<<
  IMPLIB RXASYNC.LIB RXASYNC.DLL

{.}.c.obj:
   ICC.EXE /Ss /Q /V"RXASYNC Version 2.0 by Ian Timms, 26th Dec 1994." /Gm /Ge- /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Ss /Q /V"RXASYNC Version 2.0 by Ian Timms, 26th Dec 1994." /Gm /Ge- /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Ss /Q /V"RXASYNC Version 2.0 by Ian Timms, 26th Dec 1994." /Gm /Ge- /C   .\$*.cxx

!include RXASYNC.DEP
