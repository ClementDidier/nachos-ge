/**
 * \file userthread.h
 * \brief Permet la création et l'execution de thread utilisateurs
*/

#ifdef CHANGED

struct userThreadParams;
extern int do_UserThreadCreate(int f, int arg);
extern int do_UserThreadExit();
extern int do_UserThreadJoin(int tid);

#endif
