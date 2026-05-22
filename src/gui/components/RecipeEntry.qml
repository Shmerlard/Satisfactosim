import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FACTORY_QT

Item {
    property var recipe

    implicitHeight: mainLayout.implicitHeight
    implicitWidth: mainLayout.implicitWidth
    Rectangle {
        anchors.fill: parent
        color: Theme.surface
    }
    ColumnLayout {
        id: mainLayout
        Text {
            id: entryText
            text: recipe["name"]
        }
        Text {
            text: recipe["class"]
        }
    }
}
