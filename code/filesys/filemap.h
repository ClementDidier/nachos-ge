#include "openfile.h"
class FileMap
{
public:
  static const int map_size = 10;
  FileMap();
  //~FileMap();
  OpenFile* Add (int key, OpenFile* value);
  void Delete (int key);
private:
  int Find(int key);
  int FindPlace();
  int table[map_size][2];
};
