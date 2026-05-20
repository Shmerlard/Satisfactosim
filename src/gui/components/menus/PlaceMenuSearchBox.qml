import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FACTORY_QT

ColumnLayout {
    spacing: 4
    property int currentMode: 0
    property bool recipeNameEnabled: true

    function clear() { searchBox.text = "" }

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

        onTextChanged: SceneManager.recipes.setFilterString(text)
    }

    RowLayout {
        Layout.fillWidth: true
        spacing: 4

        // Recipe name toggle
        Button {
            id: recipeNameBtn
            text: "Recipe Name"
            checkable: true
            checked: recipeNameEnabled
            enabled: recipeNameEnabled
            onCheckedChanged: SceneManager.recipes.setSearchRecipeName(checked)

            background: Rectangle {
                color: parent.checked ? "#e8a020" : "#2a2a2a"
                border.color: parent.checked ? "#e8a020" : "#555"
                border.width: 1
                radius: 4
            }
            contentItem: Text {
                text: parent.text
                color: parent.checked ? "#1a1a1a" : "#aaaaaa"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 11
                font.bold: parent.checked
            }
        }

        Item { Layout.fillWidth: true }

        Rectangle { width: 1; height: 18; color: "#555"; Layout.alignment: Qt.AlignVCenter }

        // All / In / Out
        Repeater {
            model: [
                { label: "All", mode: 0 },
                { label: "In",  mode: 1 },
                { label: "Out", mode: 2 }
            ]
            delegate: Button {
                required property var modelData
                checkable: true
                checked: currentMode === modelData.mode
                text: modelData.label
                ButtonGroup.group: portFilterGroup
                onClicked: {
                    currentMode = modelData.mode;
                    SceneManager.recipes.setFilterMode(modelData.mode);
                }

                background: Rectangle {
                    color: parent.checked ? "#4fc3f7" : "#2a2a2a"
                    border.color: parent.checked ? "#4fc3f7" : "#555"
                    border.width: 1
                    radius: 4
                }
                contentItem: Text {
                    text: parent.text
                    color: parent.checked ? "#1a1a1a" : "#aaaaaa"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 11
                    font.bold: parent.checked
                }
            }
        }

        ButtonGroup { id: portFilterGroup }
    }
}
