package pulse

import "errors"

var (
	ErrMainloopNew      = errors.New("pa_mainloop_new() returned nil")
	ErrContextNew       = errors.New("pa_context_new() returned nil")
	ErrContextConnect   = errors.New("pa_context_connect() failed")
	ErrContextSubscribe = errors.New("pa_context_subscribe() failed")
)
