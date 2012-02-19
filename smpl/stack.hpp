#ifndef SMPL_STACK_HPP_INCLUDED
#define SMPL_STACK_HPP_INCLUDED

// Copyright Patrick Bene 2012
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

#include <smpl/limits/stack.hpp>
#include <smpl/impl/static_error.hpp>

#include <boost/mpl/arithmetic.hpp>

#include <boost/preprocessor/iteration.hpp>
#include <boost/preprocessor/repetition.hpp>

#include <boost/config.hpp>

namespace smpl {
  namespace impl {
  
    // This type trait is used to simplify default stack specializations.
    // Default stacks are ones included in this library.
    template <typename T>
    struct is_default_stack {
      typedef char yes[1];
      typedef char no[2];
      template <typename S>
      static yes& test(typename S::default_stack_trait*);   
      template <typename>
      static no& test(...);
      static const bool value = sizeof(test<T>(0)) == sizeof(yes);
    };
  
  } // namespace smpl
} // namespace smpl

// Free stack functions gain access to implementation-defined stack internals.
// This macro is to prevent code duplication of the friend declarations.
#define SMPL_IMPL_stack_friends \
template <typename Stack, unsigned N, bool Is_default>\
friend struct stack_drop;\
template <typename Stack, unsigned N, bool Is_default>\
friend struct stack_take;\
template <typename T>\
friend struct impl::is_default_stack
// The code for the un-specialized stack functions is almost identical to
// the smpl::stack specialization, and these macros are defined to prevent
// code duplication
#define SMPL_IMPL_stack_drop(Stack, N, macro) \
typename stack_drop<typename Stack::macro(pop)::type, N - 1>::type
#define SMPL_IMPL_stack_take(Stack, N, macro) \
typename stack_take<typename Stack::macro(pop)::type, N - 1>\
::type::template push<typename Stack::macro(top)::type>::type
// The following are valid macros for the previous macro parameters 'macro'
#define SMPL_IMPL_nondefault(type) type 
#define SMPL_IMPL_default(type) template type<char>

  namespace smpl {

    // non-default stack free functions
    template <
      typename Stack,
      unsigned N,
      bool Is_default = impl::is_default_stack<Stack>::value>
    struct stack_drop {
      typedef SMPL_IMPL_stack_drop(Stack, N, SMPL_IMPL_nondefault) type;
    };
    template <typename Stack,bool Is_default>
    struct stack_drop<Stack, 0, Is_default> {
      typedef Stack type;
    }; 
    template <typename Stack>
    struct stack_drop<Stack, 0, true>
    : stack_drop<Stack, 0, false>
    {};
    template <
      typename Stack,
      unsigned N,
      bool Is_default = impl::is_default_stack<Stack>::value>
    struct stack_take {
      typedef SMPL_IMPL_stack_take(Stack, N, SMPL_IMPL_nondefault) type;
    };
    
    // default
    template <typename Stack, bool Is_default>
    struct stack_take<Stack, 0, Is_default>
    : Stack::empty_type
    {};
    template <typename Stack>
    struct stack_take<Stack, 0, true>
    : stack_take<Stack, 0, false>
    {};
    
    
  } // namespace smpl

# if defined(BOOST_NO_VARIADIC_TEMPLATES)

#   include <smpl/impl/old_variadic_param.hpp>
#   include <smpl/impl/none.hpp>

#   include <boost/preprocessor/arithmetic/sub.hpp>
#   include <boost/preprocessor/repetition/enum.hpp>
#   include <boost/preprocessor/repetition/enum.hpp>

    namespace smpl {

      template <
        BOOST_PP_ENUM(
          SMPL_LIMIT_STACK_SIZE,
          SMPL_IMPL_VARIADIC_PARAM_DEFAULT,
          ~)>
      struct stack {
        SMPL_IMPL_stack_friends;
        typedef stack type;
        private:
          typedef void default_stack_trait;
          template <typename> // dummy param to prevent static_assert
          struct top {        // from being compiled out
            typedef SMPL_IMPL_VARIADIC(~,0,~) type;
          };
          template <typename>
          struct pop {
            typedef stack<
              BOOST_PP_ENUM_SHIFTED(
                SMPL_LIMIT_STACK_SIZE,
                SMPL_IMPL_VARIADIC,
                ~),
              impl::none>
            type;
          };
          template <typename T>
          struct push
          : stack<
            T,
            BOOST_PP_ENUM(
              BOOST_PP_SUB(SMPL_LIMIT_STACK_SIZE, 1),
              SMPL_IMPL_VARIADIC,
              ~)>
          {};
          typedef stack<> empty_type;
      };
      
      template <typename Stack, unsigned N>
      struct stack_drop<Stack, N, true> {
        typedef SMPL_IMPL_stack_drop(Stack, N, SMPL_IMPL_default) type;
      };
      
      template <typename Stack, unsigned N>
      struct stack_take<Stack, N, true> {
        typedef SMPL_IMPL_stack_drop(Stack, N, SMPL_IMPL_default) type;
      };
      
    } // namespace smpl

