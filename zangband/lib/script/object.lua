-- Use objects

function eat_food(object)
	local ident = 0

	if object.sval == SV_FOOD_RATION or
	   object.sval == SV_FOOD_BISCUIT or
	   object.sval == SV_FOOD_JERKY or
	   object.sval == SV_FOOD_SLIME_MOLD or
	   object.sval == SV_FOOD_PINT_OF_ALE or
	   object.sval == SV_FOOD_PINT_OF_WINE then
		msg_print("That tastes good.")
		ident = 1
	elseif object.sval == SV_FOOD_WAYBREAD then
		msg_print("That tastes good.")
		set_poisoned(0)
		hp_player(damroll(4, 8))
		ident = 1
	elseif object.sval == SV_FOOD_RESTORING then
		if do_res_stat(A_STR) then ident = 1 end
		if do_res_stat(A_INT) then ident = 1 end
		if do_res_stat(A_WIS) then ident = 1 end
		if do_res_stat(A_DEX) then ident = 1 end
		if do_res_stat(A_CON) then ident = 1 end
		if do_res_stat(A_CHR) then ident = 1 end
	elseif object.sval == SV_FOOD_RESTORE_CON then
		if do_res_stat(A_CON) then ident = 1 end
	elseif object.sval == SV_FOOD_RESTORE_STR then
		if do_res_stat(A_STR) then ident = 1 end
	elseif object.sval == SV_FOOD_CURE_SERIOUS then
		if hp_player(damroll(4, 8)) then ident = 1 end
	elseif object.sval == SV_FOOD_CURE_CONFUSION then
		if set_confused(0) then ident = 1 end
	elseif object.sval == SV_FOOD_CURE_PARANOIA then
		if set_afraid(0) then ident = 1 end
	elseif object.sval == SV_FOOD_CURE_BLINDNESS then
		if set_blind(0) then ident = 1 end
	elseif object.sval == SV_FOOD_CURE_POISON then
		if set_poisoned(0) then ident = 1 end
	elseif object.sval == SV_FOOD_DISEASE then
		take_hit(damroll(10, 10), "poisonous food")
		do_dec_stat(A_STR)
		ident = 1
	elseif object.sval == SV_FOOD_UNHEALTH then
		take_hit(damroll(10, 10), "poisonous food")
		do_dec_stat(A_CON)
		ident = 1
	elseif object.sval == SV_FOOD_NAIVETY then
		take_hit(damroll(8, 8), "poisonous food")
		do_dec_stat(A_WIS)
		ident = 1
	elseif object.sval == SV_FOOD_STUPIDITY then
		take_hit(damroll(8, 8), "poisonous food")
		do_dec_stat(A_INT)
		ident = 1
	elseif object.sval == SV_FOOD_SICKNESS then
		take_hit(damroll(6, 6), "poisonous food")
		do_dec_stat(A_CON)
		ident = 1
	elseif object.sval == SV_FOOD_WEAKNESS then
		take_hit(damroll(6, 6), "poisonous food")
		do_dec_stat(A_STR)
		ident = 1
	elseif object.sval == SV_FOOD_PARALYSIS then
		if not p_ptr.free_act then
			if set_paralyzed(p_ptr.paralyzed + rand_int(10) + 10) then
				ident = 1
			end
		end
	elseif object.sval == SV_FOOD_HALLUCINATION then
		if not p_ptr.resist_chaos then
			if set_image(p_ptr.image + rand_int(250) + 250) then
				ident = 1
			end
		end
	elseif object.sval == SV_FOOD_CONFUSION then
		if  not p_ptr.resist_confu then
			if set_confused(p_ptr.confused + rand_int(10) + 10) then
				ident = 1
			end
		end
	elseif object.sval == SV_FOOD_PARANOIA then
		if not p_ptr.resist_fear then
			if set_afraid(p_ptr.afraid + rand_int(10) + 10) then
				ident = 1
			end
		end
	elseif object.sval == SV_FOOD_POISON then
		if not (p_ptr.resist_pois or p_ptr.oppose_pois) then
			if set_poisoned(p_ptr.poisoned + rand_int(10) + 10) then
				ident = 1
			end
		end
	elseif object.sval == SV_FOOD_BLINDNESS then
		if not p_ptr.resist_blind then
			if set_blind(p_ptr.blind + rand_int(200) + 200) then
				ident = 1
			end
		end
	end

	return ident, 1
end


function use_object_hook(object)
	local ident = 0
	local used = 0

	if object.tval == TV_FOOD then
		ident, used = eat_food(object)
	end

	return ident, used
end

