#include <iostream>
#include "team.hpp"
#include "unit.hpp"
#include "map.hpp"
#include "wfc.hpp"
#include "game.hpp"
#include <algorithm>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Context.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/String.hpp>
#include <SFML/Graphics/Texture.hpp>

int main(void)
{
	test_wfc("Maps/input_map.txt",80,50,10);
}
