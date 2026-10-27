# Kick

Application software for the BE 0700-AAM, written in C++ with [JUCE](https://juce.com).

## License

Kick is licensed under the GNU Affero General Public License v3.0 or later, see [LICENSE](LICENSE).
Bundled third-party code keeps its own license (see below).

## Building

The build will not work out of the box:

- **Private dependencies are not included.** `3rdParty.cmake` fetches two internal libraries,
  `AudIO` and `ComProtocol`, that are not part of this release. Point `MT_PLATFORM_GIT_PATH`
  at your own replacements (`-DMT_PLATFORM_GIT_PATH=<git base url>`) or remove the code that uses them.
- **No cross-compilation SDK.** Build one from the companion Buildroot tree
  (`make KTKick_defconfig && make sdk`) and pass its `share/buildroot/toolchainfile.cmake`
  as `CMAKE_TOOLCHAIN_FILE`. See `how-to/Kick_Compilation.txt`.
- **JUCE** 7.0.12 is downloaded by CPM and patched with `patches/juce_multitouch_7.0.12.patch`.

## Removed from this release

Material that could not be redistributed was removed. The code may still refer to it by path:

- Audio samples (factory content and metronome / percussion sounds).
- The Myriad Pro font. Supply any font you have the rights to.
- The prebuilt aarch64 SDK tarball.
- `modules/3rdParty/juce_serialport`, which has no license.

## Third-party code included

| Component | Location | License |
|---|---|---|
| CPM.cmake | `get_cpm.cmake` | MIT |
| cmake_timestamp (Kalle Raiskila) | `timestamp/` | see `timestamp/license.txt` |
| dywapitchtrack (Antoine Schmitt) | `modules/3rdParty/dywapitchtrack/src` | MIT |
| fpm | `modules/3rdParty/fpm` | MIT |
| juce-toys, JUCE natvis (Jim Credland) | `modules/3rdParty/juce-toys`, `juce.natvis` | MIT |
| magic_enum | `modules/3rdParty/magic_enum` | MIT |
| melatonin_parameters | `modules/3rdParty/melatonin_parameters` | MIT |
| moodycamel ConcurrentQueue | `modules/3rdParty/moodycamel` | see its `LICENSE.md` |
| NanoLog | `modules/3rdParty/NanoLog` | MIT |
| signal | `modules/3rdParty/signal` | MIT |
| dRowAudio waveform | `modules/3rdParty/waveform` | MIT |
| smbPitchShift (Stephan M. Bernsee) | `modules/engine/objects/sampler/smbPitchShift` | Wide Open License |
| stftPitchShift (Juergen Hock) | `modules/engine/objects/sampler/stftPitchShift` | MIT |
| Gaussian blur (Roland Rabien) | `modules/kick_ui_controls/source/kick_utils_blur.h` | BSD 3-Clause |
| ttymidi (Thiago Teixeira) | `apps/midiPort/src/ttymidi.c` | GPL-3.0-or-later |
