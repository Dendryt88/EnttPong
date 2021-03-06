///
/// Game.cpp
/// EnttPong
///
/// MIT LICENSE.
/// Refer to LICENSE.txt for more details.
///

#include <SDL2/SDL_timer.h>

#include "../tags/AITag.hpp"
#include "../tags/BallTag.hpp"
#include "../core/BasicLog.hpp"
#include "../tags/PlayerTag.hpp"
#include "../components/SpriteComponent.hpp"
#include "../components/PositionComponent.hpp"

#include "Game.hpp"

namespace ep
{
	Game::Game(const std::string& title, int w, int h, Uint32 flags)
		:m_window(title, w, h, flags)
	{
		// Here, we are creating the entities using EnTT and attaching the relevant components and tags.
		// We can invoke the constructor of the component or tag in the assign() and attach() methods of the registry.

		const auto player = m_registry.create();
		m_registry.attach<PlayerTag>(player);
		m_registry.assign<SpriteComponent>(player, 12, 96, SDL_Colour{ 255, 255, 255, 255 });
		m_registry.assign<PositionComponent>(player, 20.0, 20.0);

		const auto ai = m_registry.create();
		m_registry.attach<AITag>(ai);
		m_registry.assign<SpriteComponent>(ai, 12, 96, SDL_Colour{ 255, 255, 255, 255 });
		m_registry.assign<PositionComponent>(ai, w - 30.0, 20.0);

		const auto ball = m_registry.create();
		m_registry.attach<BallTag>(ball, 0.12, 0.12);
		m_registry.assign<SpriteComponent>(ball, 8, SDL_Colour{ 255, 255, 255, 255 });
		m_registry.assign<PositionComponent>(ball, (w / 2.0) - 16.0, (h / 2.0) - 16.0);
	}

	int Game::run()
	{
		// This is a fixed-step gameloop.
		// See https://gafferongames.com/post/fix_your_timestep/
		// For an explanation.

		double time = 0;
		double accumulator = 0.0;

		// 60 updates per second. We divide 1000 by 60 instead of 1 because sdl operates on milliseconds 
		// not nanoseconds.
		const double deltaTime = 1000.0 / 60.0;

		double currentTime = SDL_GetTicks();
		while (m_window.m_isOpen)
		{
			const double newTime = SDL_GetTicks();
			const double frameTime = newTime - currentTime;
			currentTime = newTime;

			accumulator += frameTime;

			while (accumulator >= deltaTime)
			{
				event();
				update(accumulator);

				accumulator -= deltaTime;
				time += deltaTime;
			}

			render();

			LOG_INFO << "TIME: " << time << "\n";
			LOG_INFO << "ACCM: " << accumulator << "\n";
		}

		return EXIT_SUCCESS;
	}

	void Game::event()
	{
		// Process game events
		while (SDL_PollEvent(&m_window.m_event) != 0)
		{
			switch (m_window.m_event.type)
			{
			case SDL_QUIT:
				m_window.m_isOpen = false;
				break;

			case SDL_KEYDOWN:
				switch (m_window.m_event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					m_window.m_isOpen = false;
					break;
				}
				break;
			}

			// Process system events.
			m_moveSystem.event(m_window.m_event, m_registry);
		}
	}

	void Game::update(double time)
	{
		m_aiSystem.update(time, m_registry);
		m_moveSystem.update(time, m_registry);
		m_collisionSystem.update(time, m_registry);
	}

	void Game::render()
	{
		// Flush renderer.
		SDL_SetRenderDrawColor(m_window.getRenderer(), 0, 0, 0, 255);
		SDL_RenderClear(m_window.getRenderer());

		// Render components.
		m_renderSystem.render(&m_window, m_registry);

		// Draw them to the screen.
		SDL_RenderPresent(m_window.getRenderer());
	}
}