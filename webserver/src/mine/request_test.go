package main

import (
	//"fmt"
	"testing"
)

func TestDispatch(t *testing.T) {
	var r NetRequest
	s, err := r.dispatchMethod()
	if nil != err || "ok" != s {
		t.Error("test err:", err)
	}
	t.Log("hahaha")
}
