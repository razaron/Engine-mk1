local agents = require "agents"
local buildings = require "buildings"
local traverse = require "luatraverse"

game = {
    delta = 0.0,
    agents = {},
    bases = {},
    deposits = {},
    bullets = {},
    wins = {blue = 0, red = 0}
}

function game.init()
    -- BASES
    table.insert(game.bases, Base.new("BLUE", glm.vec2.new(256/5,256/5), glm.u8vec3.new(100,100,255)))
    table.insert(game.bases, Base.new("RED", glm.vec2.new((4096-256)/5,(4096-256)/5), glm.u8vec3.new(100,100,255)))

    game.bases[1].cycle = 0
    game.bases[2].cycle = 0

    -- RESOURCE DEPOSITS
    table.insert(game.deposits, Deposit.new(glm.vec2.new(1024/5, 1024/5), 1.0))
    table.insert(game.deposits, Deposit.new(glm.vec2.new(1024/5, 3072/5), 0.0))
    table.insert(game.deposits, Deposit.new(glm.vec2.new(2048/5, 2048/5), 0.0))
    table.insert(game.deposits, Deposit.new(glm.vec2.new(3072/5, 1024/5), 0.0))
    table.insert(game.deposits, Deposit.new(glm.vec2.new(3072/5, 3072/5), - 1.0))
end

frame = 1
elapsed = 1
function game.update(delta)
    frame = frame + 1
    game.delta = delta

    elapsed = elapsed - delta
    if elapsed < 0 then
        elapsed = 1

        -- Determine win state
        local redDead = true
        local blueDead = true
        for _, a in pairs(game.agents) do
            if not redDead and not blueDead then break
            elseif a.team == "RED" then redDead = false
            elseif a.team == "BLUE" then blueDead = false end
        end

        if redDead or blueDead then
            if blueDead then game.wins.blue = game.wins.blue + 1 end
            if redDead then game.wins.red = game.wins.red + 1 end

            print(frame)

            for i = #game.agents, 1, - 1 do
                local agent = game.agents[i]
                deleteEntity(agent.uuid)
            end
            game.agents = {}

            for i = #game.bullets, 1, - 1 do
                local bullet = game.bullets[i]
                deleteEntity(bullet.uuid)
            end
            game.bullets = {}

            for i = #game.deposits, 1, - 1 do
                local deposit = game.deposits[i]
                deleteEntity(deposit.uuid)
            end
            game.deposits = {}


            for i = #game.bases, 1, - 1 do
                local base = game.bases[i]
                deleteEntity(base.uuid)
            end
            game.bases = {}

            game.init()
        end
    end

    -- UPDATE BASES --
    for i = #game.bases, 1, - 1 do
        local base = game.bases[i]

        base:update()
    end

    -- UPDATE AGENTS
    for i = #game.agents, 1, - 1 do
        local agent = game.agents[i]

        if agent.isDead then
            deleteEntity(agent.uuid)
            table.remove(game.agents, i)
        else
            agent:update()
        end
    end

    -- UPDATE DEPOSITS
    for i = #game.deposits, 1, - 1 do
        local deposit = game.deposits[i]

        deposit:update()
    end

    -- UPDATE BULLETS
    for i = #game.bullets, 1, - 1 do
        local bullet = game.bullets[i]
        if bullet.isDead then
            deleteEntity(bullet.uuid)
            table.remove(game.bullets, i)
        else
            bullet:update()
        end
    end
end
