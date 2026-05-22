import QtQuick
import QtQuick.Layouts
import gui.theme

Item {
    id: root

    property var nodeData
    property int modelIndex: -1
    property Item contentContainer: null
    property bool selected: false
    property bool hovered: false

    implicitWidth: 150
    implicitHeight: 100

    Rectangle {
        id: background
        anchors.fill: parent
        color: Theme.warning
        border.color: selected ? Theme.selection : Theme.border
        border.width: selected ? 2 : 1
        radius: 10
    }
    RowLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: 0

        Column {
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
        Item { Layout.fillWidth: true }
        ColumnLayout {
            id: centerCol
            Image {
                source: root.nodeData.itemIcon ? root.nodeData.machineIcon : "image://assets/misc/question-mark.png"
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredHeight: 50
                Layout.preferredWidth: 50
            }
            Text {
                id: nameText
                text: root.nodeData ? root.nodeData.itemName : ""
                color: Theme.textOnAccent
            }
        }
        Item { Layout.fillWidth: true }
        ColumnLayout {
            id: outputsCol
            Repeater {
                model: root.nodeData ? root.nodeData.outputs : []
                delegate: Port {
                    portData: modelData
                    isInput: false
                    contentContainer: root.contentContainer
                    nodeRoot: root
                }
            }
        }
    }
}
