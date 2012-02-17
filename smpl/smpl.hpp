#ifndef SMPL_SMPL_HPP_INCLUDED
#define SMPL_SMPL_HPP_INCLUDED

// Copyright Patrick Bene 2012
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/cstdint.hpp>

#include <boost/mpl/arithmetic.hpp>

#include <boost/preprocessor/iteration.hpp>
#include <boost/preprocessor/repetition.hpp>

#include <smpl/impl/stack.hpp>

#define SMPL_DEFINE_RAW_WORD() <smpl/impl/define_raw_word.hpp>

namespace smpl {
  
  template <typename T, T Value>
  struct value_wrapper;
  template <typename T>
  struct type_wrapper;
  
  template <typename Stack>
  struct pipe;
  template <typename... Words>
  struct eval;
  template <typename... Words>
  struct exec;
  
  
  template <template <typename...> class Func>
  struct func_to_word;

//////////////////////////////////////////////////////////////////////////////
// Wrapper Definitions
//////////////////////////////////////////////////////////////////////////////

/*
template <int i>
struct intc_ {
  template <typename STACK>
  struct intc__ {
    template <template <typename...> class... xs>
    using type = typename chain<typename STACK::template cons<Int<i>>>::template type<xs...>;
  };
};
#define intc(x) intc_<x>::intc__
*/

  template <typename T, T Value>
  struct value_wrapper {
    using value_type = T;
    constexpr static T value = Value;
  };

// essentially a wrapper that "pushes" the type onto the stack
  template <typename T>
  struct type_wrapper {
    //constexpr static typename T::value_type value = T::value;
    using value_type = T;
    template <typename Stack>
    struct apply {
      template <typename... Remaining>
      struct remaining_pipe
      : pipe<typename Stack::template push<T>>
      ::template remaining_pipe<Remaining...>
      {};
      //template <template <typename...> class... Remaining>
      //using type = typename remaining_pipe<Remaining...>::type;
    };
  };

// define builtin-types of the form name_<>
// and macros of form SMPL_NAME()
# define SMPL_WRAPPER_DEF_(new_name, type_name) \
  template<type_name VALUE>\
  using new_name = value_wrapper<type_name, VALUE>

#   define SMPL_TYPE(name, value) \
    smpl::type_wrapper<name<value>>::stack_func
    
    SMPL_WRAPPER_DEF_(char_,   char);
    SMPL_WRAPPER_DEF_(schar_,  signed char);
    SMPL_WRAPPER_DEF_(uchar_,  unsigned char);
    SMPL_WRAPPER_DEF_(short_,  short);
    SMPL_WRAPPER_DEF_(ushort_, unsigned short);
    SMPL_WRAPPER_DEF_(int_,    int);
    SMPL_WRAPPER_DEF_(uint_,   unsigned int);
    SMPL_WRAPPER_DEF_(long_,   long);
    SMPL_WRAPPER_DEF_(ulong_,  unsigned long);
    
#   define SMPL_CHAR(value)   SMPL_TYPE(smpl::char_,   value)
#   define SMPL_SCHAR(value)  SMPL_TYPE(smpl::schar_,  value)
#   define SMPL_UHAR(value)   SMPL_TYPE(smpl::uchar_,  value)
#   define SMPL_SHORT(value)  SMPL_TYPE(smpl::short_,  value)
#   define SMPL_USHORT(value) SMPL_TYPE(smpl::ushort_, value)
#   define SMPL_INT(value)    SMPL_TYPE(smpl::int_,    value)
#   define SMPL_UINT(value)   SMPL_TYPE(smpl::uint_,   value)
#   define SMPL_LONG(value)   SMPL_TYPE(smpl::long_,   value)
#   define SMPL_ULONG(value)  SMPL_TYPE(smpl::ulong_,  value)
    
#   ifdef BOOST_HAS_LONG_LONG
      SMPL_WRAPPER_DEF_(longlong_,  long long);
      SMPL_WRAPPER_DEF_(ulonglong_, unsigned long long);
#     define SMPL_LONGLONG(value)   SMPL_TYPE(smpl::longlong_,   value)
#     define SMPL_ULONGLONG(value)  SMPL_TYPE(smpl::ulonglong_,  value)
#   endif

