

#ifndef SOURCE_H
#define SOURCE_H

#include "angle.h"
#include "vector2d.h"
#include "interpolat.h"
#include "delaycoeff.h"
#include "twonder_globalconfig.h"

/**
 * @brief The BaseClass for all the sources.
 *
 * A Source can calculate DelayCoeffs in relation to a speaker presented to it.
 * Also because a Source contains some Interpolat type values (e.g a Position),
 * there is also a Method to advance all Interpolats.
 */

class Source {

    public:
	virtual ~Source();

	/**
	 * @brief Get the DelayCoeff for the start of the current Calculations block.
	 *
	 * @param spk The Speaker for which the Coefficient should be calculated.
	 */

	virtual DelayCoeff getDelayCoeff( const Speaker &spk ) = 0;

	/**
	 * @brief Get the DelayCoeff for the end of the current Calculations block.
	 *
	 * @param spk The Speaker for which the Coefficient should be calculated.
	 */
	
	virtual DelayCoeff getTargetDelayCoeff( const Speaker &spk ) = 0;

	/**
	 * @brief Advance the contained Interpolat s.
	 */

	virtual void       applyInterpolatTarget() = 0;
};

/**
 * @brief A Base Class for a Source which has a position.
 *
 * This enables a generic Implementation of a Move Command.
 */

class PositionSource : public Source {
    public:
	Interpolat<Vector2D> pos;
	PositionSource( const Vector2D &p );
};

/**
 * @brief The PointSource creating circular (spherical) waves.
 *
 * This implements DelayCoeff calculation for circular waves.
 */

class PointSource : public PositionSource {
    private:
	/**
	 * @brief The actual Calculation of a Delay Coeff.
	 *
	 * The calculation is done here to have a single implementation for both 
	 * the start and the end of the block.
	 *
	 * @todo The TargetDelayCoeff might be cached for the upcoming block.
	 */

	DelayCoeff calcDelayCoeff( const Speaker &spk, const Vector2D &vec, struct translation &t );

	bool focus_src( Vector2D &src );
    public:
	PointSource( const Vector2D &p ) : PositionSource( p ) { }
	DelayCoeff getDelayCoeff( const Speaker &spk );
	DelayCoeff getTargetDelayCoeff( const Speaker &spk );
	void       applyInterpolatTarget();
	~PointSource();
};

/**
 * @brief The PlaneWave creating Planewaves.
 *
 * This implements DelayCoeff calculation for PlaneWaves.
 */

class PlaneWave : public PositionSource {
    
    private:
	/**
	 * @brief The actual Calculation of a Delay Coeff.
	 *
	 * The calculation is done here to have a single implementation for both 
	 * the start and the end of the block.
	 *
	 * @todo The TargetDelayCoeff might be cached for the upcoming block.
	 */

	DelayCoeff calcDelayCoeff( const Speaker &spk, const Vector2D &vec, const Angle &angle, struct translation &t );

    public:
	Interpolat<Angle> angle; //< A plane Wave has an origonating Position and a direction. The position is inherited from PositionSource.
	PlaneWave( const Vector2D &p, float a ) : PositionSource( p ), angle( a ) { }
	DelayCoeff getDelayCoeff( const Speaker &spk );
	DelayCoeff getTargetDelayCoeff( const Speaker &spk );
	void       applyInterpolatTarget();
	~PlaneWave();
};

#endif // SOURCE_H
