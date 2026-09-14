# Windows Admin Defender — Command-Line Method Set

This document defines the supported terminal-facing command set for administrators and users who need to manage the Windows Admin Defender driver package.

## Command vocabulary

| Command | Purpose |
|---|---|
| `add` | Add/install the driver package using the supported Windows driver-package mechanism. |
| `install` | Alias for `add`. |
| `remove` | Begin administrator-directed removal of the installed driver package. |
| `delete` | Alias for `remove`. |
| `uninstall` | Alias for `remove`. |
| `support` | Display or prepare administrator support information. |
| `module` | Store the administrator support module and reference material in the local ProgramData support area. |

## Supported installation method

Installation should use the Windows Driver Store and PnPUtil rather than a custom mechanism that bypasses Windows driver security. From an elevated Administrator Command Prompt, the conceptual operation is:

```cmd
pnputil /add-driver "<path>\\WindowsAdminDefender.inf" /install
```

The INF package must be appropriate for the target architecture and Windows release, and the driver package must satisfy the applicable signing policy.

## Removal method

First enumerate the Driver Store packages and identify the published INF name assigned to Windows Admin Defender:

```cmd
pnputil /enum-drivers
```

After confirming the correct published INF, an administrator can remove that package with the supported command:

```cmd
pnputil /delete-driver oemNN.inf /uninstall
```

`oemNN.inf` is a placeholder. It must be replaced with the actual published INF name returned by Windows.

## Status

The service can be queried from an elevated terminal with:

```cmd
sc.exe query WindowsAdminDefender
```

## Module storage

The `module` method is intended to keep administrator-facing support material in a predictable local location such as:

```text
%ProgramData%\\WindowsAdminDefender
```

The module is ordinary support/reference material. It is not a hidden persistence mechanism and must not bypass Secure Boot, code-signing policy, Defender, Device Guard, or other Windows security controls.

## Terminal-facing design

A future wrapper may expose the exact vocabulary above as a visible command-line interface. It should:

1. Require elevation for installation and removal.
2. Validate the INF path before installation.
3. Use PnPUtil for Driver Store operations.
4. Show the published INF name before destructive removal.
5. Return the underlying Windows operation's exit status.
6. Keep support material in the administrator-accessible module directory.
7. Avoid silent, concealed, or security-policy-bypassing kernel persistence.

## Example interface

```text
Windows Admin Defender

  add       Install driver package
  install   Install driver package
  remove    Remove installed driver package
  delete    Alias for remove
  uninstall Alias for remove
  support   Show support information
  module    Prepare administrator support module
  status    Query driver service
```

This file is a command-line specification. The repository's current implementation remains subject to the standard Windows driver packaging, signing, and installation requirements described in `build/README.md`.
