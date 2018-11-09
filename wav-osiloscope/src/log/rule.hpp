#pragma once
#include <algorithm>
#include <vector>
#include <initializer_list>
#include "format.hpp"
#include "out.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/foreach.hpp"

namespace ouchi::log {

struct rule {
public:
	cat_v upper_ = cat_v::fatal,
		lower_ = cat_v::info;

	void set(cat_v upper, cat_v lower = cat_v::info) {
		upper_ = std::clamp(upper, cat_v::trace, cat_v::fatal);
		lower_ = std::clamp(lower, cat_v::trace, upper_);
	}
	bool is_valid(cat_v level) {
		return level <= upper_ && level >= lower_;
	}
};

template<class CharT>
class ruled_logger {
	std::vector<std::pair<rule, basic_out<CharT>>> logger_;

	void read_json(std::filesystem::path const & file, std::string && key) try {
		if (!std::filesystem::exists(file)) throw std::exception("no such file or directory.");
		namespace pjson = boost::property_tree;
		pjson::ptree pt;
		pjson::read_json(file.string(), pt);
		auto tar = pt.get_child_optional(key);
		if (!tar) throw std::exception("key not found.");
		/**
		 *	key : [
		 *		{"file":"~~", "upper":N, "lower":N},
		 *		{...}, ...
		 *	]
		 **/
		BOOST_FOREACH(const pjson::ptree::value_type & child, tar.value()){
			const pjson::ptree & data = child.second;
			out_func<CharT> of;
			rule r;
			if (auto file = data.get_optional<std::string>("file"))
				of.set(file.value());
			if (auto upper = data.get_optional<unsigned>("upper"))
				r.upper_ = convert<cat_v, unsigned>(upper.value());
			if (auto lower = data.get_optional<unsigned>("lower"))
				r.lower_ = convert<cat_v, unsigned>(lower.value());
			logger_.push_back({ r ,basic_out<CharT>(of) });
		}
	} catch (std::exception & e) {
		logger_.clear();
		logger_.push_back(std::make_pair<rule, basic_out<CharT>>(rule{}, basic_out<CharT>{}));
		LOG_WITH_FILEINFO(this->warn, e.what());
	}
	
public:
	ruled_logger(std::initializer_list<std::pair<rule, basic_out<CharT>>> && il):logger_(std::move(il)) {}
	ruled_logger(std::filesystem::path && json_setting, std::string && key) { read_json(json_setting, std::move(key)); }
	ruled_logger(std::filesystem::path const & json_setting, std::string const & key) { read_json(json_setting, std::move(key)); }

	void out(cat_v c, std::basic_string_view<CharT> msg) {
		for (auto & i : logger_) {
			if (i.first.is_valid(c)) i.second.out(c, msg);
		}
	}
	void fatal(std::basic_string_view<CharT> msg) {
		out(cat_v::fatal, msg);
	}
	void error(std::basic_string_view<CharT> msg) {
		out(cat_v::error, msg);
	}
	void warn(std::basic_string_view<CharT> msg) {
		out(cat_v::warn, msg);
	}
	void info(std::basic_string_view<CharT> msg) {
		out(cat_v::info, msg);
	}
	void debug(std::basic_string_view<CharT> msg) {
		out(cat_v::debug, msg);
	}
	void trace(std::basic_string_view<CharT> msg) {
		out(cat_v::trace, msg);
	}

};

}
