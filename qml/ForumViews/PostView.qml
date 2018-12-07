import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import "ForumItems"
import "ForumDialogs"

Page {
    Layout.fillWidth: true
    Layout.preferredWidth: 1920
    Layout.preferredHeight: 1080
    ColumnLayout{
        x: 0
        y: 0
        anchors.top: parent.top
        anchors.topMargin: 200
        anchors.horizontalCenter: parent.horizontalCenter
        Text {
            text: forumController.postTitle
            font.pointSize: 20
            font.family: "dengxian"
            Layout.preferredHeight: 80
            Layout.fillWidth: true
            Layout.preferredWidth: 1080
            width: 1080
            onTextChanged: {
                width = 1080
                wrapMode = Text.Wrap
            }
        }

        Text {
            text: forumController.postContent
            font.pointSize: 20
            font.family: "dengxian"
            Layout.fillWidth: true
        }

        ListView {
            id: commentListView
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
        bottomPadding: 0
        topPadding: 0
        rightPadding: 10
        leftPadding: 10
        font.pointSize: 50
        font.family: "dengxian"
        onClicked: forumController.viewBoard()
    }

    RoundButton {
        visible: !forumController.isAdmin && !forumController.isGuest
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

    AddCommentDialog {
        id: newCommentDialog
    }

    Connections {
        target: forumController
        onPostOpened: {
            swipeView.setCurrentIndex(3)
            commentListView.height = commentListView.contentHeight
        }
    }

    Connections {
        target: appWindow
        onAfterSynchronizing: commentListView.height = commentListView.contentHeight
    }
}
