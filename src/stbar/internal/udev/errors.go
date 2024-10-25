package udev

import "errors"

var ErrNewMonitor = errors.New("udev_monitor_new_from_netlink() returned nil")
