# Windows Admin Defender — Protected Windows Folders

## Purpose

This document defines the central folder-protection policy for Windows 10, Windows 11, Windows 12-era systems, and later Windows releases.

The purpose is to prevent an administrative tool, backup/copy operation, cleanup routine, installer, or repair operation from treating operating-system folders as ordinary application directories.

**Protected does not mean inaccessible.** It means that ordinary operations should not modify, delete, rename, replace, or wholesale-copy the folder unless the operation is an explicitly supported Windows servicing, deployment, recovery, or administrator procedure.

Windows Resource Protection (WRP) protects critical Windows resources and restricts full modification access to TrustedInstaller. Microsoft also documents architecture-specific behavior for `System32`/`SysWOW64`, and Windows component storage in `WinSxS`. citeturn0search2turn0search1turn0search9

## Protection levels

| Level | Meaning |
|---|---|
| **P0 — Critical** | Never modify or wholesale-copy during normal application administration. Use Windows servicing/deployment/recovery mechanisms. |
| **P1 — System** | Treat as operating-system state or system-managed content. Read/inspect only unless an explicit supported procedure requires a change. |
| **P2 — Managed** | Protected from indiscriminate copying or replacement, but legitimate installers, updates, and administrators may manage specific child paths using supported mechanisms. |
| **P3 — Conditional** | Contains user/application data or generated state. Protect the system-managed portions, but do not classify the entire tree as immutable. |

## P0 — Critical operating-system locations

| Path / folder | Typical contents | Why protected |
|---|---|---|
| `%SystemRoot%` / `C:\Windows` | Windows operating-system files, servicing content, configuration, fonts, resources, logs, and system components | Root of the Windows installation. Wholesale replacement can invalidate servicing, boot, component registration, and system integrity. |
| `%SystemRoot%\System32` | Core executables, DLLs, services, drivers, configuration utilities, system libraries, and architecture-native components | Core Windows runtime. On 64-bit Windows, `System32` is the native 64-bit system directory. WRP protects many resources here. citeturn0search1turn0search2 |
| `%SystemRoot%\SysWOW64` | 32-bit Windows system binaries and libraries on 64-bit Windows | Architecture boundary used by WOW64. Replacing or copying it wholesale can break 32-bit application compatibility and system redirection. citeturn0search1 |
| `%SystemRoot%\SysArm32` | ARM32 compatibility components where applicable | Architecture-specific system boundary; must not be treated as an ordinary directory. citeturn0search1 |
| `%SystemRoot%\WinSxS` | Windows component store, component manifests, payloads, servicing and rollback resources | Required for Windows operation, servicing, repair, and rollback. Microsoft specifically warns that important system files may exist only in WinSxS. citeturn0search9 |
| `%SystemRoot%\System32\drivers` | Kernel-mode drivers and related driver files | Kernel drivers participate directly in system execution and hardware access. Driver deployment belongs in the Windows Driver Store and supported installation mechanisms. citeturn0search3 |
| `%SystemRoot%\System32\DriverStore` | Staged driver packages and Driver Store metadata | Windows-managed driver package repository. Driver packages should be added/removed through supported driver-management mechanisms rather than wholesale file copying. citeturn0search1turn0search3 |
| `%SystemRoot%\System32\catroot` | Cryptographic catalog database and related catalog data | Supports verification of signed Windows components and packages; indiscriminate changes can interfere with trust and servicing. citeturn0search1 |
| `%SystemRoot%\System32\catroot2` | Active cryptographic catalog/servicing state | System-managed catalog state; do not copy or replace wholesale. citeturn0search1 |
| `%SystemRoot%\System32\config` | Registry hives and related system configuration state | Contains core registry databases. Replacement or copying from a live system is not an ordinary file operation. |
| `%SystemRoot%\System32\CodeIntegrity` | Code Integrity policy and enforcement-related files | Security policy boundary for executable and driver trust. Do not alter or replace as ordinary data. |
| `%SystemRoot%\System32\SecureBootUpdates` | Secure Boot update-related content where present | Firmware/boot trust boundary. Treat as system-managed security state. |
| `%SystemRoot%\System32\Recovery` | Windows recovery components and configuration | Used by Windows recovery workflows; changes belong to supported recovery/deployment procedures. |
| `%SystemRoot%\System32\Boot` | Boot environment components | Boot-critical content; replacement can prevent Windows from starting. |
| `%SystemRoot%\Boot` | BIOS/boot environment files where applicable | Boot-critical content. Windows documents separate BIOS and UEFI boot-file locations. citeturn1search1turn1search9 |
| `%SystemRoot%\servicing` | Component servicing infrastructure and state | Used by Windows servicing. Do not bulk replace or copy into it. |
| `%SystemRoot%\assembly` | .NET/assembly-related operating-system content where present | System-managed runtime/assembly state; do not treat as an ordinary application directory. |
| `%SystemRoot%\Fonts` | System fonts and font resources | System-wide resources used by Windows and applications. Use supported font installation/removal mechanisms. |
| `%SystemRoot%\Resources` | Windows resource files, themes, localization and UI resources | Shared operating-system resources. Wholesale replacement can change or break the Windows shell and UI. |

