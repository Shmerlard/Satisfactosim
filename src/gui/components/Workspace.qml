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

            SceneInputHandler {
                scene: sceneContainer
                nodes: nodesRepeater
                flick: flickable
                menu: placeMenu
                onMouseMoved: (x, y) => {
                    root.mouseContentX = Math.floor(x);
                    root.mouseContentY = Math.floor(y);
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
