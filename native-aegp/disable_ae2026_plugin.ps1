$ErrorActionPreference = "Stop"

$plugin = "C:\Program Files\Adobe\Adobe After Effects 2026\Support Files\Plug-ins\PersonalBar\PersonalBarQuickApply.aex"
$disabled = "$plugin.disabled"

if (Test-Path -LiteralPath $plugin) {
    if (Test-Path -LiteralPath $disabled) {
        Remove-Item -LiteralPath $disabled -Force
    }
    Rename-Item -LiteralPath $plugin -NewName "PersonalBarQuickApply.aex.disabled" -Force
}

Get-ChildItem -Force -LiteralPath (Split-Path -Parent $plugin) | Select-Object FullName, Length, LastWriteTime | Format-List
Start-Sleep -Seconds 2
