import QtQuick
import FACTORY_QT

MouseArea {
    id: root

    // dependencies injected from Workspace
    property var scene          // sceneContainer
    property var nodes          // nodesRepeater
    property var conns          // connectionsRepeater
    property Item flick         // flickable
    property var menu           // placeMenu

    signal mouseMoved(real x, real y)

    function distToSegment(px, py, ax, ay, bx, by) {
        var dx = bx - ax, dy = by - ay;
        var lenSq = dx * dx + dy * dy;
        if (lenSq === 0)
            return Math.sqrt((px - ax) * (px - ax) + (py - ay) * (py - ay));
        var t = Math.max(0, Math.min(1, ((px - ax) * dx + (py - ay) * dy) / lenSq));
        var projX = ax + t * dx, projY = ay + t * dy;
        return Math.sqrt((px - projX) * (px - projX) + (py - projY) * (py - projY));
    }

    property var _hoveredConn: null

    function updateConnectionHover(mx, my) {
        if (!conns || scene.dragMode !== "") {
            if (_hoveredConn) { _hoveredConn.hovered = false; _hoveredConn = null; }
            return;
        }
        var found = null;
        for (var i = 0; i < conns.count; i++) {
            var item = conns.itemAt(i);
            if (!item || !item.conn || !item.src || !item.dst) continue;
            var ax = item.src.posX + item.conn.srcOffset.x;
            var ay = item.src.posY + item.conn.srcOffset.y;
            var bx = item.dst.posX + item.conn.dstOffset.x;
            var by = item.dst.posY + item.conn.dstOffset.y;
            if (distToSegment(mx, my, ax, ay, bx, by) < 6) { found = item; break; }
        }
        if (found === _hoveredConn) return;
        if (_hoveredConn) _hoveredConn.hovered = false;
        if (found) found.hovered = true;
        _hoveredConn = found;
    }

    anchors.fill: parent
    acceptedButtons: Qt.LeftButton | Qt.RightButton
    hoverEnabled: true
    preventStealing: true

    onPressed: mouse => {
        if (mouse.button === Qt.RightButton) {
            if (_hoveredConn) return;  // double-right-click will handle deletion
            var screenPt = root.mapToItem(flick, mouse.x, mouse.y);
            menu.x = screenPt.x;
            menu.y = screenPt.y;
            menu.spawnX = mouse.x;
            menu.spawnY = mouse.y;
            menu.open();
            return;
        }

        // 1. ports
        for (var i = 0; i < scene.portItems.length; i++) {
            var port = scene.portItems[i];
            var localPt = root.mapToItem(port, mouse.x, mouse.y);
            if (port.contains(localPt)) {
                var cp = port.connectionPoint();
                scene.pendingConn = {
                    srcNodeIdx: port.portData.nodeIndex,
                    srcPortIdx: port.portData.portIndex,
                    startX: cp.x,
                    startY: cp.y
                };
                scene.pendingMouseX = cp.x;
                scene.pendingMouseY = cp.y;
                scene.pendingTarget = null;
                scene.dragSourcePort = port;
                scene.dragMode = "port";
                return;
            }
        }

        // 2. nodes
        for (var j = 0; j < nodes.count; j++) {
            var loader = nodes.itemAt(j);
            if (mouse.x >= loader.x && mouse.x <= loader.x + loader.width &&
                mouse.y >= loader.y && mouse.y <= loader.y + loader.height) {
                scene.dragMode = "node";
                var isSelected = loader.item && loader.item.selected;
                var loaders = [];
                if (isSelected) {
                    for (var k = 0; k < nodes.count; k++) {
                        var l = nodes.itemAt(k);
                        if (l.item && l.item.selected)
                            loaders.push({ loader: l, offsetX: mouse.x - l.nodeData.posX, offsetY: mouse.y - l.nodeData.posY });
                    }
                } else {
                    loaders.push({ loader: loader, offsetX: mouse.x - loader.nodeData.posX, offsetY: mouse.y - loader.nodeData.posY });
                }
                scene.dragLoaders = loaders;
                return;
            }
        }

        // 3. empty space
        if (mouse.modifiers & Qt.ShiftModifier) {
            scene.dragMode = "marquee";
            scene.marqueeStartX = mouse.x;
            scene.marqueeStartY = mouse.y;
            scene.marqueeEndX = mouse.x;
            scene.marqueeEndY = mouse.y;
        } else {
            for (var c = 0; c < nodes.count; c++) {
                var n = nodes.itemAt(c);
                if (n.item) n.item.selected = false;
            }
            mouse.accepted = false;
        }
    }

    onPositionChanged: mouse => {
        root.mouseMoved(mouse.x, mouse.y);
        updateConnectionHover(mouse.x, mouse.y);

        var mode = scene.dragMode;
        if (mode === "port") {
            scene.pendingMouseX = mouse.x;
            scene.pendingMouseY = mouse.y;
            scene.pendingTarget = null;
            for (var i = 0; i < scene.portItems.length; i++) {
                var port = scene.portItems[i];
                if (port === scene.dragSourcePort) continue;
                var localPt = root.mapToItem(port, mouse.x, mouse.y);
                if (port.contains(localPt)) {
                    scene.pendingTarget = port.portData;
                    break;
                }
            }
        } else if (mode === "node") {
            for (var d = 0; d < scene.dragLoaders.length; d++) {
                var entry = scene.dragLoaders[d];
                entry.loader.nodeData.posX = mouse.x - entry.offsetX;
                entry.loader.nodeData.posY = mouse.y - entry.offsetY;
            }
        } else if (mode === "marquee") {
            scene.marqueeEndX = mouse.x;
            scene.marqueeEndY = mouse.y;
        }
    }

    onReleased: mouse => {
        if (scene.dragMode === "port") {
            scene.endPortDrag();
        } else if (scene.dragMode === "marquee") {
            var x1 = Math.min(scene.marqueeStartX, scene.marqueeEndX);
            var y1 = Math.min(scene.marqueeStartY, scene.marqueeEndY);
            var x2 = Math.max(scene.marqueeStartX, scene.marqueeEndX);
            var y2 = Math.max(scene.marqueeStartY, scene.marqueeEndY);
            for (var i = 0; i < nodes.count; i++) {
                var loader = nodes.itemAt(i);
                var inside = loader.x < x2 && loader.x + loader.width > x1 &&
                             loader.y < y2 && loader.y + loader.height > y1;
                if (loader.item) loader.item.selected = inside;
            }
        }
        scene.dragMode = "";
        scene.dragLoaders = [];
        scene.dragSourcePort = null;
    }

    onDoubleClicked: mouse => {
        if (mouse.button === Qt.RightButton) {
            if (_hoveredConn) {
                SceneManager.deleteConnection(_hoveredConn.conn);
                _hoveredConn = null;
            }
            return;
        }

        for (var i = 0; i < nodes.count; i++) {
            var loader = nodes.itemAt(i);
            if (mouse.x >= loader.x && mouse.x <= loader.x + loader.width &&
                mouse.y >= loader.y && mouse.y <= loader.y + loader.height) {
                if (loader.nodeData && loader.nodeData.nodeType === 2)
                    SceneManager.enterFactory(loader.nodeData);
                return;
            }
        }
    }
}
