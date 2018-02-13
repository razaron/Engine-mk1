renderer = {
    tilemap = {}
}

function renderer.init()
    renderer.tilemap = Render.TileMap()
end

function renderer.update(delta)
    Render.draw.tilemap(renderer.tilemap);

    Render.draw.polygon(3, 32, game.player.pos, game.player.rot, game.player.col)
    Render.draw.text(tostring(game.lives), glm.vec2.new(0, 1024 - 32), 32, glm.u8vec3.new(0, 0, 255))

    local string = ""
    for i = 1, #game.agents do
        local agent = game.agents[i]

        Render.draw.polygon(3, 32, agent.pos, agent.rot, agent.col)

        if agent.curAction ~= 0 then
            local action = agent.curPlan[agent.curAction]

            string = string .. agent.name .."\t".. action.name .."\t".. action.behaviour .."\t".. glm.length(agent.vel) .."\t".. tostring(agent.rot) .."\t".. tostring(agent.theta) .."\n"
        end
    end

    Render.draw.text(string, glm.vec2.new(0, 0), 20, glm.u8vec3.new(0, 0, 0))

    for i = 1, #game.bullets do
        local bullet = game.bullets[i]

        Render.draw.polygon(8, 8, bullet.pos, 0, glm.u8vec3.new(0, 0, 0))
    end
end
