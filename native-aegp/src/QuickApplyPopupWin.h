#pragma once

#include "AE_GeneralPlug.h"

#include <functional>
#include <string>
#include <vector>

struct PBQA_ResultItem {
    std::wstring name;
    std::wstring detail;
    std::wstring action_label;
    std::wstring kind;
    std::wstring payload;
    AEGP_InstalledEffectKey effect_key = AEGP_InstalledEffectKey_NONE;
    bool favorite = false;
};

using PBQA_ApplyCallback = std::function<void(const PBQA_ResultItem&)>;

void PBQA_SetItems(std::vector<PBQA_ResultItem> items);
void PBQA_SetApplyCallback(PBQA_ApplyCallback callback);
void PBQA_ShowPopupWindow();
