package pulseaudio

/*
#cgo pkg-config: libpulse
#include "pulseaudio.h"
*/
import "C"

import (
	"fmt"
	"runtime/cgo"
	"unsafe"
)

type Pulseaudio struct {
	mainloop *C.pa_threaded_mainloop
	api      *C.pa_mainloop_api
	ctx      *C.pa_context
	output   chan string
}

func New(name string) (*Pulseaudio, error) {
	ret := &Pulseaudio{}

	h := cgo.NewHandle(ret)
	userdata := unsafe.Pointer(uintptr(h))

	// Create mainloop.
	ret.mainloop = C.pa_threaded_mainloop_new()
	if ret.mainloop == nil {
		return nil, fmt.Errorf("pa_threaded_mainloop_new failed")
	}

	// Get mainloop api.
	ret.api = C.pa_threaded_mainloop_get_api(ret.mainloop)

	// Create context.
	cname := C.CString(name)
	defer C.free(unsafe.Pointer(cname))

	ret.ctx = C.pa_context_new(ret.api, cname)

	if ret.ctx == nil {
		C.pa_threaded_mainloop_free(ret.mainloop)
		return nil, fmt.Errorf("pa_context_new failed")
	}

	// Setup context callbacks.
	C.pa_context_set_state_callback(ret.ctx, (C.pa_context_notify_cb_t)(C.ctx_state_callback), userdata)
	C.pa_context_set_subscribe_callback(ret.ctx, (C.pa_context_subscribe_cb_t)(C.ctx_subscribe_callback), userdata)
	C.pa_context_set_event_callback(ret.ctx, (C.pa_context_event_cb_t)(C.ctx_event_callback), userdata)

	// Start mainloop thread.
	if C.pa_threaded_mainloop_start(ret.mainloop) < 0 {
		C.pa_context_unref(ret.ctx)
		C.pa_threaded_mainloop_free(ret.mainloop)
		return nil, fmt.Errorf("pa_threaded_mainloop_start failed")
	}

	// Connect context.
	C.pa_threaded_mainloop_lock(ret.mainloop)

	if C.pa_context_connect(ret.ctx, nil, C.PA_CONTEXT_NOFLAGS, nil) < 0 {
		C.pa_context_unref(ret.ctx)
		C.pa_threaded_mainloop_free(ret.mainloop)
		return nil, fmt.Errorf("pa_context_connect failed")
	}

	C.pa_threaded_mainloop_unlock(ret.mainloop)

	// Return
	return ret, nil
}

func (p *Pulseaudio) Free() {
	C.pa_threaded_mainloop_stop(p.mainloop)
	C.pa_context_disconnect(p.ctx)
	C.pa_context_unref(p.ctx)
	C.pa_threaded_mainloop_free(p.mainloop)
}

func (p *Pulseaudio) Output() <-chan string {
	return p.output
}

//export ctxStateCallback
func ctxStateCallback(ctx *C.pa_context, userdata unsafe.Pointer) {
	/*
		h := cgo.Handle(userdata)
		pa := h.Value().(*Pulseaudio)
		st := C.pa_context_get_state(ctx)
	*/
}

//export ctxSubscribeCallback
func ctxSubscribeCallback(ctx *C.pa_context, t C.pa_subscription_event_type_t, i C.uint32_t, userdata unsafe.Pointer) {

	// Apply a mask to extract the event type from an event value.
	switch t & C.PA_SUBSCRIPTION_EVENT_FACILITY_MASK {

	case C.PA_SUBSCRIPTION_EVENT_SINK:
		C.pa_context_get_sink_info_by_index(ctx, i, (C.pa_sink_info_cb_t)(C.ctx_sink_info_callback), userdata)

	case C.PA_SUBSCRIPTION_EVENT_SOURCE:
		C.pa_context_get_source_info_by_index(ctx, i, (C.pa_source_info_cb_t)(C.ctx_source_info_callback), userdata)

	case C.PA_SUBSCRIPTION_EVENT_SINK_INPUT:
		C.pa_context_get_sink_input_info(ctx, i, (C.pa_sink_input_info_cb_t)(C.ctx_sink_input_info_callback), userdata)

	case C.PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT:
		C.pa_context_get_source_output_info(ctx, i, (C.pa_source_output_info_cb_t)(C.ctx_source_output_info_callback), userdata)

	case C.PA_SUBSCRIPTION_EVENT_MODULE:
		C.pa_context_get_module_info(ctx, i, (C.pa_module_info_cb_t)(C.ctx_module_info_callback), userdata)

	case C.PA_SUBSCRIPTION_EVENT_CLIENT:
		C.pa_context_get_client_info(ctx, i, (C.pa_client_info_cb_t)(C.ctx_client_info_callback), userdata)

	case C.PA_SUBSCRIPTION_EVENT_SAMPLE_CACHE:
		C.pa_context_get_sample_info_by_index(ctx, i, (C.pa_sample_info_cb_t)(C.ctx_sample_info_callback), userdata)

	case C.PA_SUBSCRIPTION_EVENT_SERVER:
		C.pa_context_get_server_info(ctx, (C.pa_server_info_cb_t)(C.ctx_server_info_callback), userdata)

	case C.PA_SUBSCRIPTION_EVENT_CARD:
		C.pa_context_get_card_info_by_index(ctx, i, (C.pa_card_info_cb_t)(C.ctx_card_info_callback), userdata)
	}
}

