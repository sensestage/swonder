
#ifndef COMMANDQUEUE_H
#define COMMANDQUEUE_H

#include <unistd.h>

#include <list>

#include "jackppringbuffer.h"
#include "pthread.h"

/**
 * @brief Wrapper to encapsulate timestamps.
 */
class TimeStamp {
// TODO: protect for wrap around.
    public:
	unsigned int ts;
	TimeStamp() : ts( 0 ) { }
	TimeStamp( unsigned int t ) : ts( t ) { }
	operator unsigned int() const { return ts; }
	operator float () const { return (float)ts; }
	bool operator< ( const TimeStamp &other ) const { return ts < other.ts; }
	bool operator== ( const int &other ) const { return ts==other; }
	TimeStamp &operator-= ( int i ) { ts -= i; }
};

/**
 * @brief Combines Commands for rt-execution with TimeStamps
 */
class QCommand {
    public:
	TimeStamp stamp;
	virtual ~QCommand();

  /// QCommand::execute() is called in rt-thread so it must not use: malloc(), new, delete in its 
  /// implementation.
	virtual void execute();

	QCommand() : stamp( 0 ) {}
	QCommand( TimeStamp ts ) : stamp( ts ) {}

  ///
  /// Makes QCommands sortable by TimeStamps
	bool operator< ( const QCommand & other ) { return stamp < other.stamp; }
};

/**
 * @brief Wraps QCommands in a list.
 */
class WrappedQCommand : public std::list<QCommand *> {
    public:
	WrappedQCommand( ) : std::list<QCommand *>() { }

	WrappedQCommand( QCommand *cmd ) : std::list<QCommand *>() {
	    push_back( cmd );
	}

	~WrappedQCommand() {
	    while( !empty() ) {
		delete front();
		pop_front();
	    }
	}
};

/**
 * @brief CommandQueue stores QCommands threadsafe in a JackRingbuffer.
 */
class CommandQueue : private JackRingbuffer {
    private:
	pthread_mutex_t mutex;

    public:
	CommandQueue() {
	    pthread_mutex_init( &mutex, NULL );
	}

	~CommandQueue() {
	    pthread_mutex_destroy( &mutex );
	}

    public:
	void put( QCommand *cmd ) {
	    WrappedQCommand *wrapped_cmd = new WrappedQCommand( cmd );

      // use mutex for thread safe writing in  JackRingbuffer
	    pthread_mutex_lock( &mutex );
	    while( !write( reinterpret_cast<void *>(wrapped_cmd) ) ) usleep( 100 );
	    pthread_mutex_unlock( &mutex );
	}
	    
	WrappedQCommand *get() {
	    return reinterpret_cast<WrappedQCommand *>( read() );
	}
};

/**
 * @brief  FreeQueue stores/deletes WrappedQCommands threadsafe in a JackRingbuffer.
 *
 * The FreeQueue is used  to hold and delete used Objects. Deletion is done in non-realtime.  
 */
class FreeQueue : private JackRingbuffer {
    private:
	pthread_mutex_t mutex;

    public:
	FreeQueue() {
	    pthread_mutex_init( &mutex, NULL );
	}

	~FreeQueue() {
	    pthread_mutex_destroy( &mutex );
	}

  /// 
 /// Puts WrappedQCommands onto the FreeQueue (in rt-thread) for later deletetion (in non-rt-thread).. 
	void put( WrappedQCommand *wrapped_cmd ) {
	    // This tries to put the object onto the free Queue.
	    // if the queue is full, then the object will not get freed
	    // producing a memory leak.
	    //
	    // XXX: for safety reasons its better to free the object in that case.
	    //      a click (x-run) is better than the whole system breaking down due to
	    //      memory shortage ;)

	    if( !write( reinterpret_cast<void *>( wrapped_cmd ) ) ) {
		delete wrapped_cmd;
	    }
	}

///
/// Deletes used WrappedQCommands in non-realtime thread.
	void flush() {
	    WrappedQCommand *tmp;
	    pthread_mutex_lock( &mutex );
	    while( (tmp = reinterpret_cast<WrappedQCommand *>( read() ) ) != NULL )
		delete tmp;
	    pthread_mutex_unlock( &mutex );
	}
};


#endif
