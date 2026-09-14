# Windows Admin Defender — Command-Line Method Set

This document defines the supported terminal-facing command set for administrators managing the Windows Admin Defender file-system minifilter.

## Command vocabulary

| Command | Purpose |
|---|---|
| `add` | Add/install the minifilter driver package using the supported Windows Driver Store mechanism. |
| `install` | Alias for `add`. |
| `remove` | Begin administrator-directed removal of the installed driver package. |
| `delete` | Alias for `remove`. |
| `uninstall` | Alias for `remove`. |
| `status` | Query the installed minifilter service. |
| `policy` | Display the directory-lock policy. |
| `support` | Display administrator support information. |
| `module` | Display the installed driver module location. |
| `path` | Display the visible administrator support directory. |

## Supported installation method

Installation uses the Windows Driver Store and PnPUtil rather than a custom mechanism that bypasses Windows driver security:

```cmd
pnputil /add-driver "<path>\\WindowsAdminDefender.inf" /install
```

The INF package must be appropriate for the target architecture and Windows release, and the driver package must satisfy applicable signing and minifilter requirements.

## Removal method

First enumerate Driver Store packages:

```cmd
pnputil /enum-drivers
```

After confirming the correct published INF, remove that package:

```cmd
pnputil /delete-driver oemNN.inf /uninstall
```

`oemNN.inf` is a placeholder and must be replaced with the actual published INF name.

## Status

The installed service can be queried from an elevated terminal:

```cmd
sc.exe query WindowsAdminDefender
```

## Directory-lock policy

The kernel component is a file-system minifilter. Its intended policy is:

- protected directories can remain reachable for normal file-path access;
- normal file opens and reads remain permitted;
- direct directory-handle opens are denied for protected roots;
- directory-handle based enumeration, wholesale copying, deletion, and rename operations are therefore restricted;
- the policy does not replace NTFS ACLs, Windows Resource Protection, Defender, Secure Boot, or code-signing controls.

The policy source is `digital/lists/PROTECTED_FOLDERS.md`.

## Build

From a WDK-configured Developer PowerShell:

```powershell
Set-ExecutionPolicy -Scope Process Bypass
.\build\build.ps1 -Configuration Release -Platform x64
```

The build script validates the expected WDK toolchain and package layout. Production catalog signing remains a separate Windows driver-signing operation.

## Terminal-facing design

The administrator utility should:

1. Require elevation for installation and removal.
2. Validate the INF path before installation.
3. Use PnPUtil for Driver Store operations.
4. Show the published INF name before destructive removal.
5. Return the underlying Windows operation's exit status.
6. Keep support material in a visible administrator-accessible directory.
7. Avoid silent, concealed, or security-policy-bypassing kernel persistence.
8. Keep directory-lock enforcement inside the documented minifilter policy.

This file is the command-line specification. Build and deployment remain subject to WDK packaging, catalog generation, signing, Secure Boot, and Windows security policy.
