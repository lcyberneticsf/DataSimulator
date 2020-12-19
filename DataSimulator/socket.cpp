
// Implementation of the Socket class.


#include "socket.h"
#include <iostream>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "../CLog.h"
extern CLog  my_log;
extern int  m_nLogSwitch;
extern int m_nTestMode;


using namespace std;

Socket::Socket() : m_sock ( -1 )
{

    memset ( &m_addr, 0, sizeof ( m_addr ) );

}

Socket::~Socket()
{
    if ( is_valid() )
		::close ( m_sock );
}
bool Socket::close()
{
	if (is_valid())
	{
		::close(m_sock);
		m_sock = -1;
	}
		
}

bool Socket::create()
{
    m_sock = socket ( AF_INET, SOCK_STREAM, 0 );

	if (!is_valid())
	{
		cout << "m_sock = socket ( AF_INET, SOCK_STREAM, 0 ) do with error;" << endl;
		return false;
	}
        


    // TIME_WAIT - argh
    int on = 1;
	if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof (on)) == -1)
	{
		cout << "Creat socket error,now return!" << endl;
		return false;
	}
	cout << "Creat socket succes,m_sock=" << m_sock<<endl;

    return true;

}


bool Socket::createNonBlocking()
{
	if ((m_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		::close(m_sock);
		return false;
	}
	int sockfd;
	int flags;
	//flags = fcntl(m_sock, F_GETFL, 0);
	//fcntl(m_sock, F_SETFL, flags | O_NONBLOCK);   //设置成非阻塞模式；
	//flags = fcntl(m_sock, F_GETFL, 0);                       //获取文件的flags值。

	//fcntl(m_sock, F_SETFL, flags | O_NONBLOCK);   //设置成非阻塞模式；

	//flags = fcntl(m_sock, F_GETFL, 0);

	//fcntl(m_sock, F_SETFL, flags&~O_NONBLOCK);    //设置成阻塞模式；
	fcntl(m_sock, F_SETFL, fcntl(m_sock, F_GETFL, 0) | O_NONBLOCK);//设置为非阻塞模式  

	if (!is_valid())
		return false;

	// TIME_WAIT - argh
	int on = 1;
	if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof (on)) == -1)
		return false;


	return true;

}



bool Socket::bind ( const int port )
{

    if ( ! is_valid() )
    {
        return false;
    }



    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = INADDR_ANY;
    m_addr.sin_port = htons ( port );

    int bind_return = ::bind ( m_sock, ( struct sockaddr * ) &m_addr, sizeof ( m_addr ) );


    if ( bind_return == -1 )
    {
        return false;
    }

    return true;
}


bool Socket::listen() const
{
    if ( ! is_valid() )
    {
        return false;
    }

    int listen_return = ::listen ( m_sock, MAXCONNECTIONS );


    if ( listen_return == -1 )
    {
        return false;
    }

    return true;
}


bool Socket::accept ( Socket& new_socket ) const
{
    int addr_length = sizeof ( m_addr );
    new_socket.m_sock = ::accept ( m_sock, ( sockaddr * ) &m_addr, ( socklen_t * ) &addr_length );

    if ( new_socket.m_sock <= 0 )
        return false;
    else
        return true;
}
void Socket::print_hex(unsigned char *_buf, int _len)
{
    for (int i = 0 ; i < _len ; i++)
    {
        printf("%02x ", _buf[i]);
        if(i>1000)  break;
    }
    std::cout << std::endl;
}

bool Socket::send (std::string s ) const
{
	int nsize=s.size();
	char buf[64];
	memset(buf,0,64);
	strncpy(buf,s.data(),nsize);
	//std::cout<<"in socket::send "<<s.size()<<" data,send message:"<<std::endl;
	//print_hex((unsigned char*)(s.data()),s.size());

	string ss=s;
   // s.data();
    //int status = ::send ( m_sock, s.c_str(), s.size(), MSG_NOSIGNAL );
	int status = ::send ( m_sock, s.data(), s.size(), MSG_NOSIGNAL );
    if ( status == -1 )
    {
        return false;
    }
    else
    {
        return true;
    }
}




