import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FACTORY_QT

Item {
    id: root
    property var recipe
    property real spawnX: 0
    property real spawnY: 0
    property var spawnMenu

    implicitHeight: 40
    implicitWidth: 450

    Rectangle {
        anchors.fill: parent
        color: "white"
        border.width: 1
        border.color: "orange"
        radius: 10
    }

    // Center: name + className stacked
    Column {
        anchors.centerIn: parent
        spacing: 2

        Row {
            // anchors.centerIn:parent
            Rectangle {
                // anchors.centerIn: parent
                color: "orange"
                width: 10
                height: 10
                radius: 4
                visible: root.recipe["isAlternate"]
            }

            Text {
                // anchors.horizontalCenter: parent.horizontalCenter
                text: root.recipe["name"]
            }
        }
    }

    // Left: inputs
    Row {
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 5
        spacing: 4

        Repeater {
            model: root.recipe["inputs"]
            delegate: Rectangle {
                width: 32
                height: 32
                radius: 4
                color: "gray"
                Image {
                    source: modelData["iconUrl"]
                    width: parent.width
                    height: parent.height
                    fillMode: Image.PreserveAspectFit
                    sourceSize.width: 128
                    sourceSize.height: 128
                }

                ToolTip.visible: hoverHandler_i.hovered
                ToolTip.text: modelData["name"]
                HoverHandler { id: hoverHandler_i }
            }
        }
    }

    // Right: outputs
    Row {
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.rightMargin: 5
        spacing: 4

        Repeater {
            model: root.recipe["outputs"]
            delegate: Rectangle {
                width: 30
                height: 30
                radius: 4
                color: "gray"
                Image {
                    source: modelData["iconUrl"]
                    width: parent.width
                    height: parent.height
                    // fillMode: Image.PreserveAspectFit
                    fillMode: Image.PreserveAspectCrop
                    sourceSize.width: 128
                    sourceSize.height: 128
                }
                ToolTip.visible: hoverHandler_o.hovered
                ToolTip.text: modelData["name"]
                HoverHandler { id: hoverHandler_o }
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            SceneManager.createMachineNode(root.recipe["className"], spawnX, spawnY);
            placeMenu.close();
        }
    }
}
