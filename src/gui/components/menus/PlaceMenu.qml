import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Popup {
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
            ScrollView {

                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                ScrollBar.vertical.policy: ScrollBar.AsNeeded
                ColumnLayout {
                    Repeater {
                        model: sceneManager.recipes
                        delegate: Item {
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
                                    text: model["name"]
                                }
                                Text {
                                    text: model["class"]
                                }
                            }
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
