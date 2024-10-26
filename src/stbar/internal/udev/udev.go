package udev

// #include <libudev.h>
import "C"
import "iter"

type Context struct {
	udev *C.struct_udev
}

func NewContext() Context {
	return Context{C.udev_new()}
}

func (c Context) Ref() Context {
	return Context{C.udev_ref(c.udev)}
}

func (c Context) Unref() {
	C.udev_unref(c.udev)
}

type ListEntry struct {
	entry *C.struct_udev_list_entry
}

func (l ListEntry) Nil() bool {
	return l.entry == nil
}

func (l ListEntry) Name() string {
	return C.GoString(C.udev_list_entry_get_name(l.entry))
}

func (l ListEntry) Value() string {
	return C.GoString(C.udev_list_entry_get_value(l.entry))
}

func (l ListEntry) GetNext() ListEntry {
	return ListEntry{C.udev_list_entry_get_next(l.entry)}
}

func (l ListEntry) GetByName(name string) ListEntry {
	return ListEntry{C.udev_list_entry_get_by_name(l.entry, C.CString(name))}
}

func (l ListEntry) All() iter.Seq2[string, string] {
	return func(yield func(k, v string) bool) {
		for curr := l; !curr.Nil(); curr = curr.GetNext() {
			if !yield(curr.Name(), curr.Value()) {
				break
			}
		}
	}
}

func (l ListEntry) Entries() iter.Seq[ListEntry] {
	return func(yield func(v ListEntry) bool) {
		for curr := l; !curr.Nil(); curr = curr.GetNext() {
			if !yield(curr) {
				break
			}
		}
	}
}

type Device struct {
	device *C.struct_udev_device
}

func (c Context) NewDeviceFromSyspath(syspath string) Device {
	return Device{C.udev_device_new_from_syspath(c.udev, C.CString(syspath))}
}

func (c Context) NewDeviceFromSubsystemSysname(subsystem, sysname string) Device {
	return Device{C.udev_device_new_from_subsystem_sysname(c.udev, C.CString(subsystem), C.CString(sysname))}
}

func (d Device) Ref() Device {
	return Device{C.udev_device_ref(d.device)}
}

func (d Device) Unref() {
	C.udev_device_unref(d.device)
}

func (d Device) Attrs() ListEntry {
	return ListEntry{C.udev_device_get_sysattr_list_entry(d.device)}
}

func (d Device) GetAttr(sysattr string) string {
	return C.GoString(C.udev_device_get_sysattr_value(d.device, C.CString(sysattr)))
}

func (d Device) GetAction() string {
	return C.GoString(C.udev_device_get_action(d.device))
}

func (d Device) GetDevpath() string {
	return C.GoString(C.udev_device_get_devpath(d.device))
}

func (d Device) GetSubsystem() string {
	return C.GoString(C.udev_device_get_subsystem(d.device))
}

func (d Device) GetDevtype() string {
	return C.GoString(C.udev_device_get_devtype(d.device))
}

type Monitor struct {
	monitor *C.struct_udev_monitor
}

func (c Context) NewMonitorFromNetlink(name string) (Monitor, error) {
	mon := C.udev_monitor_new_from_netlink(c.udev, C.CString(name))

	if mon == nil {
		return Monitor{}, ErrNewMonitor
	}

	return Monitor{mon}, nil
}

func (m Monitor) Ref() Monitor {
	return Monitor{C.udev_monitor_ref(m.monitor)}
}

func (m Monitor) Unref() {
	C.udev_monitor_unref(m.monitor)
}

func (m Monitor) EnableReceiving() {
	C.udev_monitor_enable_receiving(m.monitor)
}

func (m Monitor) Fd() int {
	return int(C.udev_monitor_get_fd(m.monitor))
}

func (m Monitor) ReciveDevices() []Device {
	var ret []Device

	for {
		dev := C.udev_monitor_receive_device(m.monitor)
		if dev == nil {
			break
		}
		ret = append(ret, Device{dev})
	}

	return ret
}

func (m Monitor) FilterAddMatchSubsystemDevtype(subsystem, devtype string) int {
	return int(C.udev_monitor_filter_add_match_subsystem_devtype(m.monitor, C.CString(subsystem), C.CString(devtype)))
}

// TODO: chequear
func (m Monitor) FilterAddMatchSubsystem(subsystem string) int {
	return int(C.udev_monitor_filter_add_match_subsystem_devtype(m.monitor, C.CString(subsystem), nil))
}

func (m Monitor) FilterAddMatchTag(tag string) int {
	return int(C.udev_monitor_filter_add_match_tag(m.monitor, C.CString(tag)))
}

func (m Monitor) FilterUpdate() int {
	return int(C.udev_monitor_filter_update(m.monitor))
}

func (m Monitor) FilterRemove() int {
	return int(C.udev_monitor_filter_remove(m.monitor))
}

type Enumerate struct {
	enumerate *C.struct_udev_enumerate
}

func (c Context) NewEnumerate() Enumerate {
	return Enumerate{C.udev_enumerate_new(c.udev)}
}

func (e Enumerate) Ref() Enumerate {
	return Enumerate{C.udev_enumerate_ref(e.enumerate)}
}

func (e Enumerate) Unref() {
	C.udev_enumerate_unref(e.enumerate)
}

func (e Enumerate) AddMatchSubsystem(subsytem string) {
	C.udev_enumerate_add_match_subsystem(e.enumerate, C.CString(subsytem))
}

func (e Enumerate) AddNoMatchSubsystem(subsystem string) {
	C.udev_enumerate_add_nomatch_subsystem(e.enumerate, C.CString(subsystem));
}

func (e Enumerate) AddMatchSysattr(sysattr string, value string) {
	C.udev_enumerate_add_match_sysattr(e.enumerate, C.CString(sysattr), C.CString(value))
}

func (e Enumerate) AddNoMatchSysattr(sysattr string, value string) {
	C.udev_enumerate_add_nomatch_sysattr(e.enumerate, C.CString(sysattr), C.CString(value))
}

func (e Enumerate) AddMatchProperty(property string, value string) {
	C.udev_enumerate_add_match_property(e.enumerate, C.CString(property), C.CString(value))
}

func (e Enumerate) AddMatchSysname(sysname string) {
	C.udev_enumerate_add_match_sysname(e.enumerate, C.CString(sysname))
}

func (e Enumerate) AddMatchTag(tag string) {
	C.udev_enumerate_add_match_tag(e.enumerate, C.CString(tag))
}

func (e Enumerate) AddMatchParent(device Device) {
	C.udev_enumerate_add_match_parent(e.enumerate, device.device)
}

func (e Enumerate) AddMatchIsInit() {
	C.udev_enumerate_add_match_is_initialized(e.enumerate)
}

func (e Enumerate) AddSyspath(syspath string) {
	C.udev_enumerate_add_syspath(e.enumerate, C.CString(syspath))
}

func (e Enumerate) ScanDevices() {
	C.udev_enumerate_scan_devices(e.enumerate)
}

func (e Enumerate) ScanSubsystems() {
	C.udev_enumerate_scan_subsystems(e.enumerate)
}

func (e Enumerate) Enumerate() ListEntry {
	return ListEntry{C.udev_enumerate_get_list_entry(e.enumerate)}
}
