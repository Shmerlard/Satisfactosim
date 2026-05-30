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
        width: parent.width
        spacing: 4

        Text {
            text: "Overclock (%)"
            color: Theme.textMuted
            font.pixelSize: 11
        }
        RowLayout {
            GButton {
                text: "-5%"
                Layout.preferredWidth: 30
                Layout.fillHeight: true
                onClicked: {
                    SceneManager.incOverclock(root.nodeData, -0.05);
                }
            }
            TextField {
                id: overclockField
                Layout.fillWidth: true
                placeholderText: root.nodeData.overclockStr
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                background: Rectangle {
                    color: Theme.surfaceInput
                    radius: 4
                }
                color: Theme.textPrimary
                onEditingFinished: {
                    let val = parseFloat(text);
                    if (!isNaN(val))
                        SceneManager.setOverclock(root.nodeData, val / 100.0);
                }
            }
            GButton {
                text: "+5%"
                Layout.preferredWidth: 30
                Layout.fillHeight: true
                onClicked: {
                    SceneManager.incOverclock(root.nodeData, 0.05);
                }
            }
            GButton {
                text: "100%"
                Layout.preferredWidth: 30
                Layout.fillHeight: true
                onClicked: {
                    SceneManager.setOverclock(root.nodeData, 1);
                }
            }
        }
    }
}
