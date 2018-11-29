import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import "ForumItems"
import "ForumDialogs"

Page {
    Layout.fillWidth: true
    Layout.preferredWidth: 1080
    Layout.preferredHeight: 720
    Flickable {
        width: parent.width
        height: parent.height
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.horizontalCenter: parent.horizontalCenter
        ColumnLayout{
            anchors.top: parent.top
            anchors.topMargin: 200
            anchors.horizontalCenter: parent.horizontalCenter
            Text {
                text: forumController.boardTitle
                font.pointSize: 20
                font.family: "dengxian"
                Layout.preferredHeight: 100
                Layout.preferredWidth: 1080
                Layout.fillWidth: true
            }

            ListView {
                height: contentHeight
                width: contentWidth
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 1080
                Layout.fillWidth: true
                model: threadListModel
                delegate: ThreadItem {}
            }
        }

        ScrollBar.vertical: ScrollBar {}
    }

    RoundButton {
        anchors.top: parent.top
        anchors.topMargin: 100
        anchors.left: parent.left
        anchors.leftMargin: 100
        radius: 50
        text: qsTr("🡰")
        font.pointSize: 50
        font.family: "dengxian"
        onClicked: swipeView.setCurrentIndex(1)
    }

    RoundButton {
        visible: forumController.isAdmin
        anchors.top: parent.top
        anchors.topMargin: 100
        anchors.right: parent.right
        anchors.rightMargin: 300
        radius: 50
        text: qsTr("⚙")
        font.pointSize: 50
        font.family: "dengxian"
        onClicked: setModeratorDialog.open()
    }

    RoundButton {
        anchors.top: parent.top
        anchors.topMargin: 100
        anchors.right: parent.right
        anchors.rightMargin: 100
        radius: 50
        text: qsTr("➕")
        font.pointSize: 50
        font.family: "dengxian"
        onClicked: newThreadDialog.open()
    }

    PostThreadDialog {
        id: newThreadDialog
    }

    SetModeratorDialog {
        id: setModeratorDialog
    }
}
