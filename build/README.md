# Windows Admin Defender — Build

This directory defines the supported build, packaging, deployment, and administrator command-line boundary for the Windows Admin Defender file-system minifilter under `source/`.

## Build requirements

The driver is a Windows file-system minifilter and must be built with the Windows Driver Kit (WDK). The build must include the Filter Manager headers/libraries and produce the kernel driver binary from `source/WindowsAdminDefender.c`.

The package boundary is:

- `source/WindowsAdminDefender.c` — Filter Manager minifilter implementation.
- `source/WindowsAdminDefender.h` — shared driver definitions.
- `source/WindowsAdminDefender.inf` — minifilter service, instance, and package definition.
- `source/WindowsAdminDefender.cat` — signed catalog generated during packaging.
- `WindowsAdminDefender.sys` — WDK-built kernel driver output.
- `driver-defender.cpp` — administrator-facing user-mode utility.

The repository does not claim that a production `.sys` or signed `.cat` has already been generated.

## Project files

The build is driven by checked-in MSBuild project files under `source/`:

- `source/WindowsAdminDefender.vcxproj` — WDK kernel-mode (WDM) minifilter project for the driver. Requires the WDK's `WindowsKernelModeDriver10.0` platform toolset and links `fltMgr.lib`.
- `source/DriverDefenderUtility.vcxproj` — user-mode Win32 console project for `driver-defender.cpp` (C++17 for `<filesystem>`, Unicode `wmain`). Produces `driver-defender.exe`.
- `source/WindowsAdminDefender.sln` — solution aggregating both projects for `x64` and `ARM64`, `Debug` and `Release`.

`build/build.ps1` targets these projects directly rather than discovering an arbitrary `.vcxproj`.

## Build script

Use `build/build.ps1` from a Developer PowerShell or an appropriately configured WDK build environment:

```powershell
Set-ExecutionPolicy -Scope Process Bypass
.\build\build.ps1 -Configuration Release -Platform x64
```

The script:

1. Runs the fail-closed SHA-256 **verification gate** (`build/verify.ps1`) before any compilation, per `updates/VERIFY-BEFORE-BUILD.md`. A missing manifest, malformed digest, missing file, unsafe path, or SHA-256 mismatch aborts the build.
2. Builds the kernel minifilter driver (`WindowsAdminDefender.vcxproj`).
3. Builds the user-mode administrator utility (`DriverDefenderUtility.vcxproj`) into `driver-defender.exe`, unless `-SkipUtility` is passed.

Useful parameters:

- `-ManifestPath <path>` — verification manifest (default `updates/UPDATE-MANIFEST.json`).
- `-AllowExampleManifest` — permit the placeholder example manifest to exercise the gate. Placeholder digests still fail closed, so this never yields a false "verified" result.
- `-SkipUtility` — build the driver only.
- `-SkipVerification` — explicit, loud escape hatch; not for production builds.

The verification gate can also be run on its own:

```powershell
.\build\verify.ps1 -ManifestPath .\updates\UPDATE-MANIFEST.json
```

The script validates the expected WDK tools and source/package layout. It does not disable driver-signing policy, Secure Boot, Defender, or other Windows security controls.

## Verification manifest

`build/verify.ps1` verifies build inputs against a JSON manifest that lists each protected file with its expected SHA-256 digest. An example is provided at `updates/UPDATE-MANIFEST.example.json`, and the policy is described in `config/update-policy.json` and `updates/README.md`. Generate a real `updates/UPDATE-MANIFEST.json` (replacing the placeholder digests with the actual SHA-256 of each listed file) before a verified build.

## Minifilter package requirements

The INF now registers the driver as a file-system minifilter service and defines its instance configuration. The checked-in altitude is a development/package placeholder and must be replaced by the officially assigned production altitude before production deployment.

Catalog generation is a separate packaging step and must be performed with the WDK packaging tools for the target architecture. A production catalog must be signed according to the applicable Windows driver-signing policy.

## Command-line administration

The supported terminal vocabulary is documented in `COMMANDS.md`:

- `add` / `install` — install the driver package through PnPUtil and the Driver Store.
- `remove` / `delete` / `uninstall` — remove the published driver package through PnPUtil.
- `status` — query the installed service.
- `policy` — display the directory-lock policy.
- `support` — display administrator support information.
- `module` / `path` — display the driver and support locations.

Installation and removal require an elevated Administrator terminal.

## Installation

Use the Windows Driver Store and PnPUtil:

```cmd
pnputil /add-driver "<path>\\WindowsAdminDefender.inf" /install
```

The package must satisfy the architecture, Windows release, signing, minifilter, and policy requirements of the target machine.

## Removal

Identify the published INF name first:

```cmd
pnputil /enum-drivers
```

Then remove the confirmed package:

```cmd
pnputil /delete-driver oemNN.inf /uninstall
```

`oemNN.inf` is a placeholder and must be replaced with the actual published INF name.

## Runtime policy

The minifilter denies direct directory-handle opens for protected roots while allowing ordinary file opens and reads. This is intended to prevent directory-handle based enumeration, wholesale copying, deletion, and rename operations without making ordinary file content unreadable.

The policy source is `digital/lists/PROTECTED_FOLDERS.md`. The implementation must remain selective and must not treat every writable Windows directory as immutable.

## Security boundary

Installation and deployment must not bypass Secure Boot, code-signing policy, driver-signing requirements, Microsoft Defender, Device Guard, or UAC. Administrative operations should remain visible and auditable.

## Status

WDK compilation, INF validation, catalog generation, signing, and production installation remain environment-specific operations. The build script provides a repeatable validation/build entry point but does not claim to replace Microsoft's signing or deployment requirements.
