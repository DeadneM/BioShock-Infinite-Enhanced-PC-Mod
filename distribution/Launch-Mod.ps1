[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$GameDirectory)
. (Join-Path $PSScriptRoot 'Support.ps1')
Assert-GameClosed
$layout=Get-GameLayout $GameDirectory
$package=Read-ModPackage $PSScriptRoot
if($package.version -notin @('0.25.0','0.27.0')){throw 'Expected a native FOV settings package.'}
$tool=Join-Path $PSScriptRoot 'NativeFOV.exe';Assert-NoReparse $tool
if($package.nativeFOVToolSHA256 -notmatch '^[0-9A-Fa-f]{64}$' -or (Get-ModHash $tool) -ne $package.nativeFOVToolSHA256){throw 'Native FOV utility checksum mismatch.'}
$module=@($package.files | Where-Object {$_.path -ceq 'Binaries/Win32/InfiniteMod.asi'})[0]
if((Get-ModHash (Join-Path $layout.Target 'InfiniteMod.asi')) -ne $module.sha256){throw 'Install this package before using its launcher.'}
if($package.version -eq '0.27.0'){& $tool apply-slider $layout.Game}else{& $tool apply $layout.Game}
if($LASTEXITCODE -ne 0){throw 'Native range preparation failed; the game was not launched.'}
Assert-GameClosed
Start-Process -FilePath $layout.Exe -WorkingDirectory $layout.Target
