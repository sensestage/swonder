
#ifndef TWONDER_GLOBALCONFIG_H
#define TWONDER_GLOBALCONFIG_H

#include <vector>


enum InOutMode {
    IOM_H104, IOM_XZPLANE, IOM_ALWAYSOUT, IOM_ALWAYSIN, IOM_EN325
};

/**
 * @brief  Holds offsets and rotation values for room transformation (e.g. for headtracker) 
 */
struct translation {
	float xoffset, yoffset, zoffset;
	float rotZ, rotZax, rotZay;
};

/**
 * @brief  TWonderGlobalConfig holds global configs for twonder.
 */

class TWonderGlobalConfig {
    private:
	void parse_args( int argc, char **argv );
    public:
	char *config_path;
	char *swonder_conf;
	char *global_array;
	char *jack_name;
	int   verbose;
	int   daemon;
	int   num_sources;
	char *portname;
	float planecomp;
	float sound_speed;
	float negdelay_init;
	enum InOutMode iomode;
	bool headphonemode;
	bool nonrt_debug;

	struct translation gtrans;
	struct translation gtrans_old;
	std::vector<int> *fadejump_threshold;

	/**
 *  @brief Constructor to scan given config.
 *
 *  The Constructor sets the defaults and reads config from commandline (argv). 
 */
	TWonderGlobalConfig( int argc, char **argv );
};

extern TWonderGlobalConfig *conf;

#endif
