param([switch]$Test,[string]$ZigPath=$env:INFINITEMOD_ZIG)
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$zig=if($ZigPath){[IO.Path]::GetFullPath($ZigPath)}else{Join-Path $root 'tools/zig-windows-x86_64-0.13.0/zig.exe'}
if(-not (Test-Path -LiteralPath $zig -PathType Leaf)){throw 'Zig not found. Run distribution/bootstrap.ps1 or set INFINITEMOD_ZIG.'}
$zigVersion=& $zig version
if($LASTEXITCODE -ne 0 -or $zigVersion.Trim() -ne '0.13.0'){throw 'Required compiler: Zig 0.13.0.'}
$env:ZIG_GLOBAL_CACHE_DIR=Join-Path $root 'tools/zig-cache'
$env:ZIG_LOCAL_CACHE_DIR=Join-Path $root 'tools/zig-local-cache'
$imgui=Join-Path $root 'vendor/imgui-1.91.9b'
$sources=@((Join-Path $PSScriptRoot 'main.cpp'),(Join-Path $PSScriptRoot 'overlay.cpp'),(Join-Path $PSScriptRoot 'weapons.cpp'),(Join-Path $PSScriptRoot 'aim.cpp'))
$output=Join-Path $PSScriptRoot 'InfiniteMod.asi'
$kind=@('-shared')
$libs=@('-luser32','-lkernel32','-lshell32','-ld3dcompiler_47','-ldxguid')
if($Test){$sources=@((Join-Path $PSScriptRoot 'tests/test.cpp'));$output=Join-Path $PSScriptRoot 'tests/test.exe';$kind=@();$libs+=@('-ld3d11','-ldxgi')}
$minhook=Join-Path $root 'vendor/minhook-1.3.4'
$sources+=@('src/buffer.c','src/hook.c','src/trampoline.c','src/hde/hde32.c') | ForEach-Object {Join-Path $minhook $_}
$sources+=@('imgui.cpp','imgui_draw.cpp','imgui_tables.cpp','imgui_widgets.cpp','backends/imgui_impl_dx11.cpp') | ForEach-Object {Join-Path $imgui $_}
& $zig c++ -target x86-windows-gnu -O2 -Wall -Wextra @kind @sources -I $imgui -I (Join-Path $minhook 'include') -o $output @libs
if($LASTEXITCODE -ne 0){throw 'Compilation failed'}
Get-FileHash $output
