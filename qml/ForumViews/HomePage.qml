import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Page {
    Layout.fillWidth: true
    ColumnLayout {
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        Text {
            text: qsTr("CForum")
            Layout.preferredWidth: 600
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            font.family: "dengxian"
            font.pixelSize: 70
        }

        TextField {
            id: usernameTextField
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
            font.family: "dengxian"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            font.pointSize: 45
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
            echoMode: TextInput.Password
        }

        Row {
            id: row
            Layout.fillWidth: true

            RowLayout {
                id: rowLayout
                width: parent.width

                Button {
                    id: registerButton
                    text: qsTr("注册")
                    Layout.preferredWidth: 250
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
                    id: loginButton
                    text: qsTr("登录")
                    Layout.preferredWidth: 250
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

        }
    }
}

