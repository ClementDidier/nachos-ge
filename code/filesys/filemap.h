/**
 * \file filemap.h
 * \brief Gère les fichiers ouverts dans le system
*/
#ifndef __FILEMAP_H__
#define __FILEMAP_H__

#include "openfile.h"
class Thread;
class Lock;
class FileMap
{
public:

/**
 *\brief Nombre maximal de fichier ouvert dans le system et par thread
*/
  static const int map_size = 10;
  FileMap();
  ~FileMap();
/**
 * \fn OpenFile* Add(int key, OpenFile value)
 * \brief Ajoute un OpenFile a la table des fichiers ouverts, qui a pour secteur key
 * \param key Secteur du fichier à supprimer
 * \param value OpenFile qui doit être inséré dans la table
 * \return Retourne l'OpenFile si le fichier a pu être ajouté, NULL sinon
**/
  OpenFile* Add (int key, OpenFile* value);
/**
 * \fn bool Delete(int key)
 * \brief Supprime un fichier de la table des fichiers ouverts
 * \param key Secteur du fichier à supprimer
 * \return Retourn True si le fichier a pu être supprimer, false sinon
*/
  bool Delete (int key);
private:

/**
 * \brief protège l'accès a la table
*/
  Lock* tableLock;
  /**
   * \fn int FindPlace()
   * \brief Trouve un fichier déjà présent dans le system de fichier ouverts
   * \param key Secteur du fichier à trouver et à comparer.
   * \return Retourne un entier positif si le fichier a été trouvé qui Corresepond
   * à l'endroit  où il se trouve dans le tableau, -1 s'il ne s'y trouve pas.
  **/
  int Find(int key);

/**
 * \fn int FindPlace()
 * \brief Trouve un emplacement dans la table pour ranger un nouveau fichier ouvert
 * \return Retourne un entier positif si une place a été trouvé qui Corresepond
 * à l'endroit ou peut etre ajouté le fichier, -1 si la table est complète.
**/
  int FindPlace();

/**
 *
 * \brief table des fichier ouverts et des thread les possédant
*/
  int table[map_size][2];
/**
 * \fn bool isInThread(OpenFile * of, Thread * cr)
 * \brief Fonction interne qui définit si un fichier ouvert appartient au thread
 * qui essaye de l'ouvrir / le fermer
 * \param of OpenFile a vérifier
 * \param cr Thread dont on doit verifier si il est actuellement le propriétaire
 * du fichier ouvert
 * \return Renvoie true si ce fichier ouvert appartient au thread, sinon false
 *
*/
  bool isInThread(OpenFile * of, Thread * cr);
};

#endif //__FILEMAP_H__
