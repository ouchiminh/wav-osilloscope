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