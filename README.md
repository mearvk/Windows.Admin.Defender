# Windows.Admin.Defender

Windows 10, 11, 12-era Windows DLL, SYS, and EXE administrative defense project.

## Directory-locking model

The kernel component is a Windows file-system minifilter. The operating system can continue normal file opens and reads, while protected directories are not intended to be opened directly as directory handles for enumeration, wholesale copying, deletion, or rename operations.

The protected-folder policy is documented in `digital/lists/PROTECTED_FOLDERS.md`.

## Build

The WDK build and package boundary is documented in `build/README.md` and the repeatable entry point is `build/build.ps1`.

Installation uses the Windows Driver Store and PnPUtil. The project does not bypass Secure Boot, driver signing, Defender, Device Guard, UAC, or other Windows security controls.

## Source

- `source/WindowsAdminDefender.c` — file-system minifilter.
- `source/WindowsAdminDefender.h` — driver definitions.
- `source/WindowsAdminDefender.inf` — minifilter package definition.
- `source/driver-defender.cpp` — administrator utility.
