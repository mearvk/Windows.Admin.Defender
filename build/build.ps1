[CmdletBinding()]
<#
.SYNOPSIS
    Windows Admin Defender WDK build entry point.

.DESCRIPTION
    Repeatable build entry point for the Windows Admin Defender file-system
    minifilter and its administrator utility.

    Order of operations (see updates/VERIFY-BEFORE-BUILD.md):
      1. Run the fail-closed SHA-256 verification gate (verify.ps1). No build
         consumes an unverified input.
      2. Build the kernel minifilter project (WindowsAdminDefender.vcxproj).
      3. Build the user-mode administrator utility
         (DriverDefenderUtility.vcxproj), unless skipped.

    This script does NOT disable driver-signing policy, Secure Boot, Defender,
    Device Guard, or UAC. Catalog generation and signing remain a separate,
    explicit WDK packaging step.
#>
param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release",

    [ValidateSet("x64", "ARM64")]
    [string]$Platform = "x64",

    # Skip the utility build (driver only).
    [switch]$SkipUtility,

    # Path to the verification manifest; forwarded to verify.ps1.
    [string]$ManifestPath,

    # Permit the example (placeholder) manifest to exercise the gate. The
    # placeholder digests still fail closed, so this never fabricates success.
    [switch]$AllowExampleManifest,

    # Escape hatch to bypass verification. Explicit, loud, and off by default.
    [switch]$SkipVerification
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $PSScriptRoot
$sourceDir = Join-Path $repoRoot "source"
$outputDir = Join-Path $repoRoot "build\out\$Platform\$Configuration"
$verifyScript = Join-Path $PSScriptRoot "verify.ps1"

$driverProject = Join-Path $sourceDir "WindowsAdminDefender.vcxproj"
$utilityProject = Join-Path $sourceDir "DriverDefenderUtility.vcxproj"
$infPath = Join-Path $sourceDir "WindowsAdminDefender.inf"
$driverSource = Join-Path $sourceDir "WindowsAdminDefender.c"
$adminUtilitySource = Join-Path $sourceDir "driver-defender.cpp"

Write-Host "Windows Admin Defender WDK build" -ForegroundColor Cyan
Write-Host "Configuration: $Configuration"
Write-Host "Platform:      $Platform"

# --- Validate required inputs exist -----------------------------------------
$requiredInputs = @(
    $sourceDir,
    $infPath,
    $driverSource,
    $adminUtilitySource,
    $driverProject
)
if (-not $SkipUtility) {
    $requiredInputs += $utilityProject
}
foreach ($requiredPath in $requiredInputs) {
    if (-not (Test-Path -LiteralPath $requiredPath)) {
        throw "Required build input not found: $requiredPath"
    }
}

# --- 1. Verification gate (verify before build) -----------------------------
if ($SkipVerification) {
    Write-Warning "Verification gate SKIPPED by explicit -SkipVerification. This should not be used for production builds."
}
else {
    if (-not (Test-Path -LiteralPath $verifyScript)) {
        throw "Verification script not found: $verifyScript"
    }
    Write-Host "Running verification gate before build..." -ForegroundColor Cyan

    $verifyArgs = @{}
    if ($ManifestPath) { $verifyArgs["ManifestPath"] = $ManifestPath }
    if ($AllowExampleManifest) { $verifyArgs["AllowExampleManifest"] = $true }

    & $verifyScript @verifyArgs
    if ($LASTEXITCODE -ne 0) {
        throw "Verification gate failed (exit $LASTEXITCODE). Build aborted (fail closed)."
    }
    Write-Host "Verification gate passed." -ForegroundColor Green
}

# --- Locate the WDK toolchain -----------------------------------------------
$msbuild = Get-Command msbuild.exe -ErrorAction SilentlyContinue
if (-not $msbuild) {
    throw "msbuild.exe was not found. Run this script from a Visual Studio/WDK Developer PowerShell or add MSBuild to PATH."
}

$inf2cat = Get-Command Inf2Cat.exe -ErrorAction SilentlyContinue
if (-not $inf2cat) {
    Write-Warning "Inf2Cat.exe was not found. The driver can be compiled, but catalog generation must be performed later in a WDK packaging environment."
}

New-Item -ItemType Directory -Force -Path $outputDir | Out-Null

function Invoke-MsBuild([string]$projectPath, [string]$label) {
    Write-Host "Building ${label}: $projectPath" -ForegroundColor Cyan
    & $msbuild.Source `
        $projectPath `
        "/m" `
        "/p:Configuration=$Configuration" `
        "/p:Platform=$Platform" `
        "/p:OutDir=$outputDir\"
    if ($LASTEXITCODE -ne 0) {
        throw "MSBuild failed for $label with exit code $LASTEXITCODE."
    }
}

# --- 2. Build the kernel minifilter driver ----------------------------------
Invoke-MsBuild $driverProject "kernel minifilter driver"

# --- 3. Build the user-mode administrator utility ---------------------------
if ($SkipUtility) {
    Write-Host "Skipping user-mode utility build (-SkipUtility)." -ForegroundColor Yellow
}
else {
    Invoke-MsBuild $utilityProject "administrator utility (driver-defender.exe)"
}

Write-Host "Build completed: $outputDir" -ForegroundColor Green

if ($inf2cat) {
    Write-Host "Catalog generation is intentionally a separate packaging step; run Inf2Cat against the finalized package directory after verifying the target OS/architecture list." -ForegroundColor Yellow
}

Write-Host "No signing-policy, Secure Boot, Defender, or UAC bypass is performed by this script."
