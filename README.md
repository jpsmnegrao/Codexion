*This project has been created as part of the 42 curriculum by jpaim-so.*

# Codexion

## Description

Codexion is a C concurrency project that simulates multiple coders competing
for shared USB dongles using POSIX threads and mutexes.

Each coder runs in its own thread and must compile, debug, and refactor while
safely managing shared resources. The project implements synchronization,
deadlock prevention, resource cooldowns, and FIFO/EDF scheduling to ensure
fair access and avoid burnout.

## Instructions

### Compilation

To compile the project and create the codexion executable run:

`make`

### Usage

`./codexion number_of_coders time_to_burnout time_to_compile time_to_debug`
`time_to_refactor number_of_compiles_required dongle_cooldown scheduler`

- number_of_coders — number of coders (and dongles) in the simulation;
- time_to_burnout — ms a coder can go since its last compile (or sim start)
  before burning out;
- time_to_compile — ms a coder spends compiling (holding two dongles);
- time_to_debug — ms a coder spends debugging;
- time_to_refactor — ms a coder spends refactoring;
- number_of_compiles_required — number of compiles each coder must reach to
  stop the simulation. Otherwise, the simulation runs until a coder burns out;
- dongle_cooldown — ms a dongle stays unavailable after being released;
- scheduler — dongle arbitration policy: fifo or edf:
  - fifo — First In, First Out scheduling;
  - edf — Earliest Deadline First scheduling.

All arguments are mandatory; times are in milliseconds.

### Example:

`./codexion 5 800 200 200 200 10 50 edf`

### Cleanup

Remove object files:

`make clean`

Remove executable and objects:

`make fclean`

Recompile:

`make re`

### Testing / Debugging

The Makefile includes a lint rule that runs:

- Norminette check;
- Valgrind memory leak checks;
- Helgrind thread-safety checks.

Run:

`make lint`

This can be used to verify code style, memory management, and possible threading issues.

## Resources

### References

- POSIX Threads documentation (pthread_create, pthread_mutex, synchronization
  primitives);
- Man pages for pthread functions and gettimeofday;
- Research on the following concepts: threads, race conditions, deadlocks, and
  synchronization - this YouTube channel was quite useful -
  https://www.youtube.com/playlist?list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2;
- Classic Dining Philosophers problem and resource allocation strategies.

### AI Usage

AI was used as a support tool during development, specifically for:

- Designing the heap-based priority queue (min-heap with sift-up/sift-down)
  used for FIFO/EDF dongle arbitration;
- Reviewing locking discipline (lock ordering between `state_lock` and
  `output_lock`) to avoid deadlocks and data races;
- Reviewing Makefile structure (PHONY targets, lint rule combining
  norminette, valgrind, and helgrind);
- Helping draft and refine this README's required sections.

All AI-suggested code and explanations were reviewed, tested, and modified
before integration; nothing was used without understanding its behavior.

## Additional Sections:

### Blocking cases handled

- **Deadlock prevention (Coffman's conditions):** a coder never holds one
  dongle while waiting for the other. Both dongles are acquired atomically:
  the coder is inserted into a priority heap and only takes its dongles once
  it is at the top of the heap AND both dongles are free. This breaks the
  hold-and-wait condition, so no circular wait can form.

- **Starvation prevention:** the heap-based scheduler always grants dongles
  to the coder with the highest priority (earliest request for FIFO, earliest
  deadline for EDF). No coder can be skipped indefinitely while others are
  served repeatedly.

- **Dongle cooldown handling:** the implementation does not use condition
  variables for dongle acquisition. Waiting coders poll under state_lock, sleep
  briefly with usleep(500), and check again. This matches the cooldown model
  and keeps the acquisition logic simple and deterministic with the heap-based
  scheduler.

- **Precise burnout detection:** a dedicated monitor thread polls every 500
  microseconds, checking `now - last_compile_start >= time_to_burnout` for
  every coder. This keeps detection well within the 10ms tolerance required
  by the subject.

- **Log serialization:** every state-change log is wrapped in `output_lock`,
  ensuring two threads can never interleave characters within or across log
  lines.

- **Single-coder edge case:** when only one coder exists, its left and right
  dongle point to the same physical dongle. The code explicitly detects this
  (`left == right`) and never allows a coder to "duplicate" a dongle by
  acquiring the same one twice — it always burns out instead, matching the
  subject's two-dongles-required-to-compile rule.

### Thread synchronization mechanisms

- **state_lock (pthread_mutex_t):** protects shared simulation state, including
  sim->stop, each coder’s times_compiled, last_compile_start, and request_time,
  each dongle’s status and release_time, and the wait queue heap. Any thread
  that reads or updates this state does so while holding the mutex.

- **output_lock (pthread_mutex_t):** protects printf so log lines are never
  interleaved.

- **Heap-based priority queue:** the wait queue is a custom min-heap used to
  decide which coder may proceed. FIFO uses request_time as the priority; EDF
  uses last_compile_start + time_to_burnout as the primary key, with the coder
  ID acting as a tie-breaker to guarantee deterministic behavior in edge cases.
  A coder only takes dongles when it is at the top of the heap and both dongles
  are available.

- **Monitor and coder coordination:** coder threads and the monitor thread do not
  communicate directly. They only coordinate through shared state under
  state_lock. The monitor sets sim->stop = 1 when a coder burns out or when
  all coders complete the required number of compiles, and coders check that
  flag between actions and while waiting for dongles.

- **Polling instead of condition variables:** the project does not use
  pthread_cond_wait for dongle acquisition. Coders periodically release
  state_lock, sleep briefly, and re-check the shared state. This avoids
  blocking forever on a purely time-based cooldown condition.
