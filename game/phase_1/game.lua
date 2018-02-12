local agents = require("agents")

game = {
	frame = 0,
	running = true,
	player = {},
	agents = {},
	bullets = {},
	mouse = glm.vec2.new(0, 0),
	delta = 0.0,
	lives = 10,
	godmode = false,
	godmodeTimer = 0
}

function game.init()
	game.player = Human.new("Player", glm.vec2.new(200, 200), glm.u8vec3.new(0, 255, 0))

	for i = 1, 3 do
		local blue = Human.new("BLUE_"..tostring(i), glm.vec2.new(0, math.random(1024)), glm.u8vec3.new(0, 0, 255))
		blue.team = "BLUE"

		table.insert(game.agents, blue)

		local red = Human.new("RED_"..tostring(i), glm.vec2.new(1024, math.random(1024)), glm.u8vec3.new(255, 0, 0))
		red.team = "RED"

		table.insert(game.agents, red)
	end
end

function game.update(delta)
	game.delta = delta
	if game.running then
		game.frame = game.frame + 1

		-- UPDATE PLAYER
		game.godmodeTimer = game.godmodeTimer + delta
		if game.godmodeTimer >= 2 then
			game.godmode = false
		end

		local cosTheta = glm.dot(glm.normalize(game.mouse - game.player.pos), glm.vec2.new(1, 0))
		local theta = math.acos(cosTheta)

		if game.mouse.y > game.player.pos.y then
			game.player.rot = theta
		else
			game.player.rot = 2 * 3.14159 - theta
		end

		game.player.lastShot = game.player.lastShot + delta * 10

		-- UPDATE AGENTS
		for i = #game.agents, 1, - 1 do
			local agent = game.agents[i]

			agent.lastShot = agent.lastShot + delta
			agent:update()
		end

		-- UPDATE BULLETS
		for i = #game.bullets, 1, - 1 do
			local bullet = game.bullets[i]

			local dir = glm.normalize(bullet.target - bullet.pos)

			if glm.length(bullet.target - bullet.pos) < 1000 * delta then
				table.remove(game.bullets, i)
			end

			bullet.pos = bullet.pos + dir * 1000 * delta

			-- CHECK PLAYER-BULLET COLLISION
			if not game.godmode and game.player.name ~= bullet.owner and bullet.pos.x > game.player.pos.x - 16 and bullet.pos.x < game.player.pos.x + 16 and bullet.pos.y > game.player.pos.y - 16 and bullet.pos.y < game.player.pos.y + 16 then
				table.remove(game.bullets, i)
				game.player.pos.x = 512
				game.player.pos.y = 512
				game.lives = game.lives - 1
				game.godmode = true
				game.godmodeTimer = 0
			end

			-- CHECK AGENT-BULLET COLLISION
			for j = #game.agents, 1, - 1 do
				local agent = game.agents[j]

				if agent.name ~= bullet.owner and bullet.pos.x > agent.pos.x - 16 and bullet.pos.x < agent.pos.x + 16 and bullet.pos.y > agent.pos.y - 16 and bullet.pos.y < agent.pos.y + 16 then
					table.remove(game.bullets, i)
					table.remove(game.agents, j)
				end
			end
		end
	end
end
