WFSSharedServer : SharedServer{

	// for now
	numReverbChannels_ { |num| options.numReverbChannels = num }
	numReverbChannels { ^options.numReverbChannels }

	myOuts {
		var numEach = (options.numOutputBusChannels - options.numReverbChannels) div: options.numClients;
		^(0 .. (numEach - 1)) + (numEach * (clientID-1) );
	}

	reverbChannels{
		^( (1..options.numReverbChannels) + (options.numOutputBusChannels - options.numReverbChannels - 1) )
	}
}

WFSSharedServerOptions : SharedServerOptions {

	var <>numReverbChannels;

	*configArgs{
		^(super.configArgs ++ [\numReverbChannels]);
	}

}