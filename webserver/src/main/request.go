package main

import (
	"crypto/rc4"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
)

type NetRequest struct {
	User   string
	Time   string
	Method string
	Data   string
}

func NewNetRequest() *NetRequest {
	return &NetRequest{}
}

// 解析请求内容到NetRequest
func (n *NetRequest) decodeRequest(r *http.Request) error {
	//从http请求中读取请求内容
	p := make([]byte, r.ContentLength)
	l, err := r.Body.Read(p)
	if nil != err && io.EOF != err {
		return fmt.Errorf("read http request body failed, content len: %d, err: %v", r.ContentLength, err)
	}
	//GetLogInstance().Println("receive:", l)

	//生成解密句柄
	k := []byte{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}
	crp, err := rc4.NewCipher(k)
	if nil != err {
		return fmt.Errorf("create cipher failed: %v", err)
	}

	//解密
	dnp := make([]byte, l)
	crp.XORKeyStream(dnp, p[0:l])
	// GetLogInstance().Println("decrypto:", string(dnp))

	//对解密后的明文字符串json解码
	json.Unmarshal(dnp, &n)
	//GetLogInstance().Printf("lalala: %v\n", *n)
	return nil
}

type OutUserLogin struct {
	User string
}

func (n *NetRequest) createUserChannel() (string, error) {
	ui := GetUmInstance().Login()

	out := OutUserLogin{User: ui.name}

	//编码结果
	ret, err := json.Marshal(out)
	if nil != err {
		GetLogInstance().Println("encode ret failed,", err)
		return "", fmt.Errorf("parse param failed, %v", err)
	}

	go ui.StartUser()

	return string(ret), nil
}

// 取得用户线程通道
func (n *NetRequest) getUserChannel() (chan *UserChan, error) {
	if 16 != len(n.User) {
		return nil, fmt.Errorf("invalid User string: %v", n.User)
	}

	um := GetUmInstance()
	_, err := um.CheckLogin(n.User)
	if nil != err {
		return nil, fmt.Errorf("check login failed. %v", err)
	}

	return um.GetUserChan(n.User), nil
}

// 分发请求的处理方法
func (n *NetRequest) dispatchMethod() (string, error) {
	//取该用户线程的channel
	cin, err := n.getUserChannel()
	if nil != err {
		return "", err
	}

	uc := NewUserChan(n.Method, n.Data)

	cin <- uc

	return <-uc.Resp, <-uc.Err
}
