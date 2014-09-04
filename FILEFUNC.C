/*****************************************************************************/
/*                                                                           */
/*  MODULE         FILEFUNC.C                                                */
/*                                                                           */
/*  VERSION        Version 2.0 - 26th Dec 1994                               */
/*                                                                           */
/*  COPYRIGHT      Copyright (c) 1993, 1994 by Crucial Applications          */
/*                             All rights reserved.                          */
/*                                                                           */
/*  DESCRIPTION    File System Functions                                     */
/*                                                                           */
/*  FUNCTIONS                                                                */
/*                                                                           */
/*    fFileExists  - Test for file existence                                 */
/*    fFileDelete  - Delete specified file                                   */
/*    fFileCopy    - Copy specified file                                     */
/*    fFileRename  - Rename specified file                                   */
/*    fFileMove    - Move specified file                                     */
/*    FileNameIs   - Return name component of specified file                 */
/*    FilePathIs   - Return path component of specified file                 */
/*    FileChgDir   - Change current drive/directory                          */
/*    FileGetDir   - Query current drive/directory                           */
/*                                                                           */
/*****************************************************************************/

/*********************************************************************/
/* Includes needed by this module                                    */
/*********************************************************************/

#define  _MT
#define  _DLL
#define  INCL_DOS
#define  INCL_ERRORS

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "miscfunc.h"
#include "filefunc.h"

/*********************************************************************/
/* fFileExists()                                                     */
/*********************************************************************/
BOOL fFileExists( PSZ pszFileName )
{
   /*---------------------------------------------------------------*/
   /* Note: It is normally best to return TRUE (exists) if the find */
   /*       test fails for some unknown reason.  Thus any operation */
   /*       expecting the file to exist will be performed and that  */
   /*       will then fail/suceed.  If one was to return FALSE then */
   /*       operations expecting it not to exist ie.  create/copy   */
   /*       etc.  could possibly overwrite a file which does exist. */
   /*---------------------------------------------------------------*/

   BOOL         fExists;
   APIRET       rc;
   HDIR         hdirFindHandle;
   FILEFINDBUF  finfFileInfo;
   ULONG        ulFindCount;

   hdirFindHandle = HDIR_CREATE;
   ulFindCount    = 1;
   fExists        = TRUE;

   rc = DosFindFirst( (PSZ)    pszFileName,
                      (PHDIR)  &hdirFindHandle,
                      (ULONG)  FILE_NORMAL,
                      (PVOID)  &finfFileInfo,
                      (ULONG)  sizeof(finfFileInfo),
                      (PULONG) &ulFindCount,
                      (ULONG)  FIL_STANDARD );

   if ( (rc == NO_ERROR) || (rc == ERROR_NO_MORE_FILES) ) {

      if ( rc == ERROR_NO_MORE_FILES ) {
         fExists = FALSE;
      }

      if ( (rc == NO_ERROR) || (rc == ERROR_EAS_DIDNT_FIT) ) {
         rc = DosFindClose( hdirFindHandle );
         if ( rc != NO_ERROR ) {
            fprintf( stdout, "DosFindClose() failed with RC=%d.\r\n", rc );
         }
      }

   } else {
      fprintf( stdout, "DosFindFirst('%s') failed with RC=%d.\r\n", pszFileName, rc );
   }

   return( fExists );

} /* end fFileExists() */

/*********************************************************************/
/* fFileDelete()                                                     */
/*********************************************************************/
BOOL fFileDelete( PSZ pszFileName )
{
   /*-----------------------------------------------------------------*/
   /* Note: If it didn't exist then it may as well have been deleted. */
   /*-----------------------------------------------------------------*/

   BOOL   fDeleted;
   APIRET rc;

   fDeleted = FALSE;

   rc = DosDelete( pszFileName );
   if ( rc == NO_ERROR ) {
      fDeleted = TRUE;
   } else {

      if ( rc == ERROR_FILE_NOT_FOUND ) {
         fDeleted = TRUE;
      } else {
         fprintf( stdout, "DosDelete('%s') failed with RC=%d.\r\n", pszFileName, rc );
      }
   }

   return( fDeleted );

} /* end fFileDelete() */

/*********************************************************************/
/* fFileCopy()                                                       */
/*********************************************************************/
BOOL fFileCopy( PSZ pszFromName, PSZ pszToName )
{
   BOOL         fCopied;
   APIRET       rc;

   fCopied = FALSE;

   rc = DosCopy( pszFromName, pszToName, (ULONG) DCPY_EXISTING );
   if ( rc == NO_ERROR ) {
      fCopied = TRUE;
   } else {

      if ( rc != ERROR_FILE_NOT_FOUND ) {
         fprintf( stdout, "DosCopy('%s','%s') failed with RC=%d.\r\n", pszFromName, pszToName, rc );
      }
   }

   return( fCopied );

} /* end fFileCopy() */

