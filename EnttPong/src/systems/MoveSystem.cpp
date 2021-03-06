///
/// MoveSystem.cpp
/// EnttPong
///
/// MIT LICENSE.
/// Refer to LICENSE.txt for more details.
///

#include "../tags/BallTag.hpp"
#include "../tags/PlayerTag.hpp"
#include "../utils/RandomVelocity.hpp"
#include "../components/PositionComponent.hpp"

#include "MoveSystem.hpp"

namespace ep
{
	void MoveSystem::event(const SDL_Event& event, entt::DefaultRegistry& registry) noexcept
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_w:
				m_movingNorth = true;
				break;

			case SDLK_s:
				m_movingSouth = true;
				break;
			}
			break;

		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			case SDLK_w:
				m_movingNorth = false;
				break;

			case SDLK_s:
				m_movingSouth = false;
				break;
			}
			break;
		}
	}

	void MoveSystem::update(double time, entt::DefaultRegistry& registry)
	{
		// We only need to update the player position, since the ai wil be managed by the aisystem.
		// Here, we can retrieve the player entity, because only ONE entity can have a single instance component (tag).
		const auto player = registry.attachee<PlayerTag>();
		PositionComponent& pc = registry.get<PositionComponent>(player);

		if (m_movingNorth)
		{
			pc.m_y -= 0.15 * time;
		}

		if (m_movingSouth)
		{
			pc.m_y += 0.15 * time;
		}

		// lock to screen
		if (pc.m_y < 0.0)
		{
			pc.m_y = 0.0;
		}
		else if (pc.m_y > (480.0 - 96.0)) // screen width - sprite width
		{
			pc.m_y = (480.0 - 96.0);
		}

		// Next, we want to update the balls position and move it according to the directions its currently travelling.
		const auto ball = registry.attachee<BallTag>();
		BallTag& ballTag = registry.get<BallTag>();
		PositionComponent& ballPos = registry.get<PositionComponent>(ball);

		// allow the ball to move based on a fixed-timestep loop.
		ballPos.m_x += ballTag.m_velX * time;
		ballPos.m_y += ballTag.m_velY * time;

		// ensure ball can be reset
		if (ballPos.m_x < 0.0)
		{
			// ball passed the player paddle, reset it.
			ballPos.m_x = (640.0 / 2.0) - 16.0;
			ballPos.m_y = (480.0 / 2.0) - 16.0;

			ballTag.m_velX = randomVelocitySign(ballTag.m_startingVelX);
			ballTag.m_velY = randomVelocitySign(ballTag.m_startingVelY);
		}
		else if (ballPos.m_x > (640.0 - 16.0)) // screen width - sprite width
		{
			// ball passed the ai paddle, reset it.
			ballPos.m_x = (640.0 / 2.0) - 16.0;
			ballPos.m_y = (480.0 / 2.0) - 16.0;

			ballTag.m_velX = randomVelocitySign(ballTag.m_startingVelX);
			ballTag.m_velY = randomVelocitySign(ballTag.m_startingVelY);
		}

		// lock to screen
		if (ballPos.m_y < 0.0)
		{
			// Reverse ball, "bouncing" it.
			ballPos.m_y = 0.0;
			ballTag.m_velY *= -1;
		}
		else if (ballPos.m_y > (480.0 - 16.0)) // screen height - sprite height
		{
			// Reverse ball, "bouncing" it.
			ballPos.m_y = (480.0 - 16.0);
			ballTag.m_velY *= -1;
		}
	} 
}