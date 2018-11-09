#pragma once
#include <utility>

namespace ouchi {

class is_callable_r_impl {
public:
	template<class F, class R, class ...Args>
	static auto check(...)
		->std::false_type;

	template<class F, class R, class ...Args>
	static auto check(F*, Args...)
		->decltype(R{std::declval<F>()(std::declval<Args>()...)}, std::true_type{});
};

template<class F, class R, class ...Args>
class is_callable_r :
	public decltype(is_callable_r_impl::check<F, R, Args...>(nullptr, std::declval<Args>()...))
{};


}