    SMPL_WRAPPER_DEF_(int8_t_,   boost::int8_t);
    SMPL_WRAPPER_DEF_(uint8_t_,  boost::uint8_t);
    SMPL_WRAPPER_DEF_(int16_t_,  boost::int16_t);
    SMPL_WRAPPER_DEF_(uint16_t_, boost::uint16_t);
    SMPL_WRAPPER_DEF_(int32_t_,  boost::int32_t);
    SMPL_WRAPPER_DEF_(uint32_t_, boost::uint32_t);
    
#   ifndef BOOST_NO_INT64_T
      SMPL_WRAPPER_DEF_(int64_t_,  boost::int64_t);
      SMPL_WRAPPER_DEF_(uint64_t_, boost::uint64_t);
#   endif

# undef SMPL_WRAPPER_DEF_

//////////////////////////////////////////////////////////////////////////////
// Evaluation definitions
//////////////////////////////////////////////////////////////////////////////

/*
template <typename t>
struct chain {
  template <template <typename...> class... xs>
  struct foo {
    using type = t;
  };
  template <template <typename...> class x, template <typename...> class... xs>
  struct foo<x, xs...> {
    using type = typename x<t>::template type<xs...>;
  };
  template <template <typename...> class... xs>
  using type = typename foo<xs...>::type;
};

template <template <typename...> class... xs>
struct eval {
  using type = typename chain<stack<>>::template type<xs...>;
};
*/

  template <typename Stack>
  struct pipe {
    // this only catches empty parameter pack
    template <typename...>
    struct remaining_pipe {
      using type = Stack;
    };
    template <
      typename Func,
      typename... Remaining
    >
    struct remaining_pipe<Func, Remaining...> 
    : Func::template apply<Stack>::template remaining_pipe<Remaining...>
    {};
    //template <template <typename...> class... xs>
    //using type = typename remaining_pipe<xs...>::type;
  };
  
  template <typename... Words>
  struct eval {
    template <typename T = stack<>>
    using apply =
      typename pipe<T>::template remaining_pipe<Words...>::type;
  };
  
  template <typename... Words>
  struct eval_kludge {
    template <typename T = stack<>>
    using stack_func =
      typename pipe<stack<>>::template remaining_pipe<Words...>::type;
  };
  
  template <typename... Words>
  struct word {
    template <typename Stack>
    struct apply {
      template <typename... Remaining>
      struct remaining_pipe {
        using type = typename pipe<
          typename eval<Words...>::template apply<Stack>
        >::template remaining_pipe<Remaining...>::type;
      };
    };
  };
  
//////////////////////////////////////////////////////////////////////////////
// Function util definitions
//////////////////////////////////////////////////////////////////////////////

// In order to determine argument numbers, the template must be specialized
// for each argument number.
# define SMPL_TYPENAME_(z,n,data) typename
#   define BOOST_PP_LOCAL_MACRO(arg_num) \
    template <\
      template <BOOST_PP_ENUM(arg_num, SMPL_TYPENAME_,), typename...> class Func\
    >\
    struct func##arg_num##_to_word {\
      public:\
      template <typename Stack>\
      struct apply {\
        template <template <typename...> class... Remaining>\
        struct remaining_pipe {\
          using type = typename pipe<\
            typename stack_drop<Stack, arg_num>::type::template push<\
              typename stack_take<\
                Stack, arg_num\
              >::type::template apply<Func>::type\
            >\
          >::template remaining_pipe<Remaining...>::type;\
        };\
      };\
    };\
    template <template <BOOST_PP_ENUM(arg_num, SMPL_TYPENAME_,)> class Func>\
    struct func_to_word<Func> {\
      template <typename Stack>\
      using apply =\
        typename func##arg_num##_to_word<Func>\
        ::template word<Stack>;\
    };
      
#   define BOOST_PP_LOCAL_LIMITS (1, 4)
#   include BOOST_PP_LOCAL_ITERATE()
# undef SMPL_FUNC_TO_WORD_
# undef SMPL_TYPENAME_

struct swap {
  template <typename Stack>
  struct apply {
    template <typename... Remaining>
    struct remaining_pipe {
      using type = typename pipe<
        typename stack_drop<Stack, 2>
        ::template push<typename Stack::top>
        ::template push<stack_index<Stack,1>>
      >::template remaining_pipe<Remaining...>::type;
    };
  };
};

#define SMPL_RAW_WORD_NAME dup
#define SMPL_RAW_WORD_BODY(Stack) \
typename Stack::template push<typename Stack::top>
#include SMPL_DEFINE_RAW_WORD()

//TODO:
#define SMPL_WORD(stack) \
typename stack::template push<typename stack::top>

SMPL_DEFINE_WORD(add,     func2_to_word<boost::mpl::minus>);
SMPL_DEFINE_WORD(minus,   func2_to_word<boost::mpl::minus>);
SMPL_DEFINE_WORD(mul,     func2_to_word<boost::mpl::times>);
SMPL_DEFINE_WORD(divides, func2_to_word<boost::mpl::divides>);
SMPL_DEFINE_WORD(modulus, func2_to_word<boost::mpl::modulus>);

