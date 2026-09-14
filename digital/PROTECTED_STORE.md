# Windows Admin Defender — Protected System Store

## Purpose

Windows Admin Defender includes a **stored-protected** mode that treats selected Windows system files as managed objects in a local, relational-style protected store. The design is conceptually similar to a MySQL-backed catalog: a catalog identifies objects and their metadata, while content is stored once under a cryptographic object identifier.

This is a **vault/copy model**, not a replacement of the live Windows filesystem. The live operating-system files remain under Windows ownership and servicing rules. The store provides a protected reference copy, inventory, integrity record, and provenance record.

## Default mode

`protected_store.enabled` defaults to `true` in `config/windows-admin-defender.json`.

The mode can be selected explicitly by the administrator utility:

```text
driver-defender store-mode on
driver-defender store-mode off
driver-defender store-mode status
```

A future service deployment may also expose the same setting through the Windows service configuration or a centrally managed policy file.

## Storage layout

The default store is:

```text
%ProgramData%\\WindowsAdminDefender\\store\\
    catalog.jsonl
    objects\\
        <SHA-256>
    metadata\\
        <object-id>.json
    locks\\
```

### Catalog

`catalog.jsonl` is the relational-style index. Each record represents a stored object and may contain:

- object identifier;
- SHA-256 content digest;
- original Windows path;
- normalized path;
- file size;
- creation/last-write/access metadata when available;
- Windows file attributes;
- security/ownership metadata where safely readable;
- store timestamp;
- source classification (`P0`, `P1`, `P2`, or `P3`);
- Windows version/build information when collected;
- store status and provenance.

The catalog is append-oriented so an administrative audit trail can be preserved rather than silently overwriting history.

### Objects

The `objects` directory contains content addressed by SHA-256. Identical content therefore needs only one stored object even when several protected paths refer to it.

The object store is not intended to be executable search-and-replace infrastructure. Stored content should be treated as protected evidence/reference material unless a separate, supported Windows recovery procedure explicitly authorizes restoration.

## Protection boundary

The store itself is a protected Windows Admin Defender resource. The minifilter's directory policy and normal Windows ACLs remain independent security layers. The implementation must not disable Windows Resource Protection, TrustedInstaller, Secure Boot, Code Integrity, Defender, UAC, or Driver Store controls to populate or access the store.

The store should normally be created with restrictive ACLs so ordinary applications cannot enumerate or modify its contents. Administrative access should be auditable.

## What is stored

The store is intended to cover most **file-backed system resources** identified by the protected-folder policy, subject to safety and practicality checks. It must not blindly copy every NTFS namespace or live kernel/boot metadata structure.

Examples appropriate for explicit object storage include:

- Windows executables and DLLs;
- system configuration files;
- driver package files when read through supported mechanisms;
- protected application binaries;
- selected Windows-managed data files;
- hashes and metadata for files that should be inventoried but not copied.

Examples that require special handling or should normally be represented by metadata only include live registry hives, NTFS metadata, EFI/MSR structures, active event logs, WinSxS servicing relationships, and other Windows-managed state whose meaning cannot be preserved by a simple byte copy.

## Safety rule

**Do not replace live Windows files merely because a corresponding object exists in the store.** Restoration must be a separate, explicit operation using supported Windows servicing, deployment, recovery, or package-management mechanisms.

## Integrity

Every stored object should be verified against its SHA-256 digest after writing. Duplicate content should resolve to the existing object rather than creating a second content copy.

The catalog should support later integrity checks without trusting a filename or source path as the identity of an object.

## Configuration precedence

The intended precedence is:

1. explicit command-line flag;
2. administrator-managed configuration file;
3. compiled safe default (`enabled`).

An explicit `--protected-store` or `--no-protected-store` flag therefore overrides the JSON configuration for that invocation. The persistent configuration remains unchanged unless an administrator explicitly edits it or uses a configuration-management command.

## Design objective

The protected store provides a durable system-file inventory and reference layer while preserving the distinction between **stored data** and the **live Windows operating system**. Its purpose is integrity, provenance, controlled administration, and protection against unauthorized modification or wholesale copying—not circumvention of Windows security controls.
