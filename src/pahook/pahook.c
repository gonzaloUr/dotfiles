#include "pahook.h"
#include <stdio.h>

int main() {
    // Create mainloop.
    pa_mainloop *mainloop = pa_mainloop_new();
    if (!mainloop) {
        fprintf(stderr, "error creating mainloop\n");
        return 1;
    }

    // Create context.
    pa_mainloop_api *api = pa_mainloop_get_api(mainloop);
    pa_context *ctx = pa_context_new(api, "pahook");
    if (!ctx) {
        fprintf(stderr, "error creating context\n");
        pa_mainloop_free(mainloop);
        return 1;
    }

    // Setup context callbacks.
    pa_context_set_state_callback(ctx, ctx_state_callback, mainloop);
    pa_context_set_event_callback(ctx, ctx_event_callback, mainloop);
    pa_context_set_subscribe_callback(ctx, ctx_subscribe_callback, mainloop);

    // Connect context.
    if (pa_context_connect(ctx, NULL, PA_CONTEXT_NOFLAGS, NULL) < 0) {
        fprintf(stderr, "error connecting context\n");

        pa_context_unref(ctx);
        pa_mainloop_free(mainloop);

        return 1;
    }

    // Run mainloop.
    int retval;
    pa_mainloop_run(mainloop, &retval);

    // Clean up.
    pa_context_disconnect(ctx);
    pa_context_unref(ctx);
    pa_mainloop_free(mainloop);

    return retval;
}

// no need to lock the mainloop thread because these run in that same thread.
void ctx_state_callback(pa_context *ctx, void *userdata) {
    pa_mainloop *mainloop = userdata;
    pa_context_state_t state = pa_context_get_state(ctx);

    switch (state) {
        case PA_CONTEXT_UNCONNECTED:
            printf("PA_CONTEXT_UNCONNECTED\n");
            break;

        case PA_CONTEXT_CONNECTING:
            printf("PA_CONTEXT_CONNECTING\n");
            break;

        case PA_CONTEXT_AUTHORIZING:
            printf("PA_CONTEXT_AUTHORIZING\n");
            break;

        case PA_CONTEXT_SETTING_NAME:
            printf("PA_CONTEXT_SETTING_NAME\n");
            break;

        case PA_CONTEXT_READY:
            printf("PA_CONTEXT_READY\n");

            pa_operation *op = pa_context_subscribe(ctx, PA_SUBSCRIPTION_MASK_ALL, NULL, NULL);
            pa_operation_unref(op);

            break;

        case PA_CONTEXT_FAILED:
            printf("PA_CONTEXT_FAILED\n");
            pa_mainloop_quit(mainloop, 1);
            break;

        case PA_CONTEXT_TERMINATED:
            printf("PA_CONTEXT_TERMINATED\n");
            pa_mainloop_quit(mainloop, 0);
            break;
    }
}

// no need to lock the mainloop thread because these run in that same thread.
void ctx_event_callback(pa_context *ctx, const char *name, pa_proplist *pl, void *userdata) {
}

