#!/usr/bin/env -S gawk --csv -f

BEGIN {
    type = "none"
    event_type = "none"

    default_sink = "??"
    active_port_type = "??"
    volumne = "??"
    muted = "??"

    default_source = "??"
    mic_active_port_type = "??"
    mic_volume = "??"
    mic_muted = "??"
}

{
    delete kv
    for (i = 1; i <= NF; i += 2) {
        kv[$i] = $(i + 1)
    }
}

# An event related to the pulseaudio client.
$2 == "state" {
    if ($4 == "PA_CONTEXT_TERMINATED") {
        exit
    }
    if ($4 == "PA_CONTEXT_FAILED") {
        exit 1
    }
}

# An event related to a sink, a sink is an audio output device, like your speakers, headphones, HDMI audio, etc.
$2 == "sinkinfo" {
    if ("type" in kv && "event_type" in kv) {
        type = kv["type"]
        event_type = kv["event_type"]
    } else {
        type = "none"
        event_type = "none"
    }

    volume = kv["volume"]
    muted = kv["mute"]
    active_port_type = port_icon(kv["active_port_type"])

    print_status()
}

# An event related to a source, which is an audio input device, like a microphone or a virtual capture source.
$2 == "sourceinfo" {
    if ("type" in kv && "event_type" in kv) {
        type = kv["type"]
        event_type = kv["event_type"]
    } else {
        type = "none"
        event_type = "none"
    }

    mic_volume = kv["volume"]
    mic_muted = kv["mute"]
    mic_active_port_type = port_icon(kv["active_port_type"])

    print_status()
}

# An event about a sink input, which is an audio stream that’s going into a sink.
$2 == "sinkinputinfo" {
}

# An event about a source output, which is a stream being recorded from a source.
$2 == "sourceoutputinfo" {
}

# An event about a PulseAudio module. Modules are plug-ins that provide functionality, like Bluetooth support.
$2 == "moduleinfo" {
}

# An event about a PulseAudio client, which is any process connected to the server.
$2 == "clientinfo" {
}

# An event about a sample cache item, which is an audio sample stored for quick playback.
$2 == "sampleinfo" {
}

# An event about a sample cache item, which is an audio sample stored for quick playback.
$2 == "serverinfo" {
    default_sink = kv["default_sink_name"]
    default_source = kv["default_source_name"]
}

# Indicates a global server state change.
$2 == "cardinfo" {
}

function print_status() {
    print type, event_type
    print "sink [", muted, volume, active_port_type, "] source [", mic_muted, mic_volume, mic_active_port_type, "]"
    print ""
}

function debug_kv() {
    for (k in kv) {
        printf("key='%s' value='%s'\n", k, kv[k])
    }
}

function port_icon(type) {
    switch (type) {
        case "PA_DEVICE_PORT_TYPE_UNKNOWN":
            return ""
        case "PA_DEVICE_PORT_TYPE_AUX":
            return ""
        case "PA_DEVICE_PORT_TYPE_SPEAKER":
            return ""
        case "PA_DEVICE_PORT_TYPE_HEADPHONES":
            return ""
        case "PA_DEVICE_PORT_TYPE_LINE":
            return ""
        case "PA_DEVICE_PORT_TYPE_MIC":
            return ""
        case "PA_DEVICE_PORT_TYPE_HEADSET":
            return ""
        case "PA_DEVICE_PORT_TYPE_HANDSET":
            return ""
        case "PA_DEVICE_PORT_TYPE_EARPIECE":
            return ""
        case "PA_DEVICE_PORT_TYPE_SPDIF":
            return ""
        case "PA_DEVICE_PORT_TYPE_HDMI":
            return ""
        case "PA_DEVICE_PORT_TYPE_TV":
            return ""
        case "PA_DEVICE_PORT_TYPE_RADIO":
            return ""
        case "PA_DEVICE_PORT_TYPE_VIDEO":
            return ""
        case "PA_DEVICE_PORT_TYPE_USB":
            return ""
        case "PA_DEVICE_PORT_TYPE_BLUETOOTH":
            return ""
        case "PA_DEVICE_PORT_TYPE_PORTABLE":
            return ""
        case "PA_DEVICE_PORT_TYPE_HANDSFREE":
            return ""
        case "PA_DEVICE_PORT_TYPE_CAR":
            return ""
        case "PA_DEVICE_PORT_TYPE_HIFI":
            return ""
        case "PA_DEVICE_PORT_TYPE_PHONE":
            return ""
        case "PA_DEVICE_PORT_TYPE_NETWORK":
            return ""
        case "PA_DEVICE_PORT_TYPE_ANALOG":
            return ""
        default:
            return ""
    }
}
