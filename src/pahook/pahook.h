#ifndef PAHOOK_H
#define PAHOOK_H

#include <pulse/pulseaudio.h>

// Signal handling.
void signal_callback(pa_mainloop_api *api, pa_signal_event *e, int sig, void *userdata);

// Main context callbacks.
void ctx_event_callback(pa_context *ctx, const char *name, pa_proplist *pl, void *userdata);
void ctx_state_callback(pa_context *ctx, void *userdata);
void ctx_subscribe_callback(pa_context *ctx, pa_subscription_event_type_t t, uint32_t idx, void *userdata);

// Introspect operation callbacks.
void ctx_sink_info_callback(pa_context *ctx, const pa_sink_info *i, int eol, void *userdata);
void ctx_source_info_callback(pa_context *ctx, const pa_source_info *i, int eol, void *userdata);
void ctx_sink_input_info_callback(pa_context *ctx, const pa_sink_input_info *i, int eol, void *userdata);
void ctx_source_output_info_callback(pa_context *ctx, const pa_source_output_info *i, int eol, void *userdata);
void ctx_module_info_callback(pa_context *ctx, const pa_module_info *i, int eol, void *userdata);
void ctx_client_info_callback(pa_context *ctx, const pa_client_info *i, int eol, void *userdata);
void ctx_sample_info_callback(pa_context *ctx, const pa_sample_info *i, int eol, void *userdata);
void ctx_server_info_callback(pa_context *ctx, const pa_server_info *i, void *userdata);
void ctx_card_info_callback(pa_context *ctx, const pa_card_info *i, int eol, void *userdata);

// Utils.
typedef struct {
    void *userdata;
    int invalid_t_and_inx;
    pa_subscription_event_type_t t;
    uint32_t idx;
} context_get_callback_userdata;

const char *pa_sample_format_str(pa_sample_format_t format);
const char *pa_sink_state_str(pa_sink_state_t state);
const char *pa_subscribe_event_type_t_event_type_str(pa_subscription_event_type_t);
const char *pa_subscribe_event_type_t_event_facility_str(pa_subscription_event_type_t);

#endif
