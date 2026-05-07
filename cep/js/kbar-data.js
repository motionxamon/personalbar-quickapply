window.KBAR_IMPORT = {
  "source": "com.khanyu.kbar",
  "toolbars": [
    {
      "id": "45fa5721-eb68-45b9-977b-be9c36fda7b3",
      "name": "Working",
      "buttons": [
        {
          "id": "b2999dce-6d46-473e-af0c-967075584810",
          "name": "4K",
          "description": "",
          "label": "4K",
          "color": "rgba(252, 67, 88, 1)",
          "iconType": "text",
          "svg": "",
          "sourceType": 2,
          "action": {
            "kind": "scriptlet",
            "value": "app.beginUndoGroup(\"Create 4K Structure\");\n\n            var Folder1 = app.project.items.addFolder(\"Footages\");\n            var Folder2 = app.project.items.addFolder(\"Precomps\");\n            var Folder3 = app.project.items.addFolder(\"Scenes\");\n            var Folder4 = app.project.items.addFolder(\"Tr\");\n\n            var main= app.project.items.addComp(\"MAIN\", 3840, 2160, 1, 20, 30);\n             main.comment = \"Main Comp\";\n             main.parentFolder = Folder3;\n                      \n            var render= app.project.items.addComp(\" _RENDER_THIS\", 3840, 2160, 1, 20, 30);\n            render.comment = \"Render this\";  \n            \n            render.layers.add(main);\n             \n            var bg = main.layers.addSolid([0,0,0], \"BG\",  main.width, main.height, main.pixelAspect, main.duration);\n            bg.source.name =\"BG\";   \n            var bgcolor = bg.Effects.addProperty(\"ADBE Fill\");\n            bgcolor.name = \"BG Color\";\n            bgcolor.color.setValue([1,1,1]);\n            \n            var sol = main.layers.addSolid([1,0,0], \"Solid\",  main.width, main.height, main.pixelAspect, main.duration);\n             \n            var solid = main.layers.addSolid([1,1,1], \"CC\",  main.width, main.height, main.pixelAspect, main.duration);\n            solid.source.name =\"CC\";    \n            solid.adjustmentLayer = true;\n\nrender.openInViewer(); \nmain.openInViewer();\n\n    app.endUndoGroup;"
          }
        },
        {
          "id": "e9d1483c-110e-4612-b18c-5d0d95effb90",
          "name": "Pack Sctructure",
          "description": "",
          "label": "●●",
          "color": "rgba(248, 231, 28, 1)",
          "iconType": "symbol",
          "svg": "",
          "sourceType": 2,
          "action": {
            "kind": "scriptlet",
            "value": "app.beginUndoGroup(\"Create Structure\");\n                      \n            var render= app.project.items.addComp(\"Scene\", 3840, 2160, 1, 10, 30);\n      \n            var Folder1 = app.project.items.addFolder(\"_Assets (do not edit)\");\n            var Folder2 = app.project.items.addFolder(\"_Edit Text (edit here)\");\n\n            var Folder3 = app.project.items.addFolder(\"Footages\");\n                    Folder3.parentFolder = Folder1;\n            var Folder4 = app.project.items.addFolder(\"Precomps\");\n                    Folder4.parentFolder = Folder1;\n            var Folder5 = app.project.items.addFolder(\"Tr\");\n\nrender.openInViewer(); \n    \n    app.endUndoGroup;"
          }
        },
        {
          "id": "3fbfbf0e-7dcd-41b7-94b9-30bd3c803c4c",
          "name": "MA Structure",
          "description": "",
          "label": "MA",
          "color": "rgba(12, 209, 243, 1)",
          "iconType": "text",
          "svg": "",
          "sourceType": 2,
          "action": {
            "kind": "scriptlet",
            "value": "app.beginUndoGroup(\"Create MA Structure\");\n\n var Folder1 = app.project.items.addFolder(\"01. Edit Comps\");\n    Folder1.comment = \"Edit These\";\n \n    var subFolder1 = app.project.items.addFolder(\"Color\");\n    subFolder1.parentFolder = Folder1;\n    subFolder1.comment = \"Edit Color Here\";\n    var subFolder2 = app.project.items.addFolder(\"Logo\");\n    subFolder2.parentFolder = Folder1;\n    subFolder2.comment = \"Place Your Logo Here\";\n    var subFolder3 = app.project.items.addFolder(\"Text\");\n    subFolder3.parentFolder = Folder1;\n    subFolder3.comment = \"Edit Text Here\";\n    var subFolder4 = app.project.items.addFolder(\"Media\");\n    subFolder4.parentFolder = Folder1;\n    subFolder4.comment = \"Place Your Footages Here\";\n    \n    var Folder2 = app.project.items.addFolder(\"02. Final Comp\");\n    Folder2.comment = \"Render This\";\n    \n    var finalcomp= app.project.items.addComp(\"FINAL_RENDER\", 3840, 2160, 1, 20, 30);\n            finalcomp.parentFolder = Folder2;\n    \n    \n            var comp1= app.project.items.addComp(\"_EDIT_COLOR\", 3840, 2160, 1, 20, 30);\n            comp1.parentFolder = subFolder1;\n            \n                             comp1.layers.add(finalcomp);\n            \n            var solid = comp1.layers.addSolid([1,1,1], \"COLOR CONTROL\",  comp1.width, comp1.height, comp1.pixelAspect, comp1.duration);\n            solid.source.name =\"COLOR CONTROL\";    \n            solid.adjustmentLayer = true;\n            \n                                     ///CREATE COLOR CONTROLS\n             \n            var sl_color0 = solid.Effects.addProperty(\"ADBE Color Control\");\n            sl_color0.name = \"Text Color 1\";\n            sl_color0.color.setValue([1,1,1]);\n                        \n            var sl_colorel1 = solid.Effects.addProperty(\"ADBE Color Control\");\n            sl_colorel1.name = \"Elements Color 1\";\n            sl_colorel1.color.setValue([1,1,1]);\n            \n            var sl_colorel2 = solid.Effects.addProperty(\"ADBE Color Control\");\n            sl_colorel2.name = \"Elements Color 2\";\n            sl_colorel2.color.setValue([1,1,1]);\n\n            var sl_color1 = solid.Effects.addProperty(\"ADBE Color Control\");\n            sl_color1.name = \"BG Color 1\";\n            sl_color1.color.setValue([1,1,1]);\n\n                                       ///CREATE COMPS\n            \n            var comp2= app.project.items.addComp(\"LOGO\", 3840, 2160, 1, 20, 30);\n            comp2.parentFolder = subFolder2;\n            var comp3= app.project.items.addComp(\"Text 1\", 3840, 2160, 1, 20, 30);\n            comp3.parentFolder = subFolder3;\n            \n            var tag = comp3.layers.addText(\"Text\");\n            \n            var comp4= app.project.items.addComp(\"Media 1\", 3840, 2160, 1, 20, 30);\n            comp4.parentFolder = subFolder4;\n\n       ///CREATE OTHER FOLDERS\n    \n    var Folder3 = app.project.items.addFolder(\"03. Others\");\n    Folder3.comment = \"Ignore\";\n    \n    var subFolder2 = app.project.items.addFolder(\"Footages\");\n    subFolder2.parentFolder = Folder3;\n    \n    var subFolder3 = app.project.items.addFolder(\"Pre-comps\");\n    subFolder3.parentFolder = Folder3;\n    \n    var subFolderOther = app.project.items.addFolder(\"Other\");\n    subFolderOther.parentFolder = subFolder3;\n    \n    var subFolderScene = app.project.items.addFolder(\"Scenes\");\n    subFolderScene.parentFolder = subFolder3;\n    \n    var subFolderDesign = app.project.items.addFolder(\"Design Elements\");\n    subFolderDesign.parentFolder = subFolder3;\n    \n    var mainscn= app.project.items.addComp(\"MainScene\", 3840, 2160, 1, 20, 30);\n    mainscn.parentFolder = subFolderScene;\n            \n    var scn= app.project.items.addComp(\"Scene 1\", 3840, 2160, 1, 20, 30);\n    scn.parentFolder = subFolderScene;\n    \n    \n                        ///CREATE SOLID\n            \n            var bgsolid = scn.layers.addSolid([0,0,0], \"BG\",  scn.width, scn.height, scn.pixelAspect, scn.duration);\n            bgsolid.source.name =\"BG\";   \n            \n            var gr_ramp = bgsolid.Effects.addProperty(\"ADBE Ramp\");\n            gr_ramp.property(2).setValue([0.3,0.3,0.3]);\n            gr_ramp.property(4).setValue([0,0,0]);\n            gr_ramp.property(5).setValue(2);\n            \n                         scn.layers.add(comp3);\n                         scn.layers.add(comp2);\n            ///CREATE CAMERA\n            var w = scn.width /2 ;\n            var h = scn.height /2 ;\n            var newCamera = scn.layers.addCamera(\"Camera\",[w,h]);\n\n            var cam = newCamera.Transform.position.setValue([w,h,-2666.6667]);\n\n            var myNull = scn.layers.addNull();\n            myNull.threeDLayer = true;\n\n            myNull.name = \"Camera Control\";\n            scn.layer(\"Camera\").parent = myNull;\n\n            \n                         finalcomp.layers.add(mainscn);\n                         mainscn.layers.add(scn);\n\n                         \n                         \n\n    \nvar Folder4 = app.project.items.addFolder(\"Tr\");\n    Folder4.comment = \"Delete\";\n\n\ncomp1.openInViewer(); \ncomp2.openInViewer();\ncomp3.openInViewer(); \nfinalcomp.openInViewer(); \nscn.openInViewer();  \n \n    \n    app.endUndoGroup;"
          }
        },
        {
          "id": "8bbee325-eee3-4e79-a224-f13a140113b6",
          "name": "ExpUniverse",
          "description": "",
          "label": "⚑",
          "color": "rgba(208, 2, 27, 1)",
          "iconType": "symbol",
          "svg": "",
          "sourceType": 1,
          "action": {
            "kind": "externalScript",
            "value": "C:/Program Files/Adobe/Adobe After Effects 2022/Support Files/Scripts/ScriptUI Panels/ExpressionUniversalizer.jsxbin"
          }
        },
        {
          "id": "b649d4b8-9fa4-4e73-9d7e-d8902a8c49ef",
          "name": "Mask to Layers",
          "description": "",
          "label": "MASK2LAY",
          "color": "rgba(248, 231, 28, 1)",
          "iconType": "text",
          "svg": "",
          "sourceType": 2,
          "action": {
            "kind": "scriptlet",
            "value": "function MasksToLayers()\n{\n\tvar masksToLayers = this;\n\t\n\tvar utils = new MasksToLayersUtils();\n\n\t// infos\n\tthis.scriptName = \"MasksToLayers.jsx\";\t\n\tthis.scriptVersion = \"3.0\";\n\tthis.scriptTitle = \"Masks To Layers\";\n\tthis.scriptCopyright = \"Copyright (c) 2012 Charles Bordenave\";\n\tthis.scriptHomepage = \"http://www.nabscripts.com\";\n\tthis.scriptDescription = {en: \"This script places the masks of the selected layer on individual layers.\\\\r\\\\rFor each new layer, you can specify the blending mode, and you can choose to move the anchor point to the center of the mask.\", fr:\"Ce script place chaque masque du calque sГ©lectionnГ© sur un calque sГ©parГ©.\\\\r\\\\rPour chaque nouveau calque, il est possible de prГ©ciser un mode de fusion, et de choisir de dГ©placer le point d\\\\'ancrage au centre du masque.\"};\n\tthis.scriptAbout = {en:this.scriptName + \", v\" + this.scriptVersion + \"\\\\r\" + this.scriptCopyright + \"\\\\r\" + this.scriptHomepage + \"\\\\r\\\\r\" + utils.loc(this.scriptDescription), \n\t\t\t\t\t\tfr:this.scriptName + \", v\" + this.scriptVersion + \"\\\\r\" + this.scriptCopyright + \"\\\\r\" + this.scriptHomepage + \"\\\\r\\\\r\" + utils.loc(this.scriptDescription)};\t\t\n\tthis.scriptUsage = {en:\t\"\\u25BA Select a layer having at least two masks \\\\r\" +\n\t\t\t\t\t\t\t\"\\u25BA Specify the blending mode of the new layers and whether you want to move the layer\\\\\\'s anchor point to the center of the mask \\\\r\" +\n\t\t\t\t\t\t\t\"\\u25BA Click on Proceed\",\n\t\t\t\t\t\tfr:\t\"\\u25BA SГ©lectionnez un calque contenant au moins deux masques \\\\r\" +\n\t\t\t\t\t\t\t\"\\u25BA SpГ©cifiez le mode de fusion des nouveaux calques et si le point d\\\\\\'ancrage doit ГЄtre dГ©placГ© au centre du masque \\\\r\" +\n\t\t\t\t\t\t\t\"\\u25BA Cliquez sur ExГ©cuter\"};\n\t\t\t\t\t\t\t\n\t// errors\n\tthis.noCompErr = {en:\"A comp must be active.\", fr:\"Une composition doit ГЄtre active.\"};\n\tthis.noLayersErr = {en:\"Select a layer first.\", fr:\"SГ©lectionnez d'abord un calque.\"};\n\n\t// UI strings \n\tthis.aboutBtnName = \"?\";\n\tthis.optionsPnlName = {en:\"Options\", fr:\"Options\"};\n\tthis.blendingModeStName = {en:\"Blending Mode:\", fr:\"Mode de fusion:\"};\n\tthis.blendingModeLstChoices = {en:\"['Normal','Add','Alpha Add']\", fr:\"['Normal','Ajout','Ajout Alpha']\"};\n\tthis.anchorPointStName = {en:\"Anchor Point:\", fr:\"Point d\\\\'ancrage:\"};\n\tthis.anchorPointLstChoices = {en:\"['Do not change','Move To Mask Center']\", fr:\"['Ne pas changer','DГ©placer au centre du masque']\"};\n\tthis.runBtnName = {en:\"Proceed\", fr:\"ExГ©cuter\"};\n\t\n\t\n\tthis.buildUI = function (thisObj)\n\t{\n\t\t// dockable panel or palette\n\t\tvar pal = (thisObj instanceof Panel) ? thisObj : new Window(\"palette\", this.scriptTitle, undefined, {resizeable:false});\n\n\t\t// resource specifications\n\t\tvar res =\n\t\t\"group { orientation:'column', alignment:['left','top'], alignChildren:['right','top'], \\\n\t\t\tgr1: Group { \\\n\t\t\t\taboutBtn: Button { text:'\" + this.aboutBtnName + \"', preferredSize:[25,20] } \\\n\t\t\t}, \\\n\t\t\tgr2: Panel { orientation:'column', alignment:['fill','fill'], alignChildren:['right','top'], text:'\" + utils.loc(this.optionsPnlName) + \"', \\\n\t\t\t\tgr21: Group { orientation:'row', \\\n\t\t\t\t\tblendingModeSt: StaticText { text:'\" + utils.loc(this.blendingModeStName) + \"', value:true }, \\\n\t\t\t\t\tblendingModeLst: DropDownList { properties:{items:\" + utils.loc(this.blendingModeLstChoices) + \"}, preferredSize:[150,20] } \\\n\t\t\t\t}, \\\n\t\t\t\tgr22: Group { orientation:'row', \\\n\t\t\t\t\tanchorPointSt: StaticText { text:'\" + utils.loc(this.anchorPointStName) + \"', value:true }, \\\n\t\t\t\t\tanchorPointLst: DropDownList { properties:{items:\" + utils.loc(this.anchorPointLstChoices) + \"}, preferredSize:[150,20] } \\\n\t\t\t\t} \\\n\t\t\t}, \\\n\t\t\tgr3: Group { orientation:'row', alignment:['fill','top'], \\\n\t\t\t\trunBtn: Button { text:'\" + utils.loc(this.runBtnName) + \"', alignment:['right','center'] } \\\n\t\t\t} \\\n\t\t}\"; \n\t\tpal.gr = pal.add(res);\n\t\t\n\t\tpal.gr.gr2.gr21.blendingModeLst.selection = 0;\n\t\tpal.gr.gr2.gr22.anchorPointLst.selection = 0;\n\t\t\n\t\t// event callbacks\n\t\tpal.gr.gr1.aboutBtn.onClick = function () \n\t\t{ \n\t\t\tutils.createAboutDlg(masksToLayers.scriptAbout, masksToLayers.scriptUsage); \n\t\t};\n\t\t\n\t\tpal.gr.gr3.runBtn.onClick = function () \n\t\t{ \n\t\t\tmasksToLayers.distributeMasks(pal); \n\t\t};\t\t\n\t\t\t\t\n\t\t// show user interface\n\t\tif (pal instanceof Window)\n\t\t{\n\t\t\tpal.center();\n\t\t\tpal.show();\n\t\t}\n\t\telse\n\t\t{\n\t\t\tpal.layout.layout(true);\n\t\t}\t   \n\t};\n\n\t// Determines whether the active item is a composition  \n\tthis.checkActiveItem = function () \n\t{\n\t\treturn !(app.project.activeItem instanceof CompItem);\n\t};   \n\t\n\t// Removes all masks except one\n\tthis.removeLayerMasksExceptOne = function (layer, idToKeep)\n\t{\n\t\tfor (var id = layer.Masks.numProperties; id > 0; id--)\n\t\t{\n\t\t\tif (id != idToKeep)\n\t\t\t{\n\t\t\t\tlayer.Masks.property(id).remove();\n\t\t\t}\n\t\t}\n\t};\n\n\t// Repositions the anchor point around the center of the first mask\n\tthis.moveAnchorPointToMaskCenter = function (layer)\n\t{\n\t\tvar curTime = layer.containingComp.time;\n\t\tvar shape = layer.Masks.property(1).maskShape.valueAtTime(curTime, false);\n\t\tvar verts = shape.vertices;\n\t\t\n\t\t// Compute centroid\n\t\tvar centroid = utils.getCenterOfMass(verts);\n\t\tvar x = centroid[0];\n\t\tvar y = centroid[1];\n\t\tvar z = 0;\n\t\t\n\t\t// Reposition anchor point and adjust position\n\t\tvar anchPt = layer.anchorPoint;\n\t\tvar pos = layer.position;\n\t\tvar offset = [x,y,z] - anchPt.valueAtTime(curTime, false);\n\t\tvar newAnchPt = [x,y,z];\n\t\tvar newPos = pos.valueAtTime(curTime, false) + offset;\t\t\t\t\n\t\t\n\t\tanchPt.numKeys ? anchPt.setValueAtTime(curTime, newAnchPt) : anchPt.setValue(newAnchPt);\n\t\tpos.numKeys ? pos.setValueAtTime(curTime, newPos) : pos.setValue(newPos);\n\t};\n\t\t\n\t// Functional part of the script: places each mask of the selected layer on its own layer\n\tthis.distributeMasks = function (pal)\n\t{\n\t\ttry\n\t\t{\n\t\t\tvar comp = app.project.activeItem;\n\t\t\tvar err = this.noCompErr;\n\t\t\tif (this.checkActiveItem(comp)) throw(err);\n\t\t\t\t\t\n\t\t\tvar err = this.noLayersErr;\n\t\t\tif (comp.selectedLayers.length < 1) throw(err);\n\t\t\t\n\t\t\tvar layer = comp.selectedLayers[0];\n\t\t\t\n\t\t\tvar blendingModeOption = pal.gr.gr2.gr21.blendingModeLst.selection.index;\n\t\t\tvar anchorPointOption = pal.gr.gr2.gr22.anchorPointLst.selection.index;  \n\t\t\t\t\t\t\n\t\t\tapp.beginUndoGroup(this.scriptTitle);\n\t\t\t\t  \n\t\t\tfor (var maskId = 1; maskId <= layer.Masks.numProperties; maskId++) \n\t\t\t{\n\t\t\t\tvar dupLayer = layer.duplicate();\n\t\t\t\t\n\t\t\t\tthis.removeLayerMasksExceptOne(dupLayer, maskId);\n\t\t\t\t\n\t\t\t\tdupLayer.name = (layer.name + \" - \" + layer.Masks.property(maskId).name).substring(0,31);\t\t\t\n\t\t\t\t\n\t\t\t\tdupLayer.audioEnabled = false;\n\t\t\t\t\n\t\t\t\tswitch (blendingModeOption)\n\t\t\t\t{\n\t\t\t\t\tcase 0: break;\n\t\t\t\t\tcase 1: dupLayer.blendingMode = BlendingMode.ADD; break;\n\t\t\t\t\tcase 2: dupLayer.blendingMode = BlendingMode.ALPHA_ADD; break;\n\t\t\t\t\tdefault: break;\n\t\t\t\t}\n\t\t\t\t\n\t\t\t\tif (anchorPointOption == 1)\n\t\t\t\t{\n\t\t\t\t\tthis.moveAnchorPointToMaskCenter(dupLayer);\n\t\t\t\t}\t\t\t\t\n\t\t\t}\n\t\t\t\n\t\t\tlayer.enabled = false;\n\t\t\t\t  \n\t\t\tapp.endUndoGroup();\n\t\t}\n\t\tcatch(err)\n\t\t{\n\t\t\tutils.throwErr(err);\n\t\t}\t\t\t\t\n\t};\n\t\n\tthis.run = function (thisObj) \n\t{\n\t\tthis.buildUI(thisObj);\n\t};\n}\n\n\n// This class provides some utility functions\nfunction MasksToLayersUtils()\n{\n\tvar utils = this;\n\t\n\tthis.loc = function (str)\n\t{\n\t\tvar lang = parseFloat(app.version) < 9 ? $.locale : app.isoLanguage;\n\t\treturn lang.toLowerCase().match(\"fr\") ? str.fr : str.en;\n\t};\n\n\tthis.throwErr = function (err)\n\t{\n\t\tvar title = $.fileName.substring($.fileName.lastIndexOf(\"/\")+1, $.fileName.lastIndexOf(\".\"));\n\t\talert(this.loc(err), title, true);\n\t};\n\t\n\tthis.createAboutDlg = function (aboutStr, usageStr)\n\t{\t\n\t\teval(unescape('%09%09%76%61%72%20%64%6C%67%20%3D%20%6E%65%77%20%57%69%6E%64%6F%77%28%22%64%69%61%6C%6F%67%22%2C%20%22%41%62%6F%75%74%22%29%3B%0A%09%20%20%20%20%20%20%09%20%20%20%20%20%20%20%09%0A%09%20%20%20%20%76%61%72%20%72%65%73%20%3D%0A%09%09%22%67%72%6F%75%70%20%7B%20%6F%72%69%65%6E%74%61%74%69%6F%6E%3A%27%63%6F%6C%75%6D%6E%27%2C%20%61%6C%69%67%6E%6D%65%6E%74%3A%5B%27%66%69%6C%6C%27%2C%27%66%69%6C%6C%27%5D%2C%20%61%6C%69%67%6E%43%68%69%6C%64%72%65%6E%3A%5B%27%66%69%6C%6C%27%2C%27%66%69%6C%6C%27%5D%2C%20%5C%0A%09%09%09%70%6E%6C%3A%20%50%61%6E%65%6C%20%7B%20%74%79%70%65%3A%27%74%61%62%62%65%64%70%61%6E%65%6C%27%2C%20%5C%0A%09%09%09%09%61%62%6F%75%74%54%61%62%3A%20%50%61%6E%65%6C%20%7B%20%74%79%70%65%3A%27%74%61%62%27%2C%20%74%65%78%74%3A%27%44%65%73%63%72%69%70%74%69%6F%6E%27%2C%20%5C%0A%09%09%09%09%09%61%62%6F%75%74%45%74%3A%20%45%64%69%74%54%65%78%74%20%7B%20%74%65%78%74%3A%27%22%20%2B%20%74%68%69%73%2E%6C%6F%63%28%61%62%6F%75%74%53%74%72%29%20%2B%20%22%27%2C%20%70%72%65%66%65%72%72%65%64%53%69%7A%65%3A%5B%33%36%30%2C%32%30%30%5D%2C%20%70%72%6F%70%65%72%74%69%65%73%3A%7B%6D%75%6C%74%69%6C%69%6E%65%3A%74%72%75%65%7D%20%7D%20%5C%0A%09%09%09%09%7D%2C%20%5C%0A%09%09%09%09%75%73%61%67%65%54%61%62%3A%20%50%61%6E%65%6C%20%7B%20%74%79%70%65%3A%27%74%61%62%27%2C%20%74%65%78%74%3A%27%55%73%61%67%65%27%2C%20%5C%0A%09%09%09%09%09%75%73%61%67%65%45%74%3A%20%45%64%69%74%54%65%78%74%20%7B%20%74%65%78%74%3A%27%22%20%2B%20%74%68%69%73%2E%6C%6F%63%28%75%73%61%67%65%53%74%72%29%20%2B%20%22%27%2C%20%70%72%65%66%65%72%72%65%64%53%69%7A%65%3A%5B%33%36%30%2C%32%30%30%5D%2C%20%70%72%6F%70%65%72%74%69%65%73%3A%7B%6D%75%6C%74%69%6C%69%6E%65%3A%74%72%75%65%7D%20%7D%20%5C%0A%09%09%09%09%7D%20%5C%0A%09%09%09%7D%2C%20%5C%0A%09%09%09%62%74%6E%73%3A%20%47%72%6F%75%70%20%7B%20%6F%72%69%65%6E%74%61%74%69%6F%6E%3A%27%72%6F%77%27%2C%20%61%6C%69%67%6E%6D%65%6E%74%3A%5B%27%66%69%6C%6C%27%2C%27%62%6F%74%74%6F%6D%27%5D%2C%20%5C%0A%09%09%09%09%6F%74%68%65%72%53%63%72%69%70%74%73%42%74%6E%3A%20%42%75%74%74%6F%6E%20%7B%20%74%65%78%74%3A%27%4F%74%68%65%72%20%53%63%72%69%70%74%73%2E%2E%2E%27%2C%20%61%6C%69%67%6E%6D%65%6E%74%3A%5B%27%6C%65%66%74%27%2C%27%63%65%6E%74%65%72%27%5D%20%7D%2C%20%5C%0A%09%09%09%09%6F%6B%42%74%6E%3A%20%42%75%74%74%6F%6E%20%7B%20%74%65%78%74%3A%27%4F%6B%27%2C%20%61%6C%69%67%6E%6D%65%6E%74%3A%5B%27%72%69%67%68%74%27%2C%27%63%65%6E%74%65%72%27%5D%20%7D%20%5C%0A%09%09%09%7D%20%5C%0A%09%09%7D%22%3B%20%0A%09%09%64%6C%67%2E%67%72%20%3D%20%64%6C%67%2E%61%64%64%28%72%65%73%29%3B%0A%09%09%0A%09%09%64%6C%67%2E%67%72%2E%70%6E%6C%2E%61%62%6F%75%74%54%61%62%2E%61%62%6F%75%74%45%74%2E%6F%6E%43%68%61%6E%67%65%20%3D%20%64%6C%67%2E%67%72%2E%70%6E%6C%2E%61%62%6F%75%74%54%61%62%2E%61%62%6F%75%74%45%74%2E%6F%6E%43%68%61%6E%67%69%6E%67%20%3D%20%66%75%6E%63%74%69%6F%6E%20%28%29%0A%09%09%7B%0A%09%09%09%74%68%69%73%2E%74%65%78%74%20%3D%20%75%74%69%6C%73%2E%6C%6F%63%28%61%62%6F%75%74%53%74%72%29%2E%72%65%70%6C%61%63%65%28%2F%5C%5C%72%2F%67%2C%20%27%5C%72%27%29%3B%0A%09%09%7D%3B%0A%09%09%0A%09%09%64%6C%67%2E%67%72%2E%70%6E%6C%2E%75%73%61%67%65%54%61%62%2E%75%73%61%67%65%45%74%2E%6F%6E%43%68%61%6E%67%65%20%3D%20%64%6C%67%2E%67%72%2E%70%6E%6C%2E%75%73%61%67%65%54%61%62%2E%75%73%61%67%65%45%74%2E%6F%6E%43%68%61%6E%67%69%6E%67%20%3D%20%66%75%6E%63%74%69%6F%6E%20%28%29%0A%09%09%7B%0A%09%09%09%74%68%69%73%2E%74%65%78%74%20%3D%20%75%74%69%6C%73%2E%6C%6F%63%28%75%73%61%67%65%53%74%72%29%2E%72%65%70%6C%61%63%65%28%2F%5C%5C%72%2F%67%2C%20%27%5C%72%27%29%2E%72%65%70%6C%61%63%65%28%2F%5C%5C%27%2F%67%2C%20%22%27%22%29%3B%0A%09%09%7D%3B%0A%09%09%09%0A%09%09%64%6C%67%2E%67%72%2E%62%74%6E%73%2E%6F%74%68%65%72%53%63%72%69%70%74%73%42%74%6E%2E%6F%6E%43%6C%69%63%6B%20%3D%20%66%75%6E%63%74%69%6F%6E%20%28%29%0A%09%09%7B%0A%09%09%09%76%61%72%20%63%6D%64%20%3D%20%22%22%3B%0A%09%09%09%76%61%72%20%75%72%6C%20%3D%20%22%68%74%74%70%3A%2F%2F%61%65%73%63%72%69%70%74%73%2E%63%6F%6D%2F%61%75%74%68%6F%72%73%2F%6D%2D%70%2F%6E%61%62%2F%22%3B%0A%09%0A%09%09%09%69%66%20%28%24%2E%6F%73%2E%69%6E%64%65%78%4F%66%28%22%57%69%6E%22%29%20%21%3D%20%2D%31%29%0A%09%09%09%7B%0A%09%20%20%20%20%20%20%20%20%09%69%66%20%28%46%69%6C%65%28%22%43%3A%2F%50%72%6F%67%72%61%6D%20%46%69%6C%65%73%2F%4D%6F%7A%69%6C%6C%61%20%46%69%72%65%66%6F%78%2F%66%69%72%65%66%6F%78%2E%65%78%65%22%29%2E%65%78%69%73%74%73%29%0A%09%09%09%09%09%63%6D%64%20%2B%3D%20%22%43%3A%2F%50%72%6F%67%72%61%6D%20%46%69%6C%65%73%2F%4D%6F%7A%69%6C%6C%61%20%46%69%72%65%66%6F%78%2F%66%69%72%65%66%6F%78%2E%65%78%65%20%22%20%2B%20%75%72%6C%3B%0A%09%09%09%09%65%6C%73%65%20%69%66%20%28%46%69%6C%65%28%22%43%3A%2F%50%72%6F%67%72%61%6D%20%46%69%6C%65%73%20%28%78%38%36%29%2F%4D%6F%7A%69%6C%6C%61%20%46%69%72%65%66%6F%78%2F%66%69%72%65%66%6F%78%2E%65%78%65%22%29%2E%65%78%69%73%74%73%29%0A%09%09%09%09%09%63%6D%64%20%2B%3D%20%22%43%3A%2F%50%72%6F%67%72%61%6D%20%46%69%6C%65%73%20%28%78%38%36%29%2F%4D%6F%7A%69%6C%6C%61%20%46%69%72%65%66%6F%78%2F%66%69%72%65%66%6F%78%2E%65%78%65%20%22%20%2B%20%75%72%6C%3B%0A%09%09%09%09%65%6C%73%65%0A%09%09%09%09%09%63%6D%64%20%2B%3D%20%22%43%3A%2F%50%72%6F%67%72%61%6D%20%46%69%6C%65%73%2F%49%6E%74%65%72%6E%65%74%20%45%78%70%6C%6F%72%65%72%2F%69%65%78%70%6C%6F%72%65%2E%65%78%65%20%22%20%2B%20%75%72%6C%3B%0A%09%09%09%7D%0A%09%09%09%65%6C%73%65%0A%09%09%09%09%63%6D%64%20%2B%3D%20%22%6F%70%65%6E%20%5C%22%22%20%2B%20%75%72%6C%20%2B%20%22%5C%22%22%3B%20%20%20%20%20%20%20%20%20%0A%09%09%09%74%72%79%0A%09%09%09%7B%0A%09%09%09%09%73%79%73%74%65%6D%2E%63%61%6C%6C%53%79%73%74%65%6D%28%63%6D%64%29%3B%0A%09%09%09%7D%0A%09%09%09%63%61%74%63%68%28%65%29%0A%09%09%09%7B%0A%09%09%09%09%61%6C%65%72%74%28%65%29%3B%0A%09%09%09%7D%0A%09%09%7D%3B%0A%09%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%0A%09%09%64%6C%67%2E%67%72%2E%62%74%6E%73%2E%6F%6B%42%74%6E%2E%6F%6E%43%6C%69%63%6B%20%3D%20%66%75%6E%63%74%69%6F%6E%20%28%29%20%0A%09%09%7B%0A%09%09%09%64%6C%67%2E%63%6C%6F%73%65%28%29%3B%20%0A%09%09%7D%3B%0A%09%20%20%20%20%20%20%20%0A%09%09%64%6C%67%2E%63%65%6E%74%65%72%28%29%3B%0A%09%09%64%6C%67%2E%73%68%6F%77%28%29%3B'));\n\t};\t\t\n\n\t// Computes area of a polygon defined by a set of points \n\tthis.getArea = function (points)\n\t{\n\t\tvar area = 0;\t\n\t\tfor (var i = 0; i < points.length; i++) \n\t\t{\n\t\t\tvar j = (i + 1) % points.length;\n\t\t\tarea += points[i][0] * points[j][1];\n\t\t\tarea -= points[i][1] * points[j][0];\n\t\t}\n\t\tarea /= 2.0;\t\n\t\treturn area; \n\t};\n\t\n\tthis.getCenterOfMass = function (points)\n\t{\n\t\tvar cx = 0, cy = 0;\n\t\tvar factor = 0;\n\t\tvar A = this.getArea(points);\n\t\tfor (var i = 0; i < points.length; i++) \n\t\t{\n\t\t\tvar j = (i + 1) % points.length;\n\t\t\tfactor = points[i][0] * points[j][1] - points[j][0] * points[i][1];\n\t\t\tcx+= (points[i][0] + points[j][0]) * factor;\n\t\t\tcy+= (points[i][1] + points[j][1]) * factor;\n\t\t}\n\t\tA *= 6.0;\n\t\tfactor = 1 / A;\n\t\tcx *= factor;\n\t\tcy *= factor;\n\t\treturn [cx,cy];\n\t};\n}\n\n\n// run script\nnew MasksToLayers().run(this);\n"
          }
        },
        {
          "id": "a8010111-a9c1-472a-9e84-6fb9d9e64e76",
          "name": "Loop Pingpong",
          "description": "",
          "label": "↻",
          "color": "rgba(80, 227, 194, 1)",
          "iconType": "symbol",
          "svg": "",
          "sourceType": 0,
          "action": {
            "kind": "expression",
            "value": "loopOut(\"pingpong\")"
          }
        },
        {
          "id": "ef82a2c7-a052-4389-af53-1a6526b9dbc9",
          "name": "Loop Cycle",
          "description": "",
          "label": "↻",
          "color": "rgba(80, 227, 194, 1)",
          "iconType": "symbol",
          "svg": "",
          "sourceType": 0,
          "action": {
            "kind": "expression",
            "value": "loopOut(\"cycle\")"
          }
        },
        {
          "id": "daf4485c-2683-4861-9921-e4a479f1c5c9",
          "name": "Add Null",
          "description": "",
          "label": "NULL",
          "color": "rgba(245, 166, 35, 1)",
          "iconType": "text",
          "svg": "",
          "sourceType": 2,
          "action": {
            "kind": "scriptlet",
            "value": "if (app.project.activeItem instanceof CompItem) {\n  var comp = app.project.activeItem;\n  \n  // Check if any layers are selected in the active composition\n  if (comp.selectedLayers.length > 0) {\n    // Get the index of the topmost selected layer\n    var topLayerIndex = comp.selectedLayers[0].index;\n\n    // Loop through the selected layers to find the topmost index\n    for (var i = 1; i < comp.selectedLayers.length; i++) {\n      var layerIndex = comp.selectedLayers[i].index;\n      if (layerIndex < topLayerIndex) {\n        topLayerIndex = layerIndex;\n      }\n    }\n\n    // Create a null layer above the topmost selected layer\n    var nullLayer = comp.layers.addNull(comp.duration);\n    nullLayer.moveTo(topLayerIndex);\n    nullLayer.position.setValue([comp.width / 2, comp.height / 2]);\n\n    // Center the anchor point of the null layer\n    nullLayer.property(\"Anchor Point\").setValue([nullLayer.width / 2, nullLayer.height / 2]);\n  } else {\n    // Create a null layer at the center of the active composition\n    var nullLayer = comp.layers.addNull(comp.duration);\n    nullLayer.position.setValue([comp.width / 2, comp.height / 2]);\n\n    // Center the anchor point of the null layer\n    nullLayer.property(\"Anchor Point\").setValue([nullLayer.width / 2, nullLayer.height / 2]);\n  }\n}\n"
          }
        },
        {
          "id": "db00dc10-65e0-46c0-a73b-793610485a20",
          "name": "Solid",
          "description": "",
          "label": "SOLID",
          "color": "rgba(245, 166, 35, 1)",
          "iconType": "text",
          "svg": "",
          "sourceType": 2,
          "action": {
            "kind": "scriptlet",
            "value": "try{\n    var solid = app.project.activeItem.layers.addSolid([1,1,1], \"Solid Layer\", app.project.activeItem.width, app.project.activeItem.height, app.project.activeItem.pixelAspect, app.project.activeItem.duration);\n\n    }\ncatch (e) {\n    alert (e);\n    }"
          }
        },
        {
          "id": "3afc2742-3ad4-412a-8c52-c051b1fb174c",
          "name": "Adjustment Layer",
          "description": "",
          "label": "ADJ LYR",
          "color": "rgba(245, 166, 35, 1)",
          "iconType": "text",
          "svg": "",
          "sourceType": 2,
          "action": {
            "kind": "scriptlet",
            "value": "try{\n    var adjLayer = app.project.activeItem.layers.addSolid([1,1,1], \"Adjustment Layer\", app.project.activeItem.width, app.project.activeItem.height, app.project.activeItem.pixelAspect, app.project.activeItem.duration);\n    adjLayer.adjustmentLayer = true;\n    }\ncatch (e) {\n    alert (e);\n    }"
          }
        },
        {
          "id": "bb67edeb-8e92-46e2-96c5-2297ba0aadac",
          "name": "Make Comp Size",
          "description": "",
          "label": "COMPSIZE",
          "color": "rgba(245, 166, 35, 1)",
          "iconType": "text",
          "svg": "",
          "sourceType": 2,
          "action": {
            "kind": "scriptlet",
            "value": "       //Make layer Comp Size and Fit to Comp\n       //Nikita Shilov\napp.beginUndoGroup(\"Make layer Comp Size and Fit to Comp\");\nvar myLayers = app.project.activeItem.selectedLayers;\n            poisk: for (i=0; i<myLayers.length; i++){\n            if (myLayers[i] instanceof CameraLayer || myLayers[i] instanceof LightLayer || myLayers[i] instanceof ShapeLayer || myLayers[i] instanceof TextLayer)\n                            {continue poisk;}\n                var comp = app.project.activeItem;\n                var source = myLayers[i].source;\n                source.width = comp.width;\n                source.height = comp.height;\n                app.executeCommand(2156);\n \n            }\n            app.endUndoGroup;"
          }
        },
        {
          "id": "a0eb8fb4-d9c5-4830-b2ab-4a9e66addf9c",
          "name": "Swap Height and Width",
          "description": "",
          "label": "SWAP-WH",
          "color": "rgba(245, 166, 35, 1)",
          "iconType": "text",
          "svg": "",
          "sourceType": 2,
          "action": {
            "kind": "scriptlet",
            "value": "       //Swap Comp Width and Height \n       //Nikita Shilov\napp.beginUndoGroup(\"Swap Comp Width and Height \");\n            \nvar comp = app.project.activeItem;\nvar compSize = [comp.width,comp.height];\ncomp.height = compSize[0];\ncomp.width = compSize[1];\n\n            app.endUndoGroup;"
          }
        },
        {
          "id": "9cc44e0e-e3ab-44e2-9edb-717a75a689e9",
          "name": "Shape Strim",
          "description": "",
          "label": "Shape Strim",
          "color": "#b6b6b6",
          "iconType": "svg",
          "svg": "data:image/svg+xml;utf8,<svg id=\"圖層_1\" data-name=\"圖層 1\" xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 30 30\"><defs><style>.cls-1,.cls-2{fill:none;stroke-miterlimit:10;stroke-width:2.5px;}.cls-1{stroke:#4d4d4d;}.cls-2{stroke:#8cc63f;}.cls-3{fill:#8cc63f;}</style></defs><path class=\"cls-1\" d=\"M8.49,7.91a11.36,11.36,0,0,0-4.9,8\"/><path class=\"cls-1\" d=\"M26.41,15.92a11.31,11.31,0,0,0-4.89-8\"/><path class=\"cls-2\" d=\"M21.52,7.92a11.64,11.64,0,0,0-13,0\"/><rect class=\"cls-3\" x=\"19.04\" y=\"5.44\" width=\"4.95\" height=\"4.95\" transform=\"translate(2.86 21.24) rotate(-55.73)\"/><rect class=\"cls-3\" x=\"6.01\" y=\"5.44\" width=\"4.95\" height=\"4.95\" transform=\"matrix(0.83, -0.56, 0.56, 0.83, -2.98, 6.15)\"/><path class=\"cls-3\" d=\"M13.31,20.42A4,4,0,0,1,9.25,24.7c-2.6,0-4-1.88-4-4.25A3.94,3.94,0,0,1,9.36,16.2C11.78,16.2,13.31,17.89,13.31,20.42Zm-6.17,0c0,1.62.71,2.84,2.17,2.84s2.16-1.33,2.16-2.81-.64-2.79-2.19-2.79S7.14,18.8,7.14,20.41Z\"/><path class=\"cls-3\" d=\"M20.05,22.78c0,.6,0,1.25,0,1.8H18.38c0-.16,0-.5-.05-.71a1.76,1.76,0,0,1-1.71.86c-1.26,0-2.07-.77-2.07-2.31v-4h1.72v3.7c0,.68.21,1.25,1,1.25s1.1-.42,1.1-1.65v-3.3h1.72Z\"/><path class=\"cls-3\" d=\"M21.1,18.38h.95V16.7h1.72v1.68H25v1.29H23.77v3.06c0,.47.15.66.66.66l.39,0v1.18a3.79,3.79,0,0,1-1.16.12c-1.21,0-1.61-.65-1.61-1.74V19.67H21.1Z\"/></svg>",
          "sourceType": 1,
          "action": {
            "kind": "externalScript",
            "value": "C:/Program Files/Adobe/Adobe After Effects 2022/Support Files/Scripts/ScriptUI Panels/trim-pack.jsxbin"
          }
        },
        {
          "id": "ead64901-7e5d-4009-b48b-dcb6b1537534",
          "name": "Explode shapes",
          "description": "",
          "label": "Z-X EXPL",
          "color": "rgba(248, 231, 28, 1)",
          "iconType": "text",
          "svg": "",
          "sourceType": 1,
          "action": {
            "kind": "externalScript",
            "value": "C:/Program Files/Adobe/Adobe After Effects 2022/Support Files/Scripts/ScriptUI Panels/zl_ExplodeShapeLayers.jsxbin"
          }
        },
        {
          "id": "707e3342-1fb5-4f7f-8228-5c1b99177df1",
          "name": "Scale Comp",
          "description": "",
          "label": "✣",
          "color": "rgba(248, 231, 28, 1)",
          "iconType": "symbol",
          "svg": "",
          "sourceType": 2,
          "action": {
            "kind": "scriptlet",
            "value": "{\n\t// Scale Composition.jsx\n\t// \n\t// This script scales the active comp and all the layers within it.\n\t//\n\t// First, it prompts the user for a scale_factor, a new comp width, \n\t// or a new comp height.\n\t// \n\t// Next, it scales the comp and all the layers within it, including\n\t// cameras.\n\t\n\tfunction ScaleComposition(thisObj)\n\t{\n\t\tvar scriptName = \"Scale Composition\";\n\t\t\n\t\t// This variable stores the scale_factor.\n\t\tvar scale_factor = 1.0;\n\t\tvar text_input = null;\n\t\tvar scaleButton  = null;\n\t\tvar widthButton  = null;\n\t\tvar heightButton = null;\n\t\t\n\t\t\n\t\tfunction onScaleButtonClick()\n\t\t{\n\t\t\tthis.parent.text_input.text = scale_factor;\n\t\t}\n\t\t\n\t\t\n\t\tfunction onWidthButtonClick()\n\t\t{\n\t\t\tvar activeItem = app.project.activeItem;\n\t\t\tif ((activeItem == null) || !(activeItem instanceof CompItem)) {\n\t\t\t\talert(\"Please select or open a composition first.\", scriptName);\n\t\t\t} else {\n\t\t\t\tthis.parent.text_input.text = Math.floor(activeItem.width * scale_factor);\n\t\t\t}\n\t\t}\n\t\t\n\t\t\n\t\tfunction onHeightButtonClick()\n\t\t{\n\t\t\tvar activeItem = app.project.activeItem;\n\t\t\tif ((activeItem == null) || !(activeItem instanceof CompItem)) {\n\t\t\t\talert(\"Please select or open a composition first.\", scriptName);\n\t\t\t} else {\n\t\t\t\tthis.parent.text_input.text = Math.floor(activeItem.height * scale_factor);\n\t\t\t}\n\t\t}\n\t\t\n\t\t\n\t\tfunction testNewScale(test_scale)\n\t\t{\n\t\t\tvar is_ok = true;\n\t\t\tvar activeItem = app.project.activeItem;\n\t\t\tif ((activeItem == null) || !(activeItem instanceof CompItem)) {\n\t\t\t\talert(\"Please select or open a composition first.\", scriptName);\n\t\t\t} else {\n\t\t\t\tif (test_scale * activeItem.width < 1 || test_scale * activeItem.width > 30000) {\n\t\t\t\t\tis_ok = false;\n\t\t\t\t} else if (test_scale * activeItem.height < 1 || test_scale * activeItem.height > 30000) {\n\t\t\t\t\tis_ok = false;\n\t\t\t\t}\n\t\t\t}\n\t\t\t\n\t\t\treturn is_ok;\n\t\t}\n\t\t\n\t\t\n\t\t//\n\t\t// This function is called when the user enters text for the scale.\n\t\t//\n\t\tfunction on_textInput_changed()\n\t\t{\n\t\t\tvar activeItem = app.project.activeItem;\n\t\t\tif ((activeItem == null) || !(activeItem instanceof CompItem)) {\n\t\t\t\talert(\"Please select or open a composition first.\", scriptName);\n\t\t\t} else {\n\t\t\t\t// Set the scale_factor based on the text.\n\t\t\t\tvar value = this.text;\n\t\t\t\tif (isNaN(value)) {\n\t\t\t\t\talert(value + \" is not a number. Please enter a number.\", scriptName);\n\t\t\t\t} else {\n\t\t\t\t\tvar new_scale_factor;\n\t\t\t\t\tif (this.parent.scaleButton.value == true) {\n\t\t\t\t\t\tnew_scale_factor = value;\n\t\t\t\t\t} else if (this.parent.widthButton.value == true) {\n\t\t\t\t\t\tnew_scale_factor = value / activeItem.width;\n\t\t\t\t\t} else {\n\t\t\t\t\t\tnew_scale_factor = value / activeItem.height;\n\t\t\t\t\t}\n\t\t\t\t\tif (testNewScale(new_scale_factor)) {\n\t\t\t\t\t\tscale_factor = new_scale_factor;\n\t\t\t\t\t} else {\n\t\t\t\t\t\talert(\"Value will make height or width out of range 1 to 30000. Reverting to previous value.\", scriptName);\n\t\t\t\t\t\t// Load text back in from current values.\n\t\t\t\t\t\tif (scaleButton.value == true) {\n\t\t\t\t\t\t\tonScaleButtonClick();\n\t\t\t\t\t\t} else if (widthButton.value == true) {\n\t\t\t\t\t\t\tonWidthButtonClick();\n\t\t\t\t\t\t} else {\n\t\t\t\t\t\t\tonHeightButtonClick();\n\t\t\t\t\t\t}\n\t\t\t\t\t}\n\t\t\t\t}\n\t\t\t}\n\t\t}\n\t\t\n\t\t\n\t\tfunction onScaleClick()\n\t\t{\n\t\t\tvar activeItem = app.project.activeItem;\n\t\t\tif ((activeItem == null) || !(activeItem instanceof CompItem)) {\n\t\t\t\talert(\"Please select or open a composition first.\", scriptName);\n\t\t\t} else {\n\t\t\t\t// Validate the input field, in case the user didn't defocus it first (which often can be the case).\n\t\t\t\tthis.parent.parent.optsRow.text_input.notify(\"onChange\");\n\t\t\t\t\n\t\t\t\tvar activeComp = activeItem;\n\t\t\t\t\n\t\t\t\t// By bracketing the operations with begin/end undo group, we can \n\t\t\t\t// undo the whole script with one undo operation.\n\t\t\t\tapp.beginUndoGroup(scriptName);\n\t\t\t\t\n\t\t\t\t// Create a null 3D layer.\n\t\t\t\tvar null3DLayer = activeItem.layers.addNull();\n\t\t\t\tnull3DLayer.threeDLayer = true;\n\t\t\t\t\n\t\t\t\t// Set its position to (0,0,0).\n\t\t\t\tnull3DLayer.position.setValue([0,0,0]);\n\t\t\t\t\n\t\t\t\t// Set null3DLayer as parent of all layers that don't have parents.  \n\t\t\t\tmakeParentLayerOfAllUnparented(activeComp, null3DLayer);\n\t\t\t\t\n\t\t\t\t// Set new comp width and height.\n\t\t\t\tactiveComp.width  = Math.floor(activeComp.width * scale_factor);\n\t\t\t\tactiveComp.height = Math.floor(activeComp.height * scale_factor);\n\t\t\t\t\n\t\t\t\t// Then for all cameras, scale the Zoom parameter proportionately.\n\t\t\t\tscaleAllCameraZooms(activeComp, scale_factor);\n\t\t\t\t\n\t\t\t\t// Set the scale of the super parent null3DLayer proportionately.\n\t\t\t\tvar superParentScale = null3DLayer.scale.value;\n\t\t\t\tsuperParentScale[0] = superParentScale[0] * scale_factor;\n\t\t\t\tsuperParentScale[1] = superParentScale[1] * scale_factor;\n\t\t\t\tsuperParentScale[2] = superParentScale[2] * scale_factor;\n\t\t\t\tnull3DLayer.scale.setValue(superParentScale);\n\t\t\t\t\n\t\t\t\t// Delete the super parent null3DLayer with dejumping enabled.\n\t\t\t\tnull3DLayer.remove();\n\t\t\t\t\n\t\t\t\tapp.endUndoGroup();\n\t\t\t\t\n\t\t\t\t// Reset scale_factor to 1.0 for next use.\n\t\t\t\tscale_factor = 1.0;\n\t\t\t\tif (this.parent.parent.optsRow.scaleButton.value) {\n\t\t\t\t\tthis.parent.parent.optsRow.text_input.text = \"1.0\";\n\t\t\t\t}\n\t\t\t}\n\t\t}\n\t\t\n\t\t\n\t\t// \n\t\t// This function puts up a modal dialog asking for a scale_factor.\n\t\t// Once the user enters a value, the dialog closes, and the script scales the comp.\n\t\t// \n\t\tfunction BuildAndShowUI(thisObj)\n\t\t{\n\t\t\t// Create and show a floating palette.\n\t\t\tvar my_palette = (thisObj instanceof Panel) ? thisObj : new Window(\"palette\", scriptName, undefined, {resizeable:true});\n\t\t\tif (my_palette != null)\n\t\t\t{\n\t\t\t\tvar res = \n\t\t\t\t\t\"group { \\\n\t\t\t\t\t\torientation:'column', alignment:['fill','top'], alignChildren:['left','top'], spacing:5, margins:[0,0,0,0], \\\n\t\t\t\t\t\tintroStr: StaticText { text:'Scale composition using:', alignment:['left','center'] }, \\\n\t\t\t\t\t\toptsRow: Group { \\\n\t\t\t\t\t\t\torientation:'column', alignment:['fill','top'], \\\n\t\t\t\t\t\t\tscaleButton: RadioButton { text:'New Scale Factor', alignment:['fill','top'], value:'true' }, \\\n\t\t\t\t\t\t\twidthButton: RadioButton { text:'New Comp Width', alignment:['fill','top'] }, \\\n\t\t\t\t\t\t\theightButton: RadioButton { text:'New Comp Height', alignment:['fill','top'] }, \\\n\t\t\t\t\t\t\ttext_input: EditText { text:'1.0', alignment:['left','top'], preferredSize:[80,20] }, \\\n\t\t\t\t\t\t}, \\\n\t\t\t\t\t\tcmds: Group { \\\n\t\t\t\t\t\t\talignment:['fill','top'], \\\n\t\t\t\t\t\t\tokButton: Button { text:'Scale', alignment:['fill','center'] }, \\\n\t\t\t\t\t\t}, \\\n\t\t\t\t\t}\";\n\t\t\t\t\n\t\t\t\tmy_palette.margins = [10,10,10,10];\n\t\t\t\tmy_palette.grp = my_palette.add(res);\n\t\t\t\t\n\t\t\t\t// Workaround to ensure the edittext text color is black, even at darker UI brightness levels.\n\t\t\t\tvar winGfx = my_palette.graphics;\n\t\t\t\tvar darkColorBrush = winGfx.newPen(winGfx.BrushType.SOLID_COLOR, [0,0,0], 1);\n\t\t\t\tmy_palette.grp.optsRow.text_input.graphics.foregroundColor = darkColorBrush;\n\t\t\t\t\n\t\t\t\tmy_palette.grp.optsRow.scaleButton.onClick  = onScaleButtonClick;\n\t\t\t\tmy_palette.grp.optsRow.widthButton.onClick  = onWidthButtonClick;\n\t\t\t\tmy_palette.grp.optsRow.heightButton.onClick = onHeightButtonClick;\n\t\t\t\t\n\t\t\t\t// Set the callback. When the user enters text, this will be called.\n\t\t\t\tmy_palette.grp.optsRow.text_input.onChange = on_textInput_changed;\n\t\t\t\t\n\t\t\t\tmy_palette.grp.cmds.okButton.onClick = onScaleClick;\n\t\t\t\t\n\t\t\t\tmy_palette.onResizing = my_palette.onResize = function () {this.layout.resize();}\n\t\t\t}\n\t\t\t\n\t\t\treturn my_palette;\n\t\t}\n\t\t\n\t\t\n\t\t// \n\t\t// Sets newParent as the parent of all layers in theComp that don't have parents.\n\t\t// This includes 2D/3D lights, camera, av, text, etc.\n\t\t//\n\t\tfunction makeParentLayerOfAllUnparented(theComp, newParent)\n\t\t{\n\t\t\tfor (var i = 1; i <= theComp.numLayers; i++) {\n\t\t\t\tvar curLayer = theComp.layer(i);\n\t\t\t\tif (curLayer != newParent && curLayer.parent == null) {\n\t\t\t\t\tcurLayer.parent = newParent;\n\t\t\t\t}\n\t\t\t}\n\t\t}\n\t\t\n\t\t\n\t\t//\n\t\t// Scales the zoom factor of every camera by the given scale_factor.\n\t\t// Handles both single values and multiple keyframe values.\n\t\tfunction scaleAllCameraZooms(theComp, scaleBy)\n\t\t{\n\t\t\tfor (var i = 1; i <= theComp.numLayers; i++) {\n\t\t\t\tvar curLayer = theComp.layer(i);\n\t\t\t\tif (curLayer.matchName == \"ADBE Camera Layer\") {\n\t\t\t\t\tvar curZoom = curLayer.zoom;\n\t\t\t\t\tif (curZoom.numKeys == 0) {\n\t\t\t\t\t\tcurZoom.setValue(curZoom.value * scaleBy);\n\t\t\t\t\t} else {\n\t\t\t\t\t\tfor (var j = 1; j <= curZoom.numKeys; j++) {\n\t\t\t\t\t\t\tcurZoom.setValueAtKey(j,curZoom.keyValue(j)*scaleBy);\n\t\t\t\t\t\t}\n\t\t\t\t\t}\n\t\t\t\t}\n\t\t\t}\n\t\t}\n\t\t\n\t\t\n\t\t// \n\t\t// The main script.\n\t\t//\n\t\tif (parseFloat(app.version) < 8) {\n\t\t\talert(\"This script requires After Effects CS3 or later.\", scriptName);\n\t\t\treturn;\n\t\t}\n\t\t\n\t\tvar my_palette = BuildAndShowUI(thisObj);\n\t\tif (my_palette != null) {\n\t\t\tif (my_palette instanceof Window) {\n\t\t\t\tmy_palette.center();\n\t\t\t\tmy_palette.show();\n\t\t\t} else {\n\t\t\t\tmy_palette.layout.layout(true);\n\t\t\t\tmy_palette.layout.resize();\n\t\t\t}\n\t\t} else {\n\t\t\talert(\"Could not open the user interface.\", scriptName);\n\t\t}\n\t}\n\t\n\t\n\tScaleComposition(this);\n}"
          }
        },
        {
          "id": "4304ed50-1b65-44cc-ad0a-56dad915fbf5",
          "name": "Camera Rig",
          "description": "",
          "label": "CAM RIG",
          "color": "rgba(126, 211, 33, 1)",
          "iconType": "text",
          "svg": "",
          "sourceType": 2,
          "action": {
            "kind": "scriptlet",
            "value": "var myComp = app.project.activeItem;\n\n///CREATE CAMERA///\n\nvar w = myComp.width /2 ;\nvar h = myComp.height /2 ;\nvar newCamera = myComp.layers.addCamera(\"Camera\",[w,h]);\nvar zoom = Number(newCamera.cameraOption.focusDistance.value*-1);\n\nvar cam = newCamera.Transform.position.setValue([w,h,zoom]);\n\n\n///CREATE NULL///\nvar myNull = myComp.layers.addNull();\nmyNull.threeDLayer = true;\n\n///RENAMES NULL//\nmyNull.name = \"Camera Control\";\n\n            var pos_slider1 = myNull.Effects.addProperty(\"ADBE Slider Control\");\n            pos_slider1.name = \"Pos\";\n            var rot_slider1 = myNull.Effects.addProperty(\"ADBE Slider Control\");\n            rot_slider1.name = \"Rot\";\n\n\n///PARENTS CAMERA TO NULL//\nmyComp.layer(\"Camera\").parent = myNull;\n\nvar pos = newCamera.Transform.position;\nvar rot = newCamera.Transform.orientation;\npos.expressionEnabled = true;\nrot.expressionEnabled = true;\npos.expression =\"wiggle(8,thisComp.layer('Camera Control').effect('Pos')('ADBE Slider Control-0001'));\"\nrot.expression =\"wiggle(8,thisComp.layer('Camera Control').effect('Rot')('ADBE Slider Control-0001'));\""
          }
        },
        {
          "id": "9ca8af21-7c64-46a4-93fe-8a1f4676c3bd",
          "name": "Opacity Up",
          "description": "",
          "label": "OPACITY",
          "color": "rgba(126, 211, 33, 1)",
          "iconType": "text",
          "svg": "",
          "sourceType": 4,
          "action": {
            "kind": "presetExternal",
            "value": "C:/Program Files/Adobe/Adobe After Effects CC 2022/Support Files/Presets/My Presets/Opacity-Up.ffx"
          }
        },
        {
          "id": "2f106ce6-764d-4d53-8f48-5cde2e766dba",
          "name": "Time*150",
          "description": "",
          "label": "TIME150",
          "color": "rgba(74, 144, 226, 1)",
          "iconType": "text",
          "svg": "",
          "sourceType": 0,
          "action": {
            "kind": "expression",
            "value": "value+time*150"
          }
        },
        {
          "id": "9d5d2393-889b-4589-87c8-59ab8db00937",
          "name": "Prop Wiggle",
          "description": "",
          "label": "PROP WIG",
          "color": "rgba(74, 144, 226, 1)",
          "iconType": "text",
          "svg": "",
          "sourceType": 0,
          "action": {
            "kind": "expression",
            "value": "w=wiggle(1,20); [w[0],w[0]]"
          }
        },
        {
          "id": "bc6eda0c-0a32-4ed7-a491-93a338f2f7d2",
          "name": "Bounce",
          "description": "",
          "label": "BOUNCE",
          "color": "rgba(74, 144, 226, 1)",
          "iconType": "text",
          "svg": "",
          "sourceType": 0,
          "action": {
            "kind": "expression",
            "value": "n = 0;\n \nif (numKeys > 0){\n \nn = nearestKey(time).index;\n \nif (key(n).time > time){\n \nn--;\n \n}\n \n}\n \nif (n == 0){\n \nt = 0;\n \n}else{\n \nt = time - key(n).time;\n \n}\n \nif (n > 0){\n \nv = velocityAtTime(key(n).time - thisComp.frameDuration/10);\n \namp = 1;\n \nfreq = 2;\n \ndecay = 8;\n \nM=Math.sin(freq*t*2*Math.PI)/Math.exp(decay*t); \nvalue + v*amp*M;\n \n}else{\n \nvalue;\n \n}"
          }
        },
        {
          "id": "8006c300-e8ef-452f-affc-fd1827247d8d",
          "name": "Thumb",
          "description": "",
          "label": "80X80",
          "color": "#b6b6b6",
          "iconType": "text",
          "svg": "",
          "sourceType": 2,
          "action": {
            "kind": "scriptlet",
            "value": "// Search for the \"Tr\" folder in project items\nvar folder = null;\nfor (var i = 1; i <= app.project.numItems; i++) {\n  var item = app.project.item(i);\n  if (item instanceof FolderItem && item.name === \"Tr\") {\n    folder = item;\n    break;\n  }\n}\n\n// Create the \"Thumb\" composition\nvar thumb = app.project.items.addComp(\"Thumb\", 80, 80, 1, 30, 24);\n\n// Check if the \"Tr\" folder exists\nif (folder !== null) {\n  // Add the \"Thumb\" composition to the \"Tr\" folder\n  thumb.parentFolder = folder;\n} else {\n  // Create the \"Tr\" folder\n  folder = app.project.items.addFolder(\"Tr\");\n  // Add the \"Thumb\" composition to the project items\n  thumb.parentFolder = folder;\n}\n\n// Open the \"Thumb\" composition in the viewer\nthumb.openInViewer();\n"
          }
        },
        {
          "id": "0901e94c-4f4f-4b11-bcf0-fd15f7c1b92a",
          "name": "BlurMap",
          "description": "",
          "label": "BLURMAP",
          "color": "#b6b6b6",
          "iconType": "text",
          "svg": "",
          "sourceType": 2,
          "action": {
            "kind": "scriptlet",
            "value": "app.beginUndoGroup(\"Create Blur Map Comp\");\n//начала кода\n\n            var comp = app.project.activeItem;\n            \n            var adj = comp.layers.addSolid([1,1,1], \"Blur\",  comp.width, comp.height, comp.pixelAspect, comp.duration);\n            adj.source.name =\"Blur\";    \n            adj.adjustmentLayer = true;\n            adj.label = 2;\n            adj.moveToBeginning();\n            var camblur = adj.property(\"Effects\").addProperty(\"Camera Lens Blur\");\n            camblur.property(\"Shape\").setValue([1]);\n            camblur.property(\"Roundness\").setValue([100]);\n            \n                                  \n            var bm = app.project.items.addComp(\"BlurMap\", comp.width, comp.height, comp.pixelAspect, comp.duration, comp.frameRate);\n            bm.label = 0;\n            \n            var white = bm.layers.addSolid([1,1,1], \"White\",  bm.width, bm.height, bm.pixelAspect, bm.duration);\n            white.source.name =\"White\";\n            var shape = bm.layers.addShape();\n            var rec = shape.property(\"Contents\");\n            \n            var size = rec.addProperty(\"ADBE Vector Shape - Ellipse\");\n            size.property(\"Size\").setValue([bm.width,bm.height]);\n            var fill = rec.addProperty(\"ADBE Vector Graphic - Fill\");\n            fill.property(\"Color\").setValue([0,0,0]);\n            shape.name =\"Black Rec\";\n            \n            var blur = shape.property(\"Effects\").addProperty(\"Box Blur\");\n            if (bm.width<1920) {\n            blur.property(1).setValue(130);\n            }else{\n                    blur.property(1).setValue(250);\n                }\n           var imp = comp.layers.add(bm);\n           imp.enabled = false;\n           imp.moveToEnd();\n           camblur.property(\"Layer\").setValue(imp.index);\n\n//конец кода\n    \n    \n    app.endUndoGroup;"
          }
        },
        {
          "id": "a21378f4-d46c-4d25-8e24-d8cd3ef4a64d",
          "name": "Comp Setter",
          "description": "",
          "label": "▦",
          "color": "rgba(189, 16, 224, 1)",
          "iconType": "symbol",
          "svg": "",
          "sourceType": 1,
          "action": {
            "kind": "externalScript",
            "value": "C:/Program Files/Adobe/Adobe After Effects 2022/Support Files/Scripts/rd_CompSetter.jsx"
          }
        },
        {
          "id": "27f38ce3-5cc8-4d5a-8a39-df1b4b44f073",
          "name": "Duplicator",
          "description": "",
          "label": "▣",
          "color": "rgba(189, 16, 224, 1)",
          "iconType": "symbol",
          "svg": "",
          "sourceType": 1,
          "action": {
            "kind": "externalScript",
            "value": "C:/Program Files/Adobe/Adobe After Effects 2022/Support Files/Scripts/True Comp Duplicator.jsx"
          }
        },
        {
          "id": "c09417f9-fb02-4e54-9059-f09a8a6d4aa3",
          "name": "Griddler",
          "description": "",
          "label": "▦",
          "color": "rgba(248, 231, 28, 1)",
          "iconType": "symbol",
          "svg": "",
          "sourceType": 1,
          "action": {
            "kind": "externalScript",
            "value": "C:/Program Files/Adobe/Adobe After Effects 2022/Support Files/Scripts/Gridder.jsx"
          }
        },
        {
          "id": "e464b81e-62a2-43a8-8634-1973c15df5c6",
          "name": "Utility Box",
          "description": "",
          "label": "UTILBOX",
          "color": "rgba(255, 255, 255, 1)",
          "iconType": "text",
          "svg": "",
          "sourceType": 1,
          "action": {
            "kind": "externalScript",
            "value": "C:/Program Files/Adobe/Adobe After Effects CC 2022/Support Files/Scripts/ScriptUI Panels/Utility_Box_v1.5.jsxbin"
          }
        },
        {
          "id": "00ab7232-c4a6-45b9-9ef7-d962cc501ad2",
          "name": "MBRO",
          "description": "",
          "label": "♨",
          "color": "rgba(255, 92, 0, 1)",
          "iconType": "symbol",
          "svg": "",
          "sourceType": 6,
          "action": {
            "kind": "extension",
            "value": "MotionBro"
          }
        },
        {
          "id": "dab1553e-5df7-4c29-bf99-2176c83a01f3",
          "name": "Clean RQ",
          "description": "",
          "label": "▥",
          "color": "rgba(255, 255, 255, 1)",
          "iconType": "symbol",
          "svg": "",
          "sourceType": 2,
          "action": {
            "kind": "scriptlet",
            "value": "for(var i=app.project.renderQueue.numItems; i >= 1 ; i--){\n            \n            var thisitem = app.project.renderQueue.item(i);\n                \n                thisitem.remove();\n                    }"
          }
        },
        {
          "id": "eb307039-470e-4712-b77e-03e5c1efa083",
          "name": "FInd and Replace Expressions",
          "description": "",
          "label": "↔",
          "color": "rgba(80, 227, 194, 1)",
          "iconType": "symbol",
          "svg": "",
          "sourceType": 1,
          "action": {
            "kind": "externalScript",
            "value": "C:/Program Files/Adobe/Adobe After Effects CC 2022/Support Files/Scripts/find_and_replace_menu.jsx"
          }
        },
        {
          "id": "com.khanyu.kbar.config",
          "name": "KBar Settings",
          "description": "",
          "label": "⚙",
          "color": "#b6b6b6",
          "iconType": "symbol",
          "svg": "",
          "sourceType": 6,
          "action": {
            "kind": "extension",
            "value": "com.khanyu.kbar.config"
          }
        },
        {
          "id": "c8733405-0351-4e4a-b052-49aa3976f653",
          "name": "Autotrace",
          "description": "",
          "label": "✂",
          "color": "#b6b6b6",
          "iconType": "symbol",
          "svg": "",
          "sourceType": 5,
          "action": {
            "kind": "menuCommand",
            "value": "Auto-trace..."
          }
        },
        {
          "id": "48fc6022-cecf-42b2-9f45-e63c67eccf2b",
          "name": "AtomX",
          "description": "",
          "label": "✳",
          "color": "rgba(36, 162, 255, 1)",
          "iconType": "symbol",
          "svg": "",
          "sourceType": 6,
          "action": {
            "kind": "extension",
            "value": "Atom"
          }
        },
        {
          "id": "ae746635-120e-48d5-9a16-f8c28c0a0c6a",
          "name": "wiggle basic",
          "description": "",
          "label": "WB",
          "color": "#b6b6b6",
          "iconType": "text",
          "svg": "",
          "sourceType": 0,
          "action": {
            "kind": "expression",
            "value": "wiggle(1,20)"
          }
        },
        {
          "id": "0c01fc7b-1479-45ad-a440-ba133f5f8ee7",
          "name": "wiggle loop",
          "description": "",
          "label": "WIGLOOP",
          "color": "#b6b6b6",
          "iconType": "text",
          "svg": "",
          "sourceType": 0,
          "action": {
            "kind": "expression",
            "value": "freq = 1;\namp = 10;\nloopTime = 4;\nt = time % loopTime;\nwiggle1 = wiggle(freq, amp, 1, 0.5, t);\nwiggle2 = wiggle(freq, amp, 1, 0.5, t - loopTime);\nlinear(t, 0,  loopTime, wiggle1, wiggle2)"
          }
        },
        {
          "id": "8237f0d9-86a3-4a7f-a1bf-38fb41a0de31",
          "name": "Reduce Project",
          "description": "",
          "label": "▣",
          "color": "rgba(255, 0, 0, 1)",
          "iconType": "symbol",
          "svg": "",
          "sourceType": 5,
          "action": {
            "kind": "menuCommand",
            "value": "Reduce Project"
          }
        },
        {
          "id": "20f83113-88ff-4035-abf8-a4bbb2ad59b7",
          "name": "Project Items Renamer",
          "description": "",
          "label": "RENAMER",
          "color": "rgba(80, 227, 194, 1)",
          "iconType": "text",
          "svg": "",
          "sourceType": 1,
          "action": {
            "kind": "externalScript",
            "value": "C:/Program Files/Adobe/Adobe After Effects CC 2022/Support Files/Scripts/Project_Items_Renamer.jsx"
          }
        },
        {
          "id": "abed6bc9-3418-4a58-9465-4e32c4010f25",
          "name": "NoExp",
          "description": "",
          "label": "NOEXP",
          "color": "#b6b6b6",
          "iconType": "text",
          "svg": "",
          "sourceType": 2,
          "action": {
            "kind": "scriptlet",
            "value": "// Check if there is at least one selected layer\nif (app.project.activeItem && app.project.activeItem instanceof CompItem && app.project.activeItem.selectedLayers.length > 0) {\n  // Create a new undo group\n  app.beginUndoGroup(\"Delete Expressions\");\n\n  // Loop through all selected layers\n  for (var i = 0; i < app.project.activeItem.selectedLayers.length; i++) {\n    var layer = app.project.activeItem.selectedLayers[i];\n    // Loop through all selected properties in the current layer\n    for (var j = 0; j < layer.selectedProperties.length; j++) {\n      var prop = layer.selectedProperties[j];\n      // Check if the property has an expression\n      if (prop.expression != null) {\n        // Delete the expression\n        prop.expression = \"\";\n      }\n    }\n  }\n\n  // End the undo group\n  app.endUndoGroup();\n}\n"
          }
        },
        {
          "id": "d44958bf-e6d1-4a56-a149-4812afd08f72",
          "name": "Exp+",
          "description": "",
          "label": "EX",
          "color": "#b6b6b6",
          "iconType": "text",
          "svg": "",
          "sourceType": 2,
          "action": {
            "kind": "scriptlet",
            "value": "// Create the UI panel\nvar panel = new Window(\"dialog\", \"Apply Expression\");\npanel.alignChildren = \"fill\";\n\n// Add a text input field to the panel\nvar textInput = panel.add(\"edittext\", undefined, \"\", {\n  multiline: true,\n  scrolling: true,\n  wantReturn: true,\n  size: [200, 200]\n});\ntextInput.active = true;\n\n// Add OK and Cancel buttons to the panel\nvar buttonGroup = panel.add(\"group\");\nbuttonGroup.alignment = \"right\";\nbuttonGroup.add(\"button\", undefined, \"OK\");\nbuttonGroup.add(\"button\", undefined, \"Cancel\");\n\n// Show the panel and wait for user input\nvar result = panel.show();\n\n// Check if the user clicked OK\nif (result == 1) {\n  // Get the expression code from the text input field\n  var expressionCode = textInput.text;\n\n  // Check if the user entered any text\n  if (expressionCode == \"\") {\n    alert(\"No expression code entered.\");\n  } else {\n    // Encode the expression code string\n    var encodedCode = encodeURIComponent(expressionCode);\n\n    // Create a new undo group\n    app.beginUndoGroup(\"Apply Expression\");\n\n    // Loop through all selected layers\n    for (var i = 0; i < app.project.activeItem.selectedLayers.length; i++) {\n      var layer = app.project.activeItem.selectedLayers[i];\n      // Loop through all selected properties in the current layer\n      for (var j = 0; j < layer.selectedProperties.length; j++) {\n        var prop = layer.selectedProperties[j];\n        // Decode and apply the expression code to the current property\n        prop.expression = decodeURIComponent(encodedCode);\n      }\n    }\n\n    // End the undo group\n    app.endUndoGroup();\n  }\n}\n"
          }
        },
        {
          "id": "ea7a2d10-8944-4a7d-8f10-704fe5db608e",
          "name": "SIN",
          "description": "",
          "label": "SIN",
          "color": "#b6b6b6",
          "iconType": "text",
          "svg": "",
          "sourceType": 0,
          "action": {
            "kind": "expression",
            "value": "// Base position\nstartPos = value; // исходная позиция слоя\n\n// Parameters\nfreq = 1; // Frequency in Hz (cycles per second)\namp = 20; // Amplitude in pixels\n\n// Calculate sine wave based on time\nsinWave = Math.sin(time * freq * 2 * Math.PI);\n\n// Final position\n[startPos[0] + sinWave * amp, startPos[1]]\n"
          }
        },
        {
          "id": "95f1aef0-6f89-41ac-b025-d1f3e1a02202",
          "name": "Circle",
          "description": "",
          "label": "CIRCLE",
          "color": "#b6b6b6",
          "iconType": "text",
          "svg": "",
          "sourceType": 0,
          "action": {
            "kind": "expression",
            "value": "center = value;\nradius = 100;\nzalupa = 1; // обратное кручение -1\n\nangle = time * zalupa * 2 * Math.PI;\n\nx = Math.cos(angle) * radius;\ny = Math.sin(angle) * radius;\n\ncenter + [x, y]"
          }
        },
        {
          "id": "be660fd8-3d9c-4579-a566-9de248805f82",
          "name": "Text to Shape",
          "description": "",
          "label": "TTS",
          "color": "#b6b6b6",
          "iconType": "text",
          "svg": "",
          "sourceType": 5,
          "action": {
            "kind": "menuCommand",
            "value": "3781"
          }
        }
      ]
    },
    {
      "id": "3bc34cc9-1eed-40a2-9c49-aded45f7c875",
      "name": "Configure",
      "buttons": []
    },
    {
      "id": "4c8660c5-cf89-47dc-8310-e690343bc113",
      "name": "Layers",
      "buttons": []
    }
  ]
};
