#include "pahook.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    // Create mainloop.
    pa_mainloop *mainloop = pa_mainloop_new();
    if (!mainloop) {
        fprintf(stderr, "error creating mainloop\n");
        return 1;
    }

    // Get mainloop api.
    pa_mainloop_api *api = pa_mainloop_get_api(mainloop);

    // Setup singal handling.
    pa_signal_init(api);
    pa_signal_new(SIGINT, signal_callback, NULL);
    pa_signal_new(SIGTERM, signal_callback, NULL);
    pa_signal_new(SIGHUP, signal_callback, NULL);

    // Create context.
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

void signal_callback(pa_mainloop_api *api, pa_signal_event *e, int sig, void *userdata) {
    api->quit(api, 0);
}

// An event related to the mainloop of this pulseaudio client.
void ctx_state_callback(pa_context *ctx, void *userdata) {
    pa_mainloop *mainloop = userdata;
    pa_context_state_t state = pa_context_get_state(ctx);

    switch (state) {
        case PA_CONTEXT_UNCONNECTED:
            printf("type=state value=PA_CONTEXT_UNCONNECTED\n");
            break;

        case PA_CONTEXT_CONNECTING:
            printf("type=state value=PA_CONTEXT_CONNECTING\n");
            break;

        case PA_CONTEXT_AUTHORIZING:
            printf("type=state value=PA_CONTEXT_AUTHORIZING\n");
            break;

        case PA_CONTEXT_SETTING_NAME:
            printf("type=state value=PA_CONTEXT_SETTING_NAME\n");
            break;

        case PA_CONTEXT_READY:
            printf("type=state value=PA_CONTEXT_READY\n");

            pa_operation *op; 

            // Activate subscriptions to event.
            op = pa_context_subscribe(ctx, PA_SUBSCRIPTION_MASK_ALL, NULL, NULL);
            pa_operation_unref(op);

            // Force a call to ctx_server_info_callback even there is no event in order to get the current state of pulseaudio.
            context_get_callback_userdata *userdata = malloc(sizeof(context_get_callback_userdata));
            userdata->invalid_t_and_inx = 1;
            userdata->userdata = mainloop;

            op = pa_context_get_server_info(ctx, ctx_server_info_callback, userdata);
            pa_operation_unref(op);

            break;

        case PA_CONTEXT_FAILED:
            printf("type=state value=PA_CONTEXT_FAILED\n");
            pa_mainloop_quit(mainloop, 1);
            break;

        case PA_CONTEXT_TERMINATED:
            printf("type=state value=PA_CONTEXT_TERMINATED\n");
            pa_mainloop_quit(mainloop, 0);
            break;
    }

    fflush(stdout);
}

void ctx_event_callback(pa_context *ctx, const char *name, pa_proplist *pl, void *userdata) {
    // TODO.
}

