import QtQuick 2.15
import QtQml 2.12

Rectangle {
    id: id_param_item
    border.color: "grey"

    property string param_label: ""
    property string param_unit: ""
    property string param_type: "float"
    property int limit_len: 10
    property real value_min: 0
    property real value_max: 300
    property var item_value: 12.34

    FocusScope {
        Column {
            spacing: 1

            Text {
                id: id_label
                width: parent.width
                height: parent.height * 0.3
                leftPadding: 5
                text: param_label ? param_label + ":" : "ParamLabel"
                color: "#b73f04"
                fontSizeMode: Text.VerticalFit
            }

            Row {
                TextInput {
                    id: id_input
                    objectName: param_label
                    width: id_param_item.width * 0.75
                    height: id_param_item.height * 0.7
                    validator: DoubleValidator {
                        bottom: 11
                        top: value_max
                    }
                    focus: true
                    font.pixelSize: 17
                    color: "blue"
                    text: item_value
                    maximumLength: limit_len
                    horizontalAlignment: Text.AlignRight
                    rightPadding: 3
                    //                    onEditingFinished: {
                    //                        id_logItem.add_log(
                    //                                    `${param_label} change value to <${text}>`)
                    //                    }
                    onTextEdited: {
                        id_logItem.add_log(
                                    `${param_label} change value to <${text}>`)
                    }
                }
                Text {
                    id: id_unit
                    width: id_param_item.width * 0.2
                    height: parent.height
                    text: param_unit ? param_unit : "mm"
                    color: "blueviolet"
                    fontSizeMode: Text.VerticalFit
                }
            }
        }
    }

    function get_value() {
        if (!id_input.text) {
            return 0
        }
        var num_value = parseFloat(id_input.text)
        if (num_value !== num_value) {
            return id_input.text
        } else {
            return num_value
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