## P0 — Boot, firmware, and recovery partitions

These locations may not be ordinary folders and may not have drive letters. They must nevertheless be treated as protected storage locations by a system-management product.

| Location | Typical contents | Why protected |
|---|---|---|
| **EFI System Partition (ESP)** | `EFI\Microsoft\Boot`, Windows Boot Manager, BCD and firmware boot files | The UEFI system partition is the boot entry point for Windows. Microsoft states that it is managed by the operating system and should contain only appropriate system files. citeturn1search6turn1search1 |
| **Microsoft Reserved Partition (MSR)** | Reserved GPT partition space; normally no user files | Reserved for Windows/GPT partition management and not intended for user data. citeturn1search6 |
| **Windows Recovery Environment (WinRE) partition** | `Recovery\WindowsRE`, `Winre.wim`, recovery configuration | Provides system recovery and repair capabilities. Windows normally keeps the recovery partition hidden. citeturn1search8 |
| **BIOS/MBR System partition** | Boot Manager and BCD files under `Boot` where applicable | Contains boot-critical state required for BIOS-based Windows startup. citeturn1search7turn1search1 |

## P1 — Windows-managed system subtrees

| Path / folder | Typical contents | Why protected |
|---|---|---|
| `%SystemRoot%\Logs` | Setup, servicing, diagnostics and other Windows logs | System-generated operational evidence and diagnostics. Logs may be read or collected, but should not be bulk-replaced. |
| `%SystemRoot%\Panther` | Windows Setup/OOBE deployment logs and state | Installation/deployment diagnostics and state. |
| `%SystemRoot%\INF` | Driver and device installation information, including INF files | Device installation infrastructure; changing files arbitrarily can affect driver installation and detection. |
| `%SystemRoot%\System32\LogFiles` | Service and system log files | Windows service diagnostics and operational records. Microsoft identifies this as an important System32 subtree with special filesystem-redirection handling. citeturn0search1 |
| `%SystemRoot%\System32\spool` | Print spooler queues, drivers, processors and print-service state | Printing subsystem state. Microsoft identifies the spool subtree as exempt from normal WOW64 redirection. citeturn0search1 |
| `%SystemRoot%\System32\drivers\etc` | Hosts, networks, protocols, services and related network configuration files | Network name-resolution and service configuration boundary. Microsoft identifies this subtree as exempt from normal WOW64 redirection. citeturn0search1 |
| `%SystemRoot%\System32\Tasks` | Scheduled task definitions | System and application task definitions can provide persistence and scheduled execution. Protect against indiscriminate modification. |
| `%SystemRoot%\Tasks` | Legacy scheduled-task state where present | Scheduled execution state; protect against unauthorized changes. |
| `%SystemRoot%\Temp` | Windows temporary files | System-managed temporary state. It may be writable, but a security tool should not blindly delete or replace its entire contents. |
| `%SystemRoot%\Downloaded Program Files` | Legacy downloaded component content where present | System-managed legacy content; handle through the owning Windows mechanism. |
| `%SystemRoot%\SystemApps` | Built-in Windows packaged applications | Windows shell and system applications. Microsoft documents these as part of the modern Windows application model. citeturn0search4 |
| `%SystemRoot%\ImmersiveControlPanel` | Windows Settings components where present | Core shell/settings functionality. |
| `%SystemRoot%\PolicyDefinitions` | Administrative template definitions | Policy configuration definitions; bulk replacement can change administrative policy interpretation. |

