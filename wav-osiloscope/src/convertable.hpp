#pragma once
#include <type_traits>
#include "meta.hpp"

namespace ouchi {

namespace detail {

template<int... Is>
struct seq { };

template<int N, int... Is>
struct gen_seq : gen_seq<N - 1, N - 1, Is...> { };

template<int... Is>
struct gen_seq<0, Is...> : seq<Is...> { };

template<typename T, typename F, int... Is>
void for_each(T&& t, F f, seq<Is...>) {
	auto l = { (f(std::get<Is>(t)), 0)... };
}

}//detail

template<typename... Ts, typename F>
void for_each_in_tuple(std::tuple<Ts...> const& t, F f) {
	detail::for_each(t, f, detail::gen_seq<sizeof...(Ts)>());
}
template<typename... Ts, typename F>
void for_each_in_tuple(std::tuple<Ts...>& t, F f) {
	detail::for_each(t, f, detail::gen_seq<sizeof...(Ts)>());
}

template<class T, class ...Converters>
class auto_convert {
	std::tuple<Converters...> converters_;
	T val_;
public:
	auto_convert(T && val, Converters &&...converters) :
		converters_{ std::move(converters)... },
		val_(std::move(val))
	{}
	auto_convert(T const & val, Converters &&...converters) :
		converters_{ std::move(converters)... },
		val_(val)
	{}
	auto_convert(Converters &&...converters) :
		converters_{ std::move(converters)... }
		val_(T{})
	{}

	template<class U>
	explicit operator U() const {
		U result;
		auto convert = [&result, this](auto & converter) {
			if constexpr (std::is_invocable_r_v<U, decltype(converter), T>)
				result = converter(val_);
			else if constexpr (std::is_convertible_v<T, U>)
				result =  static_cast<U>(val_);
			else
				throw std::runtime_error("T & U is not compatible");
		};
		for_each_in_tuple(converters_, convert);
		return result;
	}
	template<class U>
	auto_convert & operator=(U&& val) {
		auto convert = [&val, this](auto & converter) {
			if constexpr (std::is_invocable_r_v<T, decltype(converter), U>)
				val_ = converter(std::move(val));
			else if constexpr (std::is_convertible_v<T, U>)
				val_ =  static_cast<T>(val);
			else
				throw std::runtime_error("T & U is not compatible");
		};
		for_each_in_tuple(converters_, convert);
		return *this;
	}
	template<class U>
	auto_convert & operator=(U const & val) {
		auto convert = [&val, this](auto & converter) {
			if constexpr (std::is_invocable_r_v<T, decltype(converter), U>)
				val_ = converter(val);
			else if constexpr (std::is_convertible_v<T, U>)
				val_ =  static_cast<T>(val);
			else
				throw std::runtime_error("T & U is not compatible");
		};
		for_each_in_tuple(converters_, convert);
		return *this;
	}
};

}// ouchi