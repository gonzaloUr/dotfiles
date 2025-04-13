package pulse

/*
#include <pulse/pulseaudio.h>

// Context callbacks.
extern void stateCallback(pa_context *context, void *userdata);
extern void subscribeCallback(pa_context *context, pa_subscription_event_type_t eventType, uint32_t idx, void *userdata);
*/
import "C"
import (
	"errors"
	"runtime/cgo"
	"unsafe"
)

type Mainloop struct {
	mainloop *C.pa_theaded_mainloop
}

type Context struct {
	mainloop *C.pa_threaded_mainloop
	context *C.pa_context
	state   chan ContextState
}

func NewMainloop() (*Mainloop, error) {
	// Create mainloop.
	mainloop := C.pa_threaded_mainloop_new()
	if mainloop == nil {
		return nil, errors.New("error while creating mainloop")
	}

	return &Mainloop{mainloop}, nil
}

func (m *Mainloop) Run() {
	C.pa_threaded_mainloop_start(m.mainloop)
}

func (m *Mainloop) Stop() {
	C.pa_mainloop_quit(m.mainloop, C.int(retval))
}

func (m *Mainloop) NewContext(name string) (*Context, error) {
	m.pause <- struct{}{}

	// Create context from mainloop api.
	context := C.pa_context_new(m.api, C.CString(name))
	if context == nil {
		return nil, errors.New("error while creating context")
	}

	// Create return value.
	ret := &Context{
		context: context,
		state:   make(chan ContextState),
	}

	// Setup callbacks for context.
	h := C.uintptr_t(cgo.NewHandle(ret))
	C.pa_context_set_state_callback(context, C.pa_context_notify_cb_t(C.stateCallback), unsafe.Pointer(&h))
	C.pa_context_set_subscribe_callback(context, C.pa_context_subscribe_cb_t(C.subscribeCallback), unsafe.Pointer(&h))

	m.play <- struct{}{}

	return ret, nil
}

func (c *Context) Connect() error {
	res := C.pa_context_connect(c.context, nil, C.PA_CONTEXT_NOFLAGS, nil)
	if res < 0 {
		return errors.New("error connecting context")
	}
	return nil
}

func (c *Context) Disconnect() {
	C.pa_context_disconnect(c.context)
}

func (c *Context) State() ContextState {
	return createContextState(C.pa_context_get_state(c.context))
}

func (c *Context) StateChan() <-chan ContextState {
	return c.state
}

func (c *Context) Close() {
	C.pa_context_unref(c.context)
}

//export stateCallback
func stateCallback(context *C.pa_context, userdata unsafe.Pointer) {

	// Get reference to pulse instance.
	handle := *(*C.uint32_t)(userdata)
	h := cgo.Handle(handle)
	c := h.Value().(*Context)

	// Obtain context state and convert to go equivalent type.
	state := createContextState(C.pa_context_get_state(c.context))

	// Send state if someone is wainting otherwise discard.
	select {
	case c.state <- state:
	default:
	}
}

//export subscribeCallback
func subscribeCallback(c *C.pa_context, t C.pa_subscription_event_type_t, inx C.uint32_t, userdata unsafe.Pointer) {
}
