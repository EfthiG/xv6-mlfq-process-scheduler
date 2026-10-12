#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/pstat.h"

static const char*
state_to_str(int st)
{
  switch(st){
  case PST_UNUSED:   
    return "UNUSED";
  case PST_USED:     
    return "USED";
  case PST_SLEEPING: 
    return "SLEEP";
  case PST_RUNNABLE: 
    return "RUNNABLE";
  case PST_RUNNING:  
    return "RUNNING";
  case PST_ZOMBIE:   
    return "ZOMBIE";
  default:           
    return "UNKNOWN";
  }
}

int
main(void)
{
  struct pstat procinfo;

  if(getpinfo(&procinfo) < 0){
    fprintf(2, "ps: getpinfo failed\n");
    exit(1);
  }

  printf("PID   PPID  PRIO  STATE     SZ      NAME\n");
  printf("=============================================\n");

  for(int i = 0; i < NPROC; i++){
    if(procinfo.inuse[i]){
      printf("%d     %d     %d     %s    %lu    %s\n",
             procinfo.pid[i],
             procinfo.ppid[i],
             procinfo.priority[i],
             state_to_str(procinfo.state[i]),
             procinfo.sz[i],
             procinfo.name[i]);
    }
  }

  exit(0);
}
