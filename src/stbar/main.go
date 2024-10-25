package main

import (
	"context"
	"fmt"
	"os"

	"github.com/gonzaloUr/dotfiles/src/stbar/internal/pulse"
	"github.com/gonzaloUr/dotfiles/src/stbar/internal/udev"
	"golang.org/x/sys/unix"
)

func pulseExample() {
	p, err := pulse.NewPulse("stbar")
	if err != nil {
		panic(err)
	}

	go func() {
		for state := range p.ListenStates(context.Background()) {
			if state == pulse.ContextReady {
				p.Subscribe()
			}
		}
	}()

	if err := p.Connect(); err != nil {
		panic(err)
	}

	p.Run()

	for event := range p.ListenEvents(context.Background()) {
		fmt.Println(event)
		sinkInfo := p.SinkInfo()
		fmt.Println(sinkInfo)
	}

	<-p.Done()
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

	mon.FilterAddMatchSubsystemDevtype("net", "wlan")
	mon.FilterAddMatchSubsystemDevtype("backlight", "intel_backlight")
	mon.FilterAddMatchSubsystemDevtype("block", "sdb")
	mon.EnableReceiving()

	fd := mon.Fd()
	if err := unix.SetNonblock(fd, true); err != nil {
		panic(err)
	}

	epfd, err := unix.EpollCreate1(0)
	if err != nil {
		panic(err)
	}

	var event unix.EpollEvent
	event.Events = unix.EPOLLIN | unix.EPOLLET
	event.Fd = int32(fd)
	if err := unix.EpollCtl(epfd, unix.EPOLL_CTL_ADD, fd, &event); err != nil {
		panic(err)
	}

	var events [32]unix.EpollEvent

	for {
		fmt.Println("waiting...")
		n, err := unix.EpollWait(epfd, events[:], 100)

		if err != nil {
			fmt.Fprintf(os.Stderr, "error: %v\n", err)
			continue
		}

		for i := range n {
			if int(events[i].Fd) == fd {
				fmt.Println("evento")
			}
		}
	}
}

func main() {
	udevExample3()
}
