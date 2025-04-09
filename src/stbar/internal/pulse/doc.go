// Asynchronous PulseAudio API.
//
// Pulseaudio async is based around a mainloop, similar to what happens in opengl applications, where the mainloop implementation
// runs and the clients can perform operations on it. In order to execute operations each mainloop exposes an mainloop api object on
// which clients can use to interact with the mainloop.
//
// Each mainloop api object can create many contexts. A context represents a connection to the pulseaudio server associated to the mainloop api
// that the client can use to perform operations, subscribe to events, etc. For some operations or events a context is not necessary and the
// mainloop api object can be used instead.
//
// Operations are done asynchronously and expect a result. In order for the caller to know the state of the operation an pa_operation object is
// returned with a reference counter equal to 1. After the caller is done with the operation in question this counter must be decremented to zero.
// Operations also require a callback to be passed at the moment of starting the execution so the caller can be notified when the operation ends.
//
// Events occur for different reasons within the life cycle of a mainloop. These inform changes happening withing the mainloop. The client can
// subscribe to events provided a callback that gets called whenever said event happens.
//
// Almost all objects in pulseaudio have a reference counter. When an object is created, through t pulseaudio functions, it has a reference
// count of one. Decreasing this to zero automatically performs a free on the object, because it is associated to some pulseaudio structure
// like context, api, etc.
package pulse
