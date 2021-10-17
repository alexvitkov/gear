#pragma once

#include "Common.h"
#include "Vector.h"
#include "String.h"

class Context;
class GlobalContext;

namespace library {

#ifdef SOVA_LIB_CORE
namespace core {
void load(GlobalContext &ctx);
};
#endif

#ifdef SOVA_LIB_OS
namespace os {
void load(GlobalContext &ctx);
};
#endif

#ifdef SOVA_LIB_TEST
namespace test {
extern class Vector<String> failed_list;
void load(GlobalContext &ctx);
}; // namespace test
#endif

}; // namespace library
