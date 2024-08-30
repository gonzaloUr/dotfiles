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

	for _, v := range p.SinkInfo() {
		fmt.Printf("%v\n", v)
	}

	p.Quit()
	p.Done()
}
