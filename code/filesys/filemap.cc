#include "filemap.h"
#include "openfile.h"

FileMap::FileMap(){
  for(int i = 0; i < map_size; ++i)
  {
    table[i][0] = -1;
  }
}

OpenFile* FileMap::Add (int key, OpenFile* value){
  int add = Find(key);
  if (add > 0)
    return (OpenFile *) table[add][1];
  else if( add < 0)
  {
    int place = FindPlace();
    if (place < 0)
    {
        DEBUG('f', "Can't OpenFile with sector %d, too many files already opened\n", key);
        return NULL;
    }
    table[place][0] = key;
    table[place][1] = (int) value;
    return (OpenFile *) table[place][1];
  }
  return NULL;
}
void FileMap::Delete (int key)
{
  int it = Find(key);
  if (it >= 0){
    delete (OpenFile *) table[it][1];
    table[it][0] = -1;
  }
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
