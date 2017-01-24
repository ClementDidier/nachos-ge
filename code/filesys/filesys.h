/**
 * \file filesys.h
 * \brief implemente des fonctions pour la gestion des fichiers
*/
// filesys.h
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system.
//	The "STUB" version just re-defines the Nachos file system
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.  This is provided in case the
//	multiprogramming and virtual memory assignments (which make use
//	of the file system) are done before the file system assignment.
//
//	The other version is a "real" file system, built on top of
//	a disk simulator.  The disk is simulated using the native UNIX
//	file system (in a file named "DISK").
//
//	In the "real" implementation, there are two key data structures used
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "openfile.h"
#include "filehdr.h"
#include "filemap.h"
#ifdef USER_PROGRAM
//#include "system.h"
#endif
class FileMap;
#ifdef FILESYS_STUB 		// Temporarily implement file system calls as
				// calls to UNIX, until the real file system
				// implementation is available
class FileSystem {
  public:
    FileSystem(bool format) {}

    bool Create(const char *name, int initialSize) {
	int fileDescriptor = OpenForWrite(name);

	if (fileDescriptor == -1) return FALSE;
	Close(fileDescriptor);
	return TRUE;
	}

    OpenFile* Open(char *name) {
	  int fileDescriptor = OpenForReadWrite(name, FALSE);

	  if (fileDescriptor == -1) return NULL;
	  return new OpenFile(fileDescriptor);
      }

    bool Remove(char *name) { return Unlink(name) == 0; }

};

#else // FILESYS
class FileSystem {
  public:

  /**
   * \fn void FileSystem(bool format);
   * \brief Initialize the file system.
   *  Must be called *after* "synchDisk" has been initialized.
   * If "format", there is nothing on the disk, so initialize the directory and the bitmap of free blocks.
  */
    FileSystem(bool format);

  /**
   * \fn void Close(const char *name, int initialSize, FileHeader::Type type = FileHeader::f);
   * \brief créer un fichier ou un dossier dans le repertoire courrant
   * \param name nom du ficheir ou repertoire à créer
   * \param initialSize taille initial du fichier ou repertoire à créer (int)
   * \param type f en cas de fichier, d en cas de repertoire
   * \return true(1) si on a pu créer le repertoire ou le fichier, 0 sinon
  */
    bool Create(const char *name, int initialSize, FileHeader::Type type = FileHeader::f);
					// Create a file (UNIX creat)

  /**
   * \fn void Close(const char * buffer);
   * \brief Open a file (UNIX open)
  */
    OpenFile* Open(const char *name);

  /**
   * \fn void Close(const char * buffer);
   * \brief ferme une fichier
  */
    void Close(const char *name);

  /**
   * \fn void Remove(const char * buffer);
   * \brief Delete a file (UNIX unlink)
  */
    bool Remove(const char *name);

  /**
   * \fn void Print(const char * buffer);
   * \brief List all the files in the file system
  */
    void List(); 

  /**
   * \fn void Print(const char * buffer);
   * \brief List all the files and their contents
  */
    void Print();

  /**
   * \fn void CreateDir(const char * buffer);
   * \brief suppime le repertoire name du dossier courrant. 
   * \param name une chaine de taille max 9 (taille maximum de nom de fichier) qui contient un nom de dossier du repertoire courrant (. et .. exclus).
   * \return retourne true(1) si on a pu créer le repertoire, false(0) sinon.
  */
    bool CreateDir(const char* name);

  /**
   * \fn void DeleteDir(const char * buffer);
   * \brief suppime le repertoire name du dossier courrant. 
   * \param name une chaine de taille max 9 (taille maximum de nom de fichier) qui contient un nom de dossier du repertoire courrant (. et .. exclus).
   * \return retourne true(1) si on a pu supprimer le repertoire, false(0) sinon.
  */
    bool DeleteDir(const char * name);

  /**
   * \fn void ChangeDir(const char * buffer);
   * \brief recherche le nom du dossier passé en paramètre, recherche le secteur associé et change l'emplacement
   * \param name une chaine de taille max 9 (taille maximum de nom de fichier) qui contient un nom de dossier du repertoire courrant (. et .. inclu).
   * \return retourne 1 si on a pu changer de repertoire, 0 sinon.
  */
    int ChangeDir(const char* name);

  /**
   * \fn void ChangeDirPath(const char * buffer);
   * \brief analyse la chaine passé en paramètre 
   * \param name une chaine de taille max 9 (taille maximum de nom de fichier) qui contient un chemin de repertoire absolu ou relatif.
   * \return retourne 1 si on a pu changer de repertoire, 0 sinon.
  */
    int ChangeDirPath(const char* name);
  /**
   * \fn void clearBuffer(char * buffer);
   * \brief réinitialise le buffer passé en paramètre  
   * \param buffer doit être de taille MAX_STRING_SIZE
  */
    void clearBuffer(char * buffer);
  private:
   OpenFile* freeMapFile;		// Bit map of free disk blocks,
					// represented as a file
   OpenFile* directoryFile;		// "Root" directory -- list of
					// file names, represented as a file
   FileMap* OpenedFiles;
};

#endif // FILESYS

#endif // FS_H
