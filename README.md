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
