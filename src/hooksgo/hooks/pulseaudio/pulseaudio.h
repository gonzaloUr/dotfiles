#ifndef PULSEAUDIO_GO_H
#define PULSEAUDIO_GO_H

#include <pulse/pulseaudio.h>

// Main context callbacks that call their go counterparts due to type conflicts.
void ctx_event_callback(pa_context *ctx, const char *name, pa_proplist *pl, void *userdata);
void ctx_state_callback(pa_context *ctx, void *userdata);
void ctx_subscribe_callback(pa_context *ctx, pa_subscription_event_type_t t, uint32_t idx, void *userdata);

// Go main context callbacks counterparts.
void ctxStateCallback(pa_context*, void*);
void ctxSubscribeCallback(pa_context*, pa_subscription_event_type_t, uint32_t, void*);
void ctxEventCallback(pa_context*, char*, pa_proplist*, void*);

// Introspect or subscribe operation callbacks that call their go counterparts due to type conflicts.
void ctx_sink_info_callback(pa_context *ctx, const pa_sink_info *i, int eol, void *userdata);
void ctx_source_info_callback(pa_context *ctx, const pa_source_info *i, int eol, void *userdata);
void ctx_sink_input_info_callback(pa_context *ctx, const pa_sink_input_info *i, int eol, void *userdata);
void ctx_source_output_info_callback(pa_context *ctx, const pa_source_output_info *i, int eol, void *userdata);
void ctx_module_info_callback(pa_context *ctx, const pa_module_info *i, int eol, void *userdata);
void ctx_client_info_callback(pa_context *ctx, const pa_client_info *i, int eol, void *userdata);
void ctx_sample_info_callback(pa_context *ctx, const pa_sample_info *i, int eol, void *userdata);
void ctx_server_info_callback(pa_context *ctx, const pa_server_info *i, void *userdata);
void ctx_card_info_callback(pa_context *ctx, const pa_card_info *i, int eol, void *userdata);

// Go introspect or subscribe operation callbacks counterparts.
void ctxSinkInfoCallback(pa_context *ctx, pa_sink_info *i, int eol, void *userdata);
void ctxSourceInfoCallback(pa_context *ctx, pa_source_info *i, int eol, void *userdata);
void ctxSinkInputInfoCallback(pa_context *ctx, pa_sink_input_info *i, int eol, void *userdata);
void ctxSourceOutputInfoCallback(pa_context *ctx, pa_source_output_info *i, int eol, void *userdata);
void ctxModuleInfoCallback(pa_context *ctx, pa_module_info *i, int eol, void *userdata);
void ctxClientInfoCallback(pa_context *ctx, pa_client_info *i, int eol, void *userdata);
void ctxSampleInfoCallback(pa_context *ctx, pa_sample_info *i, int eol, void *userdata);
void ctxServerInfoCallback(pa_context *ctx, pa_server_info *i, void *userdata);
void ctxCardInfoCallback(pa_context *ctx, pa_card_info *i, int eol, void *userdata);

#endif
