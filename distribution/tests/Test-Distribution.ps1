[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$ReferenceExecutable,[string]$CandidateASI)
# Filesystem integration fixture, never a game launch. Private copies remain
# below this test's private/ directory and must not be tracked or distributed.
. (Join-Path (Split-Path $PSScriptRoot -Parent) 'Support.ps1')
Assert-GameClosed
$distribution=Split-Path $PSScriptRoot -Parent
$repo=Split-Path $distribution -Parent
if(-not $CandidateASI){$CandidateASI=Join-Path $repo 'dist/InfiniteMod.asi'}
if((Get-ModHash $ReferenceExecutable) -ne $script:InfiniteModGameSHA256){throw 'Reference executable differs from the supported GOG build.'}
$fixture=Join-Path $PSScriptRoot ('private/install-'+[Guid]::NewGuid().ToString('N'))
Assert-ChildPath $PSScriptRoot $fixture
$package=Join-Path $fixture 'package';$game=Join-Path $fixture 'game'
$source=Join-Path $package 'Binaries/Win32';$target=Join-Path $game 'Binaries/Win32'
New-Item -ItemType Directory -Path $source,$target -Force | Out-Null
Copy-ModVerified $ReferenceExecutable (Join-Path $target 'BioShockInfinite.exe') $script:InfiniteModGameSHA256
foreach($name in @('Installer.ps1','Retour.ps1','Support.ps1')) {Copy-Item -LiteralPath (Join-Path $distribution $name) -Destination (Join-Path $package $name)}
Copy-Item -LiteralPath $CandidateASI -Destination (Join-Path $source 'InfiniteMod.asi')
Copy-Item -LiteralPath (Join-Path $repo 'clean/InfiniteMod.ini') -Destination (Join-Path $source 'InfiniteMod.ini')
Copy-Item -LiteralPath (Join-Path $repo 'tools/asi-loader/dinput8.dll') -Destination (Join-Path $source 'dinput8.dll')
$moduleFiles=@()
foreach($name in $script:InfiniteModNames) {
    $path=Join-Path $source $name
    $moduleFiles+=[pscustomobject]@{path=('Binaries/Win32/'+$name);sha256=(Get-ModHash $path);size=(Get-Item -LiteralPath $path).Length}
}
Write-ModJson (Join-Path $package 'package.json') ([ordered]@{schema=1;version='fixture';status='EXPERIMENTAL';objective='Filesystem fixture only';successCriterion='Preserve settings and restore verified files';files=$moduleFiles})
$candidateHash=Get-ModHash (Join-Path $source 'InfiniteMod.asi')
$defaultINI=[IO.File]::ReadAllBytes((Join-Path $source 'InfiniteMod.ini'))
$originalINI="[General]`r`nEnabled=1`r`n[Display]`r`nFOV=120`r`nHUDScale=1.50`r`n[Keybinds]`r`nMainMenu=Home`r`nVitals=Shift+F1`r`nBindingsMenu=Alt+F8`r`n"
$report=@()
function Check([bool]$Condition,[string]$Label) {if(-not $Condition){throw "FAILED: $Label"}}
function Set-Original {
    [IO.File]::WriteAllText((Join-Path $target 'InfiniteMod.asi'),'synthetic-original-asi',[Text.UTF8Encoding]::new($false))
    [IO.File]::WriteAllText((Join-Path $target 'InfiniteMod.ini'),$originalINI,[Text.UTF8Encoding]::new($false))
    Copy-ModVerified (Join-Path $source 'dinput8.dll') (Join-Path $target 'dinput8.dll') $script:InfiniteModLoaderSHA256
}
function Run-Installer([bool]$ShouldSucceed) {
    $previousPreference=$ErrorActionPreference;$ErrorActionPreference='Continue'
    try {
        $output=& powershell.exe -NoProfile -ExecutionPolicy Bypass -File (Join-Path $package 'Installer.ps1') -GameDirectory $game 2>&1
        $code=$LASTEXITCODE
    } finally {$ErrorActionPreference=$previousPreference}
    if(($code -eq 0) -ne $ShouldSucceed){$output | Out-Host;throw "Installer unexpected exit $code"}
}
function Run-Return([string]$Backup,[bool]$ShouldSucceed) {
    $previousPreference=$ErrorActionPreference;$ErrorActionPreference='Continue'
    try {
        $output=& powershell.exe -NoProfile -ExecutionPolicy Bypass -File (Join-Path $package 'Retour.ps1') -GameDirectory $game -BackupDirectory $Backup 2>&1
        $code=$LASTEXITCODE
    } finally {$ErrorActionPreference=$previousPreference}
    if(($code -eq 0) -ne $ShouldSucceed){$output | Out-Host;throw "Rollback unexpected exit $code"}
}
function Latest-Backup([string]$Prefix) {
    $folders=@(Get-ChildItem -LiteralPath (Join-Path $game 'InfiniteMod-Backups') -Directory | Where-Object {$_.Name.StartsWith($Prefix)} | Sort-Object LastWriteTime -Descending)
    Check ($folders.Count -gt 0) "Missing $Prefix backup"
    return $folders[0].FullName
}
function Archive-FixtureFiles([string]$Scenario) {
    $destination=Join-Path $fixture $Scenario
    Assert-ChildPath $fixture $destination
    New-Item -ItemType Directory -Path $destination | Out-Null
    foreach($name in $script:InfiniteModNames) {
        $path=Join-Path $target $name;Assert-ChildPath $game $path
        if(Test-Path -LiteralPath $path){Move-Item -LiteralPath $path -Destination (Join-Path $destination $name)}
    }
}
Set-Original
$oldASI=Get-ModHash (Join-Path $target 'InfiniteMod.asi');$oldINI=Get-ModHash (Join-Path $target 'InfiniteMod.ini')
Run-Installer $true
$backup=Latest-Backup 'install-'
Check ((Get-ModHash (Join-Path $target 'InfiniteMod.asi')) -eq $candidateHash) 'Candidate installed'
Check ((Get-ModHash (Join-Path $target 'InfiniteMod.ini')) -eq $oldINI) 'All custom INI settings and keybinds preserved byte-for-byte'
Check ((Get-ModHash (Join-Path $target 'dinput8.dll')) -eq $script:InfiniteModLoaderSHA256) 'Existing loader preserved'
Check ((Get-ModHash (Join-Path $backup 'InfiniteMod.asi')) -eq $oldASI) 'Previous ASI backup verified'
Add-Content -LiteralPath (Join-Path $target 'InfiniteMod.ini') -Value '; configuration edited after install'
$editedINI=Get-ModHash (Join-Path $target 'InfiniteMod.ini')
Run-Return $backup $true
$safety=Latest-Backup 'before-rollback-'
Check ((Get-ModHash (Join-Path $target 'InfiniteMod.asi')) -eq $oldASI) 'Previous ASI restored'
Check ((Get-ModHash (Join-Path $target 'InfiniteMod.ini')) -eq $oldINI) 'Previous INI restored'
Check ((Get-ModHash (Join-Path $safety 'InfiniteMod.ini')) -eq $editedINI) 'Latest edited INI retained in rollback safety copy'
$report+='PASS: update preserves all custom INI bindings; verified rollback and edited-INI recovery copy.'

