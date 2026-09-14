[CmdletBinding()]
param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release",
    [ValidateSet("x64", "ARM64")]
    [string]$Platform = "x64"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $PSScriptRoot
$sourceDir = Join-Path $repoRoot "source"
$outputDir = Join-Path $repoRoot "build\out\$Platform\$Configuration"
$infPath = Join-Path $sourceDir "WindowsAdminDefender.inf"
$driverSource = Join-Path $sourceDir "WindowsAdminDefender.c"
$adminUtilitySource = Join-Path $sourceDir "driver-defender.cpp"

Write-Host "Windows Admin Defender WDK build" -ForegroundColor Cyan
Write-Host "Configuration: $Configuration"
Write-Host "Platform:      $Platform"

foreach ($requiredPath in @($sourceDir, $infPath, $driverSource, $adminUtilitySource)) {
    if (-not (Test-Path -LiteralPath $requiredPath)) {
        throw "Required build input not found: $requiredPath"
    }
}

$msbuild = Get-Command msbuild.exe -ErrorAction SilentlyContinue
if (-not $msbuild) {
    throw "msbuild.exe was not found. Run this script from a Visual Studio/WDK Developer PowerShell or add MSBuild to PATH."
}

$inf2cat = Get-Command Inf2Cat.exe -ErrorAction SilentlyContinue
if (-not $inf2cat) {
    Write-Warning "Inf2Cat.exe was not found. The driver can be compiled, but catalog generation must be performed later in a WDK packaging environment."
}

$project = Get-ChildItem -Path $repoRoot -Filter *.vcxproj -File -Recurse |
    Where-Object { $_.FullName -notmatch "\\build\\out\\" } |
    Select-Object -First 1

if (-not $project) {
    throw "No Visual Studio/WDK .vcxproj was found. Add the WDK project file before invoking the compile step. The source and INF inputs were validated successfully."
}

New-Item -ItemType Directory -Force -Path $outputDir | Out-Null

Write-Host "Building: $($project.FullName)"
& $msbuild.Source `
    $project.FullName `
    "/m" `
    "/p:Configuration=$Configuration" `
    "/p:Platform=$Platform" `
    "/p:OutDir=$outputDir\"

if ($LASTEXITCODE -ne 0) {
    throw "MSBuild failed with exit code $LASTEXITCODE."
}

Write-Host "Build completed: $outputDir" -ForegroundColor Green

if ($inf2cat) {
    Write-Host "Catalog generation is intentionally a separate packaging step; run Inf2Cat against the finalized package directory after verifying the target OS/architecture list." -ForegroundColor Yellow
}

Write-Host "No signing-policy, Secure Boot, Defender, or UAC bypass is performed by this script."
