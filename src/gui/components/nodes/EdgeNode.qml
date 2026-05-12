import QtQuick
import QtQuick.Layouts

Item {
    id: root

    property var nodeData
    property int modelIndex: -1
    property Item contentContainer: null
    property bool selected: false
    property bool hovered: false

    implicitWidth: inputsCol.implicitWidth + nameText.implicitWidth + outputsCol.implicitWidth + 40
    implicitHeight: Math.max(50, mainLayout.implicitHeight)

    Rectangle {
        id: background
        anchors.fill: parent
        color: "yellow"
        border.color: selected ? "#4fc3f7" : "black"
        border.width: selected ? 2 : 1
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
