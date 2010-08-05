#/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
# *                                                                                   *
# *  WONDER - Wave field synthesis Of New Dimensions of Electronic music in Realtime  *
# *  http://swonder.sourceforge.net                                                   *
# *                                                                                   *
# *                                                                                   *
# *  Technische Universitaet Berlin, Germany                                           *
# *  Audio Communication Group                                                        *
# *  www.ak.tu-berlin.de                                                              *
# *  Copyright 2006-2008                                                              *
# *                                                                                   *
# *                                                                                   *
# *  This program is free software; you can redistribute it and/or modify             *
# *  it under the terms of the GNU General Public License as published by             *
# *  the Free Software Foundation; either version 2 of the License, or                *
# *  (at your option) any later version.                                              *
# *                                                                                   *
# *  This program is distributed in the hope that it will be useful,                  *
# *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
# *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
# *  GNU General Public License for more details.                                     *
# *                                                                                   *
# *  You should have received a copy of the GNU General Public License                *
# *  along with this program; if not, write to the Free Software                      *
# *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.       *
# *                                                                                   *
# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

# ======================================================================
#
#  This is the global SConstruct file for WONDER. 
#  Every module can be build from here.
#  Please see documentation/scons.txt for further instructions.
# 
# ======================================================================


# ======================================================================
# setup
# ======================================================================

EnsureSConsVersion(0,96)
EnsurePythonVersion(2,3)
SConsignFile()


# ======================================================================
# imports
# ======================================================================

import sys
import os
import time

import platform   # Seems to have more portable uname() than os

# ======================================================================
# constants
# ======================================================================

PACKAGE = 'WONDER'
VERSION = '3.2.0'

# ======================================================================
# utility functions
# ======================================================================

PLATFORM = platform.uname()[0].lower()

#def CheckGccVersion( context ):
#    context.Message('gcc > 4.5?')
#    ret = context.TryAction('gcc -dumpversion')[0]
#    context.Result( ret )
#    print ret    
#    return ret
    
#def CheckCC(context):
#    res = SCons.Conftest.CheckCC(context)
#    return not res



def createEnvironment(*keys):
    env = os.environ
    res = {}
    for key in keys:
        if env.has_key(key):
            res[key] = env[key]
    return res



def CheckPKGConfig( context, version ):
    context.Message( 'pkg-config installed? ' )
    ret = context.TryAction('pkg-config --atleast-pkgconfig-version=%s' % version)[0]
    context.Result( ret )
    return ret



def LookForPackage( context, name ):
    context.Message('%s installed? ' % name)
    ret = context.TryAction('pkg-config --exists \'%s\'' % name)[0]
    context.Result( ret )
    return ret



def print_config(msg, two_dee_iterable):
    print "\n %s "%msg
    for key, val in two_dee_iterable:
        print "    %-20s %s " % (key, val)
    print ""
    


def build_config_h(target, source, env):
    config_h_defines = {
        "install_directory": env['installto'],
        "version_str": VERSION,
        "build": env['build'] 
        }
    
    print_config("Generating config.h with the following settings:",
                    config_h_defines.items())
    
    for a_target, a_source in zip(target, source):
        config_h = file(str(a_target), "w")
        config_h_in = file(str(a_source), "r")
    config_h.write(config_h_in.read() % config_h_defines)
    config_h_in.close()
    config_h.close()



def daemon_config_build(target, source, env):
    config_defines = {
        "install_directory": env['installto'],
        }
    
    print_config("Generating daemon-config with the following settings:",
                 config_defines.items())
    
    for a_target, a_source in zip(target, source):
        config    = file(str(a_target), "w")
        config_in = file(str(a_source), "r")
    config.write(config_in.read() % config_defines)
    config_in.close()
    config.close()


# ======================================================================
# print infos to commandline
# ======================================================================

print "\n========== " + PACKAGE + ' ' + VERSION + " ========== "


# ======================================================================
# Build options
# ======================================================================

