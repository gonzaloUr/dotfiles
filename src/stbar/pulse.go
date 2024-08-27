package main

/*
#cgo LDFLAGS: -lpulse
#include <pulse/pulseaudio.h>

// Context callbacks.
extern void stateCallback(pa_context *context, void *userdata);
extern void subscribeCallback(pa_context *context, pa_subscription_event_type_t eventType, uint32_t idx, void *userdata);

// Operation callbacks.
extern void operationSubscribeCallback(pa_context *c, int success, void *userdata);
extern void operationClientInfoCallback(pa_context *c, pa_subscription_event_type_t t, uint32_t idx, void *userdata);
*/
import "C"
import (
	"log"
	"unsafe"
)

type Pulse struct {
	mainloop *C.pa_mainloop
	api      *C.pa_mainloop_api
	context  *C.pa_context
	ready    chan struct{}
	done     chan struct{}
}

func NewPulse(name string) *Pulse {
	ret := &Pulse{}

	// Create mainloop.
	ret.mainloop = C.pa_mainloop_new()

	if ret.mainloop == nil {
		log.Fatal("pa_mainloop_new() failed")
	}

	// Get mainloop api.
	ret.api = C.pa_mainloop_get_api(ret.mainloop)

	// Create context from mainloop api.
	ret.context = C.pa_context_new(ret.api, C.CString(name))

	if ret.context == nil {
		log.Fatal("pa_context_new() failed")
	}

	// Setup callbacks for context.
	C.pa_context_set_state_callback(ret.context, C.pa_context_notify_cb_t(C.stateCallback), unsafe.Pointer(ret))
	C.pa_context_set_subscribe_callback(ret.context, C.pa_context_subscribe_cb_t(C.subscribeCallback), unsafe.Pointer(ret))

	// Connect context.
	if C.pa_context_connect(ret.context, nil, C.PA_CONTEXT_NOFLAGS, nil) < 0 {
		log.Fatal("pa_context_connect() failed")
	}

	return ret
}

func (p *Pulse) Loop() {
	var retCode C.int
	C.pa_mainloop_run(p.mainloop, &retCode)

	if retCode < 0 {
		log.Print("pa_mainloop_run() failed")
	}

	C.pa_mainloop_free(p.mainloop)
	C.pa_context_unref(p.context)
	C.pa_context_disconnect(p.context)

	p.done <- struct{}{}
}

//export stateCallback
func stateCallback(context *C.pa_context, userdata unsafe.Pointer) {
	api := (*C.pa_mainloop_api)(userdata)

	switch state := C.pa_context_get_state(context); state {
	case C.PA_CONTEXT_READY:
		log.Print("context is ready")
		cb := C.pa_context_success_cb_t(C.operationSubscribeCallback)
		if op := C.pa_context_subscribe(context, C.PA_SUBSCRIPTION_MASK_ALL, cb, unsafe.Pointer(api)); op != nil {
			C.pa_operation_unref(op)
		}
	case C.PA_CONTEXT_FAILED, C.PA_CONTEXT_TERMINATED:
		log.Print("quitting mainloop")
		C.pa_mainloop_quit((*C.pa_mainloop)(userdata), 1)
	}
}

//export subscribeCallback
func subscribeCallback(context *C.pa_context, eventType C.pa_subscription_event_type_t, index C.uint32_t, userdata unsafe.Pointer) {
	if eventType&C.PA_SUBSCRIPTION_EVENT_SINK != 0 {
		// log.Print("PA_SUBSCRIPTION_EVENT_SINK")
	}

	if eventType&C.PA_SUBSCRIPTION_EVENT_SOURCE != 0 {
		// log.Print("PA_SUBSCRIPTION_EVENT_SOURCE")
	}

	if eventType&C.PA_SUBSCRIPTION_EVENT_SINK_INPUT != 0 {
		// log.Print("PA_SUBSCRIPTION_EVENT_SINK_INPUT")
	}

	if eventType&C.PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT != 0 {
		// log.Print("PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT")
	}

	if eventType&C.PA_SUBSCRIPTION_EVENT_MODULE != 0 {
		// log.Print("PA_SUBSCRIPTION_EVENT_MODULE")
	}

	if eventType&C.PA_SUBSCRIPTION_EVENT_CLIENT != 0 {
		// log.Print("PA_SUBSCRIPTION_EVENT_CLIENT")
	}

	if eventType&C.PA_SUBSCRIPTION_EVENT_SAMPLE_CACHE != 0 {
		// log.Print("PA_SUBSCRIPTION_EVENT_SAMPLE_CACHE")
	}

	if eventType&C.PA_SUBSCRIPTION_EVENT_SERVER != 0 {
		// log.Print("PA_SUBSCRIPTION_EVENT_SERVER")
	}

	if eventType&C.PA_SUBSCRIPTION_EVENT_CARD != 0 {
		// log.Print("PA_SUBSCRIPTION_EVENT_CARD")
	}

	if eventType&C.PA_SUBSCRIPTION_EVENT_NEW != 0 {
		// log.Print("PA_SUBSCRIPTION_EVENT_NEW")
	}

	if eventType&C.PA_SUBSCRIPTION_EVENT_CHANGE != 0 {
		// log.Print("PA_SUBSCRIPTION_EVENT_CHANGE")
	}

	if eventType&C.PA_SUBSCRIPTION_EVENT_REMOVE != 0 {
		// log.Print("PA_SUBSCRIPTION_EVENT_REMOVE")
	}
}

//export operationSubscribeCallback
func operationSubscribeCallback(context *C.pa_context, success C.int, userdata unsafe.Pointer) {
	log.Printf("success = %d", int(success))
}

//export operationClientInfoCallback
func operationClientInfoCallback(context *C.pa_context, t C.pa_subscription_event_type_t, inx C.uint32_t, userdata unsafe.Pointer) {
	log.Printf("operationClientInfoCallback")
}
