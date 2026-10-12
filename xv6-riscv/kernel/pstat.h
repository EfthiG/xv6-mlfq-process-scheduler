#ifndef _PSTAT_H_
#define _PSTAT_H_

#include "param.h"

#define PNAME_LEN 16

#define PST_UNUSED   0
#define PST_USED     1
#define PST_SLEEPING 2
#define PST_RUNNABLE 3
#define PST_RUNNING  4
#define PST_ZOMBIE   5


struct pstat {
  int inuse[NPROC];
  int pid[NPROC];
  int ppid[NPROC];
  int state[NPROC];        // procstate ως int
  int priority[NPROC];     // προς το παρόν 0, μετά θα μπει MLFQ
  uint64 sz[NPROC];
  char name[NPROC][PNAME_LEN];
};

#endif