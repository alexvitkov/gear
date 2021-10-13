#include "Block.h"
#include "Context.h"
#include "Object.h"

Object *Block::interpret(Context &ctx, bool to_lvalue) {
  Context child_ctx(ctx);

  Object *val = nullptr;

  for (Object *obj : inside)
    val = eval(child_ctx, obj, to_lvalue);

  return val;
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

Type Block::get_type() { return TYPE_AST_BLOCK; }