# Read options from the commandline
opts = Variables(None, ARGUMENTS)
opts.AddVariables(
    BoolVariable('lib',         'Set to 1 to build the wonder library',                       0),
    BoolVariable('jackpp',      'Set to 1 to build jackpplibrary',                            0),
    BoolVariable('cwonder',     'Set to 1 to build cwonder',                                  0),
    BoolVariable('twonder',     'Set to 1 to build twonder',                                  0),
    BoolVariable('fwonder',     'Set to 1 to build fwonder',                                  0),
    BoolVariable('xwonder',     'Set to 1 to build xwonder',                                  0),
    BoolVariable('scoreplayer', 'Set to 1 to build scoreplayer',                              0),
    BoolVariable('jfwonder',    'Set to 1 to build jfwonder',                                 0),
    BoolVariable('qfwonder',    'Set to 1 to build qfwonder',                                 0),
    BoolVariable('tracker',     'Set to 1 to build tracker',                                  0), 
    BoolVariable('wfs',         'Set to 1 to build all wave field synthesis related targets', 0),
    BoolVariable('bas',         'Set to 1 to build all binaural synthesis related targets',   0),
    BoolVariable('all',         'Set to 1 to build the all the targets',                      0),
    EnumVariable('build',       'Set the build version',                 'release', allowed_values=('debug', 'release'), ignorecase=2),
    PathVariable('installto',   'Set the installation directory',        '/usr/local'),
    ('arch', 'Set the target architecture', 'native' )
)


# ======================================================================
# basic environment
# ======================================================================

# Headerfiles are here
includePath = 'src/include'

# put all objectfiles in directory obj/
BuildDir( 'obj', 'src', duplicate=0 )

env = Environment(options = opts,
                  ENV     = createEnvironment('PATH', 'PKG_CONFIG_PATH'),
                  PACKAGE = PACKAGE,
                  VERSION = VERSION,
                  URL     = 'http://swonder.sourceforge.net',
                  TARBALL = PACKAGE + VERSION + '.tbz2',
                  CPPPATH = includePath)

# set options if target all is selected
if env['all']:
    env['lib']         = 1
    env['jackpp']      = 1
    env['cwonder']     = 1 
    env['twonder']     = 1
    env['fwonder']     = 1
    env['xwonder']     = 1
    if PLATFORM == 'linux':
       env['scoreplayer'] = 1
    env['jfwonder']    = 1 
    env['qfwonder']    = 1 
    env['tracker']     = 1 

# set options if target wfs (wave field synthesis) is selected
if env['wfs']:
    env['lib']         = 1
    env['jackpp']      = 1
    env['cwonder']     = 1 
    env['twonder']     = 1
    env['xwonder']     = 1 
    if PLATFORM == 'linux':
       env['scoreplayer'] = 1
    env['jfwonder']    = 1 


# set options if target bas (binaural synthesis) is selected
if env['bas']:
    env['lib']         = 1
    env['jackpp']      = 1
    env['fwonder']     = 1
    env['qfwonder']    = 1
    env['tracker']     = 1 

executables = []
libs        = []

# runtime library path
if 'install' in COMMAND_LINE_TARGETS :
    rpath = env['installto'] + '/lib/'
else:
    rpath = env.Literal('\$$ORIGIN')

# Compiler arguments
env.Append(CCFLAGS = '-Wall')

# set debug flags if necessary
if env['build'] == 'debug':
    env.Append(CCFLAGS = '-g')

print env['arch']

# target architecture and optimizations
# march=native requires gcc 4.3x,
# if you're using gcc version earlier than  4.3 please use one of the disabled lines  
if env['build'] == 'release':
    env.Append(CCFLAGS = '-O3 -msse -msse2 -msse3 -mfpmath=sse')
    if PLATFORM == 'linux': # somewhat of a hack, but osx still ships with an old gcc
       env.Append(CCFLAGS = '-march=native')
    env.Append(CPPDEFINES = 'NDEBUG')
