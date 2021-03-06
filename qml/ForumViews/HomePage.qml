import Qt.labs.platform 1.0
import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Page {
    Layout.fillWidth: true
    Layout.preferredWidth: 1920
    Layout.preferredHeight: 1080
    ColumnLayout {
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        width: 1080
        height: 720

        Text {
            text: qsTr("CForum")
            Layout.preferredWidth: 600
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            font.family: "dengxian"
            font.pixelSize: 70
        }

        RowLayout {
            Text {
                text: qsTr("服务器地址")
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
                font.family: "dengxian"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                font.pointSize: 35
            }

            TextField {
                id: urlTextField
                Layout.preferredWidth: 600
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
                font.family: "dengxian"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                font.pointSize: 35
                selectByMouse: true
                text: qsTr("ws://localhost:8118/")
            }
        }

        TextField {
            id: usernameTextField
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
            font.family: "dengxian"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            font.pointSize: 45
            selectByMouse: true
            placeholderText: qsTr("用户名")
        }

        TextField {
            id: passwordTextField
            width: usernameTextField.width
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
            font.family: "dengxian"
            placeholderText: qsTr("密码")
            font.pointSize: 45
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            selectByMouse: true
            echoMode: TextInput.Password
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            Button {
                text: qsTr("注册")
                Layout.preferredWidth: 240
                font.family: "dengxian"
                font.pointSize: 40
                focusPolicy: Qt.NoFocus
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: {
                    forumController.addUser(usernameTextField.text, passwordTextField.text)
                    usernameTextField.text = ""
                    passwordTextField.text = ""
                }
            }

            Button {
                text: qsTr("匿名用户登录")
                Layout.preferredWidth: 600
                font.family: "dengxian"
                font.pointSize: 40
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: {
                    forumController.guestLogin()
                    usernameTextField.text = ""
                    passwordTextField.text = ""
                }
            }

            Button {
                text: qsTr("登录")
                Layout.preferredWidth: 240
                font.family: "dengxian"
                font.pointSize: 40
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: {
                    forumController.login(usernameTextField.text, passwordTextField.text)
                    usernameTextField.text = ""
                    passwordTextField.text = ""
                }
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            Button {
                Layout.preferredWidth: 450
                font.family: "dengxian"
                font.pointSize: 40
                focusPolicy: Qt.NoFocus
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                text: qsTr("连接服务器")

                onClicked: {
                    forumController.connect(urlTextField.text)
                }
            }

            Button {
                Layout.preferredWidth: 450
                font.family: "dengxian"
                font.pointSize: 40
                focusPolicy: Qt.NoFocus
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                text: qsTr("断开连接")

                onClicked: {
                    forumController.disconnect()
                }
            }
        }
    }

    Connections {
        target: forumController
        onAppOpened: {
            forumController.logout()
            swipeView.setCurrentIndex(0)
        }
    }
}

