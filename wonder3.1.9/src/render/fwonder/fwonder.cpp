/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *                                                                                   *
 *  WONDER - Wave field synthesis Of New Dimensions of Electronic music in Realtime  *
 *  http://swonder.sourceforge.net                                                   *
 *                                                                                   *
 *                                                                                   *
 *  Technische Universität Berlin, Germany                                           *
 *  Audio Communication Group                                                        *
 *  www.ak.tu-berlin.de                                                              *
 *  Copyright 2006-2008                                                              *
 *                                                                                   *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or modify             *
 *  it under the terms of the GNU General Public License as published by             *
 *  the Free Software Foundation; either version 2 of the License, or                *
 *  (at your option) any later version.                                              *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.       *
 *                                                                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>

#include <fenv.h>
#include <csignal>

#include "fwonder_config.h"
#include "rtcommandengine.h"
#include "impulseresponse.h"
#include "sourcearray.h"
#include "outputarray.h"
#include "irmatrixmanager.h"
#include "delayline.h"
#include "complex_mul.h"
#include "complexdelayline.h"

#include "jackppclient.h"
#include "oscin.h"


// global data 
jackpp::Client*  jackClient;
OutputArray*     outputArray;
SourceArray*     sourceArray;
RTCommandEngine* realtimeCommandEngine;
OSCServer*       oscServer;

pthread_t managerThread;

bool stopIRCacheManager = false;



//------------------------------SetNewIRCommand-------------------------------//

class SetNewIRCommand : public Command 
{

public:
    SetNewIRCommand( int sourceID, ImpulseResponse* newIR ) : sourceID( sourceID ), impulseResponse( newIR )
    {
    }
    void execute();

private:
    int sourceID;
    ImpulseResponse* impulseResponse;
};


void SetNewIRCommand::execute()
{
    // set the new impulse response of the source with the given ID.
    sourceArray->at( sourceID )->newIR = impulseResponse;

    // remove the old impulse response (if present) of this source.
    if( sourceArray->at( sourceID )->oldIR )
    {
        sourceArray->at( sourceID )->oldIR = NULL;
    }
}

//-------------------------end of SetNewIRCommand-----------------------------//


//----------------------------------OSC-handler-------------------------------//

// arguments of the handler functions
#define handlerArgs const char* path, const char* types, lo_arg** argv, int argc, lo_message msg, void* user_data


//XXX: I don't know if this works the way it should, there is no update request to the IRMatrix
int oscResolutionHandler( handlerArgs ) 
{
    std::string strPath = path;
    if( strPath == "/WONDER/fwonder/resolution/x" )
    {
        if( fwonderConf->oscResolutionVerbose )
            cout << endl << "[fWonder::oscXResolutionHandler]: " << oscServer->getContent( path, types, argv, argc ) << endl;

        int newXResolution = argv[ 0 ]->i;
        if( fwonderConf->resolutionChangeable && newXResolution > 0)
        {
            fwonderConf->dynamicCacheXResolution = newXResolution;

            if ( fwonderConf->oscResolutionVerbose)
                cout << "new x-resolution: " << fwonderConf->dynamicCacheXResolution << endl;
        }
    }
    else if( strPath == "/WONDER/fwonder/resolution/y" ) 
    {
        if( fwonderConf->oscResolutionVerbose )
            cout << endl << "[fWonder::oscYResolutionHandler]: " << oscServer->getContent( path, types, argv, argc ) << endl;

        int newYResolution = argv[ 0 ]->i;

        if( fwonderConf->resolutionChangeable && newYResolution > 0 )
        {
            fwonderConf->dynamicCacheYResolution = newYResolution;

            if ( fwonderConf->oscResolutionVerbose ) 
                cout << "new y-resolution: " << fwonderConf->dynamicCacheYResolution << endl;
        }
    }
  
    return 0;
}


