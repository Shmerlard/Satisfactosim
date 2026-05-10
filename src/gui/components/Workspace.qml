import QtQuick
import QtQuick.Controls
import FACTORY_QT
import "./menus"

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

    // Grid drawn in viewport space so it always fills the screen.
    // Offset is computed from contentX/contentY so lines stay aligned with scene coords.
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

            Rectangle {
                anchors.fill: parent
                color: "transparent"

                Repeater {
                    model: sceneManager.model
                    delegate: Loader {
                        property var nodeData: model["nodeData"]
                        x: nodeData ? nodeData.posX : 0
                        y: nodeData ? nodeData.posY : 0
                        source: {
                            if (!nodeData) return ""
                            switch (nodeData.nodeType) {
                                case 1: return "nodes/EdgeNode.qml"
                                case 2: return "nodes/FactoryNode.qml"
                                case 3: return "nodes/ExtractionNode.qml"
                                case 4: return "nodes/ProductionNode.qml"
                                default: return ""
                            }
                        }
                        onLoaded: {
                            item.nodeData = nodeData
                            item.contentContainer = sceneContainer
                            item.modelIndex = index
                        }
                    }
                }
            }
        }
    }

    // Overlay item sits above the Flickable in Z-order so its WheelHandler
    // grabs scroll events before the Flickable's internal handler does.
    Item {
        anchors.fill: flickable

        Rectangle {
            id: backButton
            visible: !sceneManager.isRootFactory
            width: 100
            height: 100
            color: "transparent"
            Image {
                source: "image://assets/misc/arrow-right-circle-solid.png"
                fillMode: Image.PreserveAspectFit
                anchors.fill: parent
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton
                containmentMask: Item {
                    width: backButton.width
                    height: backButton.height
                    function contains(p) {
                        var r = width / 2
                        return (p.x - r) * (p.x - r) + (p.y - r) * (p.y - r) <= r * r
                    }
                }
                onClicked: SessionManager.enterParentFactory()
            }
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.RightButton
            hoverEnabled: true

            onPositionChanged: mouse => {
                root.mouseContentX = Math.floor((mouse.x + flickable.contentX) / root.zoomScale);
                root.mouseContentY = Math.floor((mouse.y + flickable.contentY) / root.zoomScale);
            }

            onClicked: mouse => {
                if (mouse.button == Qt.RightButton) {
                    placeMenu.x = mouse.x
                    placeMenu.y = mouse.y
                    placeMenu.spawnX = mouse.x
                    placeMenu.spawnY = mouse.y
                    placeMenu.open()
                }
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

                    // Clamp against new bounds ourselves so Flickable can't
                    // silently clamp and shift the anchor point.
                    let maxX = root.sceneSize * newScale - flickable.width;
                    let maxY = root.sceneSize * newScale - flickable.height;
                    flickable.contentX = Math.max(0, Math.min((mouseX * actualFactor) - point.position.x, maxX));
                    flickable.contentY = Math.max(0, Math.min((mouseY * actualFactor) - point.position.y, maxY));
                }
            }
        }
    }
}
