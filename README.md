# Go to bed - Skyrim 1.7.104.0 update

This repository contains the native SKSE plugin source for the Skyrim
`1.7.104.0` compatibility update of **Go to bed** `2.1.0`.

The original mod and source were created by [andrelo1](https://github.com/andrelo1/gotobed-se).
The runtime compatibility update was prepared by DeadOnKeyboard. Original
credits, copyright, and license terms are preserved.

## Compatibility

- Skyrim Special Edition / Anniversary Edition runtime `1.7.104.0`
- SKSE `2.3.1`
- Address Library for SKSE Plugins for runtime `1.7.104.0`
- JContainers is required by the full mod package
- SkyUI is recommended for configuration

The native hooks contain runtime-specific offsets. Do not treat this build as a
universal DLL for other Skyrim runtimes.

## Dependencies

- [CommonLibSSE](https://github.com/Ryan-rsm-McKenzie/CommonLibSSE), commit
  `0e9d380b90950eb3ece1e5b95e3b6a379ee03f8e`
- [Microsoft Detours](https://github.com/microsoft/Detours) `4.0.1`
- [nlohmann/json](https://github.com/nlohmann/json) `3.12.0`
- fmt, spdlog, binary_io, and Boost through CommonLibSSE

## Build prerequisites

- Visual Studio 2022 or newer with Desktop development with C++
- CMake 3.20 or newer
- [vcpkg](https://github.com/microsoft/vcpkg)

## Build

Open an x64 Native Tools command prompt and run:

```powershell
cmake -B build -S . `
  -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake `
  -DVCPKG_TARGET_TRIPLET=x64-windows-static-md `
  -DVCPKG_OVERLAY_PORTS=vcpkg/ports
cmake --build build --config Release
```

The release archive itself is intentionally excluded from the source
repository. To add all required notices to an existing Vortex archive without
changing its game files:

```powershell
.\tools\Add-LicensesToPackage.ps1 -ArchivePath C:\path\to\gotobed.zip
```

## License

This compatibility update is distributed under
[GPL-3.0-or-later](LICENSE). The original Go to bed source and copyright
notice remain available under their original MIT terms in
[`licenses/GoToBed-MIT.txt`](licenses/GoToBed-MIT.txt).

Bundled and linked third-party components retain their own licenses. See
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) and the [licenses](licenses)
directory.
