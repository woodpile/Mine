package main

import (
	"fmt"
	"io"
	"log"
	"os"
	"time"
)

var gLogger *log.Logger = nil

func InitLogManager() {
	go ChangeLogFile()
}

func GetLogInstance() *log.Logger {
	if nil == gLogger {
		logfile := getNowLogFile()
		gLogger = log.New(logfile, "Debug ", log.Lshortfile|log.LstdFlags)
	}

	return gLogger
}

func getNowLogFile() io.Writer {
	now := time.Now()
	logname := fmt.Sprintf("log/mine.log.%04d%02d%02d%02d", now.Year(), now.Month(), now.Day(), now.Hour())

	file, err := os.OpenFile(logname, os.O_RDWR|os.O_APPEND|os.O_CREATE, 0664)
	if nil != err {
		log.Fatal("open log file faild.", err)
	}

	return file
}

func ChangeLogFile() {
	now := time.Now()
	first := time.Date(now.Year(), now.Month(), now.Day(), now.Hour()+1, 0, 1, 0, now.Location())

	nt := time.NewTicker(time.Duration(first.Unix()-now.Unix()) * time.Second)

	<-nt.C
	gLogger = nil
	nt.Stop()

	loop := time.NewTicker(time.Hour)
	for {
		<-loop.C
		gLogger = nil
	}
}
