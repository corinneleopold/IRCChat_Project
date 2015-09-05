/*
 * Corinne Leopold
 *
 * IRCServer Project
 *
 * 2015
 */

const char * usage =
"                                                               \n"
"IRCServer:                                                   \n"
"                                                               \n"
"Simple server program used to communicate multiple users       \n"
"                                                               \n"
"To use it in one window type:                                  \n"
"                                                               \n"
"   IRCServer <port>                                          \n"
"                                                               \n"
"Where 1024 < port < 65536.                                     \n"
"                                                               \n"
"In another window type:                                        \n"
"                                                               \n"
"   telnet <host> <port>                                        \n"
"                                                               \n"
"where <host> is the name of the machine where talk-server      \n"
"is running. <port> is the port number you used when you run    \n"
"daytime-server.                                                \n"
"                                                               \n";

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "IRCServer.h"

int QueueLength = 5;

int
IRCServer::open_server_socket(int port) {

	// Set the IP address and port for this server
	struct sockaddr_in serverIPAddress; 
	memset( &serverIPAddress, 0, sizeof(serverIPAddress) );
	serverIPAddress.sin_family = AF_INET;
	serverIPAddress.sin_addr.s_addr = INADDR_ANY;
	serverIPAddress.sin_port = htons((u_short) port);
  
	// Allocate a socket
	int masterSocket =  socket(PF_INET, SOCK_STREAM, 0);
	if ( masterSocket < 0) {
		perror("socket");
		exit( -1 );
	}

	// Set socket options to reuse port. Otherwise we will
	// have to wait about 2 minutes before reusing the sae port number
	int optval = 1; 
	int err = setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, 
			     (char *) &optval, sizeof( int ) );
	
	// Bind the socket to the IP address and port
	int error = bind( masterSocket,
			  (struct sockaddr *)&serverIPAddress,
			  sizeof(serverIPAddress) );
	if ( error ) {
		perror("bind");
		exit( -1 );
	}
	
	// Put socket in listening mode and set the 
	// size of the queue of unprocessed connections
	error = listen( masterSocket, QueueLength);
	if ( error ) {
		perror("listen");
		exit( -1 );
	}

	return masterSocket;
}

void
IRCServer::runServer(int port)
{
	int masterSocket = open_server_socket(port);

	initialize();
	
	while ( 1 ) {
		
		// Accept incoming connections
		struct sockaddr_in clientIPAddress;
		int alen = sizeof( clientIPAddress );
		int slaveSocket = accept( masterSocket,
					  (struct sockaddr *)&clientIPAddress,
					  (socklen_t*)&alen);
		
		if ( slaveSocket < 0 ) {
			perror( "accept" );
			exit( -1 );
		}
		
		// Process request.
		processRequest( slaveSocket );		
	}
}

int
main( int argc, char ** argv )
{
	// Print usage if not enough arguments
	if ( argc < 2 ) {
		fprintf( stderr, "%s", usage );
		exit( -1 );
	}
	
	// Get the port from the arguments
	int port = atoi( argv[1] );

	IRCServer ircServer;

	// It will never return
	ircServer.runServer(port);
	
}

