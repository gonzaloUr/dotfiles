#ifndef PAHOOK_H
#define PAHOOK_H

#include <pulse/pulseaudio.h>

void ctx_event_callback(pa_context *ctx, const char *name, pa_proplist *pl, void *userdata);
void ctx_state_callback(pa_context *ctx, void *userdata);

#endif
