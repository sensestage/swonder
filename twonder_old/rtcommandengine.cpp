
#include "rtcommandengine.h"
#include <algorithm>

/**
 * Create RTCommandEngine
 */

RTCommandEngine::RTCommandEngine()
    : cmd_queue(), free_queue(), accumulated_cmds(){

	free_holder = NULL;
}

RTCommandEngine::~RTCommandEngine() {
    if( free_holder )
	delete free_holder;
}


/// 
/// Send a QCommand to the Realtime Process.
/// You must not delete the QCommand. This is done via the free_queue.
void
RTCommandEngine::put( QCommand *cmd ) {
    cmd_queue.put( cmd );
    
    // delete objects that came back from eval_cmds (rt-thread)
    free_queue.flush();
}

void 
RTCommandEngine::merge_incoming_cmds() {
    WrappedQCommand *tmp;

    while( (tmp = cmd_queue.get()) != NULL ) {
	accumulated_cmds.merge( *tmp );

	if( free_holder == NULL )
	    free_holder = tmp;
	else
	    if( free_holder->empty() ) {
		free_queue.put( tmp );
	    } else {
		free_queue.put( free_holder );
		free_holder = tmp;
	    }

    }
}

WrappedQCommand *
RTCommandEngine::get_due_cmds( TimeStamp ts ) {
    if( (! accumulated_cmds.empty()) && (free_holder != NULL) ) {

	WrappedQCommand::iterator it = accumulated_cmds.begin();
	while( it != accumulated_cmds.end() && ((*it)->stamp < ts) ) {
	    it++;
	}
	if( it != accumulated_cmds.begin() ) {
	    retval_list.splice( retval_list.end(), accumulated_cmds, accumulated_cmds.begin(), it );
	    return &retval_list;
	} else {
	    return NULL;
	}
    } else {
	return NULL;
    }
}

void
RTCommandEngine::put_cmd_list( WrappedQCommand *cmds ) {
    free_holder->splice( free_holder->end(), *cmds, cmds->begin(), cmds->end() );
}

void
RTCommandEngine::process_command( QCommand *cmd ) {

	cmd->execute();

}

/**
 * Pass all QCommands which are due to evaluation / execution.
 *
 * The QCommandEvaluator must not delete the Commands.
 * But it may remove pointed objects from the QCommand.
 *
 * So if you want to Exchange an object in the RTThread, then
 * you can take a pointer from the QCommand and put the pointer to
 * the old Datastructure into the QCommand. This way the old structure
 * will get deleted, and the new structure will be managed by the RT-Thread.
 *
 */
void
RTCommandEngine::eval_cmds( TimeStamp ts ) {

    merge_incoming_cmds();

    WrappedQCommand *cmds = get_due_cmds( ts );

    if( cmds ) {
	std::for_each( cmds->begin(), cmds->end(), process_command );
	put_cmd_list( cmds );
    }
}
