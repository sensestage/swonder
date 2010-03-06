
#ifndef OSC_H
#define OSC_H

#include "lo/lo.h"

/**
 * @brief Class to create and control osc-server-thread.
 */

class OSCServer {

    private:
	lo_server_thread server_thread;

    public:
	OSCServer( const char *port = "58200" );
	~OSCServer();
	void add_method( const char *path, const char *typespec, lo_method_handler h, void *user_data = NULL );
	void start();
	void stop();
};


#endif
