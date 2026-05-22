import QtQuick

Item {
    id: root

    property string text: ""
    property string icon: ""
    property bool checkable: false
    property bool checked: false
    property color color: Theme.surfaceButton
    property color colorChecked: Theme.accent
    property color textColor: Theme.textMuted
    property color textColorChecked: Theme.textOnAccent

    signal clicked()

    implicitWidth: row.implicitWidth + 24
    implicitHeight: 28

    Rectangle {
        anchors.fill: parent
        radius: 4
        color: (root.checkable && root.checked) ? root.colorChecked
             : hoverHandler.hovered            ? Qt.lighter(root.color, 1.15)
                                               : root.color
        border.color: (root.checkable && root.checked) ? root.colorChecked : Theme.borderLight
        border.width: 1

        Row {
            id: row
            anchors.centerIn: parent
            spacing: 6

            Image {
                visible: root.icon !== ""
                source: root.icon
                width: 14
                height: 14
                anchors.verticalCenter: parent.verticalCenter
                fillMode: Image.PreserveAspectFit
            }

            Text {
                id: label
                visible: root.text !== ""
                anchors.verticalCenter: parent.verticalCenter
                text: root.text
                color: (root.checkable && root.checked) ? root.textColorChecked : root.textColor
                font.pixelSize: 11
                font.bold: root.checkable && root.checked
            }
        }
    }

    HoverHandler { id: hoverHandler }

    TapHandler {
        onTapped: {
            if (root.checkable)
                root.checked = !root.checked;
            root.clicked();
        }
    }
}
