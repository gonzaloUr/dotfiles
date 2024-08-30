package pulse

// #include <pulse/pulseaudio.h>
import "C"
import "unsafe"

type SubscriptionType int

const (
	SubscriptionTypeSink SubscriptionType = iota
	SubscriptionTypeSource
	SubscriptionTypeSinkInput
	SubscriptionTypeSourceOutput
	SubscriptionTypeModule
	SubscriptionTypeClient
	SubscriptionTypeSampleCache
	SubscriptionTypeServer
	SubscriptionTypeCard
)

func createSubscriptionType(t C.pa_subscription_event_type_t) SubscriptionType {
	if t&C.PA_SUBSCRIPTION_EVENT_TYPE_MASK == C.PA_SUBSCRIPTION_EVENT_SINK {
		return SubscriptionTypeSink
	}

	if t&C.PA_SUBSCRIPTION_EVENT_TYPE_MASK == C.PA_SUBSCRIPTION_EVENT_SOURCE {
		return SubscriptionTypeSource
	}

	if t&C.PA_SUBSCRIPTION_EVENT_TYPE_MASK == C.PA_SUBSCRIPTION_EVENT_SINK_INPUT {
		return SubscriptionTypeSinkInput
	}

	if t&C.PA_SUBSCRIPTION_EVENT_TYPE_MASK == C.PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT {
		return SubscriptionTypeSourceOutput
	}

	if t&C.PA_SUBSCRIPTION_EVENT_TYPE_MASK == C.PA_SUBSCRIPTION_EVENT_MODULE {
		return SubscriptionTypeModule
	}

	if t&C.PA_SUBSCRIPTION_EVENT_TYPE_MASK == C.PA_SUBSCRIPTION_EVENT_CLIENT {
		return SubscriptionTypeClient
	}

	if t&C.PA_SUBSCRIPTION_EVENT_TYPE_MASK == C.PA_SUBSCRIPTION_EVENT_SAMPLE_CACHE {
		return SubscriptionTypeSampleCache
	}

	if t&C.PA_SUBSCRIPTION_EVENT_TYPE_MASK == C.PA_SUBSCRIPTION_EVENT_SERVER {
		return SubscriptionTypeServer
	}

	if t&C.PA_SUBSCRIPTION_EVENT_TYPE_MASK == C.PA_SUBSCRIPTION_EVENT_CARD {
		return SubscriptionTypeCard
	}

	panic("unknown event type")
}

type SubscriptionFacility int

const (
	SubscriptionFacilityNew = iota
	SubscriptionFacilityChange
	SubscriptionFacilityRemove
)

func createSubscriptionFacility(t C.pa_subscription_event_type_t) SubscriptionFacility {
	if t&C.PA_SUBSCRIPTION_EVENT_FACILITY_MASK == C.PA_SUBSCRIPTION_EVENT_NEW {
		return SubscriptionFacilityNew
	}

	if t&C.PA_SUBSCRIPTION_EVENT_FACILITY_MASK == C.PA_SUBSCRIPTION_EVENT_CHANGE {
		return SubscriptionFacilityChange
	}

	if t&C.PA_SUBSCRIPTION_EVENT_FACILITY_MASK == C.PA_SUBSCRIPTION_EVENT_REMOVE {
		return SubscriptionFacilityRemove
	}

	panic("unknown event facility")
}

type SubscriptionEvent struct {
	Type     SubscriptionType
	Facility SubscriptionFacility
}

func createSubscriptionEvent(t C.pa_subscription_event_type_t) SubscriptionEvent {
	return SubscriptionEvent{
		Type:     createSubscriptionType(t),
		Facility: createSubscriptionFacility(t),
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
