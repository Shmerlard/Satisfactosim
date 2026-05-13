import QtQuick
import QtQuick.Shapes
import QtQuick.Controls

Item {
    id: root
    z: 4

    property var conn
    property var src: conn ? conn.srcNode : null
    property var dst: conn ? conn.dstNode : null
    property bool hovered: false
    visible: src !== null && dst !== null

    Shape {
        z: 4
        ShapePath {
            strokeColor: root.hovered ? "#4fc3f7" : "#e8a020"
            strokeWidth: root.hovered ? 3 : 2
            fillColor: "transparent"
            startX: src ? src.posX + conn.srcOffset.x : 0
            startY: src ? src.posY + conn.srcOffset.y : 0
            PathLine {
                x: dst ? dst.posX + conn.dstOffset.x : 0
                y: dst ? dst.posY + conn.dstOffset.y : 0
            }
        }
    }
}
