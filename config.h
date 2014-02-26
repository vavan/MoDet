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
};


void init_log();


#endif /* CONFIG_H_ */
