/*
    PersonalBar.jsx
    A small dockable After Effects ScriptUI panel for custom production buttons.

    Install:
    Copy this file to:
    Adobe After Effects <version>/Support Files/Scripts/ScriptUI Panels/

    Then open it from:
    Window > PersonalBar.jsx
*/

(function personalBar(thisObj) {
    var SCRIPT_NAME = "PersonalBar";

    function isCompActive() {
        return app.project && app.project.activeItem && app.project.activeItem instanceof CompItem;
    }

    function getActiveComp() {
        if (!isCompActive()) {
            alert("Open or select a composition first.");
            return null;
        }

        return app.project.activeItem;
    }

    function getTrimSourceLayer(comp) {
        if (!comp || comp.selectedLayers.length < 1) {
            return null;
        }

        return comp.selectedLayers[0];
    }

    function applyTimingFromLayer(targetLayer, sourceLayer) {
        if (!targetLayer || !sourceLayer) {
            return;
        }

        targetLayer.startTime = sourceLayer.startTime;
        targetLayer.inPoint = sourceLayer.inPoint;
        targetLayer.outPoint = sourceLayer.outPoint;
    }

    function centerOfComp(comp) {
        return [comp.width / 2, comp.height / 2];
    }

    function transformProp(layer, matchName) {
        return layer.property("ADBE Transform Group").property(matchName);
    }

    function deselectCompLayers(comp) {
        for (var i = 1; i <= comp.numLayers; i++) {
            comp.layer(i).selected = false;
        }
    }

    function makeDefaultOrbitCamera() {
        var comp = getActiveComp();
        if (!comp) {
            return;
        }

        app.beginUndoGroup("Create Orbit Camera");

        try {
            var trimLayer = getTrimSourceLayer(comp);
            var cam = comp.layers.addCamera("Camera", centerOfComp(comp));
            var orbitNull = comp.layers.addNull(comp.duration);

            orbitNull.name = "Orbit Null";
            orbitNull.threeDLayer = true;
            orbitNull.label = 10;

            // After Effects creates a 2-node camera by default. Keep its point of
            // interest centered, then parent it to a centered null for orbiting.
            cam.threeDLayer = true;
            cam.label = 12;
            cam.parent = orbitNull;

            transformProp(orbitNull, "ADBE Position").setValue([comp.width / 2, comp.height / 2, 0]);
            transformProp(cam, "ADBE Position").setValue([0, 0, -2666.6667]);

            if (transformProp(cam, "ADBE Point of Interest")) {
                transformProp(cam, "ADBE Point of Interest").setValue([0, 0, 0]);
            }

            if (trimLayer) {
                applyTimingFromLayer(cam, trimLayer);
                applyTimingFromLayer(orbitNull, trimLayer);
            }

            comp.hideShyLayers = false;
            deselectCompLayers(comp);
            orbitNull.selected = true;
            cam.selected = true;
        } catch (err) {
            alert("Could not create orbit camera:\n" + err.toString());
        } finally {
            app.endUndoGroup();
        }
    }

    function deselectProjectItems() {
        if (!app.project) {
            return;
        }

        for (var i = 1; i <= app.project.numItems; i++) {
            app.project.item(i).selected = false;
        }
    }

    function revealLayerSourceManual(layer) {
        if (!layer || !layer.source) {
            alert("The selected layer does not have a project source.");
            return false;
        }

        app.project.showWindow(true);
        deselectProjectItems();
        layer.source.selected = true;
        return true;
    }

    function revealLayerSource() {
        var comp = getActiveComp();
        if (!comp) {
            return;
        }

        if (comp.selectedLayers.length < 1) {
            alert("Select a layer with a source first.");
            return;
        }

        app.beginUndoGroup("Reveal Layer Source in Project");

        try {
            var commandId = app.findMenuCommandId("Reveal Layer Source in Project");
            if (commandId) {
                app.executeCommand(commandId);
            } else {
                revealLayerSourceManual(comp.selectedLayers[0]);
            }
        } catch (err) {
            revealLayerSourceManual(comp.selectedLayers[0]);
        } finally {
            app.endUndoGroup();
        }
    }

    function button(parent, text, helpTip, onClick) {
        var b = parent.add("button", undefined, text);
        b.helpTip = helpTip;
        b.preferredSize = [118, 28];
        b.onClick = onClick;
        return b;
    }

    function buildUI(thisObj) {
        var pal = (thisObj instanceof Panel)
            ? thisObj
            : new Window("palette", SCRIPT_NAME, undefined, { resizeable: true });

        pal.orientation = "row";
        pal.alignChildren = ["fill", "top"];
        pal.spacing = 4;
        pal.margins = 6;

        button(
            pal,
            "Orbit Cam",
            "Create a camera parented to an Orbit Null. If a layer is selected, match its in/out range.",
            makeDefaultOrbitCamera
        );

        button(
            pal,
            "Reveal Src",
            "Reveal the selected layer source in the Project panel.",
            revealLayerSource
        );

        pal.layout.layout(true);
        pal.layout.resize();
        pal.onResizing = pal.onResize = function () {
            this.layout.resize();
        };

        return pal;
    }

    var panel = buildUI(thisObj);
    if (panel instanceof Window) {
        panel.center();
        panel.show();
    }
})(this);
