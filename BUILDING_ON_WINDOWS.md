# Building the KICK app on Windows

This tree is [MusicTribeOpenSource/0700-AAM](https://github.com/MusicTribeOpenSource/0700-AAM)
(`main`, upstream commit `27194180`) with the gaps filled so that the KICK's own app
(the `Kick` target) builds and runs on Windows with MSVC. It is not made, approved or
supported by Music Tribe.

- Import merge commit: `5eed5346` — "Import upstream 0700-AAM main (27194180)".
- Reference fork used to reconstruct the missing pieces:
  [Eight4aWish/0700-AAM](https://github.com/Eight4aWish/0700-AAM), branch `macos-build`
  at `bab631ed` (a macOS port; used as reference, not as authoritative Music Tribe code).
  The fork has since advanced to `96e983b0` (a fix for an empty recent-projects list); the
  stand-ins here were reconstructed from `bab631ed`, not from that later commit.

Built with Visual Studio 2022 (Build Tools 17.14, MSVC 19.44), CMake 4.2, the
"Visual Studio 17 2022" generator, x64, Release, against JUCE 7.0.12 with the release's
own multitouch patch. The app starts and shows its Projects screen at 1024 × 600.

## Quick build

```powershell
# from the repository root
Kick/build-windows.ps1
# then, from the directory that holds the exe (assets/templates/demos live there too):
cd Kick/build-win
.\Kick.exe
```

## Exact commands

```powershell
# 1. Pre-clone JUCE 7.0.12 (or let CPM fetch it), then point CMake at it.
git clone --depth 1 --branch 7.0.12 https://github.com/juce-framework/JUCE.git .3rdPartySourceCache/JUCE-7.0.12

# 2. Configure from the Kick directory. Use FORWARD SLASHES in every -D path:
#    FetchContent writes them into a generated CMakeLists.txt, and a backslash
#    becomes an invalid escape there ("Invalid character escape '\U'"). Paths with
#    spaces are fine once requested through an argument array.
$Root = (Get-Location).Path -replace '\\','/'
cmake -S "$Root/Kick" -B "$Root/Kick/build-win" -G "Visual Studio 17 2022" -A x64 `
  -DTIMESTAMP=OFF `
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5 `
  -DCPM_JUCE_SOURCE="$Root/.3rdPartySourceCache/JUCE-7.0.12" `
  -DCPM_AudIO_SOURCE="$Root/AudIO" `
  -DCPM_COM_PROTOCOL_SOURCE="$Root/ComProtocol" `
  -DMT_PLATFORM_GIT_PATH=file:///nonexistent

# 3. Build the app target in Release.
cmake --build "$Root/Kick/build-win" --config Release --target Kick --parallel

# 4. Run it from the build directory so assets/, templates/ and demos/ are found.
cd "$Root/Kick/build-win"
.\Kick.exe
```

`TIMESTAMP=OFF` drops the build-timestamp library; `MT_PLATFORM_GIT_PATH=file:///nonexistent`
keeps CMake from trying to reach the private `AudIO`/`ComProtocol` git server, because the
three `CPM_*_SOURCE` overrides make CPM use the in-tree copies instead.

The executable lands in `Kick/build-win/Kick.exe`, next to the `assets`, `templates` and
`demos` folders that the `Kick` CMakeLists copies there at configure time.

## What was missing, and what stands in for it

The release does not build as published (upstream issue #9): two modules the app includes,
and the CMake file that registers them, were not released; a third module was removed for
licensing; and AudIO ships with its recorder switched off. The pieces below are written from
how Kick uses each one — the names, types and call sites in the released code. **None of it
is Music Tribe's original code**, and every value that the code does not pin down is marked
as a guess in the header.

| Missing piece | Stand-in | Notes |
| --- | --- | --- |
| `AudIO/juce_modules.cmake` | registers `AudIO`, `AudIO_constants`, `AudIO_juce` | three lines |
| `AudIO_constants` module | `MIDI`, `MIDI_CH`, `POLYPHONY`, `GAIN`, `PAN`, `DB` ranges, `UNKNOWN_VALUE`, the engine's sample rate, block size and bus counts | ranges follow `kick_Constants.h`; guesses are marked |
| `AudIO_juce` module | includes AudIO and the JUCE audio modules, and supplies the classes below | the `AUDIO_JUCE_EXTENSIONS` block in `AudIO.h` is empty in the release |
| `SampleRecorder`, `Looper` | thin subclasses of AudIO's published `DRecorder` / `AudioRecorder`, under the names Kick calls | looper undo/redo are no-ops |
| `mt::AudioDevice` | AudIO's `IAudioDevice` driven by `juce::AudioDeviceManager` | |
| `mt::Filter`, `FilterType` | a TPT state-variable filter covering the list in `kick_Constants.h` | pole counts approximated; "Model" types fall back to low-pass |
| `mt::ADSR` five-argument `setParameters` | a subclass of AudIO's `ADSR<float>` | reorders to AudIO's `Params` |
| `juce_serialport` (removed: no licence) | serial port + JUCE input/output streams | POSIX path from the mac fork; the **Win32** path is new here |

Provenance for the reconstructed files is in each file's header. They are AGPL-3.0-or-later
like the surrounding code, and the two JUCE modules carry a `BEGIN_JUCE_MODULE_DECLARATION`
block so JUCE's CMake can register them.

## Windows-specific changes

- **`juce_serialport` Win32 implementation** (`Kick/modules/3rdParty/juce_serialport`).
  The mac fork's file is POSIX `termios`. This tree adds a Windows path behind the same
  public API: `CreateFileW` / `SetCommState` / `ReadFile` / `WriteFile`, port discovery via
  `QueryDosDeviceW` (so no extra import library is needed), and `getNativeHandle()`.
  `SerialPort`, `SerialPortConfig`, `SerialPortInputStream`, `SerialPortOutputStream` and
  `SerialPort::getSerialPortPaths()` keep the names Kick calls. The header avoids including
  `<windows.h>` by holding the handle as `void*`. **The code has never talked to real Kick
  hardware** — no device is attached, only COM-port enumeration was exercised.
- **`AudIO/AudIO/AudIO.h`** — `AUDIO_OBJECT_AUDIO_FILE_RECORDER` switched from `0` to `1`
  (Kick's `SampleRecorder` and `Looper` are built on `DRecorder`).
- **`AudIO/AudIO/Daw/DawEngine.cpp`, `DawRouting.cpp`** — `#include <thread>` added for
  `std::this_thread::sleep_for`; MSVC's standard library does not pull it in indirectly.
- **`Kick/modules/hardware/source/kick_WifiFirmware.cpp`** — `#include <sys/stat.h>` added;
  the file calls `stat()` on every platform, and not every standard library exposes it by
  default.
- **`Kick/apps/kick/Main.cpp`** — desktop-only debugging aid taken from the mac fork: set
  `KICK_SNAPSHOT` to a PNG path and the app renders its window to that file after
  `KICK_SNAPSHOT_DELAY_MS` (default 4000) and quits. This is how the Windows screenshot was
  produced.

No PktMgr wrapper was required: JUCE 7.0.12's `juce_add_module` accepts a `PktMgr.hpp`
header and does not require a module-declaration block, so
`juce_add_modules(${COM_PROTOCOL_SOURCE_DIR}/PktMgr)` works as-is.

## Known limitations

- **The software simulator (`KT_KickEM`)** is not built and is known broken upstream (its
  panel code predates the current packet protocol). It is `OFF` by default and off here.
- **VST, playback and multitouch targets** are `OFF` (the defaults).
- **Nothing has been built for the device itself** (aarch64 Buildroot).
- **Audio, recording and the reconstructed filter compile and are wired in, but have not
  been listened to.** `mt::Filter`'s pole counts and the `AudIO_constants` ranges are
  approximations, not measured against hardware.
- **The serial link is a port of the mac fork's file plus a new Win32 path; it is untested
  against real hardware.** On Windows the app opens no port unless one is present.
- The window is a normal 1024 × 600 desktop window; the device's kiosk/fullscreen behaviour
  is Linux-only and not applied on Windows.

## Licence and trademark

All first-party code here remains under the GNU Affero General Public License v3.0 or later
(`LICENSE`); the reconstructed stand-ins are distributed under the same terms. "Behringer",
"KICK" and "Music Tribe" are trademarks of their owners; this is an unofficial build for
local development and is neither endorsed by nor affiliated with Music Tribe.
