#pragma once

#include <backend/backend.hpp>

namespace hcc {
  class HyperCPUBackend : public Backend {
  public:
    HyperCPUBackend();

    uint64_t incrementRegIndex() override;

    void emitFunctionPrologue(std::string name) override;
    void emitFunctionEpilogue() override;

    std::string emitMovConst(uint64_t value, std::string reg_name = "") override;

    void emitAdd(std::string ROUT, std::string RLHS, std::string RRHS) override;
    void emitSub(std::string ROUT, std::string RLHS, std::string RRHS) override;
    void emitMul(std::string ROUT, std::string RLHS, std::string RRHS) override;
    void emitDiv(std::string ROUT, std::string RLHS, std::string RRHS) override;

    void emitMove(std::string rdest, std::string rsrc) override;

    void emitReserveStackSpae(uint64_t size) override;
    void emitStoreToStack(uint64_t align, uint64_t size, std::string rsrc) override;

    std::string emitLoadFromStack(uint64_t align, uint64_t size, std::string load_reg = "") override;
    std::string emitLoadaddrFromStack(uint64_t align, std::string load_reg = "") override;

    void emitCall(std::string name) override;

    void emitPush(std::string reg) override;
    void emitPop(std::string reg) override;

    void emitSingleRet() override;
    void emitLabel(std::string name) override;
  };
} // namespace hcc
