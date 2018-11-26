import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Page {
    id: page
    Layout.fillWidth: true
    Flickable {
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.horizontalCenter: parent.horizontalCenter
        ColumnLayout{
            x: 0
            y: 0
            anchors.top: parent.top
            anchors.topMargin: 200
            anchors.horizontalCenter: parent.horizontalCenter
            Text {
                text: forumController.greeting
                font.pointSize: 30
                Layout.preferredHeight: 100
                Layout.preferredWidth: 600
                Layout.fillWidth: true
            }

            ListView {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.fillWidth: true
                model: boardListModel
                delegate: RowLayout {
                    Text {
                        text: model.name
                        Layout.preferredWidth: 250
                        font.family: "Arial"
                        font.pointSize: 40
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    }

                    Button {
                        text: "Go"
                        Layout.preferredWidth: 100
                        font.family: "Arial"
                        font.pointSize: 40
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        onClicked: forumController.viewBoard(model.id)
                    }
                }
            }
        }

        ScrollBar.vertical: ScrollBar {}
        ScrollBar.horizontal: ScrollBar {}
    }
}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
