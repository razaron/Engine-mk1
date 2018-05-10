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
            print("w")
        end
    end

    input.handlers["s"] = function(isReleased)
        if isReleased then
            print("s")
        end
    end

    input.handlers["a"] = function(isReleased)
        if isReleased then
            print("a")
        end
    end

    input.handlers["d"] = function(isReleased)
        if isReleased then
            print("d")
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
        end
    end

    input.handlers["4"] = function(isReleased)
        if isReleased then
            renderer.overlay = not renderer.overlay
        end
    end

    --[[
        MOUSE
    ]]--
    input.handlers["m1"] = function(x, y)
        print(tostring(x)..", "..tostring(y))
        local p = glm.vec2.new(x, y)
        p = (p/renderer.zoom) + renderer.camera

        local isClicked, node = game.bases[1].interface:getClicked(p)

        if isClicked then
            node.handler()
        else
            game.bases[1].interface:close()

            isClicked, node = game.bases[2].interface:getClicked(p)

            if isClicked then
                node.handler()
            else
                game.bases[2].interface:close()
            end
        end
    end

    input.handlers["m2"] = function(x, y)
        print(tostring(x)..", "..tostring(y))
        local p = glm.vec2.new(x, y)
        p = (p/renderer.zoom) + renderer.camera

        local isClicked, node = game.bases[1].interface:getClicked(p)

        if isClicked then
            node:open()
        else
            game.bases[1].interface:close()

            isClicked, node = game.bases[2].interface:getClicked(p)

            if isClicked then
                node:open()
            else
                game.bases[2].interface:close()
            end
        end
    end

    input.handlers["move"] = function(x, y)
        local p = glm.vec2.new(x, y)
        p = (p/renderer.zoom) + renderer.camera

        game.mouse = p

        local mouse = p
        for _, node in pairs(game.rootNodes) do
            if p.x > node.pos.x - node.size and p.x < node.pos.x + node.size and p.y > node.pos.y - node.size and p.y < node.pos.y + node.size then
                node.isOpen = true
            elseif #node.children == 0 or not node.children[1].isOpen then
                node:close()
            end
        end
    end
end

function input.update(delta)
    input.delta = delta
end
