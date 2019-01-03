import QtQuick 2.9
import QtQuick.Controls 2.2

import "ForumViews"

ApplicationWindow {
    id: appWindow
    visible: true
    flags: Qt.Window | Qt.WindowFullscreenButtonHint
    visibility: "Maximized"
    width: 1280
    height: 720

    SwipeView {
        id: swipeView
        anchors.fill: parent
        interactive: false

        HomePage {
        }

        ForumView {
        }

        BoardView {
        }

        PostView {
        }

        Connections {
            target: forumController
            onMessageSent: {
                console.log(message)
                if (messagePopupAnimation.running)
                {
                    messagePopupAnimation.stop()
                    messageText.text = messageText.oldMessage + "\n"
                }
                messageText.text = messageText.text + message
                messagePopup.open()
            }
        }
    }

    Popup {
        id: messagePopup
        x: parent.width / 2 - width / 2
        y: parent.height - height
        height: contentHeight + 10
        width: contentWidth + 100
        Text {
            id: messageText
            width: 720
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.family: "dengxian"
            font.pointSize: 20
            property string oldMessage: ""
        }

        onOpened: PropertyAnimation {
            id: messagePopupAnimation
            target: messagePopup
            property: "opacity"
            from: 1
            to: 0
            duration: 1500
            onStopped: {
                messagePopup.close()
                messageText.oldMessage = messageText.text
                messageText.text = ""
            }
        }
    }
}
