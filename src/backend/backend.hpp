#pragma once

#include <metadata.hpp>
#include <pch.hpp>

namespace hcc {
  class Backend {
  protected:
    uint64_t reg_index;

  public:
    std::string output;
    ABIMetadata abi;

    bool codegen_comments = false;

    std::map<std::string, TypeMetadata> types;

    Backend();
    virtual ~Backend() = 0;

    virtual uint64_t IncrementRegIndex();
    virtual void ResetRegIndex();

    virtual void EmitFunctionPrologue(std::string name);
    virtual void EmitFunctionEpilogue();

    virtual std::string EmitMovConst(uint64_t value, std::string reg_name = "");

    virtual void EmitAdd(std::string ROUT, std::string RLHS, std::string RRHS);
    virtual void EmitSub(std::string ROUT, std::string RLHS, std::string RRHS);
    virtual void EmitMul(std::string ROUT, std::string RLHS, std::string RRHS);
    virtual void EmitDiv(std::string ROUT, std::string RLHS, std::string RRHS);

    virtual void EmitMove(std::string rdest, std::string rsrc);

    virtual void EmitReserveStackSpace(uint64_t size);

    virtual std::string EmitLoadFromStack(uint64_t align, uint64_t size, std::string load_reg = "");
    virtual void EmitStoreToStack(uint64_t align, uint64_t size, std::string rsrc);
    virtual std::string EmitLoadaddrFromStack(uint64_t align, std::string load_reg = "");

    virtual void EmitCall(std::string name);

    virtual void EmitPush(std::string reg);
    virtual void EmitPop(std::string reg);

    virtual void EmitSingleRet();
    virtual void EmitLabel(std::string name);

    virtual TypeMetadata* GetTypeFromName(std::string name);
  };
} // namespace hcc
