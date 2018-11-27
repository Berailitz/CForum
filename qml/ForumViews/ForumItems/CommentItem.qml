import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

RowLayout {
    width: 1080
    Layout.preferredWidth: 1080
    Text {
        text: model.content
        Layout.fillWidth: true
        Layout.preferredWidth: 720
        font.family: "dengxian"
        font.pointSize: 30
        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
    }
}