void
IRCServer::processRequest( int fd )
{
	// Buffer used to store the comand received from the client
	const int MaxCommandLine = 1024;
	char commandLine[ MaxCommandLine + 1 ];
	int commandLineLength = 0;
	int n;
	
	// Currently character read
	unsigned char prevChar = 0;
	unsigned char newChar = 0;
	
	//
	// The client should send COMMAND-LINE\n
	// Read the name of the client character by character until a
	// \n is found


	// Read character by character until a \n is found or the command string is full.
	while ( commandLineLength < MaxCommandLine &&
		read( fd, &newChar, 1) > 0 ) {

		if (newChar == '\n' && prevChar == '\r') {
			break;
		}
		
		commandLine[ commandLineLength ] = newChar;
		commandLineLength++;

		prevChar = newChar;
	}
	
	// Add null character at the end of the string
	// Eliminate last \r
	commandLineLength--;
        commandLine[ commandLineLength ] = 0;

	printf("RECEIVED: %s\n", commandLine);

	const char * command = (char *) malloc(50 * sizeof(char));
	char * user = (char *) malloc(100 * sizeof(char));
	const char * password = (char *) malloc(50 * sizeof(char));
	char * args = (char *) malloc(1000 * sizeof(char));
	char * room = (char *) malloc(500 * sizeof(char));
	int lastMsgNum = 0;

	int i = 0;
	int one = 0;
	int two = 0;
	int three = 0;
	int four = 0;
	char w[5][1000];
	while (commandLine[i] != ' ') {		//find and store COMMAND
		w[0][one] = commandLine[i];
		i++;
		one++;
	}
	w[0][one] = '\0';
	command = w[0];
	i++;

	while(commandLine[i] != ' ') {		//find and store USER
		w[1][two] = commandLine[i];
		i++;
		two++;
	}
	w[1][two] = '\0';
	user = w[1];
	i++;

	while(commandLine[i] != ' ' && commandLine[i] != '\n') {	//find and store PASSWORD
		w[2][three] = commandLine[i];
		i++;
		three++;
	}
	w[2][three] = '\0';
	password = w[2];
	i++;

	if (strcmp(command, "ENTER-ROOM") == 0 || strcmp(command, "GET-USERS-IN-ROOM") == 0 || strcmp(command, "CREATE-ROOM") == 0 
	|| strcmp(command, "LEAVE-ROOM") == 0) {
		while (commandLine[i] != ' ' && commandLine[i] != '\n') {
			w[3][four] = commandLine[i];
			i++;
			four++;
		}
		w[3][four] = '\0';
		room = w[3];
	}
	
	else if (strcmp(command, "GET-MESSAGES") == 0) {
		if (commandLine[i] != ' ') {
			lastMsgNum = (commandLine[i] - '0');	
			i += 2;
		}
		int wordIndex = 0;
		char word[200];
		while (commandLine[i] != ' ') {
			word[wordIndex++] = commandLine[i];
			i++;
		}
		word[wordIndex] = '\0';
		wordIndex = 0;
		room = word;
		i+= 1;
		args = "";
	}
	else if (strcmp(command, "SEND-MESSAGE") == 0) {
		int wordIndex = 0;
		char word[200];
		while (commandLine[i] != ' ') {
			word[wordIndex++] = commandLine[i];
			i++;
		}
		word[wordIndex] = '\0';
		wordIndex = 0;
		room = word;
		i+= 1;

		while (commandLine[i] != '\n') {
			w[3][four] = commandLine[i];
			i++;
			four++;
		}
		w[3][four] = '\0';
		args = w[3];
	}
	else
		args = "";

	printf("command=%s\n", command);
	printf("user=%s\n", user);
	printf( "password=%s\n", password );
	printf("args=%s\n", args);

	if (!strcmp(command, "ADD-USER")) {
		addUser(fd, user, password, args);
	}
	else if (!strcmp(command, "ENTER-ROOM")) {
		enterRoom(fd, user, password, room);
	}
	else if (!strcmp(command, "LEAVE-ROOM")) {
		leaveRoom(fd, user, password, room);
	}
	else if (!strcmp(command, "SEND-MESSAGE")) {
		sendMessage(fd, user, password, room, args);
	}
	else if (!strcmp(command, "GET-MESSAGES")) {
		getMessages(fd, user, password, lastMsgNum, room);
	}
	else if (!strcmp(command, "GET-USERS-IN-ROOM")) {
		getUsersInRoom(fd, user, password, room);
	}
	else if (!strcmp(command, "GET-ALL-USERS")) {
		getAllUsers(fd, user, password, args);
	}
	else if (!strcmp(command, "CREATE-ROOM")) {
		createRoom(fd, user, password, room);
	}
	else if (!strcmp(command, "LIST-ROOMS")) {
		listRooms(fd, user, password, args);
	}
	else {
		const char * msg =  "UNKNOWN COMMAND\r\n";
		write(fd, msg, strlen(msg));
	}
	close(fd);	
}

struct ListNode {
	char * name;
	const char * pass;
	char * myRooms[10000];
	struct LinkedList * rooms;
	int numRooms;	
	struct ListNode * next;
};
typedef struct ListNode ListNode;


struct LinkedList {
	ListNode * head;
};
typedef struct LinkedList LinkedList;

