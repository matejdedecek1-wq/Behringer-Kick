# Configure and build the KICK app (Kick target) on Windows with MSVC.
# See BUILDING_ON_WINDOWS.md. Needs CMake and the Visual Studio 2022 (x64) toolchain on
# PATH (or reachable through the generator), and network access the first time to fetch
# JUCE 7.0.12 and CPM.
#
# Note: every -D path is passed with forward slashes. FetchContent writes these paths into
# a generated CMakeLists.txt, where a Windows backslash is read as an escape sequence.
$ErrorActionPreference = "Stop"

$Root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path -replace '\\', '/'
$Juce = "$Root/.3rdPartySourceCache/JUCE-7.0.12"

if (-not (Test-Path $Juce)) {
    git clone --depth 1 --branch 7.0.12 https://github.com/juce-framework/JUCE.git $Juce
}

cmake -S "$Root/Kick" -B "$Root/Kick/build-win" -G "Visual Studio 17 2022" -A x64 `
    -DTIMESTAMP=OFF `
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 `
    -DCPM_JUCE_SOURCE="$Juce" `
    -DCPM_AudIO_SOURCE="$Root/AudIO" `
    -DCPM_COM_PROTOCOL_SOURCE="$Root/ComProtocol" `
    -DMT_PLATFORM_GIT_PATH=file:///nonexistent

cmake --build "$Root/Kick/build-win" --config Release --target Kick --parallel

Write-Host "Built: $Root/Kick/build-win/Kick.exe"
