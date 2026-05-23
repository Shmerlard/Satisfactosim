import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FACTORY_QT
import gui.theme
import gui.controls

Item {
    id: root

    required property var nodeData
    visible: nodeData && (nodeData.nodeType === 3) && nodeData.somersloopSlotSize > 0

    implicitHeight: layout.height

    ColumnLayout {
        id: layout
        // visible: root.somersloopAvailable
        width: parent.width
        spacing: 4
        Text {
            text: "Somersloop"
            color: Theme.textMuted
            font.pixelSize: 11
        }
        Item {
            Layout.fillWidth: true
            height: 30
            GButton {
                text: "-1"
                onClicked: SceneManager.setSomersloopCount(root.nodeData, root.somersloopCount - 1)
            }
            Text {
                id: somersloopField
                // anchors.left: somersloopDec1.right
                anchors.horizontalCenter: parent.horizontalCenter
                text: "sl: " + root.somersloopCount + "/" + root.somersloopSlotSize
                color: Theme.textPrimary
            }
            GButton {
                text: "+1"
                onClicked: SceneManager.setSomersloopCount(root.nodeData, root.somersloopCount + 1)
                anchors.right: parent.right
            }
        }
    }
}
