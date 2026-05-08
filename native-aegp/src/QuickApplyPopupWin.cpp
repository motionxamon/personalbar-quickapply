#include "QuickApplyPopupWin.h"

#include <windows.h>
#include <windowsx.h>
#include <algorithm>
#include <cmath>
#include <cwctype>
#include <sstream>
#include <string>
#include <vector>

#ifndef EM_SETCUEBANNER
#define EM_SETCUEBANNER 0x1501
#endif

namespace {

constexpr wchar_t kWindowClass[] = L"PersonalBarQuickApplyPopup";
constexpr wchar_t kEditSubclassProp[] = L"PBQA_EditSubclass";

enum ManagerControlId {
    IDC_BTN_LIST = 2001,
    IDC_NAME_EDIT,
    IDC_TIP_EDIT,
    IDC_ICON_EDIT,
    IDC_COLOR_EDIT,
    IDC_ACTION_EDIT,
    IDC_VALUE_EDIT,
    IDC_NEW_BUTTON,
    IDC_SAVE_BUTTON,
    IDC_DELETE_BUTTON,
    IDC_UP_BUTTON,
    IDC_DOWN_BUTTON,
    IDC_SIZE_MINUS,
    IDC_SIZE_PLUS,
    IDC_SPACE_MINUS,
    IDC_SPACE_PLUS,
    IDC_ICON_PICK_BUTTON,
    IDC_COLOR_PICK_BUTTON
};

std::vector<PBQA_ResultItem> g_items;
std::vector<size_t> g_filtered;
PBQA_ApplyCallback g_apply_callback;

HWND g_hwnd = nullptr;
HWND g_edit = nullptr;
HWND g_results = nullptr;
HWND g_tools = nullptr;
HWND g_buttons = nullptr;
HWND g_settings = nullptr;
HWND g_manager = nullptr;
HWND g_btn_list = nullptr;
HWND g_name_edit = nullptr;
HWND g_tip_edit = nullptr;
HWND g_icon_edit = nullptr;
HWND g_color_edit = nullptr;
HWND g_action_edit = nullptr;
HWND g_value_edit = nullptr;
HWND g_lbl_buttons = nullptr;
HWND g_lbl_name = nullptr;
HWND g_lbl_tip = nullptr;
HWND g_lbl_icon = nullptr;
HWND g_lbl_color = nullptr;
HWND g_lbl_action = nullptr;
HWND g_lbl_value = nullptr;
HWND g_lbl_help_actions = nullptr;
HWND g_lbl_help_icons = nullptr;
HWND g_lbl_size = nullptr;
HWND g_lbl_spacing = nullptr;
int g_manager_tab = 0;
int g_selected = 0;
HFONT g_font = nullptr;
HFONT g_small_font = nullptr;
WNDPROC g_old_edit_proc = nullptr;
bool g_include_effects = true;
bool g_include_presets = true;
bool g_include_commands = true;
bool g_keep_at_cursor = false;
int g_button_size = 138;
int g_button_spacing = 8;

struct CustomButton {
    std::wstring name;
    std::wstring tooltip;
    std::wstring icon;
    std::wstring color;
    std::wstring action;
    std::wstring value;
};

std::vector<CustomButton> g_custom_buttons;

COLORREF kBg = RGB(18, 18, 18);
COLORREF kPanel = RGB(28, 28, 28);
COLORREF kInput = RGB(8, 8, 8);
COLORREF kLine = RGB(55, 55, 55);
COLORREF kText = RGB(238, 238, 238);
COLORREF kMuted = RGB(130, 130, 130);
COLORREF kBlue = RGB(13, 120, 232);

std::wstring SettingsPath() {
    wchar_t appdata[MAX_PATH] = {};
    DWORD len = GetEnvironmentVariableW(L"APPDATA", appdata, MAX_PATH);
    if (!len || len >= MAX_PATH) {
        return L".\\PersonalBarQuickApply.ini";
    }
    return std::wstring(appdata) + L"\\PersonalBarQuickApply.ini";
}

void LoadSettings() {
    std::wstring path = SettingsPath();
    g_include_effects = GetPrivateProfileIntW(L"Search", L"Effects", 1, path.c_str()) != 0;
    g_include_presets = GetPrivateProfileIntW(L"Search", L"Presets", 1, path.c_str()) != 0;
    g_include_commands = GetPrivateProfileIntW(L"Search", L"Commands", 1, path.c_str()) != 0;
    g_keep_at_cursor = GetPrivateProfileIntW(L"Search", L"AtCursor", 0, path.c_str()) != 0;
    g_button_size = GetPrivateProfileIntW(L"Toolbar", L"ButtonSize", 138, path.c_str());
    g_button_spacing = GetPrivateProfileIntW(L"Toolbar", L"Spacing", 8, path.c_str());
    if (g_button_size < 72) {
        g_button_size = 72;
    }
    if (g_button_size > 260) {
        g_button_size = 260;
    }
    if (g_button_spacing < 0) {
        g_button_spacing = 0;
    }
    if (g_button_spacing > 32) {
        g_button_spacing = 32;
    }
}

std::wstring FavoriteKey(const PBQA_ResultItem& item) {
    std::wstring value = item.kind + L"|" + (item.payload.empty() ? item.name : item.payload);
    unsigned long long hash = 1469598103934665603ULL;
    for (wchar_t ch : value) {
        hash ^= static_cast<unsigned long long>(ch);
        hash *= 1099511628211ULL;
    }

    std::wstringstream ss;
    ss << L"F" << std::hex << hash;
    return ss.str();
}

void MarkFavorites() {
    std::wstring path = SettingsPath();
    for (auto& item : g_items) {
        item.favorite = GetPrivateProfileIntW(L"Favorites", FavoriteKey(item).c_str(), 0, path.c_str()) != 0;
    }
}

void SaveFavorite(const PBQA_ResultItem& item) {
    std::wstring path = SettingsPath();
    WritePrivateProfileStringW(L"Favorites", FavoriteKey(item).c_str(), item.favorite ? L"1" : L"0", path.c_str());
}

void SaveSettings() {
    std::wstring path = SettingsPath();
    WritePrivateProfileStringW(L"Search", L"Effects", g_include_effects ? L"1" : L"0", path.c_str());
    WritePrivateProfileStringW(L"Search", L"Presets", g_include_presets ? L"1" : L"0", path.c_str());
    WritePrivateProfileStringW(L"Search", L"Commands", g_include_commands ? L"1" : L"0", path.c_str());
    WritePrivateProfileStringW(L"Search", L"AtCursor", g_keep_at_cursor ? L"1" : L"0", path.c_str());
    wchar_t num[32] = {};
    wsprintfW(num, L"%d", g_button_size);
    WritePrivateProfileStringW(L"Toolbar", L"ButtonSize", num, path.c_str());
    wsprintfW(num, L"%d", g_button_spacing);
    WritePrivateProfileStringW(L"Toolbar", L"Spacing", num, path.c_str());
}

std::wstring ReadIniString(const wchar_t* section, const wchar_t* key, const wchar_t* fallback = L"") {
    std::vector<wchar_t> buffer(262144);
    GetPrivateProfileStringW(section, key, fallback, buffer.data(), static_cast<DWORD>(buffer.size()), SettingsPath().c_str());
    std::wstring value = buffer.data();
    std::wstring out;
    out.reserve(value.size());
    for (size_t i = 0; i < value.size(); ++i) {
        if (value[i] == L'\\' && i + 1 < value.size()) {
            wchar_t next = value[++i];
            if (next == L'n') {
                out.push_back(L'\n');
            } else if (next == L'r') {
                out.push_back(L'\r');
            } else if (next == L't') {
                out.push_back(L'\t');
            } else {
                out.push_back(next);
            }
        } else {
            out.push_back(value[i]);
        }
    }
    return out;
}

void WriteIniString(const wchar_t* section, const wchar_t* key, const std::wstring& value) {
    std::wstring out;
    out.reserve(value.size() + 16);
    for (wchar_t ch : value) {
        if (ch == L'\\') {
            out += L"\\\\";
        } else if (ch == L'\n') {
            out += L"\\n";
        } else if (ch == L'\r') {
            out += L"\\r";
        } else if (ch == L'\t') {
            out += L"\\t";
        } else {
            out.push_back(ch);
        }
    }
    WritePrivateProfileStringW(section, key, out.c_str(), SettingsPath().c_str());
}

void LoadCustomButtons() {
    LoadSettings();
    g_custom_buttons.clear();
    int count = GetPrivateProfileIntW(L"Toolbar", L"Count", 0, SettingsPath().c_str());
    if (count < 0) {
        count = 0;
    }
    if (count > 64) {
        count = 64;
    }
    for (int i = 0; i < count; ++i) {
        wchar_t section[32] = {};
        wsprintfW(section, L"Button%d", i);
        CustomButton button;
        button.name = ReadIniString(section, L"Name");
        button.tooltip = ReadIniString(section, L"Tooltip");
        button.icon = ReadIniString(section, L"Icon", L"zap");
        button.color = ReadIniString(section, L"Color", L"rgba(255, 190, 80, 1)");
        button.action = ReadIniString(section, L"Action", L"menu");
        button.value = ReadIniString(section, L"Value");
        if (!button.name.empty()) {
            g_custom_buttons.push_back(button);
        }
    }
}

void SaveCustomButtons() {
    SaveSettings();
    wchar_t num[32] = {};
    wsprintfW(num, L"%d", static_cast<int>(g_custom_buttons.size()));
    WritePrivateProfileStringW(L"Toolbar", L"Count", num, SettingsPath().c_str());
    for (int i = 0; i < 64; ++i) {
        wchar_t section[32] = {};
        wsprintfW(section, L"Button%d", i);
        if (i >= static_cast<int>(g_custom_buttons.size())) {
            WritePrivateProfileStringW(section, nullptr, nullptr, SettingsPath().c_str());
            continue;
        }
        const CustomButton& button = g_custom_buttons[static_cast<size_t>(i)];
        WriteIniString(section, L"Name", button.name);
        WriteIniString(section, L"Tooltip", button.tooltip);
        WriteIniString(section, L"Icon", button.icon);
        WriteIniString(section, L"Color", button.color);
        WriteIniString(section, L"Action", button.action);
        WriteIniString(section, L"Value", button.value);
    }
}

bool KindEnabled(const std::wstring& kind) {
    if (kind == L"Effect") {
        return g_include_effects;
    }
    if (kind == L"Preset") {
        return g_include_presets;
    }
    if (kind == L"Command") {
        return g_include_commands;
    }
    return true;
}

std::wstring Lower(std::wstring value) {
    if (!value.empty()) {
        CharLowerBuffW(&value[0], static_cast<DWORD>(value.size()));
    }
    return value;
}

std::wstring GetEditText() {
    if (!g_edit) {
        return L"";
    }
    wchar_t buffer[512] = {};
    GetWindowTextW(g_edit, buffer, 512);
    return buffer;
}

bool SearchListVisible() {
    return !GetEditText().empty();
}

std::wstring FilterText(const std::wstring& query, std::wstring* forced_kind) {
    *forced_kind = L"";
    if (query.size() > 2 && query[1] == L':') {
        wchar_t prefix = static_cast<wchar_t>(towlower(query[0]));
        if (prefix == L'e') {
            *forced_kind = L"Effect";
            return query.substr(2);
        }
        if (prefix == L'p') {
            *forced_kind = L"Preset";
            return query.substr(2);
        }
        if (prefix == L'm' || prefix == L'c') {
            *forced_kind = L"Command";
            return query.substr(2);
        }
        if (prefix == L'k') {
            *forced_kind = L"Tool";
            return query.substr(2);
        }
    }
    return query;
}

void FilterItems() {
    std::wstring forced_kind;
    std::wstring q = Lower(FilterText(GetEditText(), &forced_kind));
    g_filtered.clear();

    for (size_t i = 0; i < g_items.size() && g_filtered.size() < 90; ++i) {
        const auto& item = g_items[i];
        if (!KindEnabled(item.kind)) {
            continue;
        }
        if (!forced_kind.empty() && item.kind != forced_kind) {
            continue;
        }

        std::wstring hay = Lower(item.name + L" " + item.detail + L" " + item.kind);
        if (q.empty() || hay.find(q) != std::wstring::npos) {
            g_filtered.push_back(i);
        }
    }

    std::stable_sort(g_filtered.begin(), g_filtered.end(), [](size_t a, size_t b) {
        return g_items[a].favorite && !g_items[b].favorite;
    });

    if (g_selected >= static_cast<int>(g_filtered.size())) {
        g_selected = static_cast<int>(g_filtered.size()) - 1;
    }
    if (g_selected < 0) {
        g_selected = 0;
    }

    if (g_results) {
        ShowWindow(g_results, SearchListVisible() ? SW_SHOW : SW_HIDE);
        InvalidateRect(g_results, nullptr, TRUE);
    }
    if (g_buttons) {
        InvalidateRect(g_buttons, nullptr, TRUE);
    }
}

void MoveSelection(int delta) {
    if (g_filtered.empty()) {
        return;
    }
    g_selected += delta;
    if (g_selected < 0) {
        g_selected = 0;
    }
    if (g_selected >= static_cast<int>(g_filtered.size())) {
        g_selected = static_cast<int>(g_filtered.size()) - 1;
    }
    InvalidateRect(g_results, nullptr, FALSE);
}

void ApplySelected() {
    if (g_filtered.empty() || g_selected < 0 || g_selected >= static_cast<int>(g_filtered.size())) {
        return;
    }

    PBQA_ResultItem item = g_items[g_filtered[g_selected]];
    ShowWindow(g_hwnd, SW_HIDE);
    if (g_apply_callback) {
        g_apply_callback(item);
    }
}

void HideSettings() {
    if (g_settings) {
        ShowWindow(g_settings, SW_HIDE);
    }
}

void ToggleSearchSettings() {
    if (!g_settings || !g_hwnd) {
        return;
    }
    if (IsWindowVisible(g_settings)) {
        ShowWindow(g_settings, SW_HIDE);
        return;
    }
    RECT window_rc = {};
    GetWindowRect(g_hwnd, &window_rc);
    SetWindowPos(g_settings, HWND_TOPMOST, window_rc.left + 13, window_rc.top + 55, 330, 174, SWP_SHOWWINDOW);
    ShowWindow(g_settings, SW_SHOWNORMAL);
    SetForegroundWindow(g_hwnd);
}

std::wstring WindowText(HWND hwnd) {
    int len = GetWindowTextLengthW(hwnd);
    if (len <= 0) {
        return L"";
    }
    std::wstring out(static_cast<size_t>(len), L'\0');
    GetWindowTextW(hwnd, &out[0], len + 1);
    return out;
}

void RefreshManagerList() {
    if (!g_btn_list) {
        return;
    }
    SendMessageW(g_btn_list, LB_RESETCONTENT, 0, 0);
    for (const auto& button : g_custom_buttons) {
        std::wstring label = button.name + L"    [" + button.action + L"]";
        SendMessageW(g_btn_list, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(label.c_str()));
    }
}

void ShowManagerTab(HWND hwnd, int tab) {
    g_manager_tab = tab;
    const int show_buttons = tab == 0 ? SW_SHOW : SW_HIDE;
    const int show_settings = tab == 1 ? SW_SHOW : SW_HIDE;

    HWND button_controls[] = {
        g_btn_list, g_lbl_buttons, g_lbl_name, g_name_edit, g_lbl_tip, g_tip_edit,
        g_lbl_icon, g_icon_edit, GetDlgItem(hwnd, IDC_ICON_PICK_BUTTON),
        g_lbl_color, g_color_edit, GetDlgItem(hwnd, IDC_COLOR_PICK_BUTTON),
        g_lbl_action, g_action_edit, g_lbl_value, g_value_edit,
        g_lbl_help_actions, g_lbl_help_icons,
        GetDlgItem(hwnd, IDC_NEW_BUTTON), GetDlgItem(hwnd, IDC_SAVE_BUTTON),
        GetDlgItem(hwnd, IDC_DELETE_BUTTON), GetDlgItem(hwnd, IDC_UP_BUTTON),
        GetDlgItem(hwnd, IDC_DOWN_BUTTON)
    };
    for (HWND control : button_controls) {
        if (control) ShowWindow(control, show_buttons);
    }

    HWND settings_controls[] = {
        g_lbl_size, GetDlgItem(hwnd, IDC_SIZE_MINUS), GetDlgItem(hwnd, IDC_SIZE_PLUS),
        g_lbl_spacing, GetDlgItem(hwnd, IDC_SPACE_MINUS), GetDlgItem(hwnd, IDC_SPACE_PLUS)
    };
    for (HWND control : settings_controls) {
        if (control) ShowWindow(control, show_settings);
    }
    if (GetDlgItem(hwnd, IDCANCEL)) {
        ShowWindow(GetDlgItem(hwnd, IDCANCEL), SW_SHOW);
    }
    InvalidateRect(hwnd, nullptr, TRUE);
}

int SelectedManagerIndex() {
    if (!g_btn_list) {
        return -1;
    }
    return static_cast<int>(SendMessageW(g_btn_list, LB_GETCURSEL, 0, 0));
}

void LoadManagerFields(int index) {
    if (index < 0 || index >= static_cast<int>(g_custom_buttons.size())) {
        SetWindowTextW(g_name_edit, L"");
        SetWindowTextW(g_tip_edit, L"");
        SetWindowTextW(g_icon_edit, L"zap");
        SetWindowTextW(g_color_edit, L"rgba(255, 190, 80, 1)");
        SetWindowTextW(g_action_edit, L"menu");
        SetWindowTextW(g_value_edit, L"");
        return;
    }
    const CustomButton& button = g_custom_buttons[static_cast<size_t>(index)];
    SetWindowTextW(g_name_edit, button.name.c_str());
    SetWindowTextW(g_tip_edit, button.tooltip.c_str());
    SetWindowTextW(g_icon_edit, button.icon.c_str());
    SetWindowTextW(g_color_edit, button.color.c_str());
    SetWindowTextW(g_action_edit, button.action.c_str());
    SetWindowTextW(g_value_edit, button.value.c_str());
}

void SaveManagerFields(int index) {
    CustomButton button;
    button.name = WindowText(g_name_edit);
    button.tooltip = WindowText(g_tip_edit);
    button.icon = WindowText(g_icon_edit);
    button.color = WindowText(g_color_edit);
    button.action = Lower(WindowText(g_action_edit));
    button.value = WindowText(g_value_edit);
    if (button.name.empty()) {
        button.name = L"Untitled";
    }
    if (button.icon.empty()) {
        button.icon = L"zap";
    }
    if (button.color.empty()) {
        button.color = L"rgba(255, 190, 80, 1)";
    }
    if (button.action.empty()) {
        button.action = L"menu";
    }

    if (index < 0 || index >= static_cast<int>(g_custom_buttons.size())) {
        g_custom_buttons.push_back(button);
        index = static_cast<int>(g_custom_buttons.size()) - 1;
    } else {
        g_custom_buttons[static_cast<size_t>(index)] = button;
    }
    SaveCustomButtons();
    RefreshManagerList();
    SendMessageW(g_btn_list, LB_SETCURSEL, index, 0);
    if (g_buttons) {
        InvalidateRect(g_buttons, nullptr, TRUE);
    }
}

void ShowManager() {
    if (!g_manager) {
        return;
    }
    LoadCustomButtons();
    RefreshManagerList();
    LoadManagerFields(SelectedManagerIndex());
    RECT work = {};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);
    int w = 940;
    int h = 720;
    int x = work.left + ((work.right - work.left) - w) / 2;
    int y = work.top + 130;
    SetWindowPos(g_manager, HWND_TOPMOST, x, y, w, h, SWP_SHOWWINDOW);
    ShowWindow(g_manager, SW_SHOWNORMAL);
    SetForegroundWindow(g_manager);
}

