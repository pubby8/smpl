#ifndef SMPL_META_PIPE_HPP_INCLUDED
#define SMPL_META_PIPE_HPP_INCLUDED

// Copyright Patrick Bene 2012
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// meta_pipe simply connects the output of one word to the input of the 
// remaining words. 

#if defined(BOOST_NO_VARIADIC_TEMPLATES)
# include <smpl/impl/remaining_param.hpp>

  namespace smpl {
    // TODO:
  
  } // namespace smpl
#else

  namespace smpl {

    // TODO: move into seperate header
    template <typename Stack>
    struct meta_pipe {
      // this (un-specialized) only catches empty parameter pack
      template <typename...>
      struct remaining_pipe {
        typedef Stack type;
      };
      template <
        typename Func,
        typename... Remaining
      >
      struct remaining_pipe<Func, Remaining...> 
      : Func::template apply<Stack>::template remaining_pipe<Remaining...>
      {};
    }; 
    
    // This function is for "top-level" pipes. It is only a wrapper around
    // meta_pipe to prevent some boiler-plate that would otherwise exist
    // in word definition code.
    template <typename Stack, typename Prefunc, typename Postfunc>
    struct meta_pred_checking_pipe {
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
  } // namespace smpl
#endif

#endif // SMPL_META_PIPE_HPP_INCLUDED

