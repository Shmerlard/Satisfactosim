import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FACTORY_QT
import gui.theme
import gui.controls

Item {
    id: root

    required property var nodeData
    // visible: nodeData && nodeData.nodeType === 3 && nodeData.tierCount > 1

    implicitHeight: layout.height

    ColumnLayout {
        id: layout
        width: parent.width
        Text {
            text: "Mark"
            color: Theme.textMuted
            font.pixelSize: 11
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 4

            Repeater {
                model: root.nodeData ? root.nodeData.tierCount : 0
                delegate: Button {
                    required property int index
                    Layout.fillWidth: true
                    text: "Mk." + (index + 1)
                    checkable: true
                    checked: nodeData && nodeData.tier === index
                    onClicked: if (nodeData)
                        SceneManager.setTier(nodeData, index)

                    background: Rectangle {
                        color: parent.checked ? Theme.accent : Theme.surfaceInput
                        border.color: parent.checked ? Theme.accent : Theme.borderLight
                        border.width: 1
                        radius: 4
                    }
                    contentItem: Text {
                        text: parent.text
                        color: parent.checked ? Theme.textOnAccent : Theme.textMuted
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 11
                        font.bold: parent.checked
                    }
                }
            }
        }
    }
}
