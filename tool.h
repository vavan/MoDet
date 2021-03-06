#ifndef CONFIG_H_
#define CONFIG_H_
#include <log4cpp/Category.hh>
#include <libconfig.h++>

/*
 * Logging system
 */
//Access to the log
#define LOG log4cpp::Category::getRoot()
//Initialization of the log
void init_log();
bool isDebug();

/*
 * Major configuration class. Handles reading a config from file and provide access to the fields
 */
class MDConfig
{
private:
	static MDConfig* instance;
	libconfig::Config cfg;
	MDConfig();
public:
	//Singletone
	static void init() { instance = new MDConfig(); };
	static libconfig::Setting& getRoot() {
		return instance->cfg.getRoot();
	};
	static libconfig::Config& get() {
		return instance->cfg;
	};
	//Check is the config were initiated and loaded form file
	static bool valid() {
		return (instance != NULL);
	}
};

/*
 * Name of the MoDet System (form major_version.minor_version)
 */
std::string version_name();


#endif /* CONFIG_H_ */
