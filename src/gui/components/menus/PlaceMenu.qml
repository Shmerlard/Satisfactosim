import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Popup {
    id: root
    property real spawnX: 0
    property real spawnY: 0

    width: 300
    height: 300
    clip: true

    RowLayout {
        anchors.fill: parent
        Rectangle {
            Layout.preferredHeight: 30
            Layout.preferredWidth: 30
        }
        ColumnLayout {
            TextField {
                id: searchBox
                Layout.fillWidth: true
                implicitHeight: 35
                placeholderText: "Search recipes..."
                color: "white"
                placeholderTextColor: "#888888"
                leftPadding: 10

                background: Rectangle {
                    color: "#1a1a1a"
                    border.color: searchBox.activeFocus ? "#666666" : "#444444"
                    border.width: 1
                    radius: 4
                }

                onTextChanged: {
                    sceneManager.recipes.setFilterString(text)
                }
            }

            ScrollView {

                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                ScrollBar.vertical.policy: ScrollBar.AsNeeded
                ColumnLayout {
                    // spacing: 8
                    Repeater {
                        model: sceneManager.recipes
                        delegate: PlaceMenuEntry {
                            recipe: model
                            spawnX: root.spawnX
                            spawnY: root.spawnY
                            spawnMenu: root
                        }
                    }
                }
            }
        }
        Rectangle {
            Layout.preferredHeight: 30
            Layout.preferredWidth: 30
        }
    }
}
