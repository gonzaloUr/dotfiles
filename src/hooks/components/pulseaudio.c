#include "pulseaudio.h"
#include <pulse/pulseaudio.h>

pa_component* pa_component_new(const char *name) {
    // Create component.
    pa_component *ret = malloc(sizeof(pa_component));

    // Create mainloop.
    ret->mainloop = pa_threaded_mainloop_new();
    if (!ret->mainloop) {
        free(ret);
        return NULL;
    }

    // Get mainloop api.
    ret->api = pa_threaded_mainloop_get_api(ret->mainloop);

    // Create context.
    ret->ctx = pa_context_new(ret->api, name);
    if (!ret->ctx) {
        pa_threaded_mainloop_free(ret->mainloop);
        free(ret);
        return NULL;
    }

    // Setup context callbacks.
    pa_context_set_state_callback(ret->ctx, ctx_state_callback, ret);
    pa_context_set_subscribe_callback(ret->ctx, ctx_subscribe_callback, ret);
    pa_context_set_event_callback(ret->ctx, ctx_event_callback, ret);

    // Start mainloop thread.
    if (pa_threaded_mainloop_start(ret->mainloop) < 0) {
        pa_context_unref(ret->ctx);
        pa_threaded_mainloop_free(ret->mainloop);
        free(ret);
        return NULL;
    }

    // Connect context.
    pa_threaded_mainloop_lock(ret->mainloop);

    if (pa_context_connect(ret->ctx, NULL, PA_CONTEXT_NOFLAGS, NULL) < 0) {
        pa_context_unref(ret->ctx);
        pa_threaded_mainloop_free(ret->mainloop);
        free(ret);
        return NULL;
    }

    pa_threaded_mainloop_unlock(ret->mainloop);

    return ret;
}

void pa_component_free(pa_component *component) {
    pa_threaded_mainloop_stop(component->mainloop);
    pa_context_disconnect(component->ctx);
    pa_context_unref(component->ctx);
    pa_threaded_mainloop_free(component->mainloop);
    free(component);
}

void ctx_state_callback(pa_context *ctx, void *userdata) {
    pa_component *comp = userdata;
    pa_threaded_mainloop *mainloop = comp->mainloop;
    pa_context_state_t state = pa_context_get_state(ctx);

    // Switch over the state of a connection context.
    switch (state) {
        // The context hasn't been connected yet.
        case PA_CONTEXT_UNCONNECTED:
            break;

        // A connection is being established.
        case PA_CONTEXT_CONNECTING:
            break;

        // The client is authorizing itself to the daemon.
        case PA_CONTEXT_AUTHORIZING:
            break;

        // The client is passing its application name to the daemon.
        case PA_CONTEXT_SETTING_NAME:
            break;

        // The connection is established, the context is ready to execute operations.
        case PA_CONTEXT_READY:
            break;

        // The connection failed or was disconnected.
        case PA_CONTEXT_FAILED:
            break;

        // The connection was terminated cleanly.
        case PA_CONTEXT_TERMINATED:
            break;
    }
}

void ctx_subscribe_callback(pa_context *ctx, pa_subscription_event_type_t t, uint32_t idx, void *userdata) {
    switch (t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {
        case PA_SUBSCRIPTION_EVENT_SINK:
            pa_context_get_sink_info_by_index(ctx, idx, ctx_sink_info_callback, userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_SOURCE:
            pa_context_get_source_info_by_index(ctx, idx, ctx_source_info_callback, userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_SINK_INPUT:
            pa_context_get_sink_input_info(ctx, idx, ctx_sink_input_info_callback, userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT:
            pa_context_get_source_output_info(ctx, idx, ctx_source_output_info_callback, userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_MODULE:
            pa_context_get_module_info(ctx, idx, ctx_module_info_callback, userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_CLIENT:
            pa_context_get_client_info(ctx, idx, ctx_client_info_callback, userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_SAMPLE_CACHE:
            pa_context_get_sample_info_by_index(ctx, idx, ctx_sample_info_callback, userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_SERVER:
            pa_context_get_server_info(ctx, ctx_server_info_callback, userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_CARD:
            pa_context_get_card_info_by_index(ctx, idx, ctx_card_info_callback, userdata);
            break;
    }
}

// A subscribe event related to a sink, a sink is an audio output device, like your speakers, headphones, HDMI audio, etc.
void ctx_sink_info_callback(pa_context *ctx, const pa_sink_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
}

// A subscribe event related to a source, which is an audio input device, like a microphone or a virtual capture source.
void ctx_source_info_callback(pa_context *ctx, const pa_source_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
}

// A subscribe event about a sink input, which is an audio stream that’s going into a sink.
void ctx_sink_input_info_callback(pa_context *ctx, const pa_sink_input_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
}

// A subscribe event about a source output, which is a stream being recorded from a source.
void ctx_source_output_info_callback(pa_context *ctx, const pa_source_output_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
}

// A subscribe event about a PulseAudio module. Modules are plug-ins that provide functionality, like Bluetooth support.
void ctx_module_info_callback(pa_context *ctx, const pa_module_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
}

// A subscribe event about a PulseAudio client, which is any process connected to the server.
void ctx_client_info_callback(pa_context *ctx, const pa_client_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
}

// A subscribe event about a sample cache item, which is an audio sample stored for quick playback.
void ctx_sample_info_callback(pa_context *ctx, const pa_sample_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
}

// Indicates a global server state change.
void ctx_server_info_callback(pa_context *ctx, const pa_server_info *i, void *userdata) {
}

// A subscribe event about an audio card, which is a representation of a physical or virtual sound device.
void ctx_card_info_callback(pa_context *ctx, const pa_card_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
}
