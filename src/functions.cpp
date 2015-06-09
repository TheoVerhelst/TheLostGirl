#include <algorithm>
#include <cmath>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <TGUI/Gui.hpp>
#include <entityx/Entity.h>
#include <TheLostGirl/components.h>

#include <TheLostGirl/functions.h>


void handleResize(sf::RenderWindow& window, bool bloomEnabled, float scale, sf::RenderTexture& bloomTexture, tgui::Gui& gui)
{
	const float iw{float(window.getSize().x)};
	const float ih{float(window.getSize().y)};
	float fh(ih), fw(iw);//If size is in a 16:9 ratio, it won't change.
	if(iw / 16.f < ih / 9.f) //Taller than a 16:9 ratio
		fh = iw * (9.0f / 16.0f);
	else if(iw / 16.f > ih / 9.f) //Larger than a 16:9 ratio
		fw = ih * (16.0f / 9.0f);
	const float scalex{fw / iw}, scaley{fh / ih};
	sf::View view{window.getView()};
	view.setViewport({(1 - scalex) / 2.0f, (1 - scaley) / 2.0f, scalex, scaley});
	view.setSize(1920.f*scale, 1080.f*scale);
	if(bloomEnabled)
		bloomTexture.setView(view);
	else
		window.setView(view);
	view.setCenter({960.f*scale, 540.f*scale});
	gui.setView(view);
}

sf::Color fadingColor(sf::Time dt, sf::Time fadingLength, bool in)
{
	float alpha{cap((dt / fadingLength) * 255.f, 0.f, 255.f)};
	alpha = in ? alpha : 255 - alpha;//Invert the fading if in is false, so if it is a fade out
	return sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha));
}

bool hasWhiteSpace(const std::string str)
{
	return std::any_of(str.cbegin(), str.cend(), [](const char& chr){return isspace(chr);});
}

sf::Vector2f b2tosf(const b2Vec2& vec)
{
	return {vec.x, vec.y};
}

b2Vec2 sftob2(const sf::Vector2f& vec)
{
	return {vec.x, vec.y};
}

sf::Color b2ColorToSf(const b2Color& color)
{
	return sf::Color(static_cast<sf::Uint8>(color.r*255.f), static_cast<sf::Uint8>(color.g*255.f),
					 static_cast<sf::Uint8>(color.b*255.f), static_cast<sf::Uint8>(color.a*255.f));
}

bool isPlayer(entityx::Entity entity)
{
	if(entity.valid())
	{
		CategoryComponent::Handle categoryComponent(entity.component<CategoryComponent>());
		return categoryComponent and categoryComponent->category & Category::Player;
	}
	return false;
}
