#include "stdafx.h"
#include <SFML/Graphics.hpp>

class Entity {
public:
	virtual void handleEvent(sf::Event &event) = 0;
	virtual void update() = 0;
	virtual void draw() = 0;
};

struct Game {
	std::vector<Entity*> entity_list;
};

class PhysicsObject : public Entity {
	sf::CircleShape shape;
	sf::RenderWindow &window;

public:
	PhysicsObject(sf::RenderWindow &window, int x, int y)
		: shape(50), window(window)
	{
		shape.setFillColor(sf::Color::Red);
		shape.setPosition(x, y);
	}

	sf::CircleShape getShape() const {
		return shape;
	}

	void handleEvent(sf::Event &event) override { }

	void update() override { }

	void draw() override {
		window.draw(shape);
	}
};

class Player : public Entity {
	bool movingLeft = false;
	bool movingRight = false;
	bool movingUp = false;
	bool movingDown = false;

	sf::CircleShape shape;

	sf::RenderWindow &window;
	Game &game;

public:
	Player(sf::RenderWindow &window, Game &game)
		: shape(20.f), window(window), game(game)
	{
		shape.setFillColor(sf::Color::Green);
	}

	void handleEvent(sf::Event &event) override {
		if (event.type == sf::Event::KeyPressed ||
			event.type == sf::Event::KeyReleased) {

			bool flag = event.type == sf::Event::KeyPressed;

			if (event.key.code == sf::Keyboard::D)
				movingRight = flag;
			if (event.key.code == sf::Keyboard::A)
				movingLeft = flag;
			if (event.key.code == sf::Keyboard::W)
				movingUp = flag;
			if (event.key.code == sf::Keyboard::S)
				movingDown = flag;
		}
	}

	void update() override {
		auto oldPos = shape.getPosition();

		auto pos = shape.getPosition();
		pos.x += 2 * (movingRight - movingLeft);
		pos.y += 2 * (movingDown - movingUp);
		pos.y += 1;
		shape.setPosition(pos);

		for (auto entity : game.entity_list) {
			auto* physics_object = dynamic_cast<PhysicsObject*>(entity);
			if (physics_object) {
				if (collidesWith(physics_object)) {
					shape.setPosition(oldPos);
				}
			}
		}

	}

	void draw() override {
		window.draw(shape);
	}

	bool collidesWith(PhysicsObject *obj) {
		auto oShape = obj->getShape();
		auto rect = 
			sf::Rect<float>(shape.getPosition(), {shape.getRadius() * 2, shape.getRadius() * 2 });
		auto oRect = 
			sf::Rect<float>(oShape.getPosition(), { oShape.getRadius() * 2, oShape.getRadius() * 2 });
		
		return rect.intersects(oRect);
	}
};

int main()
{
	sf::RenderWindow window(sf::VideoMode(600, 800), "SFML works!");
	std::vector<Entity*> entity_list;

	Game game;
	auto player = new Player(window, game);

	game.entity_list.push_back(player);
	game.entity_list.push_back(new PhysicsObject(window, 200, 200));
	game.entity_list.push_back(new PhysicsObject(window, 400, 500));

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			for(auto entity : game.entity_list)
				entity->handleEvent(event);
		}

		for (auto entity : game.entity_list)
			entity->update();


		// Очистка окна
		window.clear();

		for (auto entity : game.entity_list)
			entity->draw();

		// Вывод на экран
		window.display();

		window.setFramerateLimit(60);
	}

	return 0;
}