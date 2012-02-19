#ifndef SMPL_IMPL_PRED_CHECKING_META_PIPE_HPP_INCLUDED
#define SMPL_IMPL_PRED_CHECKING_META_PIPE_HPP_INCLUDED

// Copyright Patrick Bene 2012
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// pred_checking_meta_pipe is for "top-level" pipes. It is only a wrapper
// around meta_pipe to prevent some boiler-plate that would otherwise exist
// in word definition code.

#include <smpl/meta_pipe.hpp>


#if defined(BOOST_NO_VARIADIC_TEMPLATES)
# include <smpl/impl/remaining_param.hpp>
// TODO:
  namespace smpl {
    namespace impl {
    
    } // namespace impl  
  } // namespace smpl
#else
  namespace smpl {
    namespace impl {
    
      template <typename Prefunc, typename Postfunc, typename Stack>
      struct pred_checking_meta_pipe {
        // this (un-specialized) only catches empty parameter pack
        template <typename...>
        struct remaining_pipe 
        : Prefunc::template apply<Stack>::template remaining_pipe<>
        {};
        template <
          typename Func,
          typename... Remaining
        >
        struct remaining_pipe<Func, Remaining...> 
        : meta_pipe<
            typename Prefunc::template apply<Stack>
          >::template remaining_pipe<Func, Postfunc, Remaining...>
        {};
      };
       
    } // namespace impl 
  } // namespace smpl
#endif

#endif // SMPL_IMPL_PRED_CHECKING_META_PIPE_HPP_INCLUDED