#env.Append(CCFLAGS = '-O3 -march=pentium-m -msse -msse2 -msse3 -mfpmath=sse')
#env.Append(CCFLAGS = '-O3 -march=opteron -msse -msse2 -msse3 -mfpmath=sse')


# Help
Help("""
Command Line options:
scons -h         (WONDER help) 
scons -H         (SCons help) 
      """)

Help( opts.GenerateHelpText( env ) )

# ensure that pkg-config and external dependencies are ok
# and parse them
conf = env.Configure(custom_tests = {  'CheckPKGConfig' : CheckPKGConfig,
                                       'LookForPackage' : LookForPackage })
if not conf.CheckPKGConfig('0'):
    print 'pkg-config not found.'
    Exit(1)
    
# libsndfile and libfftw3f are only needed by fwonder
# parsing is done later by the fwonder environment
if env['fwonder']:
    if not conf.LookForPackage('sndfile'):
        Exit(1)
    if not conf.LookForPackage('fftw3f'):
        Exit(1)

# libxml is not needed for all modules of WONDER
if env['cwonder'] | env['scoreplayer'] | env['lib'] | env['twonder'] | env['tracker'] | env['fwonder'] | env['qfwonder']:
    if not conf.LookForPackage('libxml++-2.6'):
        Exit(1)
    env.ParseConfig('pkg-config --cflags --libs libxml++-2.6')

# jack is not needed for all modules of WONDER
if env['lib'] | env['twonder'] | env['jackpp'] | env['fwonder']:
    if not conf.LookForPackage('jack'):
        Exit(1)
    env.ParseConfig('pkg-config --cflags --libs jack')

# asound is not needed for all modules of WONDER
if env['scoreplayer']:
    if not conf.LookForPackage('alsa'):
        Exit(1)
    #if not conf.LookForPackage('pthread'):
        #Exit(1)
    env.ParseConfig('pkg-config --cflags --libs alsa')

# liblo is needed for all modules of WONDER
if not conf.LookForPackage('liblo'):
    Exit(1)
env.ParseConfig('pkg-config --cflags --libs liblo')

env = conf.Finish()
    


#write build and version info
wonder_version_file = file('wonder.built.info', "w")
wonder_svn_info = os.popen ("svn info").read()[:-1]
wonder_version_file.write( '###\t '+PACKAGE+' build and svn repository info     ###\n')
wonder_version_file.write( '### This file is automatically generated by scons ###\n\n')
wonder_version_file.write( '### build info ###\n')
wonder_version_file.write( 'Version: \t'+PACKAGE+' '+VERSION+' ( '+env['build']+' )\n')
wonder_version_file.write( 'last change: \t'+time.asctime(time.localtime())+'\n\n')
if env['lib']: 
    wonder_version_file.write( 'library: \t'+time.asctime(time.localtime())+'\n')
if env['cwonder']: 
    wonder_version_file.write( 'cwonder: \t'+time.asctime(time.localtime())+'\n')
if env['twonder']: 
    wonder_version_file.write( 'twonder: \t'+time.asctime(time.localtime())+'\n')
if env['fwonder']: 
    wonder_version_file.write( 'fwonder: \t'+time.asctime(time.localtime())+'\n')
if env['xwonder']: 
    wonder_version_file.write( 'xwonder: \t'+time.asctime(time.localtime())+'\n')
if env['scoreplayer']: 
    wonder_version_file.write( 'scoreplayer: \t'+time.asctime(time.localtime())+'\n')
if env['jfwonder']: 
    wonder_version_file.write( 'jfwonder: \t'+time.asctime(time.localtime())+'\n')
if env['qfwonder']: 
    wonder_version_file.write( 'qfwonder: \t'+time.asctime(time.localtime())+'\n')
if env['tracker']: 
    wonder_version_file.write( 'tracker: \t'+time.asctime(time.localtime())+'\n')
if env['jackpp']: 
    wonder_version_file.write( 'jackpp: \t'+time.asctime(time.localtime())+'\n')

wonder_version_file.write( '\n### svn repository info ###\n')
wonder_version_file.write( wonder_svn_info )
wonder_version_file.close()


