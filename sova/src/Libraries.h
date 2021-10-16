#pragma once

class Context;
class GlobalContext;

namespace library {

namespace core {
void load(Context &ctx);
};

namespace os {
void load(GlobalContext &ctx);
};

}; // namespace library
