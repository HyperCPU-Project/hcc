#include "metadata.hpp"
using namespace hcc;

TypeMetadata::TypeMetadata() : name(""), size(0){}
TypeMetadata::TypeMetadata(std::string _name, uint8_t _size) : name(_name), size(_size){}

VariableMetadata::VariableMetadata() : offset(0), type(TypeMetadata()){}
VariableMetadata::VariableMetadata(size_t _offset, TypeMetadata _type) : offset(_offset), type(_type){}