# write config.h
env.Command(includePath+'/config.h', includePath+'/config.h.in', build_config_h)


# write configfiles for daemons
if env['cwonder']:
    cwonderPath  = '/cwonder'
    env.Command('obj' + cwonderPath + '/cwonder-daemon-config', 'src' + cwonderPath + '/cwonder-daemon-config.in',  daemon_config_build)

if env['jfwonder']:
    jfwonderPath = '/jfwonder'
    env.Command('obj' + jfwonderPath + '/jfwonder-daemon-config', 'src' + jfwonderPath + '/jfwonder-daemon-config.in', daemon_config_build)


# ======================================================================
# wonder modules 
# ======================================================================


# +++ Wonder library +++
lib_srcs = Split('''
obj/lib/oscin.cpp
obj/lib/speakersegment.cpp
obj/lib/liblo_extended.cpp
obj/lib/timestamp.cpp
obj/lib/wonder_log.cpp
obj/lib/wonder_path.cpp
obj/lib/project.cpp
obj/lib/rtcommandengine.cpp
obj/lib/commandqueue.cpp
obj/lib/jackringbuffer.cpp
''')

wonderLibraryName    = 'wonder3'
wonderLibraryVersion = VERSION       
libName              = 'lib' + wonderLibraryName + '.so'
libNameV             =  libName + '.' + wonderLibraryVersion

if env['lib']:
    print 'Wonder library\t[Yes]'
    libenv = env.Clone()
    libenv.Append(LIBS=['jack'], RPATH=rpath)
    wonderlib = libenv.SharedLibrary('bin/' + wonderLibraryName, lib_srcs)
    libs.append(wonderlib)
else:
    print 'Wonder library\t[No]'


# +++ jackpp library+++
jackpp_srcs = Split('''
obj/jackpp/jackppclient.cpp 
obj/jackpp/jackppport.cpp
''')

jackppLibraryName    = 'jackpp'
libNameJackpp        = 'lib' + jackppLibraryName + '.so'
jackppLibraryVersion = '1.0.0'       
libNameVJackpp       =  libNameJackpp + '.' + jackppLibraryVersion

if env['jackpp']:
    print 'jackpp library\t[Yes]'
    jackppenv = env.Clone()
    jackppenv.Append(LIBS=['jack'])
    jackpplib = jackppenv.SharedLibrary('bin/'+jackppLibraryName, jackpp_srcs)
    libs.append(jackpplib)
else:
    print 'jackpp library\t[No]'



# +++ cwonder +++
cwonder_srcs = Split('''
obj/cwonder/main.cpp
obj/cwonder/cwonder.cpp
obj/cwonder/cwonder_config.cpp
obj/cwonder/oscinctrl.cpp
obj/cwonder/events.cpp
obj/cwonder/oscstream.cpp
obj/cwonder/oscping.cpp
''')

if env['cwonder']:
    print 'cwonder\t\t[Yes]'
    cwonderenv = env.Clone()
    cwonderenv.Append(LIBS=[wonderLibraryName], LIBPATH='#bin/', RPATH=rpath)
    cwonderprog = cwonderenv.Program('bin/cwonder3', cwonder_srcs)
    executables.append(cwonderprog)    
else:
    print 'cwonder\t\t[No]'


# +++ twonder +++
twonder_srcs = Split('''
obj/render/twonder/twonder.cpp
obj/render/twonder/delayline.cpp
obj/render/twonder/delaycoeff.cpp
obj/render/twonder/speaker.cpp
obj/render/twonder/osc.cpp
obj/render/twonder/source.cpp
obj/render/twonder/angle.cpp
obj/render/twonder/twonder_config.cpp
''')

if env['twonder']:
    print 'twonder\t\t[Yes]'
    twonderenv = env.Clone()
    twonderenv.Append(CPPPATH='src/jackpp/')
    twonderenv.Append(LIBS=['jack']) # explicit link to jack needed on osx
    twonderenv.Append(LIBS=[wonderLibraryName, jackppLibraryName], LIBPATH='#bin/', RPATH=rpath)
    twonderprog = twonderenv.Program('bin/twonder3', twonder_srcs)
    executables.append(twonderprog)
