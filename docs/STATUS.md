# PersonalBar Status

Current checkpoint: 2026-05-07

## PersonalBar CEP

Installed extension:

```text
C:\Users\User\AppData\Roaming\Adobe\CEP\extensions\PersonalBar
```

Current features:

- KBar JSON import from `cep/js/kbar-data.js`.
- Toolbar selector for `Working`, `Configure`, and `Layers`.
- Imported KBar buttons.
- Personal built-ins: `CAM ORBIT` and `SRC FIND`.
- Add custom button:
  - external `.jsx` / `.jsxbin` file path;
  - inline JSX/scriptlet;
  - expression;
  - menu command;
  - `.ffx` preset path.
- Toolbar settings:
  - hide/show buttons;
  - move buttons up/down;
  - delete custom buttons.
- Quick search inside the CEP panel with `?`, `Ctrl+K`, or `Ctrl+Space` while the panel has focus.

## Next Track: QuickApply+

Goal:

- Separate launcher command for After Effects.
- Assignable AE keyboard shortcut.
- Search and apply:
  - effects/plugins;
  - animation presets;
  - menu commands;
  - PersonalBar/KBar/custom tools later.

Implementation direction:

- Start with a `.jsx` command script because AE can assign application shortcuts to script commands.
- Use ScriptUI for the first floating search window.
- Keep CEP PersonalBar as the docked toolbar and config surface.

## Native AEGP Track

Scaffold created:

```text
native-aegp
```

Current native target:

- AEGP command plug-in.
- Menu command: `PersonalBar QuickApply+`.
- Command hook opens a native Win32 popup styled closer to Quick Apply.
- Popup now receives real installed effects/plugins from `AEGP_EffectSuite5`.
- Search/filter runs in the native popup.
- Arrow keys, Enter, Escape are handled in the search field.
- Enter attempts to apply the selected effect to selected comp layers via `AEGP_ApplyEffect`.

Build/install status:

- Adobe After Effects SDK 25.6 build 61 is now unpacked locally.
- Visual Studio 2026 Insiders C++ toolchain is detected through `vcvars64.bat`.
- `PersonalBarQuickApply.aex` builds successfully with NMake.
- Export `EntryPointFunc` is present.
- Installed to AE 2026:

```text
C:\Program Files\Adobe\Adobe After Effects 2026\Support Files\Plug-ins\PersonalBar\PersonalBarQuickApply.aex
```

Last fix:

- Corrected AEGP `EntryPointFunc` signature to match SDK typedef:
  `SPBasicSuite*, major, minor, AEGP_PluginID, AEGP_GlobalRefcon*`.
- Previous build used an incompatible signature and caused AE error:
  `{Plugin ID is invalid} (5027 :: 47)`.

Next test:

- Restart After Effects 2026.
- Look for `PersonalBar QuickApply+` in the Edit menu.
- Check whether it appears in Keyboard Shortcuts.
- Run it and confirm the new dark popup appears.
- Type an effect name, select a layer, press Enter, and check whether the effect applies.
