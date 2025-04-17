#include <value/value.hpp>

using namespace hcc;

Value::Value() {
}

Value::~Value() {
}

bool Value::isRegister() {
	return (reg_name != "");
}