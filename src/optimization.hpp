#pragma once

namespace hcc {
  enum class Optimization { OPT_CONSTANT_FOLDING,
                            OPT_FUNCTION_BODY_ELIMINATION,
                            OPT_DCE,
                            OPT_FP_OMISSION,
                            OPT_STACK_RESERVE,
                            OPT_CONSTANT_PROPAGATION };
}