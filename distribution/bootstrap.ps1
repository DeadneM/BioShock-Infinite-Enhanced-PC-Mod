[CmdletBinding()]
param([switch]$SkipZig,[switch]$IncludeLoader,[switch]$ListOnly)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$repo=Split-Path $PSScriptRoot -Parent
$tools=Join-Path $repo 'tools'
$dependencies=@()
if(-not $SkipZig) {
    $dependencies+=[pscustomobject]@{name='Zig 0.13.0 Windows x86_64';url='https://ziglang.org/download/0.13.0/zig-windows-x86_64-0.13.0.zip';sha256='D859994725EF9402381E557C60BB57497215682E355204D754EE3DF75EE3C158';kind='zig'}
}
if($IncludeLoader) {
    $dependencies+=[pscustomobject]@{name='Ultimate ASI Loader 9.7.4 Win32';url='https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/download/v9.7.4/Ultimate-ASI-Loader.zip';sha256='952CEBFC30D525AFC2BDBACA954329D405DED3AA688A83027354DAE14DFD5C5F';kind='loader'}
}
if($ListOnly){$dependencies | Select-Object name,url,sha256;return}
if(-not [Environment]::Is64BitOperatingSystem){throw 'The pinned compiler requires a 64-bit Windows host. The resulting module is still x86.'}
. (Join-Path $PSScriptRoot 'Support.ps1')
Assert-NoReparse $tools
New-Item -ItemType Directory -Path $tools -Force | Out-Null
Add-Type -AssemblyName System.IO.Compression.FileSystem
foreach($dep in $dependencies) {
    $destination=if($dep.kind -eq 'zig'){Join-Path $tools 'zig-windows-x86_64-0.13.0'}else{Join-Path $tools 'asi-loader'}
    Assert-ChildPath $tools $destination;Assert-NoReparse $destination
    $binary=Join-Path $destination $(if($dep.kind -eq 'zig'){'zig.exe'}else{'dinput8.dll'})
    if(Test-Path -LiteralPath $binary) {
        Assert-NoReparse $binary
        if($dep.kind -eq 'zig') {
            $version=& $binary version
            if($LASTEXITCODE -ne 0 -or $version.Trim() -ne '0.13.0'){throw 'Existing Zig differs from 0.13.0; replacement refused.'}
            $env:INFINITEMOD_ZIG=$binary
        } elseif((Get-ModHash $binary) -ne $script:InfiniteModLoaderSHA256){throw 'Existing loader differs from the verified version; replacement refused.'}
        Write-Output "$($dep.name) is already available.";continue
    }
    if(Test-Path -LiteralPath $destination){throw "An incomplete directory already exists: $destination. Replacement refused."}
    $downloads=Join-Path $tools 'downloads';Assert-NoReparse $downloads
    New-Item -ItemType Directory -Path $downloads -Force | Out-Null
    $taskId=[Guid]::NewGuid().ToString('N')
    $archivePath=Join-Path $downloads ($dep.kind+'-'+$taskId+'.zip')
    $stage=Join-Path $downloads ('extract-'+$taskId)
    Assert-ChildPath $tools $archivePath;Assert-ChildPath $tools $stage
    Invoke-WebRequest -UseBasicParsing -Uri $dep.url -OutFile $archivePath
    if((Get-ModHash $archivePath) -ne $dep.sha256){throw "Download checksum mismatch: $($dep.name)"}
    $archive=[IO.Compression.ZipFile]::OpenRead($archivePath)
    try {
        foreach($entry in $archive.Entries) {
            $relative=$entry.FullName.Replace('/',[IO.Path]::DirectorySeparatorChar)
            if([IO.Path]::IsPathRooted($relative) -or $relative.Contains(':')){throw 'Absolute path refused in a dependency archive.'}
            Assert-ChildPath $stage (Join-Path $stage $relative)
        }
    } finally {$archive.Dispose()}
    Expand-Archive -LiteralPath $archivePath -DestinationPath $stage
    if($dep.kind -eq 'zig') {
        $source=Join-Path $stage 'zig-windows-x86_64-0.13.0'
        Assert-ChildPath $tools $source;Assert-ChildPath $tools $destination
        if(-not (Test-Path -LiteralPath (Join-Path $source 'zig.exe'))){throw 'Invalid Zig archive layout.'}
        Move-Item -LiteralPath $source -Destination $destination
        $version=& $binary version
        if($LASTEXITCODE -ne 0 -or $version.Trim() -ne '0.13.0'){throw 'Incorrect Zig version after extraction.'}
        $env:INFINITEMOD_ZIG=$binary
    } else {
        $source=Join-Path $stage 'dinput8.dll'
        if((Get-ModHash $source) -ne $script:InfiniteModLoaderSHA256){throw 'Extracted loader checksum mismatch.'}
        New-Item -ItemType Directory -Path $destination | Out-Null
        Copy-ModVerified $source $binary $script:InfiniteModLoaderSHA256
    }
    Write-Output "$($dep.name) downloaded and verified."
}
