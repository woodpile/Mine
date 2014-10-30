package main

import (
	"net/http"
)

func startServer() {
	err := http.ListenAndServe(":9991", nil)
	if nil != err {
		GetLogInstance().Println("ListenAndServe:", err)
	}
}

func regServerHandle() {
	http.HandleFunc("/mine", hMineMain)
}

func hMineMain(w http.ResponseWriter, r *http.Request) {
	if "POST" != r.Method {
		return
	}

	request := NewNetRequest()
	response := NewNetResponse(request)

	//解析请求内容
	err := request.decodeRequest(r)
	if nil != err {
		response.responseError(w, err)
		return
	}

	var ret string
	if ".UserLogin" == request.req.Method {
		//创建请求
		ret, err = request.createUserChannel()
		if nil != err {
			response.responseError(w, err)
			return
		}
	} else {
		//分发请求
		ret, err = request.dispatchMethod()
		if nil != err {
			response.responseError(w, err)
			return
		}
	}
	//写响应
	err = response.responseData(w, ret)
	if nil != err {
		response.responseError(w, err)
		return
	}
}
