package main

import (
	"flag"
	"fmt"
	"strings"
	"time"
)

type Poll struct {
	Duration time.Duration
	Path string
}

func (p *Poll) String() string {
	return fmt.Sprintf("%v %v", p.Duration, p.Path)
}

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

func main() {
	var polls PollList

	flag.Var(&polls, "poll", "interval:path")
	flag.Parse()

	for _, v := range polls {
		fmt.Println(v)
	}
}