[IO.File]::WriteAllText((Join-Path $target 'dinput8.dll'),'unknown-loader')
$unknownLoader=Get-ModHash (Join-Path $target 'dinput8.dll')
Run-Installer $false
Check ((Get-ModHash (Join-Path $target 'dinput8.dll')) -eq $unknownLoader) 'Unknown loader untouched'
Check ((Get-ModHash (Join-Path $target 'InfiniteMod.asi')) -eq $oldASI) 'Unknown-loader refusal leaves ASI untouched'
Check ((Get-ModHash (Join-Path $target 'InfiniteMod.ini')) -eq $oldINI) 'Unknown-loader refusal leaves INI untouched'
$report+='PASS: unknown existing loader rejected before mutation.'

Set-Original
Add-Content -LiteralPath (Join-Path $source 'InfiniteMod.ini') -Value '; synthetic package tampering'
Run-Installer $false
Check ((Get-ModHash (Join-Path $target 'InfiniteMod.asi')) -eq $oldASI) 'Tampered package leaves ASI untouched'
Check ((Get-ModHash (Join-Path $target 'InfiniteMod.ini')) -eq $oldINI) 'Tampered package leaves INI untouched'
[IO.File]::WriteAllBytes((Join-Path $source 'InfiniteMod.ini'),$defaultINI)
$report+='PASS: altered package content rejected before mutation.'

Archive-FixtureFiles 'prior-to-first-install'
Run-Installer $true
$backup=Latest-Backup 'install-'
Run-Return $backup $true
foreach($name in $script:InfiniteModNames){Check (-not (Test-Path -LiteralPath (Join-Path $target $name))) "First-install rollback removes owned $name into recovery folder"}
$report+='PASS: first installation and return to absent mod files, with recovery copies.'

Run-Installer $true
$backup=Latest-Backup 'install-'
$otherPlugin=Join-Path $target 'OtherFixtureMod.asi'
[IO.File]::WriteAllText($otherPlugin,'another synthetic plugin')
$otherHash=Get-ModHash $otherPlugin
Run-Return $backup $true
Check ((Get-ModHash (Join-Path $target 'dinput8.dll')) -eq $script:InfiniteModLoaderSHA256) 'Newly shared loader preserved'
Check ((Get-ModHash $otherPlugin) -eq $otherHash) 'Other plugin preserved'
Assert-ChildPath $game $otherPlugin
Move-Item -LiteralPath $otherPlugin -Destination (Join-Path $fixture 'OtherFixtureMod.asi')
$report+='PASS: loader retained when another ASI shares it.'

Set-Original
Run-Installer $true
$backup=Latest-Backup 'install-'
[IO.File]::WriteAllText((Join-Path $target 'InfiniteMod.asi'),'different-newer-asi')
$newerASI=Get-ModHash (Join-Path $target 'InfiniteMod.asi')
Run-Return $backup $false
Check ((Get-ModHash (Join-Path $target 'InfiniteMod.asi')) -eq $newerASI) 'Changed ASI refused without overwrite'
$report+='PASS: rollback rejects changed ASI without overwriting it.'
Check ((Get-ModHash (Join-Path $target 'BioShockInfinite.exe')) -eq $script:InfiniteModGameSHA256) 'Private reference EXE remains unchanged'
$report | Write-Output
Write-Output "Private fixture only: $fixture"
Write-Output 'No real game directory modified and no game launched. Do not publish private/.'
