/*
** mruby/boxing_nan.h - nan boxing mrb_value definition
**
** See Copyright Notice in mruby.h
*/

#ifndef MRUBY_BOXING_NAN_H
#define MRUBY_BOXING_NAN_H

#ifdef MRB_USE_FLOAT
# error ---->> MRB_NAN_BOXING and MRB_USE_FLOAT conflict <<----
#endif

#ifdef MRB_INT64
# error ---->> MRB_NAN_BOXING and MRB_INT64 conflict <<----
#endif

#define MRB_FIXNUM_SHIFT 0
#define MRB_TT_HAS_BASIC MRB_TT_OBJECT

#ifdef MRB_ENDIAN_BIG
#define MRB_ENDIAN_LOHI(a,b) a b
#else
#define MRB_ENDIAN_LOHI(a,b) b a
#endif

/* value representation by nan-boxing:
 *   float : FFFFFFFFFFFFFFFF FFFFFFFFFFFFFFFF FFFFFFFFFFFFFFFF FFFFFFFFFFFFFFFF
 *   object: 1111111111110000 0000000000TTTTTT PPPPPPPPPPPPPPPP PPPPPPPPPPPPPPPP
 *   int   : 1111111111110000 0000000000TTTTTT IIIIIIIIIIIIIIII IIIIIIIIIIIIIIII
 *   sym   : 1111111111110000 0100000000TTTTTT SSSSSSSSSSSSSSSS SSSSSSSSSSSSSSSS
 * In order to get enough bit size to save TT, all pointers are shifted 2 bits
 * in the right direction. Also, TTTTTT is the mrb_vtype;
 * P...P is 32bit signed offset from mrb
 */
typedef struct mrb_value {
  union {
    mrb_float f;
    union {
      struct {
        MRB_ENDIAN_LOHI(
          uint32_t ttt;
          ,union {
	    int32_t p;
            mrb_int i;
            mrb_sym sym;
          };
        )
      };
    } value;
  };
} mrb_value;

#define mrb_float_pool(mrb,f) mrb_float_value(mrb,f)

#define mrb_tt(o)       (enum mrb_vtype)((o).value.ttt & 0xfffff)
#define mrb_type(o)     ((((uint32_t)0xfff00000 < (o).value.ttt) && ((o).value.ttt != 0xfff80000)) ? mrb_tt(o) : MRB_TT_FLOAT)
//#define mrb_ptr(o)      ((void*)((((uintptr_t)0x3fffffffffff)&((uintptr_t)((o).value.p)))<<2))
#define mrb_ptr(o)      ((void *)((intptr_t)mrb + (o).value.p))
#define mrb_float(o)    (o).f
#define mrb_cptr(o)     mrb_ptr(o)
#define mrb_fixnum(o)   (o).value.i
#define mrb_symbol(o)   (o).value.sym

#define BOXNAN_SET_VALUE(o, tt, attr, v) do {\
  (o).value.ttt = (0xfff00000|(tt));	     \
  switch (tt) {\
  case MRB_TT_FALSE:\
  case MRB_TT_TRUE:\
  case MRB_TT_UNDEF:\
  case MRB_TT_FIXNUM:\
  case MRB_TT_SYMBOL: (o).attr = (mrb_int)((intptr_t)v); break;		\
  default: (o).value.p = ((int32_t)((intptr_t)(v) - (intptr_t)mrb));break; \
  }\
} while (0)

#define SET_FLOAT_VALUE(mrb,r,v) do { \
  if (v != v) { \
    (r).value.ttt = 0x7ff80000; \
    (r).value.i = 0; \
  } else { \
    (r).f = v; \
  }} while(0)

#define SET_NIL_VALUE(r) BOXNAN_SET_VALUE(r, MRB_TT_FALSE, value.i, 0)
#define SET_FALSE_VALUE(r) BOXNAN_SET_VALUE(r, MRB_TT_FALSE, value.i, 1)
#define SET_TRUE_VALUE(r) BOXNAN_SET_VALUE(r, MRB_TT_TRUE, value.i, 1)
#define SET_BOOL_VALUE(r,b) BOXNAN_SET_VALUE(r, b ? MRB_TT_TRUE : MRB_TT_FALSE, value.i, 1)
#define SET_INT_VALUE(r,n) BOXNAN_SET_VALUE(r, MRB_TT_FIXNUM, value.i, (n))
#define SET_SYM_VALUE(r,v) BOXNAN_SET_VALUE(r, MRB_TT_SYMBOL, value.sym, (v))
#define SET_OBJ_VALUE(r,v) BOXNAN_SET_VALUE(r, (((struct RObject*)(v))->tt), value.p, (v))
#define SET_CPTR_VALUE(mrb,r,v) BOXNAN_SET_VALUE(r, MRB_TT_CPTR, value.p, v)
#define SET_UNDEF_VALUE(r) BOXNAN_SET_VALUE(r, MRB_TT_UNDEF, value.i, 0)

#endif  /* MRUBY_BOXING_NAN_H */
