// wav-osiloscope.cpp : アプリケーションのエントリ ポイントを定義します。
//

#ifndef NOMINMAX
#	define NOMINMAX
#endif
#include <string>
#include <iostream>
#include "SFML/window.hpp"
#include "SFML/Graphics.hpp"

#include "wave-viewer.hpp"
#include "fps-counter.hpp"

int main(int argc, char * argv[]) {
	sf::RenderWindow window(sf::VideoMode(1200, 200), "SFML works!");
	sf::Text fps;
	sf::Font font;
	jk::fps_counter<> cnter;
	ouchi::wave_viewer wv("sample.wav");
	font.loadFromFile("meiryo.ttc");
	fps.setFont(font);
	fps.setFillColor((sf::Color)wv.frg_);
	fps.setPosition(10, 10);

	while (window.isOpen())
	{
		cnter.on_frame();
		if (cnter.get_count() % 1000 == 0) {
			fps.setString(std::to_string((int)cnter.get_framerate()));
			cnter.reset();
		}
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		wv.update(window);
		wv.draw(window);
		window.draw(fps);
		wv.offset_ += 1;
		window.display();
		if (wv.offset_ == 30000) 
			wv.offset_ = 80000;
	}

	return 0;
}