struct Room {
	char * room;	//one particular room
	LinkedList * peeps;	//list of people in that room
	char * messages[100];
	struct Room * next;
}; typedef struct Room Room;

struct RoomList {
	Room * head;
};
typedef struct RoomList RoomList;

LinkedList * people;
LinkedList * rooms; 

RoomList * r;

FILE * f;
char word[10000][50];
int count = 0;
int msgNum = 0;
int userNum = 0;

void
IRCServer::initialize() {

	people = (LinkedList *) malloc(sizeof(LinkedList));
	people->head = (ListNode*) malloc(sizeof(ListNode));
	rooms = (LinkedList *) malloc(sizeof(LinkedList));
	rooms->head = (ListNode*) malloc(sizeof(ListNode));
	r = (RoomList*) malloc(sizeof(RoomList));
	r->head = (Room*) malloc(sizeof(Room));
	ListNode * list = people->head;
	list->rooms = (LinkedList*) malloc(sizeof(LinkedList));
	list->rooms->head = (ListNode*) malloc(sizeof(ListNode));

	f = fopen("password.txt", "a+");
	int c = 0;
	int i = 0;
	int newlineCount = 0;
	c = fgetc(f);
	while (c != -1 && c != EOF) {
		if (c != ' ' && c != '\n' && c != '\r') {
			count++;
		}
		while (c != ' ' && c != '\n') {
			word[userNum][i++] = c;
			c = fgetc(f);
			newlineCount++;
		}
		if (newlineCount != 0) {
		
		word[userNum][i] = '\0';
			i = 0;
			if (count % 2 != 0)
				list->name = word[userNum];
			else if (count % 2 == 0) {
				list->pass = word[userNum];
				list->next = (ListNode *) malloc(sizeof(ListNode));
				list = list->next;
			}
		}
		c = fgetc(f);
		userNum++;
		newlineCount = 0;
	}
}

bool
IRCServer::checkPassword(int fd, char * user, const char * password) {
	ListNode * e = people->head;
	while (e != NULL) {
		if (strcmp(e->name, user) == 0) {
			if (strcmp(e->pass, password) == 0) 
				return true;
			else
				return false;
		}
		if (e->next->name == NULL)
			return false;
		e = e->next;
	}
	//User does not exist
	return false;
}

void llist_insert(int fd, char * user, const char * password) {
	
	f = fopen("password.txt", "a+");	
	fprintf(f, "%s %s ", user, password);	
	fclose(f);
	return;
}

void
IRCServer::addUser(int fd, char * user, const char * password, char * args) {

	llist_insert(fd, user, password);
	initialize();
	write(fd, "OK\r\n", 4);
	return;
}
void
IRCServer::createRoom(int fd, char * user, const char * password, char * room) {	
	Room * x = r->head;
	if (x->room == NULL) {
		x->room = strdup(room);
	} else {
		while (x != NULL) {
			if (x->room != NULL) {
				if (strcmp(x->room, room) == 0) {
					write(fd, "DENIED\r\n", 8);
					return;
				}
			} else
				break;
			x = x->next;
		}
		x->room = strdup(room);
		x = x->next;
	}
	f = fopen("rooms.txt", "a+");
	fprintf(f, "%s ", room);
	fclose(f);
	write(fd, "OK\r\n", 4);
}

void
IRCServer::listRooms(int fd, char * user, const char * password, char * args) {
	f = fopen("rooms.txt", "r");
	char word[500];
	int c = 0;
	int i = 0;
	c = fgetc(f);
	if (c == EOF || c == NULL) {
		write(fd, "No rooms\r\n", 10);
		return;
	}
	while (c != EOF) {
		if (c != ' ' && c != '\n') 
			word[i++] = c;
		else {
			word[i] = '\0';
			i = 0;
			if (word[0] != '\0') {
				write(fd, word, strlen(word));
				write(fd, "\r\n", 2);
			}
		}
		c = fgetc(f);
	}
}

