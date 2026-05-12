import QtQuick
import QtQuick.Shapes
import QtQuick.Controls
import FACTORY_QT
import "./menus"
import "./nodes"

Item {
    id: root
    property real zoomScale: 1.0
    property int sceneSize: 5000
    property real mouseContentX: 0
    property real mouseContentY: 0
    property alias contentX: flickable.contentX
    property alias contentY: flickable.contentY

    PlaceMenu {
        id: placeMenu
    }

    Rectangle {
        anchors.fill: parent
        color: "#1a1a1a"
    }

    Canvas {
        id: gridCanvas
        anchors.fill: parent

        onWidthChanged: requestPaint()
        onHeightChanged: requestPaint()

        Connections {
            target: root
            function onZoomScaleChanged() { gridCanvas.requestPaint() }
        }
        Connections {
            target: flickable
            function onContentXChanged() { gridCanvas.requestPaint() }
            function onContentYChanged() { gridCanvas.requestPaint() }
        }

        onPaint: {
            var ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height);

            var cellSize = 50 * root.zoomScale;
            var startX = (-(flickable.contentX % cellSize) + cellSize) % cellSize;
            var startY = (-(flickable.contentY % cellSize) + cellSize) % cellSize;

            ctx.strokeStyle = "#333333";
            ctx.lineWidth = 1;

            for (var x = startX; x <= width; x += cellSize) {
                ctx.beginPath(); ctx.moveTo(x, 0); ctx.lineTo(x, height); ctx.stroke();
            }
            for (var y = startY; y <= height; y += cellSize) {
                ctx.beginPath(); ctx.moveTo(0, y); ctx.lineTo(width, y); ctx.stroke();
            }
        }
    }

    Flickable {
        id: flickable
        anchors.fill: parent
        contentWidth: root.sceneSize * root.zoomScale
        contentHeight: root.sceneSize * root.zoomScale
        boundsBehavior: Flickable.StopAtBounds
        clip: true

        Item {
            id: sceneContainer
            width: root.sceneSize
            height: root.sceneSize
            scale: root.zoomScale
            transformOrigin: Item.TopLeft

            // --- drag state ---
            property string dragMode: ""       // "node" | "port" | "marquee"
            property var dragLoaders: []        // [{loader, offsetX, offsetY}]
            property var dragSourcePort: null

            // --- port connection ---
            property var pendingConn: null
            property var pendingTarget: null
            property real pendingMouseX: 0
            property real pendingMouseY: 0

            // --- registries ---
            property var portItems: []

            // --- marquee ---
            property real marqueeStartX: 0
            property real marqueeStartY: 0
            property real marqueeEndX: 0
            property real marqueeEndY: 0

            function endPortDrag() {
                if (pendingConn && pendingTarget) {
                    sceneManager.connectNodes(
                        pendingConn.srcNodeIdx, pendingConn.srcPortIdx,
                        pendingTarget.nodeIndex, pendingTarget.portIndex
                    );
                }
                pendingConn = null;
                pendingTarget = null;
            }

            // pending connection line
            Shape {
                visible: sceneContainer.pendingConn !== null
                z: 5
                ShapePath {
                    strokeColor: "white"
                    strokeWidth: 2
                    fillColor: "transparent"
                    startX: sceneContainer.pendingConn ? sceneContainer.pendingConn.startX : 0
                    startY: sceneContainer.pendingConn ? sceneContainer.pendingConn.startY : 0
                    PathLine {
                        x: sceneContainer.pendingMouseX
                        y: sceneContainer.pendingMouseY
                    }
                }
            }

            // marquee rectangle
            Rectangle {
                visible: sceneContainer.dragMode === "marquee"
                x: Math.min(sceneContainer.marqueeStartX, sceneContainer.marqueeEndX)
                y: Math.min(sceneContainer.marqueeStartY, sceneContainer.marqueeEndY)
                width: Math.abs(sceneContainer.marqueeEndX - sceneContainer.marqueeStartX)
                height: Math.abs(sceneContainer.marqueeEndY - sceneContainer.marqueeStartY)
                color: "#224fc3f7"
                border.color: "#4fc3f7"
                border.width: 1
                z: 10
            }

            MouseArea {
                id: sceneMouseArea
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                hoverEnabled: true
                preventStealing: true

                onPressed: mouse => {
                    if (mouse.button === Qt.RightButton) {
                        var screenPt = sceneMouseArea.mapToItem(flickable, mouse.x, mouse.y);
                        placeMenu.x = screenPt.x;
                        placeMenu.y = screenPt.y;
                        placeMenu.spawnX = mouse.x;
                        placeMenu.spawnY = mouse.y;
                        placeMenu.open();
                        return;
                    }

                    // 1. ports take priority
                    for (var i = 0; i < sceneContainer.portItems.length; i++) {
                        var port = sceneContainer.portItems[i];
                        var localPt = sceneMouseArea.mapToItem(port, mouse.x, mouse.y);
                        if (port.contains(localPt)) {
                            var cp = port.connectionPoint();
                            sceneContainer.pendingConn = {
                                srcNodeIdx: port.portData.nodeIndex,
                                srcPortIdx: port.portData.portIndex,
                                startX: cp.x,
                                startY: cp.y
                            };
                            sceneContainer.pendingMouseX = cp.x;
                            sceneContainer.pendingMouseY = cp.y;
                            sceneContainer.pendingTarget = null;
                            sceneContainer.dragSourcePort = port;
                            sceneContainer.dragMode = "port";
                            return;
                        }
                    }

                    // 2. nodes
                    for (var j = 0; j < nodesRepeater.count; j++) {
                        var loader = nodesRepeater.itemAt(j);
                        if (mouse.x >= loader.x && mouse.x <= loader.x + loader.width &&
                            mouse.y >= loader.y && mouse.y <= loader.y + loader.height) {
                            sceneContainer.dragMode = "node";
                            var isSelected = loader.item && loader.item.selected;
                            var loaders = [];
                            if (isSelected) {
                                for (var k = 0; k < nodesRepeater.count; k++) {
                                    var l = nodesRepeater.itemAt(k);
                                    if (l.item && l.item.selected)
                                        loaders.push({ loader: l, offsetX: mouse.x - l.nodeData.posX, offsetY: mouse.y - l.nodeData.posY });
                                }
                            } else {
                                loaders.push({ loader: loader, offsetX: mouse.x - loader.nodeData.posX, offsetY: mouse.y - loader.nodeData.posY });
                            }
                            sceneContainer.dragLoaders = loaders;
                            return;
                        }
                    }

                    // 3. empty space: shift = marquee, else clear selection + pan
                    if (mouse.modifiers & Qt.ShiftModifier) {
                        sceneContainer.dragMode = "marquee";
                        sceneContainer.marqueeStartX = mouse.x;
                        sceneContainer.marqueeStartY = mouse.y;
                        sceneContainer.marqueeEndX = mouse.x;
                        sceneContainer.marqueeEndY = mouse.y;
                    } else {
                        for (var c = 0; c < nodesRepeater.count; c++) {
                            var l = nodesRepeater.itemAt(c);
                            if (l.item) l.item.selected = false;
                        }
                        mouse.accepted = false;
                    }
                }

                onPositionChanged: mouse => {
                    root.mouseContentX = Math.floor(mouse.x);
                    root.mouseContentY = Math.floor(mouse.y);

                    var mode = sceneContainer.dragMode;
                    if (mode === "port") {
                        sceneContainer.pendingMouseX = mouse.x;
                        sceneContainer.pendingMouseY = mouse.y;
                        sceneContainer.pendingTarget = null;
                        for (var i = 0; i < sceneContainer.portItems.length; i++) {
                            var port = sceneContainer.portItems[i];
                            if (port === sceneContainer.dragSourcePort) continue;
                            var localPt = sceneMouseArea.mapToItem(port, mouse.x, mouse.y);
                            if (port.contains(localPt)) {
                                sceneContainer.pendingTarget = port.portData;
                                break;
                            }
                        }
                    } else if (mode === "node") {
                        for (var d = 0; d < sceneContainer.dragLoaders.length; d++) {
                            var entry = sceneContainer.dragLoaders[d];
                            entry.loader.nodeData.posX = mouse.x - entry.offsetX;
                            entry.loader.nodeData.posY = mouse.y - entry.offsetY;
                        }
                    } else if (mode === "marquee") {
                        sceneContainer.marqueeEndX = mouse.x;
                        sceneContainer.marqueeEndY = mouse.y;
                    }
                }

                onReleased: mouse => {
                    if (sceneContainer.dragMode === "port") {
                        sceneContainer.endPortDrag();
                    } else if (sceneContainer.dragMode === "marquee") {
                        var x1 = Math.min(sceneContainer.marqueeStartX, sceneContainer.marqueeEndX);
                        var y1 = Math.min(sceneContainer.marqueeStartY, sceneContainer.marqueeEndY);
                        var x2 = Math.max(sceneContainer.marqueeStartX, sceneContainer.marqueeEndX);
                        var y2 = Math.max(sceneContainer.marqueeStartY, sceneContainer.marqueeEndY);
                        for (var i = 0; i < nodesRepeater.count; i++) {
                            var loader = nodesRepeater.itemAt(i);
                            var inside = loader.x < x2 && loader.x + loader.width > x1 &&
                                         loader.y < y2 && loader.y + loader.height > y1;
                            if (loader.item) loader.item.selected = inside;
                        }
                    }
                    sceneContainer.dragMode = "";
                    sceneContainer.dragLoaders = [];
                    sceneContainer.dragSourcePort = null;
                }

                onDoubleClicked: mouse => {
                    for (var i = 0; i < nodesRepeater.count; i++) {
                        var loader = nodesRepeater.itemAt(i);
                        if (mouse.x >= loader.x && mouse.x <= loader.x + loader.width &&
                            mouse.y >= loader.y && mouse.y <= loader.y + loader.height) {
                            if (loader.nodeData && loader.nodeData.nodeType === 2)
                                sceneManager.enterFactory(loader.nodeData);
                            return;
                        }
                    }
                }
            }

            Repeater {
                id: connectionsRepeater
                model: sceneManager.connections
                delegate: Connection {
                    conn: modelData
                }
            }

            Repeater {
                id: nodesRepeater
                model: sceneManager.model
                delegate: Loader {
                    id: nodeLoader
                    property var nodeData: model["nodeData"]
                    x: nodeData ? nodeData.posX : 0
                    y: nodeData ? nodeData.posY : 0
                    source: {
                        if (!nodeData) return "";
                        switch (nodeData.nodeType) {
                        case 1: return "nodes/EdgeNode.qml";
                        case 2: return "nodes/FactoryNode.qml";
                        case 3: return "nodes/ExtractionNode.qml";
                        case 4: return "nodes/ProductionNode.qml";
                        default: return "";
                        }
                    }
                    onLoaded: {
                        item.nodeData = nodeData;
                        item.contentContainer = sceneContainer;
                        item.modelIndex = index;
                    }
                }
            }
        }
    }

    // screen-space overlays
    Item {
        id: gui
        anchors.fill: flickable

        Notification { id: notification }
        BackButton { id: backButton }
        MousePos {
            id: mousePos
            mouseContentX: root.mouseContentX
            mouseContentY: root.mouseContentY
        }

        WheelHandler {
            acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
            onWheel: event => {
                let zoomFactor = event.angleDelta.y > 0 ? 1.1 : 0.9;
                let oldScale = root.zoomScale;
                let newScale = Math.min(Math.max(oldScale * zoomFactor, 0.1), 3.0);

                if (newScale !== oldScale) {
                    let actualFactor = newScale / oldScale;
                    let mouseX = flickable.contentX + point.position.x;
                    let mouseY = flickable.contentY + point.position.y;
                    root.zoomScale = newScale;
                    let maxX = root.sceneSize * newScale - flickable.width;
                    let maxY = root.sceneSize * newScale - flickable.height;
                    flickable.contentX = Math.max(0, Math.min((mouseX * actualFactor) - point.position.x, maxX));
                    flickable.contentY = Math.max(0, Math.min((mouseY * actualFactor) - point.position.y, maxY));
                }
            }
        }
    }
}
