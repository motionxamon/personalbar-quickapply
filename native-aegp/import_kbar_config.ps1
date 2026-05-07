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

$buttons = @(
    (New-Button -Name "Orbit Camera" -Tooltip "Create camera with orbit null" -Icon "camera" -Color "rgba(120, 255, 95, 1)" -Action "script" -Value $orbitScript),
    (New-Button -Name "Reveal Source" -Tooltip "Reveal selected layer source in Project" -Icon "search" -Color "rgba(66, 160, 255, 1)" -Action "script" -Value $revealScript)
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
