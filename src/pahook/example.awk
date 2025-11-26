{
    for (i = 1; i <= NF; i++) {
        split($i, kv, "=")
        gsub(/^"|"$/, "", kv[2])
        event[kv[1]] = kv[2]
    }
}

# An event related to the pulseaudio client.
$1 == "type=state" {
    if (state["value"] == "PA_CONTEXT_TERMINATED") {
        exit
    }
    if (state["value"] == "PA_CONTEXT_FAILED") {
        exit 1
    }
}

# An event related to a sink, a sink is an audio output device, like your speakers, headphones, HDMI audio, etc.
$1 == "type=sinkinfo" {
    print "Event name:", "sinkinfo"
    print "volume:", event["volume"]
    # print "Event type:", event["event_type"]
    # print "Event facility:", event["event_facility"]
}

# An event related to a source, which is an audio input device, like a microphone or a virtual capture source.
$1 == "type=sourceinfo" {
    print "Event name:", "sourceinfo"
    print "volume:", event["volume"]
    # print "Event type:", event["event_type"]
    # print "Event facility:", event["event_facility"]
}

# An event about a sink input, which is an audio stream that’s going into a sink.
$1 == "type=sinkinputinfo" {
    # print "Event name:", "sinkinputinfo"
    # print "Event type:", event["event_type"]
    # print "Event facility:", event["event_facility"]
}

# An event about a source output, which is a stream being recorded from a source.
$1 == "type=sourceoutputinfo" {
    # print "Event name:", "sourceoutputinfo"
    # print "Event type:", event["event_type"]
    # print "Event facility:", event["event_facility"]
}

# An event about a PulseAudio module. Modules are plug-ins that provide functionality, like Bluetooth support.
$1 == "type=moduleinfo" {
    # print "Event name:", "moduleinfo"
    # print "Event type:", event["event_type"]
    # print "Event facility:", event["event_facility"]
}

# An event about a PulseAudio client, which is any process connected to the server.
$1 == "type=clientinfo" {
    # print "Event name:", "clientinfo"
    # print "Event type:", event["event_type"]
    # print "Event facility:", event["event_facility"]
}

# An event about a sample cache item, which is an audio sample stored for quick playback.
$1 == "type=sampleinfo" {
    # print "Event name:", "sampleinfo"
    # print "Event type:", event["event_type"]
    # print "Event facility:", event["event_facility"]
}

# An event about a sample cache item, which is an audio sample stored for quick playback.
$1 == "type=serverinfo" {
    print "Event name:", "serverinfo"
    # print "Event type:", event["event_type"]
    # print "Event facility:", event["event_facility"]
    # print "Default sink name:", event["default_sink_name"]
    # print "Default source name:", event["default_source_name"]
}

# Indicates a global server state change.
$1 == "type=cardinfo" {
    # print "Event name:", "cardinfo"
    # print "Event type:", event["event_type"]
    # print "Event facility:", event["event_facility"]
}
