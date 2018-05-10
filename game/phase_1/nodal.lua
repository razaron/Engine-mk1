Node = {
    title = "DEFAULT",
    pos = glm.vec2.new(0,0),
    isOpen = false,
    data = {},
    handler = function()end,
    children = {},
    size = 128
}

function Node.new(title, pos, data, handler, children)
    local self = {}
    setmetatable(self, {__index = Node})

    self.title = title or Node.title
    self.pos = pos or Node.pos
    self.data = data or Node.data
    self.handler = handler or Node.handler
    self.isOpen = Node.isOpen
    self.children = children or Node.children

    return self
end

function Node:addChild(child)
    if self.children == Node.children then
        self.children = {}
    end
    
    table.insert(self.children, child)
end

function Node:getClicked(mouse)
    if self.isOpen and mouse.x > self.pos.x - Node.size and mouse.x < self.pos.x + Node.size and mouse.y > self.pos.y - Node.size and mouse.y < self.pos.y + Node.size then
        return true, self
    else
        for i=1,#self.children do
            if self.children[i]:getClicked(mouse) then
                return self.children[i]:getClicked(mouse)
            end
        end
    end

    return false, {}
end

function Node:open()
    for i=1,#self.children do
        self.children[i].isOpen = true
    end
end

function Node:close()
    self.isOpen = false
    for i=1,#self.children do
        self.children[i]:close()
    end
end

function Node:getText()
    return self.title
end
