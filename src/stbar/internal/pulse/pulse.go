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
	"fmt"
	"runtime/cgo"
	"unsafe"
)

type Pulse struct {
	// Pulseaudio single thread mainloop, permits contexts to connect and run (similar to a mainloop opengl app).
	mainloop *C.pa_mainloop

	// Pulseaudio context, represents a connection to the pulseaudio daemon.
	context *C.pa_context

	// Channels to communicate with loop goroutine.
	loopStart   chan struct{}
	loopRunning chan bool
	loopPause   chan struct{}
	loopPlay    chan struct{}
	loopQuit    chan struct{}
	loopDone    chan struct{}

	// Channels to communicate with handleState goroutine.
	states        chan ContextState
	statesRequest chan chan ContextState
	statesCancel  chan chan ContextState
	statesStop    chan struct{}

	// Channels to communicate with handleEvent goroutine.
	events        chan SubscriptionEvent
	eventsRequest chan chan SubscriptionEvent
	eventsCancel  chan chan SubscriptionEvent
	eventsStop    chan struct{}
}

func NewPulse(name string) (*Pulse, error) {
	p := &Pulse{
		loopStart:     make(chan struct{}),
		loopRunning:   make(chan bool),
		loopPause:     make(chan struct{}),
		loopPlay:      make(chan struct{}),
		loopQuit:      make(chan struct{}),
		loopDone:      make(chan struct{}),
		states:        make(chan ContextState),
		statesRequest: make(chan chan ContextState),
		statesCancel:  make(chan chan ContextState),
		statesStop:    make(chan struct{}),
		events:        make(chan SubscriptionEvent),
		eventsRequest: make(chan chan SubscriptionEvent),
		eventsCancel:  make(chan chan SubscriptionEvent),
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

	// Start goroutines.
	go p.handleState()
	go p.handleEvent()
	go p.loop()

	return p, nil
}

func (p *Pulse) loop() {
L1:
	for {
		select {
		case <-p.loopStart:
			break L1
		case p.loopRunning <- false:
		case <-p.loopQuit:
		case <-p.loopPause:
			<-p.loopPlay
		}
	}

	var retval C.int

L2:
	for {
		select {
		case <-p.loopStart:
		case p.loopRunning <- true:
		case <-p.loopPause:
			<-p.loopPlay
		case <-p.loopQuit:
			break L2
		default:
			C.pa_mainloop_iterate(p.mainloop, 0, &retval)

			if retval < 0 {
				break L2
			}
		}
	}

	// Disconnecting the context sends state notification, handleState must be running or callback will block.
	C.pa_context_disconnect(p.context)

	// Now stop both goroutines and continue.
	p.eventsStop <- struct{}{}
	p.statesStop <- struct{}{}

	C.pa_context_unref(p.context)
	C.pa_mainloop_free(p.mainloop)

L3:
	for {
		select {
		case p.loopDone <- struct{}{}:
		default:
			break L3
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
	p.loopPause <- struct{}{}
	ret := C.pa_context_connect(p.context, nil, C.PA_CONTEXT_NOFAIL, nil)
	p.loopPlay <- struct{}{}

	if ret < 0 {
		return ErrContextConnect
	}

	return nil
}

func (p *Pulse) Run() {
	p.loopStart <- struct{}{}
}

func (p *Pulse) ListenStates(ctx context.Context) <-chan ContextState {
	ch := <-p.statesRequest
	ret := make(chan ContextState)

	go func() {
	loop:
		for {
			select {
			case v, ok := <-ch:
				if ok {
					ret <- v
				} else {
					close(ret)
					break loop
				}

			case <-ctx.Done():
				p.statesCancel <- ch
				close(ret)
				break loop
			}
		}
	}()

	return ret
}

func (p *Pulse) ListenEvents(ctx context.Context) <-chan SubscriptionEvent {
	ch := <-p.eventsRequest
	ret := make(chan SubscriptionEvent)

	go func() {
	loop:
		for {
			select {
			case v, ok := <-ch:
				if ok {
					ret <- v
				} else {
					close(ret)
					break loop
				}

			case <-ctx.Done():
				p.eventsCancel <- ch
				close(ret)
				break loop
			}
		}
	}()

	return ret
}

func (p *Pulse) Quit() {
	if <-p.loopRunning {
		p.loopQuit <- struct{}{}
	}
}

func (p *Pulse) Done() <-chan struct{} {
	return p.loopDone
}

func (p *Pulse) Subscribe() error {
	p.loopPause <- struct{}{}
	ch := make(chan bool)
	h := C.uint32_t(cgo.NewHandle(ch))

	op := C.pa_context_subscribe(p.context, C.PA_SUBSCRIPTION_MASK_ALL, C.pa_context_success_cb_t(C.operationSubscribeCallback), unsafe.Pointer(&h))
	p.loopPlay <- struct{}{}

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
	p.loopPause <- struct{}{}
	ch := make(chan ClientInfo)
	h := C.uint32_t(cgo.NewHandle(ch))

	idx := C.pa_context_get_index(p.context)
	op := C.pa_context_get_client_info(p.context, idx, C.pa_client_info_cb_t(C.operationClientInfoCallback), unsafe.Pointer(&h))
	p.loopPlay <- struct{}{}

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
	p.loopPause <- struct{}{}
	ch := make(chan SinkInfo)
	h := C.uint32_t(cgo.NewHandle(ch))

	op := C.pa_context_get_sink_info_list(p.context, C.pa_sink_info_cb_t(C.operationSinkInfoCallback), unsafe.Pointer(&h))
	p.loopPlay <- struct{}{}

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
	handle := *(*C.uint32_t)(userdata)
	h := cgo.Handle(handle)
	p := h.Value().(*Pulse)

	fmt.Println("event")
	p.events <- createSubscriptionEvent(t)
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