## P1/P2 — Installed application and package locations

| Path / folder | Typical contents | Why protected |
|---|---|---|
| `%ProgramFiles%` | 64-bit applications and shared application components | Machine-wide executable code. Do not wholesale-copy or replace installed applications without the application's installer/package mechanism. |
| `%ProgramFiles(x86)%` | 32-bit applications on 64-bit Windows | Machine-wide 32-bit executable code. Architecture-sensitive and installer-managed. citeturn0search5 |
| `%ProgramFiles%\Common Files` | Shared application libraries/components | Shared dependencies can be used by multiple applications. |
| `%ProgramFiles(x86)%\Common Files` | Shared 32-bit application libraries/components | Shared 32-bit dependencies and architecture-specific components. |
| `%ProgramFiles%\WindowsApps` | Microsoft Store/MSIX application packages | Protected package repository. Windows applications are package-managed and the directory is protected, commonly by TrustedInstaller. citeturn0search4 |
| `%ProgramFiles%\ModifiableWindowsApps` | Package-specific modifiable content where supported | Package-management boundary; modifications must follow package rules. |
| `%ProgramData%` | Machine-wide application data and configuration | Shared application state. Protect system/application-managed subtrees, but do not mark the entire directory immutable because applications legitimately write here. Microsoft identifies ProgramData as the common program-data location. citeturn0search0 |
| `%ProgramData%\Microsoft` | Microsoft application/service state | Contains system and Microsoft application data; protect managed subtrees. |
| `%ProgramData%\Microsoft\Windows` | Windows service and application data | Windows-managed state; protect against indiscriminate copying or deletion. |
| `%ProgramData%\Microsoft\Windows\Start Menu` | Machine-wide Start Menu entries | Shell/application integration state. |

## P2/P3 — User-profile areas requiring selective protection

The entire `C:\Users` tree must **not** be treated as a single immutable system folder. User data belongs to the user. The protection model should instead protect security-sensitive and system-managed subtrees while allowing normal user files to remain usable.

| Path / folder | Typical contents | Why protected |
|---|---|---|
| `%UserProfile%\AppData\Local\Microsoft\Windows` | Windows per-user caches, shell state, application state and system-generated data | Contains security- and shell-relevant per-user state. Protect managed child paths rather than wholesale-copying the tree. |
| `%UserProfile%\AppData\Local\Packages` | Per-user packaged application data | Package-managed application state; preserve package semantics. |
| `%UserProfile%\AppData\Local\Microsoft\WindowsApps` | Per-user application command aliases/package integration | Windows application integration boundary. |
| `%UserProfile%\AppData\Roaming\Microsoft\Windows` | Roaming Windows shell/application settings | User-specific Windows configuration state. |
| `%UserProfile%\NTUSER.DAT` | User registry hive | Active user configuration database; never treat as an ordinary document. |
| `%UserProfile%\AppData\Local\Temp` | User temporary files | Writable temporary area; cleanup should be selective and policy-driven rather than blind. |
| `%Public%` | Shared user-accessible data | Shared data should not be overwritten wholesale because it may contain legitimate user/application content. |

## P1 — Security and identity-sensitive locations

