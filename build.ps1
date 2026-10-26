param([switch]$Test,[switch]$BuildLegacyNativeFOV)
$ErrorActionPreference='Stop'
& (Join-Path $PSScriptRoot 'clean/build.ps1') -Test:$Test -BuildLegacyNativeFOV:$BuildLegacyNativeFOV
if(-not $Test){
    New-Item -ItemType Directory -Path (Join-Path $PSScriptRoot 'dist') -Force | Out-Null
    Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'clean/InfiniteMod.asi') -Destination (Join-Path $PSScriptRoot 'dist/InfiniteMod.asi') -Force
    Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'clean/InfiniteMod.ini') -Destination (Join-Path $PSScriptRoot 'dist/InfiniteMod.ini') -Force
}
