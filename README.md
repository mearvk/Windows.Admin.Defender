# Windows.Admin.Defender

Windows 10, 11, 12-era Windows DLL, SYS, and EXE administrative defense project.

## Directory-locking model

The kernel component is a Windows file-system minifilter. The operating system can continue normal file opens and reads, while protected directories are not intended to be opened directly as directory handles for enumeration, wholesale copying, deletion, or rename operations.

The protected-folder policy is documented in `digital/lists/PROTECTED_FOLDERS.md`.

## Protective Intent

The project intends to protect designated system files, directories, executables, libraries, and other system resources from unauthorized exposure to malicious programs, unauthorized use, unauthorized modification, and unauthorized copying or duplication. The objective is to preserve system integrity, administrative control, provenance, and lawful use of protected items.

Any deployment should be implemented consistently with applicable federal law, other applicable law, licensing obligations, Windows security requirements, and authorized administrative procedures. This README describes a defensive engineering objective; it does not claim that every particular protective measure is required or authorized by federal law.

## Intelligent Design Principle

The project applies an engineering principle of deliberate, intelligent design: security controls should be designed to identify, constrain, audit, and prevent corporate or organizational fraud rather than to facilitate fraud, concealment, unauthorized appropriation, or the misrepresentation of protected property. The objective is protection of legitimate systems, records, software, and other protected items—not the creation of mechanisms for corporate fraud or the unlawful taking or concealment of assets.

The project favors high-assurance reasoning and explicit system boundaries: protection should be understandable, auditable, technically constrained, and directed toward legitimate defensive purposes. No numerical intelligence claim is made by this repository; the emphasis is on rigorous engineering and sound judgment.

## Build

The WDK build and package boundary is documented in `build/README.md` and the repeatable entry point is `build/build.ps1`.

Installation uses the Windows Driver Store and PnPUtil. The project does not bypass Secure Boot, driver signing, Defender, Device Guard, UAC, or other Windows security controls.

## Source

- `source/WindowsAdminDefender.c` — file-system minifilter.
- `source/WindowsAdminDefender.h` — driver definitions.
- `source/WindowsAdminDefender.inf` — minifilter package definition.
- `source/driver-defender.cpp` — administrator utility.
