import QtQuick
import QtQuick.Layouts

Item {
    id: root

    property var nodeData
    property int modelIndex: -1
    property Item contentContainer: null
    property bool selected: false
    property bool hovered: false

    implicitWidth: nameText.implicitWidth + outputsCol.implicitWidth + 40
    implicitHeight: Math.max(50, mainLayout.implicitHeight)

    Rectangle {
        id: background
        anchors.fill: parent
        color: "#1a2e1a"
        border.color: selected ? "#4fc3f7" : "#27ae60"
        border.width: selected ? 2 : 1
        radius: 10
    }
    RowLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: 0

        Item { Layout.fillWidth: true }
        ColumnLayout {
            id: centerCol
            Image {
                source: root.nodeData ? root.nodeData.machineIcon : ""
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredHeight: 50
                Layout.preferredWidth: 50
            }
            Text {
                id: nameText
                text: root.nodeData ? root.nodeData.machineName : ""
                color: "white"
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
                    nodeRoot: root
                    contentContainer: root.contentContainer
                }
            }
        }
    }
}
