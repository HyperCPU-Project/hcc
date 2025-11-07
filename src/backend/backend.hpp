#pragma once

#include <metadata.hpp>
#include <pch.hpp>

namespace hcc {
  class Backend {
  protected:
    uint64_t regIndex;

  public:
    std::string output;
    ABIMetadata abi;

    bool codegenComments = false;

    std::map<std::string, TypeMetadata> types;

    Backend();
    virtual ~Backend() = 0;

    virtual uint64_t incrementRegIndex();
    virtual void resetRegIndex();

    virtual void emitFunctionPrologue(std::string name);
    virtual void emitFunctionEpilogue();

    virtual std::string emitMovConst(uint64_t value, std::string reg_name = "");

    virtual void emitAdd(std::string ROUT, std::string RLHS, std::string RRHS);
    virtual void emitSub(std::string ROUT, std::string RLHS, std::string RRHS);
    virtual void emitMul(std::string ROUT, std::string RLHS, std::string RRHS);
    virtual void emitDiv(std::string ROUT, std::string RLHS, std::string RRHS);

    virtual void emitMove(std::string rdest, std::string rsrc);

    virtual void emitReserveStackSpae(uint64_t size);

    virtual std::string emitLoadFromStack(uint64_t align, uint64_t size, std::string load_reg = "");
    virtual void emitStoreToStack(uint64_t align, uint64_t size, std::string rsrc);
    virtual std::string emitLoadaddrFromStack(uint64_t align, std::string load_reg = "");

    virtual void emitCall(std::string name);

    virtual void emitPush(std::string reg);
    virtual void emitPop(std::string reg);

    virtual void emitSingleRet();
    virtual void emitLabel(std::string name);

    virtual TypeMetadata* getTypeFromName(std::string name);
  };
} // namespace hcc
