
#include "twonder_globalconfig.h"
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

TWonderGlobalConfig::TWonderGlobalConfig( int argc, char **argv ) {

    // Set the defaults:

    verbose = 0;
    config_path = "/rfs/bender/etc/twonder-default/";
    swonder_conf = "/usr/local/share/swonder/default.xml";
	global_array = "/usr/local/share/swonder/globalarray.xml";

    jack_name = "twonder";
    daemon = 0;
    planecomp = 0.02;

    num_sources=32;
    portname = "58200";

    sound_speed = 340.0;
    negdelay_init = 20.0;

    gtrans.xoffset = 0; gtrans.yoffset = 0; gtrans.zoffset = 0.5;
    gtrans.rotZ = 0; gtrans.rotZax = 0; gtrans.rotZay = -0.75;

    gtrans_old = gtrans;
    iomode = IOM_H104;
    headphonemode = false;
    nonrt_debug = false;
    
    // Now parse the commandline args...

    parse_args( argc, argv );
    fadejump_threshold = new vector<int>(num_sources);
    for( int i=0; i<num_sources; i++ ) {
	fadejump_threshold->at( i ) = 500;
    }
}


void TWonderGlobalConfig::parse_args( int argc, char **argv ) {

    int c;

    while (1) {
	int option_index = 0;
	static struct option long_options[] = {
	    {"configpath",  required_argument,	NULL, 'c' },
	    {"swonderconf",required_argument,	NULL, 9   },
		{"globalarray",required_argument,	NULL, 13   },
	    {"jackname",    required_argument,	NULL, 'n' },
	    {"portname",    required_argument,	NULL, 'p' },
	    {"verbose",	    no_argument,	NULL, 'v' },
	    {"daemon",	    no_argument,	NULL, 'd' },
	    {"numsources",  required_argument,	NULL, 's' },
	    {"planecomp",   required_argument,	NULL, 'o' },
	    {"soundspeed",   required_argument,	NULL, 10 },
	    {"negdelay",   required_argument,	NULL, 11 },
	    {"alwaysout",   no_argument,	NULL, 6	  },
	    {"alwaysin",    no_argument,	NULL, 7	  },
 	    {"en325",    no_argument,	NULL, 12	  },
	    {"headphonemode",    no_argument,	NULL, 'h'  },
	    {"nonrtdebug",  no_argument,	NULL, 8  },
	    {0, 0, 0, 0}
	};

	c = getopt_long (argc, argv, "c:n:o:vhds:p:", long_options, &option_index);
	if (c == -1)
	    break;

	switch (c) {
	    case 'c':
		config_path = strdup(optarg);
		break;

	    case 'n':
		jack_name = strdup(optarg);
		break;

	    case 'p':
		portname = strdup(optarg);
		break;

	    case 'v':
		verbose = 1;
		break;

	    case 'h':
		headphonemode = 1;
		break;

	    case 'd':
		daemon = 1;
		break;

	    case 's':
		num_sources = atoi(optarg);
		break;

	    case 'o':
		planecomp = atof(optarg);
		break;

	    case 6:
		iomode = IOM_ALWAYSOUT;
		break;

	    case 7:
		iomode = IOM_ALWAYSIN;
		break;

	    case 8:
		nonrt_debug = true;
		break;

	    case 9:
		swonder_conf = strdup(optarg);
		break;

	    case 10:
		sound_speed = atof(optarg);
		break;

	    case 11:
		negdelay_init = atof(optarg);
		break;

	    case 12:
		iomode = IOM_EN325;
		break;

	    case 13:
		global_array = strdup(optarg);
		break;

	    case '?':
		break;

	    default:
		printf ("?? getopt returned character code 0%o ??\n", c);
	}
    }

	printf( "iomode %i\n", iomode );
	printf( "swonder_conf %s\n", swonder_conf );

    if (optind < argc) {
	printf ("non-option ARGV-elements: ");
	while (optind < argc)
	    printf ("%s ", argv[optind++]);
	printf ("\n");

	exit(20);
    }
}

TWonderGlobalConfig *conf = NULL;
