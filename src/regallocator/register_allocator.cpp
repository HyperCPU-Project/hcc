#include "regallocator/register_allocator.hpp"
#include <cassert>
#include <fmt/base.h>

hcc::RegisterAllocator::RegisterAllocator(hcc::ABIMetadata _abi) : abi(_abi) {
  for (std::string reg : abi.registers) {
    registers[reg] = 0;
  }
}

hcc::RegisterAllocator::~RegisterAllocator() {
  for (auto& [reg, count] : registers) {
    if (count > 0)
      fmt::println("~RegisterAllocator(): leaked {} with count {}", reg, count);
  }
}

std::string hcc::RegisterAllocator::AllocateRegister() {
  for (auto& [reg, count] : registers) {
    if (count == 0) {
      count = 1;
      fmt::println("allocate {} count {}", reg, registers[reg]);
      return reg;
    }
  }
  assert(0 && "AllocateRegister failed: no available registers");
  return "";
}

void hcc::RegisterAllocator::RetainRegister(std::string reg) {
  if (!registers.contains(reg)) {
    assert(0 && "RetainRegister failed: no such register");
  }

  assert(registers[reg] > 0);
  registers[reg]++;
  fmt::println("retain {} now {}", reg, registers[reg]);
}

void hcc::RegisterAllocator::ReleaseRegister(std::string reg) {
  if (!registers.contains(reg)) {
    assert(0 && "RetainRegister failed: no such register");
  }

  assert(registers[reg] > 0);
  registers[reg]--;
  fmt::println("release {} now {}", reg, registers[reg]);
}