int oscMoveHandler( handlerArgs )
{
    if( fwonderConf->oscMoveVerbose )
        cout << endl << "[fWonder::oscMoveHandler]: " << oscServer->getContent( path, types, argv, argc ) << endl;

    SourceArray::iterator sourceIt;
    std::string strPath = path;

    int i = 0;
    for( sourceIt = sourceArray->begin(); sourceIt != sourceArray->end(); ++sourceIt )
    {
        SourceAggregate& source = **sourceIt;

	if( strPath == "/WONDER/tracker/move" ) // argv[ 0 ]->i is sourceID, which is currently not used
            source.IRManager->setPanAndTilt( argv[ 1 ]->f, argv[ 2 ]->f );
        else if( strPath == "/WONDER/tracker/move/pan" )
            source.IRManager->setPan ( argv[ 0 ]->f );
        else if( strPath == "/WONDER/tracker/move/tilt" )
            source.IRManager->setTilt( argv[ 0 ]->f );
        // rot value is currently not evaluated

        ImpulseResponse* fittingIR = source.IRManager->getBestIR();

        if( fittingIR && ( fittingIR != source.scheduledIR ) ) 
        {
            source.scheduledIR = fittingIR;
            realtimeCommandEngine->put( new SetNewIRCommand( i, fittingIR ) );
        } 
        ++i;
    }   
    return 0;
}


int oscGenericHandler( handlerArgs )
{
    cerr << endl << "[fwonder]: received unknown osc message: " << path << endl;
    return 0;
}

//------------------------------end of OSC-Handler---------------------------//


//--------------------------------IR management------------------------------//


void* manageIRCache( void* arg )
{
    SourceArray::iterator sourceIt;

    while( ! stopIRCacheManager )
    {
	bool notAllDone = false;
	for( sourceIt = sourceArray->begin(); sourceIt != sourceArray->end(); ++sourceIt )
        {
	    notAllDone |= (*sourceIt)->IRManager->manage();
        }
	if( ! notAllDone )
        {
	    usleep( 100000 );
	}
    }

     return 0;
}

//----------------------------end of IR management---------------------------//


//-------------------------------Audio processing----------------------------// 

