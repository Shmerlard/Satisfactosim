import QtQuick
import QtQuick.Layouts

Item {
    id: root
    property var portData
    property bool isInput: false

    implicitWidth: mainLayout.width + 5
    implicitHeight: mainLayout.height + 5
    z: 1

        Component.onCompleted: {
            console.log("PORT: ", root.z)
        }
    Rectangle {
        id: background
        anchors.fill: parent
        color: root.isInput ? "#0d3349" : "#3d1a0d"
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
            color: "white"
        }
    }
    MouseArea {
        id: mouseArea

        anchors.fill: parent
        hoverEnabled: true
        preventStealing: true
        acceptedButtons: Qt.LeftButton

        propagateComposedEvents: true
        onPressed: mouse => {
            if (mouse.button === Qt.LeftButton) {
                console.log(root.portData)
            }
        }


    }
}