else:
    print 'twonder\t\t[No]'


# +++ fwonder +++
fwonder_srcs = Split('''
obj/render/fwonder/fwonder.cpp
obj/render/fwonder/outputarray.cpp
obj/render/fwonder/sourcearray.cpp
obj/render/fwonder/outputaggregate.cpp
obj/render/fwonder/sourceaggregate.cpp
obj/render/fwonder/complex_mul.cpp
obj/render/fwonder/complexdelayline.cpp
obj/render/fwonder/fftwbuf.cpp
obj/render/fwonder/impulseresponse.cpp
obj/render/fwonder/irmatrixmanager.cpp
obj/render/fwonder/irmatrix.cpp
obj/render/fwonder/fwonder_config.cpp
obj/render/fwonder/delayline.cpp
''')

if env['fwonder']:
    print 'fwonder\t\t[Yes]'
    fwonderenv = env.Clone()
    fwonderenv.ParseConfig('pkg-config --libs sndfile')
    fwonderenv.ParseConfig('pkg-config --libs fftw3f')
    fwonderenv.Append(CPPPATH='src/jackpp/')
    fwonderenv.Append(LIBS=[wonderLibraryName, jackppLibraryName], LIBPATH='#bin/', RPATH=rpath)
    fwonderprog = fwonderenv.Program('bin/fwonder3', fwonder_srcs)
    executables.append(fwonderprog)
    from os.path import expanduser,split
else:
    print 'fwonder\t\t[No]'


# +++ xwonder +++
xwonder_files = Split("""
obj/xwonder/Xwonder.cpp
obj/xwonder/XwonderMainWindow.cpp
obj/xwonder/XwonderConfig.cpp
obj/xwonder/Sources3DWidget.cpp
obj/xwonder/SourcePositionDialog.cpp
obj/xwonder/ChannelsWidget.cpp
obj/xwonder/SourceWidget.cpp
obj/xwonder/Source.cpp
obj/xwonder/SourceGroup.cpp
obj/xwonder/TimelineWidget.cpp
obj/xwonder/TimeLCDNumber.cpp
obj/xwonder/SnapshotSelector.cpp
obj/xwonder/SnapshotSelectorButton.cpp
obj/xwonder/SnapshotNameDialog.cpp
obj/xwonder/OSCReceiver.cpp
obj/xwonder/FilenameDialog.cpp
obj/xwonder/AddChannelsDialog.cpp
obj/xwonder/StreamClientWidget.cpp
obj/lib/oscin.cpp
""")

if env['xwonder']:
    print 'xwonder\t\t[Yes]'
    xwonderenv = env.Clone()
    xwonderenv.Append(LIBS=['GLU'])
    xwonderenv.Tool('qt4', toolpath=['./tools/qt4tool'])
    xwonderenv.EnableQt4Modules(['QtCore','QtGui','QtOpenGL','QtXml'],debug=False) 
    resource_files = xwonderenv.Qrc("obj/xwonder/icons.qrc")
    xwonderprog = xwonderenv.Program(target=['bin/xwonder3'], source=xwonder_files+resource_files)
    executables.append(xwonderprog)    
else:
    print 'xwonder\t\t[No]'


# +++ scoreplayer +++
scoreplayer_srcs = Split('''
obj/score/main.cpp
obj/score/mtc_com.cpp
obj/score/w_osc.cpp
obj/score/score_line.cpp
obj/score/file_io.cpp
obj/score/scoresaxparser.cpp
obj/score/scorecontrol.cpp
obj/score/scoreplayer_config.cpp
obj/score/rtmidi/RtMidi.cpp
''')

