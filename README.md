*This project has been created as part of the 42 curriculum by jpaim-so*

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

AI was used as a support tool during development for:

	- Reviewing project structure and identifying possible edge cases;
	- Improving README documentation;
	- Helping create a .sh script to run some tests and confirm project follows
      the pdf specs;
	- Helping prepare explanations for concurrency and synchronization concepts;

## Additional Sections:

### Blocking cases handled

The project handles several concurrency challenges:

- Deadlock prevention: dongle access is controlled through a scheduler to
  prevent circular waiting;
- Starvation prevention: FIFO and EDF scheduling ensure fair access to
  resources;
- Dongle cooldown handling: released dongles remain unavailable until the
  cooldown period has passed;
- Burnout detection: a dedicated monitor thread checks coder deadlines and
  stops the simulation when necessary;
- Log synchronization: output is protected to prevent mixed or corrupted
  messages;
- Thread synchronization mechanisms.

### Thread synchronization mechanisms

The project uses POSIX threads and mutexes to safely coordinate concurrent
execution.

- pthread_mutex_t is used to protect shared simulation state, dongle status,
  and output logging;
- a custom heap-based queue manages dongle requests according to FIFO or EDF
  scheduling;
- a monitor thread safely communicates simulation state changes through
  protected shared variables.

Mutex protection prevents race conditions when multiple coder threads access
shared resources at the same time.
