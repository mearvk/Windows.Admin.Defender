# Windows 11

Central list for Windows 11 release, architecture, driver, security, administration, compatibility, and support information.

Future entries should be grouped by release/build where useful and should preserve the applicable Windows security and driver-signing requirements.

## Protected folders and files baseline

The Windows 11 protected-system baseline incorporates the folder/file series defined in the Windows 10 `sacred-files.json` collection as a cross-generation administrative baseline. These locations should be treated as protected from wholesale copying, replacement, deletion, or uncontrolled alteration except through appropriate Windows administration, servicing, deployment, recovery, or application-management procedures.

| Protected location | Protection rationale |
|---|---|
| `C:\\Windows` | Central Windows operating-system tree containing critical system components, execution binaries, configuration, and servicing resources. |
| `C:\\Windows\\System32` | Core Windows DLLs, executables, subsystem components, and system utilities. |
| `C:\\Windows\\SysWOW64` | 32-bit Windows compatibility and system components on applicable 64-bit installations. |
| `C:\\Windows\\WinSxS` | Windows Component Store used for servicing, updates, component management, repair, and rollback. |
| `C:\\Program Files` | Standard application installation tree containing executables and supporting application files. |
| `C:\\Program Files (x86)` | 32-bit application installation tree on applicable 64-bit Windows installations. |
| `C:\\ProgramData` | Shared application and machine-wide configuration/data area; protection should be selective and application-aware. |
| `C:\\Users` | Root of user profiles and user-owned operating data; administrative protection should preserve user data and profile integrity. |
| `C:\\Users\\[Username]\\AppData` | Per-user application state under Local, LocalLow, and Roaming profile areas. |
| `C:\\System Volume Information` | System-managed restore, volume, and indexing data; should not be treated as an ordinary user directory. |
| `C:\\Recovery` | Windows recovery resources used for repair and reset operations where present. |
| `C:\\$Recycle.Bin` | System-managed deleted-file storage on supported volumes. |

### Protection rule

The list is a defensive administration policy, not an instruction to make every location immutable. Windows-managed servicing and authorized application installation may legitimately modify protected locations. Windows security controls, access control, Driver Store rules, code-signing requirements, Secure Boot, and administrator authorization remain authoritative.

The baseline is cross-generation documentation. Windows 11-specific additions should be recorded here as they are identified and verified.