if env['scoreplayer']:
    print 'scoreplayer\t[Yes]'
    scoreplayerenv = env.Clone()
    scoreplayerenv.Append(CCFLAGS = ['-D__LINUX_ALSASEQ__'])
    if env['build']=='debug':
        scoreplayerenv.Append(CCFLAGS = ['-D__RTMIDI_DEBUG__'])
    scoreplayerenv.Append(LIBS=[wonderLibraryName, 'pthread', 'asound'], LIBPATH='#bin/', RPATH=rpath)
    scoreplayerprog = scoreplayerenv.Program('bin/scoreplayer3', scoreplayer_srcs)
    executables.append(scoreplayerprog)    
else:
    print 'scoreplayer\t[No]'


# +++ jfwonder +++
jfwonder_srcs = Split('''
obj/jfwonder/jfwonder.cpp
obj/jfwonder/jfwonder_config.cpp
''')

if env['jfwonder']:
    print 'jfwonder\t[Yes]'
    jfwonderenv = env.Clone()
    jfwonderenv.Append(LIBS=['jack']) # explicit link to jack needed on osx
    jfwonderenv.Append(LIBS=[wonderLibraryName], LIBPATH='#bin/', RPATH=rpath)
    jfwonderprog = jfwonderenv.Program('bin/jfwonder3', jfwonder_srcs)
    executables.append(jfwonderprog)
else:
    print 'jfwonder\t[No]'


# +++ tracker +++
tracker_srcs = Split('''
obj/tracker/tracker.cpp
obj/tracker/tracker_state.cpp
obj/tracker/tracker_config.cpp
obj/tracker/isense.c
''')

if env['tracker']:
    print 'tracker\t\t[Yes]'
    trackerenv = env.Clone()
    trackerenv.Append(LIBS=[wonderLibraryName], LIBPATH='#bin/', RPATH=rpath)
    trackerprog = trackerenv.Program('bin/tracker3', tracker_srcs)
    executables.append(trackerprog)
    libs.append('src/tracker/libisense.so')
else:
    print 'tracker\t\t[No]'


# +++ qfwonder +++
qfwonder_srcs = Split('''
obj/qfwonder/main.cpp
obj/qfwonder/qfwonder.cpp
obj/qfwonder/cachematrix.cpp
obj/qfwonder/osccontrol.cpp
obj/qfwonder/qfwonder_config.cpp
''')

if env['qfwonder']:
    print 'qfwonder\t[Yes]'
    qfwonderenv = env.Clone()
    qfwonderenv.Tool('qt4', toolpath=['./tools/qt4tool'])
    qfwonderenv.EnableQt4Modules(['QtCore','QtGui','QtXml'],debug=False) 
    qfwonderenv.Append(LIBS=[wonderLibraryName], LIBPATH='#bin/', RPATH=rpath)
    qfwonderprog = qfwonderenv.Program('bin/qfwonder3', qfwonder_srcs )
    executables.append(qfwonderprog)
else:
    print 'qfwonder\t[No]'


