# KickEmulator (community) — what it is, and whether we can run it on Windows

_Dated 2026-10-29. Local analysis only: nothing here is committed or pushed. This file is
untracked (`docs/EMULATOR.md`); the emulator checkout it describes lives in
`.3rdPartySourceCache/KickEmulator`, which is gitignored._

This is a working note for our project (an unofficial KICK build, see
[`../BUILDING_ON_WINDOWS.md`](../BUILDING_ON_WINDOWS.md)). It assesses the community
[KickEmulator](https://github.com/KickEm-Tools/KickEmulator) and how (or whether) it fits a
Windows host.

> **Status (2026-10-30):** native Windows fast mode is not possible (the launcher and the panel
> transport are POSIX). The full **fast-mode stack is verified working under WSL2 (Ubuntu 24.04)** on
> this machine, including audio, and the official `checks/smoke` passes for the fork and for our own
> tree — see [Windows (via WSL2)](#windows-via-wsl2--verified-working).

## What it is

KickEmulator is a desktop **front-panel emulator** for the Behringer Kick firmware (`0700-AAM`) and
any fork of it. You give it a firmware repository URL and a branch; it fetches that ref, builds the
fork's desktop `Kick` app and its own panel program, and runs them connected.

The firmware needs **no emulator code**. The only coupling is the control-surface serial protocol
(`ComProtocol/PktMgr`): the panel program plays the microcontroller's side of that protocol over a
transport (a pseudo-terminal, a TCP socket, or a real serial port). The protocol headers and control
ids (`ComProtocol/PktMgr`, `Kick/modules/contracts/dcl`) are read from the *fetched firmware ref*,
not copied, so a fork that extends the protocol still works. Licence: AGPL-3.0-or-later, the same as
the firmware.

The panel draws the KICK's front panel (pads, keys, LEDs, encoders, slider) and sends/receives
`CPktMgr` frames exactly as a unit's microcontroller would; the KICK's own screen is the app's own
window in a separate process.

## Modes

Both modes use the same panel and checks.

| | Fast mode (`--mode fast`, default) | Faithful mode (`--mode vm`) |
|---|---|---|
| Runs | The fork's `Kick` app built for the desktop (macOS/Linux), connected to the panel over a pty | The fork's Linux arm64 `KT_Kick` build inside a QEMU arm64 VM, panel on the VM's serial port |
| For | Quick edits/rebuilds, a debugger, scripted checks | The device's Linux code paths and services; firmware CI |
| Needs in the firmware | It builds on the desktop, has a working serial port, and a few opt-in desktop seams (`KICK_SERIAL_DEVICE`, `KICK_TEST_CONTROL`, `KICK_AUDIO_PROBE`, ...) | A working serial port |
| Status | **fast mode works on macOS (Apple Silicon) — and, verified here, on Windows via WSL2 (Ubuntu 24.04); upstream still says Linux desktop "not tried"** | **phase 6, not started** |

Upstream status line (README, 2026-10-28): fast mode runs on macOS and the smoke test passes over
the wire; everything else is planned. Faithful mode is not implemented. We additionally confirmed
fast mode builds and reaches `SimSerial: LIVE` under WSL2 on Windows (details below).

## Transports

`kickem-panel --transport <spec>`; the launcher passes `KICK_SERIAL_DEVICE=<the peer>` to the app.

| Spec | Used by | Mechanism |
|---|---|---|
| `pty[:<symlink>]` | fast mode (macOS/Linux) | Panel opens a pseudo-terminal (`posix_openpt`), holds the master and slave, logs `SimSerial: listening on <slave path>` |
| `tcp:<host>:<port>` | faithful mode; the option named for Windows | Panel connects as a client to QEMU's `-serial tcp:...,server` |
| `serial:<dev>[@<baud>]` | a real USB-serial adapter | 8N1, raw, no flow control; default 230400 |

`--throttle` paces whole frames at the unit's 230400-baud UART rate (whole frames, because the app's
reader drops a frame split across two reads).

**All three transports are POSIX.** `transport/Transport.h` says so in its first line: _"POSIX only
(macOS, Linux)."_ `Transport.cpp` uses `posix_openpt`/`grantpt`/`unlockpt`/`ptsname`, `termios`,
`select`, `sys/socket.h`, `netinet/*`, `unistd.h`, `fcntl.h`. There is **no Win32 backend**, not even
for the TCP transport (it still uses POSIX sockets, `fcntl` and `select`).

## Requirements (upstream)

- CMake 3.22+, git, Python 3.8+ (standard library only)
- A C++17 toolchain; on macOS Xcode command-line tools with `MACOSX_DEPLOYMENT_TARGET=12.0`
- The first run downloads JUCE (~170 MB) into `~/.cache/kickem/cpm` unless `CPM_SOURCE_CACHE` points
  at a cache that already has it (our `.3rdPartySourceCache` has JUCE 7.0.12)

## The launcher is POSIX-only

`./kickem` is a Python 3 script. On **native Windows** it does not start:

```
$ python kickem --help
  File "...\KickEmulator\kickem", line 41, in <module>
    import fcntl
ModuleNotFoundError: No module named 'fcntl'
```

_(verified 2026-10-29 on the Windows host below.)_ Beyond `import fcntl` it uses `fcntl.flock`, the
`pgrep` binary, `os.waitpid`, `os.kill`, `start_new_session`, and the lock `/tmp/kt_kickem.lock`.
None of those exist natively on Windows. The launcher would need a port, or a POSIX environment.

## Exact commands

The default firmware is `KickEm-Tools/0700-AAM`; the ref that fast mode needs today is
`integration/standalone`. CPM downloads (JUCE) go to `$CPM_SOURCE_CACHE`, else `~/.cache/kickem/cpm`.

### macOS (the proven path)

```sh
git clone https://github.com/KickEm-Tools/KickEmulator.git && cd KickEmulator
./kickem run --ref integration/standalone        # fetch, build, start panel + Kick, connect them
```

Needs Xcode command-line tools. Two windows open; on the Projects screen press **PUSH** then
**PLAY**. Ctrl-C stops both. (Reuse an existing JUCE cache with
`CPM_SOURCE_CACHE=<dir-with-JUCE> ./kickem run ...`.)

### Linux desktop

Same commands as macOS, without `MACOSX_DEPLOYMENT_TARGET`. Upstream has **not tried** this. Extra
caveat from `modes/fast/README.md`: on Linux the app resolves its data from absolute paths
(`/app/assets`, `/data/sounds`, ...), so fast mode also needs those paths to point at the build; this
is not automated.

### Windows (via WSL2) — verified working

There is no native Windows path for the emulator (see the assessment below), so the stack runs under
WSL2 Linux, where it is POSIX like macOS/Linux. **This is the setup used and confirmed on this
machine on 2026-10-29:**

- Windows 11 (build 26200) with WSL **3.0.3.0**, kernel `6.18.54.1-microsoft-standard-WSL2`,
  **WSLg 1.0.79** (X/Wayland for the windows), **WSL 2**.
- Distro **Ubuntu 24.04.5 LTS**; gcc/g++ 13.3, CMake 3.28.3, git 2.43.0, Python 3.12.3,
  pkg-config 1.8.1.
- Emulator cloned to `~/KickEmulator` (`KickEm-Tools/KickEmulator` at commit `dc0d38ca`).
- JUCE reused from the Windows-side cache, so no ~170 MB download:
  `CPM_SOURCE_CACHE=/mnt/d/Coding  Projects/Behringer Kick/.3rdPartySourceCache` (it holds
  JUCE 7.0.12).

Exact commands that were run:

```sh
# in WSL2 Ubuntu
sudo apt update && sudo apt install -y cmake git python3 build-essential \
  libasound2-dev libjack-jackd2-dev libcurl4-openssl-dev libfreetype6-dev \
  libx11-dev libxcomposite-dev libxcursor-dev libxext-dev libxinerama-dev \
  libxrandr-dev libxrender-dev libwebkit2gtk-4.1-dev libglu1-mesa-dev mesa-common-dev \
  libx11-xcb-dev libxcb1-dev libxkbcommon-dev libxkbcommon-x11-dev libgtk-3-dev \
  libasound2-plugins alsa-utils pulseaudio-utils python3-numpy tmux
git clone https://github.com/KickEm-Tools/KickEmulator.git ~/KickEmulator
cd ~/KickEmulator
CPM_SOURCE_CACHE='/mnt/d/Coding  Projects/Behringer Kick/.3rdPartySourceCache' \
  ./kickem run --ref integration/standalone
```

Result (fast mode): the firmware was fetched (`0700-AAM` `integration/standalone`), the desktop
`Kick` app and `kickem-panel` were built, the panel opened a pty (device `/dev/pts/N`), and the
handshake reached **`SimSerial: LIVE`**. Driving it with `checks/lib/kickem_ctl` worked over the
wire: control frames were delivered and acked (`KNOB_ENTER`, then `PLAY` → `PanelControl: ok`).

Audio (verified 2026-10-30): after installing `libasound2-plugins alsa-utils pulseaudio-utils`,
`pactl info` shows WSLg's `RDPSink`, and the app opens **`PulseAudio Sound Server`** at 48 kHz.
`--probe` produced a growing WAV with peaks around **-3 dBFS** during playback.

Official smoke test (verified 2026-10-30 on `integration/standalone` @ `2d2c9462` after a clean
rebuild):

```sh
cd ~/KickEmulator
checks/smoke --ref integration/standalone --no-build
```

`SMOKE OK` — handshake, PUSH loaded `/app/demos/Starter Kit`, 10 PAD colour packets, silent before
PLAY (peak -120 dBFS), PLAY/STOP LED packets, 7/7 seconds of playback above -40 dBFS, max peak
-3.3 dBFS (no clipping), and silent after STOP. This is the acceptance test to run after any change
to the app, engine or protocol.

### Running our own tree (verified 2026-10-30)

Our tree now has the emulator prerequisites (in-tree `AudIO`/`ComProtocol`, relative sample paths,
`ui::overlayRoot`, FeatureUnavailable screens) and the Windows desktop seams. The same fast-mode
stack builds it on Linux and passes the official smoke test:

```sh
# in WSL2, once: a bare mirror avoids the spaces in the Windows path
git clone --bare '/mnt/d/Coding  Projects/Behringer Kick' /home/matej/kick-ours.git
cd ~/KickEmulator
./kickem build --firmware /home/matej/kick-ours.git --ref main
checks/smoke --firmware /home/matej/kick-ours.git --ref main --no-build
```

Result: `SMOKE OK` 10/10 (handshake, PUSH loads the Starter Kit, 10 PAD colour packets, silent
before PLAY, PLAY/STOP LEDs, 7/7 s above -40 dBFS, max peak -5.2 dBFS, silent after STOP).
Refresh the mirror with `git --git-dir=/home/matej/kick-ours.git fetch origin main` after a push.

The Linux app resolves absolute paths (`/app/assets`, `/app/demos`, `/app/templates`,
`/data/sounds`, `/home/Kick/{projects,logs,prefs}`). After a build, point them at that build; the
emulator does not create them:

```sh
sudo ln -sfn ~/.cache/kickem/build/<...>/kick /opt/kick-build
sudo mkdir -p /app /data /home/Kick/projects /home/Kick/logs /home/Kick/prefs
sudo ln -sfn /opt/kick-build/assets /app/assets
sudo ln -sfn /opt/kick-build/demos /app/demos
sudo ln -sfn /opt/kick-build/templates /app/templates
sudo ln -sfn /opt/kick-build/data/samplesKick /data/sounds
sudo chown -R matej:matej /home/Kick
```

Known rough edges:

- An earlier manual run logged **`free(): invalid pointer`** on teardown. The official `checks/smoke`
  run did not reproduce it; it may be triggered by a manual `kickem_ctl status` or a forced stop.
  Keep an eye on it when scripting shutdown.
- Linux-only screens log `FeatureUnavailable: resources shown/hidden` (expected; they have no device
  to talk to).

Two newer launcher features are convenient here: `kickem run --no-fetch` reuses the cached firmware
checkout **as-is, local edits included** (so a branch of ours can be tried without pushing), and
`kickem_ctl touch X Y` is a real JUCE touch through the window (vs `tap`, which is injected).

### Windows (native MSVC) — partially works, not enough for fast mode

Verified on the host below with MSVC 19.44 / CMake 4.2 / "Visual Studio 17 2022" x64:

- **What does build natively:** the emulator's protocol/CLI-independent part. Configure with
  `-DKICKEM_FIRMWARE_DIR=<our checkout>` and build the `protocol_roundtrip` target — it compiles the
  firmware's own `ComProtocol/PktMgr/PktMgr.cpp` against the JUCE "shim" (no JUCE download) and the
  resulting `.exe` runs and prints round-trip frames. So the framing/CRC layer is fine on Windows.
- **What does not:** `kickem-panel` (its `kickem_transport` is POSIX) and the `kickem` launcher
  (`fcntl`). Fast mode therefore cannot run natively, regardless of the firmware.
- Gotcha: building under a very deep path fails with MSBuild `FileTracker : error FTK1011` (MAX_PATH).
  Use a short build directory such as `C:\kb`.

## Can our KICK app be driven by the panel on Windows?

Our own tree already builds `Kick.exe` on Windows with MSVC, including a **Win32 `juce_serialport`**
(`Kick/modules/3rdParty/juce_serialport`, see `../BUILDING_ON_WINDOWS.md`). That is the firmware half
of fast mode. But two things are missing on our side and on the emulator side:

1. **Our tree lacks the desktop seams.** `Kick/apps/kick/Main.cpp` in our tree only has
   `KICK_SNAPSHOT`; it has no `KICK_SERIAL_DEVICE`, `KICK_TEST_CONTROL`, `KICK_AUDIO_PROBE` or
   `KICK_HARDWARE`. Fast mode's preflight **requires** `KICK_SERIAL_DEVICE` (it stops and names the
   branch) and warns without the others. Those seams are the KickEm-Tools PRs
   [#15 `feature/serialport`](https://github.com/KickEm-Tools/0700-AAM/pull/15) and
   [#2 `feature/desktop-seams`](https://github.com/KickEm-Tools/0700-AAM/pull/2).
2. **The panel cannot run natively on Windows** (POSIX transport + POSIX launcher), so even with the
   seams there is nothing to connect to unless the panel/transport are ported or run under
   WSL/MSYS2.

So: our `Kick.exe` is buildable and runnable on Windows, but the emulator's fast mode is a POSIX
stack. Validating our Windows build through the emulator means running the panel under Linux (WSL)
and giving the app a device it can open. A cross-boundary hybrid (panel in WSL ↔ `Kick.exe` on
Windows) would need a transport that spans the two — the current transports don't; that is unproven
work, not a supported path.

## Using the right firmware base

The fork's `main` has moved past upstream `27194180`: at the time of writing it is `4f8853e9`,
**78 commits ahead**, and already contains the desktop seams. The
[`integration/standalone`](https://github.com/KickEm-Tools/0700-AAM/tree/integration/standalone)
branch merges every open pull request and is the branch used for the verification above:

- `integration/standalone` head `2d2c9462`, **71 commits ahead of upstream `main` (`27194180`), 0
  behind**.
- **1 open pull request** at the time of writing (#21, CI for the emulator).
- The prerequisite PRs fast mode needed: `build/release-builds` (#1, reconstructed `AudIO_juce` +
  build fixes — required), `feature/serialport` (#15, real `juce_serialport` + `KICK_SERIAL_DEVICE`
  — required), `feature/desktop-seams` (#2, `KICK_TEST_CONTROL` / `KICK_AUDIO_PROBE` /
  `KICK_HARDWARE` — checks that need them fail without it), `fix/relative-sample-paths` (#7, the
  Starter Kit's samples load), `fix/shift-state` (#16, SHIFT functions from the surface). These are
  merged into the fork's `main`/`integration/standalone` now; the hashes and counts are a moving
  target.

`integration/standalone` is a scratch branch (it force-merges open PRs). It is the right base for
**trying** the emulator; it is not a base for our own work.

## What it does and does not validate

- **Fast mode runs the desktop build, not the device's Linux build.** It says nothing about
  systemd units, scripts, device nodes, or the Wi-Fi / Ethernet / CV / firmware-update screens (shown
  as "not available on this system"). Timing, latency and CPU load are your computer's, not the
  unit's A53 cores. The panel emulates the microcontroller's **protocol**, not its firmware.
- **Faithful mode** would cover the Linux code paths but cannot tell latency, CPU headroom on the
  A53 cores, the codec, the GPU driver, or the microcontroller's own behaviour — and it is not built.
- It **can** validate: the app's control-surface protocol end to end (handshake, key/pad/knob/slider
  packets, LED and pad-colour feedback), project load and playback with the Starter Kit, scripted UI
  interaction (`checks/smoke`, `checks/lib/kickem_ctl`), audio output level (`--probe OUT.wav`), and
  fork-specific protocol changes.

## Recommended workflow for us

1. **Do not attempt native Windows fast mode.** It is a POSIX stack (launcher + panel transport).
2. **Use WSL2 (Ubuntu) for the emulator** — it is installed (Ubuntu 24.04.5) and fast mode is
   confirmed working; keep `CPM_SOURCE_CACHE` pointed at our `.3rdPartySourceCache` to reuse JUCE
   7.0.12 and skip the download. See [Windows (via WSL2)](#windows-via-wsl2--verified-working).
3. **Our tree is now confirmed** to build and pass `checks/smoke` under fast mode; see
   [Running our own tree](#running-our-own-tree-verified-2026-10-30). Use the bare mirror because the
   Windows path has spaces, and keep the emulator clone separate.
4. Keep our Windows MSVC `Kick.exe` build (proven) separate from the emulator workflow. Use the
   emulator to exercise **app/engine behaviour over the protocol**, not to validate our Windows
   serial port (no unit is attached; the Win32 `juce_serialport` path is untested against hard­ware).
5. If native Windows fast mode is ever required, the work is: a Win32 backend in
   `transport/Transport.cpp` (COM port + overlapped I/O or `select`), a Windows port of the `kickem`
   launcher (`msvcrt.locking`/`CreateFile` lock, process enumeration instead of `pgrep`), and a
   `juce_serialport` on the app side that accepts a `COMx`/`\\.\pipe\...` peer.

## Environment notes (this machine, 2026-10-29)

- WSL2: **installed** — Ubuntu 24.04.5 LTS, WSL 3.0.3.0, kernel 6.18.54.1, WSLg 1.0.79; fast mode
  verified working there (see the WSL2 section above).
- Visual Studio Build Tools 2022 (MSVC 19.44) present; `cl.exe` is not on `PATH` (use the VS
  generator or a developer prompt). No `gcc`/`clang`/`ninja`/`make` on the Windows `PATH`; Git Bash
  is present.
- Windows: Python 3.14.3, CMake 4.2.3, git 2.55. WSL2: gcc/g++ 13.3, CMake 3.28.3, git 2.43.0,
  Python 3.12.3.
- Emulator clones: the Windows-side cache `.3rdPartySourceCache/KickEmulator` is depth-1 at
  `c2077d34` (2026-10-28); the WSL clone `~/KickEmulator` is at `dc0d38ca` (2026-10-29, adds
  `kickem --no-fetch` and `kickem_ctl touch`).

## Links

- KickEmulator — https://github.com/KickEm-Tools/KickEmulator
- KickEm-Tools integration branch (`integration/standalone`) —
  https://github.com/KickEm-Tools/0700-AAM/tree/integration/standalone
- Eight4aWish macOS port (`macos-build`, used as reference for our reconstruction) —
  https://github.com/Eight4aWish/0700-AAM/tree/macos-build
- Upstream release-gap issue —
  https://github.com/MusicTribeOpenSource/0700-AAM/issues/9
- Our Windows build notes — [`../BUILDING_ON_WINDOWS.md`](../BUILDING_ON_WINDOWS.md)
- Emulator PRs fast mode needs — #15 `feature/serialport`, #2 `feature/desktop-seams`,
  #1 `build/release-builds`, #7 `fix/relative-sample-paths`, #16 `fix/shift-state`
  (https://github.com/KickEm-Tools/0700-AAM/pulls)
