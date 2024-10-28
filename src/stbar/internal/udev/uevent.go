package udev

import "strings"

type Uevent struct {
	Device Device
	Uevent map[string]string
}

func (d Device) NewUevent() Uevent {
	attr := d.GetAttr("uevent")
	lines := strings.Split(attr, "\n")
	props := map[string]string{}

	for _, line := range lines {
		prop, value, ok := strings.Cut(line, "=")
		if !ok {
			continue
		}
		props[prop] = value
	}

	return Uevent{d, props}
}

func (u Uevent) IsBattery() bool {
	if u.Device.GetSubsystem() != "power_supply" {
		return false
	}

	if u.Device.GetDevtype() != "power_supply" {
		return false
	}

	if u.Uevent["POWER_SUPPLY_TYPE"] != "Battery" {
		return false
	}

	return true
}

func (u Uevent) IsAC() bool {
	if u.Device.GetSubsystem() != "power_supply" {
		return false
	}

	if u.Device.GetDevtype() != "power_supply" {
		return false
	}

	if u.Uevent["POWER_SUPPLY_TYPE"] != "Mains" {
		return false
	}

	return true
}

func (u Uevent) IsBacklight() bool {
	return u.Device.GetSubsystem() != "backlight"
}