# else

    namespace smpl {

      template <typename...>
      struct stack;
      template <typename X, typename... Xs>
      struct stack {
        typedef stack type;
        typedef stack<> empty;
        template <typename>
        struct bottom {
          typedef X type;
        };
        template <typename T>
        struct push {
          typedef stack<X, Xs..., T> type;
        };
        template <typename T>
        struct insert_bottom {
          typedef stack<T, X, Xs...> type;
        };
        template <typename>
        struct erase_bottom {
          typedef stack<Xs...> type;
        };
      };
      template <>
      struct stack<> {
        typedef stack type;
        template <typename Dummy>
        struct bottom {
          SMPL_IMPL_STATIC_TEMPLATE_ERROR(
            Dummy,
            "accessing top of empty rstack"); // technically bottom - it works
        };
        template <typename T>
        struct push {
          typedef stack<T> type;
        };
        template <typename T>
        struct insert_bottom {
          typedef stack<T> type;
        };
        template <typename Dummy>
        struct erase_bottom {
          SMPL_IMPL_STATIC_TEMPLATE_ERROR(
            Dummy,
            "popping top of empty rstack"); // technically bottom - it works
        };
      };
      
      template <typename...>
      struct rstack;
      template <typename X, typename... Xs>
      struct rstack {
        typedef rstack type;
        typedef rstack<> empty;
        template <typename>
        struct top {
          typedef X type;
        };
        template <typename T>
        struct push {
          typedef rstack<T, X, Xs...> type;
        };
        template <typename>
        struct pop {
          typedef rstack<Xs...> type;
        };
      };
      template <>
      struct rstack<> {
        typedef stack type;
        template <typename Dummy>
        struct top {
          SMPL_IMPL_STATIC_TEMPLATE_ERROR(
            Dummy,
            "accessing top of empty stack");
        };
        template <typename T>
        struct push {
          typedef rstack<T> type;
        };
        template <typename Dummy>
        struct pop {
          SMPL_IMPL_STATIC_TEMPLATE_ERROR(
            Dummy,
            "popping top of empty stack");
        };
      };
      
      //TODO:
      template <typename... Xs, unsigned N>
      struct stack_drop<stack<Xs...>, N> {
        typedef type;
      };
      
      template <typename... Xs, unsigned N>
      struct stack_takerstack<Xs...>, N, true> {
        typedef type;
      };
      
      //
      
      template <typename... Xs, unsigned N>
      struct stack_drop<rstack<Xs...>, N> {
        typedef SMPL_IMPL_stack_take(rstack<Xs...>, N, SMPL_IMPL_default) type;
      };
      
      template <typename... Xs, unsigned N>
      struct stack_take<rstack<Xs...>, N, true> {
        typedef SMPL_IMPL_stack_drop(rstack<Xs...>, N, SMPL_IMPL_default) type;
      };
      //
      
      
      /////////
      
      template <typename...>
      struct stack;

      template <typename Stack, unsigned N>
      using stack_index = typename stack_drop<Stack, N>::top;
      
      template <typename Top, typename... Rest>
      struct stack<Top, Rest...> {
        SMPL_IMPL_stack_friends;
        private:
          typedef void default_stack_trait;
          template <typename>
          struct top {
            typedef Top type;
          };
          template <typename>
          struct pop {
            typedef stack<Rest...> type;
          };
          template <typename T>
          struct push
          : stack<T, Top, Rest...>
          {};
          typedef stack<> empty_type;
        public:
          typedef stack type;
          template <template <typename...> class Func>
          struct apply : Func<Top, Rest...> {};
      };
      template <>
      struct stack<> {
        SMPL_IMPL_stack_friends;
        private:
          template <typename Dummy>
          struct top {
            SMPL_IMPL_STATIC_TEMPLATE_ERROR(Dummy,"accessing top of empty stack");
          };
          template <typename Dummy>
          struct pop {
            SMPL_IMPL_STATIC_TEMPLATE_ERROR(Dummy,"popping on an empty stack");
          };
          template <typename... T>
          struct push 
          : stack<T...>
          {};
          typedef stack<> empty_type;
      };

      template <typename Stack, unsigned N>
      struct stack_drop<Stack, N, true> {
        typedef SMPL_IMPL_stack_drop(Stack, N, SMPL_IMPL_default) type;
      };
      
      template <typename Stack, unsigned N>
      struct stack_take<Stack, N, true> {
        typedef SMPL_IMPL_stack_drop(Stack, N, SMPL_IMPL_default) type;
      };
      
    } // namespace smpl

# endif

#undef SMPL_IMPL_nondefault
#undef SMPL_IMPL_default
#undef SMPL_IMPL_stack_drop
#undef SMPL_IMPL_stack_take
#undef SMPL_IMPL_stack_friends

#endif // SMPL_STACK_HPP_INCLUDED

