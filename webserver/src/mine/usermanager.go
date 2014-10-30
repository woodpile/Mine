package main

import (
	"fmt"
	"reflect"
	"strconv"
	"strings"
	"sync"
	"time"
)

type userIndex struct {
	name        string
	cin         chan *UserChan
	sessionData map[string]interface{}
}

type userManager struct {
	newIndex int64
	indexMap map[int64]*userIndex
	lock     sync.Mutex
}

var gUserManagerInst *userManager = nil

func newUserManager() *userManager {
	return &userManager{
		newIndex: 10000,
		indexMap: make(map[int64]*userIndex),
	}
}

func newUserIndex() *userIndex {
	return &userIndex{
		cin:         make(chan *UserChan),
		sessionData: make(map[string]interface{}, 0),
	}
}

//---------------

type UserChan struct {
	Method string
	Data   string
	Resp   chan string
	Err    chan error
}

func NewUserChan(m, d string) *UserChan {
	return &UserChan{
		Method: m,
		Data:   d,
		Resp:   make(chan string),
		Err:    make(chan error),
	}
}

func GetUmInstance() *userManager {
	if nil == gUserManagerInst {
		GetLogInstance().Println("create userManager")
		gUserManagerInst = newUserManager()
	}

	return gUserManagerInst
}

func (u *userManager) Login() *userIndex {
	ui := newUserIndex()

	u.lock.Lock()
	defer u.lock.Unlock()

	ui.name = fmt.Sprintf("%016X", u.newIndex)
	u.indexMap[u.newIndex] = ui
	GetLogInstance().Println("user login %v", ui)

	u.newIndex += 1

	return ui
}

func (u *userManager) Logoff(n string) {
	index, err := strconv.ParseInt(n, 16, 64)
	if nil != err {
		GetLogInstance().Println("user name parse failed, name", n)
		return
	}

	u.lock.Lock()
	defer u.lock.Unlock()

	GetLogInstance().Println("user logoff", u.indexMap[index])

	delete(u.indexMap, index)
}

func (u *userManager) CheckLogin(n string) (bool, error) {
	index, err := strconv.ParseInt(n, 16, 64)
	if nil != err {
		return false, fmt.Errorf("user name parse failed, name %v", n)
	}

	_, exist := u.indexMap[index]
	if false == exist {
		return false, fmt.Errorf("user %v did not login", n)
	}

	return true, nil
}

func (u *userManager) GetUserChan(n string) chan *UserChan {
	index, err := strconv.ParseInt(n, 16, 64)
	if nil != err {
		return nil
	}

	return u.indexMap[index].cin
}

// 解析Method字符串
func (n *UserChan) parseMethodString() (string, string, error) {
	//将Method字符串截为两段
	ps := strings.Split(n.Method, ".")
	if 2 != len(ps) {
		return "", "", fmt.Errorf("invalid method struct: %v", ps)
	}

	if 0 == len(ps[0]) || 0 == len(ps[1]) {
		return ps[0], ps[1], fmt.Errorf("invalid method string")
	}

	//两段分别为模块名module和方法名method
	return ps[0], ps[1], nil
}

// 分发请求的处理方法
func (n *UserChan) dispatchMethod(u *userIndex) (string, error) {
	invalidRet := ""

	//解析Method字符串
	mo, me, err := n.parseMethodString()
	if nil != err {
		return invalidRet, fmt.Errorf("method parse failed: %v, module: %s, method: %s", err, mo, me)
	}

	var frame ModuleList

	//取出相应的模块的反射类型
	rtf := reflect.ValueOf(frame)
	rmo := rtf.FieldByName(mo)
	if false == rmo.IsValid() {
		return invalidRet, fmt.Errorf("invalid module", mo)
	}

	//取出相应的方法的反射类型
	rme := reflect.New(rmo.Type()).MethodByName(me)
	if false == rme.IsValid() {
		return invalidRet, fmt.Errorf("invalid method", me)
	}

	GetLogInstance().Printf("user %s dispatch method:%s.%s, data%s\n", u.name, mo, me, n.Data)

	//调用方法
	ret := rme.Call([]reflect.Value{reflect.ValueOf(u), reflect.ValueOf(n.Data)})
	if 2 != len(ret) {
		GetLogInstance().Fatal("method return param num is not 2, is %d", len(ret))
	}
	// GetLogInstance().Printf("ret0, type is %v, value is %v", ret[0].Kind(), ret[0].String())
	// GetLogInstance().Printf("ret1, type is %v, value is %v", ret[1].Kind(), ret[1].Interface())

	//返回值0位为返回字符串，1位为error
	if nil != ret[1].Interface() {
		return invalidRet, fmt.Errorf("method call failed: %v", ret[1].Interface())
	}

	return ret[0].String(), nil
}

//启动一个用户goruntime用于处理这个用户的请求
func (u *userIndex) StartUser() {
	GetLogInstance().Println("user", u.name, "goruntime start")

	timeout := 0
	for {
		select {
		case uc := <-u.cin:
			ret, err := uc.dispatchMethod(u)
			uc.Resp <- ret
			uc.Err <- err
			timeout = 0
		case <-time.After(1 * time.Minute):
			if 0 == timeout {
				var m Matrix
				m.ReleasePage(u, "")

				GetUmInstance().Logoff(u.name)
			}
			timeout += 1
			// GetLogInstance().Println("timeout", timeout)
		}
		if 2 == timeout {
			break
		}
	}

	GetLogInstance().Println("user", u.name, "goruntime end")
}

func (u *userIndex) SetSessionData(k string, v interface{}) {
	u.sessionData[k] = v
}

func (u *userIndex) GetSessionData(k string) interface{} {
	return u.sessionData[k]
}
