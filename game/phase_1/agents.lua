local actions = require("actions")
local steering = require("steering")

--[[===============================
		AGENT BASE CLASS
===============================]]--
Agent = {
	name = "Agent",
	texID = 0,
	col = glm.u8vec3.new(0, 0, 0),
	pos = glm.vec2.new(0, 0),
	vel = glm.vec2.new(0, 0),
	rot = 0.0,

	steer = Steering.new(),

	actions = ActionSet(),
	effects = {},
	curPlan = {},
	curAction = 0,

	debug = {
		["savePlan"] = false
	}
}

function Agent.new(name)
	local self = {}
	setmetatable(self, {__index = Agent})

	self.name = name
	self.texID = 0
	self.col = glm.u8vec3.new(0, 0, 0)
	self.pos = glm.vec2.new(0, 0)
	self.vel = glm.vec2.new(0, 0)
	self.rot = 0.0

	self.steer = Steering.new()

	self.actions = ActionSet()
	self.effects = {}

	self.curPlan = {}
	self.curAction = 0

	return self
end

function Agent:sense() -- returns world state
	ws = ConditionSet()
	return ws
end

function Agent:update()
	if self.curAction > 0 then
		local action = self.curPlan[self.curAction]
		self:move(action.target.pos, action.behaviour)

		if action.effect() then
			if self.curAction < #self.curPlan then
				self.curAction = self.curAction + 1
			else
				self.curAction = 0
			end
		end
	else
		self:plan()
	end
end

function Agent:move(target, behaviour)
	steering = glm.vec2.new(0, 0)

	if behaviour == "SEEK" then
		steering = self.steer:seek(self.pos, target, self.vel)
	elseif behaviour == "ARRIVE" then
		steering = self.steer:arrive(self.pos, target, self.vel, 500)
	elseif behaviour == "MAINTAIN" then
		steering = self.steer:maintain(self.pos, target, self.vel, 250)
	end

	if behaviour ~= "PLAYER" then
		self.vel = self.vel + (steering * game.delta)
		self.pos = self.pos + (self.vel * game.delta)

		self.rot = glm.angle(self.vel, glm.vec2.new(1, 0))
	else
		-- FOR PLAYERS
		self.pos = self.pos + (self.vel * game.delta)
	end

	self.pos.x = self.pos.x % 1024
	self.pos.y = self.pos.y % 1024
end

function Agent:plan()
	local planner = Planner.new(self:sense())

	local plan = planner:plan(self.actions, self:decideGoal())

	if plan:size() > 0 then
		if Agent.debug.savePlan then
			planner:savePlan("dot/"..self.name.."_"..game.frame..".dot")
		end

		-- Add target and effect function to curPlan
		self.curPlan = {}
		for k, v in pairs(plan) do
			local action = self.effects[v.name]

			table.insert(self.curPlan, action)
		end

		self.curAction = 1
	end
end

function Agent:decideGoal()
	return Action.new("GOAL", 0, ConditionSet(), ConditionSet())
end

--[[===============================
		HUMAN SUBCLASS
===============================]]--
Human = {
	lastShot = math.huge,
	team = "BLUE"
}
setmetatable(Human, {__index = Agent})

function Human.new(name, pos, col)
	local self = Agent.new(name)
	setmetatable(self, {__index = Human})

	self.texID = 1
	self.pos = pos or glm.vec2.new(0, 0)
	self.col = col or glm.u8vec3.new(math.random(255), math.random(255), math.random(255))

	self.actions:add(Actions.getInRange("player"))
	self.effects[Actions.getInRange("player").name] = Effects.getInRange(self, "player")

	self.actions:add(Actions.shoot("player"))
	self.effects[Actions.shoot("player").name] = Effects.shoot(self, "player")

	return self
end

function Human:shoot(target)
	if self.lastShot > 1 then
		self.lastShot = 0

		local dest = glm.normalize(target - self.pos) * 256
		self.theta = glm.angle(dest, glm.vec2.new(1, 0))

		if self.theta > self.rot - 3.14159 / 2 and self.theta < self.rot + 3.14159 / 2 then
			local bullet = {
				["owner"] = self.name,
				["pos"] = glm.vec2.new(self.pos.x, self.pos.y),
				["target"] = self.pos + dest
			}

			table.insert(game.bullets, bullet)
		end
	end
end

function Human:decideGoal()
	return Action.new("GOAL", 0, ConditionSet(Condition.new("player", "isDead", true, OPERATION.EQUAL)), ConditionSet())
end
