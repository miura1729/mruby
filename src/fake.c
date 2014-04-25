#include <stdlib.h>

#include "mruby.h"
#include "mruby/array.h"
#include "mruby/string.h"
#include "mruby/class.h"

mrb_value
mrb_fake_conv_ary(mrb_state *mrb, mrb_value fake)
{
  mrb_sym name;
  mrb_value *a;
  int alen;
  mrb_value aself;
  mrb_value orig;

  mrb_get_args(mrb, "n*", &name, &a, &alen);
  orig = mrb_orignal_value(fake);
  aself = mrb_ary_new_from_values(mrb, 1, &orig);
  return mrb_funcall_argv(mrb, aself, name, alen, a);
}

void
mrb_init_fake(mrb_state *mrb)
{
  struct RClass *fake;

  fake = mrb->fake_class = mrb_define_class(mrb, "Fake", mrb->object_class);
  mrb_define_method(mrb, fake, "method_missing",          mrb_fake_conv_ary,          MRB_ARGS_ANY());  /* 15.3.1.3.30 */
}
