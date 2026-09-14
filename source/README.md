# Windows Admin Defender source

Initial Windows 10+ kernel-mode driver source and INF package definition.

## Source contents

- `WindowsAdminDefender.c` — kernel-mode driver entry/unload foundation.
- `WindowsAdminDefender.h` — device and symbolic-link definitions.
- `WindowsAdminDefender.inf` — Windows driver package/service definition.

## Administration

The supported terminal-facing administration model is documented in `../build/COMMANDS.md`. Installation and removal are performed through the standard Windows driver-package mechanisms and require appropriate administrator privileges.

The source package does not contain a concealed self-installation or security-policy bypass. Production deployment remains subject to WDK packaging, catalog generation, signing, Secure Boot, and the security policy of the target Windows system.

## Build relationship

The build documentation in `../build/README.md` describes expected `.sys`, `.inf`, and signed `.cat` outputs and the supported deployment boundary.
