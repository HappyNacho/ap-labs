package main

import (
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"strings"
	"sync"
)

func main() {
	if len(os.Args) < 2 {
		fmt.Println("Did not enter a server to connect to.")
		os.Exit(1)
	}

	var serverlist sync.WaitGroup

	for _, arg := range os.Args[1:] {

		location := strings.Split(arg, "=")

		if len(location) < 2 {

			fmt.Printf("Please enter: location=server", arg)
			continue
		}
		name := location[0]
		server := location[1]

		serverlist.Add(1)
		go clientCon(name, server)
	}

	serverlist.Wait()
}

func clientCon(name string, server string) {

	fmt.Printf("Location : %v \n", name)
	conn, err := net.Dial("tcp", server)
	if err != nil {
		conn.Close()
		log.Fatal(err)
	}

	//os.Stdout.WriteString(name + " ")
	//io.WriteString(conn, name)
	if _, err := io.Copy(os.Stdout, conn); err != nil {
		log.Fatal(err)
	}

}
