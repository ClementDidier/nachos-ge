#ifdef CHANGED
#include "synch.h"

extern Semaphore * do_UserSemCreate(char * name, int nb);
extern void do_UserSemP(Semaphore * sem);
extern void do_UserSemV(Semaphore * sem);
extern void do_UserSemDelete(Semaphore * sem);

#endif
