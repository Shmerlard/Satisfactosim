import QtQuick
import QtQuick.Shapes
import QtQuick.Controls

Item {
    id: root
    z: 4

    property var conn
    property var src: conn ? conn.srcNode : null
    property var dst: conn ? conn.dstNode : null
    visible: src !== null && dst !== null

    readonly property real portH: 55
    readonly property real nodeW: 260
    readonly property real topOffset: 10
    Shape {
        id: shape
        ShapePath {
            id: sp
            strokeColor: "#e8a020"
            strokeWidth: 2
            fillColor: "transparent"
            startX: src ? src.posX + nodeW : 0
            startY: src ? src.posY + topOffset + (conn.srcPortIdx + 0.5) * portH : 0
            PathLine {
                x: dst ? dst.posX : 0
                y: dst ? dst.posY + topOffset + (conn.dstPortIdx + 0.5) * portH : 0
            }
        }
    }
}