//SMPL_DEFINE_WORD(sub, swap, minus);
SMPL_DEFINE_WORD(div, swap, divides);

//using add = additive_func_to_word<add_>;

/*  
template <typename STACK>
struct add {
  template <template <typename...> class... xs>
  using type = typename chain<
    typename STACK::tail::tail::template cons<
      Int<indv(0) + indv(1)>
    >
  >::template type<xs...>;
};
*/
} // namespace smpl


//#include <stdio.h>
/*
#define indv(x) index<STACK, x>::type::head::value
#define indt(x) typename index<STACK, x>::type::head
#define indr(x) typename index<STACK, x>::type

#define cf template <typename...> class

template <int x>
struct Int {
  static const auto value = x;
};

template <typename... xs>
struct stack {
  template <typename... t>
  using cons = stack<t...>;
};

template <typename x, typename... xs>
struct stack<x, xs...> {
  template <typename... t>
  using cons = stack<t..., x, xs...>;
  using head = x;
  using tail = stack<xs...>;
  //template <int i>
 // using index = typename index_<i>::type;
};

template <typename t, int i>
struct index {
  using type = typename index<typename t::tail, i-1>::type;
};
template <typename t>
struct index<t, 0> {
  using type = t;
};


template <typename t>
struct chain {
  template <template <typename...> class... xs>
  struct foo {
    using type = t;
  };
  template <template <typename...> class x, template <typename...> class... xs>
  struct foo<x, xs...> {
    using type = typename x<t>::template type<xs...>;
  };
  template <template <typename...> class... xs>
  using type = typename foo<xs...>::type;
};

template <template <typename...> class... xs>
struct eval {
  using type = typename chain<stack<>>::template type<xs...>;
};



template <template <typename...> class... xs>
struct exec {
  
  //struct foo {
  //  using type = typename chain<ttt>::template type<xs...>;
  //};
  //template <class ttt>
  template <typename t>
  struct exec_ {
    template <template <typename...> class... ys>
    using type = typename chain<typename eval<xs...>::type>::template type<ys...>;
  };
  //using type = typename foo::type;
};


template <int i>
struct intc_ {
  template <typename STACK>
  struct intc__ {
    template <template <typename...> class... xs>
    using type = typename chain<typename STACK::template cons<Int<i>>>::template type<xs...>;
  };
};
#define intc(x) intc_<x>::intc__


template <typename STACK>
struct add {
  template <template <typename...> class... xs>
  using type = typename chain<
    typename STACK::tail::tail::template cons<
      Int<indv(0) + indv(1)>
    >
  >::template type<xs...>;
};

template <typename STACK>
struct mul {
  template <template <typename...> class... xs>
  using type = typename chain<
    typename STACK::tail::tail::template cons<
      Int<indv(0) * indv(1)>
    >
  >::template type<xs...>;
};

#define def_word(name, ...) \
template <typename t>\
using name = exec<__VA_ARGS__>::exec_<t>

def_word(five_five, intc(5), intc(5));

int main() {
  //quote<five,five>::type<stack<>> y;
  int x = eval<five_five, add, intc(5), mul>::type::head::value;
  printf("%i", x);
  //teval<Int<5>::type, Int<6>::type> x;
}


/*
template <typename x>
struct Int_ {
  template <cf r, cf... rs>
  using type = typename r<x>::template type<rs...>;
};

template <int x>
struct BoxedInt {
  static const auto value = x;
};

template<int x>
using Int = Int_<BoxedInt<x>>;

template <typename t, int i>
struct Index {
  using type = typename Index<typename t::tail, i-1>::type;
};
template <typename t>
struct Index<t, 0> {
  using type = t;
};

template <typename... xs>
struct Stack {
};

template <typename x, typename... xs>
struct Stack<x, xs...> {
  using head = x;
  using tail = Stack<xs...>;
  //template <int i>
 // using index = typename index_<i>::type;
};

template <typename STACK>
struct add {
  using type = Stack<
    Int<indv(0) + indv(1)>
  , indr(2)>;
};

template <typename STACK>
struct sub {
  using type = Stack<
    Int<indv(0) - indv(1)>
  , indr(2)>;
};

template <cf... xs>
struct eval {
  using type = Int_<void>::type<xs...>;
};

//eval<int, int, add>;
*/
#endif // SMPL_SMPL_HPP_INCLUDED

