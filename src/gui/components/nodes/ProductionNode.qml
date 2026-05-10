import QtQuick
import QtQuick.Layouts

Item {
    id: root

    property var nodeData
    property int modelIndex: -1
    property Item contentContainer: null

    implicitWidth: inputsCol.implicitWidth + nameText.implicitWidth + outputsCol.implicitWidth + 40
    implicitHeight: Math.max(50, mainLayout.implicitHeight)

    Rectangle {
        id: background
        anchors.fill: parent
        color: "yellow"
        border.color: "black"
        border.width: 1
        radius: 10
    }
    RowLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: 0

        ColumnLayout {
            id: inputsCol
            Repeater {
                model: root.nodeData ? root.nodeData.inputs : []
                delegate: Port {
                    portData: modelData
                    isInput: true
                }
            }
        }
        Item {
            Layout.fillWidth: true
        }
        ColumnLayout {
            id: centerCol
            Image {
                source: root.nodeData ? root.nodeData.machineIcon : ""
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredHeight: 50
                Layout.preferredWidth: 50
                // onStatusChanged: console.log("icon status:", status, "source:", source)
            }
            Text {
                id: nameText
                // text: root.nodeData ? root.nodeData.name : ""
                text: root.nodeData ? root.nodeData.machineName : ""
            }
        }
        Item {
            Layout.fillWidth: true
        }
        ColumnLayout {
            id: outputsCol
            Repeater {
                model: root.nodeData ? root.nodeData.outputs : []
                delegate: Port {
                    portData: modelData
                    isInput: false
                }
            }
        }
    }
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        preventStealing: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton   // ← add RightButton

        property real lastX: 0
        property real lastY: 0

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
