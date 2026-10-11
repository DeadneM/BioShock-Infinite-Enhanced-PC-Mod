# Shared by the installable ZIP's installer and rollback script.
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$script:InfiniteModGameSHA256='CE469D440E92A5477E35F12D817B6AFF6710D7FCB4F6F6C154A6CF0C59D6492A'
$script:InfiniteModLoaderSHA256='D5A059AA467A7A7127C8F6169F79FA63FF0F55986EE9EB2FD9A281BEBF2AA2E6'
$script:InfiniteModNames=@('InfiniteMod.asi','InfiniteMod.ini','dinput8.dll')
function Get-ModHash([string]$Path) {(Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash}
function Assert-GameClosed {
    # Run from an ordinary local PowerShell session or an elevated one when the
    # game is elevated. Sandboxed process inventories are not authoritative.
    if(Get-Process -Name BioShockInfinite -ErrorAction SilentlyContinue){throw 'Close BioShock Infinite normally before continuing.'}
}
function Get-AbsolutePath([string]$Path) {[IO.Path]::GetFullPath($Path).TrimEnd('\','/')}
function Assert-ChildPath([string]$Parent,[string]$Child) {
    $rootPath=(Get-AbsolutePath $Parent)+[IO.Path]::DirectorySeparatorChar
    $childPath=Get-AbsolutePath $Child
    if(-not $childPath.StartsWith($rootPath,[StringComparison]::OrdinalIgnoreCase)){throw "Path is outside the expected directory: $Child"}
}
function Assert-NoReparse([string]$Path) {
    if((Test-Path -LiteralPath $Path) -and ((Get-Item -LiteralPath $Path -Force).Attributes -band [IO.FileAttributes]::ReparsePoint)) {
        throw "Linked file or directory refused for this operation: $Path"
    }
}
function Get-GameLayout([string]$GameDirectory) {
    $gamePath=Get-AbsolutePath $GameDirectory
    $binariesPath=Join-Path $gamePath 'Binaries'
    $targetPath=Join-Path $binariesPath 'Win32'
    foreach($path in @($gamePath,$binariesPath,$targetPath)){Assert-NoReparse $path}
    $exePath=Join-Path $targetPath 'BioShockInfinite.exe'
    Assert-ChildPath $gamePath $exePath;Assert-NoReparse $exePath
    if(-not (Test-Path -LiteralPath $exePath -PathType Leaf)){throw 'BioShockInfinite.exe was not found in Binaries/Win32.'}
    if((Get-ModHash $exePath) -ne $script:InfiniteModGameSHA256){throw 'Unrecognized executable. This package supports only the verified GOG x86 build.'}
    return [pscustomobject]@{Game=$gamePath;Target=$targetPath;Exe=$exePath}
}
function Write-ModJson([string]$Path,$Value) {
    $tempPath=$Path+'.tmp-'+[Guid]::NewGuid().ToString('N')
    $json=$Value | ConvertTo-Json -Depth 8
    [IO.File]::WriteAllText($tempPath,$json,[Text.UTF8Encoding]::new($false))
    Move-Item -LiteralPath $tempPath -Destination $Path -Force
}
function Copy-ModVerified([string]$Source,[string]$Destination,[string]$Expected) {
    Assert-NoReparse $Source;Assert-NoReparse $Destination
    if((Get-ModHash $Source) -ne $Expected){throw "Source checksum mismatch: $Source"}
    $tempPath=$Destination+'.tmp-'+[Guid]::NewGuid().ToString('N')
    Copy-Item -LiteralPath $Source -Destination $tempPath
    if((Get-ModHash $tempPath) -ne $Expected){throw "Copy checksum mismatch: $Destination"}
    Move-Item -LiteralPath $tempPath -Destination $Destination -Force
    if((Get-ModHash $Destination) -ne $Expected){throw "Final checksum mismatch: $Destination"}
}
function New-ModBackupDirectory([string]$GameDirectory,[string]$Kind) {
    $backupRoot=Join-Path $GameDirectory 'InfiniteMod-Backups'
    Assert-ChildPath $GameDirectory $backupRoot;Assert-NoReparse $backupRoot
    $name=$Kind+'-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff')+'-'+[Guid]::NewGuid().ToString('N').Substring(0,8)
    $path=Join-Path $backupRoot $name
    Assert-ChildPath $backupRoot $path
    New-Item -ItemType Directory -Path $path -Force | Out-Null
    return $path
}
function Read-ModPackage([string]$Directory) {
    $manifestPath=Join-Path $Directory 'package.json'
    $package=Get-Content -LiteralPath $manifestPath -Raw -Encoding UTF8 | ConvertFrom-Json
    if($package.schema -ne 1 -or @($package.files).Count -ne 3){throw 'Unrecognized package manifest.'}
    foreach($name in $script:InfiniteModNames) {
        $relative='Binaries/Win32/'+$name
        $fileEntries=@($package.files | Where-Object {$_.path -ceq $relative})
        if($fileEntries.Count -ne 1 -or $fileEntries[0].sha256 -notmatch '^[0-9A-Fa-f]{64}$'){throw "Missing or invalid entry: $name"}
        $source=Join-Path $Directory $relative
        Assert-ChildPath $Directory $source;Assert-NoReparse $source
        if((Get-ModHash $source) -ne $fileEntries[0].sha256 -or (Get-Item -LiteralPath $source).Length -ne $fileEntries[0].size){throw "Package content was altered: $name"}
    }
    $loader=@($package.files | Where-Object {$_.path -ceq 'Binaries/Win32/dinput8.dll'})[0]
    if($loader.sha256 -ne $script:InfiniteModLoaderSHA256){throw 'Loader differs from the verified public version.'}
    return $package
}