/*********************************************************************/
/* fFileRename()                                                     */
/*********************************************************************/
BOOL fFileRename( PSZ pszFromName, PSZ pszToName )
{
   BOOL   fRenamed;
   APIRET rc;
   PSZ    pszNameBuff;                 /* Pointer to name buffer     */
   UCHAR  ucNameBuff[_MAX_DIR];        /* File name buffer           */
   PSZ    pszPath;
   PSZ    pszName;
   PSZ    pszColon;
   PSZ    pszSlash;

   fRenamed = FALSE;

   // Place the FromName into the file name buffer
   pszNameBuff = &ucNameBuff[0];
   strcpy( pszNameBuff, pszFromName );

   // Point to the start of the File component of FromName in buffer
   pszPath  = pszNameBuff;
   pszColon = strrchr( pszPath, DRIVE_DELIM_CH );
   pszSlash = strrchr( pszPath, PATH_DELIM_CH );
   if (pszSlash != NULL) {
      pszColon = pszSlash;
   }
   if (pszColon != NULL) {
      pszColon++;
      pszPath = pszColon;
   }

   // Point to the start of the File component of ToName
   pszName  = pszToName;
   pszColon = strrchr( pszName, DRIVE_DELIM_CH );
   pszSlash = strrchr( pszName, PATH_DELIM_CH );
   if (pszSlash != NULL) {
      pszColon = pszSlash;
   }
   if (pszColon != NULL) {
      pszColon++;
      pszName = pszColon;
   }

   // Copy the ToName File component into the buffer
   strcpy( pszPath, pszName );

   rc = DosMove( pszFromName, pszNameBuff );
   if ( rc == NO_ERROR ) {
      fRenamed = TRUE;
   } else {

      if ( rc != ERROR_FILE_NOT_FOUND ) {
         fprintf( stdout, "DosMove('%s', '%s') failed with RC=%d.\r\n", pszFromName, pszToName, rc );
      }
   }

   return( fRenamed );

} /* end fFileRename() */

/*********************************************************************/
/* fFileMove()                                                       */
/*********************************************************************/
BOOL fFileMove( PSZ pszFromName, PSZ pszToName )
{
   /*----------------------------------------------------------------*/
   /* Note: Doing it this way means we can move files across drives. */
   /*----------------------------------------------------------------*/

   BOOL   fMoved;
   APIRET rc;

   fMoved = FALSE;

   if ( !fFileExists( pszToName ) ) {
      if ( fFileCopy( pszFromName, pszToName ) ) {

         if ( fFileDelete( pszFromName ) ) {
            fMoved = TRUE;
         } else {
            fFileDelete( pszToName );
         }
      }
   }

   return( fMoved );

} /* end fFileMove() */

/*********************************************************************/
/* FileNameIs()                                                      */
/*********************************************************************/
VOID FileNameIs( PSZ pszFile, PSZ pszName )
{
   /*--------------------------------------------*/
   /* Note: Caller must allocate buffer pszName. */
   /*--------------------------------------------*/

   PSZ pszColon;
   PSZ pszSlash;

   pszName[0] = EOSTR_CH;

   if ( pszFile != NULL ) {
      if ( strlen(pszFile) > 0 ) {

         pszColon = strrchr( pszFile, DRIVE_DELIM_CH );
         pszSlash = strrchr( pszFile, PATH_DELIM_CH );

         if (pszSlash != NULL) {
            pszColon = pszSlash;
         }

         if (pszColon != NULL) {
            pszColon++;
            strcpy( pszName, pszColon );
         } else {
            strcpy( pszName, pszFile );
         }
      }
   }

   return;

} /* end FileNameIs() */

/*********************************************************************/
/* FilePathIs()                                                      */
/*********************************************************************/
VOID FilePathIs( PSZ pszFile, PSZ pszPath )
{
   /*--------------------------------------------*/
   /* Note: Caller must allocate buffer pszPath. */
   /*--------------------------------------------*/

   PSZ pszColon;
   PSZ pszSlash;

   pszPath[0] = EOSTR_CH;

   if ( pszFile != NULL ) {
      if ( strlen(pszFile) > 0 ) {

         strcpy( pszPath, pszFile );

         pszColon = strrchr( pszPath, DRIVE_DELIM_CH );
         pszSlash = strrchr( pszPath, PATH_DELIM_CH );

         if (pszSlash != NULL) {
            pszColon = pszSlash;
         }

         if (pszColon != NULL) {
            pszColon++;
         } else {
            pszColon = pszPath;
         }

         pszColon[0] = EOSTR_CH;
      }
   }

   return;

} /* end FilePathIs() */

