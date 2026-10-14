[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$GameDirectory)
. (Join-Path $PSScriptRoot 'Support.ps1')
Assert-GameClosed
$layout=Get-GameLayout $GameDirectory
if($layout.Game -eq (Get-AbsolutePath $PSScriptRoot)){throw 'Extract the ZIP into a separate directory, then specify -GameDirectory. This allows files to be backed up before replacement.'}
$package=Read-ModPackage $PSScriptRoot
if($package.version -eq '0.25.0') {
    $nativeTool=Join-Path $PSScriptRoot 'NativeFOV.exe';Assert-NoReparse $nativeTool
    if($package.nativeFOVToolSHA256 -notmatch '^[0-9A-Fa-f]{64}$' -or (Get-ModHash $nativeTool) -ne $package.nativeFOVToolSHA256){throw 'Native FOV utility checksum mismatch.'}
}
$loaderPath=Join-Path $layout.Target 'dinput8.dll'
Assert-NoReparse $loaderPath
$existingLoader=Test-Path -LiteralPath $loaderPath
if($existingLoader -and (Get-ModHash $loaderPath) -ne $script:InfiniteModLoaderSHA256) {
    throw 'A different dinput8.dll is already present. It is preserved; installation stopped to avoid replacing a shared or unknown loader.'
}
$entries=@()
foreach($name in $script:InfiniteModNames) {
    $path=Join-Path $layout.Target $name
    Assert-ChildPath $layout.Game $path;Assert-NoReparse $path
    $exists=Test-Path -LiteralPath $path
    $source=@($package.files | Where-Object {$_.path -ceq ('Binaries/Win32/'+$name)})[0]
    $beforeHash=if($exists){Get-ModHash $path}else{$null}
    # Existing preferences and loaders are reused byte-for-byte. New keybinds
    # receive their defaults from the ASI without rewriting the user's INI.
    $write=($name -eq 'InfiniteMod.asi') -or (-not $exists)
    $afterHash=if($write){$source.sha256}else{$beforeHash}
    $entries+=[pscustomobject]@{name=$name;beforeExists=$exists;beforeSHA256=$beforeHash;afterSHA256=$afterHash;written=$write}
}
$backup=New-ModBackupDirectory $layout.Game 'install'
foreach($entry in $entries) {
    if($entry.beforeExists){Copy-ModVerified (Join-Path $layout.Target $entry.name) (Join-Path $backup $entry.name) $entry.beforeSHA256}
}
$record=[ordered]@{schema=1;kind='install';state='PREPARED';gameDirectory=$layout.Game;version=$package.version;status=$package.status;objective=$package.objective;successCriterion=$package.successCriterion;files=$entries}
if($package.version -eq '0.25.0'){$record.nativeFOVToolSHA256=$package.nativeFOVToolSHA256;$record.nativeFOVApplied=$false}
$recordPath=Join-Path $backup 'transaction.json'
Write-ModJson $recordPath $record
Assert-GameClosed
try {
    foreach($entry in $entries) {
        $target=Join-Path $layout.Target $entry.name
        # Reject a file changed between preparation and the actual mutation.
        if($entry.beforeExists) {
            if(-not (Test-Path -LiteralPath $target) -or (Get-ModHash $target) -ne $entry.beforeSHA256){throw "File changed during preparation: $($entry.name)"}
        } elseif(Test-Path -LiteralPath $target){throw "A new file appeared during preparation: $($entry.name)"}
        if($entry.written){Copy-ModVerified (Join-Path $PSScriptRoot ('Binaries/Win32/'+$entry.name)) $target $entry.afterSHA256}
    }
    foreach($entry in $entries) {if((Get-ModHash (Join-Path $layout.Target $entry.name)) -ne $entry.afterSHA256){throw "Final verification failed: $($entry.name)"}}
    if($package.version -eq '0.25.0') {
        & $nativeTool apply $layout.Game
        if($LASTEXITCODE -ne 0){throw 'Native FOV range configuration failed; see verified module backup.'}
        $record.nativeFOVApplied=$true;Write-ModJson $recordPath $record
    }
    $record.state='INSTALLED';Write-ModJson $recordPath $record
} catch {
    $record.state='INCOMPLETE';Write-ModJson $recordPath $record
    Write-Warning "Installation interrupted. Verified backup: $backup. Use Rollback.ps1 with this directory."
    throw
}
Write-Output "InfiniteMod $($package.version) installed. Status: $($package.status)."
if($package.version -eq '0.25.0') {
    Write-Output 'Existing mod INI preserved. Only the native FOV range option was configured; EXE and saves unchanged.'
    Write-Output "Launch after range changes: .\Launch-Mod.ps1 -GameDirectory '$($layout.Game)'"
} else {Write-Output 'Existing INI preserved. No game, save or native configuration files were replaced.'}
Write-Output "Rollback: .\Rollback.ps1 -GameDirectory '$($layout.Game)' -BackupDirectory '$backup'"
Write-Output "In-game success criterion: $($package.successCriterion)"
