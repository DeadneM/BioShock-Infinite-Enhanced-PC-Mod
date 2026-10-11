$ErrorActionPreference='Stop'
& (Join-Path $PSScriptRoot 'clean/build.ps1') -Test
Push-Location (Join-Path $PSScriptRoot 'clean/tests')
try {
    & .\test.exe
    if($LASTEXITCODE -ne 0){throw 'Clean module tests failed'}
} finally {Pop-Location}
