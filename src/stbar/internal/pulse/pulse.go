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
	mainloop *C.pa_mainloop
}

type Api struct {
	api               *C.pa_mainloop_api
	stateChannels     map[chan C.pa_context_state_t]struct{}
}

type Context struct {
	context *C.pa_context
}

func NewMainloop() (*Mainloop, error) {
	mainloop := C.pa_mainloop_new()
	if mainloop == nil {
		return nil, errors.New("error while creating mainloop")
	}
	return &Mainloop{mainloop}, nil
}

func (m *Mainloop) NewApi() *Api {
	return &Api{
		C.pa_mainloop_get_api(m.mainloop),
		map[chan C.pa_context_state_t]struct{}{},
	}
}

func (a *Api) NewContext(name string) (*Context, error) {
	// Create context from mainloop api.
	context := C.pa_context_new(a.api, C.CString(name))
	if context == nil {
		return nil, errors.New("error while creating context")
	}

	// Setup callbacks for context.
	h := C.uintptr_t(cgo.NewHandle(context))
	C.pa_context_set_state_callback(context, C.pa_context_notify_cb_t(C.stateCallback), unsafe.Pointer(&h))
	C.pa_context_set_subscribe_callback(context, C.pa_context_subscribe_cb_t(C.subscribeCallback), unsafe.Pointer(&h))

	return &Context{context}, nil
}

func (c *Context) Connect() error {
	res := C.pa_context_connect(c.context, nil, C.PA_CONTEXT_NOFLAGS, nil)
	if res < 0 {
		return errors.New("error connecting context")
	}

	return nil
}

func (m *Mainloop) Run() {
	var retval C.int
	C.pa_mainloop_run(m.mainloop, &retval)
	C.pa_mainloop_free(m.mainloop)
}

//export stateCallback
func stateCallback(context *C.pa_context, userdata unsafe.Pointer) {
}

//export subscribeCallback
func subscribeCallback(c *C.pa_context, t C.pa_subscription_event_type_t, inx C.uint32_t, userdata unsafe.Pointer) {
}
