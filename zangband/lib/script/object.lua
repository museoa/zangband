-- Helper functions for complicated object activations

function cure_potion(hp, do_unblind, do_unconfuse, do_unpoison)
	local ident = FALSE

	if hp_player(hp) then ident = TRUE end

	if do_unblind and clear_blind() then ident = TRUE end
	
	if do_unconfuse and clear_confused() then ident = TRUE end
	
	if do_unpoison and clear_poisoned() then ident = TRUE end
	if do_unpoison and clear_stun() then ident = TRUE end

	if do_unpoison then
		if clear_cut() then ident = TRUE end
	elseif do_unconfuse then
		if inc_cut(-50) then ident = TRUE end
	elseif do_unblind then
		if inc_cut(-10) then ident = TRUE end
	end

	return ident
end

function do_dream()
	if ironman_nightmare then
		msgf("A horrible vision enters your mind.")

		-- Pick a nightmare
		get_mon_num_prep(get_nightmare, NULL)

		-- Have some nightmares
		have_nightmare(get_mon_num(MAX_DEPTH))

		-- Remove the monster restriction
		get_mon_num_prep(NULL, NULL)

		return TRUE
	else
		return FALSE
	end
end

function wand_of_wonder(dir)
	local sval
	local ident = FALSE
	
	sval = randint0(SV_WAND_WONDER)
	
	if sval == SV_WAND_HEAL_MONSTER then
		if heal_monster(dir) then ident = TRUE end
	elseif sval == SV_WAND_HASTE_MONSTER then
		if speed_monster(dir) then ident = TRUE end
	elseif sval == SV_WAND_CLONE_MONSTER then
		if clone_monster(dir) then ident = TRUE end
	elseif sval == SV_WAND_TELEPORT_AWAY then
		if teleport_monster(dir) then ident = TRUE end
	elseif sval == SV_WAND_DISARMING then
		if disarm_trap(dir) then ident = TRUE end
	elseif sval == SV_WAND_TRAP_DOOR_DEST then
		if destroy_door(dir) then ident = TRUE end
	elseif sval == SV_WAND_STONE_TO_MUD then
		if wall_to_mud(dir) then ident = TRUE end
	elseif sval == SV_WAND_LITE then
		msgf("A line of blue shimmering light appears.")
		lite_line(dir)
		ident = TRUE
	elseif sval == SV_WAND_SLEEP_MONSTER then
		if sleep_monster(dir) then ident = TRUE end
	elseif sval == SV_WAND_SLOW_MONSTER then
		if slow_monster(dir) then ident = TRUE end
	elseif sval == SV_WAND_CONFUSE_MONSTER then
		if confuse_monster(dir, 20) then ident = TRUE end
	elseif sval == SV_WAND_FEAR_MONSTER then
		if fear_monster(dir, 20) then ident = TRUE end
	elseif sval == SV_WAND_DRAIN_LIFE then
		if drain_life(dir, 150) then ident = TRUE end
	elseif sval == SV_WAND_POLYMORPH then
		if poly_monster(dir) then ident = TRUE end
	elseif sval == SV_WAND_STINKING_CLOUD then
		ident = fire_ball(GF_POIS, dir, 15, 2)
	elseif sval == SV_WAND_MAGIC_MISSILE then
		ident = fire_bolt_or_beam(20, GF_MISSILE, dir, damroll(2, 6))
	elseif sval == SV_WAND_ACID_BOLT then
		ident = fire_bolt_or_beam(20, GF_ACID, dir, damroll(6, 8))
	elseif sval == SV_WAND_CHARM_MONSTER then
		ident = charm_monster(dir, 45)
	elseif sval == SV_WAND_FIRE_BOLT then
		ident = fire_bolt_or_beam(20, GF_FIRE, dir, damroll(10, 8))
	elseif sval == SV_WAND_COLD_BOLT then
		ident = fire_bolt_or_beam(20, GF_COLD, dir, damroll(6, 8))
	elseif sval == SV_WAND_ACID_BALL then
		ident = fire_ball(GF_ACID, dir, 125, 2)
	elseif sval == SV_WAND_ELEC_BALL then
		ident = fire_ball(GF_ELEC, dir, 75, 2)
	elseif sval == SV_WAND_FIRE_BALL then
		ident = fire_ball(GF_FIRE, dir, 150, 2)
	elseif sval == SV_WAND_COLD_BALL then
		ident = fire_ball(GF_COLD, dir, 100, 2)
	end

	return ident
end

function do_life_potion()
	msgf("You feel life flow through your body!")
	restore_level()
	clear_poisoned()
	clear_blind()
	clear_confused()
	clear_image()
	clear_stun()
	clear_cut()
	do_res_stat(A_STR)
	do_res_stat(A_CON)
	do_res_stat(A_DEX)
	do_res_stat(A_WIS)
	do_res_stat(A_INT)
	do_res_stat(A_CHR)

	-- Recalculate max. hitpoints
	update_stuff()

	hp_player(5000)
end

function werewindle()
	local i = randint1(13)

	if n <= 5 then
		teleport_player(10)
	elseif n <= 10 then
		teleport_player(222)
	elseif n <= 12 then
		stair_creation()
	else
		if get_check("Leave this level? ") then
			if autosave_l then do_cmd_save_game(TRUE) end
			player.state.leaving = TRUE
		end
	end
end

