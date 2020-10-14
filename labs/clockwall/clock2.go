// Clock2 is a concurrent TCP server that periodically writes the time.
package main

import (
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"time"
)

func handleConn(c net.Conn, tzlocation string) {
	defer c.Close()

	//location := "Current server time : "
	//location := "name" + " "+":"+" "

	//fmt.Println(t.In(time.Local))
	for {
		_, tzErr := time.LoadLocation(tzlocation)
		if tzErr != nil {
			log.Fatal(tzErr)
			return
		}
		_, err := io.WriteString(c, tzlocation+" : "+time.Now().Format("15:04:05\n"))
		if err != nil {
			return
		}
		time.Sleep(1 * time.Second)
	}
}

/*
func printTime(t time.Time) {
	zone, offset := t.Zone()
	fmt.Println(t.Format(time.Kitchen), "Zone:", zone, "Offset UTC:", offset)
}*/

func main() {

	if os.Args[1] != "-port" {
		fmt.Println("Please use -port as first flag")
		os.Exit(1)
	}

	//Port input from run
	arg := os.Args[2]
	fmt.Println("Running on port: " + arg)

	listener, err := net.Listen("tcp", "localhost:"+arg)
	if err != nil {
		log.Fatal(err)
	}

	tzlocation := os.Getenv("TZ") //DOESNT WORK EVER

	if len(tzlocation) == 0 {
		tzlocation = "Local"
	}

	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err) // e.g., connection aborted
			continue
		}
		go handleConn(conn, tzlocation) // handle connections concurrently
	}
}
