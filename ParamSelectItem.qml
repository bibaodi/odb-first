import QtQuick 2.15
import QtQml 2.12
import QtQuick.Controls 2.15

Rectangle {
    id: id_param_item
    //border.color: "grey"

    //color: "transparent"
    property string param_label: ""
    property string param_unit: ""
    property string param_type: "float"
    property int limit_len: 10
    property real value_min: 0
    property real value_max: 300

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
            ComboBox {
                id: id_input
                width: id_param_item.width * 0.75
                height: id_param_item.height * 0.7 - 1
                validator: DoubleValidator {
                    bottom: 11
                    top: value_max
                }
                font.pixelSize: 17
                model: {
                    [1, 2, 3, 4, 5, 6, 7, 8]
                }
                leftPadding: 5
                bottomPadding: 5
                rightPadding: 3
                background: Rectangle {
                    opacity: enabled ? 1 : 0.3
                    color: control.down ? "#d0d0d0" : "transparent"
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
