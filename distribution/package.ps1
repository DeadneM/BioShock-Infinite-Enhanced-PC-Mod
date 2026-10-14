[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][ValidatePattern('^\d+\.\d+\.\d+$')][string]$Version,
    [Parameter(Mandatory=$true)][ValidatePattern('^[0-9A-Fa-f]{64}$')][string]$AsiSHA256,
    [string]$AsiPath,
    [string]$IniPath,
    [string]$LoaderPath,
    [string]$OutputPath,
    [string]$Objective='Configurable shortcuts and current-vitals refill on validated base 0.15.0.',
    [string]$SuccessCriterion='Test the new bindings, persistent remapping, F1 without capacity increases, and previously validated features.',
    [string]$ValidatedBase='0.15.0',
    [switch]$ListOnly
)
. (Join-Path $PSScriptRoot 'Support.ps1')
$repo=Split-Path $PSScriptRoot -Parent
if(-not $AsiPath){$AsiPath=Join-Path $repo 'dist/InfiniteMod.asi'}
if(-not $IniPath){$IniPath=Join-Path $repo 'dist/InfiniteMod.ini'}
if(-not $LoaderPath){$LoaderPath=Join-Path $repo 'tools/asi-loader/dinput8.dll'}
if(-not $OutputPath){$OutputPath=Join-Path $repo ('releases/InfiniteMod-v'+$Version+'-experimental.zip')}
if((Get-ModHash $AsiPath) -ne $AsiSHA256){throw 'ASI differs from the explicitly checked build.'}
if((Get-ModHash $LoaderPath) -ne $script:InfiniteModLoaderSHA256){throw 'Loader differs from the checked dependency.'}
$pe=[IO.File]::ReadAllBytes((Get-AbsolutePath $AsiPath))
if($pe.Length -lt 0x100 -or [BitConverter]::ToUInt16($pe,0) -ne 0x5a4d){throw 'ASI is not recognized as a PE file.'}
$peOffset=[BitConverter]::ToInt32($pe,0x3c)
if($peOffset -lt 0x40 -or $peOffset -gt $pe.Length-24 -or [BitConverter]::ToUInt32($pe,$peOffset) -ne 0x4550 -or
    [BitConverter]::ToUInt16($pe,$peOffset+4) -ne 0x14c -or -not ([BitConverter]::ToUInt16($pe,$peOffset+22) -band 0x2000)){throw 'Expected ASI format: x86 PE DLL.'}
