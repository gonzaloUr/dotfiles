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
	"context"
	"runtime/cgo"
	"sync"
	"unsafe"
)

type Pulse struct {
	// Pulseaudio single thread mainloop, permits contexts to connect and run (similar to a mainloop opengl app).
	mainloop *C.pa_mainloop

	// Pulseaudio context, represents a connection to the pulseaudio daemon.
	context *C.pa_context

	// Lock to pulseaudio mainloop and context.
	//
	// The loop goroutine, which calls pulseaudio mainloop single thread execution, should have less priority
	// to access pulseaudio versus each operation, otherwise, because the loop is always running, there is a
	// chance of entering a deadlock. sync.RWMutex gives writers higher priority to readers, that is if a writer
	// wants to access the lock and there are readers waiting, the writer wins (see docs). The loop goroutine
	// will wait on the RLock() while each operation will wait on Lock() to give more priority to operations
	// than the mainloop execution.
	mutex sync.RWMutex

	// Channels to communicate with loop goroutine.
	loopDone chan struct{}

	// Channels to communicate with handleState goroutine.
	states        chan ContextState
	statesRequest chan chan ContextState
	statesCancel  chan chan ContextState
	statesClosing chan chan struct{}
	statesStop    chan struct{}

	// Channels to communicate with handleEvent goroutine.
	events        chan SubscriptionEvent
	eventsRequest chan chan SubscriptionEvent
	eventsCancel  chan chan SubscriptionEvent
	eventsClosing chan chan struct{}
	eventsStop    chan struct{}
}

func NewPulse(name string) (*Pulse, error) {
	p := &Pulse{
		loopDone:      make(chan struct{}),
		states:        make(chan ContextState),
		statesRequest: make(chan chan ContextState),
		statesCancel:  make(chan chan ContextState),
		statesClosing: make(chan chan struct{}),
		statesStop:    make(chan struct{}),
		events:        make(chan SubscriptionEvent),
		eventsRequest: make(chan chan SubscriptionEvent),
		eventsCancel:  make(chan chan SubscriptionEvent),
		eventsClosing: make(chan chan struct{}),
		eventsStop:    make(chan struct{}),
	}

	// Create mainloop.
	p.mainloop = C.pa_mainloop_new()

	if p.mainloop == nil {
		return nil, ErrMainloopNew
	}

	// Get mainloop api.
	api := C.pa_mainloop_get_api(p.mainloop)

	// Create context from mainloop api.
	p.context = C.pa_context_new(api, C.CString(name))

	if p.context == nil {
		return nil, ErrContextNew
	}

	// Setup callbacks for context.
	h := C.uintptr_t(cgo.NewHandle(p))
	C.pa_context_set_state_callback(p.context, C.pa_context_notify_cb_t(C.stateCallback), unsafe.Pointer(&h))
	C.pa_context_set_subscribe_callback(p.context, C.pa_context_subscribe_cb_t(C.subscribeCallback), unsafe.Pointer(&h))

	// Start goroutines that handle state updates and events.
	go p.handleState()
	go p.handleEvent()

	return p, nil
}

func (p *Pulse) loop() {
	var retval C.int

	for {
		p.mutex.RLock()
		C.pa_mainloop_iterate(p.mainloop, 0, &retval)

		if retval < 0 {
			break
		}

		p.mutex.RUnlock()
	}

	C.pa_context_disconnect(p.context)
	C.pa_context_unref(p.context)
	C.pa_mainloop_free(p.mainloop)
	p.mutex.RUnlock()

	for {
		select {
		case p.loopDone <- struct{}{}:
		default:
			return
		}
	}
}

func (p *Pulse) handleState() {
	outs := map[chan ContextState]struct{}{}
	ch := make(chan ContextState)

	for {
		select {
		case s := <-p.states:
			for out := range outs {
				select {
				case out <- s:
				default:
				}
			}

		case p.statesRequest <- ch:
			outs[ch] = struct{}{}
			ch = make(chan ContextState)

		case cancelCh := <-p.statesCancel:
			delete(outs, cancelCh)
			close(cancelCh)

		case <-p.statesStop:
			for out := range outs {
				close(out)
			}
			return
		}
	}
}

func (p *Pulse) handleEvent() {
	outs := map[chan SubscriptionEvent]struct{}{}
	ch := make(chan SubscriptionEvent)

	for {
		select {
		case e := <-p.events:
			for out := range outs {
				select {
				case out <- e:
				default:
				}
			}

		case p.eventsRequest <- ch:
			outs[ch] = struct{}{}
			ch = make(chan SubscriptionEvent)

		case cancelCh := <-p.eventsCancel:
			delete(outs, cancelCh)
			close(cancelCh)

		case <-p.eventsStop:
			for out := range outs {
				close(out)
			}
			return
		}
	}
}

func (p *Pulse) Connect() error {
	p.mutex.Lock()
	ret := C.pa_context_connect(p.context, nil, C.PA_CONTEXT_NOFAIL, nil)
	p.mutex.Unlock()

	if ret < 0 {
		return ErrContextConnect
	}

	return nil
}

func (p *Pulse) Run() {
	go p.loop()
}

func (p *Pulse) ListenStates(ctx context.Context) <-chan ContextState {
	ch := <-p.statesRequest
	return ch
}

func (p *Pulse) Quit() {
	p.eventsStop <- struct{}{}
	p.statesStop <- struct{}{}
	C.pa_mainloop_quit(p.mainloop, 0)
}

func (p *Pulse) Done() <-chan struct{} {
	return p.loopDone
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

	state := C.pa_context_get_state(context)
	p.states <- createContextState(state)
}

//export subscribeCallback
func subscribeCallback(c *C.pa_context, t C.pa_subscription_event_type_t, inx C.uint32_t, userdata unsafe.Pointer) {
	// handle := *(*C.uint32_t)(userdata)
	// h := cgo.Handle(handle)
	// p := h.Value().(*Pulse)
	// subEvent := createSubscriptionEvent(t)
}

//export operationSubscribeCallback
func operationSubscribeCallback(_ *C.pa_context, success C.int, userdata unsafe.Pointer) {
	// handle := *(*C.uint32_t)(userdata)
	// h := cgo.Handle(handle)
	// ch := h.Value().(chan bool)

	// ch <- int(success) != 0
	// close(ch)
}

//export operationClientInfoCallback
func operationClientInfoCallback(_ *C.pa_context, i *C.pa_client_info, eol C.int, userdata unsafe.Pointer) {
	// handle := *(*C.uint32_t)(userdata)
	// h := cgo.Handle(handle)
	// ch := h.Value().(chan ClientInfo)

	// if eol != 0 {
	// 	close(ch)
	// 	return
	// }

	// ch <- createClientInfo(i)
}

//export operationSinkInfoCallback
func operationSinkInfoCallback(_ *C.pa_context, i *C.pa_sink_info, eol C.int, userdata unsafe.Pointer) {
	// handle := *(*C.uint32_t)(userdata)
	// h := cgo.Handle(handle)
	// ch := h.Value().(chan SinkInfo)

	// if eol != 0 {
	// 	close(ch)
	// 	return
	// }

	// ch <- createSinkInfo(i)
}
