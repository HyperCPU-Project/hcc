#include <pch.hpp>

namespace hcc {
class Backend;
class IPlugin {
public:
	std::string name;

	virtual ~IPlugin();
	virtual Backend* backendPlugin_AllocBackend();
};
}; // namespace hcc

#define REGISTER_PLUGIN(className)                                                                                                                                                                                                                             \
	extern "C" {                                                                                                                                                                                                                                                 \
	hcc::IPlugin* hccplugin_create() {                                                                                                                                                                                                                           \
		return new className();                                                                                                                                                                                                                                    \
	}                                                                                                                                                                                                                                                            \
	void hccplugin_destroy(hcc::IPlugin* plugin) {                                                                                                                                                                                                               \
		delete plugin;                                                                                                                                                                                                                                             \
	}                                                                                                                                                                                                                                                            \
	}

typedef fnptr(hcc::IPlugin*, hccplugin_create_t);
typedef fnptr(void, hccplugin_destroy_t, hcc::IPlugin*);
