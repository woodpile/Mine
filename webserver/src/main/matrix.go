package main

import (
	"encoding/json"
	"fmt"
	"math/rand"
	"sync"
	"time"
)

type Matrix struct {
}

type BoxAttr struct {
	Width  int
	Height int
	Bomb   int
	IsOpen bool
	IsFlag bool
}

type OutBoxAttr struct {
	Width  int
	Height int
	Num    int
	IsOpen bool
	Bomb   int
	IsFlag bool
}

type PageAttr struct {
	own       string
	numOpened int

	MaxW   int
	MaxH   int
	Width  int
	Height int
	Boxs   map[int]*BoxAttr
}

type OutPageAttr struct {
	Width       int
	Height      int
	HasOwn      bool
	OpenPercent float32
}

type MapAttr struct {
	ps_lock sync.Mutex
	MaxW    int
	MaxH    int
	Pages   map[int]map[int]*PageAttr
}

type InShowPages struct {
}
type OutShowPages struct {
	MapW  int
	MapH  int
	Pages []OutPageAttr
}

func (m *Matrix) ShowPages(u *userIndex, p string) (string, error) {
	var out OutShowPages
	out.MapW = getMapInstance().MaxW
	out.MapH = getMapInstance().MaxH
	getMapInstance().showPagesSimpleInfo(&out.Pages)

	//编码结果
	ret, err := json.Marshal(out)
	if nil != err {
		GetLogInstance().Println("encode ret failed,", err)
		return "", fmt.Errorf("encode out failed, %v", err)
	}

	return string(ret), nil
}

type InSelectPage struct {
	PageW int
	PageH int
}

type OutSelectPage struct {
	Ret  bool
	MaxW int
	MaxH int
}

func (m *Matrix) SelcetPage(u *userIndex, p string) (string, error) {
	//解析参数
	var param InSelectPage
	err := json.Unmarshal([]byte(p), &param)
	if nil != err {
		GetLogInstance().Println("parse param failed,", err)
	}

	//处理
	page := getMapInstance().getPage(param.PageW, param.PageH)
	if nil != page && "" != page.own {
		return "", fmt.Errorf("page has own (%d, %d)", param.PageW, param.PageH)
	}
	var out OutSelectPage
	select_ret := getMapInstance().selectPage(u.name, param.PageW, param.PageH)
	if true == select_ret {
		page = getMapInstance().getPage(param.PageW, param.PageH)
		u.SetSessionData("ownpage", page)
		out.MaxW = page.MaxW
		out.MaxH = page.MaxH
	}
	out.Ret = select_ret

	//编码结果
	ret, err := json.Marshal(out)
	if nil != err {
		GetLogInstance().Println("encode ret failed,", err)
		return "", fmt.Errorf("encode out failed, %v", err)
	}

	return string(ret), nil
}

type InReleasePage struct {
}

type OutReleasePage struct {
	Ret bool
}

func (m *Matrix) ReleasePage(u *userIndex, p string) (string, error) {
	//处理
	sd := u.GetSessionData("ownpage")
	if nil == sd {
		return "", fmt.Errorf("no session ownpage")
	}
	page := sd.(*PageAttr)
	getMapInstance().releasePage(u.name, page.Width, page.Height)
	var out OutReleasePage
	out.Ret = true

	//编码结果
	ret, err := json.Marshal(out)
	if nil != err {
		GetLogInstance().Println("encode ret failed,", err)
		return "", fmt.Errorf("encode out failed, %v", err)
	}

	return string(ret), nil
}

type InLoadSubPage struct {
	PageW  int
	PageH  int
	Width  int
	Height int
}
type OutLoadSubPage struct {
	PageW   int
	PageH   int
	Refresh []OutBoxAttr
}

func (m *Matrix) LoadSubPage(u *userIndex, p string) (string, error) {
	//解析参数
	var param InLoadSubPage
	err := json.Unmarshal([]byte(p), &param)
	if nil != err {
		GetLogInstance().Println("parse param failed,", err)
	}

	//处理
	page := getMapInstance().getPage(param.PageW, param.PageH)
	if nil == page {
		return "", fmt.Errorf("invalid page index (%d, %d)", param.PageW, param.PageH)
	}
	if false == page.checkOwn(u.name) {
		return "", fmt.Errorf("not own page (%d, %d)", param.PageW, param.PageH)
	}
	box := page.getBox(param.Width, param.Height)
	if nil == box {
		return "", fmt.Errorf("invalid box index (%d, %d)", param.Width, param.Height)
	}
	var out OutLoadSubPage
	page.loadBoxes(param.Width, param.Height, &out.Refresh)
	out.PageW = param.PageW
	out.PageH = param.PageH

	//编码结果
	ret, err := json.Marshal(out)
	if nil != err {
		GetLogInstance().Println("encode ret failed,", err)
		return "", fmt.Errorf("encode out failed, %v", err)
	}

	return string(ret), nil
}

