# Windows Admin Defender — Build

This directory defines the supported build, deployment, and administrator command-line boundary for the kernel-mode driver under `source/`.

## Build requirements

The driver is intended to be built with the Windows Driver Kit (WDK), packaged with its INF/CAT files, and installed through the standard Windows driver deployment path.

A production package should contain, as applicable:

- `WindowsAdminDefender.sys` — kernel driver binary
- `WindowsAdminDefender.inf` — driver installation information
- `WindowsAdminDefender.cat` — signed catalog
- Associated symbols/debug artifacts for development builds

The repository documentation does not claim that a production `.sys` or signed `.cat` has already been generated.

## Command-line administration

The supported terminal vocabulary is documented in `COMMANDS.md`:

- `add` / `install` — install the driver package.
- `remove` / `delete` / `uninstall` — identify and remove the installed package through PnPUtil.
- `status` — query the driver service.
- `support` — prepare administrator support information.
- `module` — store administrator-facing support/reference material in `%ProgramData%\\WindowsAdminDefender`.

Installation and removal require an elevated Administrator terminal.

## Installation

The supported installation mechanism is Windows PnPUtil and the Driver Store. Conceptually:

```cmd
pnputil /add-driver "<path>\\WindowsAdminDefender.inf" /install
```

The package must satisfy the architecture, Windows release, signing, and policy requirements of the target machine.

## Removal

Removal must first identify the actual published INF name assigned by Windows:

```cmd
pnputil /enum-drivers
```

After confirmation, the administrator can remove the published package:

```cmd
pnputil /delete-driver oemNN.inf /uninstall
```

`oemNN.inf` is a placeholder and must be replaced with the actual published INF name.

## Administrator module storage

The administrator support module is intentionally stored in a predictable, visible location:

```text
%ProgramData%\\WindowsAdminDefender
```

It may contain documentation, package references, diagnostics, and support material. It is not a hidden persistence mechanism.

## Security boundary

Installation and deployment must not bypass Secure Boot, code-signing policy, driver-signing requirements, Microsoft Defender, Device Guard, or other Windows security controls. Administrative operations should remain visible and auditable.

## Status

This is the build/deployment specification and command-line administration layer. WDK compilation, catalog generation, signing, and production installation remain environment-specific operations.
