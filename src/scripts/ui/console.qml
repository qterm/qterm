import QtQuick 2
import QtQuick.Layouts 1
import QtQuick.Controls 2

ApplicationWindow {
    width: 640
    height: 480
    title: qsTr("QTerm Script Console")
    property var commands: ListModel {}

    GridLayout {
        id: grid
        columns: 2
        rows: 2
        anchors.fill: parent

        ListView {
            id: list
            Layout.row: 0
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 900
            Layout.preferredHeight: 600
            model: commands
            delegate: Text {
                text: command
            }
        }

        TextField {
            id: input
            Layout.rowSpan: 2
            Layout.columnSpan: 2
            Layout.row: 2
            Layout.fillWidth: true
            Layout.fillHeight: true
            //Layout.preferredWidth: 900
            //Layout.preferredHeight: 200
            placeholderText: qsTr("Enter command here...")
        }

        Button {
            Layout.columnSpan: 1
            Layout.row: 4
            text: qsTr("Execute Code")
            onClicked: {
                var command = input.text
                if (command) {
                    eval(command)
                    commands.append({"command": command})
                    input.text = ""
                }
            }
        }
    }
}
