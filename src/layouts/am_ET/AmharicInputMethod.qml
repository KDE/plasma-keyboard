/*
    SPDX-FileCopyrightText: 2026 Abenezer Wesenseged <wseged@proton.me>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.VirtualKeyboard as VKB

VKB.InputMethod {

    property var fidelMap: ({
            "\u1200": ["\u1200", "\u1201", "\u1202", "\u1203", "\u1204", "\u1205", "\u1206", "\u1207"],
            "\u1208": ["\u1208", "\u1209", "\u120A", "\u120B", "\u120C", "\u120D", "\u120E", "\u120F"],
            "\u1210": ["\u1210", "\u1211", "\u1212", "\u1213", "\u1214", "\u1215", "\u1216", "\u1217"],
            "\u1218": ["\u1218", "\u1219", "\u121A", "\u121B", "\u121C", "\u121D", "\u121E", "\u121F"],
            "\u1220": ["\u1220", "\u1221", "\u1222", "\u1223", "\u1224", "\u1225", "\u1226", "\u1227"],
            "\u1228": ["\u1228", "\u1229", "\u122A", "\u122B", "\u122C", "\u122D", "\u122E", "\u122F"],
            "\u1230": ["\u1230", "\u1231", "\u1232", "\u1233", "\u1234", "\u1235", "\u1236", "\u1237"],
            "\u1238": ["\u1238", "\u1239", "\u123A", "\u123B", "\u123C", "\u123D", "\u123E", "\u123F"],
            "\u1240": ["\u1240", "\u1241", "\u1242", "\u1243", "\u1244", "\u1245", "\u1246", "\u1247"],
            "\u1260": ["\u1260", "\u1261", "\u1262", "\u1263", "\u1264", "\u1265", "\u1266", "\u1267"],
            "\u1268": ["\u1268", "\u1269", "\u126A", "\u126B", "\u126C", "\u126D", "\u126E", "\u126F"],
            "\u1270": ["\u1270", "\u1271", "\u1272", "\u1273", "\u1274", "\u1275", "\u1276", "\u1277"],
            "\u1278": ["\u1278", "\u1279", "\u127A", "\u127B", "\u127C", "\u127D", "\u127E", "\u127F"],
            "\u1280": ["\u1280", "\u1281", "\u1282", "\u1283", "\u1284", "\u1285", "\u1286", "\u1287"],
            "\u1290": ["\u1290", "\u1291", "\u1292", "\u1293", "\u1294", "\u1295", "\u1296", "\u1297"],
            "\u1298": ["\u1298", "\u1299", "\u129A", "\u129B", "\u129C", "\u129D", "\u129E", "\u129F"],
            "\u12A0": ["\u12A0", "\u12A1", "\u12A2", "\u12A3", "\u12A4", "\u12A5", "\u12A6", "\u12A7"],
            "\u12A8": ["\u12A8", "\u12A9", "\u12AA", "\u12AB", "\u12AC", "\u12AD", "\u12AE", "\u12AF"],
            "\u12B0": ["\u12B0", "\u12B1", "\u12B2", "\u12B3", "\u12B4", "\u12B5", "\u12B6", "\u12B7"],
            "\u12B8": ["\u12B8", "\u12B9", "\u12BA", "\u12BB", "\u12BC", "\u12BD", "\u12BE", "\u12C3"],
            "\u12C8": ["\u12C8", "\u12C9", "\u12CA", "\u12CB", "\u12CC", "\u12CD", "\u12CE", "\u12CF"],
            "\u12D0": ["\u12D0", "\u12D1", "\u12D2", "\u12D3", "\u12D4", "\u12D5", "\u12D6"],
            "\u12D8": ["\u12D8", "\u12D9", "\u12DA", "\u12DB", "\u12DC", "\u12DD", "\u12DE", "\u12DF"],
            "\u12E0": ["\u12E0", "\u12E1", "\u12E2", "\u12E3", "\u12E4", "\u12E5", "\u12E6", "\u12E7"],
            "\u12E8": ["\u12E8", "\u12E9", "\u12EA", "\u12EB", "\u12EC", "\u12ED", "\u12EE", "\u12EF"],
            "\u12F0": ["\u12F0", "\u12F1", "\u12F2", "\u12F3", "\u12F4", "\u12F5", "\u12F6", "\u12F7"],
            "\u1300": ["\u1300", "\u1301", "\u1302", "\u1303", "\u1304", "\u1305", "\u1306", "\u1307"],
            "\u1308": ["\u1308", "\u1309", "\u130A", "\u130B", "\u130C", "\u130D", "\u130E", "\u130F"],
            "\u1320": ["\u1320", "\u1321", "\u1322", "\u1323", "\u1324", "\u1325", "\u1326", "\u1327"],
            "\u1328": ["\u1328", "\u1329", "\u132A", "\u132B", "\u132C", "\u132D", "\u132E", "\u132F"],
            "\u1330": ["\u1330", "\u1331", "\u1332", "\u1333", "\u1334", "\u1335", "\u1336", "\u1337"],
            "\u1338": ["\u1338", "\u1339", "\u133A", "\u133B", "\u133C", "\u133D", "\u133E", "\u133F"],
            "\u1340": ["\u1340", "\u1341", "\u1342", "\u1343", "\u1344", "\u1345", "\u1346", "\u1347"],
            "\u1348": ["\u1348", "\u1349", "\u134A", "\u134B", "\u134C", "\u134D", "\u134E", "\u134F"],
            "\u1350": ["\u1350", "\u1351", "\u1352", "\u1353", "\u1354", "\u1355", "\u1356", "\u1357"]
        })

    property var candidates: []
    property string currentBase: ""

    function inputModes(locale) {
        return [VKB.InputEngine.InputMode.Latin];
    }

    function setInputMode(locale, inputMode) {
        return true;
    }

    function keyEvent(key, text, modifiers) {
        if (inputContext.preeditText.length > 0) {
            inputContext.commit();
        }

        if (fidelMap[text] !== undefined) {
            currentBase = text;
            candidates = fidelMap[text];
            inputContext.preeditText = text;

            selectionListChanged(VKB.SelectionListModel.Type.WordCandidateList);

            return true;
        }

        return false;
    }

    function selectionLists() {
        return [VKB.SelectionListModel.Type.WordCandidateList];
    }

    function selectionListItemCount(type) {
        if (type == VKB.SelectionListModel.Type.WordCandidateList) {
            return candidates.length;
        }
        return 0;
    }

    function selectionListData(type, index, role) {
        var result = null;
        if (type == VKB.SelectionListModel.Type.WordCandidateList) {
            switch (role) {
            case VKB.SelectionListModel.Role.Display:
                result = candidates[index];
                break;
            default:
                break;
            }
        }
        return result;
    }

    function selectionListItemSelected(type, index) {
        if (type === VKB.SelectionListModel.Type.WordCandidateList) {
            inputContext.commit(candidates[index]);
            update();
        }
    }
}
