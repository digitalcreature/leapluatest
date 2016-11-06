require "leaplua"

function love.load()
	leap.load()
end

local frame

function love.update()
	frame = leap.frame()
end

local viewsize = 15
local planedistance = 30

local function project(vec)
	if -vec.z >= planedistance then
		return 0, 0, 0
	else
		local x = vec.x
		local y = - vec.y + 20
		local z = (vec.z + planedistance) / planedistance
		return x * z, y * z, z
	end
end

local function drawvec(mode, vec, radius)
	radius = radius or 1
	local x, y, z = project(vec)
	love.graphics.push()
		love.graphics.translate(x, y)
		love.graphics.scale(z)
		love.graphics.circle(mode, 0, 0, radius, 24)
	love.graphics.pop()
end

local function drawline(vecA, vecB)
	local xA, yA, zA = project(vecA)
	local xB, yB, zB = project(vecB)
	love.graphics.line(xA, yA, xB, yB)
end

function love.draw()
	if frame then
		local w, h = love.graphics.getDimensions()
		local text = ""
		love.graphics.push()
		love.graphics.translate(w / 2, h / 2)
		local ppcm = h / viewsize
		love.graphics.scale(ppcm)
		love.graphics.setLineWidth(1/ppcm)
		love.graphics.setColor(255, 255, 255)
		for h, hand in ipairs(frame.hands) do
			text = text..string.format("hand %d:\n", hand.id)
			for f, finger in ipairs(hand) do
				text = text..string.format("\tfinger %d:\n", finger.id)
				local lastjoint = nil
				for j, joint in ipairs(finger) do
					text = text..string.format("\t\tjoint %d: (%d, %d, %d)\n", j, joint.x, joint.y, joint.z)
					drawvec("fill", joint, 1/2)
					if lastjoint then
						drawline(lastjoint, joint)
					end
					lastjoint = joint
				end
				drawvec("line", finger.tip)
			end
		end
		for h, hand in ipairs(frame.hands) do
			for f, finger in ipairs(hand) do
				local x, y, z = project(finger.tip)
				love.graphics.setColor(0, 255, 255)
				love.graphics.print(finger.id, x + 1, y, 0, 1/ppcm, 1/ppcm)
			end
		end
		love.graphics.pop()
		love.graphics.print(text)
	else
		love.graphics.print("invalid frame")
	end
end
