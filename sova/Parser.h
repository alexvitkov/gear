#pragma once
#include "Context.h"
#include <string>
#include <unordered_map>
#include <vector>

void set_infix_precedence(std::string op, int precedence, bool is_right_assoc);

bool parse(const char *code, Object **out);
