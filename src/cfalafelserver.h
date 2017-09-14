#ifndef CFALAFELSERVER_H
#define CFALAFELSERVER_H

#include <vector>
#include <string>
#include <mutex>
#include <map>

class CFalafelServer;

class CFalafelServerFactory
{
protected:

    CFalafelServerFactory();

public:

    virtual ~CFalafelServerFactory();

    static bool startServer( const char* port )
    {
        return m_instance.startServerImpl( port );
    }

    static bool stopServer(  const char* port )
    {
        return m_instance.stopServerImpl( port );
    }

    static std::vector<std::string> getRunningInstances()
    {
        return m_instance.getRunningInstancesImpl();
    }

protected:

    void stop();

private:

    bool startServerImpl( const char* port );

    bool stopServerImpl(  const char* port );

    std::vector<std::string> getRunningInstancesImpl();

private:

    static CFalafelServerFactory m_instance;

    std::mutex mMutex;
    std::map<std::string, CFalafelServer*> mRunningInstancesSet;
};

#endif // CFALAFELSERVER_H
