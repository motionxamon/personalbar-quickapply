var PersonalBarHost = PersonalBarHost || {};

(function () {
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

    function selectedLayersOrAlert() {
        var comp = getActiveComp();
        if (!comp) {
            return null;
        }

        if (comp.selectedLayers.length < 1) {
            alert("Select at least one layer first.");
            return null;
        }

        return comp.selectedLayers;
    }

    function selectedPropertiesOrAlert() {
        var comp = getActiveComp();
        if (!comp) {
            return null;
        }

        if (comp.selectedProperties.length < 1) {
            alert("Select at least one property first.");
            return null;
        }

        return comp.selectedProperties;
    }

    PersonalBarHost.createOrbitCamera = function () {
        var comp = getActiveComp();
        if (!comp) {
            return "No active comp";
        }

        app.beginUndoGroup("Create Orbit Camera");

        try {
            var trimLayer = getTrimSourceLayer(comp);
            var cam = comp.layers.addCamera("Camera", centerOfComp(comp));
            var orbitNull = comp.layers.addNull(comp.duration);

            orbitNull.name = "Orbit Null";
            orbitNull.threeDLayer = true;
            orbitNull.label = 10;

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

            return trimLayer ? "Orbit camera trimmed to selected layer" : "Orbit camera created";
        } catch (err) {
            alert("Could not create orbit camera:\n" + err.toString());
            return "Create Orbit Camera failed";
        } finally {
            app.endUndoGroup();
        }
    };

    PersonalBarHost.revealLayerSource = function () {
        var comp = getActiveComp();
        if (!comp) {
            return "No active comp";
        }

        if (comp.selectedLayers.length < 1) {
            alert("Select a layer with a source first.");
            return "No layer selected";
        }

        app.beginUndoGroup("Reveal Layer Source in Project");

        try {
            var commandId = app.findMenuCommandId("Reveal Layer Source in Project");
            if (commandId) {
                app.executeCommand(commandId);
                return "Reveal command executed";
            }

            if (revealLayerSourceManual(comp.selectedLayers[0])) {
                return "Source selected in Project";
            }

            return "Reveal failed";
        } catch (err) {
            if (revealLayerSourceManual(comp.selectedLayers[0])) {
                return "Source selected in Project";
            }

            return "Reveal failed";
        } finally {
            app.endUndoGroup();
        }
    };

    PersonalBarHost.invokeMenuCommand = function (menuName) {
        if (!menuName) {
            return "Menu name is empty";
        }

        var commandId = String(menuName).match(/^\d+$/)
            ? parseInt(menuName, 10)
            : app.findMenuCommandId(menuName);
        if (!commandId) {
            alert("Menu command was not found:\n" + menuName);
            return "Menu command not found";
        }

        app.executeCommand(commandId);
        return "Menu command executed";
    };

    PersonalBarHost.runScriptlet = function (buttonName, scriptText) {
        if (!scriptText) {
            return "Scriptlet is empty";
        }

        try {
            eval(scriptText);
            return buttonName ? buttonName + " done" : "Scriptlet done";
        } catch (err) {
            alert("Could not run scriptlet" + (buttonName ? ":\n" + buttonName : "") + "\n\n" + err.toString());
            return "Scriptlet failed";
        }
    };

    PersonalBarHost.setExpression = function (expressionText) {
        var props = selectedPropertiesOrAlert();
        if (!props) {
            return "No properties selected";
        }

        app.beginUndoGroup("Set Expression");

        try {
            var count = 0;
            for (var i = 0; i < props.length; i++) {
                if (props[i].canSetExpression) {
                    props[i].expression = expressionText || "";
                    props[i].expressionEnabled = true;
                    count++;
                }
            }

            return count + " expression(s) set";
        } catch (err) {
            alert("Could not set expression:\n" + err.toString());
            return "Set expression failed";
        } finally {
            app.endUndoGroup();
        }
    };

    PersonalBarHost.applyEffect = function (effectMatchName) {
        var layers = selectedLayersOrAlert();
        if (!layers) {
            return "No layers selected";
        }

        if (!effectMatchName) {
            return "Effect matchName is empty";
        }

        app.beginUndoGroup("Apply Effect");

        try {
            var count = 0;
            for (var i = 0; i < layers.length; i++) {
                layers[i].property("ADBE Effect Parade").addProperty(effectMatchName);
                count++;
            }

            return count + " effect(s) applied";
        } catch (err) {
            alert("Could not apply effect. Use an AE effect matchName, for example ADBE Gaussian Blur 2.\n\n" + err.toString());
            return "Apply effect failed";
        } finally {
            app.endUndoGroup();
        }
    };

    PersonalBarHost.applyPreset = function (presetPath) {
        var layers = selectedLayersOrAlert();
        if (!layers) {
            return "No layers selected";
        }

        if (!presetPath) {
            return "Preset path is empty";
        }

        var file = new File(presetPath);
        if (!file.exists) {
            alert("Preset file was not found:\n" + presetPath);
            return "Preset file not found";
        }

        app.beginUndoGroup("Apply Preset");

        try {
            for (var i = 0; i < layers.length; i++) {
                layers[i].selected = true;
            }

            layers[0].applyPreset(file);
            return "Preset applied";
        } catch (err) {
            alert("Could not apply preset:\n" + err.toString());
            return "Apply preset failed";
        } finally {
            app.endUndoGroup();
        }
    };

    PersonalBarHost.runScriptFile = function (scriptPath) {
        if (!scriptPath) {
            return "Script path is empty";
        }

        var file = new File(scriptPath);
        if (!file.exists) {
            alert("Script file was not found:\n" + scriptPath);
            return "Script file not found";
        }

        try {
            $.evalFile(file);
            return "Script file executed";
        } catch (err) {
            alert("Could not run script file:\n" + err.toString());
            return "Run script failed";
        }
    };
})();
