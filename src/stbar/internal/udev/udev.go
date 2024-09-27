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

type Monitor struct {
	monitor *C.struct_udev_monitor
}

func (c Context) NewMonitorFromNetlink(name string) Monitor {
	return Monitor{C.udev_monitor_new_from_netlink(c.udev, C.CString(name))}
}

func (m Monitor) Ref() Monitor {
	return Monitor{C.udev_monitor_ref(m.monitor)}
}

func (m Monitor) Unref() {
	C.udev_monitor_unref(m.monitor)
}

func (m Monitor) FilterSubsystemDevtype(subsystem, devtype string) int {
	return int(C.udev_monitor_filter_add_match_subsystem_devtype(m.monitor, C.CString(subsystem), C.CString(devtype)))
}

func (m Monitor) FilterMatchTag(tag string) int {
	return int(C.udev_monitor_filter_add_match_tag(m.monitor, C.CString(tag)))
}

func (m Monitor) EnableReceiving() {
	C.udev_monitor_enable_receiving(m.monitor);
}
