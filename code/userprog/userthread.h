/**
 * \file userthread.h
 * \brief Permet la création et l'execution de thread utilisateurs
*/


struct userThreadParams;

struct argRetparams{
  int arg;
  int retaddr;
};
extern int do_UserThreadCreate(int f, int arg);
extern void do_UserThreadExit();
extern int do_UserThreadJoin(int tid);
