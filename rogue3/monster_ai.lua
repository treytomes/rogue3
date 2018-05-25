-- monster_ai.lua

--[[
	Imported variables:
		Constants: COST_MOVE, COST_WAIT, COST_MOVE_FAIL, COST_ATTACK, COST_GET_ITEM
		_x: This actor's x-position.
		_y: This actor's y-position.
		_player_x: The player's x-position.
		_player_y: The player's y-position.

	Imported functions:
		this:message(text)
		this:can_walk(x, y)
		this:is_in_fov(x, y)

		owner:is_destructible
		owner:is_dead()
		owner:is_attacker()
		owner:move_by(dx, dy)
		owner:attack(x, y)

		zip:put_int
		zip:get_int
]]

--[[
-- Receives a table, returns the sum of its components.
--io.write("The table the script received has:\n")
this:message(3.14159)
this:message("The table the script received has:\n")
x = 10
for i = 1, #foo do
	--print(i, foo[i])
	this:message(i..'\t'..foo[i])
	x = x + foo[i]
end
--io.write("Returning data back to C\n")
this:message("Returning data back to C\n")
return x
]]

--io.write("Loading a monster AI.\n");

local TRACKING_TURNS = 3
local move_count = 0

function save(zip)
	zip:put_int(move_count)
end

function load(zip)
	move_count = zip:get_int()
	--io.write("Move count: "..tostring(move_count))
end

function update(owner)
	if owner:is_destructible() and owner:is_dead() then
		return 0
	end

	local x, y = owner:get_position()
	if this:is_in_fov(x, y) then
		move_count = TRACKING_TURNS
	else
		move_count = move_count - 1
	end

	if move_count > 0 then
		player_x, player_y = this:get_player_position()
		return move_or_attack(owner, x, y, player_x, player_y)
	end

	return COST_WAIT
end

function move_or_attack(owner, owner_x, owner_y, target_x, target_y)
	local dx = target_x - owner_x
	local dy = target_y - owner_y
	local step_x = (dx > 0) and 1 or -1
	local step_y = (dy > 0) and 1 or -1

	local distance = math.sqrt(dx * dx + dy * dy)
	if distance >= 2 then
		-- Normalize the distance.
		dx = math.floor(dx / distance)
		dy = math.floor(dy / distance)

		if this:can_walk(owner_x + dx, owner_y + dy) then
			owner:move_by(dx, dy)
			return COST_MOVE
		elseif this:can_walk(owner_x + step_x, owner_y) then
			owner:move_by(step_x, 0)
			return COST_MOVE
		elseif this:can_walk(owner_x, owner_y + step_y) then
			owner:move_by(0, step_y)
			return COST_MOVE
		else
			return COST_MOVE_FAIL
		end
	elseif owner:is_attacker() then
		-- TODO: The target position may not be a player.
		owner:attack(owner_x + dx, owner_y + dy)
		return COST_ATTACK
	else
		return COST_WAIT
	end
end
