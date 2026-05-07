# PersonalBar for After Effects

Small dockable ScriptUI panel experiment inspired by K-Bar / MoBar.

This repo currently has three tracks:

- `PersonalBar.jsx` is a simple ScriptUI prototype.
- `cep/` is the CEP panel prototype, closer to K-Bar / MoBar.
- `native-aegp/` is the current native Quick Apply / FX Console style plug-in.

## ScriptUI install

Copy `PersonalBar.jsx` into the After Effects ScriptUI Panels folder:

```text
Adobe After Effects <version>/Support Files/Scripts/ScriptUI Panels/
```

Restart After Effects, then open:

```text
Window > PersonalBar.jsx
```

## CEP install

Copy the whole `cep` folder to the Adobe CEP extensions folder and rename it to `PersonalBar` if desired:

```text
Windows user extensions:
C:\Users\<User>\AppData\Roaming\Adobe\CEP\extensions\PersonalBar
```

The installed folder should contain:

```text
PersonalBar
  CSXS\manifest.xml
  index.html
  css\main.css
  js\main.js
  jsx\host.jsx
```

For unsigned local CEP panels, enable PlayerDebugMode in the Windows registry:

```text
HKEY_CURRENT_USER\Software\Adobe\CSXS.11
PlayerDebugMode = 1
```

Depending on the After Effects / CEP version, the CSXS key can be `CSXS.11`, `CSXS.12`, or another nearby version.

Restart After Effects, then open:

```text
Window > Extensions > PersonalBar
```

## Buttons

- `Orbit Cam` creates a camera parented to a 3D `Orbit Null`.
- If a comp layer is selected, `Orbit Cam` trims the camera and null to that layer's `inPoint` and `outPoint`.
- `Reveal Src` reveals the selected layer's source in the Project panel. It first tries the native AE menu command, then falls back to selecting the source Project item directly.
- The other CEP buttons are placeholders so we can quickly attach new actions.

## KBar import prototype

The current CEP version imports the old KBar export from `kbar.json` into `cep/js/kbar-data.js`.

Current behavior:

- Click the toolbar name in the panel header to switch between imported toolbars.
- Right-click inside the panel to open the same toolbar chooser.
- Click `?` or press `Ctrl+K` / `Ctrl+Space` while the panel is focused to open quick search.
- Click the gear to add custom buttons, hide buttons, or reorder the active toolbar.
- Inline scriptlets, expressions, menu items, extension-open buttons, script file paths, and preset paths are wired.

Custom buttons are stored in CEP `localStorage`.

Supported custom button actions:

- Run `.jsx` / `.jsxbin` by file path.
- Run inline jsx/scriptlet text.
- Set an expression on selected properties.
- Invoke an After Effects menu command.
- Apply a `.ffx` preset file to selected layers.

## QuickApply+

First standalone launcher:

```text
quickapply-plus\PersonalBar_QuickApply.jsx
```

Installed user copies:

```text
C:\Users\User\AppData\Roaming\Adobe\After Effects\26.2\Scripts\PersonalBar_QuickApply.jsx
C:\Users\User\AppData\Roaming\Adobe\After Effects\26.0\Scripts\PersonalBar_QuickApply.jsx
```

Current prototype:

- ScriptUI search window.
- Searches effects/plugins through `app.effects`.
- Scans `.ffx` animation presets from AE preset folders.
- Runs a starter curated list of AE menu commands.
- Prefixes:
  - `e:` effects;
  - `p:` presets;
  - `m:` menu commands.
- Enter applies/runs the selected result.
- Ctrl+Enter applies an effect/preset to a new solid.
- Ctrl+Alt+Enter applies an effect/preset to a new adjustment layer.

To test immediately:

```text
File > Scripts > Run Script File...
```

Choose `quickapply-plus\PersonalBar_QuickApply.jsx`.

If AE picks up the user Scripts folder after restart, it should also be assignable in Keyboard Shortcuts as `PersonalBar_QuickApply.jsx`.

## Native Quick Apply

The active prototype is the native AEGP plug-in in:

```text
native-aegp
```

Current behavior:

- Registers `PersonalBar QuickApply+` in After Effects so it can be assigned to a keyboard shortcut.
- Opens a dark Quick Apply popup.
- Searches installed effects, `.ffx` animation presets, curated AE menu commands, and PersonalBar tools.
- Supports filters from the wrench menu: effects, presets, menu commands, and cursor-position popup mode.
- Supports favorites through right-click; favorites stay higher in search results.
- Includes starter PersonalBar tool buttons such as Orbit Camera and Reveal Source.

The Adobe After Effects SDK is intentionally not committed. Put the SDK under:

```text
native-aegp\third_party\ae_sdk
```

Then configure and build with the commands documented in `native-aegp\README.md`.
