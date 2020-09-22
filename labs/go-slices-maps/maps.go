package main

import (
	"strings"

	"golang.org/x/tour/wc"
)

func WordCount(s string) map[string]int {
	//creates the map with strings as keys and 0 initial value
	stringmapcounter := make(map[string]int)

	//the loop basically creates an entry for every word and if it already exists, adds a counter
	for _, wordKey := range strings.Fields(s) {
		stringmapcounter[wordKey]++
	}

	return stringmapcounter
}

func main() {
	wc.Test(WordCount)
}