std::vector<size_t> ButtonItems() {
    std::vector<size_t> out;
    return out;
}

PBQA_ResultItem CustomToItem(const CustomButton& button) {
    PBQA_ResultItem item;
    item.name = button.name;
    item.detail = button.tooltip.empty() ? L"PersonalBar Button" : button.tooltip;
    item.action_label = L"Run";
    item.kind = L"Custom";
    item.payload = button.action + L"\n" + button.value;
    return item;
}

void ApplyButton(int index) {
    auto buttons = ButtonItems();
    int tool_count = static_cast<int>(buttons.size());
    if (index < 0) {
        return;
    }
    PBQA_ResultItem item;
    if (index < tool_count) {
        item = g_items[buttons[static_cast<size_t>(index)]];
    } else {
        int custom_index = index - tool_count;
        if (custom_index < 0 || custom_index >= static_cast<int>(g_custom_buttons.size())) {
            return;
        }
        item = CustomToItem(g_custom_buttons[static_cast<size_t>(custom_index)]);
    }
    ShowWindow(g_hwnd, SW_HIDE);
    if (g_apply_callback) {
        g_apply_callback(item);
    }
}

void PaintToolbarButton(HDC hdc, RECT rc, bool editor_button) {
    HBRUSH bg = CreateSolidBrush(RGB(35, 35, 35));
    FillRect(hdc, &rc, bg);
    DeleteObject(bg);

    HPEN pen = CreatePen(PS_SOLID, 1, RGB(65, 65, 65));
    HGDIOBJ old_pen = SelectObject(hdc, pen);
    HGDIOBJ old_brush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, 4, 4);
    SelectObject(hdc, old_brush);
    SelectObject(hdc, old_pen);
    DeleteObject(pen);

    HPEN gear_pen = CreatePen(PS_SOLID, 2, RGB(195, 195, 195));
    HGDIOBJ gear_old = SelectObject(hdc, gear_pen);
    int cx = (rc.left + rc.right) / 2;
    int cy = (rc.top + rc.bottom) / 2;
    if (editor_button) {
        MoveToEx(hdc, cx - 9, cy + 10, nullptr);
        LineTo(hdc, cx + 8, cy - 7);
        Ellipse(hdc, cx + 4, cy - 12, cx + 15, cy - 1);
        MoveToEx(hdc, cx - 12, cy + 7, nullptr);
        LineTo(hdc, cx - 6, cy + 13);
    } else {
        Ellipse(hdc, cx - 11, cy - 11, cx + 11, cy + 11);
        MoveToEx(hdc, cx - 15, cy, nullptr);
        LineTo(hdc, cx + 15, cy);
        MoveToEx(hdc, cx, cy - 15, nullptr);
        LineTo(hdc, cx, cy + 15);
        MoveToEx(hdc, cx - 5, cy - 5, nullptr);
        LineTo(hdc, cx + 5, cy + 5);
        MoveToEx(hdc, cx + 5, cy - 5, nullptr);
        LineTo(hdc, cx - 5, cy + 5);
    }
    SelectObject(hdc, gear_old);
    DeleteObject(gear_pen);
}