type InClickBox struct {
	PageW  int
	PageH  int
	Width  int
	Height int
}
type OutClickBox struct {
	Num     int
	Bomb    int
	Refresh []OutBoxAttr
}

func (m *Matrix) ClickBox(u *userIndex, p string) (string, error) {
	//解析参数
	var param InClickBox
	err := json.Unmarshal([]byte(p), &param)
	if nil != err {
		GetLogInstance().Println("parse param failed,", err)
		return "", fmt.Errorf("parse param failed, %v", err)
	}

	//处理
	page := getMapInstance().getPage(param.PageW, param.PageH)
	if nil == page {
		return "", fmt.Errorf("invalid page index (%d, %d)", param.PageW, param.PageH)
	}
	if false == page.checkOwn(u.name) {
		return "", fmt.Errorf("not own page (%d, %d)", param.PageW, param.PageH)
	}
	box := page.getBox(param.Width, param.Height)
	if nil == box {
		return "", fmt.Errorf("invalid box index (%d, %d)", param.Width, param.Height)
	}
	if true == box.IsOpen {
		return "", fmt.Errorf("already opend")
	}
	if true == box.IsFlag {
		return "", fmt.Errorf("already flaged")
	}

	out := OutClickBox{Num: 0, Bomb: box.Bomb, Refresh: make([]OutBoxAttr, 0)}
	if 0 == box.Bomb {
		out.Num = page.getBoxNum(param.Width, param.Height)
		box.IsOpen = true

		page.numOpened += 1
	} else {
		page.boxExplosion(param.Width, param.Height, &out.Refresh)
	}

	//编码结果
	ret, err := json.Marshal(out)
	if nil != err {
		GetLogInstance().Println("encode ret failed,", err)
		return "", fmt.Errorf("encode out failed, %v", err)
	}

	return string(ret), nil
}

type InFlagBox struct {
	PageW  int
	PageH  int
	Width  int
	Height int
}
type OutFlagBox struct {
	Num     int
	Bomb    int
	Refresh []OutBoxAttr
}

func (m *Matrix) FlagBox(u *userIndex, p string) (string, error) {
	//解析参数
	var param InFlagBox
	err := json.Unmarshal([]byte(p), &param)
	if nil != err {
		GetLogInstance().Println("parse param failed,", err)
		return "", fmt.Errorf("parse param failed, %v", err)
	}

	//处理
	page := getMapInstance().getPage(param.PageW, param.PageH)
	if nil == page {
		return "", fmt.Errorf("invalid page index (%d, %d)", param.PageW, param.PageH)
	}
	if false == page.checkOwn(u.name) {
		return "", fmt.Errorf("not own page (%d, %d)", param.PageW, param.PageH)
	}
	box := page.getBox(param.Width, param.Height)
	if nil == box {
		return "", fmt.Errorf("invalid box index (%d, %d)", param.Width, param.Height)
	}
	if true == box.IsOpen {
		return "", fmt.Errorf("already opend")
	}
	if true == box.IsFlag {
		return "", fmt.Errorf("already flaged")
	}

	out := OutFlagBox{Num: 0, Bomb: box.Bomb, Refresh: make([]OutBoxAttr, 0)}
	if 0 == box.Bomb {
		out.Num = page.getBoxNum(param.Width, param.Height)
		page.boxExplosion(param.Width, param.Height, &out.Refresh)
	} else {
		box.IsFlag = true
		page.numOpened += 1
	}

	//编码结果
	ret, err := json.Marshal(out)
	if nil != err {
		GetLogInstance().Println("encode ret failed,", err)
		return "", fmt.Errorf("encode out failed, %v", err)
	}

	return string(ret), nil
}

//--------内部处理

var gMap *MapAttr = nil

func getMapInstance() *MapAttr {
	if nil == gMap {
		gMap = new(MapAttr)
		gMap.MaxW = 3
		gMap.MaxH = 3
		gMap.Pages = make(map[int]map[int]*PageAttr, 0)
		rand.Seed(time.Now().Unix())

		for h := 0; h <= gMap.MaxH; h++ {
			gMap.Pages[h] = make(map[int]*PageAttr, 0)
			for w := 0; w <= gMap.MaxW; w++ {
				gMap.Pages[h][w] = nil
			}
		}
	}

	return gMap
}

func (m *MapAttr) getPage(w, h int) *PageAttr {
	if w > m.MaxW || w < 0 || h > m.MaxH || h < 0 {
		return nil
	}

	return m.Pages[h][w]
}

func (m *MapAttr) showPagesSimpleInfo(a *[]OutPageAttr) {
	for w := 0; w <= m.MaxW; w++ {
		for h := 0; h <= m.MaxH; h++ {
			o := new(OutPageAttr)
			o.Width = w
			o.Height = h
			o.HasOwn = false
			o.OpenPercent = 0.0
			if nil != m.Pages[h] && nil != m.Pages[h][w] {
				o.OpenPercent = float32(m.Pages[h][w].numOpened) * 100.0 / float32((m.Pages[h][w].MaxW+1)*(m.Pages[h][w].MaxH+1))
				if "" != m.Pages[h][w].own {
					o.HasOwn = true
				}
			}
			*a = append(*a, *o)
		}
	}
}

