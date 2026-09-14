# Windows Admin Defender — Central OS Lists

This directory is the central collection point for Windows operating-system administration lists used by the Windows Admin Defender project.

## Supported Windows generations

The collection is organized around the Windows 10, Windows 11, and Windows 12-era operating-system families, with room for subsequent Windows releases and servicing generations.

- `windows-10/` — Windows 10 family and supported release/service information.
- `windows-11/` — Windows 11 family and supported release/service information.
- `windows-12/` — Windows 12-era planning and compatibility information.
- `future/` — Reserved for later Windows generations.

## Central collection model

Each operating-system list should collect administrator-relevant information in a consistent structure rather than scattering OS-specific assumptions throughout the repository.

Recommended categories include:

1. **Release** — release family, version, build, and lifecycle information.
2. **Architecture** — x64, ARM64, and other applicable architecture considerations.
3. **Driver** — driver model, signing, Driver Store, and deployment requirements.
4. **Security** — Secure Boot, code-signing, Defender, Device Guard, UAC, and related policy boundaries.
5. **Administration** — supported administrative commands and service-management behavior.
6. **Compatibility** — compatibility requirements for Windows Admin Defender components.
7. **Support** — documentation, diagnostics, and administrator support material.

## Collection rule

The lists are documentation and organization structures. They do not authorize bypassing Windows security controls, concealed persistence, unsigned kernel loading, or unsupported installation mechanisms.

Driver installation and removal remain subject to the target Windows release, applicable Microsoft driver policies, the Driver Store, code-signing requirements, Secure Boot, and administrator authorization.

## Directory purpose

`digital/lists/` is intended to be the central, careful OS-oriented index for Windows Admin Defender as the project tracks Windows 10, Windows 11, Windows 12, and subsequent Windows generations.
