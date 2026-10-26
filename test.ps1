$ErrorActionPreference='Stop'
& (Join-Path $PSScriptRoot 'clean/build.ps1') -Test
$testRun=Join-Path $PSScriptRoot ('tools/test-runs/'+[Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $testRun -Force | Out-Null
Push-Location $testRun
try {
    & (Join-Path $PSScriptRoot 'clean/tests/test.exe')
    if($LASTEXITCODE -ne 0){throw 'Clean module tests failed'}
    Write-Output "Private test output: $testRun"
} finally {Pop-Location}
