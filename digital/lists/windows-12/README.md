# Windows 12

Central planning and compatibility list for the Windows 12-era operating-system family.

Entries should capture confirmed release/build information as it becomes available, together with architecture, driver, security, administration, compatibility, and support requirements.

Do not treat planning entries as confirmation of an unreleased Windows feature or policy.

## Protected folders and files baseline

For Windows 12-era planning, the protected-system baseline carries forward the folder/file series defined in the Windows 10 `sacred-files.json` collection. The descriptions below explain what each location does and why it is important. These are a forward-looking administrative baseline, not a claim that an unreleased Windows version will use exactly the same layout.

| Protected location | What it does | Why it is important |
|---|---|---|
| `C:\\Windows` | The primary Windows operating-system directory. It contains system components, executable files, configuration resources, servicing data, and other OS-managed content. | Damage or uncontrolled replacement can prevent Windows components and services from operating correctly. The exact future layout must be verified against the released operating system. |
| `C:\\Windows\\System32` | The native Windows system directory containing core executables, DLLs, libraries, and subsystem components. | System services, administration tools, and applications can depend on these components. Architecture-specific behavior must be verified for the actual release. |
| `C:\\Windows\\SysWOW64` | On applicable 64-bit Windows installations, the architecture-specific location used for 32-bit Windows components under WOW64. | It preserves compatibility with 32-bit applications. Its presence and exact role should be verified against the target Windows 12-era architecture. |
| `C:\\Windows\\WinSxS` | The Windows Component Store used by Windows servicing for components, updates, optional features, repair, and rollback. | Component-store integrity is important to system servicing and recovery. Microsoft documentation establishes the role of WinSxS in current Windows versions; future implementation should be verified rather than assumed. |
| `C:\\Program Files` | The standard per-machine application installation location in current Windows versions. | Applications and installers can depend on its expected structure and permissions. The future Windows 12-era implementation should be verified against Microsoft release documentation. |
| `C:\\Program Files (x86)` | The standard 32-bit application installation location on applicable 64-bit Windows systems. | It supports separation and compatibility between 32-bit and 64-bit application components. |
| `C:\\ProgramData` | A machine-wide application-data location for information shared across users and other application state. | Software may depend on shared configuration or data stored here. Microsoft also documents its relationship to Windows servicing, making uncontrolled alteration inappropriate. |
| `C:\\Users` | The root of Windows user profiles, containing user-specific files, settings, and application state. | User profiles contain potentially irreplaceable user data and should be preserved independently of system-maintenance operations. |
| `C:\\Users\\[Username]\\AppData` | Per-user application state, conventionally divided into `Local`, `LocalLow`, and `Roaming` areas. | Applications can depend on these locations for configuration, caches, and user-specific state. Future path behavior must be verified against the released platform. |
| `C:\\System Volume Information` | A protected system-managed location associated with volume-level Windows services and system data. | It is not an ordinary user directory and should not be indiscriminately copied, deleted, or modified. |
| `C:\\Recovery` | A recovery-related location that may contain or lead to Windows recovery resources. Current Windows recovery deployments also use `Windows\\System32\\Recovery` and dedicated recovery-tools partitions. | Recovery infrastructure can provide repair and troubleshooting when normal Windows startup fails. Preserving recovery resources is therefore important to system resilience. |
| `C:\\$Recycle.Bin` | A hidden per-volume system directory used for Recycle Bin contents. | It maintains deleted-file state that can allow users to recover accidentally deleted files. Future filesystem behavior should be verified against the released platform. |

## Planning and protection rule

This is a planning baseline, not a claim about a finalized Windows 12 filesystem layout. Paths, components, permissions, and security policies must be verified against the actual Windows release before implementation.

The list is a defensive administration policy, not an instruction to make every location immutable. Windows servicing, authorized installation, updates, recovery operations, and legitimate application activity may modify protected locations.

Windows security controls, access control, Driver Store rules, code-signing requirements, Secure Boot, and administrator authorization remain authoritative.

## Trusted references

The Windows 12 planning baseline is grounded in Microsoft documentation for currently supported Windows generations. Future entries should be added only when verified against authoritative Windows documentation for the corresponding release.

- Microsoft Learn — [Protected Resource List](https://learn.microsoft.com/en-us/windows/win32/wfp/protected-file-list)
- Microsoft Learn — [File System Redirector](https://learn.microsoft.com/en-us/windows/win32/winprog64/file-system-redirector)
- Microsoft Learn — [Manage the Component Store / WinSxS](https://learn.microsoft.com/en-us/windows-hardware/manufacture/desktop/manage-the-component-store?view=windows-11)
- Microsoft Learn — [Determine the Actual Size of the WinSxS Folder](https://learn.microsoft.com/en-us/windows-hardware/manufacture/desktop/determine-the-actual-size-of-the-winsxs-folder?view=windows-11)
- Microsoft Learn — [Known Folders](https://learn.microsoft.com/en-us/windows/win32/shell/known-folders)
- Microsoft Learn — [Profiles Directory](https://learn.microsoft.com/en-us/windows/win32/shell/profiles-directory)
- Microsoft Learn — [ProgramData](https://learn.microsoft.com/en-us/windows-hardware/customize/desktop/unattend/microsoft-windows-shell-setup-folderlocations-programdata)
- Microsoft Learn — [Windows Recovery Environment](https://learn.microsoft.com/en-us/windows-hardware/manufacture/desktop/windows-recovery-environment--windows-re--technical-reference?view=windows-11)
