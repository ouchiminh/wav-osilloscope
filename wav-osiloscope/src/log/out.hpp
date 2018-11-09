#pragma once
#include <time.h>
#include <iostream>
#include <functional>
#include <optional>
#include <string>
#include <vector>
#include <variant>
#include <fstream>
#include <sstream>
#include <utility>
#include "format.hpp"

namespace ouchi::log{

inline auto default_out = [](std::string_view str) { std::clog << str << std::endl; };

template<class CharT>
class out_func {
	inline static std::map<std::string, std::basic_ofstream<CharT>> files_;
	std::string my_file_;

	void open_file() {
		if (files_.count(my_file_)) return;
		files_.insert_or_assign(my_file_, std::basic_ofstream<CharT>(my_file_));
	}
public:
	out_func(std::string && filename) : my_file_(std::move(filename)) { open_file(); }
	out_func(std::string const & filename) : my_file_(filename) { open_file(); }
	out_func() = default;
	void set(std::string_view str) {
		my_file_ = str;
		open_file();
	}

	void operator() (std::basic_string_view<CharT> str) {
		files_.at(my_file_) << str << std::endl;
	}
};

/// <summary>
/// this class writes log.
/// </summary>
/// <param name="Func">このクラスの()演算子に渡された文字列が出力されます。operator()(std::basic_string_view)</param>
/// <param name="Args">Funcのコンストラクタに渡す引数。</param>
template<class CharT>
class basic_out {
	inline static constexpr char category_str[][32] = {
		"[TRACE]","[DEBUG]","[INFO ]", "[WARN ]", "[ERROR]","[FATAL]"
		//"[FATAL]", "[ERROR]", "[WARN ]", "[INFO ]", "[DEBUG]", "[TRACE]"
	};
	using Func = std::function<void(std::basic_string_view<CharT>)>;
	Func f_;
	message_format<CharT> format_;
	// visitorのためのヘルパー型。
	template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
	template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

public:
	basic_out(Func && func = default_out) : f_(std::forward<Func>(func)) {
		std::basic_ostringstream<CharT> ss;
		format_ << time << ss.widen(' ') << cat << ss.widen(' ') << msg;
	}

	void out(cat_v c, std::basic_string_view<CharT> msg) {
		std::basic_string<CharT> str;
		for (msg_element<CharT> const & i : format_)
			std::visit(
				overloaded{
					[&str](Time) { str.append(time_to_ISO8601<CharT>()); },
					[&](Category) { std::basic_string<CharT> result; for (auto const & i : category_str[(unsigned)c]) { if (i == '\0') break; result.push_back(std::basic_ofstream<CharT>().widen(i)); } str.append(result); },
					[&msg, &str](Msg) { str.append(msg); },
					[&str](std::basic_string<CharT> const & arg) { str.append(arg); },
					[&str](CharT c) { str.push_back(c); }
				},
				i.get_content()
			);

		f_(str);
	}

	basic_out<CharT> & fatal(std::basic_string_view<CharT> msg) {
		out(cat_v::fatal, msg);
		return *this;
	}
	basic_out<CharT> & error(std::basic_string_view<CharT> msg) {
		out(cat_v::error, msg);
		return *this;
	}
	basic_out<CharT> & warn(std::basic_string_view<CharT> msg) {
		out(cat_v::warn, msg);
		return *this;
	}
	basic_out<CharT> & info(std::basic_string_view<CharT> msg) {
		out(cat_v::info, msg);
	}
	basic_out<CharT> & debug(std::basic_string_view<CharT> msg) {
		out(cat_v::debug, msg);
		return *this;
	}
	basic_out<CharT> & trace(std::basic_string_view<CharT> msg) {
		out(cat_v::trace, msg);
		return *this;
	}

	message_format<CharT> & reset_format() noexcept {
		format_.clear();
		return format_;
	}
};

#pragma region "log out utl macro"

#define MYFILE (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define LOG_WITH_FILEINFO(logger, msg)	do{\
	std::basic_stringstream<char> ss;\
	ss << msg << '@' << MYFILE << ':' << __LINE__;\
	logger(ss.str());\
}while(false)

#pragma endregion

} // ouchi::log
