// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_EXPECTED_ADAPTOR_HPP
#define BOOST_EXPECTED_ADAPTOR_HPP

#include <boost/move/move.hpp>
#include <boost/utility/result_of.hpp>

namespace boost
{

  template <class H>
  class adaptor_holder
  {
  public:
    typedef H holder_type;
    typedef typename H::funct_type funct_type;
    template <class E>
    struct bind
    {
      typedef typename H::template bind<E>::type type;
    };

    explicit adaptor_holder(funct_type f) :
      fct_(f)
    {
    }
    ;

    template <class E>
    typename H::template bind<E>::type operator()(E const&)
    {
      return typename H::template bind<E>::type(fct_);
    }
  private:
    funct_type fct_;
  };

  namespace detail
  {

    template <class E, class F, class V>
    class if_valued
    {
      F fct_;
    public:
      explicit if_valued(F f) :
        fct_(f)
      {
      }
      ;

      typedef typename E::value_type value_type;
      typedef typename E::template bind<typename result_of<F(value_type)>::type>::type result_type;

      result_type operator()(E e)
      {
        if (e.valid())
        {
          return result_type(fct_(*e));
        }
        else
        {
          return result_type(e.get_unexpected());
        }
      }
    };

    template <class E, class F, class R>
    class if_valued2
    {
      F fct_;
    public:
      explicit if_valued2(F f) :
        fct_(f)
      {
      }
      ;

      typedef void value_type;
      typedef typename E::template bind<R>::type result_type;

      result_type operator()(E e)
      {
        if (e.valid())
        {
          return result_type(fct_());
        }
        else
        {
          return result_type(e.get_unexpected());
        }
      }
    };

    template <class E, class F>
    class if_valued2<E, F, void>
    {
      F fct_;
    public:
      explicit if_valued2(F f) :
        fct_(f)
      {
      }
      ;

      typedef void value_type;
      typedef typename E::template bind<void>::type result_type;

      result_type operator()(E e)
      {
        if (e.valid())
        {
          fct_();
          return result_type();
        }
        else
        {
          return result_type(e.get_unexpected());
        }
      }
    };

    template <class E, class F>
    struct if_valued<E, F, void> : if_valued2<E, F, typename result_of<F()>::type>
    {

      explicit if_valued(F f) :
        if_valued2<E, F, typename result_of<F()>::type> (f)
      {
      }
      ;

    };

    template <class F>
    struct if_valued_adaptor
    {
      typedef F funct_type;
      template <class E>
      struct bind
      {
        typedef if_valued<E, funct_type, typename E::value_type> type;
      };
    };
  }

  template <class F>
  inline adaptor_holder<detail::if_valued_adaptor<F> > if_valued(F f)
  {
    return adaptor_holder<detail::if_valued_adaptor<F> > (f);
  }

  namespace detail
  {

    template <class F>
    class ident_t
    {
      F fct_;
    public:

      explicit ident_t(F f) :
        fct_(f)
      {
      }
      ;

      typedef typename result_of<F()>::type result_type;

      template <class G>
      result_type operator()(G e)
      {
        return fct_();
      }
    };
  }

  template <class F>
  inline detail::ident_t<F> ident(F f)
  {
    return detail::ident_t<F>(f);
  }

  namespace detail
  {

    template <class E, class F, class V>
    class if_unexpected
    {
      F fct_;
    public:
      typedef F funct_type;
      typedef E result_type;

      explicit if_unexpected(funct_type f) :
        fct_(f)
      {
      }
      ;

      E operator()(E e)
      {
        if (!e.valid())
        {
          return result_type(fct_(e.error()));
        }
        else
        {
          return boost::move(e);
        }
      }
    };

    template <class F>
    struct if_unexpected_adaptor
    {
      typedef F funct_type;
      template <class E>
      struct bind
      {
        typedef if_unexpected<E, funct_type, typename E::value_type> type;
      };
    };
  }

  template <class F>
  inline adaptor_holder<detail::if_unexpected_adaptor<F> > if_unexpected(F f)
  {
    return adaptor_holder<detail::if_unexpected_adaptor<F> > (f);
  }
} // namespace boost

#endif // BOOST_EXPECTED_ADAPTOR_HPP
