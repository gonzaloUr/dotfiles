package pulse

// #include <pulse/pulseaudio.h>
import "C"
import "unsafe"

type EventFacility int

const (
	EventFacilitySink EventFacility = iota
	EventFacilitySource
	EventFacilitySinkInput
	EventFacilitySourceOutput
	EventFacilityModule
	EventFacilityClient
	EventFacilitySampleCache
	EventFacilityServer
	EventFacilityCard
)

func createEventFacility(t C.pa_subscription_event_type_t) EventFacility {
	if t&C.PA_SUBSCRIPTION_EVENT_FACILITY_MASK == C.PA_SUBSCRIPTION_EVENT_SINK {
		return EventFacilitySink
	}

	if t&C.PA_SUBSCRIPTION_EVENT_FACILITY_MASK == C.PA_SUBSCRIPTION_EVENT_SOURCE {
		return EventFacilitySource
	}

	if t&C.PA_SUBSCRIPTION_EVENT_FACILITY_MASK == C.PA_SUBSCRIPTION_EVENT_SINK_INPUT {
		return EventFacilitySinkInput
	}

	if t&C.PA_SUBSCRIPTION_EVENT_FACILITY_MASK == C.PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT {
		return EventFacilitySourceOutput
	}

	if t&C.PA_SUBSCRIPTION_EVENT_FACILITY_MASK == C.PA_SUBSCRIPTION_EVENT_MODULE {
		return EventFacilityModule
	}

	if t&C.PA_SUBSCRIPTION_EVENT_FACILITY_MASK == C.PA_SUBSCRIPTION_EVENT_CLIENT {
		return EventFacilityClient
	}

	if t&C.PA_SUBSCRIPTION_EVENT_FACILITY_MASK == C.PA_SUBSCRIPTION_EVENT_SAMPLE_CACHE {
		return EventFacilitySampleCache
	}

	if t&C.PA_SUBSCRIPTION_EVENT_FACILITY_MASK == C.PA_SUBSCRIPTION_EVENT_SERVER {
		return EventFacilityServer
	}

	if t&C.PA_SUBSCRIPTION_EVENT_FACILITY_MASK == C.PA_SUBSCRIPTION_EVENT_CARD {
		return EventFacilityCard
	}

	panic("unknown facility")
}

type EventType int

const (
	EventTypeNew = iota
	EventTypeChange
	EventTypeRemove
)

func createEventType(t C.pa_subscription_event_type_t) EventType {
	if t&C.PA_SUBSCRIPTION_EVENT_TYPE_MASK == C.PA_SUBSCRIPTION_EVENT_NEW {
		return EventTypeNew
	}

	if t&C.PA_SUBSCRIPTION_EVENT_TYPE_MASK == C.PA_SUBSCRIPTION_EVENT_CHANGE {
		return EventTypeChange
	}

	if t&C.PA_SUBSCRIPTION_EVENT_TYPE_MASK == C.PA_SUBSCRIPTION_EVENT_REMOVE {
		return EventTypeRemove
	}

	panic("unknown type")
}

type SubscriptionEvent struct {
	Type     EventType
	Facility EventFacility
}

func createSubscriptionEvent(t C.pa_subscription_event_type_t) SubscriptionEvent {
	return SubscriptionEvent{
		Type:     createEventType(t),
		Facility: createEventFacility(t),
	}
}

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
		Name:   C.GoString(i.name),
		Index:  int(i.index),
		Desc:   C.GoString(i.description),
		Mute:   int(i.mute) != 0,
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
