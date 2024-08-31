package pulse

/*
#include <pulse/pulseaudio.h>

// Context callbacks.
extern void stateCallback(pa_context *context, void *userdata);
extern void subscribeCallback(pa_context *context, pa_subscription_event_type_t eventType, uint32_t idx, void *userdata);

// Operation callbacks.
extern void operationSubscribeCallback(pa_context *c, int success, void *userdata);
extern void operationClientInfoCallback(pa_context *c, pa_client_info *i, int eol, void *userdata);
extern void operationSinkInfoCallback(pa_context *c, pa_sink_info *i, int eol, void *userdata);
*/
import "C"
import (
	"fmt"
	"runtime/cgo"
	"sync"
	"unsafe"
)

type Pulse struct {
	mainloop     *C.pa_mainloop
	api          *C.pa_mainloop_api
	context      *C.pa_context
	ready        *sync.Cond
	done         *sync.Cond
	eventsMu     sync.RWMutex
	eventsOut    map[chan SubscriptionEvent]struct{}
	eventsCancel map[chan SubscriptionEvent]<-chan struct{}
}

func NewPulse(name string) (*Pulse, error) {
	ret := &Pulse{
		ready:        sync.NewCond(&sync.Mutex{}),
		done:         sync.NewCond(&sync.Mutex{}),
		eventsOut:    map[chan SubscriptionEvent]struct{}{},
		eventsCancel: map[chan SubscriptionEvent]<-chan struct{}{},
	}

	// Create mainloop.
	ret.mainloop = C.pa_mainloop_new()

	if ret.mainloop == nil {
		return nil, ErrMainloopNew
	}

	// Get mainloop api.
	ret.api = C.pa_mainloop_get_api(ret.mainloop)

	// Create context from mainloop api.
	ret.context = C.pa_context_new(ret.api, C.CString(name))

	if ret.context == nil {
		return nil, ErrContextNew
	}

	// Setup callbacks for context.
	h := C.uintptr_t(cgo.NewHandle(ret))
	C.pa_context_set_state_callback(ret.context, C.pa_context_notify_cb_t(C.stateCallback), unsafe.Pointer(&h))
	C.pa_context_set_subscribe_callback(ret.context, C.pa_context_subscribe_cb_t(C.subscribeCallback), unsafe.Pointer(&h))

	// Connect context.
	if C.pa_context_connect(ret.context, nil, C.PA_CONTEXT_NOFLAGS, nil) < 0 {
		return nil, ErrContextConnect
	}

	return ret, nil
}

func (p *Pulse) Ready() {
	p.ready.L.Lock()
	p.ready.Wait()
	p.ready.L.Unlock()
}

func (p *Pulse) Done() {
	p.done.L.Lock()
	p.done.Wait()
	p.done.L.Unlock()
}

func (p *Pulse) Loop() {
	go func() {
		var retCode C.int
		C.pa_mainloop_run(p.mainloop, &retCode)

		if retCode < 0 {
			panic("pa_mainloop_run() failed")
		}

		C.pa_context_disconnect(p.context)
		C.pa_context_unref(p.context)
		C.pa_mainloop_free(p.mainloop)

		p.done.Broadcast()
	}()
}

func (p *Pulse) Quit() {
	C.pa_mainloop_quit(p.mainloop, 1)
}

func (p *Pulse) Subscribe() error {
	ch := make(chan bool)
	h := C.uint32_t(cgo.NewHandle(ch))

	op := C.pa_context_subscribe(p.context, C.PA_SUBSCRIPTION_MASK_ALL, C.pa_context_success_cb_t(C.operationSubscribeCallback), unsafe.Pointer(&h))

	if op == nil {
		panic("operation is nil")
	}

	C.pa_operation_unref(op)

	if ok := <-ch; !ok {
		return ErrContextSubscribe
	}

	return nil
}

func (p *Pulse) Event(cancel <-chan struct{}) <-chan SubscriptionEvent {
	p.eventsMu.Lock()
	ret := make(chan SubscriptionEvent)
	p.eventsOut[ret] = struct{}{}
	p.eventsCancel[ret] = cancel

	go func() {
		<-cancel
		p.eventsMu.Lock()
		delete(p.eventsOut, ret)
		delete(p.eventsCancel, ret)
		p.eventsMu.Unlock()
	}()

	p.eventsMu.Unlock()

	return ret
}

func (p *Pulse) ClientInfo() []ClientInfo {
	ch := make(chan ClientInfo)
	h := C.uint32_t(cgo.NewHandle(ch))

	idx := C.pa_context_get_index(p.context)
	op := C.pa_context_get_client_info(p.context, idx, C.pa_client_info_cb_t(C.operationClientInfoCallback), unsafe.Pointer(&h))

	if op == nil {
		panic("operation is nil")
	}

	C.pa_operation_unref(op)

	var ret []ClientInfo

	for info := range ch {
		ret = append(ret, info)
	}

	return ret
}

func (p *Pulse) SinkInfo() []SinkInfo {
	ch := make(chan SinkInfo)
	h := C.uint32_t(cgo.NewHandle(ch))

	op := C.pa_context_get_sink_info_list(p.context, C.pa_sink_info_cb_t(C.operationSinkInfoCallback), unsafe.Pointer(&h))

	if op == nil {
		panic("operation is nil")
	}

	C.pa_operation_unref(op)

	var ret []SinkInfo

	for info := range ch {
		ret = append(ret, info)
	}

	return ret
}

//export stateCallback
func stateCallback(context *C.pa_context, userdata unsafe.Pointer) {
	handle := *(*C.uint32_t)(userdata)
	h := cgo.Handle(handle)
	p := h.Value().(*Pulse)

	switch state := C.pa_context_get_state(context); state {
	case C.PA_CONTEXT_READY:
		p.ready.Broadcast()

	case C.PA_CONTEXT_FAILED, C.PA_CONTEXT_TERMINATED:
		p.done.Broadcast()
	}
}

//export subscribeCallback
func subscribeCallback(c *C.pa_context, t C.pa_subscription_event_type_t, inx C.uint32_t, userdata unsafe.Pointer) {
	handle := *(*C.uint32_t)(userdata)
	h := cgo.Handle(handle)
	p := h.Value().(*Pulse)
	subEvent := createSubscriptionEvent(t)

	p.eventsMu.RLock()
	for v := range p.eventsOut {
		fmt.Printf("enviando para %v\n", v)
		v <- subEvent
	}
	p.eventsMu.RUnlock()
}

//export operationSubscribeCallback
func operationSubscribeCallback(_ *C.pa_context, success C.int, userdata unsafe.Pointer) {
	handle := *(*C.uint32_t)(userdata)
	h := cgo.Handle(handle)
	ch := h.Value().(chan bool)

	ch <- int(success) != 0
	close(ch)
}

//export operationClientInfoCallback
func operationClientInfoCallback(_ *C.pa_context, i *C.pa_client_info, eol C.int, userdata unsafe.Pointer) {
	handle := *(*C.uint32_t)(userdata)
	h := cgo.Handle(handle)
	ch := h.Value().(chan ClientInfo)

	if eol != 0 {
		close(ch)
		return
	}

	ch <- createClientInfo(i)
}

//export operationSinkInfoCallback
func operationSinkInfoCallback(_ *C.pa_context, i *C.pa_sink_info, eol C.int, userdata unsafe.Pointer) {
	handle := *(*C.uint32_t)(userdata)
	h := cgo.Handle(handle)
	ch := h.Value().(chan SinkInfo)

	if eol != 0 {
		close(ch)
		return
	}

	ch <- createSinkInfo(i)
}
