#include <plugin/hcc_plugin.hpp>

class MainPlugin : public hcc::IPlugin {
public:
	MainPlugin() {
		name = "main";
	}
};

REGISTER_PLUGIN(MainPlugin);
