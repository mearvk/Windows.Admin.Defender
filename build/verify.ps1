[CmdletBinding()]
<#
.SYNOPSIS
    Windows Admin Defender fail-closed SHA-256 verification gate.

.DESCRIPTION
    Implements the verification requirement documented in
    updates/VERIFY-BEFORE-BUILD.md and config/update-policy.json:

      1. A manifest must be present.
      2. Each listed file must exist at a safe, repository-relative path.
      3. The exact bytes of each file are hashed with SHA-256.
      4. The computed digest must equal the manifest's expected digest.
      5. ANY problem (missing manifest, malformed entry, missing file, unsafe
         path, or digest mismatch) stops the pipeline (fail closed).

    This script performs integrity verification only. It does not establish
    publisher identity; production unattended updates should additionally
    require a trusted Authenticode publisher policy. It does not disable
    Secure Boot, Defender, Code Integrity, UAC, or driver-signing controls.

.PARAMETER ManifestPath
    Path to the verification manifest (JSON). Defaults to
    updates/UPDATE-MANIFEST.json, falling back to nothing (fail closed) if
    absent.

.PARAMETER AllowExampleManifest
    When set, permits the example manifest with placeholder digests to be used
    ONLY to exercise the gate's structure. Placeholder digests still fail the
    comparison, so this never yields a false "verified" result.

.OUTPUTS
    Exits 0 on successful verification of every file; exits non-zero otherwise.
#>
param(
    [string]$ManifestPath,
    [switch]$AllowExampleManifest
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$buildDir = $PSScriptRoot
$repoRoot = Split-Path -Parent $buildDir

function Fail([string]$message) {
    Write-Error "verify: $message"
    exit 1
}

# --- 1. Locate the manifest (fail closed if absent) -------------------------
if (-not $ManifestPath) {
    $defaultManifest = Join-Path $repoRoot "updates\UPDATE-MANIFEST.json"
    if (Test-Path -LiteralPath $defaultManifest) {
        $ManifestPath = $defaultManifest
    }
    elseif ($AllowExampleManifest) {
        $exampleManifest = Join-Path $repoRoot "updates\UPDATE-MANIFEST.example.json"
        if (Test-Path -LiteralPath $exampleManifest) {
            $ManifestPath = $exampleManifest
            Write-Warning "verify: using EXAMPLE manifest with placeholder digests; comparison will fail closed."
        }
    }
}

if (-not $ManifestPath -or -not (Test-Path -LiteralPath $ManifestPath)) {
    Fail "no manifest found. A verification manifest is required before build. See updates/UPDATE-MANIFEST.example.json."
}

Write-Host "verify: manifest = $ManifestPath" -ForegroundColor Cyan

# --- 2. Parse the manifest (fail closed on malformed JSON) ------------------
try {
    $manifest = Get-Content -LiteralPath $ManifestPath -Raw | ConvertFrom-Json
}
catch {
    Fail "manifest is not valid JSON: $($_.Exception.Message)"
}

if (-not ($manifest.PSObject.Properties.Name -contains "algorithm")) {
    Fail "manifest is missing the 'algorithm' field."
}
if ($manifest.algorithm -ne "sha256") {
    Fail "unsupported manifest algorithm '$($manifest.algorithm)'; only 'sha256' is supported."
}
if (-not $manifest.files -or $manifest.files.Count -eq 0) {
    Fail "manifest lists no files to verify."
}

# --- 3-5. Verify each file --------------------------------------------------
$repoRootFull = [System.IO.Path]::GetFullPath($repoRoot)
$verified = 0

foreach ($entry in $manifest.files) {
    if (-not $entry.path) {
        Fail "manifest contains an entry with no 'path'."
    }
    if (-not $entry.sha256) {
        Fail "manifest entry '$($entry.path)' has no 'sha256' digest."
    }

    $expected = ("$($entry.sha256)").Trim().ToLowerInvariant()
    if ($expected -notmatch '^[0-9a-f]{64}$') {
        Fail "manifest entry '$($entry.path)' has a malformed SHA-256 digest (expected 64 hex chars)."
    }

    # Resolve the repo-relative path and reject path escapes (unsafe paths).
    $candidate = Join-Path $repoRoot $entry.path
    $candidateFull = [System.IO.Path]::GetFullPath($candidate)
    if (-not $candidateFull.StartsWith($repoRootFull, [System.StringComparison]::OrdinalIgnoreCase)) {
        Fail "manifest entry '$($entry.path)' resolves outside the repository (unsafe path)."
    }
    if (-not (Test-Path -LiteralPath $candidateFull -PathType Leaf)) {
        Fail "protected file listed in manifest is missing: $($entry.path)"
    }

    $actual = (Get-FileHash -LiteralPath $candidateFull -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($actual -ne $expected) {
        Fail "SHA-256 mismatch for '$($entry.path)'`n  expected: $expected`n  actual:   $actual"
    }

    Write-Host "verify: OK  $($entry.path)" -ForegroundColor Green
    $verified++
}

Write-Host "verify: $verified file(s) verified successfully." -ForegroundColor Green
exit 0
