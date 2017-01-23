
#ifndef __FILEMAP_H__
#define __FILEMAP_H__

#include "openfile.h"
class Thread;
class Lock;
class FileMap
{
public:
  static const int map_size = 10;
  FileMap();
  ~FileMap();
  OpenFile* Add (int key, OpenFile* value);
  bool Delete (int key);
private:
  Lock* tableLock;
  int Find(int key);
  int FindPlace();
  int table[map_size][2];

  bool isInThread(OpenFile *, Thread * );
};

#endif //__FILEMAP_H__
