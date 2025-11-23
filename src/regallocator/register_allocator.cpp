#include "regallocator/register_allocator.hpp"
#include "util.hpp"
#include <cassert>
#include <fmt/base.h>
#include <locale>

hcc::RegisterAllocator::Register::Register(std::string _name)
    : name(_name), count(0) {
}

void hcc::RegisterAllocator::Register::Retain() {
  assert(count > 0);
  count++;
  fmt::println("retain {} now {}", name, count);
}

void hcc::RegisterAllocator::Register::Release() {
  assert(count > 0);
  count--;
  fmt::println("release {} now {}", name, count);
}

hcc::RegisterAllocator::RegisterAllocator(hcc::ABIMetadata _abi, std::vector<std::string> blacklisted_registers)
    : abi(_abi) {
  for (std::string reg : abi.registers) {
    if (VectorContains(blacklisted_registers, reg))
      continue;
    registers.emplace_back(Register(reg));
  }
}

hcc::RegisterAllocator::~RegisterAllocator() {
  for (auto& reg : registers) {
    if (reg.count > 0)
      fmt::println("~RegisterAllocator(): leaked {} with count {}", reg.name, reg.count);
  }
}

std::string hcc::RegisterAllocator::AllocateRegister() {
  for (auto& reg : registers) {
    if (reg.count == 0) {
      reg.count = 1;
      fmt::println("allocate {} count {}", reg.name, reg.count);
      return reg.name;
    }
  }
  assert(0 && "AllocateRegister failed: no available registers");
  return "";
}

hcc::RegisterAllocator::Register& hcc::RegisterAllocator::FindRegisterByName(std::string reg_name) {
  for (auto& reg : registers) {
    if (reg.name == reg_name) {
      return reg;
    }
  }
  assert(0 && "FindRegisterByName failed: no such register");
}

void hcc::RegisterAllocator::RetainRegister(std::string reg_name) {
  FindRegisterByName(reg_name).Retain();
}

void hcc::RegisterAllocator::ReleaseRegister(std::string reg_name) {
  FindRegisterByName(reg_name).Release();
}