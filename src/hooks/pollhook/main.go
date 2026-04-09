package main

import (
	"context"
	"flag"
	"fmt"
	"os"
	"os/signal"
	"strings"
	"syscall"
	"time"
)

// Poll represents a file to poll.
type Poll struct {
	Duration time.Duration
	Path     string
}

func (p *Poll) String() string {
	return fmt.Sprintf("%v %v", p.Duration, p.Path)
}

// PollList is a a list of Polls that can be used with flag package.
type PollList []*Poll

func (p *PollList) String() string {
	var parts []string

	for _, v := range *p {
		parts = append(parts, v.String())
	}

	return strings.Join(parts, ", ")
}

func (p *PollList) Set(s string) error {
	parts := strings.SplitN(s, ":", 2)
	if len(parts) != 2 {
		return fmt.Errorf("format must be duration:path")
	}
	if parts[1] == "" {
		return fmt.Errorf("path is empty")
	}

	d, err := time.ParseDuration(parts[0])
	if err != nil {
		return err
	}

	*p = append(*p, &Poll{d, parts[1]})
	return nil
}

// PollRes represents the result of reading or polling a file.
type PollRes struct {
	Path string
	Data string
}

func main() {
	var polls PollList

	flag.Var(&polls, "poll", "interval:path")
	flag.Parse()

	for _, v := range polls {
		info, err := os.Stat(v.Path)

		if err != nil {
			fmt.Fprintln(os.Stderr, err)
			os.Exit(1)
		}

		if info.IsDir() {
			fmt.Fprintf(os.Stderr, "path %s is a directory\n", v.Path)
			os.Exit(1)
		}
	}

	ctx, cancel := context.WithCancel(context.Background())

	for _, v := range polls {
		go pollFile(v.Path, v.Duration, ctx)
	}

	sig := make(chan os.Signal, 1)
	signal.Notify(sig, syscall.SIGINT, syscall.SIGTERM)

	<-sig
	cancel()
}

func pollFile(file string, duration time.Duration, ctx context.Context) {
	ticks := time.Tick(duration)

	for {
		select {
		case <-ticks:
			data, err := os.ReadFile(file)
			if err == nil {
				fmt.Printf("%s: %s", file, string(data))
			}
		case <-ctx.Done():
			return
		}
	}
}
