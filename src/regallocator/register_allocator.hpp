#pragma once

#include "abi_metadata.hpp"
namespace hcc {
  class Backend;

  class RegisterAllocator {
  protected:
    struct Register {
      std::string name;
      std::size_t count;

      Register(std::string name);
      void Retain();
      void Release();
    };

  protected:
    ABIMetadata abi;

    std::vector<Register> registers;

  public:
    RegisterAllocator(ABIMetadata abi, std::vector<std::string> blacklisted_registers = {});
    ~RegisterAllocator();

    std::string AllocateRegister();
    void RetainRegister(std::string reg);
    void ReleaseRegister(std::string reg);
  };
} // namespace hcc