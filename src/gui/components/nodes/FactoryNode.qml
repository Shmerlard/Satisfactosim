import QtQuick
import QtQuick.Layouts

Item {
    id: root

    property var nodeData
    property int modelIndex: -1
    property Item contentContainer: null

    implicitWidth: mainLayout.width + 10
    implicitHeight: mainLayout.height + 10
    Rectangle {
        id: background
        anchors.fill: parent
        color: "#2d1e3d"
        border.color: "#8e44ad"
        border.width: 1
        radius: 10
    }
    RowLayout {
        id: mainLayout
        ColumnLayout {
            id: inputsCol
            Repeater {
                model: root.nodeData ? root.nodeData.inputs : []
                delegate: Port {
                    portData: modelData
                    isInput: true
                    contentContainer: root.contentContainer
                    nodeRoot: root
                }
            }
        }

        ColumnLayout {
            Image {
                source: "image://assets/misc/industry.png"
                Layout.preferredHeight: 50
                Layout.preferredWidth: 50
                Layout.alignment: Qt.AlignHCenter
            }
            Text {
                text: root.nodeData ? root.nodeData.name : ""
                color: "white"
            }
        }
        ColumnLayout {
            id: outputsCol
            Repeater {
                model: root.nodeData ? root.nodeData.outputs : []
                delegate: Port {
                    portData: modelData
                    isInput: true
                    contentContainer: root.contentContainer
                    nodeRoot: root
                }
            }
        }

    }
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        preventStealing: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        property real lastX: 0
        property real lastY: 0

        onDoubleClicked: {
            sceneManager.enterFactory(root.nodeData)
        }
        onPressed: mouse => {
            if (mouse.button === Qt.RightButton)
                return;
            if (!root.contentContainer || !root.nodeData)
                return;
            var pt = mouseArea.mapToItem(root.contentContainer, mouse.x, mouse.y);
            lastX = pt.x - root.nodeData.posX;
            lastY = pt.y - root.nodeData.posY;
        }

        onPositionChanged: mouse => {
            if (!(pressedButtons & Qt.LeftButton) || !root.contentContainer || !root.nodeData)
                return;
            var pt = mouseArea.mapToItem(root.contentContainer, mouse.x, mouse.y);
            root.nodeData.posX = pt.x - lastX;
            root.nodeData.posY = pt.y - lastY;
        }
    }
}
