
#ifndef IRC_SERVER
#define IRC_SERVER

#define PASSWORD_FILE "password.txt"

class IRCServer {
	// Add any variables you need

private:
	int open_server_socket(int port);

public:
	void initialize();
	bool checkPassword(int fd, char * user, const char * password);
	void processRequest( int socket );
	void addUser(int fd, char * user, const char * password, char * args);
	void createRoom(int fd, char * user, const char * password, char * room);
	void listRooms(int fd, char * user, const char * password, char * args);
	void initRooms();
	void enterRoom(int fd, char * user, const char * password, char * room);
	void leaveRoom(int fd, char * user, const char * password, char * room);
	void sendMessage(int fd, char * user, const char * password, char * room, char * args);
	void getMessages(int fd, char * user, const char * password, int lastMsgNum, char * room);
	void getUsersInRoom(int fd, char * user, const char * password, char * room);
	void llist_sort(int fd);
	void getAllUsers(int fd, char * user, const char * password, char * args);
	void runServer(int port);
};

#endif
