#include "pahook.h"
#include <stdlib.h>

void* pa_init_component(int pipefd[2]) {
    return NULL;
}

void pa_start_component(void *userdata) {
}

void pa_stop_component(void *userdata) {
}

void pa_free_component(void *userdata) {
}

pa_hook* pa_hook_new() {
    // Create pa_hook.
    pa_hook *ret = malloc(sizeof(pa_hook));

    // Create mainloop.
    ret->mainloop = pa_threaded_mainloop_new();
    if (!ret->mainloop) {
        free(ret);

        return NULL;
    }

    // Get mainloop api.
    ret->api = pa_threaded_mainloop_get_api(ret->mainloop);

    // Create context.
    ret->ctx = pa_context_new(ret->api, "pahook");
    if (!ret->ctx) {
        pa_threaded_mainloop_free(ret->mainloop);
        free(ret);

        return NULL;
    }

    // Setup context callbacks.
    pa_context_set_state_callback(ret->ctx, ctx_state_callback, ret);
    pa_context_set_event_callback(ret->ctx, ctx_event_callback, ret);
    pa_context_set_subscribe_callback(ret->ctx, ctx_subscribe_callback, ret);

    // Connect context.
    pa_threaded_mainloop_lock(ret->mainloop);

    if (pa_context_connect(ret->ctx, NULL, PA_CONTEXT_NOFLAGS, NULL) < 0) {
        pa_context_unref(ret->ctx);
        pa_threaded_mainloop_free(ret->mainloop);
        free(ret);

        return NULL;
    }

    pa_threaded_mainloop_unlock(ret->mainloop);

    // Return.
    return ret;
}

void pa_hook_free(pa_hook *h) {
	pa_threaded_mainloop_stop(h->mainloop);
	pa_context_disconnect(h->ctx);
	pa_context_unref(h->ctx);
	pa_threaded_mainloop_free(h->mainloop);
}

// Callback related to the mainloop of this pulseaudio client.
void ctx_state_callback(pa_context *ctx, void *userdata) {
    pa_context_state_t state = pa_context_get_state(ctx);

    switch (state) {
        case PA_CONTEXT_UNCONNECTED:
            break;

        case PA_CONTEXT_CONNECTING:
            break;

        case PA_CONTEXT_AUTHORIZING:
            break;

        case PA_CONTEXT_SETTING_NAME:
            break;

        case PA_CONTEXT_READY:
            break;

        case PA_CONTEXT_FAILED:
            break;

        case PA_CONTEXT_TERMINATED:
            break;
    }
}

// Init function that prints the current server info.
void init_ctx_server_info_callback(pa_context *ctx, const pa_server_info *i, void *userdata) {
}

// context callback for events.
void ctx_event_callback(pa_context *ctx, const char *name, pa_proplist *pl, void *userdata) {
}

// context callback for subscriptions, changes in the server pretty much, then dispatchs to different functions.
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