LRESULT CALLBACK ToolsProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (msg == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);
        HBRUSH bg = CreateSolidBrush(kBg);
        FillRect(hdc, &rc, bg);
        DeleteObject(bg);
        RECT filters = {0, 0, 46, 38};
        RECT editor = {52, 0, 98, 38};
        PaintToolbarButton(hdc, filters, false);
        PaintToolbarButton(hdc, editor, true);
        EndPaint(hwnd, &ps);
        return 0;
    }
    if (msg == WM_LBUTTONDOWN) {
        int x = GET_X_LPARAM(lparam);
        if (x < 50) {
            ToggleSearchSettings();
        } else {
            HideSettings();
            ShowManager();
        }
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wparam, lparam);
}

const wchar_t* KindIcon(const PBQA_ResultItem& item) {
    if (item.kind == L"Effect") {
        return L"AE";
    }
    if (item.kind == L"Preset") {
        return L"ffx";
    }
    if (item.kind == L"Command") {
        return L"|||";
    }
    if (item.kind == L"Custom") {
        return L"*";
    }
    return L"\u25c6";
}

COLORREF KindAccent(const PBQA_ResultItem& item) {
    if (item.kind == L"Effect") {
        return RGB(66, 160, 255);
    }
    if (item.kind == L"Preset") {
        return RGB(220, 220, 235);
    }
    if (item.kind == L"Command") {
        return RGB(170, 170, 170);
    }
    if (item.kind == L"Custom") {
        return RGB(255, 190, 80);
    }
    return RGB(75, 235, 205);
}

COLORREF ParseButtonColor(const std::wstring& value);

std::wstring ButtonIcon(const CustomButton& button) {
    if (button.icon.empty()) return L"*";
    return button.icon.substr(0, std::min<size_t>(4, button.icon.size()));
}

bool DrawNamedIcon(HDC hdc, RECT rc, const std::wstring& icon, COLORREF color) {
    std::wstring name = Lower(icon);
    HPEN pen = CreatePen(PS_SOLID, 2, color);
    HBRUSH brush = CreateSolidBrush(color);
    HGDIOBJ old_pen = SelectObject(hdc, pen);
    HGDIOBJ old_brush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
    int cx = (rc.left + rc.right) / 2;
    int cy = (rc.top + rc.bottom) / 2;
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;

    auto finish = [&]() {
        SelectObject(hdc, old_brush);
        SelectObject(hdc, old_pen);
        DeleteObject(brush);
        DeleteObject(pen);
        return true;
    };

    if (name == L"camera" || name == L"video" || name == L"videocam" || name == L"photo_camera") {
        Rectangle(hdc, rc.left + w / 6, cy - h / 5, rc.left + w * 2 / 3, cy + h / 5);
        MoveToEx(hdc, rc.left + w * 2 / 3, cy - h / 8, nullptr);
        LineTo(hdc, rc.right - w / 8, cy - h / 3);
        LineTo(hdc, rc.right - w / 8, cy + h / 3);
        LineTo(hdc, rc.left + w * 2 / 3, cy + h / 8);
        return finish();
    }
    if (name == L"search" || name == L"zoom") {
        Ellipse(hdc, rc.left + w / 6, rc.top + h / 6, rc.left + w * 2 / 3, rc.top + h * 2 / 3);
        MoveToEx(hdc, rc.left + w * 2 / 3 - 2, rc.top + h * 2 / 3 - 2, nullptr);
        LineTo(hdc, rc.right - w / 8, rc.bottom - h / 8);
        return finish();
    }
    if (name == L"scissors" || name == L"cut" || name == L"content_cut") {
        Ellipse(hdc, rc.left + 2, cy - 8, rc.left + 12, cy + 2);
        Ellipse(hdc, rc.left + 2, cy + 4, rc.left + 12, cy + 14);
        MoveToEx(hdc, rc.left + 12, cy, nullptr); LineTo(hdc, rc.right - 3, rc.top + 4);
        MoveToEx(hdc, rc.left + 12, cy + 6, nullptr); LineTo(hdc, rc.right - 3, rc.bottom - 4);
        return finish();
    }
    if (name == L"trash" || name == L"delete") {
        Rectangle(hdc, rc.left + w / 4, rc.top + h / 3, rc.right - w / 4, rc.bottom - h / 8);
        MoveToEx(hdc, rc.left + w / 5, rc.top + h / 4, nullptr); LineTo(hdc, rc.right - w / 5, rc.top + h / 4);
        MoveToEx(hdc, cx - 4, rc.top + h / 6, nullptr); LineTo(hdc, cx + 4, rc.top + h / 6);
        return finish();
    }
    if (name == L"refresh" || name == L"refresh-cw" || name == L"repeat") {
        Arc(hdc, rc.left + 4, rc.top + 4, rc.right - 4, rc.bottom - 4, rc.right - 7, cy, cx, rc.top + 4);
        MoveToEx(hdc, rc.right - 9, cy - 8, nullptr); LineTo(hdc, rc.right - 4, cy); LineTo(hdc, rc.right - 14, cy);
        return finish();
    }
    if (name == L"film") {
        Rectangle(hdc, rc.left + 4, rc.top + 3, rc.right - 4, rc.bottom - 3);
        for (int y = rc.top + 6; y < rc.bottom - 5; y += 7) {
            Rectangle(hdc, rc.left + 7, y, rc.left + 10, y + 3);
            Rectangle(hdc, rc.right - 10, y, rc.right - 7, y + 3);
        }
        return finish();
    }
    if (name == L"printer" || name == L"print") {
        Rectangle(hdc, rc.left + 5, cy - 3, rc.right - 5, cy + 9);
        Rectangle(hdc, rc.left + 9, rc.top + 5, rc.right - 9, cy - 3);
        Rectangle(hdc, rc.left + 10, cy + 6, rc.right - 10, rc.bottom - 4);
        return finish();
    }
    if (name == L"grid" || name == L"grid-3x3" || name == L"apps") {
        HGDIOBJ old_fill = SelectObject(hdc, brush);
        for (int yy = 0; yy < 3; ++yy) {
            for (int xx = 0; xx < 3; ++xx) {
                Rectangle(hdc, rc.left + 4 + xx * 9, rc.top + 4 + yy * 9, rc.left + 10 + xx * 9, rc.top + 10 + yy * 9);
            }
        }
        SelectObject(hdc, old_fill);
        return finish();
    }
    if (name == L"box" || name == L"layers") {
        Rectangle(hdc, rc.left + 9, rc.top + 6, rc.right - 4, rc.bottom - 9);
        Rectangle(hdc, rc.left + 4, rc.top + 11, rc.right - 9, rc.bottom - 4);
        return finish();
    }
    if (name == L"play" || name == L"play_arrow") {
        HGDIOBJ old_fill = SelectObject(hdc, brush);
        POINT pts[3] = {{rc.left + 8, rc.top + 5}, {rc.left + 8, rc.bottom - 5}, {rc.right - 5, cy}};
        Polygon(hdc, pts, 3);
        SelectObject(hdc, old_fill);
        return finish();
    }
    if (name == L"zap" || name == L"bolt") {
        HGDIOBJ old_fill = SelectObject(hdc, brush);
        POINT pts[6] = {{cx, rc.top + 2}, {rc.left + 8, cy + 1}, {cx - 1, cy + 1}, {cx - 4, rc.bottom - 2}, {rc.right - 7, cy - 2}, {cx + 1, cy - 2}};
        Polygon(hdc, pts, 6);
        SelectObject(hdc, old_fill);
        return finish();
    }
    if (name == L"wand" || name == L"sparkles" || name == L"auto_fix_high") {
        MoveToEx(hdc, rc.left + 7, rc.bottom - 6, nullptr); LineTo(hdc, rc.right - 8, rc.top + 7);
        MoveToEx(hdc, rc.right - 10, rc.top + 4, nullptr); LineTo(hdc, rc.right - 10, rc.top + 14);
        MoveToEx(hdc, rc.right - 15, rc.top + 9, nullptr); LineTo(hdc, rc.right - 5, rc.top + 9);
        return finish();
    }
    if (name == L"flag" || name == L"flag-checkered") {
        MoveToEx(hdc, rc.left + 6, rc.top + 5, nullptr); LineTo(hdc, rc.left + 6, rc.bottom - 4);
        Rectangle(hdc, rc.left + 7, rc.top + 5, rc.right - 5, cy + 2);
        return finish();
    }
    if (name == L"share" || name == L"share-alt") {
        HGDIOBJ old_fill = SelectObject(hdc, brush);
        Ellipse(hdc, rc.left + 4, cy - 4, rc.left + 12, cy + 4);
        Ellipse(hdc, rc.right - 12, rc.top + 3, rc.right - 4, rc.top + 11);
        Ellipse(hdc, rc.right - 12, rc.bottom - 11, rc.right - 4, rc.bottom - 3);
        SelectObject(hdc, old_fill);
        MoveToEx(hdc, rc.left + 12, cy, nullptr); LineTo(hdc, rc.right - 12, rc.top + 7);
        MoveToEx(hdc, rc.left + 12, cy, nullptr); LineTo(hdc, rc.right - 12, rc.bottom - 7);
        return finish();
    }
    if (name == L"move-horizontal" || name == L"swap_horiz") {
        MoveToEx(hdc, rc.left + 5, cy, nullptr); LineTo(hdc, rc.right - 5, cy);
        MoveToEx(hdc, rc.left + 5, cy, nullptr); LineTo(hdc, rc.left + 11, cy - 6); MoveToEx(hdc, rc.left + 5, cy, nullptr); LineTo(hdc, rc.left + 11, cy + 6);
        MoveToEx(hdc, rc.right - 5, cy, nullptr); LineTo(hdc, rc.right - 11, cy - 6); MoveToEx(hdc, rc.right - 5, cy, nullptr); LineTo(hdc, rc.right - 11, cy + 6);
        return finish();
    }
    if (name == L"code") {
        MoveToEx(hdc, cx - 4, rc.top + 5, nullptr); LineTo(hdc, rc.left + 5, cy); LineTo(hdc, cx - 4, rc.bottom - 5);
        MoveToEx(hdc, cx + 4, rc.top + 5, nullptr); LineTo(hdc, rc.right - 5, cy); LineTo(hdc, cx + 4, rc.bottom - 5);
        return finish();
    }

    SelectObject(hdc, old_brush);
    SelectObject(hdc, old_pen);
    DeleteObject(brush);
    DeleteObject(pen);
    return false;
}

