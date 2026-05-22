pragma Singleton
import QtQuick

QtObject {
    id: root

    property var current: DarkTheme {}

    readonly property color background:          current.background
    readonly property color gridLine:            current.gridLine
    readonly property color overlayBg:           current.overlayBg
    readonly property color surface:             current.surface
    readonly property color surfaceInput:        current.surfaceInput
    readonly property color surfaceButton:       current.surfaceButton
    readonly property color surfaceMenu:         current.surfaceMenu
    readonly property color surfaceNotification: current.surfaceNotification
    readonly property color surfaceFactory:      current.surfaceFactory
    readonly property color surfaceExtraction:   current.surfaceExtraction
    readonly property color accent:              current.accent
    readonly property color accentFactory:       current.accentFactory
    readonly property color accentExtraction:    current.accentExtraction
    readonly property color selection:           current.selection
    readonly property color selectionFill:       current.selectionFill
    readonly property color warning:             current.warning
    readonly property color error:               current.error
    readonly property color badge:               current.badge
    readonly property color portInput:           current.portInput
    readonly property color portOutput:          current.portOutput
    readonly property color textPrimary:         current.textPrimary
    readonly property color textMuted:           current.textMuted
    readonly property color textOnAccent:        current.textOnAccent
    readonly property color textNotification:    current.textNotification
    readonly property color border:              current.border
    readonly property color borderLight:         current.borderLight
    readonly property color borderFocus:         current.borderFocus
    readonly property color iconBg:              current.iconBg
}