| Path / folder | Typical contents | Why protected |
|---|---|---|
| `%SystemRoot%\System32\GroupPolicy` | Local Group Policy data | Policy state can affect security and administration. |
| `%SystemRoot%\System32\GroupPolicyUsers` | Per-user Group Policy state where present | Policy state affecting individual users. |
| `%SystemRoot%\System32\winevt\Logs` | Windows Event Log files | Security, system, application and operational evidence. Never bulk-replace event logs as ordinary files. |
| `%SystemRoot%\System32\Microsoft-EdgeUpdate` | Microsoft-managed update components where present | Updater components and scheduled update state. |
| `%SystemRoot%\System32\wbem` | WMI infrastructure, providers and repository-related components | WMI is a core management subsystem; arbitrary changes can disrupt system administration and instrumentation. |
| `%SystemRoot%\System32\WindowsPowerShell` | Windows PowerShell components | System administration runtime and supporting files. |
| `%SystemRoot%\System32\WindowsSecurity` | Windows Security components where present | Security UI and supporting components. |

## P1 — Volume and filesystem metadata locations

| Location | Typical contents | Why protected |
|---|---|---|
| `System Volume Information` | Volume restore/indexing/system metadata and filesystem-managed state | Windows-managed volume state. Do not copy, delete, or replace wholesale. |
| `$Recycle.Bin` | Per-volume recycle-bin metadata and deleted-file state | Filesystem/user deletion state; manage through Windows APIs rather than copying its internal structure. |
| `$Extend` | NTFS metadata streams/subdirectories where present | Filesystem metadata rather than ordinary user files. |
| `$Secure` | NTFS security descriptor metadata where present | Filesystem security metadata. |
| `$Extend\$ObjId` | NTFS object identifiers where present | Filesystem metadata. |
| `$Extend\$UsnJrnl` | NTFS change journal where enabled | Filesystem change-tracking state. |

## Protection rules for Windows Admin Defender

1. **Never wholesale-copy P0 folders into a live Windows installation.**
2. **Never recursively replace a protected folder merely because the destination exists.**
3. **Do not use directory ownership or ACL changes as a normal installation technique.**
4. **Do not disable TrustedInstaller, Windows Resource Protection, Secure Boot, Code Integrity, Defender, or UAC to perform a copy.**
5. **Driver packages must use the Driver Store and supported driver-installation mechanisms.** Microsoft documents Driver Store directory 13 specifically as a managed driver-package destination and warns against ordinary `CopyFiles`/`DelFiles` treatment there. citeturn0search3
6. **Boot files must use supported boot/deployment tooling**, such as BCDBoot, when a legitimate repair or deployment operation requires them. citeturn1search1
7. **WinSxS must be serviced through Windows servicing tools**, not manually copied or cleaned by path-based heuristics. citeturn0search9
8. **System32 must be architecture-aware.** Do not assume that `System32` means 32-bit on a 64-bit installation; Microsoft documents the WOW64 redirection model. citeturn0search1
9. **Use environment variables and Windows APIs where possible** rather than hard-coding `C:\Windows` or assuming a particular system-drive letter.
10. **Do not classify every writable system-adjacent directory as immutable.** Logs, temporary directories, ProgramData, and user profiles require selective, context-aware handling.
11. **Before copying any protected tree, preserve metadata semantics** such as ACLs, ownership, reparse points, alternate data streams, hard links, sparse files, and package/servicing relationships. A byte-for-byte recursive copy is not necessarily a valid Windows system image.
12. **Treat unknown Windows system directories conservatively.** If a directory is under `%SystemRoot%`, a system partition, a protected package store, or a Windows-managed metadata namespace, default to inspection-only until its role is established.

## Implementation intent

Windows Admin Defender should use this document as a policy source for future filesystem scanning, backup, comparison, repair, and administration features. The implementation should distinguish:

- **read/inspect** operations;
- **hash/compare** operations;
- **backup/export** operations using supported imaging or backup semantics;
- **supported servicing** operations; and
- **ordinary write/delete/copy operations**, which should be blocked or require a specific administrator-approved workflow when they target protected resources.

This list is deliberately broader than Microsoft's formal WRP protected-resource list. It is an administrative safety boundary for the project, not a claim that every listed directory is individually WRP-locked on every Windows release.
