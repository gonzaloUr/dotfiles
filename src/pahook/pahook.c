#include "pahook.h"
#include <stdio.h>
#include <stdlib.h>

#define FS (",")
#define RS ("\n")

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
            printf("type");
            printf(FS);
            printf("state");
            printf(FS);
            printf("value");
            printf(FS);
            printf("PA_CONTEXT_UNCONNECTED");
            printf(RS);
            fflush(stdout);

            break;

        case PA_CONTEXT_CONNECTING:
            printf("type");
            printf(FS);
            printf("state");
            printf(FS);
            printf("value");
            printf(FS);
            printf("PA_CONTEXT_CONNECTING");
            printf(RS);
            fflush(stdout);

            break;

        case PA_CONTEXT_AUTHORIZING:
            printf("type");
            printf(FS);
            printf("states"); 
            printf(FS);
            printf("value");
            printf(FS);
            printf("PA_CONTEXT_AUTHORIZING");
            printf(RS);
            fflush(stdout);

            break;

        case PA_CONTEXT_SETTING_NAME:
            printf("type");
            printf(FS);
            printf("state");
            printf(FS);
            printf("value");
            printf(FS);
            printf("PA_CONTEXT_SETTING_NAME");
            printf(RS);
            fflush(stdout);

            break;

        case PA_CONTEXT_READY:
            printf("type");
            printf(FS);
            printf("state");
            printf(FS);
            printf("value");
            printf(FS);
            printf("PA_CONTEXT_READY");
            printf(RS);
            fflush(stdout);

            pa_context_get_server_info(ctx, init_ctx_server_info_callback, NULL);

            break;

        case PA_CONTEXT_FAILED:
            printf("type");
            printf(FS);
            printf("state");
            printf(FS);
            printf("value");
            printf(FS);
            printf("PA_CONTEXT_FAILED");
            printf(RS);
            fflush(stdout);

            pa_mainloop_quit(mainloop, 1);
            break;

        case PA_CONTEXT_TERMINATED:
            printf("type");
            printf(FS);
            printf("state"); 
            printf(FS);
            printf("value");
            printf(FS);
            printf("PA_CONTEXT_TERMINATED");
            printf(RS);
            fflush(stdout);

            pa_mainloop_quit(mainloop, 0);
            break;
    }

    fflush(stdout);
}

void init_ctx_server_info_callback(pa_context *ctx, const pa_server_info *i, void *userdata) {
    print_pa_server_info(i, NULL);
    pa_context_get_sink_info_by_name(ctx, i->default_sink_name, init_ctx_sink_info_callback, (void*) i);
}

void init_ctx_sink_info_callback(pa_context *ctx, const pa_sink_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;

    print_pa_sink_info(i, NULL);

    pa_server_info *server_info = (pa_server_info*) userdata;
    pa_context_get_source_info_by_name(ctx, server_info->default_source_name, init_ctx_source_info_callback, NULL);
}

void init_ctx_source_info_callback(pa_context *ctx, const pa_source_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;

    print_pa_source_info(i, NULL);

    pa_context_subscribe(ctx, PA_SUBSCRIPTION_MASK_ALL, NULL, NULL);
}

void ctx_event_callback(pa_context *ctx, const char *name, pa_proplist *pl, void *userdata) {
    // TODO.
}