//export ctxEventCallback
func ctxEventCallback(ctx *C.pa_context, name *C.char, proplist *C.pa_proplist, userdata unsafe.Pointer) {
	/*
		h := cgo.Handle(userdata)
		pa := h.Value().(*Pulseaudio)
	*/
}

// A subscribe event related to a sink, a sink is an audio output device, like your speakers, headphones, HDMI audio, etc.
//
//export ctxSinkInfoCallback
func ctxSinkInfoCallback(ctx *C.pa_context, info *C.pa_sink_info, eol C.int, userdata unsafe.Pointer) {
	if eol > 0 || info == nil {
		return
	}
}

// A subscribe event related to a source, which is an audio input device, like a microphone or a virtual capture source.
//
//export ctxSourceInfoCallback
func ctxSourceInfoCallback(ctx *C.pa_context, info *C.pa_source_info, eol C.int, userdata unsafe.Pointer) {
	if eol > 0 || info == nil {
		return
	}
}

// A subscribe event about a sink input, which is an audio stream that’s going into a sink.
//
//export ctxSinkInputInfoCallback
func ctxSinkInputInfoCallback(ctx *C.pa_context, info *C.pa_sink_input_info, eol C.int, userdata unsafe.Pointer) {
	if eol > 0 || info == nil {
		return
	}
}

// A subscribe event about a source output, which is a stream being recorded from a source.
//
//export ctxSourceOutputInfoCallback
func ctxSourceOutputInfoCallback(ctx *C.pa_context, info *C.pa_source_output_info, eol C.int, userdata unsafe.Pointer) {
	if eol > 0 || info == nil {
		return
	}
}

// A subscribe event about a PulseAudio module. Modules are plug-ins that provide functionality, like Bluetooth support.
//
//export ctxModuleInfoCallback
func ctxModuleInfoCallback(ctx *C.pa_context, info *C.pa_module_info, eol C.int, userdata unsafe.Pointer) {
	if eol > 0 || info == nil {
		return
	}
}

// A subscribe event about a PulseAudio client, which is any process connected to the server.
//
//export ctxClientInfoCallback
func ctxClientInfoCallback(ctx *C.pa_context, info *C.pa_client_info, eol C.int, userdata unsafe.Pointer) {
	if eol > 0 || info == nil {
		return
	}
}

// A subscribe event about a sample cache item, which is an audio sample stored for quick playback.
//
//export ctxSampleInfoCallback
func ctxSampleInfoCallback(ctx *C.pa_context, info *C.pa_sample_info, eol C.int, userdata unsafe.Pointer) {
	if eol > 0 || info == nil {
		return
	}
}

// Indicates a global server state change.
//
//export ctxServerInfoCallback
func ctxServerInfoCallback(ctx *C.pa_context, info *C.pa_server_info, userdata unsafe.Pointer) {
}

// A subscribe event about an audio card, which is a representation of a physical or virtual sound device.
//
//export ctxCardInfoCallback
func ctxCardInfoCallback(ctx *C.pa_context, info *C.pa_card_info, eol C.int, userdata unsafe.Pointer) {
	if eol > 0 || info == nil {
		return
	}
}