void DrawButtonIconOrText(HDC hdc, RECT rc, const CustomButton& button) {
    COLORREF accent = ParseButtonColor(button.color);
    if (!DrawNamedIcon(hdc, rc, button.icon, accent)) {
        SetTextColor(hdc, accent);
        std::wstring icon = ButtonIcon(button);
        DrawTextW(hdc, icon.c_str(), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    }
}

COLORREF ParseButtonColor(const std::wstring& value) {
    int r = 255;
    int g = 190;
    int b = 80;
    if (swscanf_s(value.c_str(), L"rgba(%d, %d, %d", &r, &g, &b) == 3 ||
        swscanf_s(value.c_str(), L"rgb(%d, %d, %d", &r, &g, &b) == 3) {
        return RGB(std::max(0, std::min(255, r)), std::max(0, std::min(255, g)), std::max(0, std::min(255, b)));
    }
    if (value.size() == 7 && value[0] == L'#') {
        int rr = 255, gg = 190, bb = 80;
        swscanf_s(value.c_str() + 1, L"%02x%02x%02x", &rr, &gg, &bb);
        return RGB(rr, gg, bb);
    }
    return RGB(r, g, b);
}

void DrawButtonStripButton(HDC hdc, RECT rc, const PBQA_ResultItem& item, int number) {
    (void)number;
    HBRUSH bg = CreateSolidBrush(RGB(31, 31, 31));
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(64, 64, 64));
    HGDIOBJ old_brush = SelectObject(hdc, bg);
    HGDIOBJ old_pen = SelectObject(hdc, pen);
    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, 6, 6);
    SelectObject(hdc, old_pen);
    SelectObject(hdc, old_brush);
    DeleteObject(pen);
    DeleteObject(bg);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, KindAccent(item));
    RECT icon_rc = {rc.left + 8, rc.top + 5, rc.left + 42, rc.bottom - 5};
    DrawTextW(hdc, KindIcon(item), -1, &icon_rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    SetTextColor(hdc, RGB(235, 235, 235));
    RECT label_rc = {rc.left + 43, rc.top + 5, rc.right - 10, rc.bottom - 5};
    DrawTextW(hdc, item.name.c_str(), -1, &label_rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
}

void DrawCustomStripButton(HDC hdc, RECT rc, const CustomButton& button) {
    HBRUSH bg = CreateSolidBrush(RGB(31, 31, 31));
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(64, 64, 64));
    HGDIOBJ old_brush = SelectObject(hdc, bg);
    HGDIOBJ old_pen = SelectObject(hdc, pen);
    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, 6, 6);
    SelectObject(hdc, old_pen);
    SelectObject(hdc, old_brush);
    DeleteObject(pen);
    DeleteObject(bg);

    SetBkMode(hdc, TRANSPARENT);
    RECT icon_rc = {rc.left + 6, rc.top + 5, rc.left + 38, rc.bottom - 5};
    DrawButtonIconOrText(hdc, icon_rc, button);

    SetTextColor(hdc, RGB(235, 235, 235));
    RECT label_rc = {rc.left + 42, rc.top + 5, rc.right - 10, rc.bottom - 5};
    DrawTextW(hdc, button.name.c_str(), -1, &label_rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
}