void ctx_subscribe_callback(pa_context *ctx, pa_subscription_event_type_t t, uint32_t idx, void *userdata) {

    callback_userdata *cb_userdata = malloc(sizeof(callback_userdata));
    cb_userdata->userdata = userdata;
    cb_userdata->t = t;
    cb_userdata->idx = idx;

    switch (t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {
        case PA_SUBSCRIPTION_EVENT_SINK:
            pa_context_get_sink_info_by_index(ctx, idx, ctx_sink_info_callback, cb_userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_SOURCE:
            pa_context_get_source_info_by_index(ctx, idx, ctx_source_info_callback, cb_userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_SINK_INPUT:
            pa_context_get_sink_input_info(ctx, idx, ctx_sink_input_info_callback, cb_userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT:
            pa_context_get_source_output_info(ctx, idx, ctx_source_output_info_callback, cb_userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_MODULE:
            pa_context_get_module_info(ctx, idx, ctx_module_info_callback, cb_userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_CLIENT:
            pa_context_get_client_info(ctx, idx, ctx_client_info_callback, cb_userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_SAMPLE_CACHE:
            pa_context_get_sample_info_by_index(ctx, idx, ctx_sample_info_callback, cb_userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_SERVER:
            pa_context_get_server_info(ctx, ctx_server_info_callback, cb_userdata);
            break;
        case PA_SUBSCRIPTION_EVENT_CARD:
            pa_context_get_card_info_by_index(ctx, idx, ctx_card_info_callback, cb_userdata);
            break;
    }
}

// An event related to a sink, a sink is an audio output device, like your speakers, headphones, HDMI audio, etc.
void ctx_sink_info_callback(pa_context *ctx, const pa_sink_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;

    callback_userdata *cb_userdata = (callback_userdata*) userdata;
    print_pa_sink_info(i, cb_userdata);
    free(userdata);
}

// An event related to a source, which is an audio input device, like a microphone or a virtual capture source.
void ctx_source_info_callback(pa_context *ctx, const pa_source_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;

    callback_userdata *cb_userdata = (callback_userdata*) userdata;
    print_pa_source_info(i, cb_userdata);
    free(userdata);
}

// An event about a sink input, which is an audio stream that’s going into a sink.
void ctx_sink_input_info_callback(pa_context *ctx, const pa_sink_input_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;

    callback_userdata *cb_userdata = (callback_userdata*) userdata;
    print_pa_sink_input_info(i, cb_userdata);
    free(userdata);
}

// An event about a source output, which is a stream being recorded from a source.
void ctx_source_output_info_callback(pa_context *ctx, const pa_source_output_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;

    callback_userdata *cb_userdata = (callback_userdata*) userdata;
    print_pa_source_output_info(i, cb_userdata);
    free(userdata);
}

// An event about a PulseAudio module. Modules are plug-ins that provide functionality, like Bluetooth support.
void ctx_module_info_callback(pa_context *ctx, const pa_module_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;

    callback_userdata *cb_userdata = (callback_userdata*) userdata;
    print_pa_module_info(i, cb_userdata);
    free(userdata);
}

// An event about a PulseAudio client, which is any process connected to the server.
void ctx_client_info_callback(pa_context *ctx, const pa_client_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;

    callback_userdata *cb_userdata = (callback_userdata*) userdata;
    print_pa_client_info(i, cb_userdata);
    free(userdata);
}

// An event about a sample cache item, which is an audio sample stored for quick playback.
void ctx_sample_info_callback(pa_context *ctx, const pa_sample_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;

    callback_userdata *cb_userdata = (callback_userdata*) userdata;
    print_pa_sample_info(i, cb_userdata);
    free(userdata);
}

// Indicates a global server state change.
void ctx_server_info_callback(pa_context *ctx, const pa_server_info *i, void *userdata) {
    callback_userdata *cb_userdata = (callback_userdata*) userdata;
    print_pa_server_info(i, cb_userdata);
    free(userdata);
}

// An event about an audio card, which is a representation of a physical or virtual sound device.
void ctx_card_info_callback(pa_context *ctx, const pa_card_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;

    callback_userdata *cb_userdata = (callback_userdata*) userdata;
    print_pa_card_info(i, cb_userdata);
    free(userdata);
}

void print_pa_sink_info(const pa_sink_info *i, const callback_userdata *cb_userdata) {
    printf("type"); 
    printf(FS);
    printf("sinkinfo");
    printf(FS);

    if (cb_userdata) {
        printf("event_type");
        printf(FS);
        printf("\"%s\"", pa_subscribe_event_type_t_event_type_str(cb_userdata->t));
        printf(FS);
        printf("event_facility"); 
        printf(FS);
        printf("\"%s\"", pa_subscribe_event_type_t_event_facility_str(cb_userdata->t));
        printf(FS);
    }

    printf("name"); 
    printf(FS);
    printf("\"%s\"", i->name);
    printf(FS);
    printf("index"); 
    printf(FS);
    printf("%d", i->index);
    printf(FS);
    printf("desc"); 
    printf(FS);
    printf("\"%s\"", i->description);
    printf(FS);
    printf("sample_spec_rate"); 
    printf(FS);
    printf("%u", i->sample_spec.rate);
    printf(FS);
    printf("sample_spec_format"); 
    printf(FS);
    printf("\"%s\"", pa_sample_format_str(i->sample_spec.format));
    printf(FS);
    printf("sample_spec_channels"); 
    printf(FS);
    printf("%u", i->sample_spec.channels);
    printf(FS);
    // TODO: channel_map.
    // TODO: owner_module.
    printf("volume");
    printf(FS);
    printf("%d", pa_cvolume_max(&i->volume));
    printf(FS);
    printf("mute"); 
    printf(FS);
    printf("%d", i->mute);
    printf(FS);
    printf("monitor_source_index"); 
    printf(FS);
    printf("%u", i->monitor_source);
    printf(FS);
    printf("monitor_source_name"); 
    printf(FS);
    printf("\"%s\"", i->monitor_source_name);
    printf(FS);
    printf("latency"); 
    printf(FS);
    printf("%llu", (unsigned long long)i->latency);
    printf(FS);
    printf("driver"); 
    printf(FS);
    printf("\"%s\"", i->driver);
    printf(FS);
    // TODO: flags.
    // TODO: proplist.
    printf("configured_latency"); 
    printf(FS);
    printf("%llu", (unsigned long long)i->configured_latency);
    printf(FS);
    printf("base_volume"); 
    printf(FS);
    printf("%u", i->base_volume);
    printf(FS);
    printf("state"); 
    printf(FS);
    printf("\"%s\"", pa_sink_state_str(i->state));
    printf(FS);
    printf("n_volume_steps"); 
    printf(FS);
    printf("%u", i->n_volume_steps);
    printf(FS);
    printf("card"); 
    printf(FS);
    printf("%u", i->card);
    printf(FS);
    // TODO: n_ports.
    // TODO: ports.

    if (i->active_port) {
        printf("active_port_name");
        printf(FS);
        printf("\"%s\"", i->active_port->name);
        printf(FS);
        printf("active_port_desc");
        printf(FS);
        printf("\"%s\"", i->active_port->description);
        printf(FS);
        printf("active_port_type");
        printf(FS);
        printf("\"%s\"", pa_device_port_type_str(i->active_port->type));
    }

    // TODO: n_formats.
    // TODO: formats.
    printf(RS);

    fflush(stdout);
}

void print_pa_source_info(const pa_source_info *i, const callback_userdata *cb_userdata) {
    printf("type"); 
    printf(FS);
    printf("sourceinfo");
    printf(FS);

    if (cb_userdata) {
        printf("event_type"); 
        printf(FS);
        printf("\"%s\"", pa_subscribe_event_type_t_event_type_str(cb_userdata->t));
        printf(FS);
        printf("event_facility"); 
        printf(FS);
        printf("\"%s\"", pa_subscribe_event_type_t_event_facility_str(cb_userdata->t));
        printf(FS);
    }

    printf("name"); 
    printf(FS);
    printf("\"%s\"", i->name);
    printf(FS);
    printf("index"); 
    printf(FS);
    printf("%d", i->index);
    printf(FS);
    printf("desc"); 
    printf(FS);
    printf("\"%s\"", i->description);
    printf(FS);
    printf("sample_spec_rate"); 
    printf(FS);
    printf("%u", i->sample_spec.rate);
    printf(FS);
    printf("sample_spec_format"); 
    printf(FS);
    printf("\"%s\"", pa_sample_format_str(i->sample_spec.format));
    printf(FS);
    printf("sample_spec_channels"); 
    printf(FS);
    printf("%u", i->sample_spec.channels);
    printf(FS);
    // TODO: channel_map.
    // TODO: owner_module.
    printf("volume"); 
    printf(FS);
    printf("%d", pa_cvolume_max(&i->volume));
    printf(FS);
    printf("mute"); 
    printf(FS);
    printf("%d", i->mute);
    printf(FS);
    printf("monitor_of_sink"); 
    printf(FS);
    printf("%d", i->monitor_of_sink);
    printf(FS);
    printf("monitor_of_sink_name"); 
    printf(FS);
    printf("\"%s\"", i->monitor_of_sink_name);
    printf(FS);
    printf("latency"); 
    printf(FS);
    printf("%llu", (unsigned long long)i->latency);
    printf(FS);
    printf("driver"); 
    printf(FS);
    printf("\"%s\"", i->driver);
    printf(FS);
    // TODO: flags.
    // TODO: proplist.
    printf("configured_latency"); 
    printf(FS);
    printf("%llu", (unsigned long long)i->configured_latency);
    printf(FS);
    printf("base_volume"); 
    printf(FS);
    printf("%u", i->base_volume);
    printf(FS);
    printf("state"); 
    printf(FS);
    printf("\"%s\"", pa_sink_state_str(i->state));
    printf(FS);
    // TODO: n_ports.
    // TODO: ports.

    if (i->active_port) {
        printf("active_port_name");
        printf(FS);
        printf("\"%s\"", i->active_port->name);
        printf(FS);
        printf("active_port_desc");
        printf(FS);
        printf("\"%s\"", i->active_port->description);
        printf(FS);
        printf("active_port_type");
        printf(FS);
        printf("\"%s\"", pa_device_port_type_str(i->active_port->type));
    }

    // TODO: n_formats.
    // TODO: formats.
    printf(RS);

    fflush(stdout);
}

void print_pa_sink_input_info(const pa_sink_input_info *i, const callback_userdata *cb_userdata) {
    printf("type"); 
    printf(FS);
    printf("sinkinputinfo");
    printf(FS);

    if (cb_userdata) {
        printf("event_type");
        printf(FS);
        printf("\"%s\"", pa_subscribe_event_type_t_event_type_str(cb_userdata->t));
        printf(FS);
        printf("event_facility"); 
        printf(FS);
        printf("\"%s\"", pa_subscribe_event_type_t_event_facility_str(cb_userdata->t));
        printf(FS);
    }

    printf("name"); 
    printf(FS);
    printf("\"%s\"", i->name);
    printf(FS);
    printf("index"); 
    printf(FS);
    printf("%d", i->index);
    printf(FS);
    printf("owner_module"); 
    printf(FS);
    printf("%d", i->owner_module);
    printf(FS);
    printf("client"); 
    printf(FS);
    printf("%d", i->client);
    printf(FS);
    printf("sink"); 
    printf(FS);
    printf("%d", i->sink);
    printf(FS);
    printf("sample_spec_rate"); 
    printf(FS);
    printf("%u", i->sample_spec.rate);
    printf(FS);
    printf("sample_spec_format"); 
    printf(FS);
    printf("%s", pa_sample_format_str(i->sample_spec.format));
    printf(FS);
    printf("sample_spec_channels"); 
    printf(FS);
    printf("%u ", i->sample_spec.channels);
    printf(FS);
    // TODO: channel map.
    printf("volume"); 
    printf(FS);
    printf("%d", pa_cvolume_max(&i->volume));
    printf(FS);
    printf("buffer_usec"); 
    printf(FS);
    printf("%llu", (unsigned long long)i->buffer_usec);
    printf(FS);
    printf("sink_usec"); 
    printf(FS);
    printf("%llu", (unsigned long long)i->sink_usec);
    printf(FS);
    printf("resample_method"); 
    printf(FS);
    printf("\"%s\"", i->resample_method);
    printf(FS);
    printf("driver"); 
    printf(FS);
    printf("\"%s\"", i->driver);
    printf(FS);
    printf("mute"); 
    printf(FS);
    printf("%d", i->mute);
    printf(FS);
    // TODO: proplist
    printf("corked"); 
    printf(FS);
    printf("%d", i->corked);
    printf(FS);
    printf("has_volume"); 
    printf(FS);
    printf("%d", i->has_volume);
    printf(FS);
    printf("volume_writable"); 
    printf(FS);
    printf("%d", i->volume_writable);
    // TODO: format.
    printf(RS);

    fflush(stdout);
}

void print_pa_source_output_info(const pa_source_output_info *i, const callback_userdata *cb_userdata) {
    printf("type"); 
    printf(FS);
    printf("sourceoutputinfo");
    printf(FS);

    if (cb_userdata) {
        printf("event_type"); 
        printf(FS);
        printf("\"%s\"", pa_subscribe_event_type_t_event_type_str(cb_userdata->t));
        printf(FS);
        printf("event_facility"); 
        printf(FS);
        printf("\"%s\"", pa_subscribe_event_type_t_event_facility_str(cb_userdata->t));
        printf(FS);
    }

    printf("name"); 
    printf(FS);
    printf("\"%s\"", i->name);
    printf(FS);
    printf("index"); 
    printf(FS);
    printf("%d", i->index);
    printf(FS);
    printf("owner_module"); 
    printf(FS);
    printf("%d", i->owner_module);
    printf(FS);
    printf("client"); 
    printf(FS);
    printf("%d", i->client);
    printf(FS);
    printf("source"); 
    printf(FS);
    printf("%d", i->client);
    printf(FS);
    // TODO: sample_spec.
    // TODO: channel_map.
    printf("buffer_usec"); 
    printf(FS);
    printf("%llu", (unsigned long long)i->buffer_usec);
    printf(FS);
    printf("source_usec"); 
    printf(FS);
    printf("%llu", (unsigned long long)i->source_usec);
    printf(FS);
    printf("resample_method"); 
    printf(FS);
    printf("\"%s\"", i->resample_method);
    printf(FS);
    printf("driver"); 
    printf(FS);
    printf("\"%s\"", i->driver);
    printf(FS);
    // TODO: proplist.
    printf("corked"); 
    printf(FS);
    printf("%d", i->corked);
    printf(FS);
    // TODO: pa_cvolume.
    printf("mute"); 
    printf(FS);
    printf("%d", i->mute);
    printf(FS);
    printf("has_volume"); 
    printf(FS);
    printf("%d", i->has_volume);
    printf(FS);
    printf("volume_writable"); 
    printf(FS);
    printf("%d", i->volume_writable);
    // TODO: format.
    printf(RS);

    fflush(stdout);
}

void print_pa_module_info(const pa_module_info *i, const callback_userdata *cb_userdata) {
    printf("type"); 
    printf(FS);
    printf("moduleinfo");
    printf(FS);

    if (cb_userdata) {
        printf("event_type");
        printf(FS);
        printf("\"%s\"", pa_subscribe_event_type_t_event_type_str(cb_userdata->t));
        printf(FS);
        printf("event_facility"); 
        printf(FS);
        printf("\"%s\"", pa_subscribe_event_type_t_event_facility_str(cb_userdata->t));
        printf(FS);
    }

    printf("name"); 
    printf(FS);
    printf("\"%s\"", i->name);
    printf(FS);
    printf("index"); 
    printf(FS);
    printf("%d", i->index);
    printf(FS);
    printf("argument"); 
    printf(FS);
    printf("\"%s\"", i->argument);
    printf(FS);
    printf("n_used"); 
    printf(FS);
    printf("%d", i->n_used);
    // TODO: proplist.
    printf(RS);

    fflush(stdout);
}

void print_pa_client_info(const pa_client_info *i, const callback_userdata *cb_userdata) {
    printf("type"); 
    printf(FS);
    printf("clientinfo");
    printf(FS);

    if (cb_userdata) {
        printf("event_type");
        printf(FS);
        printf("\"%s\"", pa_subscribe_event_type_t_event_type_str(cb_userdata->t));
        printf(FS);
        printf("event_facility"); 
        printf(FS);
        printf("\"%s\"", pa_subscribe_event_type_t_event_facility_str(cb_userdata->t));
        printf(FS);
    }

    printf("name"); 
    printf(FS);
    printf("\"%s\"", i->name);
    printf(FS);
    printf("index"); 
    printf(FS);
    printf("%d", i->index);
    printf(FS);
    printf("owner_module"); 
    printf(FS);
    printf("%d", i->owner_module);
    printf(FS);
    printf("driver"); 
    printf(FS);
    printf("\"%s\"", i->driver);
    // TODO: proplist.
    printf(RS);

    fflush(stdout);
}

void print_pa_sample_info(const pa_sample_info *i, const callback_userdata *cb_userdata) {
    printf("type"); 
    printf(FS);
    printf("sampleinfo");
    printf(FS);

    if (cb_userdata) {
        printf("event_type");
        printf(FS);
        printf("\"%s\"", pa_subscribe_event_type_t_event_type_str(cb_userdata->t));
        printf(FS);
        printf("event_facility"); 
        printf(FS);
        printf("\"%s\"", pa_subscribe_event_type_t_event_facility_str(cb_userdata->t));
        printf(FS);
    }

    printf("name"); 
    printf(FS);
    printf("\"%s\"", i->name);
    printf(FS);
    printf("index"); 
    printf(FS);
    printf("%d", i->index);
    printf(FS);
    printf("volume"); 
    printf(FS);
    printf("%d", pa_cvolume_max(&i->volume));
    printf(FS);
    printf("sample_spec_rate"); 
    printf(FS);
    printf("%u", i->sample_spec.rate);
    printf(FS);
    printf("sample_spec_format"); 
    printf(FS);
    printf("\"%s\"", pa_sample_format_str(i->sample_spec.format));
    printf(FS);
    printf("sample_spec_channels"); 
    printf(FS);
    printf("%u", i->sample_spec.channels);
    printf(FS);
    // TODO: channel_map.
    printf("duration"); 
    printf(FS);
    printf("%llu", (unsigned long long)i->duration);
    printf(FS);
    printf("bytes"); 
    printf(FS);
    printf("%d", i->bytes);
    printf(FS);
    printf("lazy"); 
    printf(FS);
    printf("%d", i->lazy);
    printf(FS);
    printf("filename"); 
    printf(FS);
    printf("\"%s\"", i->filename);
    // TODO: proplist.
    printf(RS);

    fflush(stdout);
}

void print_pa_server_info(const pa_server_info *i, const callback_userdata *cb_userdata) {
    printf("type"); 
    printf(FS);
    printf("serverinfo");
    printf(FS);

    if (cb_userdata) {
        printf("event_type");
        printf(FS);
        printf("\"%s\"", pa_subscribe_event_type_t_event_type_str(cb_userdata->t));
        printf(FS);
        printf("event_facility"); 
        printf(FS);
        printf("\"%s\"", pa_subscribe_event_type_t_event_facility_str(cb_userdata->t));
        printf(FS);
    }

    printf("user_name"); 
    printf(FS);
    printf("\"%s\"", i->user_name);
    printf(FS);
    printf("host_name"); 
    printf(FS);
    printf("\"%s\"", i->host_name);
    printf(FS);
    printf("server_version"); 
    printf(FS);
    printf("\"%s\"", i->server_version);
    printf(FS);
    printf("server_name"); 
    printf(FS);
    printf("\"%s\"", i->server_name);
    printf(FS);
    printf("sample_spec_rate"); 
    printf(FS);
    printf("%u", i->sample_spec.rate);
    printf(FS);
    printf("sample_spec_format"); 
    printf(FS);
    printf("\"%s\"", pa_sample_format_str(i->sample_spec.format));
    printf(FS);
    printf("sample_spec_channels"); 
    printf(FS);
    printf("%u", i->sample_spec.channels);
    printf(FS);
    printf("default_sink_name"); 
    printf(FS);
    printf("\"%s\"", i->default_sink_name);
    printf(FS);
    printf("default_source_name"); 
    printf(FS);
    printf("\"%s\"", i->default_source_name);
    printf(FS);
    printf("cookie"); 
    printf(FS);
    printf("%d", i->cookie);
    // TODO: channel_map.
    printf(RS);

    fflush(stdout);
}

void print_pa_card_info(const pa_card_info *i, const callback_userdata *cb_userdata) {
    printf("type");
    printf(FS);
    printf("cardinfo");
    printf(FS);

    if (cb_userdata) {
        printf("event_type");
        printf(FS);
        printf("\"%s\"", pa_subscribe_event_type_t_event_type_str(cb_userdata->t));
        printf(FS);
        printf("event_facility"); 
        printf(FS);
        printf("\"%s\"", pa_subscribe_event_type_t_event_facility_str(cb_userdata->t));
        printf(FS);
    }

    printf("name");
    printf(FS);
    printf("\"%s\"", i->name);
    printf(FS);
    printf("index"); 
    printf(FS);
    printf("%d", i->index);
    printf(FS);
    printf("owner_module"); 
    printf(FS);
    printf("%d", i->owner_module);
    printf(FS);
    printf("driver"); 
    printf(FS);
    printf("\"%s\"", i->driver);
    printf(FS);
    printf("n_profiles"); 
    printf(FS);
    printf("%d", i->n_profiles);
    printf(FS);
    // TODO: profiles.
    // TODO: active_profile.
    // TODO: proplist.
    printf("n_ports"); 
    printf(FS);
    printf("%d", i->n_ports);
    // TODO: ports.
    // TODO: profiles2.
    // TODO: active_profile2.
    printf(RS);

    fflush(stdout);
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

// Return device port type as a string.
const char *pa_device_port_type_str(pa_device_port_type_t t) {
    switch (t) {
        case PA_DEVICE_PORT_TYPE_UNKNOWN:
            return "PA_DEVICE_PORT_TYPE_UNKNOWN";
        case PA_DEVICE_PORT_TYPE_AUX:
            return "PA_DEVICE_PORT_TYPE_AUX";
        case PA_DEVICE_PORT_TYPE_SPEAKER:
            return "PA_DEVICE_PORT_TYPE_SPEAKER";
        case PA_DEVICE_PORT_TYPE_HEADPHONES:
            return "PA_DEVICE_PORT_TYPE_HEADPHONES";
        case PA_DEVICE_PORT_TYPE_LINE:
            return "PA_DEVICE_PORT_TYPE_LINE";
        case PA_DEVICE_PORT_TYPE_MIC:
            return "PA_DEVICE_PORT_TYPE_MIC";
        case PA_DEVICE_PORT_TYPE_HEADSET:
            return "PA_DEVICE_PORT_TYPE_HEADSET";
        case PA_DEVICE_PORT_TYPE_HANDSET:
            return "PA_DEVICE_PORT_TYPE_HANDSET";
        case PA_DEVICE_PORT_TYPE_EARPIECE:
            return "PA_DEVICE_PORT_TYPE_EARPIECE";
        case PA_DEVICE_PORT_TYPE_SPDIF:
            return "PA_DEVICE_PORT_TYPE_SPDIF";
        case PA_DEVICE_PORT_TYPE_HDMI:
            return "PA_DEVICE_PORT_TYPE_HDMI";
        case PA_DEVICE_PORT_TYPE_TV:
            return "PA_DEVICE_PORT_TYPE_TV";
        case PA_DEVICE_PORT_TYPE_RADIO:
            return "PA_DEVICE_PORT_TYPE_RADIO";
        case PA_DEVICE_PORT_TYPE_VIDEO:
            return "PA_DEVICE_PORT_TYPE_VIDEO";
        case PA_DEVICE_PORT_TYPE_USB:
            return "PA_DEVICE_PORT_TYPE_USB";
        case PA_DEVICE_PORT_TYPE_BLUETOOTH:
            return "PA_DEVICE_PORT_TYPE_BLUETOOTH";
        case PA_DEVICE_PORT_TYPE_PORTABLE:
            return "PA_DEVICE_PORT_TYPE_PORTABLE";
        case PA_DEVICE_PORT_TYPE_HANDSFREE:
            return "PA_DEVICE_PORT_TYPE_HANDSFREE";
        case PA_DEVICE_PORT_TYPE_CAR:
            return "PA_DEVICE_PORT_TYPE_CAR";
        case PA_DEVICE_PORT_TYPE_HIFI:
            return "PA_DEVICE_PORT_TYPE_HIFI";
        case PA_DEVICE_PORT_TYPE_PHONE:
            return "PA_DEVICE_PORT_TYPE_PHONE";
        case PA_DEVICE_PORT_TYPE_NETWORK:
            return "PA_DEVICE_PORT_TYPE_NETWORK";
        case PA_DEVICE_PORT_TYPE_ANALOG:
            return "PA_DEVICE_PORT_TYPE_ANALOG";
    }

    return NULL;
}
