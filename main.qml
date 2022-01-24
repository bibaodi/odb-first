import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.2
import EsiModule 1.0

Window {
    id: id_window0
    width: 640
    height: 580
    visible: true
    color: "#77c08e"
    title: qsTr("APnI Measure Tool")

    APnI_GuiAdapter {
        id: id_apni_adapter
    }

    //    UTPs {
    //        id: id_utp_obj
    //    }
    Column {
        id: id_column_0
        x: id_window0.width * 0.05
        y: 15
        width: id_window0.width * 0.9
        height: id_window0.height * 0.9
        spacing: 20

        Row {
            id: id_row_import
            width: parent.width
            height: 50
            spacing: id_grid_params.columnSpacing

            ParamInputItem {
                id: id_utp_item
                width: id_location_item.width
                height: parent.height
                param_label: "utp_id"
                param_unit: "id"
                item_value: 14491
            }
            Rectangle {
                border.color: "green"
                border.width: 1
                width: id_location_item.width
                height: id_utp_item.height
                TextInput {
                    id: textInput
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    color: "green"
                    objectName: "ProbeName"
                    text: qsTr("ProbeName")
                    font.pixelSize: 12
                }
            }

            RoundButton {
                id: button
                width: id_location_item.width
                text: qsTr("Import")
                onClicked: {
                    var utp_id = id_utp_item.get_value()
                    var utpobj
                    console.log(text, ":clicked: ", utp_id)
                    id_logItem.append(`import UTP(id= ${utp_id}).`)
                    var succ = id_apni_adapter.getUtpDatasById(utp_id)
                    console.log("succ=", succ)
                    if (!succ) {
                        id_window0.alert(3000)
                        id_msg_dialog.alert("UTP id not found.")
                        // id_msg_dialog.open()
                    } else {
                        console.log("update info with select result.")
                    }
                }
            }
        }

        Grid {
            id: id_grid_params
            width: id_column_0.width
            height: 130
            columns: 3
            rowSpacing: height / 20
            columnSpacing: width / 4 / 2
            ParamInputItem {
                id: id_location_item
                param_label: "Location"
                param_unit: "mm"
                height: parent.height / 3
                width: parent.width / 4
            }
            ParamInputItem {
                param_label: "NB_ele"
                param_unit: "<600"
                value_min: 1
                value_max: 600
                limit_len: 3
                height: parent.height / 3
                width: parent.width / 4
            }
            ParamInputItem {
                param_label: "id_Apod"
                param_unit: "id"
                value_min: 0
                value_max: 10
                limit_len: 2
                height: parent.height / 3
                width: parent.width / 4
            }
            ParamInputItem {
                param_label: "freq"
                param_unit: "MHz"
                height: parent.height / 3
                width: parent.width / 4
            }
            ParamInputItem {
                param_label: "Nb_half_cycle"
                value_min: 1
                value_max: 2000
                limit_len: 4
                param_unit: "<2k"
                height: parent.height / 3
                width: parent.width / 4
            }
            ParamInputItem {
                param_label: "polarity"
                param_unit: "[1/-1]"
                value_min: -1
                value_max: 1
                height: parent.height / 3
                width: parent.width / 4
            }
            ParamInputItem {
                param_label: "id_Mode"
                param_unit: "id"
                value_min: 0
                value_max: 10
                limit_len: 2
                height: parent.height / 3
                width: parent.width / 4
            }
            ParamInputItem {
                param_label: "duty_cycle"
                param_unit: "%"
                height: parent.height / 3
                width: parent.width / 4
            }
            ParamInputItem {
                param_label: "id_pulse_type"
                param_unit: "id"
                value_min: 0
                value_max: 10
                height: parent.height / 3
                width: parent.width / 4
            }
        }

        Row {
            id: id_row_vlotage
            width: parent.width
            height: 40
            spacing: id_grid_params.columnSpacing

            ParamInputItem {
                param_label: "Voltage"
                param_unit: "V"
                value_min: 1
                value_max: 200
                limit_len: 5
                height: id_location_item.height
                width: id_location_item.width
            }
            ParamInputItem {
                param_label: "PRF"
                param_unit: "Hz"
                value_min: 1
                value_max: 200
                limit_len: 5
                height: id_location_item.height
                width: id_location_item.width
            }
            Column {
                Row {
                    ParamInputItem {
                        param_label: "startLine"
                        param_unit: "#"
                        value_min: 1
                        value_max: 200
                        limit_len: 5
                        height: id_location_item.height
                        width: id_location_item.width / 2
                    }
                    ParamInputItem {
                        param_label: "stopLine"
                        param_unit: "#"
                        value_min: 1
                        value_max: 200
                        limit_len: 5
                        height: id_location_item.height
                        width: id_location_item.width / 2
                    }
                }
            }
        }

        Row {
            id: id_row_cmd
            width: parent.width
            height: 40
            spacing: width / 5 / 3

            RoundButton {
                id: id_button_start
                width: id_row_cmd.width / 5
                text: "start"
            }

            RoundButton {
                id: roundButton1
                width: id_button_start.width
                text: "stop"
            }

            RoundButton {
                id: roundButton2
                width: id_button_start.width
                text: "add"
            }

            RoundButton {
                id: roundButton3
                width: id_button_start.width
                text: "parse Result"
            }
        }

        Row {
            id: id_row_loginfo
            width: parent.width
            height: 190
            Rectangle {
                width: parent.width
                height: parent.height
                border.color: "black"
                Flickable {
                    id: id_flick
                    width: parent.width
                    height: parent.height
                    contentWidth: id_logItem.paintedWidth
                    contentHeight: id_logItem.paintedHeight
                    boundsBehavior: Flickable.StopAtBounds
                    ScrollBar.vertical: ScrollBar {}
                    clip: true

                    function ensureVisible(r) {
                        if (contentX >= r.x)
                            contentX = r.x
                        else if (contentX + width <= r.x + r.width)
                            contentX = r.x + r.width - width
                        if (contentY >= r.y)
                            contentY = r.y
                        else if (contentY + height <= r.y + r.height)
                            contentY = r.y + r.height - height
                    }

                    TextEdit {
                        id: id_logItem
                        width: id_flick.width
                        //height: parent.height
                        textFormat: TextEdit.AutoText
                        text: "<b>logs:</b>"
                        font.family: "Helvetica"
                        font.pointSize: 9
                        color: "blue"
                        focus: true
                        selectByMouse: true
                        leftPadding: 5
                        wrapMode: TextEdit.Wrap
                        readOnly: true
                        onCursorRectangleChanged: id_flick.ensureVisible(
                                                      cursorRectangle)
                    }
                }
            }
        }
    }
    Component.onCompleted: {
        id_apni_adapter.setRoot_win(id_window0)
        console.log("Component.onCompleted")
    }

    MessageDialog {
        id: id_msg_dialog
        title: "APnI Error"
        text: "APni Message box."
        onAccepted: {
            visible = false
            //Qt.quit()
        }
        Component.onCompleted: visible = false
        function alert(msg) {
            text = msg + "\nplease try again~"
            visible = true
        }
    }
}
