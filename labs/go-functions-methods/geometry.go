// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 156.

// Package geometry defines simple types for plane geometry.
//!+point
package main

import (
	"fmt"
	"math"
	"math/rand"
	"os"
	"strconv"
)

type Point struct {
	X, Y float64
}

func (p Point) getX() float64 {
	return p.X
}
func (p Point) getY() float64 {
	return p.Y
}

//the functions below are taken from the page given: https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
//and adapted from C++ to Go.

// Given three colinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
func onSegment(p, q, r Point) bool {
	if q.getX() <= math.Max(p.getX(), r.getX()) && q.getX() >= math.Min(p.getX(), r.getX()) &&
		q.getY() <= math.Max(p.getY(), r.getY()) && q.getY() >= math.Min(p.getY(), r.getY()) {

		return true
	}

	return false
}

//Orientation function
func orientation(p, q, r Point) int {
	val := (q.getY()-p.getY())*(r.getX()-q.getX()) - (q.getX()-p.getX())*(r.getY()-q.getY())

	if val == 0 {
		return 0 // colinear
	}

	// clock or counterclock wise
	if val < 0 {
		return 2
	}

	return 1
}

// The main function that returns true if line segment 'p1q1'
// and 'p2q2' intersect.
func doIntersect(p1, q1, p2, q2 Point) bool {
	// Find the four orientations needed for general and
	// special cases
	o1 := orientation(p1, q1, p2)
	o2 := orientation(p1, q1, q2)
	o3 := orientation(p2, q2, p1)
	o4 := orientation(p2, q2, q1)

	// General case
	if o1 != o2 && o3 != o4 {
		return true
	}

	// Special Cases
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1
	if o1 == 0 && onSegment(p1, p2, q1) {
		return true
	}
	// p1, q1 and q2 are colinear and q2 lies on segment p1q1
	if o2 == 0 && onSegment(p1, q2, q1) {
		return true
	}

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2
	if o3 == 0 && onSegment(p2, p1, q2) {
		return true
	}

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2
	if o4 == 0 && onSegment(p2, q1, q2) {
		return true
	}

	return false // Doesn't fall in any of the above cases
}

// same thing, but as a method of the Point type
func (p Point) Distance(q Point) float64 {
	return math.Hypot(q.getX()-p.getX(), q.getY()-p.getY())
}

// traditional function
func Distance(p, q Point) float64 {
	return math.Hypot(q.getX()-p.getX(), q.getY()-p.getY())
}

//!-point

//!+path

// A Path is a journey connecting the points with straight lines.
type Path []Point

// Distance returns the distance traveled along the path.
func (path Path) Distance() float64 {
	sum := 0.0
	for i := range path {
		if i > 0 {
			sum += path[i-1].Distance(path[i])
		}
	}
	sum += path[0].Distance(path[len(path)-1])
	return sum
}

//trucated to 2 decimal places in the random point generation as that was what was shown on the readme example, although not specified.
func createPoint() Point {
	createPoint := Point{float64(rand.Intn((100+100)/.01))*.01 - 100, float64(rand.Intn((100+100)/.01))*.01 - 100}
	return createPoint
}

func intersect(path Path) bool {
	//checks for the intersection of points.
	for i := 0; i < len(path); i++ {
		if doIntersect(path[i%len(path)], path[(i+1)%len(path)], path[(i+2)%len(path)], path[(i+3)%len(path)]) == true {
			return false
		}
	}
	for i := 1; i < len(path)-3; i++ {
		if doIntersect(path[len(path)-1], path[0], path[(i)], path[(i+1)]) == true {
			return false
		}
	}
	for i := 0; i < len(path)-4; i++ {
		if doIntersect(path[len(path)-1], path[len(path)-2], path[(i)], path[(i+1)]) == true {
			return false
		}
	}
	return true
}

func makeFigure(totalPoints int) Path {
	var path Path

	for i := 0; i < totalPoints; i++ {
		path = append(path, createPoint())
		if i == 2 {
			for orientation(path[0], path[1], path[2]) == 0 {
				path[2] = createPoint()
			}
		} else if i > 2 {
			for intersect(path) == false {
				path[i] = createPoint()
			}
		}

	}
	return path
}

func main() {

	var points int

	//if more input arguments are given
	if len(os.Args) != 2 {
		fmt.Println("Please only include the number of vertices")
		return
	}
	//get the number of points
	points, err := strconv.Atoi(os.Args[1])
	if err != nil {
		fmt.Println("Invalid input ", err.Error())
		return
	} else if points < 3 {
		fmt.Println("3 or more points needed")
		return
	}
	//interesting that you cannot use a referenced variable with println but you can with printf
	fmt.Printf("- Generating a [%v] sides figure \n", points)
	fmt.Printf("- Figure's vertices \n")

	figure := makeFigure(points)

	for _, point := range figure {
		fmt.Printf("  - (%.2f, %.2f)\n", point.getX(), point.getY())
	}

	fmt.Printf("- Figure's Perimeter \n")

	for i := range figure {
		if i > 0 && i+1 < len(figure) {
			fmt.Printf(" + %.2f", figure[i].Distance(figure[i+1]))
		} else if i == 0 {
			fmt.Printf("  - %.2f", figure[i].Distance(figure[i+1]))
		} else if i+1 == len(figure) {
			fmt.Printf(" + %.2f", figure[0].Distance(figure[len(figure)-1]))
		}
	}
	fmt.Printf(" = %.2f \n", figure.Distance())

}

//!-path
