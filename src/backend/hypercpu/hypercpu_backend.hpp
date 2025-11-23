#pragma once

#include <backend/backend.hpp>

namespace hcc {
  class HyperCPUBackend : public Backend {
  public:
    HyperCPUBackend(HCC* hcc);

    void EmitFunctionPrologue(std::string name) override;
    void EmitFunctionEpilogue() override;

    std::string EmitMovConst(uint64_t value, std::string reg_name) override;

    void EmitAdd(std::string ROUT, std::string RLHS, std::string RRHS) override;
    void EmitSub(std::string ROUT, std::string RLHS, std::string RRHS) override;
    void EmitMul(std::string ROUT, std::string RLHS, std::string RRHS) override;
    void EmitDiv(std::string ROUT, std::string RLHS, std::string RRHS) override;

    void EmitMove(std::string rdest, std::string rsrc) override;

    void EmitReserveStackSpace(uint64_t size) override;
    void EmitStoreToStack(uint64_t align, uint64_t size, std::string rsrc) override;

    std::string EmitLoadFromStack(uint64_t align, uint64_t size, std::string load_reg = "") override;
    std::string EmitLoadaddrFromStack(uint64_t align, std::string load_reg = "") override;

    void EmitCall(std::string name) override;

    void EmitPush(std::string reg) override;
    void EmitPop(std::string reg) override;

    void EmitSingleRet() override;
    void EmitLabel(std::string name) override;
  };
} // namespace hcc
