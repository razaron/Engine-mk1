local nodal = require("nodal")

Base = {
    class = "Base",
	pos = glm.vec2.new(0,0),
    col = glm.u8vec3.new(255,255,255),
	team = "DEFAULT",
	ammo = 10,
    resupplying = 0,
	interface = {}
}

function Base.new(pos, team, col)
	local self = {}
	setmetatable(self, {__index = Base})

    self.pos = pos or Base.pos
	self.col = col or Base.col
	self.team = team or Base.team
	self.ammo = Base.ammo
	self.interface = BaseUI.new(pos, self)

    table.insert(game.rootNodes, self.interface)

    print(self.interface.pos.x, self.interface.pos.y)

	return self
end

function Base:build(type)
    if type == "Worker" then
        local agent = Worker.new(self.team.."_CRAFTER", glm.vec2.new(self.pos), glm.u8vec3.new(self.col))
        agent.team = self.team

        if self.ammo >= 2 then
            self.ammo = self.ammo - 2
            table.insert(game.agents, agent)
        end
    elseif type == "Attacker" then
        local agent = Attacker.new(self.team.."_ATTACKER", glm.vec2.new(self.pos), glm.u8vec3.new(self.col))
        agent.team = self.team
        agent.ammo = 3

        if self.ammo >= 5 then
            self.ammo = self.ammo - 5
            table.insert(game.agents, agent)
        end
    elseif type == "Defender" then
        local agent = Defender.new(self.team.."_DEFENDER", glm.vec2.new(self.pos), glm.u8vec3.new(self.col))
        agent.team = self.team
        agent.ammo = 3

        if self.ammo >= 5 then
            self.ammo = self.ammo - 5
            table.insert(game.agents, agent)
        end
    end
end
BaseUI = {
}

setmetatable(BaseUI, {__index = Node})

function BaseUI.new(pos, base)
    local self = Node.new("Base", pos)
    setmetatable(self, {__index = BaseUI})

    local buildWorkerNode = Node.new("Worker", glm.vec2.new(0,0), {cost = 2}, function()
        base:build("Worker")
    end)

    local buildAttackerNode = Node.new("Attacker", glm.vec2.new(0,0), {cost = 5}, function()
        base:build("Attacker")
    end)

    local buildDefenderNode = Node.new("Defender", glm.vec2.new(0,0), {cost = 5}, function()
        base:build("Defender")
    end)

    local buildMenuNode = Node.new("Build", glm.vec2.new(0,0), {}, Node.handler, {buildWorkerNode, buildAttackerNode, buildDefenderNode})

    self:addChild(buildMenuNode)

    return self
end
