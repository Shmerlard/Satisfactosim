import QtQuick
import QtQuick.Layouts

Item {
    id: root

    property var nodeData
    property int modelIndex: -1
    property Item contentContainer: null
    property bool selected: false
    property bool hovered: false

    implicitWidth: mainLayout.width + 10
    implicitHeight: mainLayout.height + 10

    Rectangle {
        id: background
        anchors.fill: parent
        color: "#2d1e3d"
        border.color: selected ? "#4fc3f7" : "#8e44ad"
        border.width: selected ? 2 : 1
        radius: 10
    }
    RowLayout {
        id: mainLayout
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
        ColumnLayout {
            Image {
                source: "image://assets/misc/industry.png"
                Layout.preferredHeight: 50
                Layout.preferredWidth: 50
                Layout.alignment: Qt.AlignHCenter
            }
            Text {
                text: root.nodeData ? root.nodeData.name : ""
                color: "white"
            }
        }
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
