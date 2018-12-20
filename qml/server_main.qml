import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import QtQuick.Controls 2.4

ApplicationWindow {
    id: applicationWindow
    width: 1280
    height: 720
    visible: true

    ColumnLayout {
        anchors.fill: parent

        Item {
            width: 1080
            height: 400
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            ScrollView {
                anchors.fill: parent
                clip: true

                TextArea {
                    id: historyTextArea
                    font.pointSize: 20
                    font.family: "dengxian"
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    text: qsTr("历史记录\n")
                    horizontalAlignment: Text.AlignLeft
                    selectByMouse: true

                    background: Rectangle {
                        implicitWidth: parent.parent.width
                        implicitHeight: parent.parent.height
                        border.color: "#21be2b"
                    }
                }
            }
        }

        Item {
            width: 1080
            height: 60
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            TextField {
                id: targetTextField
                anchors.fill: parent
                font.pointSize: 20
                font.family: "dengxian"
                placeholderText: qsTr("IP:Port")
            }
        }

        Item {
            width: 1080
            height: 100
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            ScrollView {
                anchors.fill: parent
                clip: true

                TextArea {
                    id: newMessageTextArea
                    font.pointSize: 20
                    font.family: "dengxian"
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    placeholderText: qsTr("新消息")
                    horizontalAlignment: Text.AlignLeft
                    selectByMouse: true

                    background: Rectangle {
                        implicitWidth: parent.parent.width
                        implicitHeight: parent.parent.height
                        border.color: "#21be2b"
                    }
                }
            }
        }

        Button {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            text: qsTr("发送")

            onClicked: {
                forumServer.sendMessage(targetTextField.text, newMessageTextArea.text)
                newMessageTextArea.text = ""
            }
        }
    }

    Connections {
        target: forumServer
        onMessageReceived: {
            console.log(message)
            historyTextArea.append(message)
        }
    }
}
