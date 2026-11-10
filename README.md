*This project has been created as part of the 42 curriculum by bryfajar.*

# Codexion

## Description

Codexion is a concurrency simulation written in C with POSIX threads.

A group of coders sit around a circular table. Between each pair of neighbours there is one USB dongle, so there are as many dongles as coders. To compile, a coder needs **two dongles at the same time** (left and right). After compiling, a coder debugs, then refactors, then tries to compile again.

If a coder does not **start** a compilation within `time_to_burnout` ms of the start of their previous compilation (or of the simulation), they burn out and the simulation stops. The simulation also stops when every coder has compiled at least `number_of_compiles_required` times.

The goal is to share the dongles fairly and safely: no deadlock, no starvation, no data races, a precise burnout detection and a clean log.

Main features:

- One thread per coder and one separate monitor thread.
- One mutex-protected state per dongle, with a mandatory cooldown after release.
- Fair arbitration with a priority queue (min-heap) per dongle, using `fifo` or `edf`.
- Serialized log: messages never mix.
- No global variables, and all memory and mutexes are released at the end.

## Instructions

### Build

```
cd coders
make
```

Other rules: `make clean`, `make fclean`, `make re`. The code is compiled with `cc -Wall -Wextra -Werror -pthread`.

### Run

```
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument | Meaning |
|---|---|
| `number_of_coders` | Number of coders (and of dongles) |
| `time_to_burnout` | ms allowed between the starts of two compilations |
| `time_to_compile` | ms spent compiling (holding two dongles) |
| `time_to_debug` | ms spent debugging |
| `time_to_refactor` | ms spent refactoring |
| `number_of_compiles_required` | Compilations each coder must reach to end the simulation |
| `dongle_cooldown` | ms a released dongle is unavailable |
| `scheduler` | `fifo` or `edf` (exactly, in lowercase) |

All arguments are mandatory. Only non-negative decimal integers that fit in a `long` are accepted (no signs, no spaces, no decimals), and some values have a minimum (for example, at least one coder). Any invalid input prints an error on `stderr` and returns 1.

### Examples

```
./codexion 4 800 200 200 200 5 50 fifo     # ends when all coders compiled 5 times
./codexion 5 800 200 200 200 5 50 edf
./codexion 4 300 200 200 200 5 50 fifo     # a coder burns out
./codexion 1 800 200 200 200 5 50 fifo     # one dongle only: burns out
```

### Log format

```
timestamp_in_ms X has taken a dongle
timestamp_in_ms X is compiling
timestamp_in_ms X is debugging
timestamp_in_ms X is refactoring
timestamp_in_ms X burned out
```

`timestamp_in_ms` is measured from the start of the simulation, and `X` is the coder number (1 to `number_of_coders`).

### Testing

```
valgrind --leak-check=full ./codexion 4 800 200 200 200 2 50 edf
valgrind --tool=helgrind ./codexion 4 2000 200 200 200 2 50 fifo
norminette
```

Under Valgrind the program is much slower, so use larger time values to avoid false burnouts.

## Blocking cases handled

**Deadlock (Coffman conditions).**
The four conditions are: mutual exclusion, hold and wait, no preemption and circular wait. Dongles are exclusive resources and a coder holds one while waiting for the other, so the first three always apply. The code breaks the **circular wait**: coders with an odd id take their left dongle first and then the right one, and coders with an even id take the right one first and then the left one. Neighbours therefore compete for the same dongle first, and nobody can hold one dongle while the whole circle waits. If the second dongle cannot be acquired (the simulation stopped), the first one is released.

**Startup chains.**
If all coders start at the same time, some of them can take one dongle and wait for the other one, blocking their neighbours for a long time (with many coders and tight times this made some of them burn out). To avoid it, coders with an even id wait `time_to_compile` ms before their first attempt. The odd ones (which do not share dongles with each other) take both dongles without competing, and the even ones arrive just when those dongles are being released.

**Starvation.**
Every dongle has its own min-heap of pending requests. A coder only takes a dongle when it is free, its cooldown is over, **and** their request is at the top of the heap.
- `fifo`: the key is the time of the request, so the oldest request goes first.
- `edf`: the key is `last_compile_start + time_to_burnout`, so the coder closest to burning out goes first.

Ties are broken by the lowest coder id. Each dongle is requested only by its two neighbours, so the heap capacity is 2.

**Dongle cooldown.**
Releasing a dongle stores `free_ms = now + dongle_cooldown`. A dongle can be taken only when it is not taken and the current time is at least `free_ms`.

**Precise burnout detection.**
A separate monitor thread checks every coder against their burnout deadline and the number of compilations done. The burnout message is printed while holding the log mutex, together with raising the stop flag, so it appears within the 10 ms tolerance and nothing is printed after it.

**Log serialization.**
Every message goes through one function protected by the log mutex, so two messages never mix on one line. The function checks the stop flag inside the mutex, so no line is printed after the end of the simulation.

**Clean stop.**
Waiting for a dongle and the debug/compile/refactor sleeps are done in short slices (`usleep`) that check the stop flag. No thread stays blocked, all of them can be joined, and the program ends by itself without leaks.

**Single coder.**
With one coder there is only one dongle, so compiling is impossible. The coder keeps waiting and burns out at `time_to_burnout`, without deadlocking.

**Initialization failures.**
If a `malloc` or a `pthread_mutex_init` fails, everything already created is destroyed and freed before exiting with an error.

## Thread synchronization mechanisms

Only `pthread_mutex_t` is used. Waiting is done by polling with short `usleep` calls, and no condition variable is used.

| Mutex | Protects | Used by |
|---|---|---|
| `dongle->lock` (one per dongle) | `is_taken`, `free_ms` and the request heap of that dongle | the two neighbour coders |
| `coder->data_lock` (one per coder) | `last_compilation` and `number_compilation` | the coder (writes) and the monitor (reads) |
| `shared->log_lock` | standard output | every thread that prints |
| `shared->signal_lock` | the stop flag | coders and monitor |

**Rules that avoid data races**

- A shared value is always read or written with its own mutex locked. A reader copies the value to a local variable and unlocks before using it:

```c
pthread_mutex_lock(&coder->data_lock);
last_comp = coder->last_compilation;
pthread_mutex_unlock(&coder->data_lock);
```

- The coder and the monitor use the **same** mutex for the same data (`coder->data_lock`). Using two different mutexes for one variable does not protect it, and Helgrind reported it until both sides were unified.
- Dongles are never copied. Each coder holds **pointers** to the real dongles in the array, so neighbours see the same state.
- Lock order: `signal_lock` is always the innermost lock (nothing else is locked while holding it), and `log_lock` is never taken while holding a dongle mutex, so no lock cycle exists between them.
- Constants (the configuration) are only read after being written, and `start_time` and `last_compilation` are set **before** the threads are created.

**Thread-safe communication between coders and the monitor**

- Coders → monitor: after starting a compilation, a coder updates `last_compilation` and `number_compilation` under `data_lock`. The monitor reads them under the same mutex.
- Monitor → coders: when a burnout is detected (or everyone finished), the monitor raises the stop flag under `signal_lock`. Coders read it through a function that locks `signal_lock`, and they check it in every loop and sleep.

**Time**

`gettimeofday` is used to get the time in milliseconds. All timestamps are absolute values, and the log prints `now - start_time`.

## Resources

References:

- Linux man pages: `pthread_create(3)`, `pthread_join(3)`, `pthread_mutex_lock(3)`, `pthread_mutex_init(3)`, `gettimeofday(2)`, `usleep(3)`.
- Valgrind manual: Memcheck and Helgrind (data race detector).
- Dining philosophers problem (Dijkstra), and the Coffman conditions for deadlock.
- Earliest Deadline First scheduling.
- Binary heap (priority queue).
- 42 Norm (`norminette`).

### Use of AI

I used Claude (Anthropic) as a tutor and code reviewer. I describe here what it was used for, so that the reader knows which parts were AI-assisted:

- **Explaining concepts:** how threads and mutexes work, data races, deadlock and the Coffman conditions, the dining philosophers problem, `enum`, pointers to shared structures, `gettimeofday`, and how to read Valgrind/Helgrind output.
- **Reviewing my code:** it pointed out compile errors, Norm problems (line limits, naming, splitting functions), and bugs such as a mutex locked twice, repeated log messages, and two different mutexes protecting the same data.
- **Code provided or rewritten with AI help:** the binary min-heap functions (`heap_push`, `heap_peek`, `heap_pop`), the dongle acquisition and release code (`lock_dongle` with the request queue, the odd/even ordering, `release_dongle`), the burnout printing function, and the first draft of this README.
- **Debugging:** interpreting the Helgrind and Valgrind reports and fixing the uninitialized fields they found.

I wrote and understood the argument parsing, the structures, the initialization and cleanup, the thread creation and the coder routine, and I read, tested and adapted every part generated with AI. I checked the results with Valgrind (Memcheck and Helgrind) and by running the scenarios above, and I can explain how each part works.
