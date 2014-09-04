/*****************************************************************************/
/*                                                                           */
/*  MODULE         FILEFUNC.H                                                */
/*                                                                           */
/*  VERSION        Version 2.0 - 26th Dec 1994                               */
/*                                                                           */
/*  COPYRIGHT      Copyright (c) 1993, 1994 by Crucial Applications          */
/*                             All rights reserved.                          */
/*                                                                           */
/*  DESCRIPTION    Header file for FILEFUNC.C File System Functions          */
/*                                                                           */
/*****************************************************************************/

#ifndef FILEFUNC_H_INCLUDED
#define FILEFUNC_H_INCLUDED

/*********************************************************************/
/* General definitions                                               */
/*********************************************************************/

#define  DRIVE_DELIM_CH  ':'           /* Drive delimiter as char    */
#define  PATH_DELIM_CH   '\\'          /* Path delimiter  as char    */
#define  FILE_DELIM_CH   '.'           /* File delimiter  as char    */

/*********************************************************************/
/* Prototypes                                                        */
/*********************************************************************/

BOOL fFileExists( PSZ pszFileName );
BOOL fFileDelete( PSZ pszFileName );
BOOL fFileCopy( PSZ pszFromName, PSZ pszToName );
BOOL fFileRename( PSZ pszFromName, PSZ pszToName );
BOOL fFileMove( PSZ pszFromName, PSZ pszToName );

VOID FileNameIs( PSZ pszFile, PSZ pszName );
VOID FilePathIs( PSZ pszFile, PSZ pszPath );
VOID FileChgDir( PSZ pszNewDir );
VOID FileGetDir( PSZ pszCurDir );

/*********************************************************************/

#endif

/*********************************************************************/
/* END MODULE                                                        */
/*********************************************************************/
