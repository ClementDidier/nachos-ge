/**
 * \file userthread.h
 * \brief Permet la création et l'execution de thread utilisateurs
*/

#ifdef CHANGED

struct userThreadParams;

struct argRetparams{
  int arg;
  int retaddr;
};
extern int do_UserThreadCreate(int f, int arg);
extern void do_UserThreadExit();

#endif