void ctx_subscribe_callback(pa_context *ctx, pa_subscription_event_type_t t, uint32_t idx, void *userdata) {

    context_get_callback_userdata *new_userdata = malloc(sizeof(context_get_callback_userdata));
    new_userdata->invalid_t_and_inx = 0;
    new_userdata->userdata = userdata;
    new_userdata->t = t;
    new_userdata->idx = idx;

    switch (t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {
        case PA_SUBSCRIPTION_EVENT_SINK:
            pa_context_get_sink_info_by_index(ctx, idx, ctx_sink_info_callback, new_userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_SOURCE:
            pa_context_get_source_info_by_index(ctx, idx, ctx_source_info_callback, new_userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_SINK_INPUT:
            pa_context_get_sink_input_info(ctx, idx, ctx_sink_input_info_callback, new_userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT:
            pa_context_get_source_output_info(ctx, idx, ctx_source_output_info_callback, new_userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_MODULE:
            pa_context_get_module_info(ctx, idx, ctx_module_info_callback, new_userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_CLIENT:
            pa_context_get_client_info(ctx, idx, ctx_client_info_callback, new_userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_SAMPLE_CACHE:
            pa_context_get_sample_info_by_index(ctx, idx, ctx_sample_info_callback, new_userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_SERVER:
            pa_context_get_server_info(ctx, ctx_server_info_callback, new_userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_CARD:
            pa_context_get_card_info_by_index(ctx, idx, ctx_card_info_callback, new_userdata);
            break;
    }
}

// An event related to a sink, a sink is an audio output device, like your speakers, headphones, HDMI audio, etc.
void ctx_sink_info_callback(pa_context *ctx, const pa_sink_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;

    context_get_callback_userdata *cb_userdata = (context_get_callback_userdata*) userdata;
    printf("type=sinkinfo ");

    if (!cb_userdata->invalid_t_and_inx) {
        printf("event_type=\"%s\" ", pa_subscribe_event_type_t_event_type_str(cb_userdata->t));
        printf("event_facility=\"%s\" ", pa_subscribe_event_type_t_event_facility_str(cb_userdata->t));
    }

    printf("name=\"%s\" ", i->name);
    printf("index=%d ", i->index);
    printf("desc=\"%s\" ", i->description);
    printf("sample_spec_rate=%u ", i->sample_spec.rate);
    printf("sample_spec_format=\"%s\" ", pa_sample_format_str(i->sample_spec.format));
    printf("sample_spec_channels=%u ", i->sample_spec.channels);
    // TODO: channel_map.
    // TODO: owner_module.

    /*
    printf("volume=\"");
    if (i->volume.channels) {
        for (uint8_t k = 0; k < i->volume.channels - 1; k++) printf("%d,", i->volume.values[k]);
        printf("%d", i->volume.values[i->volume.channels - 1]);
    }
    printf("\" ");
    */
    printf("volume=%d ", pa_cvolume_max(&i->volume));

    printf("mute=%d ", i->mute);
    printf("monitor_source_index=%u ", i->monitor_source);
    printf("monitor_source_name=\"%s\" ", i->monitor_source_name);
    printf("latency=%llu ", (unsigned long long)i->latency);
    printf("driver=\"%s\" ", i->driver);
    // TODO: flags.
    // TODO: proplist.
    printf("configured_latency=%llu ", (unsigned long long)i->configured_latency);
    printf("base_volume=%u ", i->base_volume);
    printf("state=\"%s\" ", pa_sink_state_str(i->state));
    printf("n_volume_steps=%u ", i->n_volume_steps);
    printf("card=%u\n", i->card);
    // TODO: n_ports.
    // TODO: ports.
    // TODO: active_port.
    // TODO: n_formats.
    // TODO: formats.

    fflush(stdout);
    free(userdata);
}

// An event related to a source, which is an audio input device, like a microphone or a virtual capture source.
void ctx_source_info_callback(pa_context *ctx, const pa_source_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;

    context_get_callback_userdata *cb_userdata = (context_get_callback_userdata*) userdata;
    printf("type=sourceinfo ");

    if (!cb_userdata->invalid_t_and_inx) {
        printf("event_type=\"%s\" ", pa_subscribe_event_type_t_event_type_str(cb_userdata->t));
        printf("event_facility=\"%s\" ", pa_subscribe_event_type_t_event_facility_str(cb_userdata->t));
    }

    printf("name=\"%s\" ", i->name);
    printf("index=%d ", i->index);
    printf("desc=\"%s\" ", i->description);
    printf("sample_spec_rate=%u ", i->sample_spec.rate);
    printf("sample_spec_format=\"%s\" ", pa_sample_format_str(i->sample_spec.format));
    printf("sample_spec_channels=%u ", i->sample_spec.channels);
    // TODO: channel_map.
    // TODO: owner_module.

    /*
    printf("volume=\"");
    if (i->volume.channels) {
        for (uint8_t k = 0; k < i->volume.channels - 1; k++) printf("%d,", i->volume.values[k]);
        printf("%d", i->volume.values[i->volume.channels - 1]);
    }
    printf("\" ");
    */

    printf("volume=%d ", pa_cvolume_max(&i->volume));

    printf("mute=%d ", i->mute);
    printf("monitor_of_sink=%d ", i->monitor_of_sink);
    printf("monitor_of_sink_name=\"%s\" ", i->monitor_of_sink_name);
    printf("latency=%llu ", (unsigned long long)i->latency);
    printf("driver=\"%s\" ", i->driver);
    // TODO: flags.
    // TODO: proplist.
    printf("configured_latency=%llu ", (unsigned long long)i->configured_latency);
    printf("base_volume=%u ", i->base_volume);
    printf("state=\"%s\"\n", pa_sink_state_str(i->state));
    // TODO: n_ports.
    // TODO: ports.
    // TODO: active_port.
    // TODO: n_formats.
    // TODO: formats.

    fflush(stdout);
    free(userdata);
}

// An event about a sink input, which is an audio stream that’s going into a sink.
void ctx_sink_input_info_callback(pa_context *ctx, const pa_sink_input_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;

    context_get_callback_userdata *cb_userdata = (context_get_callback_userdata*) userdata;
    printf("type=sinkinputinfo ");

    if (!cb_userdata->invalid_t_and_inx) {
        printf("event_type=\"%s\" ", pa_subscribe_event_type_t_event_type_str(cb_userdata->t));
        printf("event_facility=\"%s\" ", pa_subscribe_event_type_t_event_facility_str(cb_userdata->t));
    }

    printf("name=\"%s\" ", i->name);
    printf("index=%d ", i->index);
    printf("owner_module=%d ", i->owner_module);
    printf("client=%d ", i->client);
    printf("sink=%d ", i->sink);
    printf("sample_spec_rate=%u ", i->sample_spec.rate);
    printf("sample_spec_format=%s ", pa_sample_format_str(i->sample_spec.format));
    printf("sample_spec_channels=%u ", i->sample_spec.channels);
    // TODO: channel map.

    printf("volume=\"");
    if (i->volume.channels) {
        for (uint8_t k = 0; k < i->volume.channels - 1; k++) printf("%d,", i->volume.values[k]);
        printf("%d", i->volume.values[i->volume.channels - 1]);
    }
    printf("\" ");

    printf("buffer_usec=%llu ", (unsigned long long)i->buffer_usec);
    printf("sink_usec=%llu ", (unsigned long long)i->sink_usec);
    printf("resample_method=\"%s\" ", i->resample_method);
    printf("driver=\"%s\" ", i->driver);
    printf("mute=%d ", i->mute);
    // TODO: proplist
    printf("corked=%d ", i->corked);
    printf("has_volume=%d ", i->has_volume);
    printf("volume_writable=%d\n", i->volume_writable);
    // TODO: format.

    fflush(stdout);
    free(userdata);
}

// An event about a source output, which is a stream being recorded from a source.
void ctx_source_output_info_callback(pa_context *ctx, const pa_source_output_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;

    context_get_callback_userdata *cb_userdata = (context_get_callback_userdata*) userdata;
    printf("type=sourceoutputinfo ");

    if (!cb_userdata->invalid_t_and_inx) {
        printf("event_type=\"%s\" ", pa_subscribe_event_type_t_event_type_str(cb_userdata->t));
        printf("event_facility=\"%s\" ", pa_subscribe_event_type_t_event_facility_str(cb_userdata->t));
    }

    printf("name=\"%s\" ", i->name);
    printf("index=%d ", i->index);
    printf("owner_module=%d ", i->owner_module);
    printf("client=%d ", i->client);
    printf("source=%d ", i->client);
    // TODO: sample_spec.
    // TODO: channel_map.
    printf("buffer_usec=%llu ", (unsigned long long)i->buffer_usec);
    printf("source_usec=%llu ", (unsigned long long)i->source_usec);
    printf("resample_method=\"%s\" ", i->resample_method);
    printf("driver=\"%s\" ", i->driver);
    // TODO: proplist.
    printf("corked=%d ", i->corked);
    // TODO: pa_cvolume.
    printf("mute=%d ", i->mute);
    printf("has_volume=%d ", i->has_volume);
    printf("volume_writable=%d\n", i->volume_writable);
    // TODO: format.

    fflush(stdout);
    free(userdata);
}

// An event about a PulseAudio module. Modules are plug-ins that provide functionality, like Bluetooth support.
void ctx_module_info_callback(pa_context *ctx, const pa_module_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;

    context_get_callback_userdata *cb_userdata = (context_get_callback_userdata*) userdata;
    printf("type=moduleinfo ");

    if (!cb_userdata->invalid_t_and_inx) {
        printf("event_type=\"%s\" ", pa_subscribe_event_type_t_event_type_str(cb_userdata->t));
        printf("event_facility=\"%s\" ", pa_subscribe_event_type_t_event_facility_str(cb_userdata->t));
    }

    printf("name=\"%s\" ", i->name);
    printf("index=%d ", i->index);
    printf("argument=\"%s\" ", i->argument);
    printf("n_used=%d\n", i->n_used);
    // TODO: proplist.

    fflush(stdout);
    free(userdata);
}

// An event about a PulseAudio client, which is any process connected to the server.
void ctx_client_info_callback(pa_context *ctx, const pa_client_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;

    context_get_callback_userdata *cb_userdata = (context_get_callback_userdata*) userdata;
    printf("type=clientinfo ");

    if (!cb_userdata->invalid_t_and_inx) {
        printf("event_type=\"%s\" ", pa_subscribe_event_type_t_event_type_str(cb_userdata->t));
        printf("event_facility=\"%s\" ", pa_subscribe_event_type_t_event_facility_str(cb_userdata->t));
    }

    printf("name=\"%s\" ", i->name);
    printf("index=%d ", i->index);
    printf("owner_module=%d ", i->owner_module);
    printf("driver=\"%s\"\n", i->driver);
    // TODO: proplist.

    fflush(stdout);
    free(userdata);
}

// An event about a sample cache item, which is an audio sample stored for quick playback.
void ctx_sample_info_callback(pa_context *ctx, const pa_sample_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;

    context_get_callback_userdata *cb_userdata = (context_get_callback_userdata*) userdata;
    printf("type=sampleinfo ");

    if (!cb_userdata->invalid_t_and_inx) {
        printf("event_type=\"%s\" ", pa_subscribe_event_type_t_event_type_str(cb_userdata->t));
        printf("event_facility=\"%s\" ", pa_subscribe_event_type_t_event_facility_str(cb_userdata->t));
    }

    printf("name=\"%s\" ", i->name);
    printf("index=%d ", i->index);

    printf("volume=\"");
    if (i->volume.channels) {
        for (uint8_t k = 0; k < i->volume.channels - 1; k++) printf("%d,", i->volume.values[k]);
        printf("%d", i->volume.values[i->volume.channels - 1]);
    }
    printf("\" ");

    printf("sample_spec_rate=%u ", i->sample_spec.rate);
    printf("sample_spec_format=%s ", pa_sample_format_str(i->sample_spec.format));
    printf("sample_spec_channels=%u ", i->sample_spec.channels);
    // TODO: channel_map.
    printf("duration=%llu ", (unsigned long long)i->duration);
    printf("bytes=%d ", i->bytes);
    printf("lazy=%d ", i->lazy);
    printf("filename=\"%s\"\n", i->filename);
    // TODO: proplist.

    fflush(stdout);
    free(userdata);
}

// Indicates a global server state change.
void ctx_server_info_callback(pa_context *ctx, const pa_server_info *i, void *userdata) {
    context_get_callback_userdata *cb_userdata = (context_get_callback_userdata*) userdata;
    printf("type=serverinfo ");

    if (!cb_userdata->invalid_t_and_inx) {
        printf("event_type=\"%s\" ", pa_subscribe_event_type_t_event_type_str(cb_userdata->t));
        printf("event_facility=\"%s\" ", pa_subscribe_event_type_t_event_facility_str(cb_userdata->t));
    }

    printf("user_name=\"%s\" ", i->user_name);
    printf("host_name=\"%s\" ", i->host_name);
    printf("server_version=\"%s\" ", i->server_version);
    printf("server_name=\"%s\" ", i->server_name);
    printf("sample_spec_rate=%u ", i->sample_spec.rate);
    printf("sample_spec_format=%s ", pa_sample_format_str(i->sample_spec.format));
    printf("sample_spec_channels=%u ", i->sample_spec.channels);
    printf("default_sink_name=\"%s\" ", i->default_sink_name);
    printf("default_source_name=\"%s\" ", i->default_source_name);
    printf("cookie=%d\n", i->cookie);
    // TODO: channel_map.

    fflush(stdout);
    free(userdata);
}

// An event about an audio card, which is a representation of a physical or virtual sound device.
void ctx_card_info_callback(pa_context *ctx, const pa_card_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;

    context_get_callback_userdata *cb_userdata = (context_get_callback_userdata*) userdata;
    printf("type=cardinfo ");

    if (!cb_userdata->invalid_t_and_inx) {
        printf("event_type=\"%s\" ", pa_subscribe_event_type_t_event_type_str(cb_userdata->t));
        printf("event_facility=\"%s\" ", pa_subscribe_event_type_t_event_facility_str(cb_userdata->t));
    }

    printf("name=\"%s\" ", i->name);
    printf("index=%d ", i->index);
    printf("owner_module=%d ", i->owner_module);
    printf("driver=\"%s\" ", i->driver);
    printf("n_profiles=%d ", i->n_profiles);
    // TODO: profiles.
    // TODO: active_profile.
    // TODO: proplist.
    printf("n_ports=%d\n", i->n_ports);
    // TODO: ports.
    // TODO: profiles2.
    // TODO: active_profile2.
    
    fflush(stdout);
    free(userdata);
}

// Convert a pa_sample_format enum value to a human-readable string.
const char *pa_sample_format_str(pa_sample_format_t format) {
    switch (format) {
        case PA_SAMPLE_U8:
            return "Unsigned 8 Bit PCM (PA_SAMPLE_U8)";
        case PA_SAMPLE_ALAW:
            return "8 Bit a-Law (PA_SAMPLE_ALAW)";
        case PA_SAMPLE_ULAW:
            return "8 Bit mu-Law (PA_SAMPLE_ULAW)";
        case PA_SAMPLE_S16LE:
            return "Signed 16 Bit PCM, little endian (PA_SAMPLE_S16LE)";
        case PA_SAMPLE_S16BE:
            return "Signed 16 Bit PCM, big endian (PA_SAMPLE_S16BE)";
        case PA_SAMPLE_FLOAT32LE:
            return "32 Bit IEEE float, little endian, -1.0 to 1.0 (PA_SAMPLE_FLOAT32LE)";
        case PA_SAMPLE_FLOAT32BE:
            return "32 Bit IEEE float, big endian, -1.0 to 1.0 (PA_SAMPLE_FLOAT32BE)";
        case PA_SAMPLE_S32LE:
            return "Signed 32 Bit PCM, little endian (PA_SAMPLE_S32LE)";
        case PA_SAMPLE_S32BE:
            return "Signed 32 Bit PCM, big endian (PA_SAMPLE_S32BE)";
        case PA_SAMPLE_S24LE:
            return "Signed 24 Bit PCM packed, little endian (PA_SAMPLE_S24LE)";
        case PA_SAMPLE_S24BE:
            return "Signed 24 Bit PCM packed, big endian (PA_SAMPLE_S24BE)";
        case PA_SAMPLE_S24_32LE:
            return "Signed 24 Bit PCM in LSB of 32 Bit words, little endian (PA_SAMPLE_S24_32LE)";
        case PA_SAMPLE_S24_32BE:
            return "Signed 24 Bit PCM in LSB of 32 Bit words, big endian (PA_SAMPLE_S24_32BE)";
        case PA_SAMPLE_MAX:
            return "Upper limit of valid sample types (PA_SAMPLE_MAX)";
        case PA_SAMPLE_INVALID:
        default:
            return "Invalid sample format (PA_SAMPLE_INVALID)";
    }
}

// Convert a pa_sink_state enum value to a human-readable string.
const char *pa_sink_state_str(pa_sink_state_t state) {
    switch (state) {
        case PA_SINK_INVALID_STATE:
            return "Invalid state (PA_SINK_INVALID_STATE): server does not support sink state introspection.";
        case PA_SINK_RUNNING:
            return "Running (PA_SINK_RUNNING): sink is playing and used by at least one non-corked sink-input.";
        case PA_SINK_IDLE:
            return "Idle (PA_SINK_IDLE): sink is playing but there are no non-corked sink-inputs attached.";
        case PA_SINK_SUSPENDED:
            return "Suspended (PA_SINK_SUSPENDED): sink access may be closed (e.g., power-saving).";
        default:
            return "Unknown sink state.";
    }
}

// Return event type as a string.
const char *pa_subscribe_event_type_t_event_type_str(pa_subscription_event_type_t t) {
    switch (t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) {
        case PA_SUBSCRIPTION_EVENT_NEW:
            return "PA_SUBSCRIPTION_EVENT_NEW";
        case PA_SUBSCRIPTION_EVENT_CHANGE:
            return "PA_SUBSCRIPTION_EVENT_CHANGE";
        case PA_SUBSCRIPTION_EVENT_REMOVE:
            return "PA_SUBSCRIPTION_EVENT_REMOVE";
    }

    return NULL;
}

// Return event facility as a string.
const char *pa_subscribe_event_type_t_event_facility_str(pa_subscription_event_type_t t) {
    switch (t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {
        case PA_SUBSCRIPTION_EVENT_SINK:
            return "PA_SUBSCRIPTION_EVENT_SINK";
        case PA_SUBSCRIPTION_EVENT_SOURCE:
            return "PA_SUBSCRIPTION_EVENT_SOURCE";
        case PA_SUBSCRIPTION_EVENT_SINK_INPUT:
            return "PA_SUBSCRIPTION_EVENT_SINK_INPUT";
        case PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT:
            return "PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT";
        case PA_SUBSCRIPTION_EVENT_MODULE:
            return "PA_SUBSCRIPTION_EVENT_MODULE";
        case PA_SUBSCRIPTION_EVENT_CLIENT:
            return "PA_SUBSCRIPTION_EVENT_CLIENT";
        case PA_SUBSCRIPTION_EVENT_SAMPLE_CACHE:
            return "PA_SUBSCRIPTION_EVENT_SAMPLE_CACHE";
        case PA_SUBSCRIPTION_EVENT_SERVER:
            return "PA_SUBSCRIPTION_EVENT_SERVER";
        case PA_SUBSCRIPTION_EVENT_CARD:
            return "PA_SUBSCRIPTION_EVENT_CARD";
    }

    return NULL;
}
