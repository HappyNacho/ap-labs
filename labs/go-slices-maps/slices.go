package main

import "golang.org/x/tour/pic"

func Pic(dx, dy int) [][]uint8 {

	//creates the slice of slices of length dy
	picture := make([][]uint8, dy)

	for y := range picture {

		//fills each position of dy with a slice of dx
		picture[y] = make([]uint8, dx)

		for x := range picture[y] {

			//gradient value assignment
			picture[y][x] = uint8((x + y) / 2)
		}
	}

	return picture
}

func main() {
	pic.Show(Pic)
}