$files=@(
    @{path='README.md';source=(Join-Path $repo 'README.md')},
    @{path='docs/HISTORY.md';source=(Join-Path $repo 'docs/HISTORY.md')},
    @{path='docs/TECHNICAL-NOTEBOOK.md';source=(Join-Path $repo 'docs/TECHNICAL-NOTEBOOK.md')},
    @{path='docs/TODO.md';source=(Join-Path $repo 'docs/TODO.md')},
    @{path='docs/BUILD-v018.md';source=(Join-Path $repo 'docs/BUILD-v018.md')},
    @{path='INSTALLATION.md';source=(Join-Path $PSScriptRoot 'INSTALLATION.md')},
    @{path='Installer.ps1';source=(Join-Path $PSScriptRoot 'Installer.ps1')},
    @{path='Retour.ps1';source=(Join-Path $PSScriptRoot 'Retour.ps1')},
    @{path='Rollback.ps1';source=(Join-Path $PSScriptRoot 'Rollback.ps1')},
    @{path='Support.ps1';source=(Join-Path $PSScriptRoot 'Support.ps1')},
    @{path='Binaries/Win32/InfiniteMod.asi';source=$AsiPath},
    @{path='Binaries/Win32/InfiniteMod.ini';source=$IniPath},
    @{path='Binaries/Win32/dinput8.dll';source=$LoaderPath},
    @{path='licenses/Ultimate-ASI-Loader-MIT.txt';source=(Join-Path $PSScriptRoot 'licenses/Ultimate-ASI-Loader-MIT.txt')},
    @{path='licenses/Dear-ImGui-MIT.txt';source=(Join-Path $PSScriptRoot 'licenses/Dear-ImGui-MIT.txt')},
    @{path='licenses/MinHook-BSD.txt';source=(Join-Path $PSScriptRoot 'licenses/MinHook-BSD.txt')}
)
if($Version -eq '0.18.1'){$files+=@{path='docs/BUILD-v0181.md';source=(Join-Path $repo 'docs/BUILD-v0181.md')}}
if($Version -eq '0.24.0') {
    foreach($doc in @('BUILD-v024.md','NATIVE-ZOOM-TARGET.md','BUILD-v0181.md','DELIVERY-v0181.md','REJECTED-v021.md','REJECTED-v022.md','REJECTED-v023.md')) {
        $files+=@{path=('docs/'+$doc);source=(Join-Path $repo ('docs/'+$doc))}
    }
}
if(Test-Path -LiteralPath (Join-Path $repo 'LICENSE') -PathType Leaf){$files+=@{path='LICENSE';source=(Join-Path $repo 'LICENSE')}}
$moduleFiles=@()
foreach($file in $files) {
    if(-not (Test-Path -LiteralPath $file.source -PathType Leaf)){throw "Missing release file: $($file.path)"}
    Assert-NoReparse $file.source
    if($file.path.StartsWith('Binaries/Win32/')){$moduleFiles+=[pscustomobject]@{path=$file.path;sha256=(Get-ModHash $file.source);size=(Get-Item -LiteralPath $file.source).Length}}
}
$package=[ordered]@{schema=1;version=$Version;status='EXPERIMENTAL';validatedBase=$ValidatedBase;objective=$Objective;successCriterion=$SuccessCriterion;files=$moduleFiles}
if($ListOnly){@($files | ForEach-Object {$_.path})+'package.json';return}
$zipPath=Get-AbsolutePath $OutputPath
if(Test-Path -LiteralPath $zipPath){throw 'ZIP already exists. Choose a new output path; existing releases are never overwritten.'}
$releaseDirectory=Split-Path $zipPath -Parent;Assert-NoReparse $releaseDirectory
New-Item -ItemType Directory -Path $releaseDirectory -Force | Out-Null
$stage=Join-Path $releaseDirectory ('package-stage-'+[Guid]::NewGuid().ToString('N'))
Assert-ChildPath $releaseDirectory $stage
New-Item -ItemType Directory -Path $stage | Out-Null
foreach($file in $files) {
    $destination=Join-Path $stage $file.path;Assert-ChildPath $stage $destination
    New-Item -ItemType Directory -Path (Split-Path $destination -Parent) -Force | Out-Null
    Copy-ModVerified $file.source $destination (Get-ModHash $file.source)
}
Write-ModJson (Join-Path $stage 'package.json') $package
Add-Type -AssemblyName System.IO.Compression.FileSystem
[IO.Compression.ZipFile]::CreateFromDirectory($stage,$zipPath,[IO.Compression.CompressionLevel]::Optimal,$false)
$archive=[IO.Compression.ZipFile]::OpenRead($zipPath)
try {
    $expected=@($files | ForEach-Object {$_.path})+'package.json'
    if($archive.Entries.Count -ne $expected.Count){throw 'Unexpected ZIP entry count.'}
    foreach($relative in $expected) {
        $entry=@($archive.Entries | Where-Object {$_.FullName.Replace('\','/') -ceq $relative})
        if($entry.Count -ne 1){throw "Unexpected ZIP layout: $relative"}
        $hasher=[Security.Cryptography.SHA256]::Create();$stream=$entry[0].Open()
        try {$hash=[BitConverter]::ToString($hasher.ComputeHash($stream)).Replace('-','')}
        finally {$stream.Dispose();$hasher.Dispose()}
        if($hash -ne (Get-ModHash (Join-Path $stage $relative))){throw "ZIP content verification failed: $relative"}
    }
} finally {$archive.Dispose()}
$zipHash=Get-ModHash $zipPath
[IO.File]::WriteAllText($zipPath+'.sha256',$zipHash+'  '+[IO.Path]::GetFileName($zipPath)+[Environment]::NewLine,[Text.UTF8Encoding]::new($false))
Write-Output "Verified ZIP: $zipPath"
Write-Output "SHA256 : $zipHash"
Write-Output "ASI: $AsiSHA256; status EXPERIMENTAL; validated base $ValidatedBase."
Write-Output 'The ZIP and its contents were verified; this does not constitute in-game validation.'
