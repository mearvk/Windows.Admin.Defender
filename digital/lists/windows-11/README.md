# Windows 11

Central list for Windows 11 release, architecture, driver, security, administration, compatibility, and support information.

## Protected folders and files baseline

The Windows 11 protected-system baseline incorporates the folder/file series defined in the Windows 10 `sacred-files.json` collection as a cross-generation administrative baseline. The descriptions below explain what each location does and why it matters to system integrity, application operation, recovery, or user data.

| Protected location | What it does | Why it is important |
|---|---|---|
| `C:\\Windows` | The primary Windows operating-system directory. It contains system components, executable files, configuration resources, servicing data, and other OS-managed content. | Damage or uncontrolled replacement can prevent Windows components and services from operating correctly. Microsoft documents critical Windows resources as protected resources. |
| `C:\\Windows\\System32` | The native system directory containing core Windows executables, DLLs, libraries, and subsystem components. On 64-bit Windows, it is the native 64-bit system directory. | Many Windows services, applications, administration tools, and OS functions depend on these components. Microsoft also applies architecture-specific file-system redirection around this directory. |
| `C:\\Windows\\SysWOW64` | On applicable 64-bit Windows installations, this is the architecture-specific location used for 32-bit Windows components under WOW64. | It preserves compatibility with 32-bit applications. Incorrect modification can break legacy applications or create architecture/version mismatches. |
| `C:\\Windows\\WinSxS` | The Windows Component Store. It contains Windows components used by servicing, updates, optional features, repair, and rollback. | It is part of the servicing infrastructure. Microsoft warns that some important system files are located only in WinSxS, so manual deletion or replacement can damage servicing or recovery. |
| `C:\\Program Files` | The standard per-machine installation location for applications and their supporting files. | Installed software expects its files and permissions to remain consistent. Unauthorized modification can break applications and weaken application integrity. |
| `C:\\Program Files (x86)` | The standard installation location for 32-bit applications on applicable 64-bit Windows systems. | It maintains separation between 32-bit and 64-bit application components and supports Windows application compatibility. |
| `C:\\ProgramData` | A machine-wide application-data location used by applications for data shared across users, configuration, and other non-user-profile information. | Applications may depend on this data even when no individual user profile is active. Microsoft documents ProgramData as a distinct application-data location and warns that changing its servicing location can affect Windows servicing. |
| `C:\\Users` | The root of Windows user profiles. Individual profiles contain personal files, settings, and application data. | It contains user-owned information and profile state. Administrative tooling must avoid treating user data as disposable system content. |
| `C:\\Users\\[Username]\\AppData` | Per-user application state, conventionally divided into `Local`, `LocalLow`, and `Roaming` areas. | Applications use these locations for settings, caches, and user-specific state. Microsoft documents these as per-user known-folder locations, and incorrect changes can cause application or profile problems. |
| `C:\\System Volume Information` | A system-managed directory associated with volume-level Windows services and protected system data. | It is not an ordinary user directory. It can contain data required by system recovery and volume-management features, so administrative tools should not modify it indiscriminately. |
| `C:\\Recovery` | A recovery-related location where recovery resources may be present. Windows Recovery Environment resources are commonly associated with `Windows\\System32\\Recovery` and, after deployment, a dedicated recovery-tools partition. | Recovery infrastructure provides repair and troubleshooting capability when Windows cannot boot normally. Preserving it increases the ability to recover the operating system. |
| `C:\\$Recycle.Bin` | A hidden per-volume system directory used to maintain Recycle Bin contents for deleted files. | It provides a recoverable deletion mechanism and maintains filesystem-managed deleted-file state. It should not be treated as an ordinary application directory. |

## Administrative protection rule

This list is a defensive administration policy, not an instruction to make every location immutable. Windows servicing, authorized installation, updates, recovery operations, and legitimate application activity may modify protected locations. Security controls, access control, Driver Store rules, code-signing requirements, Secure Boot, and administrator authorization remain authoritative.

Paths are documented as representative Windows locations. Actual layout can vary by architecture, installation configuration, release, and recovery configuration. Windows 11-specific additions should be recorded here when they are verified against Microsoft documentation and the target release.

## Trusted references

- Microsoft Learn — [Protected Resource List](https://learn.microsoft.com/en-us/windows/win32/wfp/protected-file-list)
- Microsoft Learn — [File System Redirector](https://learn.microsoft.com/en-us/windows/win32/winprog64/file-system-redirector)
- Microsoft Learn — [Manage the Component Store / WinSxS](https://learn.microsoft.com/en-us/windows-hardware/manufacture/desktop/manage-the-component-store)
- Microsoft Learn — [Determine the Actual Size of the WinSxS Folder](https://learn.microsoft.com/en-us/windows-hardware/manufacture/desktop/determine-the-actual-size-of-the-winsxs-folder)
- Microsoft Learn — [Known Folders](https://learn.microsoft.com/en-us/windows/win32/shell/known-folders)
- Microsoft Learn — [Profiles Directory](https://learn.microsoft.com/en-us/windows/win32/shell/profiles-directory)
- Microsoft Learn — [ProgramData](https://learn.microsoft.com/en-us/windows-hardware/customize/desktop/unattend/microsoft-windows-shell-setup-folderlocations-programdata)
- Microsoft Learn — [Windows Recovery Environment](https://learn.microsoft.com/en-us/windows-hardware/manufacture/desktop/windows-recovery-environment--windows-re--technical-reference?view=windows-11)
