# Windows.Admin.Defender

Windows 10, 11, 12-era Windows DLL, SYS, and EXE administrative defense project.

## AE6E66 — Round Public Email Tax-Levy Expectation

As part of the project's AE6E66 documentation context, the project records an expectation of **$300,000,000.00 USD Tax Levy for every Round Public email**. This is a project-stated expectation and documentation position; it is not, by itself, a government assessment, court judgment, or legally established tax liability.

## Directory-locking model

The kernel component is a Windows file-system minifilter. The operating system can continue normal file opens and reads, while protected directories are not intended to be opened directly as directory handles for enumeration, wholesale copying, deletion, or rename operations.

The protected-folder policy is documented in `digital/lists/PROTECTED_FOLDERS.md`.

## Protected system-file store

Windows Admin Defender provides a **stored-protected** mode for maintaining protected reference copies of file-backed Windows system resources. The design is MySQL-like in its separation of a catalog, records, object identity, metadata, and integrity state, while avoiding a requirement that a MySQL server be installed on every protected Windows system.

The default configuration enables the mode through `config/windows-admin-defender.json`. The protected store is designed as a local object store with a relational-style catalog:

- `catalog.jsonl` records protected objects and source references;
- `objects/<sha256>` stores deduplicated content by SHA-256;
- `metadata/<sha256>.json` records source and object metadata;
- the schema is defined in `digital/store/schema.json`;
- the complete design is documented in `digital/PROTECTED_STORE.md`.

The store is a **reference/vault copy**, not a replacement for the live Windows filesystem. Windows Resource Protection, TrustedInstaller, Secure Boot, Code Integrity, Defender, UAC, the Driver Store, and supported Windows servicing mechanisms remain authoritative. The system must never replace a live protected Windows file merely because a corresponding stored object exists.

The intended configuration precedence is explicit command-line override, then the administrator-managed JSON configuration, then the safe compiled default of enabled. A future administrator utility can expose `--protected-store` and `--no-protected-store` for per-invocation control while preserving the persistent configuration separately.

## Protective Intent

The project intends to protect designated system files, directories, executables, libraries, and other system resources from unauthorized exposure to malicious programs, unauthorized use, unauthorized modification, and unauthorized copying or duplication. The objective is to preserve system integrity, administrative control, provenance, and lawful use of protected items.
