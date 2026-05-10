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
        color: "blue"
        border.color: "black"
        border.width: 1
        radius: 10
    }
    RowLayout {
        id: mainLayout
        Text {
            text: root.nodeData ? root.nodeData.name : ""
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
