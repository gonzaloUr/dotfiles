package main

import (
	"fmt"
	"io"
	"log"
	"os"
	"os/exec"
	"regexp"
	"strconv"
	"strings"
	"sync"
)

type Model struct {
	Inputs  []string
	Outputs []string
	Matrix  [][]bool
}

func main() {
	var inputs []string
	var outputs []string
	var redirects []string
	var curr *[]string
	for _, v := range os.Args[1:] {
		if v == "-i" {
			curr = &inputs
			continue
		} else if v == "-o" {
			curr = &outputs
			continue
		} else if v == "-r" {
			curr = &redirects
			continue
		}
		if curr == nil {
			fmt.Printf("invalid argument %s\n", v)
			os.Exit(1)
		}
		*curr = append(*curr, v)
	}
	missing := make([]string, 0, 3)
	if len(inputs) == 0 {
		missing = append(missing, "inputs")
	}
	if len(outputs) == 0 {
		missing = append(missing, "outputs")
	}
	if len(redirects) == 0 {
		missing = append(missing, "redirects")
	}
	if len(missing) != 0 {
		fmt.Printf("no %s", missing[0])
		for _, v := range missing[1 : len(missing)-1] {
			fmt.Printf(", %s", v)
		}
		fmt.Printf(" and %s\n", missing[len(missing)-1])
		os.Exit(1)
	}
	reg := regexp.MustCompile(`(\d+)-(\d+)`)
	matrix := make([][]bool, len(outputs))
	for i := range matrix {
		matrix[i] = make([]bool, len(inputs))
	}
	for _, redirect := range redirects {
		subs := reg.FindStringSubmatch(redirect)
		if len(subs) == 0 {
			fmt.Printf("invalid redirection %s\n", redirect)
			os.Exit(1)
		}
		input, _ := strconv.Atoi(subs[1])
		if input > len(inputs) {
			fmt.Printf("no input with value %d\n", input)
			os.Exit(1)
		}
		output, _ := strconv.Atoi(subs[2])
		if output > len(outputs) {
			fmt.Printf("no output with value %d\n", output)
			os.Exit(1)
		}
		matrix[output-1][input-1] = true
	}
	Switch(&Model{
		Inputs:  inputs,
		Outputs: outputs,
		Matrix:  matrix,
	})
}

func CreateCommand(command string) *exec.Cmd {
	split := strings.Split(command, " ")
	return exec.Command(split[0], split[1:]...)
}

func InputCommand(command string) (*exec.Cmd, io.Reader) {
	cmd := CreateCommand(command)
	pipe, err := cmd.StdoutPipe()
	if err != nil {
		log.Fatal(err)
	}
	return cmd, pipe
}

func OutputCommand(input io.Reader, command string) *exec.Cmd {
	cmd := CreateCommand(command)
	cmd.Stdin = input
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	return cmd
}

type MergeReader struct {
	call     []chan int
	calling  []bool
	eof      []bool
	response chan *readResponse
	buffer   []byte
}

type readResponse struct {
	p   []byte
	n   int
	err error
	i   int
}

func MergeReaders(readers ...io.Reader) io.Reader {
	call := make([]chan int, len(readers))
	for i := 0; i < len(readers); i++ {
		call[i] = make(chan int, 1)
	}
	resp := make(chan *readResponse, len(readers))
	for i := 0; i < len(readers); i++ {
		go func(c chan int, resp chan *readResponse, i int, r io.Reader) {
			for {
				length := <-c
				p := make([]byte, length)
				n, err := r.Read(p)
				resp <- &readResponse{p, n, err, i}
				if err != nil {
					break
				}
			}
		}(call[i], resp, i, readers[i])
	}
	return &MergeReader{
		call:     call,
		calling:  make([]bool, len(readers)),
		eof:      make([]bool, len(readers)),
		response: resp,
	}
}

func (m *MergeReader) allDone() bool {
	for _, v := range m.eof {
		if !v {
			return false
		}
	}
	return true
}

func (m *MergeReader) Read(p []byte) (int, error) {
	if len(m.buffer) == 0 {
		if len(m.response) == 0 {
			if m.allDone() {
				return 0, io.EOF
			}
			for i, c := range m.call {
				if !m.eof[i] && !m.calling[i] {
					m.calling[i] = true
					c <- len(p)
				}
			}
		}
		var resp *readResponse
		for {
			resp = <-m.response
			m.calling[resp.i] = false
			if resp.err == nil {
				break
			}
			m.eof[resp.i] = true
			if len(m.response) == 0 {
				break
			}
		}
		if resp == nil {
			return 0, io.EOF
		}
		m.buffer = make([]byte, resp.n)
		copy(m.buffer, resp.p[:resp.n])
	}
	var n int
	if len(m.buffer) > len(p) {
		copy(p, m.buffer)
		m.buffer = m.buffer[len(p):]
		n = len(p)
	} else {
		copy(p, m.buffer)
		n = len(m.buffer)
		m.buffer = nil
	}
	return n, nil
}

func Switch(model *Model) {
	var inputsCmd []*exec.Cmd
	var inputsReaders []io.Reader
	for _, str := range model.Inputs {
		cmd, r := InputCommand(str)
		inputsCmd = append(inputsCmd, cmd)
		inputsReaders = append(inputsReaders, r)
	}
	outputReaders := make([][]io.Reader, len(model.Inputs))
	for i := range model.Inputs {
		var n int
		for j := range model.Outputs {
			if model.Matrix[j][i] {
				n++
			}
		}
		readers := NewCloneReader(inputsReaders[i], n)
		var i int
		for j := range model.Outputs {
			if model.Matrix[j][i] {
				outputReaders[j] = append(outputReaders[j], readers[i])
				i++
			}
		}
	}
	var outputsCmd []*exec.Cmd
	for i, str := range model.Outputs {
		stdin := MergeReaders(outputReaders[i]...)
		outputsCmd = append(outputsCmd, OutputCommand(stdin, str))
	}
	for _, cmd := range inputsCmd {
		err := cmd.Start()
		if err != nil {
			log.Fatal(err)
		}
	}
	var wg sync.WaitGroup
	wg.Add(len(outputsCmd))
	for _, cmd := range outputsCmd {
		go func(cmd *exec.Cmd, wg *sync.WaitGroup) {
			err := cmd.Run()
			if err != nil {
				log.Fatal(err)
			}
			wg.Done()
		}(cmd, &wg)
	}
	wg.Wait()
}
