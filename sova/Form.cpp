#include "Form.h"

void Form::print(std::ostream &o) { o << "<form>"; }

Form *Form::as_form() { return this; }

type_t Form::get_type() { return TYPE_FORM; }