# +++ Install +++                                          
if 'install' in COMMAND_LINE_TARGETS and '-c' not in sys.argv:
    PREFIX            = env['installto']
    BIN_DIR           = PREFIX + "/bin"
    LIB_DIR           = PREFIX + "/lib"
    DTD_DIR           = PREFIX + "/share/wonder3/dtd"
    CONF_DIR          = PREFIX + "/share/wonder3/configs"
    DAEMON_DIR        = "/etc/init.d"
    DAEMON_CONFIG_DIR = "/etc/default"

    # detemine which dtd and configuration files need to be installed
    dtds    = []
    configs = []

    if env['cwonder']:
        dtds.append('dtd/cwonder_project.dtd')
        dtds.append('dtd/cwonder_config.dtd')
        configs.append('configs/cwonder_config.xml')

    if env['twonder']:
        dtds.append('dtd/twonder_config.dtd')
        dtds.append('dtd/twonder_speakerarray.dtd')
        configs.append('configs/twonder_config.xml')
        configs.append('configs/twonder_speakerarray.xml')

    if env['scoreplayer']:
        dtds.append('dtd/scoreplayer_config.dtd')
        dtds.append('dtd/scoreplayer_score.dtd')
        configs.append('configs/scoreplayer_config.xml')

    if env['fwonder']:
        dtds.append('dtd/fwonder_config.dtd')
        configs.append('configs/fwonder_config.xml')

    if env['tracker']:
        dtds.append('dtd/tracker_config.dtd')
        configs.append('configs/tracker_config.xml')


    # now do the installation 
    env.Alias( 'install', env.Install(CONF_DIR, "wonder.built.info") )
    env.Alias( 'install', env.Install(CONF_DIR, configs) )
    env.Alias( 'install', env.Install(DTD_DIR, dtds) )
    env.Alias( 'install', env.Install(LIB_DIR, libs) )
    env.Alias( 'install', env.Install(BIN_DIR, executables) )

    # install daemon services
    if env['cwonder']: 
        if 'install' in COMMAND_LINE_TARGETS and '-c' not in sys.argv:
            Execute(Chmod('src/cwonder/cwonder-daemon', 0755))
        env.Alias( 'install', env.InstallAs(DAEMON_CONFIG_DIR + "/cwonder-daemon",  "obj/cwonder/cwonder-daemon-config") )
        env.Alias( 'install', env.Install(DAEMON_DIR, 'src/cwonder/cwonder-daemon') )

    if env['jfwonder']:
        if 'install' in COMMAND_LINE_TARGETS and '-c' not in sys.argv:
            Execute(Chmod('src/jfwonder/jfwonder-daemon', 0755))
        env.Alias( 'install', env.InstallAs(DAEMON_CONFIG_DIR + "/jfwonder-daemon", "obj/jfwonder/jfwonder-daemon-config") )
        env.Alias( 'install', env.Install(DAEMON_DIR, 'src/jfwonder/jfwonder-daemon') )



# +++ print buildmode +++
print 'build\t\t[' + env['build'] + ']'


# +++ print installdirectory +++
if 'install' in COMMAND_LINE_TARGETS :
    print 'install to:\t' + env['installto']
    print '\n'


# ======================================================================
# distribution
# ======================================================================

# for packaging:
import tarfile
import re

ANY_FILE_RE = re.compile('.*')
DTD_FILE_RE = re.compile('.*\.dtd')
XML_FILE_RE = re.compile('.*\.xml')
DOC_FILE_RE = re.compile('.*\.(txt|ods|pdf)$')
SRC_FILE_RE = re.compile('.*\.(c(pp)|h|in|s)$')


DIST_FILES = Split('''
SConstruct
README
''')

DIST_SPECS = [
    ('configs', XML_FILE_RE),
    ('documentation', DOC_FILE_RE),
    ('dtd', DTD_FILE_RE),
    ('src', ANY_FILE_RE),
    #('src/ladspa', ANY_FILE_RE),
    #('src/score/rtmidi', ANY_FILE_RE),
	#('src/xwonder', ANY_FILE_RE),
    ('tools', ANY_FILE_RE)
    ]


def dist_paths():
    paths = DIST_FILES[:]
    specs = DIST_SPECS[:]
    while specs:
        base, re = specs.pop()
        if not re: re = ANY_FILE_RE
        for root, dirs, files in os.walk(base):
            if '.svn' in dirs: dirs.remove('.svn')
            for path in dirs[:]:
                if re.match(path):
                    specs.append((os.path.join(root, path), re))
                    dirs.remove(path)
            for path in files:
                if re.match(path):
                    paths.append(os.path.join(root, path))
    paths.sort()
    return paths

def build_tar(env, target, source):
    paths = dist_paths()
    tarfile_name = str(target[0])
    tar_name = os.path.splitext(os.path.basename(tarfile_name))[0]
    tar = tarfile.open(tarfile_name, "w:bz2")
    for path in paths:
        tar.add(path, os.path.join(tar_name, path))
    tar.close()

if 'dist' in COMMAND_LINE_TARGETS:
    env.Alias('dist', env['TARBALL'])
    env.Command(env['TARBALL'], 'SConstruct', build_tar)


# ======================================================================
# end of SConstruct
