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

                onClicked: roundRobin.restart()
            }

            RadioButton {
                id: radioButton1

                text: "A method"
                checked: roundRobin.settings.Amethod

                onCheckedChanged: roundRobin.settings.Amethod = checked
            }

            RadioButton {
                id: radioButton2

                text: "B method"

                checked: roundRobin.settings.Bmethod

                onCheckedChanged: roundRobin.settings.Bmethod = checked
            }

            Button {
                id: settings
                width: 182
                height: 40
                text: "Settings"

                onClicked: dialog.open()
            }
        }

        Button {
            id: pause

            width: 182
            height: 40

            text: "Pause"

            onClicked: rounRobin.pause()
        }

        Row {
            id: row2

            height: 300

            ListView {
                id: executors

                width: 225
                height: 432

                clip: true

                model: rounRobin.executorsModel

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
                        tasks.model = rounRobin.tasks(name)
                    }
                    highlighted: executors.currentIndex === index
                }

                Component.onCompleted: {
                    executors.currentIndex = -1
                }
            }

            ListView {
                id: tasks

                width: 225
                height: 432

                model: roundRobin.InfoModel

                delegate: Row {
                    spacing: 5

                    Text {
                        width: 100
                        text: display
                    }
                }
            }

            ListView {
                id: info

                width: 225
                height: 432

                model: roundRobin.InfoModel
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
                id: tmt
                spacing: 10
                Label {
                    text: "Timeout:"
                }
                TextField {
                    text: rounRobin.settings.timeout
                    onTextChanged: rounRobin.settings.timeout = parseInt(text)
                }
            }
            Row {
                id: exec

                spacing: 10
                Label {
                    text: "Executors:"
                }
                Label {
                    text: "Min"
                }
                TextField {
                    text: rounRobin.settings.executor.min
                    onTextChanged: rounRobin.settings.executor.min = parseInt(text)
                }

                Label {
                    text: "Max"
                }
                TextField {
                    text: rounRobin.settings.executor.max
                    onTextChanged: rounRobin.settings.executor.max = parseInt(text)
                }
            }
            Row {
                id: perf

                spacing: 10
                Label {
                    text: "Performance:"
                }
                Label {
                    text: "Min"
                }
                TextField {
                    text: rounRobin.settings.performance.min
                    onTextChanged: rounRobin.settings.performance.min = parseInt(text)
                }

                Label {
                    text: "Max"
                }
                TextField {
                    text: rounRobin.settings.performance.max
                    onTextChanged: rounRobin.settings.performance.max = parseInt(text)
                }
            }
            Row {
                id: tsk

                spacing: 10
                Label {
                    text: "Tasks:"
                }
                Label {
                    text: "Min"
                }
                TextField {
                    text: rounRobin.settings.tasks.min
                    onTextChanged: rounRobin.settings.tasks.min = parseInt(text)
                }

                Label {
                    text: "Max"
                }
                TextField {
                    text: rounRobin.settings.tasks.max
                    onTextChanged: rounRobin.settings.tasks.max = parseInt(text)
                }
            }
            Row {
                id: compl

                spacing: 10
                Label {
                    text: "Complexity:"
                }
                Label {
                    text: "Min"
                }
                TextField {
                    text: rounRobin.settings.complexity.min
                    onTextChanged: rounRobin.settings.complexity.min = parseInt(text)
                }

                Label {
                    text: "Max"
                }
                TextField {
                    text: rounRobin.settings.complexity.max
                    onTextChanged: rounRobin.settings.complexity.max = parseInt(text)
                }
            }
        }
    }
}