void
IRCServer::initRooms() {
	Room * a = r->head;
	f = fopen("rooms.txt", "r");
	char word[500];
	int c = 0;
	int i = 0;
	c = fgetc(f);
	//find the room in the text file
	while (c != EOF) {
		if (c != ' ' && c != '\n') 
			word[i++] = c;
		else {
			word[i] = '\0';
			i = 0;
			if (word[0] != '\0') { //populate the Room list			
				a->room = strdup(word);
				a = a->next;
			}
		}
		c = fgetc(f);
	}
}

void
IRCServer::enterRoom(int fd, char * user, const char * password, char * room)
{	
	if (checkPassword(fd, user, password) == false) {
		write(fd, "ERROR (Wrong password)\r\n", 24);
		return;
	}
	Room * x = r->head;
	while (x != NULL) {
		if (x->room == NULL) {
			write(fd, "ERROR (No room)\r\n", 17);
			return;
		}
		if (strcmp(x->room, room) == 0) 
			break;
		x = x->next;
	}
	ListNode * e = people->head;
	while (e != NULL) {
		if (strcmp(e->name, user) == 0) {
			e->myRooms[e->numRooms] = strdup(room);
			e->numRooms++;
			write(fd, "OK\r\n", 4);
			return;
		} 
		e = e->next;
		if (e->name == NULL)
			write(fd, "ERROR (No user)\r\n", 17);
	}
}

void
IRCServer::leaveRoom(int fd, char * user, const char * password, char * room) {
	if (checkPassword(fd, user, password) == false) {
		write(fd, "ERROR (Wrong password)\r\n", 24);
		return;
	}
	ListNode * e = people->head;
	while (e != NULL) {
		if (strcmp(e->name, user) == 0) {
			int i;
			for (i = 0; i < 10000; i++) {
				if (e->myRooms[i] != NULL) {
					if (strcmp(e->myRooms[i], room) == 0) {
						e->myRooms[i] = "NO ROOM IN THIS SPOT";
						write(fd, "OK\r\n", 4);
						return;
					}
				} else
					break;
			}
		}
		e = e->next;
		if (e->name == NULL) {
			write(fd, "ERROR (No user in room)\r\n", 25);
			return;
		}
	}
	
}
void
IRCServer::sendMessage(int fd, char * user, const char * password, char * room, char * args)
{	
	if (checkPassword(fd, user, password) == false) {
		write(fd, "ERROR (Wrong password)\r\n", 24);
		return;
	}
	ListNode * e = people->head;
	while (e != NULL) {
		if (e->name == NULL) {
			write(fd, "ERROR (No user)\r\n", 17);
			return;
		}
		if (strcmp(e->name, user) == 0) {	//finding the user
			int i;
			int found = 0;
			for (i = 0; i < 1000; i++) {	//check if user in is room
				if (e->myRooms[i] != NULL) {
					if (strcmp(e->myRooms[i], room) == 0) {
						found = 1;
						break;
					}
				} 
			}
			if (found == 0) {
				write(fd, "ERROR (user not in room)\r\n", 26);
				return;
			}
			if (found == 1)
				break;
		}
		e = e->next;
	}
			Room * x = r->head;
			while (x != NULL) {
				
				if (x->room == NULL) {
					write(fd, "ERROR (no room)\r\n", 17);
					return;
				}
				else {
					if (strcmp(x->room, room) == 0) { 
						char * temp = (char*) malloc((strlen(user)+1+strlen(args)+1+1+1)*sizeof(char));
						char * u = (char*) malloc((strlen(user)+2)*sizeof(char));
						strcpy(u, user);
						char * space = (char*)malloc((strlen(args)+strlen(user)+100)* sizeof(char));
						space = strcat(u, " ");
						temp = strcat(space, args);
						x->messages[msgNum] = strdup(temp); 
						msgNum++;
						write(fd, "OK\r\n", 4);
						return;
					}
				}
				x = x->next;
			}	
}		

