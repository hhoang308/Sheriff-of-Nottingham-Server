#ifndef SERVER_H
#define SERVER_H


/* TODO: add server port and server address here */
#define SERVER_ADDRESS 
#define SERVER_PORT 

enum ServerState {
    SERVER_WAITING,
    SERVER_READY,
    SERVER_IN_PROGRESS,
    SERVER_INVALID_STATE
};

class Server {
public:
    bool init();
    void run();
    ~Server();

private:
    
};

#endif // SERVER_H