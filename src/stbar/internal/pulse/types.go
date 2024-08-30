package pulse

// #include <pulse/pulseaudio.h>
import "C"
import "unsafe"

type Proplist map[string]string

func createProplist(proplist *C.pa_proplist) Proplist {
	p := Proplist{}
	var state unsafe.Pointer

	for {
		key := C.pa_proplist_iterate(proplist, &state)

		if key == nil {
			break
		}

		value := C.pa_proplist_gets(proplist, key)

		p[C.GoString(key)] = C.GoString(value)
	}

	return p
}

type ClientInfo struct {
	Index  int
	Name   string
	Owner  int
	Driver string
	Props  Proplist
}

func createClientInfo(i *C.pa_client_info) ClientInfo {
	return ClientInfo{
		Index:  int(i.index),
		Name:   C.GoString(i.name),
		Owner:  int(i.owner_module),
		Driver: C.GoString(i.driver),
		Props:  createProplist(i.proplist),
	}
}

type SinkInfo struct {
	Name   string
	Index  int
	Desc   string
	Mute   bool
	Volume Volume
}

func createSinkInfo(i *C.pa_sink_info) SinkInfo {
	return SinkInfo{
		Name: C.GoString(i.name),
		Index: int(i.index),
		Desc: C.GoString(i.description),
		Mute: int(i.mute) != 0,
		Volume: createVolume(i.volume),
	}
}

type Volume struct {
	Channels int
	Values   []int
}

func createVolume(v C.pa_cvolume) Volume {
	channels := int(v.channels)
	values := make([]int, 0, channels)

	for i := range channels {
		values = append(values, int(v.values[i]))
	}

	return Volume{channels, values}
}
