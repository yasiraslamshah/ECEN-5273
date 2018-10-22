Readme File

Author: Yasir Aslam Shah
Network System
Programming Assignment 1
ECEN 5723
Fall 2018

This project contains a server.c and a client.c along with the respective makefiles for programming assignment 1.The server and client are connected over command prompt as ./server <Port_Number> for running the server and ./client <IP_ADDRESS> <Port_Number> for running the client. The port number is greater than 5000.
The Code follows UDP protocol and can send and receive files with server. There are five commands running in the program:

1.get <filename>
	get command is used to receive a file from server. The file name is included with the get 	command separated by filename.If no such file exist over server, the Server and Client 	respond with an error message as No file Found. The received file is saved as 	Recieved_<filename>  

2.put <filename>
	put command is used to send a file to server. The file name is included with the put command 	separated by filename. If no such file exist over server, the Server and Client respond with an 	error message as No file Found.The sent file is saved as Sent_<filename>

3.ls
	ls command is used to list out all the files and display the contents over client side and all the 	content is stored in a file and that file is sent to client. The ls received is stored as 	recieved_ls_op.txt 

4.remove <filename>
	remove command is used to remove a file on server. The file name is included with the remove 	command  separated by filename. If no such file exist over server, the Server and Client respond 	with an  error message as No file Found

5.exit
	exit command is used to exit on both client and server side

Reliability in this code is implemented using Stop and Wait and Timeout applications. This code has been tested on remote server for files greater than 5MB.

Yasir Aslam Shah
