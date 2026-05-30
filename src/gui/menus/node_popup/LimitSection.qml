import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FACTORY_QT
import gui.theme
import gui.controls

Item {
    id: root

    required property var nodeData
    // visible: nodeData && nodeData.isMachine

    implicitHeight: layout.height

    ColumnLayout {
        id: layout
        // Layout.fillWidth: true
        width: parent.width
        spacing: 4

        Text {
            text: "Machine Limit"
            color: Theme.textMuted
            font.pixelSize: 11
        }
        RowLayout {

            GButton {
                text: "-1"
                Layout.preferredWidth: 30
                Layout.fillHeight: true
                onClicked: {
                    SceneManager.incMachineLimit(root.nodeData, -1);
                }
            }
            TextField {
                id: limitField
                Layout.fillWidth: true
                placeholderText: root.nodeData.machineLimitStr
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                background: Rectangle {
                    color: Theme.surfaceInput
                    radius: 4
                }
                color: Theme.textPrimary
                onEditingFinished: {
                    let val = parseFloat(text);
                    if (!isNaN(val))
                        SceneManager.setMachineLimit(root.nodeData, val);
                }
            }
            GButton {
                text: "+1"
                Layout.preferredWidth: 30
                Layout.fillHeight: true
                onClicked: {
                    SceneManager.incMachineLimit(root.nodeData, 1);
                }
            }
            GButton {
                text: "X"
                Layout.preferredWidth: 30
                Layout.fillHeight: true
                onClicked: {
                    SceneManager.setMachineLimit(root.nodeData, -1);
                }
            }
        }
    }
}
