import QtQuick

// Mouse device render — MX Master 3S PNG with invisible clickable button zones overlaid.
Item {
    id: root

    implicitWidth:  220
    implicitHeight: 326

    signal buttonClicked(int buttonId)

    // ── Mouse image ──────────────────────────────────────────────────────────
    Image {
        id: mouseImage
        anchors.centerIn: parent
        width: parent.implicitWidth
        height: parent.implicitHeight
        source: "qrc:/Logitune/qml/assets/mx-master-3s.png"
        fillMode: Image.PreserveAspectFit
        smooth: true
        mipmap: true
    }

    // ── Button zone overlays with hotspot circles ────────────────────────────
    // Positions tuned for the 220x326 MX Master 3S render at 3/4 angle.
    // Each zone has a 16x16 hotspot circle (2px border, full radius).

    // Hotspot positions derived from Options+ core_metadata.json
    // Metadata marker coords are percentage of 636x1024, scaled to our 220x326 render
    readonly property var hotspotPositions: [
        // 0: Left click — no metadata marker, approximated (top-left half)
        { zoneX: 20, zoneY: 10, zoneW: 85, zoneH: 120, dotX: 60,  dotY: 60  },
        // 1: Right click — no metadata marker, approximated (top-right half)
        { zoneX: 110, zoneY: 10, zoneW: 85, zoneH: 120, dotX: 155, dotY: 60  },
        // 2: Middle / scroll wheel — metadata: (71%, 15%)
        { zoneX: 130, zoneY: 25, zoneW: 50, zoneH: 50,  dotX: 156, dotY: 49  },
        // 3: Back — metadata: (45%, 60%)
        { zoneX: 70, zoneY: 175, zoneW: 60, zoneH: 45,  dotX: 99,  dotY: 196 },
        // 4: Forward — metadata: (35%, 43%)
        { zoneX: 50, zoneY: 118, zoneW: 55, zoneH: 45,  dotX: 77,  dotY: 140 },
        // 5: Gesture button — metadata: (8%, 58%)
        { zoneX: 0, zoneY: 165, zoneW: 40, zoneH: 50,   dotX: 18,  dotY: 189 },
        // 6: Top/ModeShift button — metadata: (81%, 34%)
        { zoneX: 155, zoneY: 88, zoneW: 45, zoneH: 45,  dotX: 178, dotY: 111 }
    ]

    Repeater {
        model: 7

        Item {
            required property int modelData

            // Invisible hit area
            MouseArea {
                x: root.hotspotPositions[modelData].zoneX
                y: root.hotspotPositions[modelData].zoneY
                width:  root.hotspotPositions[modelData].zoneW
                height: root.hotspotPositions[modelData].zoneH
                cursorShape: Qt.PointingHandCursor
                onClicked: root.buttonClicked(modelData)
            }

            // 16x16 hotspot circle
            Rectangle {
                x: root.hotspotPositions[modelData].dotX - 8
                y: root.hotspotPositions[modelData].dotY - 8
                width: 16; height: 16
                radius: 8
                color: "transparent"
                border.color: "#814EFA"
                border.width: 2
                opacity: 0.6

                Behavior on opacity { NumberAnimation { duration: 200 } }

                // Pulse on hover (subtle)
                Rectangle {
                    anchors.centerIn: parent
                    width: 6; height: 6
                    radius: 3
                    color: "#814EFA"
                    opacity: 0.5
                }
            }
        }
    }
}
