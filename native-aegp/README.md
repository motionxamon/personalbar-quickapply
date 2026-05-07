# PersonalBar QuickApply+ AEGP

Native After Effects plug-in track for a real FX Console / Quick Apply style launcher.

## Current Status

This is the active native Quick Apply prototype.

- Builds a Windows `.aex` AEGP plug-in.
- Adds a menu command named `PersonalBar QuickApply+`.
- Makes the command assignable in After Effects Keyboard Shortcuts.
- Shows a dark popup search window.
- Indexes installed effects/plugins through `AEGP_EffectSuite5`.
- Scans `.ffx` animation presets from the AE preset folders.
- Includes a curated starter list of AE menu commands.
- Supports favorites through right-click, with favorites ranked above regular results.
- Includes PersonalBar tool buttons, currently Orbit Camera and Reveal Source.

## Requirements

Current local status:

- Adobe After Effects SDK 25.6 build 61 is unpacked under `third_party\ae_sdk`.
- Visual Studio 2026 Insiders C++ toolchain is detected through `vcvars64.bat`.
- `PersonalBarQuickApply.aex` builds successfully with NMake.

Install Visual Studio 2022 Build Tools with:

- Desktop development with C++;
- Windows 10/11 SDK;
- MSVC x64 toolset;
- CMake tools for Windows.
- MSBuild.

## Expected SDK Layout

This scaffold expects:

```text
native-aegp
  CMakeLists.txt
  src
  third_party
    ae_sdk
      ae25.6_61.64bit.AfterEffectsSDK
        Examples
          Headers
          Resources
          Util
```

`CMakeLists.txt` auto-detects the nested SDK folder when `AE_SDK_ROOT` points at `third_party\ae_sdk`.

Original downloaded SDK zip:

```text
C:\Users\User\Downloads\AfterEffectsSDK_25.6_61_win.zip
```

Unpacked intermediate:

```text
third_party\ae_sdk_extract
```

Active SDK root:

```text
third_party\ae_sdk\ae25.6_61.64bit.AfterEffectsSDK
```

## Visual Studio Check

The VS 2026 Insiders dev environment works through:

```powershell
cmd /c "call ""C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat"" && cmake --build native-aegp\build-nmake --config Release"
```

Note: `vswhere` does not list this Insiders install for the v143 requirement, but direct `vcvars64.bat` works.

## Build Direction

The first build should target a Windows `.aex` plug-in for AE 2026:

```powershell
cmd /c "call ""C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat"" && cmake -S native-aegp -B native-aegp\build-nmake -G ""NMake Makefiles"" -DCMAKE_BUILD_TYPE=Release"
cmd /c "call ""C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat"" && cmake --build native-aegp\build-nmake --config Release"
```

Install target:

```text
C:\Program Files\Adobe\Adobe After Effects 2026\Support Files\Plug-ins\PersonalBar\PersonalBarQuickApply.aex
```

Installed via elevated helper:

```powershell
native-aegp\install_ae2026_plugin.ps1
```

## SDK Notes

AEGP menu commands use:

- `AEGP_GetUniqueCommand`
- `AEGP_InsertMenuCommand`
- `AEGP_RegisterCommandHook`
- `AEGP_RegisterUpdateMenuHook`

The UpdateMenuHook matters: without it, AE may show the command disabled.
