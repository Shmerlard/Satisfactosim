import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FACTORY_QT

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
        SceneManager.recipes.setFilterString(text);
    }
}
