

#ifndef INTERPOLAT_H
#define INTERPOLAT_H

//typedef int TimeStamp;

#include "commandqueue_ts.h"
 /**
 * @brief Template Class to interpolate interim source positions.
 */
template< class T >
class Interpolat {

    private:
	T cur_val;
	T target_val;
	T block_target_val;


    public:
	TimeStamp target_time;
	Interpolat( const T &val ) :  cur_val( val ), target_val( val ), block_target_val( val ), target_time( 0 ) { }

	const T &getVal() const { return cur_val; }
	
  ///
  /// @brief Get source position at the end of current sample block.
  const T getTargetVal() const {
	    if( target_time == 0 )
		return target_val;
	    else {
		// XXX: time unit is blocks.
		return cur_val + (target_val - cur_val) * ((float)1.0/(float) target_time);
	    }
	}

 ///
 /// @brief Set current source to the position it should be at the end of current sample block.  
	void applyTargetVal() {
	    if( target_time == 0 )
		cur_val = target_val;
	    else {
		cur_val = cur_val + (target_val - cur_val) * ((float)1.0/(float) target_time);
		target_time -= 1;
	    }
	}
	    
  ///
  /// @brief Sets new target position and time.
	void setVal( const T &val, TimeStamp time = 0 ) {
	    target_val = val;
	    target_time = time;
	}

};

#endif // INTERPOLAT_H
