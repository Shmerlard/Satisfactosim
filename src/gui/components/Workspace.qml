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
            function onZoomScaleChanged() {
                gridCanvas.requestPaint();
            }
        }
        Connections {
            target: flickable
            function onContentXChanged() {
                gridCanvas.requestPaint();
            }
            function onContentYChanged() {
                gridCanvas.requestPaint();
            }
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
                ctx.beginPath();
                ctx.moveTo(x, 0);
                ctx.lineTo(x, height);
                ctx.stroke();
            }
            for (var y = startY; y <= height; y += cellSize) {
                ctx.beginPath();
                ctx.moveTo(0, y);
                ctx.lineTo(width, y);
                ctx.stroke();
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

            property var pendingConn: null
            property var pendingTarget: null
            property real pendingMouseX: 0
            property real pendingMouseY: 0
            property var portItems: []

            function startPortDrag(nodeIdx, portIdx, startX, startY) {
                pendingConn = {
                    srcNodeIdx: nodeIdx,
                    srcPortIdx: portIdx,
                    startX: startX,
                    startY: startY
                };
                pendingMouseX = startX;
                pendingMouseY = startY;
            }
            function endPortDrag() {
                if (pendingConn && pendingTarget) {
                    sceneManager.connectNodes(pendingConn.srcNodeIdx, pendingConn.srcPortIdx, pendingTarget.nodeIndex, pendingTarget.portIndex);
                }
                pendingConn = null;
                pendingTarget = null;
            }
            // function endPortDrag(mouseX, mouseY) {
            //     if (pendingConn && pendingTarget) {
            //         // console.log("endPortDrag at", mouseX, mouseY);
            //         var target = sceneManager.portAtPosition(mouseX, mouseY);
            //         console.log("target:", JSON.stringify(target));
            //         if (target && target.nodeIndex !== undefined) {
            //             console.log("connecting", pendingConn.srcNodeIdx, pendingConn.srcPortIdx, "->", target.nodeIndex, target.portIndex);
            //             sceneManager.connectNodes(pendingConn.srcNodeIdx, pendingConn.srcPortIdx, target.nodeIndex, target.portIndex);
            //         }
            //     }
            //     pendingConn = null;
            // }

            Shape {
                visible: sceneContainer.pendingConn !== null
                z: 5
                ShapePath {
                    strokeColor: "white"
                    strokeWidth: 2
                    fillColor: "transparent"
                    // startX: pendingConn ? pendingConn.startX : 0
                    // startY: pendingConn ? pendingConn.startY : 0
                    startX: sceneContainer.pendingConn ? sceneContainer.pendingConn.startX : 0
                    startY: sceneContainer.pendingConn ? sceneContainer.pendingConn.startY : 0
                    PathLine {
                        x: sceneContainer.pendingMouseX
                        y: sceneContainer.pendingMouseY
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
                    property var nodeData: model["nodeData"]
                    x: nodeData ? nodeData.posX : 0
                    y: nodeData ? nodeData.posY : 0
                    source: {
                        if (!nodeData)
                            return "";
                        switch (nodeData.nodeType) {
                        case 1:
                            return "nodes/EdgeNode.qml";
                        case 2:
                            return "nodes/FactoryNode.qml";
                        case 3:
                            return "nodes/ExtractionNode.qml";
                        case 4:
                            return "nodes/ProductionNode.qml";
                        default:
                            return "";
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

    Item {
        id: gui
        anchors.fill: flickable
        Notification {
            id: notification
        }
        BackButton {
            id: backButton
        }
        MousePos {
            id: mousePos
            mouseContentX: root.mouseContentX
            mouseContentY: root.mouseContentY
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.RightButton | Qt.LeftButton
            hoverEnabled: true

            onPositionChanged: mouse => {
                root.mouseContentX = Math.floor((mouse.x + flickable.contentX) / root.zoomScale);
                root.mouseContentY = Math.floor((mouse.y + flickable.contentY) / root.zoomScale);
            }

            onPressed: mouse => {
                if (mouse.button === Qt.LeftButton)
                    mouse.accepted = false;
            }
            onClicked: mouse => {
                if (mouse.button == Qt.RightButton) {
                    placeMenu.x = mouse.x;
                    placeMenu.y = mouse.y;
                    placeMenu.spawnX = root.mouseContentX;
                    placeMenu.spawnY = root.mouseContentY;
                    placeMenu.open();
                }
            // if (mouse.button == Qt.LeftButton) {
            //     mouse.accepted = false;
            //     return;
            // }
            }
        }

        WheelHandler {
            acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
            onWheel: event => {
                let zoomFactor = event.angleDelta.y > 0 ? 1.1 : 0.9;
                let oldScale = root.zoomScale;
                let newScale = Math.min(Math.max(oldScale * zoomFactor, 0.1), 3.0);

                if (newScale !== oldScale) {
                    let actualFactor = newScale / oldScale;

                    let mouseX = (flickable.contentX + point.position.x);
                    let mouseY = (flickable.contentY + point.position.y);

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
