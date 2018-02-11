Steering = {
    maxVelocity = 500,
    maxForce = 500,
    mass = 1.0
}

function Steering.new(maxVelocity, maxForce, mass)
    local self = {}
    setmetatable(self, {__index = Steering})

    self.maxVelocity = maxVelocity or Steering.maxVelocity
    self.maxForce = maxForce or Steering.maxForce
    self.mass = mass or Steering.mass

    return self
end

function Steering:seek(position, target, velocity)
    desired = glm.normalize(target-position) * self.maxVelocity

    steering = glm.limit(desired - velocity, self.maxForce)

    return steering
end

function Steering:arrive(position, target, velocity, radius)
    desired = glm.normalize(target-position) * self.maxVelocity

    if glm.length(target-position) < radius then
        desired = desired * glm.length(target-position)/radius
    end

    steering = glm.limit(desired - velocity, self.maxForce)

    return steering
end

function Steering:maintain(position, target, velocity, radius)
    desired = glm.normalize(target-position) * self.maxVelocity

    if glm.length(target-position) < radius then
        desired = desired * -(1-glm.length(target-position)/radius)
    end

    steering = glm.limit(desired - velocity, self.maxForce)

    return steering
end
