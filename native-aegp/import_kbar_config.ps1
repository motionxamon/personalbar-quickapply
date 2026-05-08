param(
    [string]$KbarPath = "C:\Users\User\Downloads\kbar.json",
    [string]$OutputPath = "$env:APPDATA\PersonalBarQuickApply.ini"
)

$ErrorActionPreference = "Stop"

function Escape-IniValue {
    param([AllowNull()][string]$Value)
    if ($null -eq $Value) { return "" }
    return $Value.Replace("\", "\\").Replace("`r", "\r").Replace("`n", "\n").Replace("`t", "\t")
}

function New-Button {
    param(
        [string]$Name,
        [string]$Tooltip,
        [string]$Icon,
        [string]$Color,
        [string]$Action,
        [string]$Value
    )
    [pscustomobject]@{
        Name    = $Name
        Tooltip = $Tooltip
        Icon    = $Icon
        Color   = $Color
        Action  = $Action
        Value   = $Value
    }
}

function Get-AutoIcon {
    param([string]$Name, [string]$Action, [string]$Icon)
    $n = ($Name + " " + $Icon).ToLowerInvariant()
    if ($Icon -match '^(videocam|camera|photo_camera|search|content_cut|scissors|film|delete|trash|auto_fix_high|wand|bolt|zap|play_arrow|play|layers|box|code|refresh|repeat|print|printer|apps|grid-3x3|flag|share|share-alt|swap_horiz|move-horizontal)$') {
        return $Icon
    }
    if ($n -match 'camera|cam rig') { return "videocam" }
    if ($n -match 'screenshot|snapshot|screen') { return "photo_camera" }
    if ($n -match 'reveal|search|source') { return "search" }
    if ($n -match 'trash|clean|delete|remove') { return "delete" }
    if ($n -match 'auto.?trace|cut|scissor|split') { return "content_cut" }
    if ($n -match 'film|video|footage') { return "film" }
    if ($n -match 'reduce|print|render') { return "print" }
    if ($n -match 'grid|utility|util') { return "apps" }
    if ($n -match 'loop|cycle|pingpong|refresh') { return "refresh" }
    if ($n -match 'swap|width|height') { return "swap_horiz" }
    if ($n -match 'exp|code|script|jsx') { return "code" }
    if ($n -match 'pack|share|structure') { return "share" }
    if ($n -match 'universe|flag') { return "flag" }
    if ($n -match 'solid|box|layer') { return "layers" }
    if ($n -match 'wiggle|random|magic|blur|preset') { return "auto_fix_high" }
    if ($Action -eq "menu") { return "play_arrow" }
    if ($Action -eq "preset") { return "auto_fix_high" }
    if ($Action -eq "script") { return "code" }
    return $Icon
}

$orbitScript = @'
(function(){
    var c = app.project.activeItem;
    if (!(c instanceof CompItem)) { alert("Open or select a composition first."); return; }
    app.beginUndoGroup("Create Orbit Camera");
    var sel = c.selectedLayers;
    var ip = 0;
    var op = c.duration;
    if (sel.length > 0) {
        ip = sel[0].inPoint;
        op = sel[0].outPoint;
    }
    var center = [c.width / 2, c.height / 2, 0];
    var n = c.layers.addNull();
    n.name = "Orbit Null";
    n.threeDLayer = true;
    n.inPoint = ip;
    n.outPoint = op;
    n.property("ADBE Transform Group").property("ADBE Position").setValue(center);
    var cam = c.layers.addCamera("Camera 1", [c.width / 2, c.height / 2]);
    cam.inPoint = ip;
    cam.outPoint = op;
    cam.property("ADBE Transform Group").property("ADBE Position").setValue([c.width / 2, c.height / 2, -2666]);
    cam.property("ADBE Transform Group").property("ADBE Point of Interest").setValue(center);
    cam.parent = n;
    app.endUndoGroup();
})();
'@

$revealScript = @'
(function(){
    var c = app.project.activeItem;
    if (!(c instanceof CompItem) || c.selectedLayers.length === 0) { alert("Select a layer first."); return; }
    var src = c.selectedLayers[0].source;
    if (!src) { alert("Selected layer has no project source."); return; }
    for (var i = 1; i <= app.project.numItems; i++) {
        app.project.item(i).selected = false;
    }
    src.selected = true;
})();
'@

$screenshotPngScript = @'
(function(){
    var c = app.project.activeItem;
    if (!(c instanceof CompItem)) { alert("Open or select a composition first."); return; }
    var now = new Date();
    function pad(v) { return (v < 10 ? "0" : "") + v; }
    var name = "AE_Screenshot_" + now.getFullYear() + pad(now.getMonth() + 1) + pad(now.getDate()) + "_" + pad(now.getHours()) + pad(now.getMinutes()) + pad(now.getSeconds()) + ".png";
    var f = File(Folder.desktop.fsName + "/" + name);
    c.saveFrameToPng(c.time, f);
    alert("Saved screenshot:\n" + f.fsName);
})();
'@

$buttons = @(
    (New-Button -Name "Orbit Camera" -Tooltip "Create camera with orbit null" -Icon "camera" -Color "rgba(120, 255, 95, 1)" -Action "script" -Value $orbitScript),
    (New-Button -Name "Reveal Source" -Tooltip "Reveal selected layer source in Project" -Icon "search" -Color "rgba(66, 160, 255, 1)" -Action "script" -Value $revealScript),
    (New-Button -Name "Screenshot PNG" -Tooltip "Save current comp frame as PNG to Desktop" -Icon "photo_camera" -Color "rgba(235, 235, 235, 1)" -Action "script" -Value $screenshotPngScript)
)

if (Test-Path -LiteralPath $KbarPath) {
    $json = Get-Content -LiteralPath $KbarPath -Raw | ConvertFrom-Json
    foreach ($toolbar in $json.toolbars) {
        foreach ($button in $toolbar.buttons) {
            $action = $null
            $value = $null
            switch ([int]$button.type) {
                0 {
                    $action = "expression"
                    $value = [string]$button.expression
                }
                2 {
                    $action = "script"
                    $value = [string]$button.script
                }
                4 {
                    $action = "preset"
                    $value = [string]$button.presetPath
                }
                5 {
                    $action = "menu"
                    $value = [string]$button.menuCommand
                }
                default {
                    $action = $null
                }
            }

            if ([string]::IsNullOrWhiteSpace($action) -or [string]::IsNullOrWhiteSpace($value)) {
                continue
            }
            if ($value -match '^https?://') {
                continue
            }

            $icon = ""
            $color = "rgba(255, 190, 80, 1)"
            if ($button.icon) {
                if ($button.icon.path) { $icon = [string]$button.icon.path }
                if ($button.icon.color) { $color = [string]$button.icon.color }
            }
            if ([string]::IsNullOrWhiteSpace($icon) -or $icon -match '^data:') {
                $icon = [string]$button.name
            }
            $icon = Get-AutoIcon -Name ([string]$button.name) -Action $action -Icon $icon

            $buttons += New-Button `
                -Name ([string]$button.name) `
                -Tooltip ([string]$button.description) `
                -Icon $icon `
                -Color $color `
                -Action $action `
                -Value $value
        }
    }
}

$dir = Split-Path -Parent $OutputPath
if ($dir -and !(Test-Path -LiteralPath $dir)) {
    New-Item -ItemType Directory -Path $dir | Out-Null
}

$lines = New-Object System.Collections.Generic.List[string]
$lines.Add("[Search]")
$lines.Add("Effects=1")
$lines.Add("Presets=1")
$lines.Add("Commands=1")
$lines.Add("AtCursor=0")
$lines.Add("")
$lines.Add("[Toolbar]")
$lines.Add("ButtonSize=68")
$lines.Add("Spacing=8")
$lines.Add("Count=$($buttons.Count)")
$lines.Add("")

for ($i = 0; $i -lt $buttons.Count; $i++) {
    $button = $buttons[$i]
    $lines.Add("[Button$i]")
    $lines.Add("Name=$(Escape-IniValue $button.Name)")
    $lines.Add("Tooltip=$(Escape-IniValue $button.Tooltip)")
    $lines.Add("Icon=$(Escape-IniValue $button.Icon)")
    $lines.Add("Color=$(Escape-IniValue $button.Color)")
    $lines.Add("Action=$(Escape-IniValue $button.Action)")
    $lines.Add("Value=$(Escape-IniValue $button.Value)")
    $lines.Add("")
}

Set-Content -LiteralPath $OutputPath -Value $lines -Encoding Unicode
Write-Host "Imported $($buttons.Count) button(s) to $OutputPath"