void DrawManagerTabs(HDC hdc, RECT rc) {
    HBRUSH bg = CreateSolidBrush(RGB(18, 18, 18));
    FillRect(hdc, &rc, bg);
    DeleteObject(bg);
    SetBkMode(hdc, TRANSPARENT);
    SelectObject(hdc, g_font);
    int mid = (rc.left + rc.right) / 2;
    RECT left = {rc.left, rc.top, mid, rc.bottom};
    RECT right = {mid, rc.top, rc.right, rc.bottom};
    SetTextColor(hdc, g_manager_tab == 0 ? RGB(245, 245, 245) : RGB(165, 165, 165));
    DrawTextW(hdc, L"BUTTONS", -1, &left, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SetTextColor(hdc, g_manager_tab == 1 ? RGB(245, 245, 245) : RGB(165, 165, 165));
    DrawTextW(hdc, L"SETTINGS", -1, &right, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    HPEN pen = CreatePen(PS_SOLID, 2, RGB(26, 145, 245));
    HGDIOBJ old = SelectObject(hdc, pen);
    if (g_manager_tab == 0) {
        MoveToEx(hdc, rc.left, rc.bottom - 2, nullptr);
        LineTo(hdc, mid, rc.bottom - 2);
    } else {
        MoveToEx(hdc, mid, rc.bottom - 2, nullptr);
        LineTo(hdc, rc.right, rc.bottom - 2);
    }
    SelectObject(hdc, old);
    DeleteObject(pen);
}

LRESULT CALLBACK ButtonsProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    auto hit_index = [&](int px, int py) -> int {
        RECT rc;
        GetClientRect(hwnd, &rc);
        auto buttons = ButtonItems();
        int x = 0;
        int y = 4;
        const int gap = g_button_spacing;
        const int bh = 34;
        for (int i = 0; i < static_cast<int>(buttons.size()); ++i) {
            int bw = std::max(96, g_button_size);
            if (x > 0 && x + bw > rc.right) {
                x = 0;
                y += bh + gap;
            }
            if (py >= y && py < y + bh && px >= x && px < x + bw) {
                return i;
            }
            x += bw + gap;
        }
        int tool_count = static_cast<int>(buttons.size());
        for (int i = 0; i < static_cast<int>(g_custom_buttons.size()); ++i) {
            int bw = std::max(54, g_button_size);
            if (x > 0 && x + bw > rc.right) {
                x = 0;
                y += bh + gap;
            }
            if (py >= y && py < y + bh && px >= x && px < x + bw) {
                return tool_count + i;
            }
            x += bw + gap;
        }
        return -1;
    };

    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            HBRUSH bg = CreateSolidBrush(kBg);
            FillRect(hdc, &rc, bg);
            DeleteObject(bg);

            SelectObject(hdc, g_font);
            auto buttons = ButtonItems();
            int x = 0;
            int y = 4;
            const int gap = g_button_spacing;
            const int bh = 34;
            for (int i = 0; i < static_cast<int>(buttons.size()); ++i) {
                int bw = std::max(96, g_button_size);
                if (x > 0 && x + bw > rc.right) {
                    x = 0;
                    y += bh + gap;
                }
                if (y + bh > rc.bottom) {
                    break;
                }
                RECT brc = {x, y, x + bw, y + bh};
                DrawButtonStripButton(hdc, brc, g_items[buttons[i]], i + 1);
                x += bw + gap;
            }
            for (int i = 0; i < static_cast<int>(g_custom_buttons.size()); ++i) {
                int bw = std::max(54, g_button_size);
                if (x > 0 && x + bw > rc.right) {
                    x = 0;
                    y += bh + gap;
                }
                if (y + bh > rc.bottom) {
                    break;
                }
                RECT brc = {x, y, x + bw, y + bh};
                DrawCustomStripButton(hdc, brc, g_custom_buttons[static_cast<size_t>(i)]);
                x += bw + gap;
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_LBUTTONDOWN: {
            HideSettings();
            int hit = hit_index(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
            if (hit >= 0) {
                ApplyButton(hit);
                return 0;
            }
            SetFocus(g_edit);
            return 0;
        }
        case WM_RBUTTONDOWN: {
            HideSettings();
            auto buttons = ButtonItems();
            int hit = hit_index(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
            if (hit >= 0 && hit < static_cast<int>(buttons.size())) {
                PBQA_ResultItem& item = g_items[buttons[static_cast<size_t>(hit)]];
                if (!item.favorite) {
                    return 0;
                }
                HMENU menu = CreatePopupMenu();
                AppendMenuW(menu, MF_STRING, 1, L"Remove from Favorites");
                POINT pt = {GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)};
                ClientToScreen(hwnd, &pt);
                int cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, nullptr);
                DestroyMenu(menu);
                if (cmd == 1) {
                    item.favorite = false;
                    SaveFavorite(item);
                    FilterItems();
                }
                SetFocus(g_edit);
                return 0;
            }
            SetFocus(g_edit);
            return 0;
        }
        case WM_MOUSEMOVE: {
            auto buttons = ButtonItems();
            int hit = hit_index(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
            if (hit >= 0 && hit < static_cast<int>(buttons.size())) {
                std::wstring tip = g_items[buttons[static_cast<size_t>(hit)]].detail;
                SetWindowTextW(hwnd, tip.c_str());
                return 0;
            }
            int custom = hit - static_cast<int>(buttons.size());
            if (custom >= 0 && custom < static_cast<int>(g_custom_buttons.size())) {
                std::wstring tip = g_custom_buttons[static_cast<size_t>(custom)].tooltip.empty()
                    ? g_custom_buttons[static_cast<size_t>(custom)].name
                    : g_custom_buttons[static_cast<size_t>(custom)].tooltip;
                SetWindowTextW(hwnd, tip.c_str());
                return 0;
            }
            SetWindowTextW(hwnd, L"");
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wparam, lparam);
}

void DrawRunButton(HDC hdc, RECT rc) {
    HBRUSH brush = CreateSolidBrush(RGB(0, 96, 190));
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 96, 190));
    HGDIOBJ old_brush = SelectObject(hdc, brush);
    HGDIOBJ old_pen = SelectObject(hdc, pen);
    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, 7, 7);
    SelectObject(hdc, old_pen);
    SelectObject(hdc, old_brush);
    DeleteObject(pen);
    DeleteObject(brush);

    SetTextColor(hdc, RGB(245, 245, 245));
    SetBkMode(hdc, TRANSPARENT);
    DrawTextW(hdc, L"Run", -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

LRESULT CALLBACK ResultsProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);

            HBRUSH bg = CreateSolidBrush(kInput);
            FillRect(hdc, &rc, bg);
            DeleteObject(bg);

            SelectObject(hdc, g_font);
            SetBkMode(hdc, TRANSPARENT);

            const int row_h = 39;
            int rows = (rc.bottom - rc.top) / row_h;
            for (int i = 0; i < rows && i < static_cast<int>(g_filtered.size()); ++i) {
                const auto& item = g_items[g_filtered[i]];
                RECT row = {0, i * row_h, rc.right, (i + 1) * row_h};
                if (i == g_selected) {
                    HBRUSH sel = CreateSolidBrush(kBlue);
                    FillRect(hdc, &row, sel);
                    DeleteObject(sel);
                }

                SetTextColor(hdc, i == g_selected ? RGB(255, 255, 255) : kText);
                RECT name_rc = {42, row.top + 7, std::min<LONG>(270, rc.right - 160), row.bottom - 4};
                DrawTextW(hdc, item.name.c_str(), -1, &name_rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

                SetTextColor(hdc, i == g_selected ? RGB(225, 235, 255) : kMuted);
                RECT detail_rc = {280, row.top + 7, std::max<LONG>(290, rc.right - 126), row.bottom - 4};
                DrawTextW(hdc, item.detail.c_str(), -1, &detail_rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

                SetTextColor(hdc, i == g_selected ? RGB(255, 255, 255) : RGB(180, 180, 180));
                RECT icon_rc = {12, row.top + 9, 30, row.bottom - 8};
                SetTextColor(hdc, i == g_selected ? RGB(255, 255, 255) : KindAccent(item));
                DrawTextW(hdc, KindIcon(item), -1, &icon_rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

                if (item.favorite) {
                    SetTextColor(hdc, i == g_selected ? RGB(255, 245, 190) : RGB(220, 185, 65));
                    RECT star_rc = {rc.right - 126, row.top + 8, rc.right - 108, row.bottom - 8};
                    DrawTextW(hdc, L"\u2605", -1, &star_rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }

                if (i == g_selected) {
                    RECT run_rc = {rc.right - 105, row.top + 6, rc.right - 22, row.bottom - 7};
                    DrawRunButton(hdc, run_rc);
                }
            }

            HPEN border = CreatePen(PS_SOLID, 1, kLine);
            HGDIOBJ old = SelectObject(hdc, border);
            MoveToEx(hdc, rc.left, rc.top, nullptr);
            LineTo(hdc, rc.right, rc.top);
            LineTo(hdc, rc.right, rc.bottom);
            LineTo(hdc, rc.left, rc.bottom);
            LineTo(hdc, rc.left, rc.top);
            SelectObject(hdc, old);
            DeleteObject(border);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_LBUTTONDOWN: {
            HideSettings();
            int y = GET_Y_LPARAM(lparam);
            int x = GET_X_LPARAM(lparam);
            g_selected = y / 39;
            if (g_selected >= static_cast<int>(g_filtered.size())) {
                g_selected = static_cast<int>(g_filtered.size()) - 1;
            }
            InvalidateRect(hwnd, nullptr, FALSE);
            SetFocus(g_edit);
            RECT rc;
            GetClientRect(hwnd, &rc);
                if (g_selected >= 0 && g_selected < static_cast<int>(g_filtered.size())) {
                    ApplySelected();
                }
            return 0;
        }

        case WM_LBUTTONDBLCLK:
            ApplySelected();
            return 0;

        case WM_RBUTTONDOWN: {
            int y = GET_Y_LPARAM(lparam);
            g_selected = y / 39;
            if (g_selected < 0 || g_selected >= static_cast<int>(g_filtered.size())) {
                return 0;
            }
            size_t item_index = g_filtered[static_cast<size_t>(g_selected)];
            PBQA_ResultItem& item = g_items[item_index];
            InvalidateRect(hwnd, nullptr, FALSE);

            HMENU menu = CreatePopupMenu();
            AppendMenuW(menu, MF_STRING, 1, item.favorite ? L"Remove from Favorites" : L"Add to Favorites");
            POINT pt = {GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)};
            ClientToScreen(hwnd, &pt);
            int cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, nullptr);
            DestroyMenu(menu);
            if (cmd == 1) {
                item.favorite = !item.favorite;
                SaveFavorite(item);
                FilterItems();
            }
            SetFocus(g_edit);
            return 0;
        }
    }

    return DefWindowProcW(hwnd, msg, wparam, lparam);
}

void DrawCheckRow(HDC hdc, RECT rc, bool checked, const wchar_t* icon, const wchar_t* label) {
    RECT box = {rc.left + 18, rc.top + 10, rc.left + 36, rc.top + 28};
    HPEN pen = CreatePen(PS_SOLID, 2, checked ? kBlue : RGB(150, 150, 150));
    HBRUSH check_brush = checked ? CreateSolidBrush(kBlue) : nullptr;
    HGDIOBJ old_pen = SelectObject(hdc, pen);
    HGDIOBJ old_brush = SelectObject(hdc, checked ? check_brush : GetStockObject(NULL_BRUSH));
    RoundRect(hdc, box.left, box.top, box.right, box.bottom, 4, 4);
    if (checked) {
        SetTextColor(hdc, RGB(255, 255, 255));
        DrawTextW(hdc, L"\u2713", -1, &box, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    SelectObject(hdc, old_brush);
    SelectObject(hdc, old_pen);
    DeleteObject(pen);
    if (check_brush) {
        DeleteObject(check_brush);
    }

    SetTextColor(hdc, RGB(220, 220, 220));
    RECT icon_rc = {rc.left + 48, rc.top + 6, rc.left + 78, rc.bottom - 4};
    DrawTextW(hdc, icon, -1, &icon_rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT label_rc = {rc.left + 82, rc.top + 6, rc.right - 12, rc.bottom - 4};
    DrawTextW(hdc, label, -1, &label_rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

LRESULT CALLBACK SettingsProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            HBRUSH bg = CreateSolidBrush(RGB(25, 25, 25));
            FillRect(hdc, &rc, bg);
            DeleteObject(bg);

            SelectObject(hdc, g_font);
            SetBkMode(hdc, TRANSPARENT);
            DrawCheckRow(hdc, {0, 6, rc.right, 44}, g_include_effects, L"fx", L"Effects (e:)");
            DrawCheckRow(hdc, {0, 44, rc.right, 82}, g_include_commands, L"\u2630", L"Menu Commands (m:)");
            DrawCheckRow(hdc, {0, 82, rc.right, 120}, g_include_presets, L"ffx", L"Animation Presets (p:)");

            HPEN line = CreatePen(PS_SOLID, 1, RGB(70, 70, 70));
            HGDIOBJ old = SelectObject(hdc, line);
            MoveToEx(hdc, 18, 124, nullptr);
            LineTo(hdc, rc.right - 18, 124);
            SelectObject(hdc, old);
            DeleteObject(line);

            DrawCheckRow(hdc, {0, 130, rc.right, 168}, g_keep_at_cursor, L"\u2723", L"Show dialog at cursor location");

            HPEN border = CreatePen(PS_SOLID, 1, RGB(66, 66, 66));
            old = SelectObject(hdc, border);
            HGDIOBJ old_brush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
            RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, 7, 7);
            SelectObject(hdc, old_brush);
            SelectObject(hdc, old);
            DeleteObject(border);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_LBUTTONDOWN: {
            int y = GET_Y_LPARAM(lparam);
            if (y >= 6 && y < 44) {
                g_include_effects = !g_include_effects;
            } else if (y >= 44 && y < 82) {
                g_include_commands = !g_include_commands;
            } else if (y >= 82 && y < 120) {
                g_include_presets = !g_include_presets;
            } else if (y >= 130 && y < 168) {
                g_keep_at_cursor = !g_keep_at_cursor;
            }
            SaveSettings();
            FilterItems();
            InvalidateRect(hwnd, nullptr, TRUE);
            SetFocus(g_edit);
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wparam, lparam);
}

HWND AddStatic(HWND parent, const wchar_t* text, int x, int y, int w, int h) {
    HWND label = CreateWindowExW(0, L"STATIC", text, WS_CHILD | WS_VISIBLE, x, y, w, h, parent, nullptr, GetModuleHandleW(nullptr), nullptr);
    SendMessageW(label, WM_SETFONT, reinterpret_cast<WPARAM>(g_small_font ? g_small_font : g_font), TRUE);
    return label;
}

HWND AddEdit(HWND parent, int id, int x, int y, int w, int h, DWORD extra_style = 0) {
    HWND edit = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | extra_style,
        x, y, w, h,
        parent,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
        GetModuleHandleW(nullptr),
        nullptr);
    SendMessageW(edit, WM_SETFONT, reinterpret_cast<WPARAM>(g_small_font ? g_small_font : g_font), TRUE);
    return edit;
}

HWND AddButton(HWND parent, int id, const wchar_t* text, int x, int y, int w, int h) {
    HWND btn = CreateWindowExW(
        0,
        L"BUTTON",
        text,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        x, y, w, h,
        parent,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
        GetModuleHandleW(nullptr),
        nullptr);
    SendMessageW(btn, WM_SETFONT, reinterpret_cast<WPARAM>(g_small_font ? g_small_font : g_font), TRUE);
    return btn;
}

LRESULT CALLBACK ManagerProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_CREATE: {
            g_small_font = CreateFontW(
                -16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

            g_btn_list = CreateWindowExW(
                0,
                L"LISTBOX",
                L"",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | WS_VSCROLL,
                18, 38, 270, 360,
                hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_BTN_LIST)),
                GetModuleHandleW(nullptr),
                nullptr);
            SendMessageW(g_btn_list, WM_SETFONT, reinterpret_cast<WPARAM>(g_small_font), TRUE);

            g_lbl_buttons = AddStatic(hwnd, L"BUTTONS", 18, 14, 200, 20);
            g_lbl_name = AddStatic(hwnd, L"Name", 310, 18, 120, 20);
            g_name_edit = AddEdit(hwnd, IDC_NAME_EDIT, 405, 14, 320, 26);
            g_lbl_tip = AddStatic(hwnd, L"Tooltip", 310, 52, 120, 20);
            g_tip_edit = AddEdit(hwnd, IDC_TIP_EDIT, 405, 48, 320, 26);
            g_lbl_icon = AddStatic(hwnd, L"Icon", 310, 86, 120, 20);
            g_icon_edit = AddEdit(hwnd, IDC_ICON_EDIT, 405, 82, 160, 26);
            AddButton(hwnd, IDC_ICON_PICK_BUTTON, L"Icon...", 0, 0, 72, 28);
            g_lbl_color = AddStatic(hwnd, L"Color", 575, 86, 80, 20);
            g_color_edit = AddEdit(hwnd, IDC_COLOR_EDIT, 635, 82, 90, 26);
            AddButton(hwnd, IDC_COLOR_PICK_BUTTON, L"Color...", 0, 0, 76, 28);
            g_lbl_action = AddStatic(hwnd, L"Action", 310, 120, 120, 20);
            g_action_edit = AddEdit(hwnd, IDC_ACTION_EDIT, 405, 116, 160, 26);
            g_lbl_value = AddStatic(hwnd, L"Value", 310, 154, 120, 20);
            g_value_edit = AddEdit(hwnd, IDC_VALUE_EDIT, 405, 150, 320, 170, ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VSCROLL);

            g_lbl_help_actions = AddStatic(hwnd, L"Actions: menu, expression, preset, script, scriptfile", 405, 326, 370, 22);
            g_lbl_help_icons = AddStatic(hwnd, L"Icons: camera, search, scissors, film, trash, wand, zap, play, box, code", 405, 350, 370, 38);

            AddButton(hwnd, IDC_NEW_BUTTON, L"Add Button", 18, 412, 112, 30);
            AddButton(hwnd, IDC_SAVE_BUTTON, L"Save", 138, 412, 70, 30);
            AddButton(hwnd, IDC_DELETE_BUTTON, L"Delete", 216, 412, 76, 30);
            AddButton(hwnd, IDC_UP_BUTTON, L"Up", 18, 448, 58, 30);
            AddButton(hwnd, IDC_DOWN_BUTTON, L"Down", 84, 448, 76, 30);

            g_lbl_size = AddStatic(hwnd, L"Button size", 310, 404, 110, 22);
            AddButton(hwnd, IDC_SIZE_MINUS, L"-", 405, 400, 34, 30);
            AddButton(hwnd, IDC_SIZE_PLUS, L"+", 445, 400, 34, 30);
            g_lbl_spacing = AddStatic(hwnd, L"Spacing", 500, 404, 80, 22);
            AddButton(hwnd, IDC_SPACE_MINUS, L"-", 570, 400, 34, 30);
            AddButton(hwnd, IDC_SPACE_PLUS, L"+", 610, 400, 34, 30);
            AddButton(hwnd, IDCANCEL, L"Close", 650, 448, 76, 30);
            ShowManagerTab(hwnd, 0);

            return 0;
        }

        case WM_MEASUREITEM: {
            auto* item = reinterpret_cast<MEASUREITEMSTRUCT*>(lparam);
            if (item && item->CtlID == IDC_BTN_LIST) {
                item->itemHeight = 42;
                return TRUE;
            }
            break;
        }

        case WM_DRAWITEM: {
            auto* draw = reinterpret_cast<DRAWITEMSTRUCT*>(lparam);
            if (!draw || draw->CtlID != IDC_BTN_LIST || draw->itemID == static_cast<UINT>(-1)) {
                break;
            }
            int index = static_cast<int>(draw->itemID);
            RECT row = draw->rcItem;
            bool selected = (draw->itemState & ODS_SELECTED) != 0;
            HBRUSH bg = CreateSolidBrush(selected ? RGB(13, 120, 232) : RGB(18, 18, 18));
            FillRect(draw->hDC, &row, bg);
            DeleteObject(bg);
            if (index >= 0 && index < static_cast<int>(g_custom_buttons.size())) {
                const CustomButton& button = g_custom_buttons[static_cast<size_t>(index)];
                SetBkMode(draw->hDC, TRANSPARENT);
                SelectObject(draw->hDC, g_small_font ? g_small_font : g_font);
                SetTextColor(draw->hDC, selected ? RGB(255, 255, 255) : RGB(165, 165, 165));
                RECT chevron = {row.left + 8, row.top, row.left + 28, row.bottom};
                DrawTextW(draw->hDC, L">", -1, &chevron, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                RECT icon = {row.left + 34, row.top + 7, row.left + 64, row.bottom - 7};
                DrawButtonIconOrText(draw->hDC, icon, button);
                SetTextColor(draw->hDC, selected ? RGB(255, 255, 255) : RGB(218, 218, 218));
                RECT name = {row.left + 76, row.top + 4, row.right - 210, row.bottom - 4};
                DrawTextW(draw->hDC, button.name.c_str(), -1, &name, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
                SetTextColor(draw->hDC, selected ? RGB(220, 235, 255) : RGB(105, 105, 105));
                RECT kind = {row.right - 180, row.top + 4, row.right - 58, row.bottom - 4};
                DrawTextW(draw->hDC, button.action.c_str(), -1, &kind, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
                SetTextColor(draw->hDC, selected ? RGB(255, 255, 255) : RGB(185, 185, 185));
                RECT edit = {row.right - 44, row.top + 4, row.right - 12, row.bottom - 4};
                DrawTextW(draw->hDC, L"...", -1, &edit, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
            }
            return TRUE;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            HBRUSH bg = CreateSolidBrush(RGB(28, 28, 28));
            FillRect(hdc, &rc, bg);
            DeleteObject(bg);
            RECT tabs = {0, 0, rc.right, 56};
            DrawManagerTabs(hdc, tabs);
            if (g_manager_tab == 1) {
                SelectObject(hdc, g_font);
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, RGB(190, 190, 190));
                RECT hint = {44, 108, rc.right - 44, 144};
                DrawTextW(hdc, L"Toolbar appearance", -1, &hint, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLOREDIT: {
            HDC hdc = reinterpret_cast<HDC>(wparam);
            SetTextColor(hdc, RGB(210, 210, 210));
            SetBkColor(hdc, RGB(22, 22, 22));
            static HBRUSH dark_brush = CreateSolidBrush(RGB(22, 22, 22));
            return reinterpret_cast<LRESULT>(dark_brush);
        }

        case WM_SIZE: {
            int w = LOWORD(lparam);
            int h = HIWORD(lparam);
            int margin = 22;
            int gutter = 28;
            int footer_h = 74;
            int left_w = std::max(300, std::min(430, w / 3));
            int list_top = 92;
            int list_bottom = std::max(list_top + 150, h - footer_h - 22);
            int footer_y = list_bottom + 14;

            int right_x = left_w + gutter;
            int label_w = 96;
            int field_x = right_x + label_w + 12;
            int field_w = std::max(220, w - field_x - margin);
            int row_h = 28;
            int icon_w = std::max(120, field_w / 2 - 44);
            int color_x = field_x + icon_w + 72;
            int color_w = std::max(120, w - color_x - margin);
            int value_top = 232;
            int value_bottom = std::max(value_top + 160, h - 156);

            MoveWindow(g_lbl_buttons, margin, 66, left_w - margin * 2, 22, TRUE);
            MoveWindow(g_btn_list, margin, list_top, left_w - margin * 2, list_bottom - list_top, TRUE);

            MoveWindow(GetDlgItem(hwnd, IDC_NEW_BUTTON), margin, footer_y, 112, 30, TRUE);
            MoveWindow(GetDlgItem(hwnd, IDC_SAVE_BUTTON), margin + 122, footer_y, 70, 30, TRUE);
            MoveWindow(GetDlgItem(hwnd, IDC_DELETE_BUTTON), margin + 202, footer_y, 76, 30, TRUE);
            MoveWindow(GetDlgItem(hwnd, IDC_UP_BUTTON), margin, footer_y + 36, 58, 30, TRUE);
            MoveWindow(GetDlgItem(hwnd, IDC_DOWN_BUTTON), margin + 68, footer_y + 36, 76, 30, TRUE);

            MoveWindow(g_lbl_name, right_x, 66, label_w, 22, TRUE);
            MoveWindow(g_name_edit, field_x, 62, field_w, row_h, TRUE);
            MoveWindow(g_lbl_tip, right_x, 104, label_w, 22, TRUE);
            MoveWindow(g_tip_edit, field_x, 100, field_w, row_h, TRUE);
            MoveWindow(g_lbl_icon, right_x, 142, label_w, 22, TRUE);
            MoveWindow(g_icon_edit, field_x, 138, std::max(90, icon_w - 82), row_h, TRUE);
            MoveWindow(GetDlgItem(hwnd, IDC_ICON_PICK_BUTTON), field_x + std::max(90, icon_w - 82) + 8, 138, 74, row_h, TRUE);
            MoveWindow(g_lbl_color, field_x + icon_w + 18, 142, 54, 22, TRUE);
            MoveWindow(g_color_edit, color_x, 138, std::max(90, color_w - 84), row_h, TRUE);
            MoveWindow(GetDlgItem(hwnd, IDC_COLOR_PICK_BUTTON), color_x + std::max(90, color_w - 84) + 8, 138, 76, row_h, TRUE);
            MoveWindow(g_lbl_action, right_x, 180, label_w, 22, TRUE);
            MoveWindow(g_action_edit, field_x, 176, std::min(220, field_w), row_h, TRUE);
            MoveWindow(g_lbl_value, right_x, value_top + 4, label_w, 22, TRUE);
            MoveWindow(g_value_edit, field_x, value_top, field_w, value_bottom - value_top, TRUE);

            MoveWindow(g_lbl_help_actions, field_x, value_bottom + 12, field_w, 22, TRUE);
            MoveWindow(g_lbl_help_icons, field_x, value_bottom + 36, field_w, 42, TRUE);

            int settings_y = g_manager_tab == 1 ? 160 : std::max(value_bottom + 88, h - 58);
            int settings_x = g_manager_tab == 1 ? 46 : right_x;
            MoveWindow(g_lbl_size, settings_x, settings_y + 4, 130, 22, TRUE);
            MoveWindow(GetDlgItem(hwnd, IDC_SIZE_MINUS), settings_x + 150, settings_y, 34, 30, TRUE);
            MoveWindow(GetDlgItem(hwnd, IDC_SIZE_PLUS), settings_x + 190, settings_y, 34, 30, TRUE);
            MoveWindow(g_lbl_spacing, settings_x, settings_y + 64, 130, 22, TRUE);
            MoveWindow(GetDlgItem(hwnd, IDC_SPACE_MINUS), settings_x + 150, settings_y + 60, 34, 30, TRUE);
            MoveWindow(GetDlgItem(hwnd, IDC_SPACE_PLUS), settings_x + 190, settings_y + 60, 34, 30, TRUE);
            MoveWindow(GetDlgItem(hwnd, IDCANCEL), w - margin - 82, h - 52, 82, 30, TRUE);
            return 0;
        }

        case WM_GETMINMAXINFO: {
            auto* info = reinterpret_cast<MINMAXINFO*>(lparam);
            info->ptMinTrackSize.x = 820;
            info->ptMinTrackSize.y = 620;
            return 0;
        }

        case WM_LBUTTONDOWN: {
            int y = GET_Y_LPARAM(lparam);
            int x = GET_X_LPARAM(lparam);
            RECT rc;
            GetClientRect(hwnd, &rc);
            if (y >= 0 && y < 56) {
                ShowManagerTab(hwnd, x < rc.right / 2 ? 0 : 1);
                return 0;
            }
            break;
        }

        case WM_COMMAND: {
            int id = LOWORD(wparam);
            if (id == IDC_BTN_LIST && HIWORD(wparam) == LBN_SELCHANGE) {
                LoadManagerFields(SelectedManagerIndex());
                return 0;
            }
            if (id == IDC_NEW_BUTTON) {
                HMENU menu = CreatePopupMenu();
                AppendMenuW(menu, MF_STRING, 1, L"Invoke Menu Item");
                AppendMenuW(menu, MF_STRING, 2, L"Set Expression");
                AppendMenuW(menu, MF_STRING, 3, L"Apply Preset");
                AppendMenuW(menu, MF_STRING, 4, L"Run Scriptlet");
                AppendMenuW(menu, MF_STRING, 5, L"Run JSX/JSXBIN File");
                RECT add_rc = {};
                GetWindowRect(GetDlgItem(hwnd, IDC_NEW_BUTTON), &add_rc);
                POINT pt = {add_rc.left, add_rc.bottom + 4};
                int cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, nullptr);
                DestroyMenu(menu);
                if (!cmd) {
                    return 0;
                }
                SendMessageW(g_btn_list, LB_SETCURSEL, static_cast<WPARAM>(-1), 0);
                LoadManagerFields(-1);
                SetWindowTextW(g_name_edit, L"New Button");
                SetWindowTextW(g_icon_edit, L"zap");
                if (cmd == 1) SetWindowTextW(g_action_edit, L"menu");
                if (cmd == 2) SetWindowTextW(g_action_edit, L"expression");
                if (cmd == 3) SetWindowTextW(g_action_edit, L"preset");
                if (cmd == 4) SetWindowTextW(g_action_edit, L"script");
                if (cmd == 5) SetWindowTextW(g_action_edit, L"scriptfile");
                SetFocus(g_name_edit);
                return 0;
            }
            if (id == IDC_ICON_PICK_BUTTON) {
                const wchar_t* icons[] = {
                    L"Text / keep current", L"videocam", L"photo_camera", L"search", L"content_cut", L"film", L"delete",
                    L"auto_fix_high", L"bolt", L"play_arrow", L"layers", L"code", L"refresh", L"repeat",
                    L"print", L"apps", L"flag", L"share", L"swap_horiz"
                };
                HMENU menu = CreatePopupMenu();
                for (UINT i = 0; i < sizeof(icons) / sizeof(icons[0]); ++i) {
                    AppendMenuW(menu, MF_STRING, 100 + i, icons[i]);
                }
                RECT pick_rc = {};
                GetWindowRect(GetDlgItem(hwnd, IDC_ICON_PICK_BUTTON), &pick_rc);
                int cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pick_rc.left, pick_rc.bottom + 4, 0, hwnd, nullptr);
                DestroyMenu(menu);
                if (cmd > 100) {
                    SetWindowTextW(g_icon_edit, icons[cmd - 100]);
                    SaveManagerFields(SelectedManagerIndex());
                }
                return 0;
            }
            if (id == IDC_COLOR_PICK_BUTTON) {
                struct ColorChoice { const wchar_t* label; const wchar_t* value; };
                ColorChoice colors[] = {
                    {L"Red", L"rgba(252, 67, 88, 1)"}, {L"Yellow", L"rgba(248, 231, 28, 1)"},
                    {L"Cyan", L"rgba(12, 209, 243, 1)"}, {L"Green", L"rgba(126, 211, 33, 1)"},
                    {L"Orange", L"rgba(245, 166, 35, 1)"}, {L"Blue", L"rgba(74, 144, 226, 1)"},
                    {L"Magenta", L"rgba(189, 16, 224, 1)"}, {L"White", L"rgba(235, 235, 235, 1)"}
                };
                HMENU menu = CreatePopupMenu();
                for (UINT i = 0; i < sizeof(colors) / sizeof(colors[0]); ++i) {
                    AppendMenuW(menu, MF_STRING, 200 + i, colors[i].label);
                }
                RECT pick_rc = {};
                GetWindowRect(GetDlgItem(hwnd, IDC_COLOR_PICK_BUTTON), &pick_rc);
                int cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pick_rc.left, pick_rc.bottom + 4, 0, hwnd, nullptr);
                DestroyMenu(menu);
                if (cmd >= 200) {
                    SetWindowTextW(g_color_edit, colors[cmd - 200].value);
                    SaveManagerFields(SelectedManagerIndex());
                }
                return 0;
            }
            if (id == IDC_SAVE_BUTTON) {
                SaveManagerFields(SelectedManagerIndex());
                return 0;
            }
            if (id == IDC_DELETE_BUTTON) {
                int index = SelectedManagerIndex();
                if (index >= 0 && index < static_cast<int>(g_custom_buttons.size())) {
                    g_custom_buttons.erase(g_custom_buttons.begin() + index);
                    SaveCustomButtons();
                    RefreshManagerList();
                    LoadManagerFields(-1);
                    if (g_buttons) {
                        InvalidateRect(g_buttons, nullptr, TRUE);
                    }
                }
                return 0;
            }
            if (id == IDC_UP_BUTTON || id == IDC_DOWN_BUTTON) {
                int index = SelectedManagerIndex();
                int next = id == IDC_UP_BUTTON ? index - 1 : index + 1;
                if (index >= 0 && next >= 0 && next < static_cast<int>(g_custom_buttons.size())) {
                    std::swap(g_custom_buttons[static_cast<size_t>(index)], g_custom_buttons[static_cast<size_t>(next)]);
                    SaveCustomButtons();
                    RefreshManagerList();
                    SendMessageW(g_btn_list, LB_SETCURSEL, next, 0);
                    if (g_buttons) {
                        InvalidateRect(g_buttons, nullptr, TRUE);
                    }
                }
                return 0;
            }
            if (id == IDC_SIZE_MINUS || id == IDC_SIZE_PLUS || id == IDC_SPACE_MINUS || id == IDC_SPACE_PLUS) {
                if (id == IDC_SIZE_MINUS) g_button_size -= 8;
                if (id == IDC_SIZE_PLUS) g_button_size += 8;
                if (id == IDC_SPACE_MINUS) g_button_spacing -= 2;
                if (id == IDC_SPACE_PLUS) g_button_spacing += 2;
                if (g_button_size < 72) g_button_size = 72;
                if (g_button_size > 260) g_button_size = 260;
                if (g_button_spacing < 0) g_button_spacing = 0;
                if (g_button_spacing > 32) g_button_spacing = 32;
                SaveSettings();
                if (g_buttons) {
                    InvalidateRect(g_buttons, nullptr, TRUE);
                }
                return 0;
            }
            if (id == IDCANCEL) {
                ShowWindow(hwnd, SW_HIDE);
                return 0;
            }
            break;
        }

        case WM_CLOSE:
            ShowWindow(hwnd, SW_HIDE);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wparam, lparam);
}

LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (msg == WM_LBUTTONDOWN || msg == WM_SETFOCUS) {
        HideSettings();
    }
    if (msg == WM_KEYDOWN) {
        if (wparam == VK_ESCAPE) {
            ShowWindow(g_hwnd, SW_HIDE);
            HideSettings();
            return 0;
        }
        if (wparam == VK_RETURN) {
            ApplySelected();
            return 0;
        }
        if (wparam == VK_DOWN) {
            MoveSelection(1);
            return 0;
        }
        if (wparam == VK_UP) {
            MoveSelection(-1);
            return 0;
        }
    }
    return CallWindowProcW(g_old_edit_proc, hwnd, msg, wparam, lparam);
}

void LayoutPopup(HWND hwnd, int w, int h) {
    bool show_results = SearchListVisible();
    MoveWindow(g_tools, 13, 10, 100, 39, TRUE);
    MoveWindow(g_edit, 124, 11, std::max(160, w - 148), 38, TRUE);
    MoveWindow(g_buttons, 13, 58, std::max(220, w - 26), 126, TRUE);
    MoveWindow(g_results, 13, 194, std::max(220, w - 26), show_results ? std::max(110, h - 239) : 0, TRUE);
    ShowWindow(g_results, show_results ? SW_SHOW : SW_HIDE);
    if (g_settings) {
        RECT window_rc = {};
        GetWindowRect(hwnd, &window_rc);
        SetWindowPos(g_settings, nullptr, window_rc.left + 13, window_rc.top + 55, 330, 174, SWP_NOZORDER);
    }
}

void FitPopupToSearchState() {
    if (!g_hwnd) {
        return;
    }
    RECT rc = {};
    GetWindowRect(g_hwnd, &rc);
    int w = rc.right - rc.left;
    int target_h = SearchListVisible() ? 620 : 244;
    if ((rc.bottom - rc.top) != target_h) {
        SetWindowPos(g_hwnd, HWND_TOPMOST, rc.left, rc.top, w, target_h, SWP_NOACTIVATE);
    }
    LayoutPopup(g_hwnd, w, target_h);
}

LRESULT CALLBACK PopupProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_CREATE: {
            g_font = CreateFontW(
                -18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

            g_tools = CreateWindowExW(0, L"PBQA_Tools", L"", WS_CHILD | WS_VISIBLE, 13, 10, 100, 39, hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
            g_edit = CreateWindowExW(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 124, 11, 576, 38, hwnd, reinterpret_cast<HMENU>(1001), GetModuleHandleW(nullptr), nullptr);
            g_buttons = CreateWindowExW(0, L"PBQA_Buttons", L"", WS_CHILD | WS_VISIBLE, 13, 58, 696, 126, hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
            g_results = CreateWindowExW(0, L"PBQA_Results", L"", WS_CHILD | WS_VISIBLE, 13, 194, 696, 310, hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
            g_settings = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, L"PBQA_Settings", L"", WS_POPUP, 0, 0, 330, 174, hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
            g_manager = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, L"PBQA_Manager", L"PersonalBar Settings", WS_POPUP | WS_CAPTION | WS_THICKFRAME, 0, 0, 760, 520, hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);

            SendMessageW(g_edit, WM_SETFONT, reinterpret_cast<WPARAM>(g_font), TRUE);
            SendMessageW(g_edit, EM_SETCUEBANNER, TRUE, reinterpret_cast<LPARAM>(L"Search effects, presets, commands, tools..."));
            g_old_edit_proc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(g_edit, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(EditSubclassProc)));

            FilterItems();
            SetFocus(g_edit);
            return 0;
        }

        case WM_COMMAND:
            if (LOWORD(wparam) == 1001 && HIWORD(wparam) == EN_CHANGE) {
                HideSettings();
                g_selected = 0;
                FilterItems();
                FitPopupToSearchState();
                return 0;
            }
            break;

        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLOREDIT: {
            HDC hdc = reinterpret_cast<HDC>(wparam);
            SetTextColor(hdc, kText);
            SetBkColor(hdc, kInput);
            static HBRUSH edit_brush = CreateSolidBrush(kInput);
            return reinterpret_cast<LRESULT>(edit_brush);
        }

        case WM_ACTIVATE:
            if (LOWORD(wparam) == WA_INACTIVE) {
                HWND next = reinterpret_cast<HWND>(lparam);
                if (!next || (next != g_settings && !IsChild(g_hwnd, next))) {
                    ShowWindow(g_hwnd, SW_HIDE);
                    HideSettings();
                }
            }
            return 0;

        case WM_SIZE: {
            int w = LOWORD(lparam);
            int h = HIWORD(lparam);
            LayoutPopup(hwnd, w, h);
            return 0;
        }

        case WM_CLOSE:
            ShowWindow(hwnd, SW_HIDE);
            HideSettings();
            return 0;

        case WM_SHOWWINDOW:
            if (!wparam) {
                HideSettings();
            }
            break;

        case WM_DESTROY:
            if (g_font) {
                DeleteObject(g_font);
                g_font = nullptr;
            }
            if (g_small_font) {
                DeleteObject(g_small_font);
                g_small_font = nullptr;
            }
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wparam, lparam);
}

void RegisterClassOnce() {
    static bool registered = false;
    if (registered) {
        return;
    }

    WNDCLASSW wc = {};
    wc.lpfnWndProc = PopupProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kWindowClass;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(kPanel);
    RegisterClassW(&wc);

    WNDCLASSW results = {};
    results.lpfnWndProc = ResultsProc;
    results.hInstance = GetModuleHandleW(nullptr);
    results.lpszClassName = L"PBQA_Results";
    results.hCursor = LoadCursor(nullptr, IDC_ARROW);
    results.hbrBackground = CreateSolidBrush(kInput);
    RegisterClassW(&results);

    WNDCLASSW tools = {};
    tools.lpfnWndProc = ToolsProc;
    tools.hInstance = GetModuleHandleW(nullptr);
    tools.lpszClassName = L"PBQA_Tools";
    tools.hCursor = LoadCursor(nullptr, IDC_ARROW);
    tools.hbrBackground = CreateSolidBrush(kBg);
    RegisterClassW(&tools);

    WNDCLASSW buttons = {};
    buttons.lpfnWndProc = ButtonsProc;
    buttons.hInstance = GetModuleHandleW(nullptr);
    buttons.lpszClassName = L"PBQA_Buttons";
    buttons.hCursor = LoadCursor(nullptr, IDC_HAND);
    buttons.hbrBackground = CreateSolidBrush(kBg);
    RegisterClassW(&buttons);

    WNDCLASSW settings = {};
    settings.lpfnWndProc = SettingsProc;
    settings.hInstance = GetModuleHandleW(nullptr);
    settings.lpszClassName = L"PBQA_Settings";
    settings.hCursor = LoadCursor(nullptr, IDC_ARROW);
    settings.hbrBackground = CreateSolidBrush(RGB(25, 25, 25));
    RegisterClassW(&settings);

    WNDCLASSW manager = {};
    manager.lpfnWndProc = ManagerProc;
    manager.hInstance = GetModuleHandleW(nullptr);
    manager.lpszClassName = L"PBQA_Manager";
    manager.hCursor = LoadCursor(nullptr, IDC_ARROW);
    manager.hbrBackground = CreateSolidBrush(RGB(28, 28, 28));
    RegisterClassW(&manager);

    registered = true;
}

} // namespace

void PBQA_SetItems(std::vector<PBQA_ResultItem> items) {
    LoadSettings();
    LoadCustomButtons();
    g_items = std::move(items);
    MarkFavorites();
    FilterItems();
}

void PBQA_SetApplyCallback(PBQA_ApplyCallback callback) {
    g_apply_callback = std::move(callback);
}

void PBQA_ShowPopupWindow() {
    RegisterClassOnce();
    LoadSettings();
    LoadCustomButtons();

    if (!g_hwnd) {
        g_hwnd = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
            kWindowClass,
            L"Quick Apply",
            WS_POPUP | WS_CAPTION | WS_THICKFRAME,
            CW_USEDEFAULT, CW_USEDEFAULT, 724, 620,
            nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    }

    RECT work = {};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);
    int w = 724;
    int h = 244;
    int x = work.left + ((work.right - work.left) - w) / 2;
    int y = work.top + 110;
    if (g_keep_at_cursor) {
        POINT pt = {};
        GetCursorPos(&pt);
        x = std::min(std::max(work.left, pt.x - 120), work.right - w);
        y = std::min(std::max(work.top, pt.y - 65), work.bottom - h);
    }
    SetWindowPos(g_hwnd, HWND_TOPMOST, x, y, w, h, SWP_SHOWWINDOW);
    ShowWindow(g_hwnd, SW_SHOWNORMAL);
    SetForegroundWindow(g_hwnd);

    if (g_edit) {
        SetWindowTextW(g_edit, L"");
        g_selected = 0;
        FilterItems();
        FitPopupToSearchState();
        if (g_settings) {
            ShowWindow(g_settings, SW_HIDE);
        }
        SetFocus(g_edit);
    }
}

void PBQA_ReloadCustomButtons() {
    LoadCustomButtons();
    if (g_buttons) {
        InvalidateRect(g_buttons, nullptr, TRUE);
    }
}