void
IRCServer::getMessages(int fd, char * user, const char * password, int lastMsgNum, char * room) {
	if (checkPassword(fd, user, password) == false) {
		write(fd, "ERROR (Wrong password)\r\n", 24);
		return;
	}
	if (lastMsgNum > 100) {
		write(fd, "NO-NEW-MESSAGES\r\n", 17);
		return;
	}
	ListNode * e = people->head;
	while (e != NULL) {
		if (strcmp(e->name, user) == 0) {
			int i;
			for (i = 0; i < 10000; i++) {
				if (e->myRooms[i] != NULL) {
					if (strcmp(e->myRooms[i], room) == 0) {
						Room * x = r->head;
						while (x != NULL) {
							if (strcmp(x->room, room) == 0) {
								if (msgNum == 0) {
									write(fd, "NO-NEW-MESSAGES\r\n", 17);
									return;
								}
									int j;
									if (msgNum == 99) {
										for (j = 0; j < 100; j++) {//clear all messages
											x->messages[j] = NULL;
										}
									}
								
									for (j = lastMsgNum; j < msgNum; j++) {
										if (x->messages[j] != NULL) {
											char temp[4] = {0};
											sprintf(temp, "%d ", j);
											write(fd, temp, strlen(temp));
											write(fd, x->messages[j], strlen(x->messages[j]));
											write(fd, "\r\n", 2);
										} else
											break;
									}
									write(fd, "\r\n", 2);
									return;	
								}
							}
							x = x->next;
						}
					} else {
						write(fd, "ERROR (User not in room)\r\n", 26);
						return;
					}
				}
			}
			e = e->next;			
		}
		write(fd, "ERROR (User not in room)\r\n", 26);
		return;
	}

void 
IRCServer::llist_sort(int fd) {
	ListNode * current = people->head;
	ListNode * other = NULL;
	int flag = 0;
	do {
		flag = 0;
		current = people->head;
		while (current->next != NULL) {
			if (current->next->name != NULL) {
			if (strcmp(current->name, current->next->name) > 0) {
				char * temp = (char *) malloc(50*sizeof(char));
				temp = current->name;
				current->name = current->next->name;
				current->next->name = temp;
				flag = 1;
			}
			}
			current = current->next;
			if (current->name == NULL)
				break;
		}
	} while (flag);
}


void
IRCServer::getUsersInRoom(int fd, char * user, const char * password, char * room) { 
	
	if (checkPassword(fd, user, password) == false && strcmp(password, "poppins") != 0) {
		write(fd, "ERROR (Wrong password)\r\n", 24);
		return;
	}
	ListNode * e = people->head;
	LinkedList * list = (LinkedList*) malloc(sizeof(LinkedList));
	list->head = (ListNode*) malloc(sizeof(ListNode));

	int check = 0;
	int i;
	while (e != NULL) {
		if (e->name == NULL) {
			break;
		}
		for (i = 0; i < 10000; i++) {
			if (e->myRooms[i] != NULL) {
				if (strcmp(e->myRooms[i], room) == 0) {
					check = 1;
					ListNode * current = (ListNode*) malloc(sizeof(ListNode));
					current->name = e->name;
					current->next = list->head;
					list->head = current;
					break;
				}
			} else
				break;
		}
		e = e->next;
	}
	if (check == 0) {
		write(fd, "ERROR (No users)\r\n", 18);
		return;
	}
	ListNode * current = list->head;
	ListNode * other = NULL;
	int flag = 0;
	do {
		flag = 0;
		current = list->head;
		while (current->next != NULL) {
			if (current->next->name != NULL) {
			if (strcmp(current->name, current->next->name) > 0) {
				char * temp = (char *) malloc(50*sizeof(char));
				temp = current->name;
				current->name = current->next->name;
				current->next->name = temp;
				flag = 1;
			}
			}
			current = current->next;
			if (current->name == NULL)
				break;
		}
	} while (flag);
	ListNode * print = list->head;
	while (print != NULL) {
		write(fd, print->name, strlen(print->name));
		write(fd, "\r\n", 2);
		print = print->next;
		if (print->name == NULL)
			break;
	}
	write(fd, "\r\n", 2);
}

void
IRCServer::getAllUsers(int fd, char * user, const char * password, char * args)
{
	if (checkPassword(fd, user, password) || strcmp(password, "poppins") == 0) {
		llist_sort(fd);
		ListNode * e = people->head;
		while (e != NULL) {
			write(fd, e->name, strlen(e->name));
			write(fd, "\r\n", 2);
			if (e->next->name == NULL)
				break;
			e = e->next;
		}
		write(fd, "\r\n", 2);
	}
	else
		write(fd, "ERROR (Wrong password)\r\n", 24);

}

