#include "pahook.h"
#include <pulse/pulseaudio.h>
#include <stdio.h>

int main() {
    // Create threaded mainloop.
    pa_threaded_mainloop *mainloop = pa_threaded_mainloop_new();
    if (!mainloop) {
        fprintf(stderr, "error creating threaded mainloop\n");
        return 1;
    }

    // Create context.
    pa_mainloop_api *api = pa_threaded_mainloop_get_api(mainloop);
    pa_context *ctx = pa_context_new(api, "pahook");
    if (!ctx) {
        fprintf(stderr, "error creating context\n");

        pa_threaded_mainloop_stop(mainloop);
        pa_threaded_mainloop_free(mainloop);

        return 1;
    }

    // Setup context callbacks.
    pa_context_set_state_callback(ctx, ctx_state_callback, mainloop);
    pa_context_set_event_callback(ctx, ctx_event_callback, mainloop);

    // Connect context.
    if (pa_context_connect(ctx, NULL, PA_CONTEXT_NOFLAGS, NULL) < 0) {
        fprintf(stderr, "error connecting context\n");

        pa_threaded_mainloop_stop(mainloop);
        pa_context_disconnect(ctx);
        pa_context_unref(ctx);
        pa_threaded_mainloop_free(mainloop);

        return 1;
    }

    // Lock mainloop.
    pa_threaded_mainloop_lock(mainloop);

    // Start mainloop.
    if (pa_threaded_mainloop_start(mainloop) < 0) {
        fprintf(stderr, "error starting mainloop\n");

        pa_threaded_mainloop_unlock(mainloop);
        pa_threaded_mainloop_stop(mainloop);
        pa_context_disconnect(ctx);
        pa_context_unref(ctx);
        pa_threaded_mainloop_free(mainloop);

        return 1;
    }

    // Wait until ready.
    while (1) {
        pa_context_state_t state = pa_context_get_state(ctx);

        if (state == PA_CONTEXT_READY) {
            break;
        } else if (state == PA_CONTEXT_FAILED || state == PA_CONTEXT_TERMINATED) {
            pa_threaded_mainloop_unlock(mainloop);
            pa_threaded_mainloop_stop(mainloop);
            pa_context_disconnect(ctx);
            pa_context_unref(ctx);
            pa_threaded_mainloop_free(mainloop);

            return 1;
        }

        pa_threaded_mainloop_wait(mainloop);
    }

    pa_threaded_mainloop_unlock(mainloop);
    pa_threaded_mainloop_stop(mainloop);
    pa_context_disconnect(ctx);
    pa_context_unref(ctx);
    pa_threaded_mainloop_free(mainloop);

    return 0;
}

// no locking is necesary because this callback runs inside the mainloop.
void ctx_state_callback(pa_context *ctx, void *userdata) {
    pa_threaded_mainloop *mainloop = userdata;
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
            break;

        case PA_CONTEXT_FAILED:
            printf("PA_CONTEXT_FAILED\n");
            break;

        case PA_CONTEXT_TERMINATED:
            printf("PA_CONTEXT_TERMINATED\n");
            break;
    }

    pa_threaded_mainloop_signal(mainloop, 0);
}

// no locking is necesary because this callback runs inside the mainloop.
void ctx_event_callback(pa_context *ctx, const char *name, pa_proplist *pl, void *userdata) {
}
