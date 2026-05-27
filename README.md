*This project has been created as part of the 42 curriculum by jpaim-so*

# Codexion

## Description

MAMBO JAMBO

## Instructions

`./codexion number_of_coders time_to_burnout time_to_compile time_to_debug`
`time_to_refactor number_of_compiles_required dongle_cooldown scheduler`

- number_of_coders — number of coders (and dongles) in the simulation
- time_to_burnout — ms a coder can go since its last compile (or sim start)
  before burning out
- time_to_compile — ms a coder spends compiling (holding two dongles)
- time_to_debug — ms a coder spends debugging
- time_to_refactor — ms a coder spends refactoring
- number_of_compiles_required — if every coder has compiled at least this many
  times, the simulation stops. Otherwise, the simulation runs until a coder
  burns out
- dongle_cooldown — ms a dongle stays unavailable after being released
- scheduler — dongle arbitration policy: fifo or edf

All arguments are mandatory; times are in milliseconds.

Example:

`./codexion 5 800 200 100 100 5 50 fifo`

## Resources

### References


## Additional Sections:

### Blocking cases handled

### Thread synchronization mechanisms

#

# Readme Requirements

A README.md file must be provided at the root of your Git repository. Its purpose is
to allow anyone unfamiliar with the project (peers, staff, recruiters, etc.) to quickly
understand what the project is about, how to run it, and where to find more information
on the topic.

The README.md must include at least:
• The very first line must be italicized and read: This project has been created as part
of the 42 curriculum by <login1>[, <login2>[, <login3>[...]]].
• A “Description” section that clearly presents the project, including its goal and a
brief overview.
• An “Instructions” section containing any relevant information about compilation,
installation, and/or execution.
• A “Resources” section listing classic references related to the topic (documentation, articles, tutorials, etc.), as well as a description of how AI was used —
specifying for which tasks and which parts of the project.

➠ Additional sections may be required depending on the project (e.g., usage
examples, feature list, technical choices, etc.).
Any required additions will be explicitly listed below.

For this project, the README.md must also include:

• A “Blocking cases handled” section describing all the concurrency issues addressed in your solution (e.g., deadlock prevention and Coffman’s conditions, starvation prevention, cooldown handling, precise burnout detection, and log serialization).

• A “Thread synchronization mechanisms” section explaining the specific threading primitives used in your implementation (pthread_mutex_t, pthread_cond_t,
custom event implementation) and how they coordinate access to shared resources
(dongles, logging, monitor state). Include examples of how race conditions are
prevented and how thread-safe communication is achieved between coders and the
monitor.
