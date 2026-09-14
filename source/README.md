# Windows Admin Defender source

Windows Admin Defender uses a Windows file-system minifilter to protect selected operating-system and application directories from direct directory-handle operations while preserving normal file reads.

## Source contents

- `WindowsAdminDefender.c` — Filter Manager minifilter implementation.
- `WindowsAdminDefender.h` — shared driver definitions.
- `WindowsAdminDefender.inf` — minifilter service, instance, and package definition.
- `driver-defender.cpp` — administrator-facing user-mode control utility.

## Runtime policy

The minifilter is intentionally selective. For protected roots, ordinary file opens and reads remain available, while direct directory-handle opens are denied. This is designed to restrict directory-handle based enumeration, wholesale copying, deletion, and rename operations without making the file contents unreadable to the operating system.

The policy source is `../digital/lists/PROTECTED_FOLDERS.md`.

## Build relationship

Use `../build/build.ps1` in a WDK-configured environment. `../build/README.md` defines the expected `.sys`, `.inf`, and signed `.cat` package boundary.

The INF registers the component with Windows Filter Manager. Its checked-in altitude is a development/package placeholder; production deployment requires the appropriate assigned altitude and applicable Microsoft driver-signing requirements.

## Administration

The supported terminal-facing administration model is documented in `../build/COMMANDS.md`. Installation and removal are performed through the standard Windows Driver Store and PnPUtil mechanisms and require appropriate administrator privileges.

The source package does not contain a concealed self-installation or security-policy bypass. Production deployment remains subject to WDK packaging, catalog generation, signing, Secure Boot, and the security policy of the target Windows system.
