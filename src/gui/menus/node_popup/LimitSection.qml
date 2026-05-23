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
                text: "-5%"
                Layout.preferredWidth: 30
                Layout.fillHeight: true
            }
            // Layout.fillWidth: true
            // height: 30

            // Rectangle {
            //     id: dec1
            //     anchors.left: parent.left
            //     width: limitField.height
            //     height: limitField.height
            //     radius: 4
            //     color: Theme.surfaceButton
            //     TapHandler {
            //         onTapped: {
            //             SceneManager.incMachineLimit(root.nodeData, -1);
            //         }
            //     }
            //     Text {
            //         anchors.fill: parent
            //         horizontalAlignment: Text.AlignHCenter
            //         verticalAlignment: Text.AlignVCenter
            //         text: "-1"
            //         color: Theme.textPrimary
            //     }
            // }
            TextField {
                id: limitField
                Layout.fillWidth: true
                // anchors.left: dec1.right
                // anchors.horizontalCenter: parent.horizontalCenter
                placeholderText: "no limit"
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
            }
            // Rectangle {
            //     width: limitField.height
            //     height: limitField.height
            //     color: Theme.warning
            //     anchors.right: parent.right
            //     radius: 4
            //     TapHandler {
            //         onTapped: {
            //             SceneManager.incMachineLimit(nodeData, 1);
            //         }
            //     }
            //     Text {
            //         anchors.fill: parent
            //         horizontalAlignment: Text.AlignHCenter
            //         verticalAlignment: Text.AlignVCenter
            //         text: "+1"
            //         color: Theme.textOnAccent
            //     }
            // }
        }
    }
}
