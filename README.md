# xv6-Channel-Synchronization

+This repository extends xv6 with a simple **synchronous channel** mechanism for
+inter-process communication. Channels are single-slot buffers that block
+writers when full and block readers when empty, providing rendezvous-style
+synchronization between processes.
+
+## Features
+
+- **Kernel-managed channel table** with a fixed capacity (`NCHAN`).
+- **Blocking put/take semantics** using sleep/wakeup synchronization.
+- **User-space system call wrappers** for creating, using, and destroying
+  channels.
+- **Example programs** that exercise channels, including a basic producer/
+  consumer test and a multi-process prime pipeline.
+
+## Channel API
+
+The following system calls are available to user programs (see `user/user.h`):
+
+| Call | Description | Returns |
+| --- | --- | --- |
+| `int channel_create(void);` | Allocate a channel slot and return its descriptor. | `>= 0` on success, `-1` on failure. |
+| `int channel_put(int cd, int data);` | Write a value into the channel; blocks while the channel is full. | `0` on success, `-1` on failure. |
+| `int channel_take(int cd, int *data);` | Read a value from the channel; blocks while the channel is empty. | `0` on success, `-1` on failure. |
+| `int channel_destroy(int cd);` | Mark a channel unused and wake any sleepers. | `0` on success, `-1` on failure. |
+
+Channel behavior is implemented in the kernel with a per-channel lock and
+`ready` flag to coordinate producers and consumers. A destroy operation clears
+state and wakes waiters to prevent deadlock if the channel owner exits.
+
+## Building & Running
+
+This project follows standard xv6 build targets.
+
+```sh
+make qemu
+```
+
+To run the sample programs from the xv6 shell:
+
+```sh
+test
+primes
+```
+
+The `test` program exercises basic producer/consumer synchronization. The
+`primes` program creates a generator, a set of checker processes, and a printer
+that coordinate over channels to filter prime numbers.
+
+## Implementation Notes
+
+- Channel metadata lives in `struct channel` and the global `channels` table in
+  the kernel.
+- `channel_put` waits while `ready` is set, and `channel_take` waits while
+  `ready` is clear.
+- `channel_destroy` clears `used` and `ready` and wakes sleepers so blocked
+  calls return with failure.
+
+## Repository Layout
+
+- `kernel/`: Kernel changes, including syscall handlers and channel state.
+- `user/`: User-space wrappers and sample programs (`test.c`, `primes.c`).
+- `Makefile`: xv6 build targets.
+
