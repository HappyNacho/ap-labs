// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"bufio"
	"bytes"
	"flag"
	"fmt"
	"log"
	"net"
	"os"
	"strings"
	"time"
)

//!+broadcaster
type client chan<- string // an outgoing message channel

type userStruct struct {
	username string
	msg      string
}

type clientStruct struct {
	client     client
	connection net.Conn
}

var (
	entering      = make(chan clientStruct)
	leaving       = make(chan client)
	messages      = make(chan string) // all incoming client messages
	serverNewLine = "irc-server > "
	admin         client
	adminUser     string
	direct        = make(chan userStruct)
	kick          = make(chan string)
	users         map[string]client
	connections   map[string]net.Conn
)

func broadcaster() {
	clients := make(map[client]bool) // all connected clients
	users = make(map[string]client)
	connections = make(map[string]net.Conn)

	for {
		select {
		case msg := <-messages:
			// Broadcast incoming message to all
			// clients' outgoing message channels.
			for cli := range clients {
				cli <- msg
			}

		case uS := <-direct:
			users[uS.username] <- uS.msg

		case cS := <-entering:
			if len(clients) == 0 {
				cS.client <- serverNewLine + "Congrats, you were the first user."
				cS.client <- serverNewLine + "You're the new IRC Server ADMIN"
				fmt.Printf("[%s] was promoted as the channel ADMIN\n", adminUser)
				admin = cS.client
			}

			clients[cS.client] = true
			users[adminUser] = cS.client
			connections[adminUser] = cS.connection

		case cli := <-leaving:
			if admin == cli {

				for newAdmin := range clients {
					admin = newAdmin
					newAdmin <- serverNewLine + "You're the new admin!"
					continue
				}
			}

			delete(clients, cli)
			close(cli)

		case user := <-kick:
			connection := connections[user]
			client := users[user]
			delete(clients, client)
			delete(connections, user)
			delete(users, user)
			connection.Close()
		}
	}
}

//!-broadcaster

//!+handleConn
func handleConn(conn net.Conn) {

	var buf = make([]byte, 1024)
	conn.Read(buf)

	//trims nulls at the end and parsed to strings
	myUser := string(bytes.Trim(buf, "\x00"))
	adminUser = string(bytes.Trim(buf, "\x00"))

	ch := make(chan string) // outgoing client messages

	//checks for name duplicates
	if users[myUser] != nil {
		fmt.Fprintln(conn, "User with name: "+myUser+" already exists")
		close(ch)
		conn.Close()
		return
	}

	fmt.Printf("%s New connected user [%s] \n", serverNewLine, myUser)

	go clientWriter(conn, ch)

	//Welcoming messages
	ch <- serverNewLine + "Welcome to the Simple IRC Server"
	ch <- serverNewLine + "Your user [" + myUser + "] is successfully logged"

	//Entering messages
	messages <- serverNewLine + "New connected user [" + myUser + "]"
	entering <- clientStruct{ch, conn}

	input := bufio.NewScanner(conn)

	//will read inputs from clients
	for input.Scan() {
		if len(input.Text()) > 0 && string(input.Text()[0]) == "/" {
			slice := strings.Split(input.Text(), " ")
			command := slice[0]
			switch command {

			//get list of users
			case "/users":
				str := ""
				for listUsers := range users {
					str += listUsers + ", "
				}
				ch <- serverNewLine + str
			//Send message
			case "/msg":
				if len(slice) < 3 {
					ch <- "Command usage: /msg <user> <message>"
					continue
				}
				targetUser := slice[1]

				if _, ok := users[targetUser]; ok {
					directMessage := input.Text()[strings.Index(input.Text(), targetUser)+len(targetUser)+1:]
					direct <- userStruct{targetUser, myUser + " > " + directMessage}
				} else {
					ch <- "User: " + targetUser + " not found in the user list"
				}
			//get server time
			case "/time":

				timezone := "America/Mexico_City"
				loc, _ := time.LoadLocation(timezone)
				theTime := time.Now().In(loc).Format("15:04\n")
				ch <- "Local Time: " + timezone + " " + strings.Trim(theTime, "\n")

			//Get user info
			case "/user":
				if len(slice) < 2 {
					ch <- "Command usage: /user <user>"
					continue
				}
				user := slice[1]
				if _, ok := users[user]; ok {
					ip := connections[user].RemoteAddr().String()
					ch <- "Username: " + user + " IP: " + ip

				} else {
					ch <- "User: " + user + " doesn't exist"
				}
			//kick users
			case "/kick":
				if len(slice) < 2 {
					ch <- "Command usage: /kick <user>"
					continue
				}
				if ch == admin {
					user := slice[1]

					if users[user] == admin {
						ch <- "You can't kick your own username"
						continue
					}
					if _, ok := users[user]; ok {
						messages <- serverNewLine + "[" + user + "] was banhammered"
						kick <- user
					} else {
						ch <- "User: " + user + " doesn't exist"
					}
				} else {
					//classic Ubuntu missing root message
					ch <- "You are not in the sudoers/admin file. This incident will be reported"
				}
			default:
				ch <- "Valid commands are: /users /user /msg /kick /time"

			}

		} else {
			messages <- myUser + " > " + input.Text()
		}
	}
	// NOTE: ignoring potential errors from input.Err()
	leaving <- ch
	messages <- serverNewLine + "[" + myUser + "] left channel"
	delete(users, myUser)
	delete(connections, myUser)
	conn.Close()
}

func clientWriter(conn net.Conn, ch <-chan string) {
	for msg := range ch {
		fmt.Fprintln(conn, msg) // NOTE: ignoring network errors
	}
}

//!-handleConn

//!+main
func main() {
	if len(os.Args) < 5 {
		fmt.Println("Usage: go run server.go -host localhost -port <port>")
		os.Exit(1)
	}
	host := flag.String("host", "localhost", "localhost")
	port := flag.String("port", "9000", "the port")
	flag.Parse()

	listener, err := net.Listen("tcp", *host+":"+*port)
	fmt.Println(serverNewLine + "Simple IRC Server started at " + *host + ":" + *port)

	if err != nil {
		log.Fatal(err)
	}

	go broadcaster()

	fmt.Println(serverNewLine + "Ready for receiving new clients")

	for {

		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		go handleConn(conn)
	}
}

//!-main
