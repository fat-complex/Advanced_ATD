import QtQuick
import QtQuick.Controls

Window {
    id: root
    visible: true

    width: 800
    height: 600

    property var rounRobin: roundRobin

    Column {
        anchors.top: parent.top
        anchors.margins: 30
        anchors.horizontalCenter: parent.horizontalCenter

        spacing: 50

        Row {
            id: row1

            spacing: 30

            Button {
                id: button1
                width: 182
                height: 40

                text: "New"
            }

            RadioButton {
                id: radioButton1

                text: "A method"
            }

            RadioButton {
                id: radioButton2

                text: "B method"
            }

            Button {
                id: settings
                width: 182
                height: 40
                text: "Settings"

                onClicked: dialog.open()
            }
        }

        Row {
            id: row2
            ListView {
                id: executors

                width: 225
                height: 432

                clip: true

                model: rounRobin

                delegate: ItemDelegate {
                    Row {
                        spacing: 5
                        Text {
                            width: 100
                            text: name
                        }
                        Text {
                            width: 100
                            text: performance
                        }
                    }
                    onClicked: {
                        executors.currentIndex = index;
                    }
                    highlighted: executors.currentIndex === index
                }
            }

            ListView {
                id: tasks

                width: 225
                height: 432

                model: ListModel {
                    ListElement {
                        name: "Red"
                    }

                    ListElement {
                        name: "Green"
                    }

                    ListElement {
                        name: "Blue"
                    }

                    ListElement {
                        name: "White"
                    }
                }
                delegate: Row {
                    spacing: 5

                    Text {
                        width: 100
                        text: name
                    }
                }
            }

            ListView {
                id: info

                width: 225
                height: 432

                model: ListModel {
                    ListElement {
                        executor: "Red"
                        taskName: "Me"
                        round: 1
                    }

                    ListElement {
                        executor: "Green"
                        taskName: "Me"
                    }

                    ListElement {
                        executor: "Blue"
                        taskName: "Me"
                        round: 1
                    }

                    ListElement {
                        executor: "White"
                        taskName: "Me"
                        round: 1
                    }
                }
                delegate: Row {
                    spacing: 5

                    Text {
                        width: 100
                        text: executor
                    }
                    Text {
                        width: 100
                        text: taskName
                    }
                    Text {
                        width: 100
                        text: round
                    }
                }
            }
        }
        Button {
            id: pause

            width: 182
            height: 40

            text: "Pause"
        }
    }

    Dialog {
        id: dialog

        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        width: parent.width / 1.5
        height: parent.height / 1.5

        standardButtons: Dialog.Ok | Dialog.Cancel

        header: Label {
            horizontalAlignment: Text.AlignHCenter
            padding: 10

            font.bold: true

            text: settings.text
        }

        contentItem: Column {
            spacing: 10
            Row {
                spacing: 10
                Label {
                    text: "Timeout:"
                }
                TextField {
                }
            }
            Row {
                spacing: 10
                Label {
                    text: "Executors:"
                }
                Label {
                    text: "Min"
                }
                TextField {
                }

                Label {
                    text: "Max"
                }
                TextField {
                }
            }
            Row {
                spacing: 10
                Label {
                    text: "Performance:"
                }
                Label {
                    text: "Min"
                }
                TextField {
                }

                Label {
                    text: "Max"
                }
                TextField {
                }
            }
            Row {
                spacing: 10
                Label {
                    text: "Tasks:"
                }
                Label {
                    text: "Min"
                }
                TextField {
                }

                Label {
                    text: "Max"
                }
                TextField {
                }
            }
            Row {
                spacing: 10
                Label {
                    text: "Complexity:"
                }
                Label {
                    text: "Min"
                }
                TextField {
                }

                Label {
                    text: "Max"
                }
                TextField {
                }
            }
        }
    }
    Item {
        id: item
        component MinMax:
            Row {
            spacing: 10
            Label {
                text: "He he"
            }
            Label {
                text: "Min"
            }
            TextField {
            }
            Label {
                text: "Max"
            }
            TextField {
            }
        }

        property string text: ""
        property int min: 0
        property int max: 0
    }
}