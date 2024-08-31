package main

import (
	"fmt"
	"github.com/gonzaloUr/stbar/internal/pulse"
)

func main() {
	p, err := pulse.NewPulse("stbar")
	if err != nil {
		panic(err)
	}

	p.Loop()
	p.Ready()

	if err = p.Subscribe(); err != nil {
		panic(err)
	}

	cancel := make(chan struct{})

	go func(ch <-chan pulse.SubscriptionEvent) {
		for e := range ch {
			fmt.Println(e)

			if e.Facility == pulse.EventFacilitySink && e.Type == pulse.EventTypeChange {
				for _, v := range p.SinkInfo() {
					fmt.Printf("%v\n", v)
				}
			}
		}
	}(p.Event(cancel))

	p.Done()
}
