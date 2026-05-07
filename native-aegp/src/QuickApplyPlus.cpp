// Minimal AEGP command plug-in scaffold for PersonalBar QuickApply+.
//
// This file intentionally follows the standard AEGP pattern:
// - get a unique command id;
// - insert a menu command;
// - register command/update hooks;
// - show a native popup when the command runs.
//
// Exact include paths/types can vary slightly between AE SDK versions. Treat
// this as the first compile target once the SDK is available locally.

#include "QuickApplyPopupWin.h"

#include <windows.h>

#include "AEConfig.h"
#include "entry.h"
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AE_EffectCBSuites.h"
#include "AE_GeneralPlug.h"
#include "AE_Macros.h"
#include "AEGP_SuiteHandler.h"
#include "SPBasic.h"

#include <filesystem>
#include <string>
#include <vector>

namespace {

constexpr A_char kPluginName[] = "PersonalBar QuickApply+";
constexpr A_char kMenuName[] = "PersonalBar QuickApply+";

AEGP_PluginID g_plugin_id = 0;
AEGP_Command g_quick_apply_cmd = 0;
SPBasicSuite* g_sp_basic = nullptr;

std::wstring Utf8ToWide(const char* value) {
    if (!value || !value[0]) {
        return L"";
    }

    int len = MultiByteToWideChar(CP_UTF8, 0, value, -1, nullptr, 0);
    if (len <= 1) {
        len = MultiByteToWideChar(CP_ACP, 0, value, -1, nullptr, 0);
        if (len <= 1) {
            return L"";
        }
        std::wstring out(static_cast<size_t>(len - 1), L'\0');
        MultiByteToWideChar(CP_ACP, 0, value, -1, &out[0], len);
        return out;
    }

    std::wstring out(static_cast<size_t>(len - 1), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, value, -1, &out[0], len);
    return out;
}

std::string WideToUtf8(const std::wstring& value) {
    if (value.empty()) {
        return "";
    }

    int len = WideCharToMultiByte(CP_UTF8, 0, value.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (len <= 1) {
        return "";
    }

    std::string out(static_cast<size_t>(len - 1), '\0');
    WideCharToMultiByte(CP_UTF8, 0, value.c_str(), -1, &out[0], len, nullptr, nullptr);
    return out;
}

std::string EscapeForJsString(const std::string& value) {
    std::string out;
    out.reserve(value.size() + 8);
    for (char ch : value) {
        switch (ch) {
            case '\\': out += "\\\\"; break;
            case '"': out += "\\\""; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            default: out += ch; break;
        }
    }
    return out;
}

std::string MemHandleToString(AEGP_SuiteHandler& suites, AEGP_MemHandle handle) {
    if (!handle) {
        return "";
    }

    AEGP_MemSize size = 0;
    void* ptr = nullptr;
    std::string out;
    if (!suites.MemorySuite1()->AEGP_GetMemHandleSize(handle, &size) &&
        !suites.MemorySuite1()->AEGP_LockMemHandle(handle, &ptr) &&
        ptr && size > 0) {
        const char* text = reinterpret_cast<const char*>(ptr);
        out.assign(text, text + size);
        while (!out.empty() && out.back() == '\0') {
            out.pop_back();
        }
        suites.MemorySuite1()->AEGP_UnlockMemHandle(handle);
    }
    suites.MemorySuite1()->AEGP_FreeMemHandle(handle);
    return out;
}

std::wstring GetEnvPath(const wchar_t* name) {
    wchar_t buffer[MAX_PATH] = {};
    DWORD len = GetEnvironmentVariableW(name, buffer, MAX_PATH);
    if (!len || len >= MAX_PATH) {
        return L"";
    }
    return buffer;
}

std::pair<std::wstring, std::wstring> SplitCustomPayload(const std::wstring& payload) {
    size_t pos = payload.find(L'\n');
    if (pos == std::wstring::npos) {
        return {payload, L""};
    }
    return {payload.substr(0, pos), payload.substr(pos + 1)};
}

std::wstring PresetDisplayPath(const std::filesystem::path& root, const std::filesystem::path& file) {
    std::error_code ec;
    std::filesystem::path rel = std::filesystem::relative(file, root, ec);
    if (ec) {
        rel = file.filename();
    }
    rel.replace_extension();

    std::wstring out = L"Preset";
    for (const auto& part : rel) {
        out += L" > ";
        out += part.wstring();
    }
    return out;
}

void AddPresetItems(std::vector<PBQA_ResultItem>& items, const std::filesystem::path& root) {
    std::error_code ec;
    if (root.empty() || !std::filesystem::exists(root, ec)) {
        return;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(root, std::filesystem::directory_options::skip_permission_denied, ec)) {
        if (ec || !entry.is_regular_file(ec)) {
            continue;
        }
        const auto path = entry.path();
        if (_wcsicmp(path.extension().c_str(), L".ffx") != 0) {
            continue;
        }

        PBQA_ResultItem item;
        item.name = path.stem().wstring();
        item.detail = PresetDisplayPath(root, path);
        item.kind = L"Preset";
        item.action_label = L"Run";
        item.payload = path.wstring();
        items.push_back(item);
    }
}

void AddMenuCommand(std::vector<PBQA_ResultItem>& items, const wchar_t* name, const wchar_t* path) {
    PBQA_ResultItem item;
    item.name = name;
    item.detail = path;
    item.kind = L"Command";
    item.action_label = L"Run";
    item.payload = name;
    items.push_back(item);
}

std::vector<PBQA_ResultItem> BuildItems() {
    std::vector<PBQA_ResultItem> items;

    try {
        AEGP_SuiteHandler suites(g_sp_basic);
        A_long count = 0;
        if (!suites.EffectSuite5()->AEGP_GetNumInstalledEffects(&count)) {
            AEGP_InstalledEffectKey key = AEGP_InstalledEffectKey_NONE;
            for (A_long i = 0; i < count; ++i) {
                AEGP_InstalledEffectKey next = AEGP_InstalledEffectKey_NONE;
                if (suites.EffectSuite5()->AEGP_GetNextInstalledEffect(key, &next) || next == AEGP_InstalledEffectKey_NONE) {
                    break;
                }
                key = next;

                A_char name[AEGP_MAX_EFFECT_NAME_SIZE] = {};
                A_char match_name[AEGP_MAX_EFFECT_MATCH_NAME_SIZE] = {};
                A_char category[AEGP_MAX_EFFECT_CATEGORY_NAME_SIZE] = {};
                suites.EffectSuite5()->AEGP_GetEffectName(key, name);
                suites.EffectSuite5()->AEGP_GetEffectMatchName(key, match_name);
                suites.EffectSuite5()->AEGP_GetEffectCategory(key, category);

                PBQA_ResultItem item;
                item.name = Utf8ToWide(name);
                std::wstring category_w = Utf8ToWide(category);
                item.detail = category_w.empty() ? L"Effect" : L"Effect > " + category_w;
                item.kind = L"Effect";
                item.action_label = L"Run";
                item.payload = Utf8ToWide(match_name);
                item.effect_key = key;
                if (!item.name.empty()) {
                    items.push_back(item);
                }
            }
        }
    } catch (...) {
    }

    AddPresetItems(items, L"C:\\Program Files\\Adobe\\Adobe After Effects 2026\\Support Files\\Presets");

    std::wstring user_profile = GetEnvPath(L"USERPROFILE");
    if (!user_profile.empty()) {
        AddPresetItems(items, std::filesystem::path(user_profile) / L"Documents\\Adobe\\After Effects 2026\\User Presets");
    }

    AddMenuCommand(items, L"Undo", L"Edit > Undo");
    AddMenuCommand(items, L"Redo", L"Edit > Redo");
    AddMenuCommand(items, L"Duplicate", L"Edit > Duplicate");
    AddMenuCommand(items, L"Split Layer", L"Edit > Split Layer");
    AddMenuCommand(items, L"Pre-compose...", L"Layer > Pre-compose...");
    AddMenuCommand(items, L"Solid Settings...", L"Layer > Solid Settings...");
    AddMenuCommand(items, L"New Composition...", L"Composition > New Composition...");
    AddMenuCommand(items, L"Composition Settings...", L"Composition > Composition Settings...");
    AddMenuCommand(items, L"Add to Render Queue", L"Composition > Add to Render Queue");
    AddMenuCommand(items, L"Add to Adobe Media Encoder Queue", L"Composition > Add to Adobe Media Encoder Queue");
    AddMenuCommand(items, L"Reveal Layer Source in Project", L"Layer > Reveal > Reveal Layer Source in Project");
    AddMenuCommand(items, L"Reveal in Explorer", L"File > Reveal in Explorer");
    AddMenuCommand(items, L"Purge All Memory & Disk Cache...", L"Edit > Purge > All Memory & Disk Cache...");

    return items;
}

void ExecuteScript(const std::string& script) {
    try {
        AEGP_SuiteHandler suites(g_sp_basic);
        A_Boolean available = FALSE;
        if (!suites.UtilitySuite6()->AEGP_IsScriptingAvailable(&available) || available) {
            AEGP_MemHandle result = nullptr;
            AEGP_MemHandle error = nullptr;
            A_Err err = suites.UtilitySuite6()->AEGP_ExecuteScript(g_plugin_id, script.c_str(), FALSE, &result, &error);
            std::string error_text = MemHandleToString(suites, error);
            if (result) {
                suites.MemorySuite1()->AEGP_FreeMemHandle(result);
            }
            if (err || !error_text.empty()) {
                std::wstring message = Utf8ToWide(error_text.empty() ? "AEGP_ExecuteScript failed." : error_text.c_str());
                MessageBoxW(nullptr, message.c_str(), L"Quick Apply Script Error", MB_OK | MB_ICONERROR);
            }
        }
    } catch (...) {
        MessageBoxW(nullptr, L"Unable to execute script.", L"Quick Apply Script Error", MB_OK | MB_ICONERROR);
    }
}

void ApplyEffectToSelection(AEGP_InstalledEffectKey effect_key) {
    if (effect_key == AEGP_InstalledEffectKey_NONE) {
        return;
    }

    try {
        A_Err err = A_Err_NONE;
        A_Err err2 = A_Err_NONE;
        AEGP_SuiteHandler suites(g_sp_basic);
        AEGP_ItemH active_item = nullptr;
        AEGP_ItemType item_type = AEGP_ItemType_NONE;
        AEGP_CompH comp = nullptr;
        AEGP_Collection2H selection = nullptr;

        ERR(suites.ItemSuite6()->AEGP_GetActiveItem(&active_item));
        if (!err && active_item) {
            ERR(suites.ItemSuite6()->AEGP_GetItemType(active_item, &item_type));
        }
        if (!err && item_type == AEGP_ItemType_COMP) {
            ERR(suites.CompSuite4()->AEGP_GetCompFromItem(active_item, &comp));
        }
        if (err || !comp) {
            MessageBoxW(nullptr, L"Open or select a composition first.", L"Quick Apply", MB_OK | MB_ICONWARNING);
            return;
        }

        ERR(suites.CompSuite12()->AEGP_GetNewCollectionFromCompSelection(g_plugin_id, comp, &selection));
        if (err || !selection) {
            MessageBoxW(nullptr, L"Select one or more layers first.", L"Quick Apply", MB_OK | MB_ICONWARNING);
            return;
        }

        A_u_long count = 0;
        ERR(suites.CollectionSuite2()->AEGP_GetCollectionNumItems(selection, &count));
        if (!err && count == 0) {
            suites.CollectionSuite2()->AEGP_DisposeCollection(selection);
            MessageBoxW(nullptr, L"Select one or more layers first.", L"Quick Apply", MB_OK | MB_ICONWARNING);
            return;
        }

        ERR(suites.UtilitySuite3()->AEGP_StartUndoGroup("PersonalBar QuickApply+"));
        for (A_u_long i = 0; !err && i < count; ++i) {
            AEGP_CollectionItemV2 collection_item = {};
            ERR(suites.CollectionSuite2()->AEGP_GetCollectionItemByIndex(selection, i, &collection_item));
            if (!err && collection_item.type == AEGP_CollectionItemType_LAYER && collection_item.u.layer.layerH) {
                AEGP_EffectRefH effect_ref = nullptr;
                ERR(suites.EffectSuite5()->AEGP_ApplyEffect(g_plugin_id, collection_item.u.layer.layerH, effect_key, &effect_ref));
                if (!err && effect_ref) {
                    ERR(suites.EffectSuite5()->AEGP_DisposeEffect(effect_ref));
                }
            }
        }
        ERR2(suites.UtilitySuite3()->AEGP_EndUndoGroup());
        ERR2(suites.CollectionSuite2()->AEGP_DisposeCollection(selection));

        if (err) {
            MessageBoxW(nullptr, L"Could not apply the selected effect.", L"Quick Apply", MB_OK | MB_ICONERROR);
        }
    } catch (...) {
        MessageBoxW(nullptr, L"Could not apply the selected effect.", L"Quick Apply", MB_OK | MB_ICONERROR);
    }
}

void ApplyResult(const PBQA_ResultItem& item) {
    if (item.kind == L"Effect") {
        std::string match_name = EscapeForJsString(WideToUtf8(item.payload));
        std::string display_name = EscapeForJsString(WideToUtf8(item.name));
        ExecuteScript(
            "(function(){"
            "try{"
            "var c=app.project.activeItem;"
            "if(!(c instanceof CompItem)){alert('Open or select a composition first.');return;}"
            "app.beginUndoGroup('PersonalBar QuickApply+ Effect');"
            "var layers=c.selectedLayers;"
            "if(layers.length===0){"
            "var solid=c.layers.addSolid([1,1,1],'" + display_name + " Solid',c.width,c.height,c.pixelAspect,c.duration);"
            "for(var s=1;s<=c.numLayers;s++){c.layer(s).selected=false;}"
            "solid.selected=true;"
            "layers=[solid];"
            "}"
            "var id=app.findMenuCommandId('" + display_name + "');"
            "if(id){app.executeCommand(id);}"
            "else{for(var i=0;i<layers.length;i++){layers[i].property('ADBE Effect Parade').addProperty('" + match_name + "');}}"
            "app.endUndoGroup();"
            "}catch(e){try{app.endUndoGroup();}catch(_e){} alert('Quick Apply effect error: '+e.toString());}"
            "})();");
    } else if (item.kind == L"Preset") {
        std::string path = EscapeForJsString(WideToUtf8(item.payload));
        ExecuteScript(
            "(function(){"
            "try{"
            "var f=File(\"" + path + "\");"
            "if(!f.exists){alert('Preset file not found: '+f.fsName);return;}"
            "var c=app.project.activeItem;"
            "if(!(c instanceof CompItem)){alert('Open or select a composition first.');return;}"
            "app.beginUndoGroup('PersonalBar QuickApply+ Preset');"
            "var layers=c.selectedLayers;"
            "if(layers.length===0){"
            "var solid=c.layers.addSolid([1,1,1],'Preset Solid',c.width,c.height,c.pixelAspect,c.duration);"
            "for(var s=1;s<=c.numLayers;s++){c.layer(s).selected=false;}"
            "solid.selected=true;"
            "layers=[solid];"
            "}"
            "for(var i=0;i<layers.length;i++){layers[i].applyPreset(f);}"
            "app.endUndoGroup();"
            "}catch(e){try{app.endUndoGroup();}catch(_e){} alert('Quick Apply preset error: '+e.toString());}"
            "})();");
    } else if (item.kind == L"Command") {
        std::string command = EscapeForJsString(WideToUtf8(item.payload));
        ExecuteScript(
            "(function(){"
            "try{"
            "var id=app.findMenuCommandId(\"" + command + "\");"
            "if(id){app.executeCommand(id);}else{alert('Command not found: " + command + "');}"
            "}catch(e){alert('Quick Apply command error: '+e.toString());}"
            "})();");
    } else if (item.kind == L"Tool" && item.payload == L"orbitCamera") {
        ExecuteScript(
            "(function(){"
            "var c=app.project.activeItem;"
            "if(!(c instanceof CompItem)){alert('Open or select a composition first.');return;}"
            "app.beginUndoGroup('PersonalBar Orbit Camera');"
            "var selected=c.selectedLayers;"
            "var ip=0,op=c.duration;"
            "if(selected.length>0){ip=selected[0].inPoint;op=selected[0].outPoint;}"
            "var center=[c.width/2,c.height/2,0];"
            "function setp(layer,names,value){for(var i=0;i<names.length;i++){var p=layer.property(names[i]);if(p){try{p.setValue(value);return true;}catch(e){}}}return false;}"
            "function settp(layer,names,value){var t=layer.property('ADBE Transform Group')||layer.property('Transform');if(!t){return false;}for(var i=0;i<names.length;i++){var p=t.property(names[i]);if(p){try{p.setValue(value);return true;}catch(e){}}}return false;}"
            "var n=c.layers.addNull();"
            "n.threeDLayer=true;"
            "settp(n,['ADBE Position','Position'],center);"
            "var cam=c.layers.addCamera('Camera 1',[c.width/2,c.height/2]);"
            "settp(cam,['ADBE Position','Position'],[c.width/2,c.height/2,-2666]);"
            "settp(cam,['ADBE Point of Interest','Point of Interest'],center);"
            "try{cam.parent=n;}catch(e){}"
            "n.inPoint=ip;n.outPoint=op;cam.inPoint=ip;cam.outPoint=op;"
            "app.endUndoGroup();"
            "})();");
    } else if (item.kind == L"Tool" && item.payload == L"revealSource") {
        ExecuteScript(
            "(function(){"
            "try{"
            "var c=app.project.activeItem;"
            "if(!(c instanceof CompItem)||c.selectedLayers.length===0){alert('Select a layer first.');return;}"
            "var src=c.selectedLayers[0].source;"
            "if(!src){alert('Selected layer has no project source.');return;}"
            "for(var i=1;i<=app.project.numItems;i++){app.project.item(i).selected=false;}"
            "src.selected=true;"
            "}catch(e){alert('Reveal Source error: '+e.toString());}"
            "})();");
    } else if (item.kind == L"Custom") {
        auto [action_w, value_w] = SplitCustomPayload(item.payload);
        std::string action = EscapeForJsString(WideToUtf8(action_w));
        std::string value_raw = WideToUtf8(value_w);
        std::string value = EscapeForJsString(WideToUtf8(value_w));
        if (action == "menu") {
            ExecuteScript(
                "(function(){try{"
                "var id=app.findMenuCommandId(\"" + value + "\");"
                "if(id){app.executeCommand(id);}else{alert('Command not found: " + value + "');}"
                "}catch(e){alert('Button menu error: '+e.toString());}})();");
        } else if (action == "expression") {
            ExecuteScript(
                "(function(){try{"
                "var c=app.project.activeItem;"
                "if(!(c instanceof CompItem)){alert('Open or select a composition first.');return;}"
                "var props=c.selectedProperties;"
                "if(!props||props.length===0){alert('Select one or more properties first.');return;}"
                "app.beginUndoGroup('PersonalBar Expression');"
                "for(var i=0;i<props.length;i++){if(props[i].canSetExpression){props[i].expression=\"" + value + "\";}}"
                "app.endUndoGroup();"
                "}catch(e){try{app.endUndoGroup();}catch(_e){} alert('Button expression error: '+e.toString());}})();");
        } else if (action == "script") {
            ExecuteScript(
                "(function(){try{\n"
                + value_raw +
                "}catch(e){alert('Button script error: '+e.toString());}})();");
        } else if (action == "preset") {
            ExecuteScript(
                "(function(){try{"
                "var f=File(\"" + value + "\");"
                "if(!f.exists){alert('Preset file not found: '+f.fsName);return;}"
                "var c=app.project.activeItem;"
                "if(!(c instanceof CompItem)){alert('Open or select a composition first.');return;}"
                "app.beginUndoGroup('PersonalBar Preset');"
                "var layers=c.selectedLayers;"
                "if(layers.length===0){"
                "var solid=c.layers.addSolid([1,1,1],'Preset Solid',c.width,c.height,c.pixelAspect,c.duration);"
                "for(var s=1;s<=c.numLayers;s++){c.layer(s).selected=false;}"
                "solid.selected=true;"
                "layers=[solid];"
                "}"
                "for(var i=0;i<layers.length;i++){layers[i].applyPreset(f);}"
                "app.endUndoGroup();"
                "}catch(e){try{app.endUndoGroup();}catch(_e){} alert('Button preset error: '+e.toString());}})();");
        } else if (action == "scriptfile") {
            ExecuteScript(
                "(function(){try{"
                "var f=File(\"" + value + "\");"
                "if(!f.exists){alert('Script file not found: '+f.fsName);return;}"
                "$.evalFile(f);"
                "}catch(e){alert('Button script file error: '+e.toString());}})();");
        } else if (action == "scripturl") {
            ExecuteScript(
                "(function(){try{"
                "var url=\"" + value + "\";"
                "var dir=Folder.userData.fsName + '/PersonalBarQuickApply';"
                "var folder=Folder(dir); if(!folder.exists){folder.create();}"
                "var f=File(dir + '/remote-button.jsx');"
                "var cmd='powershell -NoProfile -ExecutionPolicy Bypass -Command \"Invoke-WebRequest -UseBasicParsing -Uri ' + url + ' -OutFile ' + f.fsName + '\"';"
                "system.callSystem(cmd);"
                "if(!f.exists){alert('Could not download script URL.');return;}"
                "$.evalFile(f);"
                "}catch(e){alert('Button script URL error: '+e.toString());}})();");
        } else {
            MessageBoxW(nullptr, item.name.c_str(), L"Unsupported button action", MB_OK | MB_ICONWARNING);
        }
    } else {
        MessageBoxW(nullptr, item.name.c_str(), L"Tool action placeholder", MB_OK | MB_ICONINFORMATION);
    }
}

static A_Err CommandHook(
    AEGP_GlobalRefcon,
    AEGP_CommandRefcon,
    AEGP_Command command,
    AEGP_HookPriority,
    A_Boolean already_handledB,
    A_Boolean* handledPB) {

    if (command == g_quick_apply_cmd && !already_handledB) {
        PBQA_SetItems(BuildItems());
        PBQA_SetApplyCallback(ApplyResult);
        PBQA_ShowPopupWindow();
        if (handledPB) {
            *handledPB = TRUE;
        }
    }

    return A_Err_NONE;
}

static A_Err UpdateMenuHook(
    AEGP_GlobalRefcon,
    AEGP_UpdateMenuRefcon,
    AEGP_WindowType) {

    A_Err err = A_Err_NONE;
    AEGP_SuiteHandler suites(g_sp_basic);

    if (g_quick_apply_cmd) {
        ERR(suites.CommandSuite1()->AEGP_EnableCommand(g_quick_apply_cmd));
    }

    return err;
}

} // namespace

extern "C" DllExport A_Err
EntryPointFunc(
    struct SPBasicSuite* sp_basicP,
    A_long major_versionL,
    A_long minor_versionL,
    AEGP_PluginID aegp_plugin_id,
    AEGP_GlobalRefcon* global_refconP) {

    (void)major_versionL;
    (void)minor_versionL;
    (void)global_refconP;

    A_Err err = A_Err_NONE;
    g_sp_basic = sp_basicP;
    g_plugin_id = aegp_plugin_id;

    AEGP_SuiteHandler suites(sp_basicP);

    ERR(suites.CommandSuite1()->AEGP_GetUniqueCommand(&g_quick_apply_cmd));

    if (!err) {
        ERR(suites.CommandSuite1()->AEGP_InsertMenuCommand(
            g_quick_apply_cmd,
            kMenuName,
            AEGP_Menu_EDIT,
            AEGP_MENU_INSERT_AT_BOTTOM));
    }

    if (!err) {
        ERR(suites.RegisterSuite5()->AEGP_RegisterCommandHook(
            g_plugin_id,
            AEGP_HP_BeforeAE,
            AEGP_Command_ALL,
            CommandHook,
            nullptr));
    }

    if (!err) {
        ERR(suites.RegisterSuite5()->AEGP_RegisterUpdateMenuHook(
            g_plugin_id,
            UpdateMenuHook,
            nullptr));
    }

    return err;
}
