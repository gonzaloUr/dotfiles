package main

func main() {
	pulse := NewPulse("stbar")
	go pulse.Loop()
	pulse.ClientInfo()
	<-pulse.done
}
