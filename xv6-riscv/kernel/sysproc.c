#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

#include "pstat.h" ///////////////added///////////////

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    if(addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


////////// added  //////////

extern struct proc proc[NPROC];        // process table (kernel)
extern struct spinlock wait_lock;      // lock protecting parent/child relations

// Implementation of system call getpinfo(struct pstat*)
// Collects information for all active processes and copies it to user space.

uint64
sys_getpinfo(void)
{
  uint64 uaddr;

  // Get user-space address where the pstat struct will be copied
  argaddr(0, &uaddr);

  struct pstat st;
  memset(&st, 0, sizeof(st));

  // Iterate over the entire process table
  for(int i = 0; i < NPROC; i++){
    struct proc *p = &proc[i];

    // Acquire process lock to safely read process fields
    acquire(&p->lock);

    // Ignore UNUSED entries
    if(p->state != UNUSED){
      st.inuse[i] = 1;                 // mark slot as active
      st.pid[i] = p->pid;              // process ID

      // Convert internal xv6 process state to pstat enum
      int pst = PST_UNUSED;
      if (p->state == UNUSED)
        pst = PST_UNUSED;
      else if (p->state == USED)
        pst = PST_USED;
      else if (p->state == SLEEPING)
        pst = PST_SLEEPING;
      else if (p->state == RUNNABLE)
        pst = PST_RUNNABLE;
      else if (p->state == RUNNING)
        pst = PST_RUNNING;
      else if (p->state == ZOMBIE)
        pst = PST_ZOMBIE;

      st.state[i] = pst;

      // Copy memory size of the process
      st.sz[i] = p->sz;

      // Copy process priority (added for MLFQ scheduling)
      st.priority[i] = p->priority;

      // Copy process name safely
      safestrcpy(st.name[i], p->name, PNAME_LEN);

      // Read parent pid safely.
      // wait_lock must be held when accessing parent relationships.
      int ppid = 0;
      acquire(&wait_lock);
      if(p->parent)
        ppid = p->parent->pid;
      release(&wait_lock);

      st.ppid[i] = ppid;
    }

    // Release process lock
    release(&p->lock);
  }

  // Copy the filled pstat structure to user space
  if(copyout(myproc()->pagetable, uaddr, (char *)&st, sizeof(st)) < 0)
    return -1;

  return 0;
}
