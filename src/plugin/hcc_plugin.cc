#include <plugin/hcc_plugin.hpp>

hcc::IPlugin::~IPlugin() {
}

hcc::Backend* hcc::IPlugin::backendPlugin_AllocBackend() {
	return nullptr;
}
