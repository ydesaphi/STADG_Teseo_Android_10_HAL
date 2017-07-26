#ifndef TESEO_HAL_UTILS_UTILS
#define TESEO_HAL_UTILS_UTILS

template<class Container, class UnaryPredicate>
void erase_if(Container & c, typename Container::iterator begin, typename Container::iterator end, UnaryPredicate p)
{
	while(begin != end)
	{
		if(p(*begin))
		{
			begin = c.erase(begin);
		}
		else
		{
			++begin;
		}
	}
}

template<class Container, class UnaryPredicate>
void erase_if(Container & c, UnaryPredicate p)
{
	erase_if(c, c.begin(), c.end(), p);
}

#endif // TESEO_HAL_UTILS_UTILS