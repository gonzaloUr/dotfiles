package main

import "io"

type CloneModel struct {
	reader    io.Reader
	buffer    []byte
	position  []int
	call      chan int
	responses []chan []byte
	errors    []chan error
}

type CloneReader struct {
	index int
	call  chan int
	res   chan []byte
}
