$ErrorActionPreference = "Stop"

$src = Resolve-Path -LiteralPath "$PSScriptRoot\build-nmake\PersonalBarQuickApply.aex"
$destDir = "C:\Program Files\Adobe\Adobe After Effects 2026\Support Files\Plug-ins\PersonalBar"
$dest = Join-Path $destDir "PersonalBarQuickApply.aex"
$disabled = "$dest.disabled"

New-Item -ItemType Directory -Force -Path $destDir | Out-Null
if (Test-Path -LiteralPath $disabled) {
    Remove-Item -LiteralPath $disabled -Force
}
Copy-Item -LiteralPath $src.Path -Destination $dest -Force
Get-Item -LiteralPath $dest | Select-Object FullName, Length, LastWriteTime | Format-List

Write-Host ""
Write-Host "Installed PersonalBarQuickApply.aex. Restart After Effects 2026."
Start-Sleep -Seconds 3
