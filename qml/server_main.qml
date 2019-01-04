import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import QtQuick.Controls 2.4

ApplicationWindow {
    id: appWindow
    visible: true
    flags: Qt.Window | Qt.WindowFullscreenButtonHint
    visibility: "Maximized"
    width: 1280
    height: 720

    ColumnLayout {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        Item {
            width: 1080
            height: 60
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            RowLayout {
                width: 1080

                Text {
                    text: qsTr("请输入服务器要监听的端口号")
                    font.pointSize: 20
                    font.family: "dengxian"
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                }

                TextField {
                    id: portTextEdit
                    font.pointSize: 20
                    font.family: "dengxian"
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                    inputMethodHints: Qt.ImhDigitsOnly
                    selectByMouse: true
                    text: qsTr("8118")
                    Layout.fillWidth: true
                }

                Button {
                    font.pointSize: 20
                    font.family: "dengxian"
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.preferredWidth: 100
                    text: qsTr("启动")

                    onClicked: {
                        forumServer.start(portTextEdit.text)
                    }
                }

                Button {
                    font.pointSize: 20
                    font.family: "dengxian"
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.preferredWidth: 100
                    text: qsTr("停止")

                    onClicked: {
                        forumServer.stop()
                    }
                }
            }
        }

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
                selectByMouse: true
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
            font.pointSize: 20
            font.family: "dengxian"
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