/*********************************************************************/
/* FileChgDir()                                                      */
/*********************************************************************/
VOID FileChgDir( PSZ pszNewDir )
{
   APIRET  rc;               /* API return code                      */
   ULONG   ulDrive;          /* Default drive number (A=1,..,Z=26)   */
   ULONG   ulPathLen;        /* Directory path buffer length         */
   PSZ     pszPath;          /* Pointer to path buffer               */
   UCHAR   ucPath[_MAX_DIR]; /* Directory path buffer                */

   rc = NO_ERROR;

   pszPath = &ucPath[0];

   if ( strlen(pszNewDir) > 0 ) {

      strcpy( pszPath, pszNewDir );

      ulPathLen = strlen( pszPath );

      /* remove trailing backslash if present and not root */
      if ( ulPathLen > 1 ) {
         if (   (pszPath[ulPathLen-1] == PATH_DELIM_CH)
             && (pszPath[ulPathLen-2] != DRIVE_DELIM_CH) ) {
            pszPath[ulPathLen-1] = EOSTR_CH;
         }
      }

      /* change to new drive if specified */
      if ( pszPath[1] == DRIVE_DELIM_CH ) {
         ulDrive = (ULONG)( (BYTE)pszPath[0] - (BYTE)'A' ) + 1;
         DosError( FERR_DISABLEHARDERR | FERR_DISABLEEXCEPTION );
         rc = DosSetDefaultDisk( ulDrive );
         DosError( FERR_ENABLEHARDERR | FERR_ENABLEEXCEPTION );
      }

      if ( rc != NO_ERROR ) {
         fprintf( stdout, "DosSetDefaultDisk('%c:') failed with RC=%d.\r\n", pszPath[0], rc );
      } else {

         /* change to new directory */
         rc = DosSetCurrentDir( pszPath );
         if ( rc != NO_ERROR ) {
            fprintf( stdout, "DosSetCurrentDir('%s') failed with RC=%d.\r\n", pszPath, rc );
         }
      }
   }

   return;

} /* end FileChgDir() */

/*********************************************************************/
/* FileGetDir()                                                      */
/*********************************************************************/
VOID FileGetDir( PSZ pszCurDir )
{
   /*----------------------------------------------*/
   /* Note: Caller must allocate buffer pszCurDir. */
   /*----------------------------------------------*/

   APIRET  rc;               /* API return code                      */
   ULONG   ulDrive;          /* Default drive number (A=1,..,Z=26)   */
   ULONG   ulMap;            /* Drive map (bits 0-25 indicate valid) */
   ULONG   ulPathLen;        /* Directory path buffer length         */
   UCHAR   ucPath[_MAX_DIR]; /* Directory path buffer                */
   PSZ     pszPath;          /* Pointer to path buffer               */

   pszPath = &ucPath[0];

   pszCurDir[0] = EOSTR_CH;

   rc = DosQueryCurrentDisk( &ulDrive, &ulMap );
   if ( rc != NO_ERROR ) {
      fprintf( stdout, "DosQueryCurrentDisk() failed with RC=%d.\r\n", rc );
   } else {

      pszPath[0] = EOSTR_CH;
      ulPathLen = _MAX_DIR;

      rc = DosQueryCurrentDir( ulDrive, pszPath, &ulPathLen );
      if ( rc != NO_ERROR ) {
         fprintf( stdout, "DosQueryCurrentDir() failed with RC=%d.\r\n", rc );
      } else {

         /* return drive spec */
         pszCurDir[0] = (CHAR)( (BYTE)'A' + (BYTE)(ulDrive-1) );
         pszCurDir[1] = DRIVE_DELIM_CH;
         pszCurDir[2] = PATH_DELIM_CH;
         pszCurDir[3] = EOSTR_CH;

         /* append path if any */
         if (strlen( pszPath ) > 0) {
            strcat( pszCurDir, pszPath );
            ulPathLen = strlen( pszCurDir );

            /* append trailing backslash if room */
            if (ulPathLen < _MAX_DIR) {
               pszCurDir[ulPathLen] = PATH_DELIM_CH;
               ulPathLen++;
               pszCurDir[ulPathLen] = EOSTR_CH;
            }
         }
      }
   }

   return;

} /* end FileGetDir() */

/*********************************************************************/
/* END MODULE                                                        */
/*********************************************************************/
