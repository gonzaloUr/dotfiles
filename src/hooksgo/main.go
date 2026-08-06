package main

import (
	"fmt"
	"github.com/gonzaloUr/dotfiles/src/hooksgo/hooks/pulseaudio"
)

func main() {
	pa, err := pulseaudio.New("hooks")
	if err != nil {
		panic(err)
	}

	for st := range pa.GetStateChan() {
		// Switch over the state of a connection context.
		switch st {

		// The context hasn't been connected yet.
		case pulseaudio.StateUnconnected:
			fmt.Println("UNCONNECTED")

		// A connection is being established.
		case pulseaudio.StateConnecting:
			fmt.Println("CONNECTING")

		// The client is authorizing itself.
		case pulseaudio.StateAuthorizing:
			fmt.Println("AUTHORIZING")

		// The client is passing its application name.
		case pulseaudio.StateSettingName:
			fmt.Println("SETTING_NAME")

		// Connection established.
		case pulseaudio.StateReady:
			fmt.Println("READY")

		// Connection failed.
		case pulseaudio.StateFailed:
			fmt.Println("FAILED")

		// Connection terminated.
		case pulseaudio.StateTerminated:
			fmt.Println("TERMINATED")
		}
	}
}
