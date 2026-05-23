package main

import (
	"context"
	"flag"
	"fmt"
	"os"
	"os/signal"
	"strings"
	"sync"
	"syscall"
	"time"
)

// pollValue implements flag.Value representing a flag for a file to poll.
type pollValue struct {
	Duration time.Duration
	Path     string
}

func (p *pollValue) String() string {
	return fmt.Sprintf("%v:%v", p.Duration, p.Path)
}

func (p *pollValue) Set(s string) error {
	parts := strings.SplitN(s, ":", 2)
	if len(parts) != 2 {
		return fmt.Errorf("format must be duration:path")
	}

	path := parts[1]
	if path == "" {
		return fmt.Errorf("path is empty")
	}

	d, err := time.ParseDuration(parts[0])
	if err != nil {
		return fmt.Errorf("invalid duration format")
	}

	p.Path = path
	p.Duration = d

	return nil
}

// pollListValue implements flag.Value representing a flag of a list of pollValue.
type pollListValue []*pollValue

func (p *pollListValue) String() string {
	var parts []string

	for _, v := range *p {
		parts = append(parts, v.String())
	}

	return strings.Join(parts, " ")
}

func (p *pollListValue) Set(s string) error {
	var pollValue pollValue

	err := pollValue.Set(s)
	if err != nil {
		return err
	}

	*p = append(*p, &pollValue)
	return nil
}

func main() {
	// Flag that represents a list of files to polls.
	var polls pollListValue

	// Configure previous flag.
	flag.Var(&polls, "poll", "interval:path")
	flag.Parse()

	// Check that each pollValue path points to an existing file.
	for _, v := range polls {
		info, err := os.Stat(v.Path)

		// Error getting file info.
		if err != nil {
			fmt.Fprintln(os.Stderr, err)
			os.Exit(1)
		}

		// File is a directory.
		if info.IsDir() {
			fmt.Fprintf(os.Stderr, "path %s is a directory\n", v.Path)
			os.Exit(1)
		}
	}

	// Start polling every file.
	var wg sync.WaitGroup
	ctx, cancel := context.WithCancel(context.Background())

	for _, v := range polls {
		wg.Add(1)
		go pollFile(v.Path, v.Duration, ctx, &wg)
	}

	// Wait for program shutdown.
	sig := make(chan os.Signal, 1)
	signal.Notify(sig, syscall.SIGINT, syscall.SIGTERM)
	<-sig
	signal.Stop(sig)

	// Stop polling goroutines and exit.
	cancel()
	wg.Wait()
}

func pollFile(file string, duration time.Duration, ctx context.Context, wg *sync.WaitGroup) {
	defer wg.Done()

	ticker := time.NewTicker(duration)
	defer ticker.Stop()

	for {
		select {
		case <-ticker.C:
			data, err := os.ReadFile(file)
			if err == nil {
				fmt.Printf("%s: %s", file, string(data))
			}
		case <-ctx.Done():
			return
		}
	}
}
