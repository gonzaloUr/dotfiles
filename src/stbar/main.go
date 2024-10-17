package main

import (
	"context"
	"fmt"

	"github.com/gonzaloUr/dotfiles/src/stbar/internal/pulse"
	"github.com/gonzaloUr/dotfiles/src/stbar/internal/udev"
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

func main() {
	udevExample2()
}
