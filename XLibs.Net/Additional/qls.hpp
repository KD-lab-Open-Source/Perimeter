#ifndef ACHILLES_H_
#define ACHILLES_H_

#include <algorithm>
#include <boost/type_traits.hpp>
#include <iterator>

namespace Loki
{
	template <int v>
	struct Int2Type
	{
		enum { value = v };
	};
}

// quic linear search
namespace qls
{
	template <class Iter, class ValueType>
	Iter find_impl(Iter from, const Iter to, const ValueType& val, Loki::Int2Type<0>)
	{
		for (; from != to && !(*from == val); ++from)
		{
		}
		return from;
	}

	template <class Iter, class ValueType>
	Iter find_impl(Iter b, const Iter e, const ValueType& v, Loki::Int2Type<1>)
	{
        switch ((e - b) & 7u)
        {
        case 0:
            while (b != e)
            {
				if (*b == v) return b; ++b;
        case 7: if (*b == v) return b; ++b;
        case 6: if (*b == v) return b; ++b;
        case 5: if (*b == v) return b; ++b;
        case 4: if (*b == v) return b; ++b;
        case 3: if (*b == v) return b; ++b;
        case 2: if (*b == v) return b; ++b;
        case 1: if (*b == v) return b; ++b;
            }
        }
        return b;
	}

	template <class Iter, class ValueType>
	Iter find_impl(Iter b, Iter e, const ValueType& v, Loki::Int2Type<2>)
	{
		if (b == e) return e;
		std::iterator_traits<Iter>::value_type tmp(v);
		--e;
		std::swap(*e, tmp);
		for (; !(*b == v); ++b)
        {
        }            
		std::swap(*e, tmp);
		if (b == e && !(*b == v)) ++b;
		return b;
	}
	
	template <class Iter, class ValueType>
		Iter find(Iter from, Iter to, const ValueType& val)
	{
		typedef std::iterator_traits<Iter>::iterator_category Category;
		enum { isBidir = boost::is_same<
			Category, std::bidirectional_iterator_tag>::value };
		enum { isRand = boost::is_same<
			Category, std::random_access_iterator_tag>::value };
		typedef std::iterator_traits<Iter>::pointer PointerType;
		typedef boost::remove_pointer<PointerType>::type
			IteratedType;
		
		enum { isMutableSeq = !boost::is_const<
			IteratedType>::value };
		typedef std::iterator_traits<Iter>::value_type ValueType;
		enum { isPod = boost::is_POD<ValueType>::value };
		enum { selector = 
			(isBidir || isRand) && isPod && isMutableSeq ? 2 :
		(isRand ? 1 : 0) };
		Loki::Int2Type<selector> sel;
		return find_impl(from, to, val, sel);
	}
	
	//поиск с мутацией контейнера
	template<class Iter, class ValueType>
		Iter mutable_find(Iter b, Iter e, const ValueType& v)
	{
		return find_impl(b, e, v, Loki::Int2Type<2>());
	}
	
	//поиск с раскруткой цикла
	template<class Iter, class ValueType>
		Iter dough_find(Iter b, Iter e, const ValueType& v)
	{
		return find_impl(b, e, v, Loki::Int2Type<1>());
	}
}

#endif