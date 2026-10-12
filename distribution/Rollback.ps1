[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$GameDirectory,[Parameter(Mandatory=$true)][string]$BackupDirectory)
# English entry point; retain the original script for existing rollback commands.
& (Join-Path $PSScriptRoot 'Retour.ps1') -GameDirectory $GameDirectory -BackupDirectory $BackupDirectory