func (m *MapAttr) selectPage(n string, w, h int) bool {
	m.ps_lock.Lock()
	defer m.ps_lock.Unlock()

	p := m.Pages[h][w]
	if nil == p {
		p = new(PageAttr)
		p.initPageDefault(w, h)
		m.Pages[h][w] = p
		GetLogInstance().Println("create page", p)
	}

	if "" != p.own {
		return false
	}

	p.own = n

	GetLogInstance().Println("user", n, "select page", w, h)

	return true
}

func (m *MapAttr) releasePage(n string, w, h int) {
	m.ps_lock.Lock()
	defer m.ps_lock.Unlock()

	p := m.Pages[h][w]
	if nil == p {
		GetLogInstance().Fatal("release nil page")
	}

	if n != p.own {
		GetLogInstance().Println("release no own page")
		return
	}

	p.own = ""

	GetLogInstance().Println("user", n, "release page", w, h)
}

func (p *PageAttr) initPageDefault(w, h int) {
	p.MaxW = 100
	p.MaxH = 100
	p.Width = w
	p.Height = h
	p.own = ""
	p.numOpened = 0
	p.Boxs = make(map[int]*BoxAttr, 0)
}

func (p *PageAttr) genIndex(w, h int) int {
	return w + h*p.MaxW
}

func (p *PageAttr) checkOwn(n string) bool {
	if p.own == n {
		return true
	}
	return false
}

func (p *PageAttr) getBox(w, h int) *BoxAttr {
	if w > p.MaxW || w < 0 || h > p.MaxH || h < 0 {
		return nil
	}

	index := p.genIndex(w, h)
	b, ok := p.Boxs[index]
	if false == ok {
		b = new(BoxAttr)
		b.initBoxAttr(w, h)
		p.Boxs[index] = b

		GetLogInstance().Println("create box,", b)
	}

	return b
}

func (p *PageAttr) getBoxNum(w, h int) int {
	num := 0

	b := p.getBox(w-1, h-1)
	if nil != b && 0 != b.Bomb {
		num += 1
	}
	b = p.getBox(w, h-1)
	if nil != b && 0 != b.Bomb {
		num += 1
	}
	b = p.getBox(w+1, h-1)
	if nil != b && 0 != b.Bomb {
		num += 1
	}
	b = p.getBox(w-1, h)
	if nil != b && 0 != b.Bomb {
		num += 1
	}
	b = p.getBox(w+1, h)
	if nil != b && 0 != b.Bomb {
		num += 1
	}
	b = p.getBox(w-1, h+1)
	if nil != b && 0 != b.Bomb {
		num += 1
	}
	b = p.getBox(w, h+1)
	if nil != b && 0 != b.Bomb {
		num += 1
	}
	b = p.getBox(w+1, h+1)
	if nil != b && 0 != b.Bomb {
		num += 1
	}

	return num
}

func (p *PageAttr) loadBoxes(w, h int, a *[]OutBoxAttr) {
	for i := h - 4; i <= h+4; i++ {
		for j := w - 4; j <= w+4; j++ {
			t := p.getBox(j, i)
			if nil == t {
				continue
			}

			ta := new(OutBoxAttr)
			ta.Width = t.Width
			ta.Height = t.Height
			ta.IsOpen = t.IsOpen
			ta.IsFlag = t.IsFlag
			if true == t.IsOpen {
				ta.Num = p.getBoxNum(t.Width, t.Height)
				ta.Bomb = t.Bomb
			}
			*a = append(*a, *ta)
		}
	}
}

func (p *PageAttr) boxExplosion(w, h int, a *[]OutBoxAttr) {
	for i := h - 4; i <= h+4; i++ {
		d := i - h
		if 0 > d {
			d = -d
		}
		d = 4 - d
		for j := w - d; j <= w+d; j++ {
			b := p.getBox(j, i)
			if nil == b {
				continue
			}

			ta := new(OutBoxAttr)
			ta.Width = b.Width
			ta.Height = b.Height
			if j <= w-d+1 || j >= w+d-1 {
				ta.IsOpen = b.IsOpen
				ta.IsFlag = b.IsFlag
				if true == b.IsOpen {
					ta.Num = p.getBoxNum(j, i)
				}
			} else {
				if true == b.IsOpen || true == b.IsFlag {
					p.numOpened -= 1
				}

				b.initBoxAttr(j, i)

				ta.IsOpen = b.IsOpen
				ta.IsFlag = b.IsFlag
				ta.Num = 0
				ta.Bomb = 0
			}
			*a = append(*a, *ta)
		}
	}
}

func (b *BoxAttr) initBoxAttr(w, h int) {
	b.Width = w
	b.Height = h
	b.IsOpen = false
	b.IsFlag = false
	r := rand.Int()
	if (r % 10000) < 1500 {
		b.Bomb = r % 10000
	} else {
		b.Bomb = 0
	}
}
