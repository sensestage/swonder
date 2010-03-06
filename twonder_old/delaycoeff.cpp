
#include "delaycoeff.h"

DelayCoeff::DelayCoeff( Speaker &spk, PointSource &src ) {

    Vector2D diff = spk.getPos() - src.getPos();
    float cosphi = diff * spk.getN() / diff.len();
    //cout << src.getPos()[0] << "  " << src.getPos()[1] << endl;
    //cout << cosphi << "  " << diff[0] << "  " << diff[1] << endl;

    // XXX: check whether before or behind speaker
    //      1. if( cosphi .... )
    //      2. define other len function
    //         then check whether negative... (???)
    
    delay = diff.len();
    
    
    // XXX: good for 1st test.... adjust to swonder soon.
    if( delay == -deltaR )
	delay += 0.01;

    //cout << cosphi << "diff... " << diff[0] << " " << diff[1];
    // XXX: This is for refenceline at position deltaR
    factor = sqrt( deltaR / ( delay + deltaR ) ) * cosphi / sqrt( delay );

    //print();
}

DelayCoeff::DelayCoeff( Speaker &spk, const Vector2D &src ) {

    Vector2D diff = spk.getPos() - src;
    float cosphi = diff * spk.getN() / diff.len();

    // XXX: check whether before or behind speaker
    //      1. if( cosphi .... )
    //      2. define other len function
    //         then check whether negative... (???)
    
    delay = diff.len();
    
    
    // XXX: good for 1st test.... adjust to swonder soon.
    if( delay == -deltaR )
	delay += 0.01;

    //cout << cosphi << "diff... " << diff[0] << " " << diff[1];
    // XXX: This is for refenceline at position deltaR
    factor = sqrt( deltaR / ( delay + deltaR ) ) * cosphi / sqrt( delay );

    //print();
}

DelayCoeff::DelayCoeff( Speaker &spk, PlaneWave &src ) {

    Vector2D diff = spk.getPos() - src.getPos();
    float cosphi = spk.getN() * src.getN();
    delay = diff.len() * cosphi;

    // XXX: multiply with volume factor. (is that part of speaker ? probalbly yes)
    factor = cosphi;
}
