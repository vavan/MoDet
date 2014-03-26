#ifndef CONFIG_H_
#define CONFIG_H_
#include <log4cpp/Category.hh>
#include <libconfig.h++>

#define ALERT_TYPE_MOTION "1"
#define CONFIG_VERSION "0.7"

#define LOG log4cpp::Category::getRoot()

class MDConfig
{
private:
	static MDConfig* instance;
	libconfig::Config cfg;
	MDConfig();
public:
	static void init() { instance = new MDConfig(); };
	static libconfig::Setting& getRoot() {
		return instance->cfg.getRoot();
	};
};


void init_log(std::string deviceId="NONE");


#endif /* CONFIG_H_ */
