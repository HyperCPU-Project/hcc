#pragma once

#include "abi_metadata.hpp"
namespace hcc {
  class Backend;

  class RegisterAllocator {
  protected:
    ABIMetadata abi;

    std::map<std::string, size_t> registers;

  public:
    RegisterAllocator(ABIMetadata abi);
    ~RegisterAllocator();

    std::string AllocateRegister();
    void RetainRegister(std::string reg);
    void ReleaseRegister(std::string reg);
  };
} // namespace hcc