import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FACTORY_QT
import gui.theme
import gui.controls

Item {
    id: root

    required property var nodeData
    readonly property bool isVisible: nodeData && nodeData.nodeType === 5

    visible: root.isVisible
    readonly property var model : nodeData && nodeData.nodeType === 5 ? nodeData.outputs : []
    implicitHeight: layout.height

    ColumnLayout {
        id: layout
        width: parent.width
        spacing: 4

        Text {
            text: "Weights"
            color: Theme.textMuted
            font.pixelSize: 11
        }

        Repeater {
            model: root.model
            delegate: RowLayout {
                required property var modelData
                Layout.fillWidth: true
                spacing: 4

                Text {
                    text: "Out " + (modelData.portIndex) + ":"
                    color: Theme.textMuted
                    font.pixelSize: 11
                }
                TextField {
                    Layout.fillWidth: true
                    text: modelData.weight
                    placeholderText: "1"
                    background: Rectangle {
                        color: Theme.surfaceInput
                        radius: 4
                    }
                    color: Theme.textPrimary
                    font.pixelSize: 11
                    onEditingFinished: SceneManager.setSplitterWeight(nodeData, modelData.portIndex, text)
                }
                Text {
                    text: modelData.proportion
                    color: Theme.textMuted
                    font.pixelSize: 11
                }
            }
        }
        GButton {
            text: "Add output"
            onClicked: {
                SceneManager.addSplitterOutput(root.nodeData)
            }
        }
    }
}
