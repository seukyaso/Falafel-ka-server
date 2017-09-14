#include "cfalafelserver.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <string.h>

#include <thread>

class CFalafelServer
{
protected:

    CFalafelServer()
    {
        m_sock_fd = 0;
        m_running = true;
    }

public:

    virtual ~CFalafelServer()
    {
        m_running = false;

        if( m_thread.joinable() )
            m_thread.join();
    }

    static CFalafelServer* bindServer( const char* port );

private:

    bool bind_port( const char* port );

    void serverRoutine();

private:

    int m_sock_fd;
    bool m_running;
    std::string m_used_port;
    std::thread m_thread;
};

CFalafelServerFactory CFalafelServerFactory::m_instance;

typedef std::lock_guard<std::mutex> TAutoLock;

CFalafelServerFactory::CFalafelServerFactory()
{

}

CFalafelServerFactory::~CFalafelServerFactory()
{
    TAutoLock lock( mMutex );

    std::map<std::string, CFalafelServer*>::iterator it = mRunningInstancesSet.begin();

    for( ; it != mRunningInstancesSet.end(); ++it )
        delete it->second;
}

bool CFalafelServerFactory::startServerImpl(const char *port)
{
    // E.M. invalid parameters
    if( port == NULL || port[0] == 0 )
        return false;

    TAutoLock lock( mMutex );

    // E.M. already running this instance
    if( mRunningInstancesSet.find( port ) != mRunningInstancesSet.end())
        return false;

    CFalafelServer* p_new_server = CFalafelServer::bindServer( port );

    if( p_new_server )
        mRunningInstancesSet[port] = p_new_server;

    return p_new_server != NULL;
}

bool CFalafelServerFactory::stopServerImpl(const char *port)
{
    // E.M. invalid parameters
    if( port == NULL || port[0] == 0 )
        return false;

    TAutoLock lock( mMutex );

    std::map<std::string, CFalafelServer*>::iterator it = mRunningInstancesSet.find( port );

    // E.M. there is no such running instance
    if( it == mRunningInstancesSet.end())
        return false;

    delete it->second;
    mRunningInstancesSet.erase(it);
}

std::vector<std::string> CFalafelServerFactory::getRunningInstancesImpl()
{
    std::vector<std::string> ret;

    TAutoLock lock( mMutex );

    std::map<std::string, CFalafelServer*>::const_iterator it = mRunningInstancesSet.begin();

    for( ; it != mRunningInstancesSet.end(); ++it )
        ret.push_back( it->first );

    return ret;
}

CFalafelServer *CFalafelServer::bindServer(const char *port)
{
    CFalafelServer* ret = new CFalafelServer();

    if( ret )
    {
        if( ret->bind_port(port) == false )
        {
            delete ret;
            ret = NULL;
        }
    }

    return ret;
}

bool CFalafelServer::bind_port(const char *port)
{
    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo;  // will point to the results

    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_INET;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    if ((status = getaddrinfo(NULL, port, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return false;
    }


    // make a socket:
    m_sock_fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

    if( m_sock_fd == -1 )
    {
        freeaddrinfo(servinfo);
        return false;
    }

    // bind it to the port we passed in to getaddrinfo():
    if( bind(m_sock_fd, servinfo->ai_addr, servinfo->ai_addrlen) != 0 )
    {
        freeaddrinfo(servinfo);
        return false;
    }

    freeaddrinfo(servinfo); // free the linked-list

    m_used_port = port;
    m_thread = std::thread( &CFalafelServer::serverRoutine, this );

    return true;
}

void CFalafelServer::serverRoutine()
{
    printf( "Bound to port: %s\n", m_used_port.c_str());

    while( m_running )
    {
        std::this_thread::sleep_for( std::chrono::seconds(1));
    }

    printf( "Server thread exited: %s\n", m_used_port.c_str());
}
