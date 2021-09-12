// Copyright Golden Hammer Software
#pragma once

#include <algorithm>

namespace GHStlUtil
{
	template<typename T>
	void deleteMe(T& t)
	{
		delete t;
	}
	
	template<typename CONTAINER, typename FUNCTOR>
	FUNCTOR for_all(CONTAINER& container, FUNCTOR func)
	{
		return std::for_each(container.begin(), container.end(), func);
	}
	
	template<typename CONTAINER>
	void deletePointerList(CONTAINER& container)
	{
		for_all(container, deleteMe<typename CONTAINER::value_type>);
		container.clear();
	}
	
	template<typename VECTOR>
	void eraseFromVector(VECTOR& container, typename VECTOR::value_type val)
	{
		typename VECTOR::iterator findIter = std::find(container.begin(), container.end(), val);
		if (findIter != container.end())
		{
			container.erase(findIter);
		}
	}
}
