import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FACTORY_QT
import gui.theme
import gui.controls

Item {
    id: root

    required property var nodeData
    // visible: nodeData && nodeData.nodeType === 3

    implicitHeight: layout.height

    ColumnLayout {
        id: layout
        // visible: root.isVisible
        Text {
            text: "Purity"
            color: Theme.textMuted
            font.pixelSize: 11
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 4

            Repeater {
                model: [
                    {
                        label: "Impure",
                        value: 0
                    },
                    {
                        label: "Normal",
                        value: 1
                    },
                    {
                        label: "Pure",
                        value: 2
                    }
                ]
                delegate: Button {
                    required property var modelData
                    Layout.fillWidth: true
                    text: modelData.label
                    checkable: true
                    checked: nodeData && nodeData.purity === modelData.value
                    onClicked: if (nodeData)
                        SceneManager.setPurity(nodeData, modelData.value)

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
