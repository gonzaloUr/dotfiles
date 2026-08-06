#include "pulseaudio.h"
#include <pulse/pulseaudio.h>

void ctx_state_callback(pa_context *ctx, void *userdata) {
    ctxStateCallback(ctx, userdata);
}

void ctx_subscribe_callback(pa_context *ctx, pa_subscription_event_type_t t, uint32_t idx, void *userdata) {
    ctxSubscribeCallback(ctx, t, idx, userdata);
}

void ctx_event_callback(pa_context *ctx, const char *name, pa_proplist *pl, void *userdata) {
    ctxEventCallback(ctx, (char*) name, pl, userdata);
}

// A subscribe event related to a sink, a sink is an audio output device, like your speakers, headphones, HDMI audio, etc.
void ctx_sink_info_callback(pa_context *ctx, const pa_sink_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
    ctxSinkInfoCallback(ctx, (pa_sink_info*) i, eol, userdata);
}

// A subscribe event related to a source, which is an audio input device, like a microphone or a virtual capture source.
void ctx_source_info_callback(pa_context *ctx, const pa_source_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
    ctxSourceInfoCallback(ctx, (pa_source_info*) i, eol, userdata);
}

// A subscribe event about a sink input, which is an audio stream that’s going into a sink.
void ctx_sink_input_info_callback(pa_context *ctx, const pa_sink_input_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
    ctxSinkInputInfoCallback(ctx, (pa_sink_input_info*) i, eol, userdata);
}

// A subscribe event about a source output, which is a stream being recorded from a source.
void ctx_source_output_info_callback(pa_context *ctx, const pa_source_output_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
    ctxSourceOutputInfoCallback(ctx, (pa_source_output_info*) i, eol, userdata);
}

// A subscribe event about a PulseAudio module. Modules are plug-ins that provide functionality, like Bluetooth support.
void ctx_module_info_callback(pa_context *ctx, const pa_module_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
    ctxModuleInfoCallback(ctx, (pa_module_info*) i, eol, userdata);
}

// A subscribe event about a PulseAudio client, which is any process connected to the server.
void ctx_client_info_callback(pa_context *ctx, const pa_client_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
    ctxClientInfoCallback(ctx, (pa_client_info*) i, eol, userdata);
}

// A subscribe event about a sample cache item, which is an audio sample stored for quick playback.
void ctx_sample_info_callback(pa_context *ctx, const pa_sample_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
    ctxSampleInfoCallback(ctx, (pa_sample_info*) i, eol, userdata);
}

// Indicates a global server state change.
void ctx_server_info_callback(pa_context *ctx, const pa_server_info *i, void *userdata) {
    ctxServerInfoCallback(ctx, (pa_server_info*) i, userdata);
}

// A subscribe event about an audio card, which is a representation of a physical or virtual sound device.
void ctx_card_info_callback(pa_context *ctx, const pa_card_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
    ctxCardInfoCallback(ctx, (pa_card_info*) i, eol, userdata);
}
