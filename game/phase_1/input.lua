input = {
    handlers = {},
    delta = 0
}

function input.init()
    --[[
        KEYBOARD
    ]]--
    input.handlers["w"] = function(isReleased)
        if isReleased then
            game.player.vel.y = 0
        else
            game.player.vel.y = -256
        end
    end

    input.handlers["s"] = function(isReleased)
        if isReleased then
            game.player.vel.y = 0
        else
            game.player.vel.y = 256
        end
    end

    input.handlers["a"] = function(isReleased)
        if isReleased then
            game.player.vel.x = 0
        else
            game.player.vel.x = -256
        end
    end

    input.handlers["d"] = function(isReleased)
        if isReleased then
            game.player.vel.x = 0
        else
            game.player.vel.x = 256
        end
    end

    input.handlers["1"] = function(isReleased)
        if isReleased then
            print("1")
            Agent.debug.savePlan = not Agent.debug.savePlan
        end
    end

    input.handlers["2"] = function(isReleased)
        if isReleased then
            print("2")
            game.running = not game.running
        end
    end

    input.handlers["3"] = function(isReleased)
        if isReleased then
            print("3")
            for i = 1, 16 do
                table.insert(game.agents, Human.new("Enemy_"..tostring(i), glm.vec2.new(math.random(1024), math.random(1024)), glm.u8vec3.new(255, 0, 0)))
            end
        end
    end

    input.handlers["4"] = function(isReleased) print("4") end

    --[[
        MOUSE
    ]]--
    input.handlers["m1"] = function(x, y)
        print(tostring(x)..", "..tostring(y))
        game.player:shoot(glm.vec2.new(x, y))
    end

    input.handlers["move"] = function(x, y)
        game.mouse.x = x
        game.mouse.y = y
    end
end

function input.update(delta)
    input.delta = delta
end
