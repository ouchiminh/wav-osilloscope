// wav-osiloscope.cpp : アプリケーションのエントリ ポイントを定義します。
//

#ifndef NOMINMAX
#	define NOMINMAX
#endif
#include <string>
#include <iostream>
#include <chrono>
#include "SFML/window.hpp"
#include "SFML/Graphics.hpp"

#include "wave-viewer.hpp"
#include "ouchilib/utl/fps-counter.hpp"
#include "ouchilib/utl/time_keeper.hpp"

int main(int argc, char * argv[]) {
	sf::RenderWindow window(sf::VideoMode(1200, 500), "SFML works!", sf::Style::Resize | sf::Style::Close);
	sf::Text fps;
	sf::Font font;
	ouchi::fps_counter<> cnter;
	ouchi::wave_viewer wv(argv[1]);
	ouchi::time_keeper<> tk(std::chrono::seconds(1));
	font.loadFromFile("meiryo.ttc");
	fps.setFont(font);
	fps.setFillColor((sf::Color)wv.frg_);
	fps.setPosition(10, 10);


	while (window.isOpen())
	{
		if (tk.is_time()) {
			fps.setString(std::to_string(cnter.get_framerate()));
			cnter.reset();
			tk.start();
		}
		cnter.on_frame();
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::Resized) {
				sf::View v;
				v.setSize(event.size.width, event.size.height);
				v.setCenter(v.getSize() / 2.f);
				window.setView(v);
				wv.clear();
			}
		}

		window.clear();
		wv.update(window);
		wv.draw(window);
		window.draw(fps);
		wv.offset_ += 1;
		window.display();
	}

	return 0;
}