int Socket::recv ( std::string& s ) const
{
    char buf [ MAXRECV + 1 ];


    memset ( buf, 0, MAXRECV + 1 );
	
    int status = ::recv ( m_sock, buf, MAXRECV, 0 );

    if ( status == -1 )
    {
        cout << "status == -1   errno == " << errno << "  in Socket::recv\n";
        return 0;
    }
    else if ( status == 0 )
    {
        return 0;
    }
    else
    {
        s = buf;
        return status;
    }
}

int Socket::recv ( std::string& s ,int size) const
{
    char buf [ size + 1 ];
	char chFile[512];
	int status=-2;

    memset ( buf, 0, size + 1 );

	
	if (m_nLogSwitch == 1)
	{
		sprintf(chFile, "before ::recv ( m_sock, buf, MAXRECV, 0 ),  status=%d", status);
		my_log.Add(chFile);
	}
    status = ::recv ( m_sock, buf, size, 0 );
	
	if (m_nLogSwitch == 1)
	{
		sprintf(chFile, "end ::recv ( m_sock, buf, MAXRECV, 0 ),  status=%d", status);
		my_log.Add(chFile);
	}

    if ( status == -1 )
    {
        cout << "status == -1   errno == " << errno << "  in Socket::recv\n";
        return 0;
    }
    else if ( status == 0 )
    {
        return 0;
    }
    else
    {
        s = buf;
        return status;
    }
}


int Socket::recv ( char& s ,int size) const
{
   // char buf [ size + 1 ];

   // memset ( buf, 0, size + 1 );

    int status = ::recv ( m_sock, &s, size, 0 );

    if ( status == -1 )
    {
        cout << "status == -1   errno == " << errno << "  in Socket::recv\n";
        return 0;
    }
    else if ( status == 0 )
    {
        return 0;
    }
    else
    {
        return status;
    }
}
int Socket::recv_non_blocking(char& s, int size) const
{
	// char buf [ size + 1 ];

	// memset ( buf, 0, size + 1 );
	//cout << "recv_non_blocking:283" << endl;
	int status = ::recv(m_sock, &s, size, MSG_DONTWAIT);
	//cout << "recv_non_blocking:285,status=" << status<<endl;

	if (status == -1)
	{
		if (m_nTestMode==1)
		cout << "status == -1   errno == " << errno << "  in Socket::recv\n";
		return status;
	}
	else if (status == 0)
	{
		return 0;
	}
	else
	{
		return status;
	}
}
bool Socket::connect ( const std::string host, const int port )
{
    if ( ! is_valid() ) return false;
	char chFile[512];

    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons ( port );
    std::cout << "connected " << host << ":" << port << std::endl;
	
	if (m_nLogSwitch == 1)
	{
		sprintf(chFile, "connected:%s:%d", host.c_str(), port);
		my_log.Add(chFile);
	}

    int status = inet_pton ( AF_INET, host.c_str(), &m_addr.sin_addr );


    if ( errno == EAFNOSUPPORT ) return false;
    cout<<"begin:m_sock="<<m_sock<<endl;
	
	if (m_nLogSwitch == 1)
	{
		sprintf(chFile, "begin:m_sock=%d,status=%d", m_sock, status);
		my_log.Add(chFile);
	}
    status = ::connect ( m_sock, ( sockaddr * ) &m_addr, sizeof ( m_addr ) );
	cout << "end:m_sock=" << m_sock << endl;
	
	if (m_nLogSwitch == 1)
	{
		sprintf(chFile, "end:m_sock=%d,status=%d", m_sock, status);
		my_log.Add(chFile);
	}
    cout<<"status="<<status<<endl;

    if ( status == 0 )
        return true;
    else
        return false;
}

void Socket::set_non_blocking ( const bool b )
{

    int opts;

    opts = fcntl ( m_sock,
                   F_GETFL );

    if ( opts < 0 )
    {
        return;
    }

    if ( b )
        opts = ( opts | O_NONBLOCK );
    else
        opts = ( opts & ~O_NONBLOCK );

    fcntl ( m_sock,
            F_SETFL,opts );

}
