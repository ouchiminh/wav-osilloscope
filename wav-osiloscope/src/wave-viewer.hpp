#pragma once
#include <fstream>
#include <istream>
#include <iterator>
#include <algorithm>
#include <filesystem>
#include <string>
#include <deque>
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include "property.hpp"
#include "convertable.hpp"

namespace ouchi {
#if 0
struct riff_fmt {
	[[noreturn]] void error() {
		throw "file is not compatible.";
	}
	void load(std::istream & file) {

		// fmtタグまで読み飛ばし
		file.ignore(4 + 4 + 4 + 4);
		{
			std::uint32_t fmt_size;
			file.read(reinterpret_cast<char*>(&fmt_size), sizeof(fmt_size));
			if (fmt_size != 16) error();
		}
		// format id
		{
			short format_id;
			file.read(reinterpret_cast<char*>(&format_id), sizeof(format_id));
			if (format_id != 1)  error();
		}
		file.read(reinterpret_cast<char*>(&channel_), sizeof(channel_));
		file.read(reinterpret_cast<char*>(&samplerate_), sizeof(samplerate_));
		// ignore data speeed
		file.ignore(4);
		file.read(reinterpret_cast<char*>(&block_size_), sizeof(block_size_));

		file.read(reinterpret_cast<char*>(&bit_depth_), sizeof(bit_depth_));
	}
	riff_fmt(std::istream & file) { load(file); }
	riff_fmt() = default;
	
	short bit_depth_;
	short channel_;
	int samplerate_;
	short block_size_;
};

union sample {
	std::int32_t data32;
	std::int16_t data16;
	std::uint8_t  data8;
};

using block = sample;

struct riff_data {
	void load(std::istream & file, riff_fmt const & fmt) {

		// ignore "data"
		file.ignore(4);
		// read size
		file.read(reinterpret_cast<char*>(&data_size_), sizeof(data_size_));
		data_.reserve(data_size_ / fmt.block_size_);
		// read data
		for (auto i = 0ul; i < data_size_ / fmt.block_size_; i++) {
			block b;
			for (auto j = 0; j < 1; j++) {
				sample buf;
				file.read(reinterpret_cast<char*>(&buf), fmt.bit_depth_ / 8);
				file.ignore(fmt.bit_depth_ / 8);
				b = buf;
			}
			data_.push_back(b);
		}
	}
	riff_data(std::istream & file, riff_fmt const & fmt) { load(file, fmt); }
	riff_data() = default;
	unsigned long data_size_;
	std::vector<block> data_;
};
#endif
class wave_viewer : sf::Drawable {
	sf::SoundBuffer sb_;
	std::vector<sf::Vertex> va_;
	long long last_offset_ = 0;
public:
	wave_viewer(std::string && file_path) { 
		sb_.loadFromFile(file_path); 
	}
	wave_viewer(std::string const & file_path) {
		sb_.loadFromFile(file_path); 
	}
	
	void draw(sf::RenderTarget & rt, sf::RenderStates rs = sf::RenderStates::Default) const override {
	#if 0
		short max_sample = 0;
		short min_sample = 0;
		va.reserve(disp_sample_width);

		for(auto i = std::max(0ll, offset_);
			i < std::min<long long>(offset_ + disp_sample_width, sb_.getSampleCount());
			i++)
		{
			max_sample = max_sample < sb_.getSamples()[i] ? sb_.getSamples()[i] : max_sample;
			min_sample = min_sample > sb_.getSamples()[i] ? sb_.getSamples()[i] : min_sample;
			if (i % sample_gap == 0) {
				auto idx = (float)va.size()/2;
				auto p = (float)rt.getSize().y * max_sample / (float)sample_range + rt.getSize().y / 2;
				va.push_back(sf::Vector2f(idx, p));
				p = (float)rt.getSize().y * min_sample / (float)sample_range + rt.getSize().y / 2;
				va.push_back(sf::Vector2f(idx, p));
				max_sample = min_sample = 0;
			}

		}
	#endif
		rs.transform *= sf::Transform{ 1.f,0.f,-offset_ * (float)(double)zoom_,0.f,1.f,0.f,0.f,0.f,1.f };
		rt.draw(va_.data(), va_.size(), sf::PrimitiveType::LineStrip, rs);
	}

	void update(sf::RenderTarget const & rt) {
		int disp_sample_width = static_cast<int>(rt.getSize().x / (double)zoom_);
		int sample_gap = static_cast<int>(1.0f / (double)zoom_);
		constexpr double sample_range = (SHRT_MAX - SHRT_MIN);
		std::vector<sf::Vertex> va;

		// cnt = offset - last_offset
		// delete cnt from front
		// (last_offset + size) <= r <= (offset + width)
		va_.erase(va_.cbegin(), va_.cbegin() + std::min<long long>(offset_ - last_offset_, va_.size()));

		for (auto i = std::max(0ll, last_offset_) + va_.size();
			 i < std::min<long long>(offset_ + disp_sample_width, sb_.getSampleCount());
			 i++) {
			auto p = (float)rt.getSize().y * sb_.getSamples()[i] / (float)sample_range + rt.getSize().y / 2;
			va_.push_back(sf::Vertex{ sf::Vector2f{i*(float)(double)zoom_, p}, (sf::Color)frg_ });
		}
		last_offset_ = offset_;
	}

	property<sf::Color> bkg_ {sf::Color::Black};
	property<sf::Color> frg_ {sf::Color::Green};
	long long offset_ = 0;
	property<double> zoom_ {
		[](double v) {return v; },
		[&](double v) {
			va_.clear();
			return v;
		},
		5e-2};
};

}