#ifndef CONFIG_H_
#define CONFIG_H_
#include <log4cpp/Category.hh>
#include <libconfig.h++>


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
	static bool valid() {
		return (instance != NULL);
	}
};


void init_log(std::string deviceId="NONE");

std::string version_name();


#endif /* CONFIG_H_ */
