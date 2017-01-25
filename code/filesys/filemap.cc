/**
 * \file filemap.cc
 * \brief Gère les fichiers ouverts dans le systeme
*/

#include "filemap.h"
#include "openfile.h"
#include "system.h"

FileMap::FileMap(){
  for(int i = 0; i < map_size; ++i)
  {
    table[i][0] = -1;
  }
  tableLock = new Lock("Opened file lock");
}
FileMap::~FileMap(){
  delete tableLock;
}

OpenFile* FileMap::Add (int key, OpenFile* value){
  tableLock->Acquire();
  int addr = Find(key);
  if (addr > 0)
    if(isInThread((OpenFile *) table[addr][1], currentThread)){
      for(int i = 0; i < FileMap::map_size; ++i)
        if(currentThread->FicOuverts[i] == NULL)
          currentThread->FicOuverts[i] = (OpenFile *) table[addr][1];
      tableLock->Release();
      return (OpenFile *) table[addr][1];
    }
    else{
      printf("This file is already opened by another thread.\n");
      tableLock->Release();
      return NULL;
    }
  else if( addr < 0)
  {
    int place = FindPlace();
    if (place < 0)
    {
        DEBUG('f', "Can't OpenFile with sector %d, too many files already opened\n", key);
        tableLock->Release();
        return NULL;
    }
    else
    {
      for(int i = 0; i < FileMap::map_size; ++i)
      {
        if(currentThread->FicOuverts[i] == NULL){
          currentThread->FicOuverts[i] = (OpenFile *) table[addr][1];
          break;
        }
      }
      table[place][0] = key;
      table[place][1] = (int) value;
      for(int i = 0; i < FileMap::map_size; ++i)
      {
        if(currentThread->FicOuverts[i] == NULL)
        {
          currentThread->FicOuverts[i] = (OpenFile *) table[place][1];
          break;
        }
      }
      tableLock->Release();
      return (OpenFile *) table[place][1];
    }
  }
  tableLock->Release();
  return NULL;
}
bool FileMap::Delete (int key)
{
  tableLock->Acquire();
  int it = Find(key);
  if (it >= 0 && isInThread((OpenFile *) table[it][1], currentThread)){
    delete (OpenFile *) table[it][1];
    table[it][0] = -1;
    tableLock->Release();
    return true;
  }
  else if( it < 0){
    printf("This file isn't open.\n");
  }
  else if(it >= 0 && !isInThread((OpenFile *) table[it][1], currentThread))
  {
    printf("This thread didn't open this file and therefore can't close it.\n");
  }
  tableLock->Release();
  return false;
}
int FileMap::Find(int key)
{
  for (int i = 0; i < map_size; ++i){
    if (table[i][0] == key)
      return i;
  }
  return -1;
}

int FileMap::FindPlace()
{
  for (int i = 0; i < map_size; ++i){
    if (table[i][0] == -1)
      return i;
  }
  return -1;
}
bool FileMap::isInThread(OpenFile * of, Thread * cr)
{
  for(int i = 0; i < FileMap::map_size; ++i)
  {
    printf("%d == %d\n", (int) cr->FicOuverts[i], (int) of );
    if( cr->FicOuverts[i] == of)
      return true;
  }
  return false;
}
