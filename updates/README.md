# Windows Admin Defender — Daily Updates

The update subsystem checks for updates once every 24 hours by default. The source may be a configured HTTPS endpoint or a named local directory.

Every candidate is staged first. The manifest supplies an expected SHA-256 digest for every file. The exact staged bytes are hashed and compared before any installation step. A mismatch is rejected and the candidate is never installed.

Production unattended updates should additionally require a trusted digital signature/Authenticode publisher policy. SHA-256 proves integrity against the expected digest; it does not establish publisher identity.

The updater must not disable Secure Boot, Defender, Code Integrity, UAC, or driver-signing controls. Driver packages continue through the Windows Driver Store and approved installation mechanisms.

See `config/update-policy.json` and `updates/UPDATE-MANIFEST.example.json`.