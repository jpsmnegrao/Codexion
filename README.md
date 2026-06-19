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

./codexion 5 800 200 200 200 10 50 edf

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

make lint

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

- **Dongle cooldown handling:** after release, a dongle's `release_time` is
  recorded. Availability checks require `now >= release_time + cooldown`
  before a dongle can be re-acquired. Coders poll periodically (instead of
  blocking on a condition variable) because cooldown expiry is time-based,
  not event-based — no thread would otherwise wake a sleeping coder when the
  cooldown ends.

- **Precise burnout detection:** a dedicated monitor thread polls every 500
  microseconds, checking `now - last_compile_start > time_to_burnout` for
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

- **`state_lock` (pthread_mutex_t):** protects all shared simulation state —
  the stop flag, each coder's `times_compiled` and `last_compile_start`,
  each dongle's `status` and `release_time`, and the priority heap itself.
  Every read or write to this state happens while holding `state_lock`,
  preventing race conditions between coder threads and the monitor thread.

- **`output_lock` (pthread_mutex_t):** protects all calls to `printf` for
  state-change logging, guaranteeing serialized, non-interleaved output.

- **Heap-based priority queue:** dongle requests are not served on a
  first-checked basis but through an explicit min-heap keyed by priority
  (`request_time` for FIFO, `last_compile_start + time_to_burnout` for EDF).
  A coder only proceeds when it is the root of the heap AND its dongles are
  physically free — this combination prevents both starvation and dongle
  duplication.

- **Coder/monitor communication:** the monitor thread and coder threads never
  communicate directly; they only observe and modify shared state under
  `state_lock`. For example, when the monitor detects burnout, it sets
  `sim->stop = 1` under the lock. Coder threads check this flag (also under
  the lock) between every action (compile/debug/refactor) and inside the
  dongle-acquisition wait loop, ensuring they stop promptly without a race
  on the flag itself.

- **Why polling instead of `pthread_cond_wait`:** dongle availability depends
  on cooldown expiry, a purely time-based condition with no corresponding
  event to broadcast on. Using `pthread_cond_wait` here would risk a
  permanent sleep if every coder is blocked only by cooldown, since nothing
  would ever signal the condition variable. Coders instead release the lock,
  sleep briefly (500 microseconds), and re-acquire the lock to re-check —
  avoiding both busy-waiting and the cooldown-deadlock risk.