int process( jack_nframes_t nframes, void* arg ) 
{
    // XXX: should we start at 0 or at 1 ??? it was 0, but I guess 1 is right
    static int processingBlockCounter = 1;

    SourceArray::iterator sourceIt;

    realtimeCommandEngine->evaluateCommands( ( wonder_frames_t ) 15 );

    // Get input samples from JACK, put the data into the delayline.
    // fill, zeropad and fft the padding buffer
    for( sourceIt = sourceArray->begin(); sourceIt != sourceArray->end(); ++sourceIt )
    {
        SourceAggregate* source = *sourceIt;

        source->inputLine->put( ( float* ) source->inputPort->getBuffer( nframes ), nframes );
        source->inputLine->get( source->paddingBuffer->getSamples(), nframes );

        source->paddingBuffer->zeroPad2ndHalf();

        source->paddingBuffer->fft();
    }

    // XXX: is this correct?
    // read samples into tailBuffer, but only when enough samples for whole tail partitionhave have been read
    if( fwonderConf->useTail  &&  ( processingBlockCounter % ( fwonderConf->tailPartitionSize / nframes ) ) == 0 )
    {
        for( sourceIt = sourceArray->begin(); sourceIt != sourceArray->end(); ++sourceIt )
        {
            SourceAggregate* source = *sourceIt;

            source->inputLine->get( source->tailPaddingBuffer->getSamples(), fwonderConf->tailPartitionSize );

            source->tailPaddingBuffer->zeroPad2ndHalf();
            source->tailPaddingBuffer->fft();
        }
    }

    
    // Do the multiplication in the frequence domain depending on which IR should be used
    // The transformed (fft) buffer of each source is multiplied with every partition of the IR.
    // 
    //	     XXX: I need a structure that maps the correct IRs.
    //	     something like HashMap< pair<Source, Output>, ImpulseResponseChannel >
    //
    //	     for now:
    //	     every source has a current IR and the channels correspond to the outputs.
    for( sourceIt = sourceArray->begin(); sourceIt != sourceArray->end(); ++sourceIt )
    {
        SourceAggregate* source = *sourceIt;

        if(      fwonderConf->doCrossfades 
            &&     source->crossfadeInbetweenIR 
            && ! ( source->crossfadeInbetweenIR->killed ) 
            &&   ( source->crossfadeInbetweenIR->getNoChannels() == 2 ) )
        {
            ImpulseResponse* thisIR = source->crossfadeInbetweenIR;

            for( int i = 0; i < thisIR->getNoChannels(); ++i )
            {
                ImpulseResponseChannel& impulseChannel = thisIR->getChannel( i );
        	ComplexDelayLine&       outputLine     = *( outputArray->at( i )->fadeIn2ComplexLine );

        	for( int j = 0; j < thisIR->getNoPartitions(); ++j )
                {
        	    FftwBuf* IRPartBuf    = impulseChannel [ j ];
        	    FftwBuf* outputBuffer = outputLine     [ j ];
        	    complex_mul( source->paddingBuffer->getSamples(), IRPartBuf->getSamples(), outputBuffer->getSamples(),
                                 source->paddingBuffer->getSSESize() ); 
        	}
            }

            thisIR = source->currentIR;

            if( thisIR && ( thisIR->getNoChannels() == 2 ) && ! ( thisIR->killed ) )
            {
                for( int i = 0; i < thisIR->getNoChannels(); ++i )
                {
                    ImpulseResponseChannel& impulseChannel = thisIR->getChannel( i );
                    ComplexDelayLine&       outputLine     = *( outputArray->at( i )->fadeOut2ComplexLine );

        	    for( int j = 0; j < thisIR->getNoPartitions(); ++j )
                    {
        		FftwBuf* IRPartBuffer = impulseChannel[ j ];
        		FftwBuf* outputBuffer = outputLine    [ j ];
        		complex_mul( source->paddingBuffer->getSamples(), IRPartBuffer->getSamples(), outputBuffer->getSamples(),
                                     source->paddingBuffer->getSSESize() ); 
        	    }
        	}
            }

            source->oldIR                = source->currentIR;
            source->currentIR            = source->crossfadeInbetweenIR;
            source->crossfadeInbetweenIR = NULL;
          
        } // end of source->crossfadeInbetweenIR
        else if( source->newIR )
        {
            ImpulseResponse* thisIR = source->newIR;

            if( thisIR && ! ( thisIR->killed ) && ( thisIR->getNoChannels() == 2 ) )
            { 
                for( int i = 0; i < thisIR->getNoChannels(); ++i )
                {
                    ImpulseResponseChannel& impulseChannel = thisIR->getChannel( i );
                    ComplexDelayLine&       outLine        = *( outputArray->at( i )->fadeInComplexLine );

                    for( int j = 0; j < thisIR->getNoPartitions(); ++j ) 
                    {
                        FftwBuf* IRPartBuffer = impulseChannel[ j ];
                        FftwBuf* outputBuffer = outLine       [ j ];
                        complex_mul( source->paddingBuffer->getSamples(), IRPartBuffer->getSamples(), outputBuffer->getSamples(),
                                     source->paddingBuffer->getSSESize() ); 
                    }
                }
            }

            thisIR = source->currentIR;

            if( thisIR && ! ( thisIR->killed ) && ( thisIR->getNoChannels() == 2 ) ) 
            {
                for( int i = 0; i < thisIR->getNoChannels(); ++i )
                {
                    ImpulseResponseChannel& impulseChannel = thisIR->getChannel( i );
                    ComplexDelayLine&       outLine     = *( outputArray->at( i )->fadeOutComplexLine );

                    for( int j = 0; j < thisIR->getNoPartitions(); ++j )
                    {
                	FftwBuf* IRPartBuffer = impulseChannel[ j ];
                	FftwBuf* outputBuffer = outLine       [ j ];
                	complex_mul( source->paddingBuffer->getSamples(), IRPartBuffer->getSamples(), outputBuffer->getSamples(),
                                     source->paddingBuffer->getSSESize() ); 
                    }
                }
            }

            source->crossfadeInbetweenIR = source->newIR;
            source->newIR = NULL;
        } // end of source->newIR
        else // no newIR nor crossfadeInbetweenIR
        {
            ImpulseResponse* thisIR = source->currentIR;

            if( thisIR &&  ! ( thisIR->killed ) && ( thisIR->getNoChannels() == 2 ) )
            {
                for( int i = 0; i < thisIR->getNoChannels(); ++i )
                {
                    ImpulseResponseChannel& impulseChannel = thisIR->getChannel( i );

        	    for( int j = 0; j < thisIR->getNoPartitions(); ++j )
                    {
                        // second last partition of IR
                        if( fwonderConf->doTailCrossfades  &&  ( j == thisIR->getNoPartitions() - 2 ) )
                        {
                            ComplexDelayLine& outFadeOutLine = *( outputArray->at( i )->fadeOutComplexLine );

        		    FftwBuf* IRPartBuffer = impulseChannel[ j ];
        		    FftwBuf* outputBuffer = outFadeOutLine[ j ];

        		    complex_mul( source->paddingBuffer->getSamples(), IRPartBuffer->getSamples(), outputBuffer->getSamples(),
                                         source->paddingBuffer->getSSESize() ); 

        		} // last partition of IR
                        else if( fwonderConf->doTailCrossfades && ( j == thisIR->getNoPartitions() - 1 ) )
                        {
                            ComplexDelayLine& outFadeOut2Line = *( outputArray->at( i )->fadeOut2ComplexLine );

        		    FftwBuf* IRPartBuffer = impulseChannel [ j ];
        		    FftwBuf* outputBuffer = outFadeOut2Line[ j ];

        		    complex_mul( source->paddingBuffer->getSamples(), IRPartBuffer->getSamples(), outputBuffer->getSamples(),
                                         source->paddingBuffer->getSSESize() ); 
        		} 
                        else
                        {
                            ComplexDelayLine&  outLine = *( outputArray->at( i )->complexLine );

        		    FftwBuf* IRPartBuffer = impulseChannel[ j ];
        		    FftwBuf* outputBuffer = outLine       [ j ];

        		    complex_mul( source->paddingBuffer->getSamples(), IRPartBuffer->getSamples(), outputBuffer->getSamples(),
                                         source->paddingBuffer->getSSESize() ); 
                        }
        	    }
        	}
            }
        } // end of no newIR nor crossfadeInbetweenIR

        if( fwonderConf->useTail )
        {
            ImpulseResponse* thisIR = source->tailIR;

            if( thisIR &&  ! ( thisIR->killed ) && ( thisIR->getNoChannels() == 2 ) ) 
            {
                for( int i = 0; i < thisIR->getNoChannels(); ++i ) 
                {
                    ImpulseResponseChannel& impulseChannel = thisIR->getChannel( i );
                    ComplexDelayLine&       outLine	   = *( outputArray->at( i )->tailComplexLine );
                    ComplexDelayLine&       outFadeInLine  = *( outputArray->at( i )->tailFadeInComplexLine );

                    int partsToCompute          = thisIR->getNoPartitions() - thisIR->getFirstPartition();
                    int periodsAvail            = fwonderConf->tailPartitionSize / nframes;
                    int step                    = ( processingBlockCounter % periodsAvail);
                    int partsToComputePerPeriod = ( partsToCompute - 1 ) / periodsAvail + 1; //XXX: check if he actually meant (x-1)/(y+1)
                    int startForNow             = thisIR->getFirstPartition() +   step       * partsToComputePerPeriod;
                    int stopBefore              = thisIR->getFirstPartition() + ( step + 1 ) * partsToComputePerPeriod;

                    if( stopBefore > thisIR->getNoPartitions() )
                        stopBefore = thisIR->getNoPartitions();

                    for( int j = startForNow; j < stopBefore; ++j ) 
                    {
                        if( fwonderConf->doTailCrossfades && ( j == thisIR->getFirstPartition() ) ) 
                        {
                            FftwBuf* IRPartBuffer = impulseChannel[ j ];
                            FftwBuf* outputBuffer = outFadeInLine [ j - 2 ];
                            complex_mul( source->tailPaddingBuffer->getSamples(), IRPartBuffer->getSamples(), outputBuffer->getSamples(),
                                         source->tailPaddingBuffer->getSSESize() ); 
                        }
                        else 
                        {
                            FftwBuf* IRPartBuffer = impulseChannel[ j ];
                            FftwBuf* outputBuffer = outLine       [ j - 2 ];
                            complex_mul( source->tailPaddingBuffer->getSamples(), IRPartBuffer->getSamples(), outputBuffer->getSamples(),
                                         source->tailPaddingBuffer->getSSESize() ); 
                        }
                    }
                }
            }
        }
    }


    // convolution of the IRs with the input for this block has been done 
    // the results are still in the various complex delaylines. so get them out for each output
    OutputArray::iterator outputIt;
    for( outputIt = outputArray->begin(); outputIt != outputArray->end(); ++outputIt )
    {
        // get the output delayline for the current output
        DelayLine* outputLine = ( *outputIt )->outputLine;

        // get samples from the complex delayline to the normal delayline
        {
            ComplexDelayLine* outLine   = ( *outputIt )->complexLine;
            FftwBuf*          outBuffer = ( *outLine )[ 0 ];

            outBuffer->ifft();

            outputLine->accumulateAt( 0, outBuffer->getSamples(), outBuffer->getRealSize() );
            outLine->clearCurrentBufferAndAdvance();
        }

        if( fwonderConf->doCrossfades )
        {
            // fade Out
            {
                ComplexDelayLine* outLine   = ( *outputIt )->fadeOutComplexLine;
                FftwBuf*          outBuffer = ( *outLine )[ 0 ];

                outBuffer->ifft();
                outputLine->accumulateFadeOutAt( 0, outBuffer->getSamples(), outBuffer->getRealSize(), nframes );
                outLine->clearCurrentBufferAndAdvance();
            }

            // fade In
            {
                ComplexDelayLine* outLine   = ( *outputIt )->fadeInComplexLine;
                FftwBuf*          outBuffer = ( *outLine )[ 0 ];

                outBuffer->ifft();
                outputLine->accumulateFadeInAt( 0, outBuffer->getSamples(), outBuffer->getRealSize(), nframes );
                outLine->clearCurrentBufferAndAdvance();
            }

            // fade Out 2
            {
                ComplexDelayLine* outLine   = ( *outputIt )->fadeOut2ComplexLine;
                FftwBuf*          outBuffer = ( *outLine )[ 0 ];

                outBuffer->ifft();
                outputLine->accumulateFadeOut2At( 0, outBuffer->getSamples(), outBuffer->getRealSize(), nframes );
                outLine->clearCurrentBufferAndAdvance();
            }

            // fade In 2
            {
                ComplexDelayLine* outLine   = ( *outputIt )->fadeIn2ComplexLine;
                FftwBuf*          outBuffer = ( *outLine )[ 0 ];

                outBuffer->ifft();
                outputLine->accumulateFadeIn2At( 0, outBuffer->getSamples(), outBuffer->getRealSize(), nframes );
                outLine->clearCurrentBufferAndAdvance();
            }
        }

        if( fwonderConf->useTail )
        {
            int periods_avail = fwonderConf->tailPartitionSize / nframes;
            int step          = processingBlockCounter % periods_avail;

            if( step == ( periods_avail - 1 ) )
            {
                // tail
                {
                    ComplexDelayLine* outLine   = ( *outputIt )->tailComplexLine;
                    FftwBuf*          outBuffer = ( *outLine )[ 0 ];

                    outBuffer->ifft();
                    outputLine->accumulateAt( fwonderConf->tailOffset * nframes, outBuffer->getSamples(), outBuffer->getRealSize() );
                    outLine->clearCurrentBufferAndAdvance();
                }

                // tail fade In
                if( fwonderConf->doTailCrossfades )
                {
                    ComplexDelayLine* outLine   = ( *outputIt )->tailFadeInComplexLine;
                    FftwBuf*          outBuffer = ( *outLine )[ 0 ];

                    outBuffer->ifft();
                    outputLine->accumulateFadeInAt( fwonderConf->tailOffset * nframes, outBuffer->getSamples(), outBuffer->getRealSize(), nframes );
                    outLine->clearCurrentBufferAndAdvance();
                }
            }
        }

        // get the resulting samples from the delayline out to JACK
        float* outputBuffer = ( float* ) ( *outputIt )->outputPort->getBuffer( nframes );
        outputLine->getAndClearAndAdvance( outputBuffer, nframes );
    }
    
    ++processingBlockCounter;
    
    return 0;
}

