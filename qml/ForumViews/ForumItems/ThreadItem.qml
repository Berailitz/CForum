import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

RowLayout {
    width: 1280
    Layout.preferredWidth: 1280
    visible: !model.isDeleted
    height: model.isDeleted ? 0 : implicitHeight
    Text {
        text: qsTr("#") + model.id + " " + model.title + qsTr(" -- by: ") + controller.getUsername(model.authorID) + ", " + model.time
        Layout.preferredWidth: 1080
        font.family: "dengxian"
        font.pointSize: 20
    }

    Button {
        visible: !model.isDeleted && (board.isModerator(user.id) || model.authorID === user.id)
        text: qsTr("❌")
        Layout.preferredWidth: 100
        font.family: "dengxian"
        font.pointSize: 20
        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
        onClicked: controller.deleteThread(board.id, model.id)
    }

    Button {
        text: qsTr("🡲")
        Layout.preferredWidth: 100
        font.family: "dengxian"
        font.pointSize: 20
        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
        onClicked: controller.viewThread(board.id, model.id)
    }
}
