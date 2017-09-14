#include <iostream>
#include  <signal.h>
#include <unistd.h>

#include "src/cfalafelserver.h"

void singnal_handler(int )
{
    exit(0);
}

int main()
{
    signal(SIGINT, singnal_handler);

    bool started1 = CFalafelServerFactory::startServer( "4444" );
    bool started2 = CFalafelServerFactory::startServer( "5555" );

    bool stopped1 =CFalafelServerFactory::stopServer( "4444");

    while (1)
         pause();

    return 0;
}