//---------------------------end of audio processing-------------------------//


void signalFPEHandler( int signalID )
{
    cerr << "Caught signal SIGFPE! A floating point exception occurred:" << endl;
    switch( signalID )
    {
        case FE_INEXACT:   
            cerr << "FE_INEXACT = inexact result " << endl;
            break;
        case FE_DIVBYZERO:
            cerr << "FE_DIVBYZERO = division by zero " << endl;
            break;
        case FE_UNDERFLOW:
            cerr << "FE_UNDERFLOW = result not representable due to underflow" << endl;
            break;
        case FE_OVERFLOW: 
            cerr << "FE_OVERFLOW = result not representable due to overflow" << endl;
            break;
        case FE_INVALID:
            cerr << "FE_INVALID = invalid operation" << endl;
            break;
        default:
            cerr<< "Unknown exception!!!" << endl;
            break;
    }

    // be sure to enable core dumps on your system,
    // if you want to know what caused the exception
    abort();
}


void exitCleanupFunction()
{
    if( jackClient )
    {
        jackClient->deactivate();
    }
    if( oscServer )
    {
        oscServer->stop();
        delete oscServer;
        oscServer = NULL;
    }
    if( managerThread )
    {
        stopIRCacheManager = true;
        pthread_join( managerThread, NULL );
    }
    if( realtimeCommandEngine )
    {
        delete realtimeCommandEngine;
        realtimeCommandEngine = NULL;
    }
    if( sourceArray )
    {
        //XXX: can cause a segfault, check cleanup order, something is not threadsafe
        //delete sourceArray;
        //sourceArray = NULL;
    }
    if( outputArray )
    {
         //delete outputArray;
         //outputArray = NULL;
    }
    if( jackClient )
    {
        delete jackClient;
        jackClient = NULL;
    }
}


