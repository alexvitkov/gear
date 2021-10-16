#include "Block.h"
#include "Context.h"
#include "Function.h"
#include "Object.h"
#include <assert.h>

Object *Block::interpret() {
  Context *child_ctx = 0;

  if (create_own_context) {
    child_ctx = new Context(context_stack.back());
    context_stack.push_back(child_ctx);
  }

  Object *val = nullptr;

  for (Object *obj : inside)
    val = eval(obj);

  if (create_own_context)
    context_stack.pop_back();

  if (eval_block_return_context) {
    assert(create_own_context);
    child_ctx->parent = nullptr;
    return child_ctx;
  } else {
    return val;
  }
}

void Block::print(Ostream &o) {
  o << "{ ";

  indent++;
  for (Object *obj : inside) {
    if (obj) {
      print_obvject_newline(o);
      o << obj << ";";
    }
  }
  indent--;
  print_obvject_newline(o);

  o << "}";
}

Block *Block::as_block() { return this; }

type_t Block::get_type() { return TYPE_AST_BLOCK; }

void Block::iterate_references(Vector<Object *> &out) {
  for (auto o : inside)
    out.push_back(o);
}

class BlockPushFunction : public Function {
  Block *block;

public:
  BlockPushFunction(Block *block) : block(block) {}

  virtual Object *call(Vector<Object *> &args) override {
    for (auto arg : args)
      block->inside.push_back(arg);
    return nullptr;
  }
};

Object *Block::dot(String str) {
  if (str == "push")
    return new BlockPushFunction(this);
  return nullptr;
}

Object *Block::clone() {
  Block *b = new Block();
  for (auto o : inside)
    b->inside.push_back(::clone(o));
  return b;
}
