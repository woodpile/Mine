package main

import ()

func main() {
	InitLogManager()

	GetLogInstance().Println("server start")

	regServerHandle()

	startServer()
}
