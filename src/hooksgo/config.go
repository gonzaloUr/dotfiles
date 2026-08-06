package main

import "github.com/gonzaloUr/dotfiles/src/hooksgo/hooks/pulseaudio"

type Hook interface {
	Output() <-chan string
}

type Arg struct {
	new func() Hook
}

var args = []Arg{
	{ PulseaudioHook },
}

func PulseaudioHook() Hook {
	ret, err := pulseaudio.New("stbar")
	if err != nil {
		panic(err)
	}

	return ret
}
