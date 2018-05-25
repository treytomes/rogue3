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

--io.write("Loading a confused monster AI.\n");

-- TODO: How do I save the old ai?

local num_turns = 0
local first_update = true
local old_background_color = TCODColor:black

function save(zip)
	zip:put_int(num_turns)
	zip:put_bool(first_update)
	zip:put_color(old_background_color)
end

function load(zip)
	num_turns = zip:get_int()
	first_update = zip:get_bool()
	old_background_color = zip:get_color()
end

function update(owner)
	if first_update then
		old_background_color = owner:get_background_color()
		owner:set_background_color(TCODColor:green)
		first_update = false
	end

	local dx = random:get_int(-1, 1)
	local dy = random:get_int(-1, 1)
	local cost = COST_WAIT

	if dx ~= 0 or dy ~= 0 then
		local dest_x, dest_y = owner:get_position()
		dest_x = dest_x + dx
		dest_y = dest_y + dy
		if this:can_walk(dest_x, dest_y) then
			owner:move_by(dx, dy)
		else
			
		end

		if this:is_in_fov(x, y) then
			move_count = TRACKING_TURNS
		else
			move_count = move_count - 1
		end

		if move_count > 0 then
			player_x, player_y = this:get_player_position()
			return move_or_attack(owner, x, y, player_x, player_y)
		end
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
