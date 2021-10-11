#include "Block.h"
#include "Context.h"

Object *Block::interpret(Context *ctx) {
  Context child_ctx(ctx);
  
  Object *val = nullptr;

  for (Object *obj : inside)
    val = eval(&child_ctx, obj);

  return val;
}

void Block::print(std::ostream &o) {
  o << "{ ";

  for (Object *obj : inside)
    o << obj << "; ";

  o << "}";
}
