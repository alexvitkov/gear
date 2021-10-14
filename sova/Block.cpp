#include "Block.h"
#include "Context.h"
#include "Function.h"
#include "Object.h"

Object *Block::interpret(Context &ctx, EvalFlags_t flags) {
  Context *child_ctx = new Context(&ctx);
  Object *val = nullptr;

  for (Object *obj : inside)
    val = eval(*child_ctx, obj, flags);

  if (flags & EVAL_BLOCK_RETURN_CONTEXT) {
    child_ctx->parent = nullptr;
    return child_ctx;
  } else {
    return val;
  }
}

void Block::print(std::ostream &o, int indent) {
  o << "{ ";

  indent++;
  for (Object *obj : inside) {
    if (obj) {
      print_obvject_newline(o, indent);
      obj->print(o, indent);
      o << ";";
    }
  }
  indent--;
  print_obvject_newline(o, indent);

  o << "}";
}

Block *Block::as_block() { return this; }

type_t Block::get_type() { return TYPE_AST_BLOCK; }

void Block::iterate_references(std::vector<Object *> &out) {
  for (auto o : inside)
    out.push_back(o);
}

class BlockPushFunction : public Function {
  Block *block;

public:
  BlockPushFunction(Block *block) : block(block) {}

  virtual Object *call_fn(Context &, std::vector<Object *> &args) override {
    for (auto arg : args)
      block->inside.push_back(arg);
    return nullptr;
  }
};

Object *Block::dot(Context &, std::string str) {
  if (str == "push")
    return new BlockPushFunction(this);
  return nullptr;
}
