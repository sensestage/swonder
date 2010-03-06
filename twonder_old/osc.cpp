
#include "osc.h"
#include <iostream>

using namespace std;

OSCServer::OSCServer( const char *port ) {
    server_thread = lo_server_thread_new( port, NULL );
    if( !server_thread ) {
	cout << "Warning !!! Server thread bogus" << endl;
    }
}


OSCServer::~OSCServer() {
    lo_server_thread_free( server_thread );
}

void
OSCServer::start() {
    lo_server_thread_start( server_thread );
}

void
OSCServer::stop() {
    lo_server_thread_stop( server_thread );
}

void
OSCServer::add_method( const char *path, const char *typespec, lo_method_handler h, void *user_data ) {
    lo_server_thread_add_method( server_thread, path, typespec, h, user_data );
}
