-- Use objects

function eat_food(object)
	local ident = FALSE

	if object.sval == SV_FOOD_RATION or
	   object.sval == SV_FOOD_BISCUIT or
	   object.sval == SV_FOOD_JERKY or
	   object.sval == SV_FOOD_SLIME_MOLD or
	   object.sval == SV_FOOD_PINT_OF_ALE or
	   object.sval == SV_FOOD_PINT_OF_WINE then
		msg_print("That tastes good.")
		ident = TRUE
	elseif object.sval == SV_FOOD_WAYBREAD then
		msg_print("That tastes good.")
		set_poisoned(0)
		hp_player(damroll(4, 8))
		ident = TRUE
	elseif object.sval == SV_FOOD_RESTORING then
		if do_res_stat(A_STR) then ident = TRUE end
		if do_res_stat(A_INT) then ident = TRUE end
		if do_res_stat(A_WIS) then ident = TRUE end
		if do_res_stat(A_DEX) then ident = TRUE end
		if do_res_stat(A_CON) then ident = TRUE end
		if do_res_stat(A_CHR) then ident = TRUE end
	elseif object.sval == SV_FOOD_RESTORE_CON then
		if do_res_stat(A_CON) then ident = TRUE end
	elseif object.sval == SV_FOOD_RESTORE_STR then
		if do_res_stat(A_STR) then ident = TRUE end
	elseif object.sval == SV_FOOD_CURE_SERIOUS then
		if hp_player(damroll(4, 8)) then ident = TRUE end
	elseif object.sval == SV_FOOD_CURE_CONFUSION then
		if set_confused(0) then ident = TRUE end
	elseif object.sval == SV_FOOD_CURE_PARANOIA then
		if set_afraid(0) then ident = TRUE end
	elseif object.sval == SV_FOOD_CURE_BLINDNESS then
		if set_blind(0) then ident = TRUE end
	elseif object.sval == SV_FOOD_CURE_POISON then
		if set_poisoned(0) then ident = TRUE end
	elseif object.sval == SV_FOOD_DISEASE then
		take_hit(damroll(10, 10), "poisonous food")
		do_dec_stat(A_STR)
		ident = TRUE
	elseif object.sval == SV_FOOD_UNHEALTH then
		take_hit(damroll(10, 10), "poisonous food")
		do_dec_stat(A_CON)
		ident = TRUE
	elseif object.sval == SV_FOOD_NAIVETY then
		take_hit(damroll(8, 8), "poisonous food")
		do_dec_stat(A_WIS)
		ident = TRUE
	elseif object.sval == SV_FOOD_STUPIDITY then
		take_hit(damroll(8, 8), "poisonous food")
		do_dec_stat(A_INT)
		ident = TRUE
	elseif object.sval == SV_FOOD_SICKNESS then
		take_hit(damroll(6, 6), "poisonous food")
		do_dec_stat(A_CON)
		ident = TRUE
	elseif object.sval == SV_FOOD_WEAKNESS then
		take_hit(damroll(6, 6), "poisonous food")
		do_dec_stat(A_STR)
		ident = TRUE
	elseif object.sval == SV_FOOD_PARALYSIS then
		if not player.free_act then
			if set_paralyzed(player.paralyzed + rand_int(10) + 10) then
				ident = TRUE
			end
		end
	elseif object.sval == SV_FOOD_HALLUCINATION then
		if not player.resist_chaos then
			if set_image(player.image + rand_int(250) + 250) then
				ident = TRUE
			end
		end
	elseif object.sval == SV_FOOD_CONFUSION then
		if  not player.resist_confu then
			if set_confused(player.confused + rand_int(10) + 10) then
				ident = TRUE
			end
		end
	elseif object.sval == SV_FOOD_PARANOIA then
		if not player.resist_fear then
			if set_afraid(player.afraid + rand_int(10) + 10) then
				ident = TRUE
			end
		end
	elseif object.sval == SV_FOOD_POISON then
		if not (player.resist_pois or player.oppose_pois) then
			if set_poisoned(player.poisoned + rand_int(10) + 10) then
				ident = TRUE
			end
		end
	elseif object.sval == SV_FOOD_BLINDNESS then
		if not player.resist_blind then
			if set_blind(player.blind + rand_int(200) + 200) then
				ident = TRUE
			end
		end
	end

	return ident, TRUE
end


