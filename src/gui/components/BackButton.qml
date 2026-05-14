import QtQuick
import FACTORY_QT

Item {
    id: root

    Rectangle {
        id: backButton
        visible: !SceneManager.isRootFactory
        width: 100
        height: 100
        color: "transparent"
        Image {
            source: "image://assets/misc/arrow-right-circle-solid.png"
            fillMode: Image.PreserveAspectFit
            anchors.fill: parent
        }
        MouseArea {
            anchors.fill: parent
            hoverEnabled: false
            acceptedButtons: Qt.LeftButton
            containmentMask: Item {
                width: backButton.width
                height: backButton.height
                function contains(p) {
                    var r = width / 2;
                    return (p.x - r) * (p.x - r) + (p.y - r) * (p.y - r) <= r * r;
                }
            }
            onClicked: SceneManager.enterParentFactory()
        }
    }
}