// no need to lock the mainloop thread because these run in that same thread.
void ctx_subscribe_callback(pa_context *ctx, pa_subscription_event_type_t t, uint32_t idx, void *userdata) {
    printf("EVENT\n");
    printf("Pulseaudio obj id/index that caused the event: %d\n", idx);

    switch (t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) {
        case PA_SUBSCRIPTION_EVENT_NEW:
            printf("event type: PA_SUBSCRIPTION_EVENT_NEW\n");
            break;
        case PA_SUBSCRIPTION_EVENT_CHANGE:
            printf("event type: PA_SUBSCRIPTION_EVENT_CHANGE\n");
            break;
        case PA_SUBSCRIPTION_EVENT_REMOVE:
            printf("event type: PA_SUBSCRIPTION_EVENT_REMOVE\n");
            break;
    }

    switch (t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {
        case PA_SUBSCRIPTION_EVENT_SINK:
            printf("event facility: PA_SUBSCRIPTION_EVENT_SINK\n");
            pa_context_get_sink_info_by_index(ctx, idx, ctx_sink_info_callback, userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_SOURCE:
            printf("event facility: PA_SUBSCRIPTION_EVENT_SOURCE\n");
            pa_context_get_source_info_by_index(ctx, idx, ctx_source_info_callback, userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_SINK_INPUT:
            printf("event facility: PA_SUBSCRIPTION_EVENT_SINK_INPUT\n");
            pa_context_get_sink_input_info(ctx, idx, ctx_sink_input_info_callback, userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT:
            printf("event facility: PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT\n");
            pa_context_get_source_output_info(ctx, idx, ctx_source_output_info_callback, userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_MODULE:
            printf("event facility: PA_SUBSCRIPTION_EVENT_MODULE\n");
            pa_context_get_module_info(ctx, idx, ctx_module_info_callback, userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_CLIENT:
            printf("event facility: PA_SUBSCRIPTION_EVENT_CLIENT\n");
            pa_context_get_client_info(ctx, idx, ctx_client_info_callback, userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_SAMPLE_CACHE:
            printf("event facility: PA_SUBSCRIPTION_EVENT_SAMPLE_CACHE\n");
            pa_context_get_sample_info_by_index(ctx, idx, ctx_sample_info_callback, userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_SERVER:
            printf("event facility: PA_SUBSCRIPTION_EVENT_SERVER\n");
            pa_context_get_server_info(ctx, ctx_server_info_callback, userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_CARD:
            printf("event facility: PA_SUBSCRIPTION_EVENT_CARD\n");
            pa_context_get_card_info_by_index(ctx, idx, ctx_card_info_callback, userdata);
            break;
    }
}

// An event related to a sink, a sink is an audio output device, like your speakers, headphones, HDMI audio, etc.
// no need to lock the mainloop thread because these run in that same thread.
void ctx_sink_info_callback(pa_context *ctx, const pa_sink_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;

    printf("name: %s\n", i->name);
    printf("desc: %s\n", i->description);

    for (uint8_t k = 0; k < i->volume.channels; k++)
        printf("volume %d: %d\n", k, i->volume.values[k]);
}

// An event related to a source, which is an audio input device, like a microphone or a virtual capture source.
// no need to lock the mainloop thread because these run in that same thread.
void ctx_source_info_callback(pa_context *ctx, const pa_source_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
}

// An event about a sink input, which is an audio stream that’s going to a sink.
// no need to lock the mainloop thread because these run in that same thread.
void ctx_sink_input_info_callback(pa_context *ctx, const pa_sink_input_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
}

// An event about a source output, which is a stream being recorded from a source.
// no need to lock the mainloop thread because these run in that same thread.
void ctx_source_output_info_callback(pa_context *ctx, const pa_source_output_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
}

// An event about a PulseAudio module. Modules are plug-ins that provide functionality, like Bluetooth support.
// no need to lock the mainloop thread because these run in that same thread.
void ctx_module_info_callback(pa_context *ctx, const pa_module_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
}

// An event about a PulseAudio client, which is any process connected to the server.
// no need to lock the mainloop thread because these run in that same thread.
void ctx_client_info_callback(pa_context *ctx, const pa_client_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
}

// An event about a sample cache item, which is an audio sample stored for quick playback.
// no need to lock the mainloop thread because these run in that same thread.
void ctx_sample_info_callback(pa_context *ctx, const pa_sample_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
}

// Indicates a global server state change.
// no need to lock the mainloop thread because these run in that same thread.
void ctx_server_info_callback(pa_context *ctx, const pa_server_info *i, void *userdata) {
}

// An event about an audio card, which is a representation of a physical or virtual sound device.
// no need to lock the mainloop thread because these run in that same thread.
void ctx_card_info_callback(pa_context *ctx, const pa_card_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;
}
