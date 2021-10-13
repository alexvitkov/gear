#include "Block.h"
#include "Context.h"

Object *Block::interpret(Context &ctx, bool to_lvalue) {
  Context child_ctx(ctx);
  
  Object *val = nullptr;

  for (Object *obj : inside)
    val = eval(child_ctx, obj, to_lvalue);

  return val;
}

void Block::print(std::ostream &o) {
  o << "{ ";

  for (Object *obj : inside)
    o << obj << "; ";

  o << "}";
}

Block* Block::as_block() {
  return this;
}
