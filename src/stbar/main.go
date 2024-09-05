package main

import (
	"context"
	"fmt"
	"time"

	"github.com/gonzaloUr/stbar/internal/pulse"
)

func main() {
	p, err := pulse.NewPulse("stbar")
	if err != nil {
		panic(err)
	}

	ctx, cancel := context.WithTimeout(context.Background(), time.Second*10)
	defer cancel()

	ch := p.ListenStates(ctx)

	go func() {
		for state := range ch {
			fmt.Printf("State is: %v\n", state)
		}
	}()

	fmt.Println("connecting...")

	if err := p.Connect(); err != nil {
		fmt.Println("error")
	}

	fmt.Println("connected...")

	<-p.Done()
}
