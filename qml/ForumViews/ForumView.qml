import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import "ForumItems"

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
                text: forumController.greeting
                font.pointSize: 20
                font.family: "dengxian"
                Layout.preferredHeight: 100
                Layout.preferredWidth: 1080
                Layout.fillWidth: true
            }

            ListView {
                id: boardListView
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 1080
                Layout.fillWidth: true
                model: boardListModel
                delegate: BoardItem {}
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
        bottomPadding: 0
        topPadding: 0
        rightPadding: 10
        leftPadding: 10
        font.pointSize: 50
        font.family: "dengxian"
        onClicked: swipeView.setCurrentIndex(0)
    }

    Connections {
        target: forumController
        onForumOpened: {
            swipeView.setCurrentIndex(1)
            boardListView.height = boardListViewcontentHeight
        }
    }
}
