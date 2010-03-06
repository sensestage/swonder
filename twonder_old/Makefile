
#programs to use...

CC=g++
CXX=g++


#CXXFLAGS=-O2 -march=athlon-xp -I/usr/local/include/jackpp -I../../../include
#CXXFLAGS=-O3 -march=pentium-m -I/usr/local/include/jackpp -I../../../include
#CXXFLAGS=-O2 -march=athlon-xp -I/usr/local/include/jackpp -I../../../include
#CXXFLAGS=-O3 -march=pentium-m -msse -I/usr/local/include/jackpp -I../../../include 
#CXXFLAGS=-O2 -march=opteron -I../../jackpp -I../../../include
CXXFLAGS=-O3 -msse -march=athlon64 -I../../jackpp -I../../../include -I ../../lib
#CXXFLAGS=-O3 -msse -march=athlon-xp -I../../jackpp -I../../../include

CXXFLAGS+=`pkg-config --cflags libxml++-2.6`

LDFLAGS+= -llo
LDFLAGS+=`pkg-config --libs jack`
LDFLAGS+=`pkg-config --libs libxml++-2.6`


all: twonder

twonder: twonder.o delayline.o speaker.o commandqueue.o osc.o rtcommandengine.o source.o angle.o twonder_globalconfig.o ../../lib/spkarray.o ../../jackpp/jackppclient.o ../../jackpp/jackppport.o ../../lib/swonder_config.o

twonder.o: twonder.cpp

delayline.o: delayline.cpp

#delaycoeff.o: delaycoeff.cpp

speaker.o: speaker.cpp

commandqueue.o: commandqueue.cpp

osc.o: osc.cpp

rtcommandengine.o: rtcommandengine.cpp

source.o: source.cpp

angle.o: angle.cpp

twonder_globalconfig.o: twonder_globalconfig.cpp

../../lib/spkarray.o: ../../lib/spkarray.cpp

../../lib/swonder_config.o: ../../lib/swonder_config.cpp

../../jackpp/jackppport.o: ../../jackpp/jackppport.cpp 

../../jackpp/jackppclient.o: ../../jackpp/jackppclient.cpp

#velocity-calc.tex: velocity-calc.gp
#	gp -q <velocity-calc.gp >velocity-calc.tex
#
#velocity-calc.dvi: velocity-calc.tex
#	latex velocity-calc.tex
