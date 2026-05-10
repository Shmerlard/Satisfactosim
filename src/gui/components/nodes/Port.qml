import QtQuick
import QtQuick.Layouts

Item {
    id: root
    property var portData
    property bool isInput: false

    implicitWidth: mainLayout.width + 5
    implicitHeight: mainLayout.height + 5

    Rectangle {
        id: background
        anchors.fill: parent
        color: "green"
    }
    RowLayout {
        id: mainLayout
        layoutDirection: root.isInput ? Qt.LeftToRight : Qt.RightToLeft
        Image {
            source: root.portData.iconUrl
            Layout.preferredWidth :15
            Layout.preferredHeight: 15
            fillMode: Image.PreserveAspectFit
        }
        Text {
            text: root.isInput ? "INPUT" : "OUTPUT"
        }
    }
}
