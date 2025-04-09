package pulse

/*
#include <pulse/pulseaudio.h>
*/
import "C"
import "errors"

type Pulse struct {
	mainloop *C.pa_mainloop
}

type Api struct {
	api *C.pa_mainloop_api
}

type Context struct {
	context *C.pa_context
}

func NewPulse() (*Pulse, error) {
	mainloop := C.pa_mainloop_new()
	if mainloop == nil {
		return nil, errors.New("error while creating mainloop")
	}
	return &Pulse{mainloop}, nil
}

func (p *Pulse) NewApi() *Api {
	return &Api{C.pa_mainloop_get_api(p.mainloop)}
}

func (a *Api) NewContext(name string) (*Context, error) {
	context := C.pa_context_new(a.api, C.CString(name))
	if context == nil {
		return nil, errors.New("error while creating context")
	}
	return context, nil
}
