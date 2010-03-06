
#ifndef RTCOMMAND_ENGINE_H
#define RTCOMMAND_ENGINE_H

#include "commandqueue_ts.h"

/**
 * @brief RTCommandEngine to run commands in the realtime thread.
 *
 * RTCommandEngine works as a asynchronous bridge the realtime thread (inside jackd context) and   
 * other threads (e.g. osc server thread).
 */
class RTCommandEngine {
    private:
	CommandQueue cmd_queue;
	FreeQueue    free_queue;

	WrappedQCommand accumulated_cmds;
	WrappedQCommand retval_list;

	WrappedQCommand *free_holder;

	void merge_incoming_cmds();

	WrappedQCommand *get_due_cmds( TimeStamp ts );
	
	void put_cmd_list( WrappedQCommand *cmds );

	static void process_command( QCommand *cmd );

    public:
	RTCommandEngine();

	~RTCommandEngine();

/**
 * @brief Add a command to CommandQueue to execute in realtime thread.
 *
 */
	void put( QCommand *cmd );
	
/**
 * @brief Execute commands from CommandQueue inside realtime thread.
 *
 */
	void eval_cmds( TimeStamp ts );
	
};

#endif //RTCOMMAND_ENGINE_H
