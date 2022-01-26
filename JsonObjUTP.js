.pragma library

var JsonObj = {
    "ProbeName": "",
    "Location": 0,
    "NB_ele": 0,
    "id_apod": 0,
    "freq": 0.0,
    "Nb_half_cycle": 0,
    "polarity": 1,
    "id_mode": 0,
    "duty_cycle": 0,
    "id_pulse_type": -1,
    "voltage": 0,
    "PRF": 0,
    "startLine": 0,
    "stopLine": 0
}

function getJsonStr() {
    return JSON.stringify(JsonObj)
}

function getJsonobj() {
    return JsonObj
}
