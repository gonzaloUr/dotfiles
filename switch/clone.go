package main

import "io"

type CloneModel struct {
	reader    io.Reader
	buffer    []byte
	err       error
	position  []int
	toldDone  []bool
	call      chan *Call
	responses []chan []byte
	errors    []chan error
}

type Call struct {
	index  int
	length int
}

type CloneReader struct {
	index int
	call  chan *Call
	res   chan []byte
	err   chan error
}

func (c *CloneReader) Read(p []byte) (int, error) {
	c.call <- &Call{c.index, len(p)}
	res := <-c.res
	err := <-c.err

	if err != nil {
		return 0, err
	}
	copy(p, res)
	return len(res), nil
}

func (c *CloneModel) ToldAllDone() bool {
	for _, v := range c.toldDone {
		if !v {
			return false
		}
	}
	return true
}

func (c *CloneModel) Routine() {
	for c.err == nil && !c.ToldAllDone() {
		call := <-c.call
		pos := c.position[call.index]
		diff := len(c.buffer) - pos
		if diff == 0 {
			if c.err != nil {
				c.responses[call.index] <- nil
				c.errors[call.index] <- c.err
				c.toldDone[call.index] = true
				continue
			}
			p := make([]byte, call.length)
			n, err := c.reader.Read(p)
			if err != nil {
				c.err = err
				c.responses[call.index] <- nil
				c.errors[call.index] <- err
				c.toldDone[call.index] = true
				continue
			}
			c.buffer = append(c.buffer, p[:n]...)
			c.responses[call.index] <- p[:n]
			c.errors[call.index] <- nil
			c.position[call.index] += n
		} else {
			slice := c.buffer[c.position[call.index]:]
			var toRead int
			if len(slice) > call.length {
				toRead = call.length
			} else {
				toRead = len(slice)
			}
			c.responses[call.index] <- slice[:toRead]
			c.errors[call.index] <- nil
			c.position[call.index] += toRead
		}
		min := c.position[0]
		for _, v := range c.position[1:] {
			if v < min {
				min = v
			}
		}
		if min > 0 {
			newBuf := make([]byte, len(c.buffer) - min)
			copy(newBuf, c.buffer[min:])
			c.buffer = newBuf
			for i := range c.position {
				c.position[i] -= min
			}
		}
	}
}

func NewCloneReader(r io.Reader, n int) []io.Reader {
	call := make(chan *Call)
	responses := make([]chan []byte, n)
	errors := make([]chan error, n)
	for i := 0; i < n; i++ {
		responses[i] = make(chan []byte)
		errors[i] = make(chan error)
	}
	model := &CloneModel{
		reader:    r,
		position:  make([]int, n),
		toldDone:  make([]bool, n),
		call:      call,
		responses: responses,
		errors:    errors,
	}
	go model.Routine()
	ret := make([]io.Reader, n)
	for i := 0; i < n; i++ {
		ret[i] = &CloneReader{
			index: i,
			call:  call,
			res:   responses[i],
			err:   errors[i],
		}
	}
	return ret
}
