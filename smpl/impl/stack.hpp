#ifndef SMPL_STACK_HPP_INCLUDED
#define SMPL_STACK_HPP_INCLUDED

// Copyright Patrick Bene 2012
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/cstdint.hpp>

#include <boost/mpl/arithmetic.hpp>

#include <boost/preprocessor/iteration.hpp>
#include <boost/preprocessor/repetition.hpp>

#include <boost/config.hpp>

#define SMPL_DEFINE_WORD(name, ...) \
struct name : word<__VA_ARGS__> {};

#define SMPL_DEFINE_WORD_USING_NS(name, ...) \
inline namespace {\
  using namespace smpl;\
  template <typename t>\
  using name = typename exec<__VA_ARGS__>::template remaining_pipe<t>;\
}

#if defined(BOOST_NO_VARIADIC_TEMPLATES)

#include <boost/mpl/erase.hpp>
#include <boost/mpl/minus.hpp>
#include <boost/mpl/vector.hpp>

namespace smpl {
  using namespace boost;

  typedef mpl::vector<int, float, char, double> empty_stack;
  
  template <typename Stack, unsigned N>
  struct stack_index
  : mpl::at<Stack, N>
  {};
  
  template <typename Stack>
  struct stack_top
  : mpl::front<Stack>
  {};
  
  template <typename Stack, unsigned N>
  struct stack_drop
  : mpl::erase<
      Stack, 
      typename mpl::begin<Stack>::type,
      typename mpl::advance<
        typename mpl::begin<Stack>::type,
        mpl::int_<N>
      >::type
    >
  {};
  
  template <typename Stack, unsigned N>
  struct stack_take
  : mpl::erase<
      Stack, 
      typename mpl::template advance<
        typename mpl::template end<Stack>::type,
        mpl::int_<-N>
      >::type,
      typename mpl::template end<Stack>::type
    >
  {}; 
} // namespace smpl

#else

namespace smpl {

  template <typename...>
  struct stack;
  template <typename Stack, unsigned N>
  struct stack_drop;
  template <typename Stack, unsigned N>
  struct stack_take;
  template <typename Stack, unsigned N>
  using stack_index = typename stack_drop<Stack, N>::top;
  
  template <typename Top, typename... Rest>
  struct stack<Top, Rest...> {
    template <typename... T>
    using push = stack<T..., Top, Rest...>;
    using top  = Top;
    using pop = stack<Rest...>;
    using type = stack<Top, Rest...>;
    template <template <typename...> class Func>
    struct apply {
      using type = typename Func<Top, Rest...>::type;
    };
  };
  template <>
  struct stack<> {
    template <typename... T>
    using push = stack<T...>;
  };

  template <typename Stack, unsigned N>
  struct stack_drop 
  : stack_drop<typename Stack::pop, N - 1>
  {};
  template <typename Stack>
  struct stack_drop<Stack, 0> 
  : Stack 
  { using type = Stack; };
  
  template <typename Stack, unsigned N>
  struct stack_take {
    using type = typename stack_take<typename Stack::pop, N - 1>::type::template push<typename Stack::top>;
  };
  template <typename Stack>
  struct stack_take<Stack, 0> {
    using type = stack<>;
  };
  
} // namespace smpl

#endif

#endif // SMPL_STACK_HPP_INCLUDED

