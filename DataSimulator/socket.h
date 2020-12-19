
// Definition of the Socket class

#ifndef SOCKET_H
#define SOCKET_H


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>


const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 5;
const int MAXRECV = 500;

class Socket
{
public:
    Socket();
    virtual ~Socket();

    // Server initialization
    bool create();
	bool createNonBlocking();
    bool bind ( const int port );
    bool listen() const;
    bool accept ( Socket& ) const;
	bool close();

    // Client initialization
    bool connect ( const std::string host, const int port );

    // Data Transimission
    //bool send ( const std::string ) const;
    bool send (std::string s) const;
    int recv ( std::string& ) const;
    int recv ( std::string& ,int size) const;
    int recv ( char& ,int size) const;
    static void print_hex(unsigned char *_buf,int _len);

	int recv_non_blocking(char&, int size) const;


    void set_non_blocking ( const bool );

    bool is_valid() const
    {
        return m_sock != -1;
    }

private:

    int m_sock;
    sockaddr_in m_addr;


};


#endif
