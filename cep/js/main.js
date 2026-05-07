(function () {
    "use strict";

    var activeKey = "personalbar.activeToolbar";
    var configKey = "personalbar.userConfig.v1";
    var panel = document.getElementById("panel");
    var picker = document.getElementById("toolbarPicker");
    var toolbarName = document.getElementById("toolbarName");
    var toolbarMenu = document.getElementById("toolbarMenu");
    var grid = document.getElementById("buttonGrid");
    var statusEl = document.getElementById("status");
    var reloadBtn = document.getElementById("reloadBtn");
    var searchBtn = document.getElementById("searchBtn");
    var settingsBtn = document.getElementById("settingsBtn");
    var searchOverlay = document.getElementById("searchOverlay");
    var quickInput = document.getElementById("quickInput");
    var quickResults = document.getElementById("quickResults");
    var settingsOverlay = document.getElementById("settingsOverlay");
    var settingsTitle = document.getElementById("settingsTitle");
    var closeSettingsBtn = document.getElementById("closeSettingsBtn");
    var settingsList = document.getElementById("settingsList");
    var addButtonBtn = document.getElementById("addButtonBtn");
    var buttonOverlay = document.getElementById("buttonOverlay");
    var closeButtonBtn = document.getElementById("closeButtonBtn");
    var saveButtonBtn = document.getElementById("saveButtonBtn");
    var newName = document.getElementById("newName");
    var newLabel = document.getElementById("newLabel");
    var newColor = document.getElementById("newColor");
    var newKind = document.getElementById("newKind");
    var newValue = document.getElementById("newValue");
    var data = window.KBAR_IMPORT || { toolbars: [] };
    var userConfig = loadUserConfig();
    var activeToolbarId = localStorage.getItem(activeKey) || (data.toolbars[0] && data.toolbars[0].id);

    addPersonalButtons();

    function defaultUserConfig() {
        return { custom: {}, hidden: {}, order: {} };
    }

    function loadUserConfig() {
        try {
            var saved = localStorage.getItem(configKey);
            return saved ? normalizeUserConfig(JSON.parse(saved)) : defaultUserConfig();
        } catch (err) {
            return defaultUserConfig();
        }
    }

    function normalizeUserConfig(value) {
        value = value || {};
        value.custom = value.custom || {};
        value.hidden = value.hidden || {};
        value.order = value.order || {};
        return value;
    }

    function saveUserConfig() {
        localStorage.setItem(configKey, JSON.stringify(userConfig));
    }

    function setStatus(message) {
        statusEl.textContent = message || "Ready";
    }

    function escapeForEvalScript(value) {
        return String(value)
            .replace(/\\/g, "\\\\")
            .replace(/'/g, "\\'")
            .replace(/\r/g, "\\r")
            .replace(/\n/g, "\\n");
    }

    function jsString(value) {
        return "'" + escapeForEvalScript(value) + "'";
    }

    function evalScript(script, callback) {
        if (!window.__adobe_cep__ || !window.__adobe_cep__.evalScript) {
            setStatus("CEP bridge is unavailable");
            return;
        }
        window.__adobe_cep__.evalScript(script, callback || function () {});
    }

    function extensionPath() {
        if (!window.__adobe_cep__ || !window.__adobe_cep__.getSystemPath) {
            return "";
        }
        return window.__adobe_cep__.getSystemPath("extension");
    }

    function loadHostScript() {
        var path = extensionPath() + "/jsx/host.jsx";
        evalScript("$.evalFile(" + jsString(path) + ")", function (result) {
            setStatus(result && result !== "undefined" ? result : "Ready");
        });
    }

    function activeToolbar() {
        for (var i = 0; i < data.toolbars.length; i++) {
            if (data.toolbars[i].id === activeToolbarId) return data.toolbars[i];
        }
        activeToolbarId = data.toolbars[0] && data.toolbars[0].id;
        return data.toolbars[0];
    }

    function addPersonalButtons() {
        var working = toolbarByName("Working");
        if (!working) return;

        var personal = [
            {
                id: "personal-orbit-camera",
                name: "Orbit Camera",
                description: "Create camera with Orbit Null, trimmed to selected layer when possible",
                label: "CAM\nORBIT",
                color: "rgba(126, 255, 54, 1)",
                iconType: "text",
                svg: "",
                sourceType: "personal",
                action: { kind: "builtIn", value: "createOrbitCamera" }
            },
            {
                id: "personal-reveal-source",
                name: "Reveal Layer Source",
                description: "Reveal selected layer source in the Project panel",
                label: "SRC\nFIND",
                color: "rgba(80, 227, 194, 1)",
                iconType: "text",
                svg: "",
                sourceType: "personal",
                action: { kind: "builtIn", value: "revealLayerSource" }
            }
        ];

        for (var p = personal.length - 1; p >= 0; p--) {
            if (!hasButton(working.buttons, personal[p].id)) working.buttons.unshift(personal[p]);
        }
    }

    function toolbarByName(name) {
        for (var i = 0; i < data.toolbars.length; i++) {
            if (data.toolbars[i].name === name) return data.toolbars[i];
        }
        return null;
    }

    function hasButton(buttons, id) {
        for (var i = 0; i < buttons.length; i++) {
            if (buttons[i].id === id) return true;
        }
        return false;
    }

    function allButtons(toolbar) {
        var custom = userConfig.custom[toolbar.id] || [];
        return toolbar.buttons.concat(custom);
    }

    function visibleButtons(toolbar) {
        var hidden = userConfig.hidden[toolbar.id] || [];
        var buttons = allButtons(toolbar).filter(function (button) {
            return hidden.indexOf(button.id) < 0;
        });
        return sortButtons(toolbar.id, buttons);
    }

    function sortButtons(toolbarId, buttons) {
        var order = userConfig.order[toolbarId] || [];
        var map = {};
        for (var i = 0; i < buttons.length; i++) map[buttons[i].id] = buttons[i];
        var sorted = [];
        for (var o = 0; o < order.length; o++) {
            if (map[order[o]]) {
                sorted.push(map[order[o]]);
                delete map[order[o]];
            }
        }
        for (var b = 0; b < buttons.length; b++) {
            if (map[buttons[b].id]) sorted.push(buttons[b]);
        }
        return sorted;
    }

    function persistCurrentOrder(ids) {
        userConfig.order[activeToolbarId] = ids;
        saveUserConfig();
    }

    function render() {
        renderMenu();
        renderButtons();
        renderSettings();
    }

    function renderMenu() {
        var toolbar = activeToolbar();
        toolbarName.textContent = toolbar ? toolbar.name : "No toolbar";
        toolbarMenu.innerHTML = "";
        for (var i = 0; i < data.toolbars.length; i++) {
            var item = document.createElement("button");
            item.className = "toolbar-menu-item" + (data.toolbars[i].id === activeToolbarId ? " active" : "");
            item.setAttribute("data-toolbar-id", data.toolbars[i].id);
            item.innerHTML = "<span>" + escapeHtml(data.toolbars[i].name) + "</span><em>" + allButtons(data.toolbars[i]).length + "</em>";
            toolbarMenu.appendChild(item);
        }
    }

    function renderButtons() {
        var toolbar = activeToolbar();
        var buttons = toolbar ? visibleButtons(toolbar) : [];
        grid.innerHTML = "";
        if (!buttons.length) {
            var empty = document.createElement("div");
            empty.className = "empty-state";
            empty.textContent = "This toolbar is empty";
            grid.appendChild(empty);
            return;
        }
        for (var i = 0; i < buttons.length; i++) grid.appendChild(renderButton(buttons[i]));
    }

    function renderButton(button) {
        var el = document.createElement("button");
        el.className = "tool " + actionClass(button.action.kind);
        el.setAttribute("data-button-id", button.id);
        el.title = tooltip(button);
        var icon = document.createElement("span");
        icon.className = "tool-icon";
        icon.style.color = button.color || "#b6b6b6";
        if (button.iconType === "svg" && button.svg) {
            icon.className += " svg-icon";
            icon.style.backgroundImage = "url(\"" + button.svg.replace(/"/g, "%22") + "\")";
        } else {
            icon.innerHTML = labelHtml(button.label);
        }
        el.appendChild(icon);
        return el;
    }

    function actionClass(kind) {
        return kind === "unsupported" ? "disabled-action" : "";
    }

    function tooltip(button) {
        var kind = button.action.kind;
        var suffix = "";
        if (kind === "externalScript") suffix = "Run script file: " + button.action.value;
        else if (kind === "presetExternal") suffix = "Apply preset: " + button.action.value;
        else if (kind === "scriptlet") suffix = "Inline jsx/scriptlet";
        else if (kind === "builtIn") suffix = "PersonalBar built-in";
        else if (kind === "expression") suffix = "Expression";
        else if (kind === "menuCommand") suffix = "Menu item: " + button.action.value;
        else if (kind === "extension") suffix = "Extension: " + button.action.value;
        return button.name + (suffix ? " - " + suffix : "");
    }

    function labelHtml(label) {
        label = String(label || "");
        if (label.length <= 3 && label.indexOf(" ") < 0) return escapeHtml(label);
        var normalized = label.replace(/-/g, "-\n").replace(/\s+/g, "\n");
        var parts = normalized.split(/\n+/).filter(Boolean).slice(0, 3);
        var html = "";
        for (var i = 0; i < parts.length; i++) html += "<span>" + escapeHtml(parts[i]) + "</span>";
        return html || escapeHtml(label);
    }

    function escapeHtml(value) {
        return String(value).replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;").replace(/"/g, "&quot;");
    }

    function buttonById(id) {
        var toolbar = activeToolbar();
        if (!toolbar) return null;
        var buttons = allButtons(toolbar);
        for (var i = 0; i < buttons.length; i++) {
            if (buttons[i].id === id) return buttons[i];
        }
        return null;
    }

    function runButton(button) {
        if (!button) return;
        var action = button.action;
        if (action.kind === "externalScript") return runHost("PersonalBarHost.runScriptFile(" + jsString(action.value) + ")");
        if (action.kind === "presetExternal") return runHost("PersonalBarHost.applyPreset(" + jsString(action.value) + ")");
        if (action.kind === "scriptlet") return runHost("PersonalBarHost.runScriptlet(" + jsString(button.name) + "," + jsString(action.value) + ")");
        if (action.kind === "builtIn") return runHost("PersonalBarHost." + action.value + "()");
        if (action.kind === "expression") return runHost("PersonalBarHost.setExpression(" + jsString(action.value) + ")");
        if (action.kind === "menuCommand") return runHost("PersonalBarHost.invokeMenuCommand(" + jsString(action.value) + ")");
        if (action.kind === "extension") return openExtension(action.value, button.name);
        setStatus(button.name + ": unsupported action");
    }

    function runHost(script) {
        setStatus("Running...");
        evalScript(script, function (result) {
            setStatus(result && result !== "undefined" ? result : "Done");
        });
    }

    function openExtension(extensionId, buttonName) {
        if (!window.__adobe_cep__ || !window.__adobe_cep__.requestOpenExtension) {
            setStatus(buttonName + ": extension bridge unavailable");
            return;
        }
        window.__adobe_cep__.requestOpenExtension(extensionId, "");
        setStatus("Opening " + buttonName);
    }

    function renderSettings() {
        var toolbar = activeToolbar();
        var buttons = toolbar ? sortButtons(toolbar.id, allButtons(toolbar)) : [];
        var hidden = toolbar ? (userConfig.hidden[toolbar.id] || []) : [];
        settingsTitle.textContent = toolbar ? toolbar.name : "Settings";
        settingsList.innerHTML = "";
        for (var i = 0; i < buttons.length; i++) settingsList.appendChild(settingsRow(buttons[i], i, buttons, hidden));
    }

    function settingsRow(button, index, buttons, hidden) {
        var row = document.createElement("div");
        row.className = "settings-row";
        row.innerHTML =
            "<label><input type=\"checkbox\" " + (hidden.indexOf(button.id) < 0 ? "checked" : "") + "> <span>" +
            escapeHtml(button.name) + "</span><em>" + escapeHtml(button.action.kind) + "</em></label>" +
            "<button data-move=\"up\">Up</button><button data-move=\"down\">Down</button>" +
            (button.id.indexOf("custom-") === 0 ? "<button data-delete=\"1\">Delete</button>" : "");

        row.querySelector("input").addEventListener("change", function (event) {
            setHidden(button.id, !event.target.checked);
        });
        row.querySelector("[data-move='up']").addEventListener("click", function () {
            moveButton(buttons, index, -1);
        });
        row.querySelector("[data-move='down']").addEventListener("click", function () {
            moveButton(buttons, index, 1);
        });
        var del = row.querySelector("[data-delete]");
        if (del) del.addEventListener("click", function () { deleteCustom(button.id); });
        return row;
    }

    function setHidden(id, hide) {
        var arr = userConfig.hidden[activeToolbarId] || [];
        if (hide && arr.indexOf(id) < 0) arr.push(id);
        if (!hide) arr = arr.filter(function (item) { return item !== id; });
        userConfig.hidden[activeToolbarId] = arr;
        saveUserConfig();
        render();
    }

    function moveButton(buttons, index, direction) {
        var next = index + direction;
        if (next < 0 || next >= buttons.length) return;
        var copy = buttons.slice();
        var moved = copy.splice(index, 1)[0];
        copy.splice(next, 0, moved);
        persistCurrentOrder(copy.map(function (button) { return button.id; }));
        render();
    }

    function deleteCustom(id) {
        var arr = userConfig.custom[activeToolbarId] || [];
        userConfig.custom[activeToolbarId] = arr.filter(function (button) { return button.id !== id; });
        saveUserConfig();
        render();
    }

    function createButton() {
        var toolbar = activeToolbar();
        if (!toolbar) return;
        var button = {
            id: "custom-" + Date.now().toString(36),
            name: newName.value || "New Button",
            description: "",
            label: newLabel.value || "NEW",
            color: newColor.value || "rgba(80, 227, 194, 1)",
            iconType: "text",
            svg: "",
            sourceType: "custom",
            action: { kind: newKind.value, value: newValue.value || "" }
        };
        var custom = userConfig.custom[toolbar.id] || [];
        custom.push(button);
        userConfig.custom[toolbar.id] = custom;
        var order = userConfig.order[toolbar.id] || visibleButtons(toolbar).map(function (item) { return item.id; });
        order.push(button.id);
        userConfig.order[toolbar.id] = order;
        saveUserConfig();
        hide(buttonOverlay);
        render();
    }

    function openSearch() {
        show(searchOverlay);
        quickInput.value = "";
        renderSearch("");
        setTimeout(function () { quickInput.focus(); }, 0);
    }

    function renderSearch(query) {
        query = String(query || "").toLowerCase();
        quickResults.innerHTML = "";
        var matches = [];
        for (var t = 0; t < data.toolbars.length; t++) {
            var buttons = visibleButtons(data.toolbars[t]);
            for (var b = 0; b < buttons.length; b++) {
                var hay = (buttons[b].name + " " + buttons[b].label + " " + buttons[b].action.kind + " " + buttons[b].action.value).toLowerCase();
                if (!query || hay.indexOf(query) >= 0) matches.push({ toolbar: data.toolbars[t], button: buttons[b] });
            }
        }
        matches = matches.slice(0, 36);
        for (var i = 0; i < matches.length; i++) quickResults.appendChild(searchRow(matches[i], i === 0));
    }

    function searchRow(match, active) {
        var row = document.createElement("button");
        row.className = "quick-row" + (active ? " active" : "");
        row.setAttribute("data-button-id", match.button.id);
        row.setAttribute("data-toolbar-id", match.toolbar.id);
        row.innerHTML = "<span>" + escapeHtml(match.button.name) + "</span><em>" + escapeHtml(match.toolbar.name) + " / " + escapeHtml(match.button.action.kind) + "</em>";
        return row;
    }

    function show(el) { el.classList.remove("hidden"); }
    function hide(el) { el.classList.add("hidden"); }

    function toggleMenu(force) {
        var shouldShow = typeof force === "boolean" ? force : toolbarMenu.classList.contains("hidden");
        toolbarMenu.classList.toggle("hidden", !shouldShow);
        picker.classList.toggle("open", shouldShow);
    }

    picker.addEventListener("click", function () { toggleMenu(); });
    panel.addEventListener("contextmenu", function (event) { event.preventDefault(); toggleMenu(true); });
    reloadBtn.addEventListener("click", loadHostScript);
    searchBtn.addEventListener("click", openSearch);
    settingsBtn.addEventListener("click", function () { renderSettings(); show(settingsOverlay); });
    closeSettingsBtn.addEventListener("click", function () { hide(settingsOverlay); });
    addButtonBtn.addEventListener("click", function () { show(buttonOverlay); newName.focus(); });
    closeButtonBtn.addEventListener("click", function () { hide(buttonOverlay); });
    saveButtonBtn.addEventListener("click", createButton);
    quickInput.addEventListener("input", function () { renderSearch(quickInput.value); });

    toolbarMenu.addEventListener("click", function (event) {
        var target = event.target;
        while (target && target !== toolbarMenu && !target.getAttribute("data-toolbar-id")) target = target.parentNode;
        if (!target || target === toolbarMenu) return;
        activeToolbarId = target.getAttribute("data-toolbar-id");
        localStorage.setItem(activeKey, activeToolbarId);
        toggleMenu(false);
        render();
    });

    grid.addEventListener("click", function (event) {
        var target = event.target;
        while (target && target !== grid && !target.getAttribute("data-button-id")) target = target.parentNode;
        if (!target || target === grid) return;
        runButton(buttonById(target.getAttribute("data-button-id")));
    });

    quickResults.addEventListener("click", function (event) {
        var target = event.target;
        while (target && target !== quickResults && !target.getAttribute("data-button-id")) target = target.parentNode;
        if (!target || target === quickResults) return;
        activeToolbarId = target.getAttribute("data-toolbar-id");
        localStorage.setItem(activeKey, activeToolbarId);
        hide(searchOverlay);
        render();
        runButton(buttonById(target.getAttribute("data-button-id")));
    });

    document.addEventListener("click", function (event) {
        if (!toolbarMenu.contains(event.target) && !picker.contains(event.target)) toggleMenu(false);
    });

    document.addEventListener("keydown", function (event) {
        if ((event.ctrlKey || event.metaKey) && (event.key === "k" || event.code === "Space")) {
            event.preventDefault();
            openSearch();
            return;
        }
        if (event.key === "Escape") {
            hide(searchOverlay);
            hide(settingsOverlay);
            hide(buttonOverlay);
            toggleMenu(false);
        }
        if (event.key === "Enter" && !searchOverlay.classList.contains("hidden")) {
            var first = quickResults.querySelector(".quick-row");
            if (first) first.click();
        }
    });

    loadHostScript();
    render();
})();