function quaff_potion(object)
	local ident = FALSE

	if object.sval == SV_POTION_WATER or
	   object.sval == SV_POTION_APPLE_JUICE or
	   object.sval == SV_POTION_SLIME_MOLD then
		msg_print("You feel less thirsty.")
		ident = TRUE
	elseif object.sval == SV_POTION_SLOWNESS then
		if set_slow(player.slow + rand_range(15, 40)) then ident = TRUE end
	elseif object.sval == SV_POTION_SALT_WATER then
		msg_print("The potion makes you vomit!")
		if player.food > PY_FOOD_STARVE - 1 then
			set_food(PY_FOOD_STARVE - 1)
		end
		set_poisoned(0)
		set_paralyzed(player.paralyzed + 4)
		ident = TRUE
	elseif object.sval == SV_POTION_POISON then
		if not (player.resist_pois or player.oppose_pois) then
			if set_poisoned(player.poisoned + rand_range(10, 25)) then
				ident = TRUE
			end
		end
	elseif object.sval == SV_POTION_BLINDNESS then
		if not player.resist_blind then
			if set_blind(player.blind + rand_range(100, 200)) then
				ident = TRUE
			end
		end
	-- Booze
	elseif object.sval == SV_POTION_CONFUSION then
		if not player.resist_confu then
			if set_confused(player.confused + rand_range(15, 35)) then
				ident = TRUE
			end
		end

		if not player.resist_chaos then
			if one_in_(2) then
				if set_image(player.image + rand_range(150, 300)) then
					ident = TRUE
				end
			end
			if one_in_(13) then
				ident = TRUE
				if one_in_(3) then
					lose_all_info()
				else
					wiz_dark()
				end
				teleport_player(100)
				wiz_dark()
				msg_print("You wake up somewhere with a sore head...")
				msg_print("You can't remember a thing, or how you got here!")
			end
		end
	elseif object.sval == SV_POTION_SLEEP then
		if not player.free_act then
			msg_print("You fall asleep.")

			if ironman_nightmare then
				msg_print("A horrible vision enters your mind.")

				-- Pick a nightmare
				get_mon_num_prep(get_nightmare, NULL)

				-- Have some nightmares
				have_nightmare(get_mon_num(MAX_DEPTH))

				-- Remove the monster restriction
				get_mon_num_prep(NULL, NULL)
			end

			if set_paralyzed(player.paralyzed + rand_range(4, 8)) then
				ident = TRUE
			end
		end
	elseif object.sval == SV_POTION_LOSE_MEMORIES then
		if not player.hold_life and (player.exp > 0) then
			msg_print("You feel your memories fade.")

			lose_exp(player.exp / 4)
			ident = TRUE
		end
	elseif object.sval == SV_POTION_RUINATION then
		msg_print("Your nerves and muscles feel weak and lifeless!")
		take_hit(damroll(10, 10), "a potion of Ruination")
		dec_stat(A_DEX, 25, TRUE)
		dec_stat(A_WIS, 25, TRUE)
		dec_stat(A_CON, 25, TRUE)
		dec_stat(A_STR, 25, TRUE)
		dec_stat(A_CHR, 25, TRUE)
		dec_stat(A_INT, 25, TRUE)
		ident = TRUE
	elseif object.sval == SV_POTION_DEC_STR then
		if do_dec_stat(A_STR) then ident = TRUE end
	elseif object.sval == SV_POTION_DEC_INT then
		if do_dec_stat(A_INT) then ident = TRUE end
	elseif object.sval == SV_POTION_DEC_WIS then
		if do_dec_stat(A_WIS) then ident = TRUE end
	elseif object.sval == SV_POTION_DEC_DEX then
		if do_dec_stat(A_DEX) then ident = TRUE end
	elseif object.sval == SV_POTION_DEC_CON then
		if do_dec_stat(A_CON) then ident = TRUE end
	elseif object.sval == SV_POTION_DEC_CHR then
		if do_dec_stat(A_CHR) then ident = TRUE end
	elseif object.sval == SV_POTION_DETONATIONS then
		msg_print("Massive explosions rupture your body!")
		take_hit(damroll(50, 20), "a potion of Detonation")
		set_stun(player.stun + 75)
		set_cut(player.cut + 5000)
		ident = TRUE
	elseif object.sval == SV_POTION_DEATH then
		msg_print("A feeling of Death flows through your body.")
		take_hit(5000, "a potion of Death")
		ident = TRUE
	elseif object.sval == SV_POTION_INFRAVISION then
		if set_tim_infra(player.tim_infra + rand_range(100, 200)) then
			ident = TRUE
		end
	elseif object.sval == SV_POTION_DETECT_INVIS then
		if set_tim_invis(player.tim_invis + rand_range(12, 24)) then
			ident = TRUE
		end
	elseif object.sval == SV_POTION_SLOW_POISON then
		if set_poisoned(player.poisoned / 2) then ident = TRUE end
	elseif object.sval == SV_POTION_CURE_POISON then
		if set_poisoned(0) then ident = TRUE end
	elseif object.sval == SV_POTION_BOLDNESS then
		if set_afraid(0) then ident = TRUE end
	elseif object.sval == SV_POTION_SPEED then
		if not player.fast then
			if set_fast(rand_range(15, 40)) then ident = TRUE end
		else
			set_fast(player.fast + 5)
		end
	elseif object.sval == SV_POTION_RESIST_HEAT then
		if set_oppose_fire(player.oppose_fire + rand_range(10, 20)) then
			ident = TRUE
		end
	elseif object.sval == SV_POTION_RESIST_COLD then
		if set_oppose_cold(player.oppose_cold + rand_range(10, 20)) then
			ident = TRUE
		end
	elseif object.sval == SV_POTION_HEROISM then
		if set_afraid(0) then ident = TRUE end
		if set_hero(player.hero + rand_range(25, 50)) then ident = TRUE end
		if hp_player(10)then ident = TRUE end
	elseif object.sval == SV_POTION_BERSERK_STRENGTH then
		if set_afraid(0) then ident = TRUE end
		if set_shero(player.shero + rand_range(25, 50)) then ident = TRUE end
		if hp_player(30) then ident = TRUE end
	elseif object.sval == SV_POTION_CURE_LIGHT then
		if hp_player(38) then ident = TRUE end
		if set_blind(0) then ident = TRUE end
		if set_cut(player.cut - 10) then ident = TRUE end
	elseif object.sval == SV_POTION_CURE_SERIOUS then
		if hp_player(75) then ident = TRUE end
		if set_blind(0) then ident = TRUE end
		if set_confused(0) then ident = TRUE end
		if set_cut((player.cut / 2) - 50) then ident = TRUE end
	elseif object.sval == SV_POTION_CURE_CRITICAL then
		if hp_player(150) then ident = TRUE end
		if set_blind(0) then ident = TRUE end
		if set_confused(0) then ident = TRUE end
		if set_poisoned(0) then ident = TRUE end
		if set_stun(0) then ident = TRUE end
		if set_cut(0) then ident = TRUE end
	elseif object.sval == SV_POTION_HEALING then
		if hp_player(300) then ident = TRUE end
		if set_blind(0) then ident = TRUE end
		if set_confused(0) then ident = TRUE end
		if set_poisoned(0) then ident = TRUE end
		if set_stun(0) then ident = TRUE end
		if set_cut(0) then ident = TRUE end
	elseif object.sval == SV_POTION_STAR_HEALING then
		if hp_player(1200) then ident = TRUE end
		if set_blind(0) then ident = TRUE end
		if set_confused(0) then ident = TRUE end
		if set_poisoned(0) then ident = TRUE end
		if set_stun(0) then ident = TRUE end
		if set_cut(0) then ident = TRUE end
	elseif object.sval == SV_POTION_LIFE then
		msg_print("You feel life flow through your body!")
		restore_level()
		set_poisoned(0)
		set_blind(0)
		set_confused(0)
		set_image(0)
		set_stun(0)
		set_cut(0)
		do_res_stat(A_STR)
		do_res_stat(A_CON)
		do_res_stat(A_DEX)
		do_res_stat(A_WIS)
		do_res_stat(A_INT)
		do_res_stat(A_CHR)

		-- Recalculate max. hitpoints
		update_stuff()

		hp_player(5000)
		ident = TRUE
	elseif object.sval == SV_POTION_RESTORE_MANA then
		if player.csp < player.msp then
			player.csp = player.msp
			player.csp_frac = 0
			msg_print("Your feel your head clear.")
			player.redraw = bOr(player.redraw, PR_MANA)
			player.window = bOr(player.window, PW_PLAYER)
			player.window = bOr(player.window, PW_SPELL)
			ident = TRUE
		end
	elseif object.sval == SV_POTION_RESTORE_EXP then
		if restore_level() then ident = TRUE end
	elseif object.sval == SV_POTION_RES_STR then
		if do_res_stat(A_STR) then ident = TRUE end
	elseif object.sval == SV_POTION_RES_INT then
		if do_res_stat(A_INT) then ident = TRUE end
	elseif object.sval == SV_POTION_RES_WIS then
		if do_res_stat(A_WIS) then ident = TRUE end
	elseif object.sval == SV_POTION_RES_DEX then
		if do_res_stat(A_DEX) then ident = TRUE end
	elseif object.sval == SV_POTION_RES_CON then
		if do_res_stat(A_CON) then ident = TRUE end
	elseif object.sval == SV_POTION_RES_CHR then
		if do_res_stat(A_CHR) then ident = TRUE end
	elseif object.sval == SV_POTION_INC_STR then
		if do_inc_stat(A_STR) then ident = TRUE end
	elseif object.sval == SV_POTION_INC_INT then
		if do_inc_stat(A_INT) then ident = TRUE end
	elseif object.sval == SV_POTION_INC_WIS then
		if do_inc_stat(A_WIS) then ident = TRUE end
	elseif object.sval == SV_POTION_INC_DEX then
		if do_inc_stat(A_DEX) then ident = TRUE end
	elseif object.sval == SV_POTION_INC_CON then
		if do_inc_stat(A_CON) then ident = TRUE end
	elseif object.sval == SV_POTION_INC_CHR then
		if do_inc_stat(A_CHR) then ident = TRUE end
	elseif object.sval == SV_POTION_AUGMENTATION then
		if do_inc_stat(A_STR) then ident = TRUE end
		if do_inc_stat(A_INT) then ident = TRUE end
		if do_inc_stat(A_WIS) then ident = TRUE end
		if do_inc_stat(A_DEX) then ident = TRUE end
		if do_inc_stat(A_CON) then ident = TRUE end
		if do_inc_stat(A_CHR) then ident = TRUE end
	elseif object.sval == SV_POTION_ENLIGHTENMENT then
		msg_print("An image of your surroundings forms in your mind...")
		wiz_lite()
		ident = TRUE
	elseif object.sval == SV_POTION_STAR_ENLIGHTENMENT then
		msg_print("You begin to feel more enlightened...")
		message_flush()
		wiz_lite()
		do_inc_stat(A_INT)
		do_inc_stat(A_WIS)
		detect_traps()
		detect_doors()
		detect_stairs()
		detect_treasure()
		detect_objects_gold()
		detect_objects_normal()
		identify_pack()
		self_knowledge()
		ident = TRUE
	elseif object.sval == SV_POTION_SELF_KNOWLEDGE then
		msg_print("You begin to know yourself a little better...")
		message_flush()
		self_knowledge()
		ident = TRUE
	elseif object.sval == SV_POTION_EXPERIENCE then
		if player.exp < PY_MAX_EXP then
			local ee = (player.exp / 2) + 10
			if ee > 100000 then ee = 100000 end
			msg_print("You feel more experienced.")
			gain_exp(ee)
			ident = TRUE
		end
	elseif object.sval == SV_POTION_RESISTANCE then
		set_oppose_acid(player.oppose_acid + rand_range(20, 40))
		set_oppose_elec(player.oppose_elec + rand_range(20, 40))
		set_oppose_fire(player.oppose_fire + rand_range(20, 40))
		set_oppose_cold(player.oppose_cold + rand_range(20, 40))
		set_oppose_pois(player.oppose_pois + rand_range(20, 40))
		ident = TRUE
	elseif object.sval == SV_POTION_CURING then
		if hp_player(150) then ident = TRUE end
		if set_blind(0) then ident = TRUE end
		if set_poisoned(0) then ident = TRUE end
		if set_confused(0) then ident = TRUE end
		if set_stun(0) then ident = TRUE end
		if set_cut(0) then ident = TRUE end
		if set_image(0) then ident = TRUE end
	elseif object.sval == SV_POTION_INVULNERABILITY then
		set_invuln(player.invuln + rand_range(7, 14))
		ident = TRUE
	elseif object.sval == SV_POTION_NEW_LIFE then
		do_cmd_rerate()
		if player.muta1 or player.muta2 or player.muta3 then
			msg_print("You are cured of all mutations.")
			player.muta1 = 0
			player.muta2 = 0
			player.muta3 = 0
			player.update = bOr(player.update, PU_BONUS)
			handle_stuff()
		end
		ident = TRUE
	end

	return ident, TRUE
end


function use_object_hook(object)
	local ident = FALSE
	local used = FALSE

	if object.tval == TV_FOOD then
		ident, used = eat_food(object)
	elseif object.tval == TV_POTION then
		ident, used = quaff_potion(object)
	end

	return ident, used
end

