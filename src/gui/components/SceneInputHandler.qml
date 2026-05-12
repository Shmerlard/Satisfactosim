import QtQuick

MouseArea {
    id: root

    // dependencies injected from Workspace
    property var scene          // sceneContainer
    property var nodes          // nodesRepeater
    property Item flick         // flickable
    property var menu           // placeMenu

    signal mouseMoved(real x, real y)

    anchors.fill: parent
    acceptedButtons: Qt.LeftButton | Qt.RightButton
    hoverEnabled: true
    preventStealing: true

    onPressed: mouse => {
        if (mouse.button === Qt.RightButton) {
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
        for (var i = 0; i < nodes.count; i++) {
            var loader = nodes.itemAt(i);
            if (mouse.x >= loader.x && mouse.x <= loader.x + loader.width &&
                mouse.y >= loader.y && mouse.y <= loader.y + loader.height) {
                if (loader.nodeData && loader.nodeData.nodeType === 2)
                    sceneManager.enterFactory(loader.nodeData);
                return;
            }
        }
    }
}
