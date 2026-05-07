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

std::vector<PBQA_ResultItem> g_items;
std::vector<size_t> g_filtered;
PBQA_ApplyCallback g_apply_callback;

HWND g_hwnd = nullptr;
HWND g_edit = nullptr;
HWND g_results = nullptr;
HWND g_tools = nullptr;
HWND g_buttons = nullptr;
HWND g_settings = nullptr;
int g_selected = 0;
HFONT g_font = nullptr;
WNDPROC g_old_edit_proc = nullptr;
bool g_include_effects = true;
bool g_include_presets = true;
bool g_include_commands = true;
bool g_keep_at_cursor = false;

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
    wchar_t buffer[512] = {};
    GetWindowTextW(g_edit, buffer, 512);
    return buffer;
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

    InvalidateRect(g_results, nullptr, TRUE);
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

std::vector<size_t> ButtonItems() {
    std::vector<size_t> out;
    for (size_t i = 0; i < g_items.size() && out.size() < 9; ++i) {
        if (g_items[i].kind == L"Tool") {
            out.push_back(i);
        }
    }
    return out;
}

void ApplyButton(int index) {
    auto buttons = ButtonItems();
    if (index < 0 || index >= static_cast<int>(buttons.size())) {
        return;
    }
    PBQA_ResultItem item = g_items[buttons[static_cast<size_t>(index)]];
    ShowWindow(g_hwnd, SW_HIDE);
    if (g_apply_callback) {
        g_apply_callback(item);
    }
}

void PaintToolbarButton(HDC hdc, RECT rc) {
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

    HPEN gear_pen = CreatePen(PS_SOLID, 3, RGB(195, 195, 195));
    HGDIOBJ gear_old = SelectObject(hdc, gear_pen);
    int cx = (rc.left + rc.right) / 2;
    int cy = (rc.top + rc.bottom) / 2;
    MoveToEx(hdc, cx - 9, cy + 10, nullptr);
    LineTo(hdc, cx + 8, cy - 7);
    Ellipse(hdc, cx + 4, cy - 12, cx + 15, cy - 1);
    MoveToEx(hdc, cx - 12, cy + 7, nullptr);
    LineTo(hdc, cx - 6, cy + 13);
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
        RECT btn = {0, 0, 46, 38};
        PaintToolbarButton(hdc, btn);
        EndPaint(hwnd, &ps);
        return 0;
    }
    if (msg == WM_LBUTTONDOWN) {
        if (g_settings && IsWindowVisible(g_settings)) {
            ShowWindow(g_settings, SW_HIDE);
        } else if (g_settings) {
            RECT window_rc = {};
            GetWindowRect(g_hwnd, &window_rc);
            SetWindowPos(g_settings, HWND_TOPMOST, window_rc.left + 13, window_rc.top + 55, 330, 174, 0);
            ShowWindow(g_settings, SW_SHOWNORMAL);
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
    return RGB(75, 235, 205);
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

LRESULT CALLBACK ButtonsProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
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
            const int gap = 8;
            for (int i = 0; i < static_cast<int>(buttons.size()); ++i) {
                int bw = (g_items[buttons[i]].kind == L"Tool") ? 164 : 138;
                if (x + bw > rc.right) {
                    break;
                }
                RECT brc = {x, 4, x + bw, rc.bottom - 4};
                DrawButtonStripButton(hdc, brc, g_items[buttons[i]], i + 1);
                x += bw + gap;
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_LBUTTONDOWN: {
            HideSettings();
            auto buttons = ButtonItems();
            int x = 0;
            int px = GET_X_LPARAM(lparam);
            const int gap = 8;
            for (int i = 0; i < static_cast<int>(buttons.size()); ++i) {
                int bw = (g_items[buttons[i]].kind == L"Tool") ? 164 : 138;
                if (px >= x && px < x + bw) {
                    ApplyButton(i);
                    return 0;
                }
                x += bw + gap;
            }
            SetFocus(g_edit);
            return 0;
        }
        case WM_RBUTTONDOWN: {
            HideSettings();
            auto buttons = ButtonItems();
            int x = 0;
            int px = GET_X_LPARAM(lparam);
            const int gap = 8;
            for (int i = 0; i < static_cast<int>(buttons.size()); ++i) {
                int bw = (g_items[buttons[i]].kind == L"Tool") ? 164 : 138;
                if (px >= x && px < x + bw) {
                    PBQA_ResultItem& item = g_items[buttons[i]];
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
                x += bw + gap;
            }
            SetFocus(g_edit);
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

LRESULT CALLBACK PopupProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_CREATE: {
            g_font = CreateFontW(
                -18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

            g_tools = CreateWindowExW(0, L"PBQA_Tools", L"", WS_CHILD | WS_VISIBLE, 13, 15, 47, 39, hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
            g_edit = CreateWindowExW(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 72, 16, 628, 38, hwnd, reinterpret_cast<HMENU>(1001), GetModuleHandleW(nullptr), nullptr);
            g_buttons = CreateWindowExW(0, L"PBQA_Buttons", L"", WS_CHILD | WS_VISIBLE, 13, 64, 696, 44, hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
            g_results = CreateWindowExW(0, L"PBQA_Results", L"", WS_CHILD | WS_VISIBLE, 13, 116, 696, 310, hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
            g_settings = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, L"PBQA_Settings", L"", WS_POPUP, 0, 0, 330, 174, hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);

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
            MoveWindow(g_tools, 13, 15, 47, 39, TRUE);
            MoveWindow(g_edit, 72, 16, std::max(160, w - 96), 38, TRUE);
            MoveWindow(g_buttons, 13, 64, std::max(220, w - 26), 44, TRUE);
            MoveWindow(g_results, 13, 116, std::max(220, w - 26), std::max(110, h - 161), TRUE);
            if (g_settings) {
                RECT window_rc = {};
                GetWindowRect(g_hwnd, &window_rc);
                SetWindowPos(g_settings, nullptr, window_rc.left + 13, window_rc.top + 55, 330, 174, SWP_NOZORDER);
            }
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

    registered = true;
}

} // namespace

void PBQA_SetItems(std::vector<PBQA_ResultItem> items) {
    LoadSettings();
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

    if (!g_hwnd) {
        g_hwnd = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
            kWindowClass,
            L"Quick Apply",
            WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
            CW_USEDEFAULT, CW_USEDEFAULT, 724, 540,
            nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    }

    RECT work = {};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);
    int w = 724;
    int h = 540;
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
        if (g_settings) {
            ShowWindow(g_settings, SW_HIDE);
        }
        SetFocus(g_edit);
    }
}
