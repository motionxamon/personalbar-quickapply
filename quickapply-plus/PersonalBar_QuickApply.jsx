#target aftereffects
#targetengine "personalbar_quickapply"

(function quickApplyPlus() {
    var SCRIPT_NAME = "QuickApply+";
    var MAX_RESULTS = 80;

    var COMMON_MENU_COMMANDS = [
        "Quick Apply",
        "New Composition...",
        "Composition Settings...",
        "Add to Render Queue",
        "Add to Adobe Media Encoder Queue...",
        "New Solid...",
        "New Null Object",
        "New Adjustment Layer",
        "New Camera...",
        "New Light...",
        "Pre-compose...",
        "Create Shapes from Text",
        "Create Masks from Text",
        "Auto-trace...",
        "Reduce Project",
        "Collect Files...",
        "Reveal Layer Source in Project",
        "Reveal in Explorer",
        "Interpret Footage > Main...",
        "Replace Footage > File...",
        "Reload Footage",
        "Purge > All Memory & Disk Cache...",
        "Keyboard Shortcuts..."
    ];

    var state = {
        items: [],
        filtered: [],
        win: null,
        search: null,
        list: null,
        status: null
    };

    function initItems() {
        state.items = [];
        collectEffects();
        collectPresets();
        collectMenuCommands();
    }

    function collectEffects() {
        if (!app.effects) {
            return;
        }

        for (var i = 0; i < app.effects.length; i++) {
            var fx = app.effects[i];
            state.items.push({
                kind: "effect",
                name: fx.displayName || fx.matchName || ("Effect " + i),
                detail: fx.category || "Effect",
                value: fx.matchName || fx.displayName
            });
        }
    }

    function collectPresets() {
        var roots = presetRoots();
        var seen = {};

        for (var i = 0; i < roots.length; i++) {
            scanPresetFolder(roots[i], seen);
        }
    }

    function presetRoots() {
        var roots = [];

        try {
            var appFolder = new Folder(app.path);
            roots.push(new Folder(appFolder.fsName + "/Presets"));
        } catch (err1) {
        }

        try {
            roots.push(new Folder(Folder.myDocuments.fsName + "/Adobe/After Effects/User Presets"));
        } catch (err2) {
        }

        return roots;
    }

    function scanPresetFolder(folder, seen) {
        if (!folder || !folder.exists) {
            return;
        }

        var files = folder.getFiles();
        for (var i = 0; i < files.length; i++) {
            var f = files[i];
            if (f instanceof Folder) {
                scanPresetFolder(f, seen);
            } else if (/\.ffx$/i.test(f.name) && !seen[f.fsName]) {
                seen[f.fsName] = true;
                state.items.push({
                    kind: "preset",
                    name: decodeName(f.displayName || f.name.replace(/\.ffx$/i, "")),
                    detail: "Animation Preset",
                    value: f.fsName
                });
            }
        }
    }

    function collectMenuCommands() {
        for (var i = 0; i < COMMON_MENU_COMMANDS.length; i++) {
            var name = COMMON_MENU_COMMANDS[i];
            state.items.push({
                kind: "menu",
                name: name,
                detail: "Menu Command",
                value: name
            });
        }
    }

    function decodeName(value) {
        try {
            return decodeURI(value);
        } catch (err) {
            return value;
        }
    }

    function ensureWindow() {
        if (state.win) {
            return state.win;
        }

        var win = new Window("palette", SCRIPT_NAME, undefined, { resizeable: true });
        win.orientation = "column";
        win.alignChildren = ["fill", "top"];
        win.spacing = 6;
        win.margins = 10;

        state.search = win.add("edittext", undefined, "");
        state.search.characters = 42;
        state.search.active = true;

        state.list = win.add("listbox", undefined, [], {
            numberOfColumns: 3,
            showHeaders: true,
            columnTitles: ["Name", "Type", "Info"],
            columnWidths: [260, 80, 180]
        });
        state.list.preferredSize = [560, 320];

        state.status = win.add("statictext", undefined, "Type to search. Enter applies. Ctrl+Enter makes a solid, Ctrl+Alt+Enter makes an adjustment layer.");
        state.status.characters = 80;

        state.search.onChanging = function () {
            filterItems(state.search.text);
        };

        state.search.addEventListener("keydown", function (event) {
            if (event.keyName === "Down") {
                moveSelection(1);
                event.preventDefault();
            } else if (event.keyName === "Up") {
                moveSelection(-1);
                event.preventDefault();
            } else if (event.keyName === "Enter") {
                applySelected(event);
                event.preventDefault();
            } else if (event.keyName === "Escape") {
                state.win.hide();
                event.preventDefault();
            }
        });

        state.list.onDoubleClick = function () {
            applySelected({});
        };

        win.onResizing = win.onResize = function () {
            this.layout.resize();
        };

        state.win = win;
        return win;
    }

    function showLauncher() {
        initItems();
        ensureWindow();
        state.search.text = "";
        filterItems("");
        state.win.center();
        state.win.show();
        state.search.active = true;
    }

    function filterItems(query) {
        var parsed = parseQuery(query);
        var q = parsed.text.toLowerCase();
        state.filtered = [];
        state.list.removeAll();

        for (var i = 0; i < state.items.length && state.filtered.length < MAX_RESULTS; i++) {
            var item = state.items[i];
            if (parsed.kind && item.kind !== parsed.kind) {
                continue;
            }

            var hay = (item.name + " " + item.detail + " " + item.value).toLowerCase();
            if (!q || hay.indexOf(q) !== -1) {
                state.filtered.push(item);
                addListItem(item);
            }
        }

        if (state.list.items.length > 0) {
            state.list.selection = 0;
        }

        state.status.text = state.filtered.length + " result(s). Prefixes: e: effects, p: presets, m: menu commands.";
    }

    function parseQuery(query) {
        query = query || "";
        var match = query.match(/^\s*([epm]):\s*(.*)$/i);
        if (!match) {
            return { kind: "", text: query };
        }

        var kind = match[1].toLowerCase();
        return {
            kind: kind === "e" ? "effect" : kind === "p" ? "preset" : "menu",
            text: match[2]
        };
    }

    function addListItem(item) {
        var row = state.list.add("item", item.name);
        row.subItems[0].text = item.kind;
        row.subItems[1].text = item.detail;
    }

    function moveSelection(delta) {
        if (!state.list.items.length) {
            return;
        }

        var index = state.list.selection ? state.list.selection.index : 0;
        index += delta;
        if (index < 0) {
            index = 0;
        }
        if (index >= state.list.items.length) {
            index = state.list.items.length - 1;
        }
        state.list.selection = index;
    }

    function applySelected(event) {
        if (!state.list.selection) {
            return;
        }

        var item = state.filtered[state.list.selection.index];
        if (!item) {
            return;
        }

        var mode = event && event.ctrlKey && event.altKey ? "adjustment" : event && event.ctrlKey ? "solid" : "selected";
        applyItem(item, mode);
    }

    function applyItem(item, mode) {
        if (item.kind === "effect") {
            applyEffect(item, mode);
        } else if (item.kind === "preset") {
            applyPreset(item, mode);
        } else if (item.kind === "menu") {
            runMenuCommand(item);
        }
    }

    function activeCompOrAlert() {
        if (!app.project || !(app.project.activeItem instanceof CompItem)) {
            alert("Open or select a composition first.");
            return null;
        }
        return app.project.activeItem;
    }

    function targetLayers(mode) {
        var comp = activeCompOrAlert();
        if (!comp) {
            return null;
        }

        if (mode === "solid" || (mode === "selected" && comp.selectedLayers.length < 1)) {
            var solid = comp.layers.addSolid([1, 1, 1], "Quick Apply Solid", comp.width, comp.height, comp.pixelAspect, comp.duration);
            return [solid];
        }

        if (mode === "adjustment") {
            var adj = comp.layers.addSolid([1, 1, 1], "Quick Apply Adjustment", comp.width, comp.height, comp.pixelAspect, comp.duration);
            adj.adjustmentLayer = true;
            return [adj];
        }

        return comp.selectedLayers;
    }

    function applyEffect(item, mode) {
        var layers = targetLayers(mode);
        if (!layers) {
            return;
        }

        app.beginUndoGroup("QuickApply+ Effect");
        try {
            for (var i = 0; i < layers.length; i++) {
                layers[i].property("ADBE Effect Parade").addProperty(item.value);
            }
            state.status.text = "Applied effect: " + item.name;
        } catch (err) {
            alert("Could not apply effect:\n" + item.name + "\n\n" + err.toString());
        } finally {
            app.endUndoGroup();
        }
    }

    function applyPreset(item, mode) {
        var layers = targetLayers(mode);
        if (!layers) {
            return;
        }

        var file = new File(item.value);
        if (!file.exists) {
            alert("Preset file was not found:\n" + item.value);
            return;
        }

        app.beginUndoGroup("QuickApply+ Preset");
        try {
            for (var i = 0; i < layers.length; i++) {
                layers[i].selected = true;
            }
            layers[0].applyPreset(file);
            state.status.text = "Applied preset: " + item.name;
        } catch (err) {
            alert("Could not apply preset:\n" + item.name + "\n\n" + err.toString());
        } finally {
            app.endUndoGroup();
        }
    }

    function runMenuCommand(item) {
        var commandId = String(item.value).match(/^\d+$/) ? parseInt(item.value, 10) : app.findMenuCommandId(item.value);
        if (!commandId) {
            alert("Menu command was not found:\n" + item.value);
            return;
        }
        app.executeCommand(commandId);
        state.status.text = "Ran command: " + item.name;
    }

    showLauncher();
})();
