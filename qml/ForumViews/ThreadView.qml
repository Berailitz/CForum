import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import "ForumItems"
import "ForumDialogs"

Page {
    Layout.fillWidth: true
    Layout.preferredWidth: 1080
    Layout.preferredHeight: 720
    ColumnLayout{
        x: 0
        y: 0
        anchors.top: parent.top
        anchors.topMargin: 200
        anchors.horizontalCenter: parent.horizontalCenter
        Text {
            text: forumController.threadTitle
            font.pointSize: 30
            font.family: "dengxian"
            Layout.preferredHeight: 100
            Layout.preferredWidth: 600
            Layout.fillWidth: true
        }

        Text {
            text: forumController.threadContent
            font.pointSize: 30
            font.family: "dengxian"
            Layout.preferredHeight: 100
            Layout.preferredWidth: 600
            Layout.fillWidth: true
        }

        ListView {
            height: contentHeight
            width: contentWidth
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 1080
            Layout.fillWidth: true
            model: commentListModel
            delegate: CommentItem {}
            ScrollBar.vertical: ScrollBar {}
        }
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
        onClicked: swipeView.setCurrentIndex(2)
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
        onClicked: newCommentDialog.open()
    }

    PostCommentDialog {
        id: newCommentDialog
    }
}
