#include <string>

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

#ifndef fnptr
#define fnptr(ret, name, ...) ret (*name)(__VA_ARGS__)
#endif

typedef fnptr(hcc::IPlugin*, hccplugin_create_t);
typedef fnptr(void, hccplugin_destroy_t, hcc::IPlugin*);