int main( int argc, char** argv )
{
    // register shutdown cleanup function
    atexit( exitCleanupFunction );

    // set up signal handler for all floating point exceptions
    signal( SIGFPE, signalFPEHandler );

    // read the twonder configuration file
    fwonderConf = new FwonderConfig( argc, argv );
    int retVal  = fwonderConf->readConfig(); 

    if( retVal != 0 )
    {
        std::cerr << "Error parsing fwonder configfile!" << std::endl;
        switch( retVal )
        {
            case 1:
                std::cerr << "configfile " << fwonderConf->fwonderConfigfile << " does not exist." << std::endl;
            break;
            case 2:
                std::cerr << "dtd file" << " could not be found. " << std::endl;
            break;
            case 3:
                std::cerr << "configfile " << fwonderConf->fwonderConfigfile << " is not well formed." << std::endl;
            break;
            case 4:
                std::cerr << "libxml caused an exception while parsing " << fwonderConf->fwonderConfigfile << std::endl;
            break;
            case 5:
                std::cerr << "the configfile contains invalid settings (" << fwonderConf->fwonderConfigfile << ")" << std::endl;
            break;
            default:
            std::cerr << " an unkown error occurred, sorry." << endl;
        }
        exit( EXIT_FAILURE );
    }

    // setup the jack client 
    jackClient = new jackpp::Client( fwonderConf->jackName.c_str() );
    if( ! jackClient->connect() )
    {
        cerr << "Error! Could not connect to JACK." << endl;
        exit( EXIT_FAILURE );
    }

    // get the current sampleRate from jack 
    fwonderConf->sampleRate = jackClient->getSampleRate();
    TimeStamp::initSampleRate( fwonderConf->sampleRate );
 
    // give input and output access to the the jackclient
    OutputAggregate::setJackClient( jackClient );
    SourceAggregate::setJackClient( jackClient );

    // start the command engine
    realtimeCommandEngine = new RTCommandEngine();
    if( ! realtimeCommandEngine )
    {
        cerr << "Error! Could not create command engine." << endl;
        exit( EXIT_FAILURE );
    }

    // set maximum allowed length of impulse response 
    int maxIRLength = fwonderConf->maxIRLength;
    if( ! fwonderConf->doTailCrossfades )
        maxIRLength += jackClient->getBufferSize();
    
    // create in- and outputs
    sourceArray = new SourceArray( fwonderConf->noSources, jackClient->getBufferSize(), maxIRLength, 
                                   fwonderConf->tailPartitionSize, fwonderConf->maxTailLength );
    outputArray = new OutputArray( fwonderConf->noOutputs, jackClient->getBufferSize(), maxIRLength,
                                   fwonderConf->tailPartitionSize, fwonderConf->maxTailLength );

    // create impulse response manager in own thread
    if( pthread_create( &managerThread, NULL, manageIRCache, NULL ) )
    {
        cerr << "Error! Could not create IR cache manager thread." << endl;
        exit( EXIT_FAILURE );
    }

    // start OSCServer and register messagehandler
    try
    {
        oscServer = new OSCServer( fwonderConf->listeningPort.c_str() );
    }
    catch( OSCServer::EServ )
    {
        cerr << "[twonder] Could not create server, maybe the server( using the same port ) is already running?" << endl;                   
        exit( EXIT_FAILURE );
    }

    oscServer->addMethod( "/WONDER/tracker/move",         "ifff", oscMoveHandler );
    oscServer->addMethod( "/WONDER/tracker/move/pan",     "f",    oscMoveHandler );
    oscServer->addMethod( "/WONDER/tracker/move/tilt",    "f",    oscMoveHandler );
    oscServer->addMethod( "/WONDER/tracker/move/rot",     "f",    oscMoveHandler );
    oscServer->addMethod( "/WONDER/fwonder/resolution/x", "i",    oscResolutionHandler );
    oscServer->addMethod( "/WONDER/fwonder/resolution/y", "i",    oscResolutionHandler );
    oscServer->addMethod( NULL,                           NULL,   oscGenericHandler );  
    oscServer->start();
    
    // tell jack which function does the audio processing and start the client
    jackClient->setProcessCallback( process );
    jackClient->activate();

    // notify user the setup did work 
    cout << "Fwonder setup done. Ready to process!" << endl
         << "Hit \"q\" to quit." << endl;

    // enter processing loop with keyboard control
    while( true )
    {
        std::string command= "" ;
	cin >> command;

	if( command == "q" )
            break;
        //else if( command == "" )
        //    break;
        //else if( command == "c0" )
	//    fwonderConf->doCrossfades = 0;
        //else if( command == "c1" )
	//    fwonderConf->doCrossfades = 1;
    }
    
    // cleanup before exiting 
    exitCleanupFunction();
    
    return 0;
} 
