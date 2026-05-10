import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Popup {
    id: root
    property real spawnX: 0
    property real spawnY: 0

    width: 600
    height: 300
    clip: true

    RowLayout {
        anchors.fill: parent
        Rectangle {
            Layout.preferredHeight: 30
            Layout.preferredWidth: 30
        }
        ColumnLayout {
            PlaceMenuSearchBox {}
            ScrollView {
                // Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.fillWidth: false
                Layout.fillHeight: true
                clip: true
                ScrollBar.vertical.policy: ScrollBar.AsNeeded
                ColumnLayout {
                    width: parent.width
                    Layout.alignment: Qt.AlignHCenter
                    Rectangle {
                        // anchors.fill: parent
                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillWidth: true
                        color: "yellow"
                    }
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
