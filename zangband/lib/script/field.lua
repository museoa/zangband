-- Helper functions for complicated field actions

--
-- Hack XXX XXX Convert the char of the monster to a corpse type
--
-- There are seven sizes of corpses.
-- 0 is large, 6 is small
--
function corpse_type(c)
	if c == 'a' then return 6 end
	if c == 'b' then return 6 end
	if c == 'c'	then return 5 end
	if c == 'd' then return 0 end
	if c == 'e' then return 6 end
	if c == 'f' then return 4 end
	if c == 'g' then return 1 end
	if c == 'h' then return 2 end
	if c == 'i' then return 5 end
	if c == 'j' then return 3 end
	if c == 'k' then return 4 end
	if c == 'l' then return 0 end
	if c == 'm' then return 6 end
	if c == 'n' then return 3 end
	if c == 'o' then return 3 end
	if c == 'p' then return 2 end
	if c == 'q' then return 4 end
	if c == 'r' then return 6 end
	if c == 's' then return 2 end
	if c == 't' then return 2 end
	if c == 'u' then return 3 end
	if c == 'v' then return 4 end
	if c == 'w' then return 5 end
	if c == 'x' then return 5 end
	if c == 'y' then return 4 end
	if c == 'z' then return 3 end
	if c == 'A' then return 2 end
	if c == 'B' then return 5 end
	if c == 'C' then return 5 end
	if c == 'D' then return 0 end
	if c == 'E' then return 3 end
	if c == 'F' then return 4 end
	if c == 'G' then return 3 end
	if c == 'H' then return 2 end
	if c == 'I' then return 6 end
	if c == 'J' then return 5 end
	if c == 'K' then return 3 end
	if c == 'L' then return 1 end
	if c == 'M' then return 1 end
	if c == 'N' then return 3 end
	if c == 'O' then return 1 end
	if c == 'P' then return 0 end
	if c == 'Q' then return 3 end
	if c == 'R' then return 5 end
	if c == 'S' then return 6 end
	if c == 'T' then return 1 end
	if c == 'U' then return 0 end
	if c == 'V' then return 1 end
	if c == 'W' then return 6 end
	if c == 'X' then return 1 end
	if c == 'Y' then return 1 end
	if c == 'Z' then return 5 end
	if c == ',' then return 6 end
	return 3;
end


--
--	Traps interact with magic
--
function trap_gf()
	local power
	local j
	if (type == GF_KILL_TRAP) or (type == GF_KILL_DOOR) then
		power = field.data[0]
		j = dam - power
		
		-- Always have a small chance of success
		if j < 2 then j = 2 end
	
		if randint0(100) < j then
			-- Success
			
			-- Check for LOS
			if known == TRUE then
				notice = TRUE
				
				msgf("There is a bright flash of light!")
			end
			
			-- Delete the field
			deleteme()
		end
	end
end


--
--	Doors interact with magic
--
function door_gf()
	if type == GF_KILL_WALL then

		-- Delete the field
		deleteme()

	elseif type == GF_KILL_DOOR then
		
		-- Check for LOS
		if known == TRUE then
			msgf("There is a bright flash of light!")
			notice = TRUE
		end
		
		-- Delete the field
		deleteme()
	elseif type == GF_KILL_TRAP then
	
		-- Unlock the door
		if known == TRUE then
			msgf("Click!")
			notice = TRUE
		end
		
		-- Delete the field
		deleteme()
	end
end


--
--	Walls interact with magic
--
function wall_gf()
	if type == GF_KILL_WALL then

		-- Check for LOS
		if known == TRUE then
			notice = TRUE
		end
		
		-- Delete the field
		deleteme()	
	end
end