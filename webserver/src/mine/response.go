package main

import (
	"crypto/rc4"
	"encoding/json"
	"fmt"
	"net/http"
)

type responseData struct {
	ErrCode int32
	Err     string
	Time    string
	Data    string
}

type NetResponse struct {
	resp    responseData
	request *NetRequest
}

func NewNetResponse(req *NetRequest) *NetResponse {
	return &NetResponse{
		request: req,
	}
}

func (n *NetResponse) responseError(w http.ResponseWriter, e error) error {
	n.resp.Err = fmt.Sprintf("err:%v", e)
	n.resp.Time = ""
	n.resp.Data = ""
	n.resp.ErrCode = n.request.ErrCode

	p, _ := json.Marshal(n.resp)
	GetLogInstance().Println("response:", len(p), ":", string(p))

	//生成加密句柄
	k := []byte{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}
	crp, err := rc4.NewCipher(k)
	if nil != err {
		return fmt.Errorf("create cipher failed: %v", err)
	}

	//加密
	ep := make([]byte, len(p))
	crp.XORKeyStream(ep, p)
	//GetLogInstance().Println("encrypto:", ep)

	//写响应
	wl, err := w.Write(ep)
	if nil != err || wl != len(p) {
		return fmt.Errorf("write response error: %v", err)
	}

	return nil
}

func (n *NetResponse) responseData(w http.ResponseWriter, d string) error {
	n.resp.Err = "ok"
	n.resp.Time = ""
	n.resp.Data = d
	n.resp.ErrCode = ERR_NOERR

	//将response编码
	p, err := json.Marshal(n.resp)
	if nil != err {
		GetLogInstance().Println("json marshal error while response :", err)
		return fmt.Errorf("json marshal error while response: %v", err)
	}
	// GetLogInstance().Println("response", len(p), ":", string(p))

	//生成加密句柄
	k := []byte{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}
	crp, err := rc4.NewCipher(k)
	if nil != err {
		GetLogInstance().Println("create cipher failed while response :", err)
		return fmt.Errorf("create cipher failed: %v", err)
	}

	//加密
	ep := make([]byte, len(p))
	crp.XORKeyStream(ep, p)
	//GetLogInstance().Println("encrypto:", ep)

	//写响应
	wl, err := w.Write(ep)
	if nil != err || wl != len(p) {
		GetLogInstance().Println("write response error :", err)
		return fmt.Errorf("write response error: %v", err)
	}

	return nil
}
