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
        color: Theme.textPrimary
        placeholderTextColor: Theme.textMuted
        leftPadding: 10

        background: Rectangle {
            color: Theme.background
            border.color: searchBox.activeFocus ? Theme.borderFocus : Theme.border
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
                color: parent.checked ? Theme.accent : Theme.surfaceButton
                border.color: parent.checked ? Theme.accent : Theme.borderLight
                border.width: 1
                radius: 4
            }
            contentItem: Text {
                text: parent.text
                color: parent.checked ? Theme.textOnAccent : Theme.textMuted
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 11
                font.bold: parent.checked
            }
        }

        Item { Layout.fillWidth: true }

        Rectangle { width: 1; height: 18; color: Theme.borderLight; Layout.alignment: Qt.AlignVCenter }

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
                    color: parent.checked ? Theme.selection : Theme.surfaceButton
                    border.color: parent.checked ? Theme.selection : Theme.borderLight
                    border.width: 1
                    radius: 4
                }
                contentItem: Text {
                    text: parent.text
                    color: parent.checked ? Theme.textOnAccent : Theme.textMuted
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
