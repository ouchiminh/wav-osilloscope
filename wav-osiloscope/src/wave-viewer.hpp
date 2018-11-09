#pragma once
#include <fstream>
#include <istream>
#include <iterator>
#include <algorithm>
#include <string>
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include "property.hpp"
#include "convertable.hpp"

namespace ouchi {
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
		rs.transform *= sf::Transform{ 1.f,0.f,-offset_ * (float)(double)zoom_,0.f,1.f,0.f,0.f,0.f,1.f };
		rt.draw(va_.data(), va_.size(), sf::PrimitiveType::LineStrip, rs);
	}

	void update(sf::RenderTarget const & rt) {
		int disp_sample_width = static_cast<int>(rt.getSize().x / (double)zoom_);
		constexpr double sample_range = (SHRT_MAX - SHRT_MIN);

		// cnt = offset - last_offset
		// delete cnt from front
		// (last_offset + size) <= r <= (offset + width)
		const auto d = offset_ - last_offset_;
		auto itr = (d >= 0 ? va_.cbegin() : va_.cend());
		auto o = (d >= 0 ? 1 : -1) * std::min<long long>(std::abs(d), va_.size());
		itr = (d >= 0) ? va_.erase(itr, itr + o) :
			va_.erase(itr + o, itr);

		for (auto i = (d >= 0) ? std::max(0ll, offset_) + va_.size() : std::max(0ll, offset_);
			 i != (d >= 0 ? std::min<long long>(offset_ + disp_sample_width, sb_.getSampleCount()) : std::min<long long>({ last_offset_, (long long)sb_.getSampleCount(), disp_sample_width}));
			 i++) {
			auto p = (float)rt.getSize().y * sb_.getSamples()[i] / (float)sample_range + rt.getSize().y / 2;
			sf::Vertex v{ sf::Vector2f{i*(float)(double)zoom_, p}, (sf::Color)frg_ };
			if (d >= 0) va_.push_back(v);
			else {
				itr = va_.insert(itr, v);
				itr++;
			}
		}
		last_offset_ = offset_;
	}

	property<sf::Color> frg_ {
		[](sf::Color const & v) {
			return v;
		},
		[&](sf::Color const & v) {
			va_.clear();
			return v;
		},
		sf::Color::Green
	};
	long long offset_ = 0;
	property<double> zoom_ {
		[](double v) { return v; },
		[&](double v) {
			va_.clear();
			return v;
		},
		5e-2};
};

}