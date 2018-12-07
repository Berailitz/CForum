import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

RowLayout {
    width: 1080
    Layout.preferredWidth: 1080
    Text {
        text: model.name
        font.family: "dengxian"
        font.pointSize: 20
        Layout.preferredWidth: 800
        width: 800
        onTextChanged: {
            width = 800
            wrapMode = Text.Wrap
        }
    }

    Button {
        text: qsTr("🡲")
        Layout.preferredWidth: 100
        font.family: "dengxian"
        font.pointSize: 20
        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
        onClicked: forumController.viewBoard(model.id)
    }
}
