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
