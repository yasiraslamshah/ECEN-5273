Readme File
Author: Yasir Aslam Shah
Network System
Programming Assignment 2
ECEN 5723
Fall 2018

Build Steps:
  A.Run the follwing on comamnd line :gcc -o server server.c
  B.Followed by : ./server <PORTNUMBER>
  C.Connect : localhost:<PORTNUMBER>/index.html


This project contains a server.c along with its respective makefile for programming assignment 2.The server is a HTTP-based web server that handles multiple simultaneous requests from users.In this project, a HTP request contains three substrings -Request(GET/POST),Request URL and Request Version(HTTP 1.0/1.1).
Initailly a socket is created and binded to a local host and and the server then listens to the socket by creating a connection queue, waiting for clients. Multiple accept connections are created for multiple child threads.In this project, multiple child threads are created using FORK to handle multiple client requests.TCP protocol is used in this programming project.
Initailly a request is recieved from the client that is processed for Request Comamnd(GET/POST), Request URL and Request version and the corresponding files are sent to the client from the HTTP-based server.Initailly a haeder is sent to the client with content type and content length details, then the requested file is sent to the client.
This project also handles POST request from the client and once the POST request is recieved by the server, it sends the appropiate POST Header to the client followed by sending the requested file.(Extra Credit)
This project also takes care of errors in case of any wrong Requested Command, Requested URL or Requested Version , replying back with a "500 Internal Error".Any invalid file requested, or any invalid command other tha GET/POST, or any invalid Requested version other than HTTP1.0/1.1 is proceesed as an error with a "500 Internal ERROR".A similar message on command line console displays if any invalid request is made.
Each Requested URL is realised as a request to fetch files from the server that has access to a directory to provide files belonging to various extensions, required by the client
The listen connection is closed as each child thread finishes its operation.
This project has been tested thoroughly using Telnet for GET/POST 
