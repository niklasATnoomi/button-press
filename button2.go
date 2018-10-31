package main

import (
	"fmt"
	"io/ioutil"
	"time"
)

var array_button [60]int
var b int

func memsetLoop() {
	b = len(array_button)
	fmt.Print("init: \n")
	for a := 0; a < b; a++ {
		array_button[a] = 0x30
	}
	for a := 0; a < b; a++ {
		fmt.Print(string(array_button[a]))
	}
	fmt.Print("\n")
}

func check(e error) {
	if e != nil {
		panic(e)
	}
}

func open_port0_24() {

	fn := fmt.Sprintf("/sys/class/gpio/export")
	d1 := []byte("24")
	//err := ioutil.WriteFile("/sys/class/gpio/export", d1, 0644)
	ioutil.WriteFile(fn, []byte(d1), 0644)
}

func direction_port_0_24() {
	fn := fmt.Sprintf("/sys/class/gpio/gpio24/direction")
	d1 := []byte("IN")
	//err := ioutil.WriteFile("/sys/class/gpio/gpio24/direction", d1, 0644)
	ioutil.WriteFile(fn, []byte(d1), 0644)
}

func read_value_0_24() (value uint8) {
	//var value uint8
	dat, err := ioutil.ReadFile("/sys/class/gpio/gpio24/value")
	check(err)
	//fmt.Print(string(dat[0]))
	value = dat[0]
	return value
}

func rollover() {
	b = len(array_button)
	for a := 0; a < b-1; a++ {
		array_button[b-a-1] = array_button[b-a-2]
	}

}

func storage_new_value(button_value int) {
	var c int
	c = 0
	array_button[0] = button_value
	b = len(array_button)
	for a := 0; a < b; a++ {
		//	fmt.Print(string(array_button[a]))

		if a < b-2 {
			if (array_button[b-a-1] != array_button[b-a-2]) && (array_button[b-a-1] >= 0) {
				c++
			}
		} /**/
	}
	fmt.Print("press times=%d", c/2)
	//fmt.Print(string(c))
	fmt.Print("\n")

}

func main() {
	//var a uint8
	var button uint8
	memsetLoop()
	open_port0_24()
	direction_port_0_24()

	//for a := 0; a < 100; a++ {
	for {

		button = read_value_0_24()
		//fmt.Print(string(button))
		rollover()
		storage_new_value(int(button))
		time.Sleep(50 * time.Millisecond)

	}

}
