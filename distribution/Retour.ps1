[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$GameDirectory,[Parameter(Mandatory=$true)][string]$BackupDirectory)
. (Join-Path $PSScriptRoot 'Support.ps1')
Assert-GameClosed
$layout=Get-GameLayout $GameDirectory
$backup=Get-AbsolutePath $BackupDirectory
$backupRoot=Join-Path $layout.Game 'InfiniteMod-Backups'
Assert-ChildPath $backupRoot $backup;Assert-NoReparse $backupRoot;Assert-NoReparse $backup
$record=Get-Content -LiteralPath (Join-Path $backup 'transaction.json') -Raw -Encoding UTF8 | ConvertFrom-Json
if($record.schema -ne 1 -or $record.kind -ne 'install' -or @($record.files).Count -ne 3 -or (Get-AbsolutePath $record.gameDirectory) -ne $layout.Game){throw 'Backup is not recognized for this game.'}
$entries=@()
$otherPlugins=@(Get-ChildItem -LiteralPath $layout.Target -Filter '*.asi' -File -Recurse | Where-Object {$_.Name -cne 'InfiniteMod.asi'})
foreach($name in $script:InfiniteModNames) {
    $fileEntries=@($record.files | Where-Object {$_.name -ceq $name})
    if($fileEntries.Count -ne 1){throw "Invalid backup entry: $name"}
    $entry=$fileEntries[0];$target=Join-Path $layout.Target $name
    Assert-ChildPath $layout.Game $target;Assert-NoReparse $target
    if($entry.afterSHA256 -notmatch '^[0-9A-Fa-f]{64}$'){throw 'Invalid installed checksum.'}
    if($entry.beforeExists) {
        $old=Join-Path $backup $name
        if($entry.beforeSHA256 -notmatch '^[0-9A-Fa-f]{64}$' -or (Get-ModHash $old) -ne $entry.beforeSHA256){throw "Backup content was altered: $name"}
    }
    # A loader already present before installation belongs to that existing
    # setup. It is never replaced, removed or rolled back by this operation.
    $restore=-not ($name -eq 'dinput8.dll' -and ($entry.beforeExists -or $otherPlugins.Count -gt 0))
    if($restore -and (Test-Path -LiteralPath $target) -and $name -ne 'InfiniteMod.ini') {
        $currentHash=Get-ModHash $target
        if($currentHash -ne $entry.afterSHA256 -and (-not $entry.beforeExists -or $currentHash -ne $entry.beforeSHA256)) {
            throw "File changed since this installation: $name. Automatic replacement refused."
        }
    }
    $entries+=[pscustomobject]@{entry=$entry;restore=$restore}
}
$safety=New-ModBackupDirectory $layout.Game 'before-rollback'
$saved=@()
foreach($item in $entries) {
    $target=Join-Path $layout.Target $item.entry.name
    if(Test-Path -LiteralPath $target) {
        $hash=Get-ModHash $target;Copy-ModVerified $target (Join-Path $safety $item.entry.name) $hash
        $saved+=[pscustomobject]@{name=$item.entry.name;sha256=$hash}
    }
}
Write-ModJson (Join-Path $safety 'rollback-safety.json') ([ordered]@{schema=1;gameDirectory=$layout.Game;restoring=$backup;files=$saved})
Assert-GameClosed
foreach($item in $entries) {
    if(-not $item.restore){continue}
    $entry=$item.entry;$target=Join-Path $layout.Target $entry.name
    $last=@($saved | Where-Object {$_.name -ceq $entry.name})
    if($last.Count) {
        if(-not (Test-Path -LiteralPath $target) -or (Get-ModHash $target) -ne $last[0].sha256){throw "File changed during rollback: $($entry.name)"}
    } elseif(Test-Path -LiteralPath $target){throw "A new file appeared during rollback: $($entry.name)"}
    if($entry.beforeExists){Copy-ModVerified (Join-Path $backup $entry.name) $target $entry.beforeSHA256}
    elseif(Test-Path -LiteralPath $target) {
        # Move only this exact verified mod-owned file, retaining a recovery
        # copy; no recursive delete and no shared loader removal.
        $removed=Join-Path $safety ('removed-'+$entry.name)
        Assert-ChildPath $layout.Game $target;Assert-ChildPath $safety $removed
        Move-Item -LiteralPath $target -Destination $removed
    }
    if($entry.beforeExists) {
        if((Get-ModHash $target) -ne $entry.beforeSHA256){throw "Rollback verification failed: $($entry.name)"}
    } elseif(Test-Path -LiteralPath $target){throw "File is still present: $($entry.name)"}
}
Write-Output "Previous ASI/INI restoration verified. Removed state preserved in: $safety"
if($otherPlugins.Count -gt 0){Write-Output 'Loader preserved: other ASI plugins are present.'}
Write-Output 'Progress saves and native game INI files are unchanged.'
