import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root
    property var recipe
    property real spawnX: 0
    property real spawnY: 0
    property var spawnMenu

    implicitHeight: mainLayout.implicitHeight
    implicitWidth: mainLayout.implicitWidth
    Rectangle {
        anchors.fill: parent
        color: "white"
    }
    ColumnLayout {
        id: mainLayout
        Text {
            id: entryText
            text: root.recipe["name"]
        }
        Text {
            text: root.recipe["class"]
        }
    }
    MouseArea {
        anchors.fill: parent
        onClicked: mouse => {
            sceneManager.createMachineNode(root.recipe["class"], spawnX, spawnY)
            placeMenu.close()
        }
    }
}
