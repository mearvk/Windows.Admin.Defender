# Windows Admin Defender — Build

This directory documents the supported build and deployment consequence of the kernel-mode driver under `source/`.

## Deployment consequence

The driver is intended to be built with the Windows Driver Kit (WDK), packaged with its INF/CAT files, and installed through the standard Windows driver deployment path. The repository does **not** provide a self-installing executable that silently establishes kernel persistence.

For production Windows 10+ systems, the build pipeline should produce a properly signed driver package. Driver signing, catalog generation, and installation policy remain subject to the target Windows version and its security configuration.

The current source package defines the kernel driver and an INF service entry with automatic service start. See `source/WindowsAdminDefender.c` and `source/WindowsAdminDefender.inf`.

## Expected build outputs

A completed WDK build/deployment package should contain, as applicable:

- `WindowsAdminDefender.sys` — kernel driver binary
- `WindowsAdminDefender.inf` — driver installation information
- `WindowsAdminDefender.cat` — signed catalog
- Associated symbols/debug artifacts for development builds

## Installation boundary

Installation should be performed by an administrator using supported Windows driver-installation mechanisms. Secure Boot, code-signing policy, driver-signing requirements, and Windows Defender/Device Guard policy must not be bypassed by the build system.

## Status

This is a build/deployment specification. The repository currently contains source and INF material, not a generated `.sys` or signed production catalog.
