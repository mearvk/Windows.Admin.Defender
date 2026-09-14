# Verification Gate — Before Build, Execution, and Diagnostics

Windows Admin Defender treats update integrity verification as a prerequisite, not a post-build diagnostic.

## Required order

1. Acquire source/update material.
2. Verify the manifest.
3. Compute SHA-256 for every protected candidate.
4. Reject the operation on any mismatch.
5. Only after successful verification may the project build.
6. Only after a successful build may the program execute.
7. Only after successful verification and build may related diagnostics execute.

No build, executable launch, test invocation, diagnostic utility, or driver-install preparation should consume an unverified update candidate.

## Fail closed

A missing manifest, malformed digest, missing file, unsafe path, SHA-256 mismatch, or failed required signature/publisher check must stop the pipeline.

## Local and Internet sources

The same gate applies to the configured local update directory and HTTPS update service. Internet transport does not replace cryptographic verification.

## Defense in depth

The build system should invoke verification first. The execution and diagnostic launchers should independently verify the current protected set before starting. This prevents a stale build-time success from being treated as proof that files remain unchanged.

**Verify before build. Verify before execution. Verify before diagnostics.**
