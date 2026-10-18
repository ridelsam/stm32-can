# STM32 CAN

Classic CAN firmware for the NUCLEO-F446RE, using the STM32 bxCAN peripheral
and a command-line build workflow on Linux/WSL. The project covers peripheral
configuration, frame transmission and reception, filtering, and CAN bus behavior.

## Project scope

- CAN frame formats and node architecture.
- Differential signaling, dominant/recessive states, and transceiver operation.
- Peripheral clocks, bit timing calculations, and sample-point configuration.
- bxCAN architecture, transmit/receive paths, and acceptance filters.
- Internal loopback, silent loopback, and peripheral self-tests.
- Normal-mode communication between two boards with external transceivers.

The current application implements CAN1 internal loopback with USART2 reporting.
The remaining scope describes planned development, not completed features.

## Hardware

Primary target: **NUCLEO-F446RE / STM32F446RE**, Cortex-M4 with single-precision
FPU. Internal loopback exercises the controller without a physical CAN network.
Normal-mode bus testing requires compatible transceivers, appropriate wiring
and termination, and a second active CAN node.

## Build environment

Supported build host: Linux x86-64, including WSL 2.

| Tool | Tested version |
|---|---|
| Arm GNU Toolchain | 14.2.Rel1 / GCC 14.2.1 |
| GNU Make | 4.4.1 |
| Bash | Required for the build wrapper |

Use the Linux x86-64 `arm-none-eabi` package for **14.2.Rel1** from
[Arm's GNU toolchain downloads](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads).
The default installation directory is:

```text
~/tools/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/
```

For a different location, export `TOOLCHAIN_ROOT` or copy
`toolchain.local.conf.example` to `toolchain.local.conf` and edit the path.
The exported variable takes precedence; the local file is ignored by Git.
Relative installation paths resolve from the project root.

`toolchain.conf` records the shared toolchain identity. The wrapper checks GCC's
numeric version. The pinned release is the tested baseline; bit-for-bit output
reproducibility has not been established.

## Build commands

Run from the repository root:

```bash
# Verify compiler location and version.
./scripts/build.sh --check

# Build Debug (default).
./scripts/build.sh

# Build Release.
./scripts/build.sh CONFIG=Release

# Clean Debug output only.
./scripts/build.sh clean

# Clean Release output only.
./scripts/build.sh CONFIG=Release clean

# Clean both Debug and Release output.
./scripts/build.sh clean-all
```

`clean` and `clean-all` are Make targets forwarded by the wrapper, not flags.
Cleaning deletes generated output; run the desired build command afterward to
rebuild it.

Debug uses `-Og -g3`; Release uses `-Os -g3`. Both use Cortex-M4/Thumb,
`fpv4-sp-d16`, and the hard-float ABI. Debug information stays in the ELF and
is omitted from the BIN. Release is configured; initial build verification
exercised Debug. Building requires no connected board or IDE.

## Flashing

The onboard probe runs J-Link firmware. Attach it to WSL with `usbipd-win`
before using Linux tools. OpenOCD 0.12.0 is the tested debug server; the shared
`openocd.cfg` selects J-Link, SWD, and STM32F4 at a 1 MHz adapter clock.
Local reset handlers retain that speed and leave the MCU at its default reset
clock during programming, overriding the bundled target script's clock boost.

Build the selected configuration, then flash it:

```bash
# Debug (default).
./scripts/build.sh
./scripts/flash.sh

# Release.
./scripts/build.sh CONFIG=Release
CONFIG=Release ./scripts/flash.sh
```

The flash script programs the existing ELF, verifies Flash contents, resets the
target to run, and exits OpenOCD. It does not rebuild automatically. Note that
the flash script takes `CONFIG` as an environment variable before the command;
the build wrapper forwards `CONFIG=Release` after the command to Make.

## Debugging with OpenOCD and GDB

Run commands from the repository root. The probe must be attached to WSL as
described above. Use the Debug build, and keep the source, ELF, and firmware
programmed on the board in sync. Building alone does not update the board.

### Terminal 1: start OpenOCD

```bash
openocd -f openocd.cfg
```

Leave this terminal running. Wait for:

```text
Listening on port 3333 for gdb connections
```

### Terminal 2: start GDB and connect

```bash
~/tools/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-gdb build/Debug/stm32-can.elf
```

If the toolchain is installed elsewhere, use `arm-none-eabi-gdb` from that
installation's `bin` directory. Opening the ELF gives GDB symbols and source
locations; it does not program Flash.

At the `(gdb)` prompt, enter commands individually:

```gdb
target remote localhost:3333
monitor reset halt
break Error_handler
tbreak main
continue
```

`target remote` connects to OpenOCD. `monitor reset halt` resets and stops the
MCU. `tbreak main` sets a temporary breakpoint that is removed after it is hit.
After stopping at `main`, use `next` to execute initialization calls one at a
time until the main loop is reached. If `Error_handler` is reached, use `bt`
to identify the caller before resetting and losing the failure state.


### Common commands and reconnecting

| GDB command / key | Purpose |
|---|---|
| `Ctrl+C` | Stop the running target and return to the GDB prompt |
| `continue` | Resume execution |
| `next` | Execute the next source line, stepping over function calls |
| `step` | Step into a function call |
| `finish` | Run until the current function returns |
| `bt` | Show the call stack |
| `info breakpoints` | List breakpoint numbers and locations |
| `disable N` | Disable breakpoint number N |
| `delete N` | Remove breakpoint number N |
| `monitor reset halt` | Reset and stop the MCU while keeping GDB connected |

If `monitor` reports that it is unsupported by the target, check that OpenOCD
is still running, then reconnect:

```gdb
target remote localhost:3333
monitor reset halt
```

Do not create another breakpoint at the same callback on each reconnect; check
`info breakpoints` first. A breakpoint on the repeating `while (1)` line can
stop immediately on each `continue`; disable it before waiting for CAN traffic.
Debugger register writes, such as enabling PA11's pull-up, are temporary and
can be cleared by reset or overwritten by firmware initialization.

To rebuild and flash using the scripts, leave GDB with `quit` (interrupt it
first with `Ctrl+C` if running), then stop OpenOCD with `Ctrl+C` in terminal 1.
Run `./scripts/build.sh` and `./scripts/flash.sh`, then restart both terminals'
debug commands above. This avoids opening a second OpenOCD process against the
same probe and reloads symbols from the newly built ELF.

## Repository layout

| Path | Role |
|---|---|
| `Src/`, `Inc/` | Application, clock setup, interrupts, MSP hooks, and HAL configuration |
| `Drivers/` | STM32 HAL and CMSIS support |
| `startup/` | Reset handler and vector table |
| `STM32F446RETx_FLASH.ld` | Memory layout and startup symbols |
| `toolchain.conf` | Shared toolchain identity |
| `toolchain.local.conf.example` | Optional installation-path override template |
| `scripts/build.sh` | Compiler lookup/version check and Make invocation |
| `Makefile` | Compilation, linking, dependency tracking, and output generation |
| `scripts/flash.sh` | Programs/verifies the selected ELF and resets the target |
| `openocd.cfg` | Verified J-Link/SWD probe and STM32F4 target settings |
| `build/Debug/`, `build/Release/` | Generated output; ignored by Git |

Each build produces:

- `stm32-can.elf`: linked program, addresses, symbols, and debugging information.
- `stm32-can.bin`: raw programming data; flashing requires a destination address.
- `stm32-can.map`: linked section and symbol placement.

Objects and header dependency files preserve the source directory layout under
each build directory. C files directly in `Src/` and the HAL source directory
are discovered automatically; additional source directories need a Makefile
update.
