# xv6 Multilevel Feedback Queue (MLFQ) Scheduler

An Operating Systems coursework project extending [MIT's xv6-riscv](https://github.com/mit-pdos/xv6-riscv) teaching operating system with a **Multilevel Feedback Queue (MLFQ) CPU scheduler**, a process-information system call, and a user-space `ps` command.

This repository presents **my university submission**, without incorporating later proposed code fixes.

## Implemented functionality

### MLFQ scheduling

The original xv6 round-robin scheduler was modified to select runnable processes using four priority levels, numbered **0 (highest)** through **3 (lowest)**.

| Priority | CPU time slice |
| --- | --- |
| 0 | 4 timer ticks |
| 1 | 8 timer ticks |
| 2 | 16 timer ticks |
| 3 | 32 timer ticks |

The implementation includes:

- **Priority-based selection:** searches for runnable processes from the highest to the lowest priority level.
- **Round-robin within a level:** maintains a per-priority starting index when scanning runnable processes.
- **New-process initialization:** assigns new processes priority 0 and initializes scheduling counters.
- **Time-slice accounting and demotion:** tracks consumed timer ticks and lowers priority after a process uses its quantum, except at the lowest level.
- **Higher-priority preemption checks:** adds timer-interrupt logic to consider runnable processes at higher priority levels.
- **Priority aging:** tracks waiting time for runnable processes and promotes processes after a threshold of ten times their level's time slice, to reduce starvation.
- **Remaining-quantum tracking:** maintains the ticks already consumed at the current priority level.

These describe mechanisms present in the submitted code, not a claim that every scheduling edge case has been formally verified.

### Process information and `ps`

A new **`getpinfo(struct pstat *)`** system call collects process information and copies it into a user-provided structure. The structure includes process IDs, parent IDs, names, states, memory sizes, and MLFQ priorities.

A user-space **`ps`** command calls `getpinfo()` and prints a process table with the columns:

```text
PID   PPID  PRIO  STATE     SZ      NAME
```

## Main modified and added files

| File | Role |
| --- | --- |
| `kernel/proc.c`, `kernel/proc.h` | MLFQ scheduler, process priority, tick counters, round-robin selection, and aging |
| `kernel/trap.c` | Timer-interrupt scheduling and accounting logic |
| `kernel/sysproc.c` | `getpinfo()` system call implementation |
| `kernel/pstat.h` | Process-information structure and state constants |
| `kernel/syscall.c`, `kernel/syscall.h` | System call dispatch and number |
| `kernel/defs.h` | Declarations for scheduling helpers |
| `user/user.h`, `user/usys.pl` | User-facing system call declaration and stub |
| `user/ps.c` | Process listing utility |
| `Makefile` | Includes `ps` in the user programs |

The remainder of the xv6 source tree is based on the MIT teaching operating system.

## Build and run

This project requires a compatible **RISC-V cross-compilation toolchain** and **QEMU RISC-V system emulator**. From the xv6 project root, run:

```bash
CPUS=1 make qemu
```

The assignment recommends starting with a single CPU when evaluating scheduling behavior.

Once xv6 starts, use its shell:

```text
ps
usertests
```

The `ps` command displays process information. `usertests` runs xv6's existing user-level test suite.


## Project background and attribution

Developed as an individual assignment for the **Operating Systems** course at the **Department of Informatics and Telecommunications, National and Kapodistrian University of Athens** (academic year 2025–2026).

The project is built on [MIT PDOS's xv6-riscv](https://github.com/mit-pdos/xv6-riscv). **MIT and its contributors created the underlying operating system**; my coursework contributions are the scheduler changes, process-information interface, and `ps` utility described above. The original xv6 license is retained in `LICENSE`.
