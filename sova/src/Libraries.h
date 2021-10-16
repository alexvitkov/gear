#pragma once

#include "Common.h"
class Context;
class GlobalContext;

namespace library {

#ifdef SOVA_LIB_CORE
namespace core {
void load(Context &ctx);
};
#endif

#ifdef SOVA_LIB_OS
namespace os {
void load(GlobalContext &ctx);
};
#endif

#ifdef SOVA_LIB_TEST
namespace test {
void load(GlobalContext &ctx);
};
#endif

}; // namespace library
