Actions = {
    getInRange = function(target)
        pre = ConditionSet()
        post = ConditionSet(
            Condition.new(target, "isInRange", true, OPERATION.ASSIGN)
        )

        return Action.new(
            "get_in_range_of_"..target,
            1,
            pre,
            post
        )
    end,
    shoot = function(target)
        pre = ConditionSet(
            Condition.new(target, "isInRange", true, OPERATION.EQUAL)
        )
        post = ConditionSet(
            Condition.new(target, "isDead", true, OPERATION.ASSIGN)
        )

        return Action.new(
            "shoot_"..target,
            1,
            pre,
            post
        )
    end
}

Effects = {
    getInRange = function(self, target)
        return {
            ["name"] = "get_in_range_of_"..target,
            ["target"] = game[target],
            ["behaviour"] = "SEEK",

            ["effect"] = function()
                if glm.length(self.pos-game[target].pos) < 500 then
                    return true
                else
                    return false
                end
            end
        }
    end,
    shoot = function(self, target)
        return {
            ["name"] = "shoot_"..target,
            ["target"] = game[target],
            ["behaviour"] = "MAINTAIN",

            ["effect"] = function()
                if game[target] then
                    self:shoot(game[target].pos)

                    if glm.length(self.pos-game[target].pos) > 500 then
                        return true
                    else
                        return false
                    end
                else
                    return true
                end
            end
        }
    end
}
