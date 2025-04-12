package main

import (
	"fmt"
	"log"
	"syscall"

	"github.com/gonzaloUr/dotfiles/src/stbar/internal/pulse"
	"github.com/gonzaloUr/dotfiles/src/stbar/internal/udev"
	"golang.org/x/sys/unix"
)

func pulseExample() {
	mainloop, err := pulse.NewMainloop()
	if err != nil {
		log.Fatal(err)
	}
	mainloop.Run()

	context, err := mainloop.NewContext("test")
	if err != nil {
		log.Fatal(err)
	}
	context.Connect()

	for x := range context.StateChan() {
		fmt.Println(x)

		switch x {
		case pulse.ContextReady:
			mainloop.Quit(0)
		case pulse.ContextTerminated:
		case pulse.ContextFailed:
			break
		}
	}

	/*
	context.Disconnect()
	context.Close()
	*/

	mainloop.Done()
}

func udevExample1() {
	ctx := udev.NewContext()
	defer ctx.Unref()

	backlight := ctx.NewDeviceFromSubsystemSysname("backlight", "intel_backlight")
	defer backlight.Unref()

	attrs := backlight.Attrs()

	for k := range attrs.All() {
		v := backlight.GetAttr(k)
		fmt.Printf("k = %v, v = %v\n", k, v)
	}
}

func udevExample2() {
	ctx := udev.NewContext()
	defer ctx.Unref()

	enum := ctx.NewEnumerate()
	defer ctx.Unref()

	enum.AddMatchSubsystem("net")
	enum.ScanDevices()
	list := enum.Enumerate()

	for path := range list.All() {
		dev := ctx.NewDeviceFromSyspath(path)
		defer dev.Unref()

		for k := range dev.Attrs().All() {
			attr := dev.GetAttr(k)
			fmt.Println(k, attr)
		}
	}
}

func udevExample3() {
	ctx := udev.NewContext()
	defer ctx.Unref()

	mon, err := ctx.NewMonitorFromNetlink("udev")
	if err != nil {
		panic(err)
	}
	defer mon.Unref()

	mon.FilterAddMatchSubsystem("backlight")
	mon.FilterAddMatchSubsystem("power_supply")
	mon.FilterAddMatchSubsystem("block")
	mon.FilterAddMatchSubsystem("bluetooth")
	mon.FilterAddMatchSubsystem("graphics")
	mon.FilterAddMatchSubsystem("input")
	mon.FilterAddMatchSubsystem("mem")
	mon.FilterAddMatchSubsystem("net")
	mon.EnableReceiving()

	fd := mon.Fd()
	if err := unix.SetNonblock(fd, true); err != nil {
		panic(err)
	}

	var stat syscall.Stat_t
	if err := syscall.Fstat(fd, &stat); err != nil {
		panic(err)
	}

	epfd, err := unix.EpollCreate1(0)
	if err != nil {
		panic(err)
	}
	defer unix.Close(epfd)

	var event unix.EpollEvent
	event.Events = unix.EPOLLIN | unix.EPOLLET
	event.Fd = int32(fd)
	if err := unix.EpollCtl(epfd, unix.EPOLL_CTL_ADD, fd, &event); err != nil {
		panic(err)
	}

	var events [32]unix.EpollEvent

	for {
		n, err := unix.EpollWait(epfd, events[:], 1000)
		errno, isErrno := err.(syscall.Errno)

		if (err != nil && !isErrno) || (isErrno && errno != syscall.EINTR) {
			break
		}

		for i := range n {
			if int(events[i].Fd) == fd {
				if events[i].Events&unix.EPOLLIN != 0 {
					for _, dev := range mon.ReciveDevices() {
						fmt.Printf("devpath = %s\n", dev.GetDevpath())
						fmt.Printf("subsystem = %s\n", dev.GetSubsystem())

						devtype := dev.GetDevtype()
						fmt.Printf("devtype = %s\n", devtype)
						fmt.Printf("devtype len = %d\n", len(devtype))

						for k := range dev.Attrs().All() {
							fmt.Printf("k = %s, v = %s\n", k, dev.GetAttr(k))
						}
						fmt.Println()
					}
				}
			}
		}
	}
}

func main() {
	pulseExample()
}
