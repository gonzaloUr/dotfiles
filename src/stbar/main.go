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

	ready := make(chan struct{})

	go func() {
		for state := range p.ListenStates() {
			fmt.Println(state)
		}
	}()

	if err := p.Connect(); err != nil {
		fmt.Println("error")
	}
	
	fmt.Println("connected")

	<-ready
}
