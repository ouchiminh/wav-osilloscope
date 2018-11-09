#pragma once
#include <optional>
#include <chrono>

namespace jk {
	/// <summary>
	/// Clock must have static `now` method that returns current time point.
	/// </summary>
	template<class Clock = std::chrono::high_resolution_clock, class TimePoint = decltype(Clock::now())>
	class fps_counter{
		unsigned long long frame_cnt_ = 0;
		std::optional<TimePoint> beg_, end_;
	public:
		/// <summary>
		/// call more than 2 times. calculate avg framerate
		/// </summary>
		void on_frame() noexcept;
		/// <summary>
		/// calc avg framerate. if frame count is less than 2, returns 0.
		/// </summary>
		/// <returns>framerate</returns>
		auto get_framerate() const noexcept->double;
		void reset() noexcept;

		/// <summary>
		/// measure time while drawing and returns framerate.
		/// </summary>
		/// <param name="f">drow function</param>
		/// <param name="...args">draw func's argument</param>
		/// <returns>framerate</returns>
		template<class DrawFunc, class ...Args>
		double draw_and_measure(DrawFunc && f, Args && ...args) const noexcept(noexcept(std::declval(DrawFunc)()));

		auto get_count() const noexcept { return frame_cnt_; }
	};
	template<class Clock, class TimePoint>
	inline void fps_counter<Clock, TimePoint>::on_frame() noexcept {
		if (!beg_)  beg_ = Clock::now();
		else end_ = Clock::now();
		frame_cnt_++;
	}
	template<class Clock, class TimePoint>
	inline auto fps_counter<Clock, TimePoint>::get_framerate() const noexcept -> double {
		using namespace std::chrono;
		if (frame_cnt_ < 2) return 0;
		// f/sec
		return frame_cnt_ / (double)duration_cast<microseconds>(end_.value() - beg_.value()).count() * 1'000'000;
	}
	template<class Clock, class TimePoint>
	inline void fps_counter<Clock, TimePoint>::reset() noexcept {
		frame_cnt_ = 0;
		beg_.reset(); end_.reset();
	}
	template<class Clock, class TimePoint>
	template<class DrawFunc, class ...Args>
	inline double fps_counter<Clock, TimePoint>::draw_and_measure(DrawFunc && f, Args && ...args) const noexcept(noexcept(std::declval(DrawFunc)())) {
		fps_counter<Clock, TimePoint> cnter;
		cnter.on_frame();
		f(args...);
		cnter.on_frame();
		return cnter.get_framerate();
	}
}