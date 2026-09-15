[CmdletBinding()]
<#
.SYNOPSIS
    Regenerate updates/UPDATE-MANIFEST.json with current SHA-256 digests.

.DESCRIPTION
    Computes the SHA-256 digest of each protected build input and writes a
    verification manifest consumable by build/verify.ps1. Run this whenever a
    listed source file changes so the verification gate reflects the intended
    bytes.

    This helper only records digests. It does not establish publisher identity
    and does not weaken any Windows security control.

.PARAMETER OutputPath
    Manifest output path. Defaults to updates/UPDATE-MANIFEST.json.
#>
param(
    [string]$OutputPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$buildDir = $PSScriptRoot
$repoRoot = Split-Path -Parent $buildDir
if (-not $OutputPath) {
    $OutputPath = Join-Path $repoRoot "updates\UPDATE-MANIFEST.json"
}

# Protected build inputs, repository-relative, forward-slash paths.
$protectedFiles = @(
    "source/WindowsAdminDefender.c",
    "source/WindowsAdminDefender.h",
    "source/WindowsAdminDefender.inf",
    "source/driver-defender.cpp"
)

$entries = foreach ($relPath in $protectedFiles) {
    $full = Join-Path $repoRoot $relPath
    if (-not (Test-Path -LiteralPath $full -PathType Leaf)) {
        throw "generate-manifest: protected file not found: $relPath"
    }
    [pscustomobject]@{
        path   = $relPath
        sha256 = (Get-FileHash -LiteralPath $full -Algorithm SHA256).Hash.ToLowerInvariant()
    }
}

$manifest = [pscustomobject]@{
    manifest_version = "1.0"
    product          = "WindowsAdminDefender"
    generated_utc    = (Get-Date).ToUniversalTime().ToString("yyyy-MM-ddTHH:mm:ssZ")
    algorithm        = "sha256"
    description      = "Verification manifest for the protected build inputs. Each digest is the SHA-256 of the exact file bytes."
    files            = @($entries)
}

$manifest | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $OutputPath -Encoding UTF8
Write-Host "generate-manifest: wrote $OutputPath ($($entries.Count) files)" -ForegroundColor Green
