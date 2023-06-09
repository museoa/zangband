/* CVS: Last edit by $Author$ on $Date$ */
/* File: melee1.c */

/* Purpose: Monster attacks */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


#include "tnb.h" /* TNB */


/*
 * Critical blow.  All hits that do 95% of total possible damage,
 * and which also do at least 20 damage, or, sometimes, N damage.
 * This is used only to determine "cuts" and "stuns".
 */
static int monster_critical(int dice, int sides, int dam)
{
	int max = 0;
	int total = dice * sides;

	/* Must do at least 95% of perfect */
	if (dam < total * 19 / 20)
		return (0);

	/* Weak blows rarely work */
	if ((dam < 20) && (rand_int(100) >= dam))
		return (0);

	/* Perfect damage */
	if (dam == total)
		max++;

	/* Super-charge */
	if (dam >= 20)
	{
		while (rand_int(100) < 2)
			max++;
	}

	/* Critical damage */
	if (dam > 160)
		return (6 + max);
	if (dam > 80)
		return (5 + max);
	if (dam > 40)
		return (4 + max);
	if (dam > 20)
		return (3 + max);
	if (dam > 10)
		return (2 + max);
	return (1 + max);
}





/*
 * Determine if a monster attack against the player succeeds.
 * Always miss 5% of the time, Always hit 5% of the time.
 * Otherwise, match monster power against player armor.
 */
static int check_hit(int power, int level)
{
	int i, k, ac;

	/* Percentile dice */
	k = rand_int(100);

	/* Hack -- Always miss or hit */
	if (k < 10)
		return (k < 5);

	/* Calculate the "attack quality" */
	i = (power + (level * 3));

	/* Total armor */
	ac = p_ptr->ac + p_ptr->to_a;

	/* Power and Level compete against Armor */
	if ((i > 0) && (randint(i) > ((ac * 3) / 4)))
		return (TRUE);

	/* Assume miss */
	return (FALSE);
}



/*
 * Hack -- possible "insult" messages
 */
static cptr desc_insult[] = {
	"insults you!",
	"insults your mother!",
	"gives you the finger!",
	"humiliates you!",
	"defiles you!",
	"dances around you!",
	"makes obscene gestures!",
	"moons you!!!"
};



/*
 * Hack -- possible "insult" messages
 */
static cptr desc_moan[] = {
	"seems sad about something.",
	"asks if you have seen his dogs.",
	"tells you to get off his land.",
	"mumbles something about mushrooms."
};


/*
 * Attack the player via physical attacks.
 */
bool make_attack_normal(int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	int ap_cnt;

	int i, k, tmp, ac, rlev;
	int do_cut, do_stun;

	s32b gold;

	object_type *o_ptr;

	object_kind *k_ptr;

	char o_name[O_NAME_MAX];

	char m_name[80];

	char ddesc[80];

	bool blinked;
	bool touched = FALSE, fear = FALSE, alive = TRUE;
	bool explode = FALSE;
	bool resist_drain = FALSE;

	/* Not allowed to attack */
	if (r_ptr->flags1 & (RF1_NEVER_BLOW))
		return (FALSE);

	/* ...nor if friendly */
	if (!is_hostile(m_ptr))
		return FALSE;

	/* Total armor */
	ac = p_ptr->ac + p_ptr->to_a;

	/* Extract the effective monster level */
	rlev = ((r_ptr->level >= 1) ? r_ptr->level : 1);


	/* Get the monster name (or "it") */
	monster_desc(m_name, m_ptr, 0);

	/* Get the "died from" information (i.e. "a kobold") */
	monster_desc(ddesc, m_ptr, 0x88);


	/* Assume no blink */
	blinked = FALSE;

	/* Scan through all four blows */
	for (ap_cnt = 0; ap_cnt < 4; ap_cnt++)
	{
		bool visible = FALSE;
		bool obvious = FALSE;

		int power = 0;
		int damage = 0;

		cptr act = NULL;

		/* Extract the attack infomation */
		int effect = r_ptr->blow[ap_cnt].effect;
		int method = r_ptr->blow[ap_cnt].method;
		int d_dice = r_ptr->blow[ap_cnt].d_dice;
		int d_side = r_ptr->blow[ap_cnt].d_side;


		/* Hack -- no more attacks */
		if (!method)
			break;


		/* Stop if player is dead or gone */
		if (!alive || death)
			break;

		/* Handle "leaving" */
		if (p_ptr->leaving)
			break;

		/* Extract visibility (before blink) */
		if (m_ptr->ml)
			visible = TRUE;

		/* Extract the attack "power" */
		switch (effect)
		{
			case RBE_HURT:
				power = 60;
				break;
			case RBE_POISON:
				power = 5;
				break;
			case RBE_UN_BONUS:
				power = 20;
				break;
			case RBE_UN_POWER:
				power = 15;
				break;
			case RBE_EAT_GOLD:
				power = 5;
				break;
			case RBE_EAT_ITEM:
				power = 5;
				break;
			case RBE_EAT_FOOD:
				power = 5;
				break;
			case RBE_EAT_LITE:
				power = 5;
				break;
			case RBE_ACID:
				power = 0;
				break;
			case RBE_ELEC:
				power = 10;
				break;
			case RBE_FIRE:
				power = 10;
				break;
			case RBE_COLD:
				power = 10;
				break;
			case RBE_BLIND:
				power = 2;
				break;
			case RBE_CONFUSE:
				power = 10;
				break;
			case RBE_TERRIFY:
				power = 10;
				break;
			case RBE_PARALYZE:
				power = 2;
				break;
			case RBE_LOSE_STR:
				power = 0;
				break;
			case RBE_LOSE_DEX:
				power = 0;
				break;
			case RBE_LOSE_CON:
				power = 0;
				break;
			case RBE_LOSE_INT:
				power = 0;
				break;
			case RBE_LOSE_WIS:
				power = 0;
				break;
			case RBE_LOSE_CHR:
				power = 0;
				break;
			case RBE_LOSE_ALL:
				power = 2;
				break;
			case RBE_SHATTER:
				power = 60;
				break;
			case RBE_EXP_10:
				power = 5;
				break;
			case RBE_EXP_20:
				power = 5;
				break;
			case RBE_EXP_40:
				power = 5;
				break;
			case RBE_EXP_80:
				power = 5;
				break;
			case RBE_DISEASE:
				power = 5;
				break;
			case RBE_TIME:
				power = 5;
				break;
			case RBE_EXP_VAMP:
				power = 5;
				break;
		}


		/* Monster hits player */
		if (!effect || check_hit(power, rlev))
		{
			/* Always disturbing */
			disturb(1, 0);


			/* Hack -- Apply "protection from evil" */
			if ((p_ptr->protevil > 0) && (r_ptr->flags3 & RF3_EVIL) &&
				(p_ptr->lev >= rlev) &&
				((rand_int(100) + p_ptr->lev) > 50))
			{
				/* Remember the Evil-ness */
				if (m_ptr->ml)
				{
					r_ptr->r_flags3 |= RF3_EVIL;
				}

				/* Message */
				msg_format("%^s is repelled.", m_name);

				/* Hack -- Next attack */
				continue;
			}


			/* Assume no cut or stun */
			do_cut = do_stun = 0;

			/* Describe the attack method */
			switch (method)
			{
				case RBM_HIT:
				{
					act = "hits you.";
					do_cut = do_stun = 1;
					touched = TRUE;
					break;
				}

				case RBM_TOUCH:
				{
					act = "touches you.";
					touched = TRUE;
					break;
				}

				case RBM_PUNCH:
				{
					act = "punches you.";
					touched = TRUE;
					do_stun = 1;
					break;
				}

				case RBM_KICK:
				{
					act = "kicks you.";
					touched = TRUE;
					do_stun = 1;
					break;
				}

				case RBM_CLAW:
				{
					act = "claws you.";
					touched = TRUE;
					do_cut = 1;
					break;
				}

				case RBM_BITE:
				{
					act = "bites you.";
					do_cut = 1;
					touched = TRUE;
					break;
				}

				case RBM_STING:
				{
					act = "stings you.";
					touched = TRUE;
					break;
				}

				case RBM_XXX1:
				{
					act = "XXX1's you.";
					break;
				}

				case RBM_BUTT:
				{
					act = "butts you.";
					do_stun = 1;
					touched = TRUE;
					break;
				}

				case RBM_CRUSH:
				{
					act = "crushes you.";
					do_stun = 1;
					touched = TRUE;
					break;
				}

				case RBM_ENGULF:
				{
					act = "engulfs you.";
					touched = TRUE;
					break;
				}

				case RBM_CHARGE:
				{
					act = "charges you.";
					touched = TRUE;
					break;
				}

				case RBM_CRAWL:
				{
					act = "crawls on you.";
					touched = TRUE;
					break;
				}

				case RBM_DROOL:
				{
					act = "drools on you.";
					break;
				}

				case RBM_SPIT:
				{
					act = "spits on you.";
					break;
				}

				case RBM_EXPLODE:
				{
					act = "explodes.";
					explode = TRUE;
					break;
				}

				case RBM_GAZE:
				{
					act = "gazes at you.";
					break;
				}

				case RBM_WAIL:
				{
					act = "wails at you.";
					break;
				}

				case RBM_SPORE:
				{
					act = "releases spores at you.";
					break;
				}

				case RBM_XXX4:
				{
					act = "projects XXX4's at you.";
					break;
				}

				case RBM_BEG:
				{
					act = "begs you for money.";
					break;
				}

				case RBM_INSULT:
				{
					act = desc_insult[rand_int(8)];
					break;
				}

				case RBM_MOAN:
				{
					act = desc_moan[rand_int(4)];
					break;
				}

				case RBM_SHOW:
				{
					if (randint(3) == 1)
						act = "sings 'We are a happy family.'";
					else
						act = "sings 'I love you, you love me.'";
					break;
				}
			}

			/* Message */
			/* Message */
			if (act)
			{
				if ((p_ptr->image) && (randint(3) == 1))
				{
					msg_format("%^s %s you.", m_name,
						silly_attacks[randint(MAX_SILLY_ATTACK) - 1]);
				}
				else
				{
					msg_format("%^s %s", m_name, act);
				}
			}


			/* Hack -- assume all attacks are obvious */
			obvious = TRUE;

			/* Roll out the damage */
			damage = damroll(d_dice, d_side);

			/*
			 * Skip the effect when exploding, since the explosion
			 * already causes the effect.
			 */
			if (!explode)
			{
				/* Apply appropriate damage */
				switch (effect)
				{
					case 0:
					{
						/* Hack -- Assume obvious */
						obvious = TRUE;

						/* Hack -- No damage */
						damage = 0;

						break;
					}

					case RBE_HURT:
					{
						/* Obvious */
						obvious = TRUE;

						/* Hack -- Player armor reduces total damage */
						damage -= (damage * ((ac < 150) ? ac : 150) / 250);

						/* Take damage */
						take_hit(damage, ddesc);

						break;
					}

					case RBE_POISON:
					{
						/* Take some damage */
						take_hit(damage, ddesc);

						/* Take "poison" effect */
						if (!(p_ptr->resist_pois || p_ptr->oppose_pois))
						{
							if (set_poisoned(p_ptr->poisoned +
									randint(rlev) + 5))
							{
								obvious = TRUE;
							}
						}

						/* Learn about the player */
						update_smart_learn(m_idx, DRS_POIS);

						break;
					}

					case RBE_UN_BONUS:
					{
						/* Take some damage */
						take_hit(damage, ddesc);

						/* Allow complete resist */
						if (!p_ptr->resist_disen)
						{
							/* Apply disenchantment */
							if (apply_disenchant(0))
								obvious = TRUE;
						}

						/* Learn about the player */
						update_smart_learn(m_idx, DRS_DISEN);

						break;
					}

					case RBE_UN_POWER:
					{
						/* Take some damage */
						take_hit(damage, ddesc);

						/* Find an item */
						for (k = 0; k < 10; k++)
						{
							/* Pick an item */
							i = rand_int(INVEN_PACK);

							/* Obtain the item */
							o_ptr = &inventory[i];

							/* Skip non-objects */
							if (!o_ptr->k_idx)
								continue;

							/* Drain charged wands/staffs */
							if (((o_ptr->tval == TV_STAFF) ||
									(o_ptr->tval == TV_WAND)) &&
								(o_ptr->pval))
							{
								/* Calculate healed hitpoints */
								int heal =
									rlev * o_ptr->pval * o_ptr->number;

								/* Don't heal more than max hp */
								heal = MIN(heal, m_ptr->maxhp - m_ptr->hp);

								msg_print("Energy drains from your pack!");

								/* Obvious */
								obvious = TRUE;

								/* Heal the monster */
								m_ptr->hp += heal;

								/* Redraw (later) if needed */
								if (p_ptr->health_who == m_idx)
									p_ptr->redraw |= (PR_HEALTH);

								/* Uncharge */
								o_ptr->pval = 0;

								/* Combine / Reorder the pack */
								p_ptr->notice |= (PN_COMBINE | PN_REORDER);

								/* Window stuff */
								p_ptr->window |= (PW_INVEN);

								/* Done */
								break;
							}
						}

						break;
					}

					case RBE_EAT_GOLD:
					{
						/* Take some damage */
						take_hit(damage, ddesc);

						/* Confused monsters cannot steal successfully. -LM- */
						if (m_ptr->confused)
							break;

						/* Obvious */
						obvious = TRUE;

						/* Saving throw (unless paralyzed) based on dex and level */
						if (!p_ptr->paralyzed &&
							(rand_int(100) <
		   (adj_dex_safe[p_ptr->stat_ind[A_DEX]] + p_ptr->lev)))
						{
							/* Saving throw message */
							msg_print
								("You quickly protect your money pouch!");

							/* Occasional blink anyway */
							if (rand_int(3))
								blinked = TRUE;
						}

						/* Eat gold */
						else
						{
							gold = (p_ptr->au / 10) + randint(25);
							if (gold < 2)
								gold = 2;
							if (gold > 5000)
								gold = (p_ptr->au / 20) + randint(3000);
							if (gold > p_ptr->au)
								gold = p_ptr->au;
							p_ptr->au -= gold;
							if (gold <= 0)
							{
								msg_print("Nothing was stolen.");
							}
							else if (p_ptr->au)
							{
								msg_print("Your purse feels lighter.");
								msg_format("%ld coins were stolen!",
									(long) gold);

								chg_virtue(V_SACRIFICE, 1);
							}
							else
							{
								msg_print("Your purse feels lighter.");
								msg_print
									("All of your coins were stolen!");

								chg_virtue(V_SACRIFICE, 2);
							}

#if 1 /* TNB */
							/* Steal that money! */
							if (gold)
							{
								monster_steal_gold(m_idx, gold);
							}
#endif /* TNB */

							/* Redraw gold */
							p_ptr->redraw |= (PR_GOLD);

							/* Window stuff */
							p_ptr->window |= (PW_PLAYER);

							/* Blink away */
							blinked = TRUE;
						}

						break;
					}

					case RBE_EAT_ITEM:
					{
						/* Take some damage */
						take_hit(damage, ddesc);

						/* Confused monsters cannot steal successfully. -LM- */
						if (m_ptr->confused)
							break;

						/* Saving throw (unless paralyzed) based on dex and level */
						if (!p_ptr->paralyzed &&
							(rand_int(100) <
		   (adj_dex_safe[p_ptr->stat_ind[A_DEX]] + p_ptr->lev)))
						{
							/* Saving throw message */
							msg_print("You grab hold of your backpack!");

							/* Occasional "blink" anyway */
							blinked = TRUE;

							/* Obvious */
							obvious = TRUE;

							/* Done */
							break;
						}

						/* Find an item */
						for (k = 0; k < 10; k++)
						{
							/* Pick an item */
							i = rand_int(INVEN_PACK);

							/* Obtain the item */
							o_ptr = &inventory[i];

							/* Skip non-objects */
							if (!o_ptr->k_idx)
								continue;

							/* Skip artifacts */
							if (artifact_p(o_ptr) || o_ptr->art_name)
								continue;

							/* Get a description */
							object_desc(o_name, o_ptr, FALSE, 3);

							/* Message */
							msg_format("%sour %s (%c) was stolen!",
								((o_ptr->number > 1) ? "One of y" : "Y"),
								o_name, index_to_label(i));

							chg_virtue(V_SACRIFICE, 1);

							/* Option */
							if (testing_carry)
							{
								s16b o_idx;

								/* Make an object */
								o_idx = o_pop();

								/* Success */
								if (o_idx)
								{
									object_type *j_ptr;

									/* Get new object */
									j_ptr = &o_list[o_idx];

									/* Copy object */
									object_copy(j_ptr, o_ptr);

									/* Modify number */
									j_ptr->number = 1;

									/* Hack -- If a rod or wand, allocate total
									 * maximum timeouts or charges between those
									 * stolen and those missed. -LM-
									 */
									if ((o_ptr->tval == TV_ROD) ||
										(o_ptr->tval == TV_WAND))
									{
										k_ptr = &k_info[o_ptr->k_idx];
										j_ptr->pval =
											o_ptr->pval / o_ptr->number;
										o_ptr->pval -= j_ptr->pval;
									}

									/* Forget mark */
									j_ptr->marked = FALSE;

									/* Memorize monster */
									j_ptr->held_m_idx = m_idx;

									/* Build stack */
									j_ptr->next_o_idx = m_ptr->hold_o_idx;

									/* Build stack */
									m_ptr->hold_o_idx = o_idx;
								}
							}
							else
							{
								if (strstr((r_name + r_ptr->name),
										"black market") && randint(2) != 1)
								{
									s16b o_idx;

									/* Make an object */
									o_idx = o_pop();

									/* Success */
									if (o_idx)
									{
										object_type *j_ptr;
										if (cheat_xtra || cheat_peek)
											msg_print
												("Moving object to black market...");

										/* Get new object */
										j_ptr = &o_list[o_idx];

										/* Copy object */
										object_copy(j_ptr, o_ptr);

										/* Modify number */
										j_ptr->number = 1;

										/* Hack -- If a rod or wand, allocate total
										 * maximum timeouts or charges between those
										 * stolen and those missed. -LM-
										 */
										if ((o_ptr->tval == TV_ROD) ||
											(o_ptr->tval == TV_WAND))
										{
											k_ptr = &k_info[o_ptr->k_idx];
											j_ptr->pval =
												o_ptr->pval /
												o_ptr->number;
											o_ptr->pval -= j_ptr->pval;
										}

										/* Forget mark */
										j_ptr->marked = FALSE;

										move_to_black_market(j_ptr);
									}
								}
							}

							/* Steal the items */
							inven_item_increase(i, -1);
							inven_item_optimize(i);

							/* Obvious */
							obvious = TRUE;

							/* Blink away */
							blinked = TRUE;

							/* Done */
							break;
						}

						break;
					}

					case RBE_EAT_FOOD:
					{
						/* Take some damage */
						take_hit(damage, ddesc);

						/* Steal some food */
						for (k = 0; k < 10; k++)
						{
							/* Pick an item from the pack */
							i = rand_int(INVEN_PACK);

							/* Get the item */
							o_ptr = &inventory[i];

							/* Skip non-objects */
							if (!o_ptr->k_idx)
								continue;

							/* Skip non-food objects */
							if (o_ptr->tval != TV_FOOD)
								continue;

							/* Get a description */
							object_desc(o_name, o_ptr, FALSE, 0);

							/* Message */
							msg_format("%sour %s (%c) was eaten!",
								((o_ptr->number > 1) ? "One of y" : "Y"),
								o_name, index_to_label(i));

							/* Steal the items */
							inven_item_increase(i, -1);
							inven_item_optimize(i);

							/* Obvious */
							obvious = TRUE;

							/* Done */
							break;
						}

						break;
					}

					case RBE_EAT_LITE:
					{
						/* Take some damage */
						take_hit(damage, ddesc);

						/* Access the lite */
						o_ptr = &inventory[INVEN_LITE];

						/* Drain fuel */
						if ((o_ptr->pval > 0) && (!artifact_p(o_ptr)))
						{
							/* Reduce fuel */
							o_ptr->pval -= (250 + randint(250));
							if (o_ptr->pval < 1)
								o_ptr->pval = 1;

							/* Notice */
							if (!p_ptr->blind)
							{
								msg_print("Your light dims.");
								obvious = TRUE;
							}

							/* Window stuff */
							p_ptr->window |= (PW_EQUIP);
						}

						break;
					}

					case RBE_ACID:
					{
						/* Obvious */
						obvious = TRUE;

						/* Message */
						msg_print("You are covered in acid!");

						/* Special damage */
						acid_dam(damage, ddesc);

						/* Learn about the player */
						update_smart_learn(m_idx, DRS_ACID);

						break;
					}

					case RBE_ELEC:
					{
						/* Obvious */
						obvious = TRUE;

						/* Message */
						msg_print("You are struck by electricity!");

						/* Special damage */
						elec_dam(damage, ddesc);

						/* Learn about the player */
						update_smart_learn(m_idx, DRS_ELEC);

						break;
					}

					case RBE_FIRE:
					{
						/* Obvious */
						obvious = TRUE;

						/* Message */
						msg_print("You are enveloped in flames!");

						/* Special damage */
						fire_dam(damage, ddesc);

						/* Learn about the player */
						update_smart_learn(m_idx, DRS_FIRE);

						break;
					}

					case RBE_COLD:
					{
						/* Obvious */
						obvious = TRUE;

						/* Message */
						msg_print("You are covered with frost!");

						/* Special damage */
						cold_dam(damage, ddesc);

						/* Learn about the player */
						update_smart_learn(m_idx, DRS_COLD);

						break;
					}

					case RBE_BLIND:
					{
						/* Take damage */
						take_hit(damage, ddesc);

						/* Increase "blind" */
						if (!p_ptr->resist_blind)
						{
							if (set_blind(p_ptr->blind + 10 +
									randint(rlev)))
							{
								obvious = TRUE;
							}
						}

						/* Learn about the player */
						update_smart_learn(m_idx, DRS_BLIND);

						break;
					}

					case RBE_CONFUSE:
					{
						/* Take damage */
						take_hit(damage, ddesc);

						/* Increase "confused" */
						if (!p_ptr->resist_conf)
						{
							if (set_confused(p_ptr->confused + 3 +
									randint(rlev)))
							{
								obvious = TRUE;
							}
						}

						/* Learn about the player */
						update_smart_learn(m_idx, DRS_CONF);

						break;
					}

					case RBE_TERRIFY:
					{
						/* Take damage */
						take_hit(damage, ddesc);

						/* Increase "afraid" */
						if (p_ptr->resist_fear)
						{
							msg_print("You stand your ground!");
							obvious = TRUE;
						}
						else if (rand_int(100) < p_ptr->skill_sav)
						{
							msg_print("You stand your ground!");
							obvious = TRUE;
						}
						else
						{
							if (set_afraid(p_ptr->afraid + 3 +
									randint(rlev)))
							{
								obvious = TRUE;
							}
						}

						/* Learn about the player */
						update_smart_learn(m_idx, DRS_FEAR);

						break;
					}

					case RBE_PARALYZE:
					{
						/* Hack -- Prevent perma-paralysis via damage */
						if (p_ptr->paralyzed && (damage < 1))
							damage = 1;

						/* Take damage */
						take_hit(damage, ddesc);

						/* Increase "paralyzed" */
						if (p_ptr->free_act)
						{
							msg_print("You are unaffected!");
							obvious = TRUE;
						}
						else if (rand_int(100) < p_ptr->skill_sav)
						{
							msg_print("You resist the effects!");
							obvious = TRUE;
						}
						else
						{
							if (set_paralyzed(p_ptr->paralyzed + 3 +
									randint(rlev)))
							{
								obvious = TRUE;
							}
						}

						/* Learn about the player */
						update_smart_learn(m_idx, DRS_FREE);

						break;
					}

					case RBE_LOSE_STR:
					{
						/* Damage (physical) */
						take_hit(damage, ddesc);

						/* Damage (stat) */
						if (do_dec_stat(A_STR))
							obvious = TRUE;

						break;
					}

					case RBE_LOSE_INT:
					{
						/* Damage (physical) */
						take_hit(damage, ddesc);

						/* Damage (stat) */
						if (do_dec_stat(A_INT))
							obvious = TRUE;

						break;
					}

					case RBE_LOSE_WIS:
					{
						/* Damage (physical) */
						take_hit(damage, ddesc);

						/* Damage (stat) */
						if (do_dec_stat(A_WIS))
							obvious = TRUE;

						break;
					}

					case RBE_LOSE_DEX:
					{
						/* Damage (physical) */
						take_hit(damage, ddesc);

						/* Damage (stat) */
						if (do_dec_stat(A_DEX))
							obvious = TRUE;

						break;
					}

					case RBE_LOSE_CON:
					{
						/* Damage (physical) */
						take_hit(damage, ddesc);

						/* Damage (stat) */
						if (do_dec_stat(A_CON))
							obvious = TRUE;

						break;
					}

					case RBE_LOSE_CHR:
					{
						/* Damage (physical) */
						take_hit(damage, ddesc);

						/* Damage (stat) */
						if (do_dec_stat(A_CHR))
							obvious = TRUE;

						break;
					}

					case RBE_LOSE_ALL:
					{
						/* Damage (physical) */
						take_hit(damage, ddesc);

						/* Damage (stats) */
						if (do_dec_stat(A_STR))
							obvious = TRUE;
						if (do_dec_stat(A_DEX))
							obvious = TRUE;
						if (do_dec_stat(A_CON))
							obvious = TRUE;
						if (do_dec_stat(A_INT))
							obvious = TRUE;
						if (do_dec_stat(A_WIS))
							obvious = TRUE;
						if (do_dec_stat(A_CHR))
							obvious = TRUE;

						break;
					}

					case RBE_SHATTER:
					{
						/* Obvious */
						obvious = TRUE;

						/* Hack -- Reduce damage based on the player armor class */
						damage -= (damage * ((ac < 150) ? ac : 150) / 250);

						/* Take damage */
						take_hit(damage, ddesc);

						/* Radius 8 earthquake centered at the monster */
						if (damage > 23)
						{
							earthquake(m_ptr->fy, m_ptr->fx, 8);
						}

						break;
					}

					case RBE_EXP_10:
					{
						/* Obvious */
						obvious = TRUE;

						/* Take damage */
						take_hit(damage, ddesc);

						if (p_ptr->hold_life && (rand_int(100) < 95))
						{
							msg_print("You keep hold of your life force!");
						}
						else
						{
							s32b d =
								damroll(10,
								6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;
							if (p_ptr->hold_life)
							{
								msg_print
									("You feel your life slipping away!");
								lose_exp(d / 10);
							}
							else
							{
								msg_print
									("You feel your life draining away!");
								lose_exp(d);
							}
						}
						break;
					}

					case RBE_EXP_20:
					{
						/* Obvious */
						obvious = TRUE;

						/* Take damage */
						take_hit(damage, ddesc);

						if (p_ptr->hold_life && (rand_int(100) < 90))
						{
							msg_print("You keep hold of your life force!");
						}
						else
						{
							s32b d =
								damroll(20,
								6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;
							if (p_ptr->hold_life)
							{
								msg_print
									("You feel your life slipping away!");
								lose_exp(d / 10);
							}
							else
							{
								msg_print
									("You feel your life draining away!");
								lose_exp(d);
							}
						}
						break;
					}

					case RBE_EXP_40:
					{
						/* Obvious */
						obvious = TRUE;

						/* Take damage */
						take_hit(damage, ddesc);

						if (p_ptr->hold_life && (rand_int(100) < 75))
						{
							msg_print("You keep hold of your life force!");
						}
						else
						{
							s32b d =
								damroll(40,
								6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;
							if (p_ptr->hold_life)
							{
								msg_print
									("You feel your life slipping away!");
								lose_exp(d / 10);
							}
							else
							{
								msg_print
									("You feel your life draining away!");
								lose_exp(d);
							}
						}
						break;
					}

					case RBE_EXP_80:
					{
						/* Obvious */
						obvious = TRUE;

						/* Take damage */
						take_hit(damage, ddesc);

						if (p_ptr->hold_life && (rand_int(100) < 50))
						{
							msg_print("You keep hold of your life force!");
						}
						else
						{
							s32b d =
								damroll(80,
								6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;
							if (p_ptr->hold_life)
							{
								msg_print
									("You feel your life slipping away!");
								lose_exp(d / 10);
							}
							else
							{
								msg_print
									("You feel your life draining away!");
								lose_exp(d);
							}
						}
						break;
					}

					case RBE_DISEASE:
					{
						/* Take some damage */
						take_hit(damage, ddesc);

						/* Take "poison" effect */
						if (!(p_ptr->resist_pois || p_ptr->oppose_pois))
						{
							if (set_poisoned(p_ptr->poisoned +
									randint(rlev) + 5))
							{
								obvious = TRUE;
							}
						}

						/* Damage CON (10% chance) */
						if (randint(100) < 11)
						{
							/* 1% chance for perm. damage */
							bool perm = (randint(10) == 1);
							if (dec_stat(A_CON, randint(10), perm))
								obvious = TRUE;
						}

						break;
					}
					case RBE_TIME:
					{
						switch (randint(10))
						{
							case 1:
							case 2:
							case 3:
							case 4:
							case 5:
							{
								msg_print
									("You feel life has clocked back.");
								lose_exp(100 +
									(p_ptr->exp / 100) * MON_DRAIN_LIFE);
								break;
							}

							case 6:
							case 7:
							case 8:
							case 9:
							{
								int stat = rand_int(6);

								switch (stat)
								{
									case A_STR:
										act = "strong";
										break;
									case A_INT:
										act = "bright";
										break;
									case A_WIS:
										act = "wise";
										break;
									case A_DEX:
										act = "agile";
										break;
									case A_CON:
										act = "hale";
										break;
									case A_CHR:
										act = "beautiful";
										break;
								}

								msg_format
									("You're not as %s as you used to be...",
									act);

								p_ptr->stat_cur[stat] =
									(p_ptr->stat_cur[stat] * 3) / 4;
								if (p_ptr->stat_cur[stat] < 3)
									p_ptr->stat_cur[stat] = 3;
								p_ptr->update |= (PU_BONUS);
								break;
							}

							case 10:
							{
								msg_print
									("You're not as powerful as you used to be...");

								for (k = 0; k < 6; k++)
								{
									p_ptr->stat_cur[k] =
										(p_ptr->stat_cur[k] * 3) / 4;
									if (p_ptr->stat_cur[k] < 3)
										p_ptr->stat_cur[k] = 3;
								}
								p_ptr->update |= (PU_BONUS);
								break;
							}
						}
						take_hit(damage, ddesc);

						break;
					}
					case RBE_EXP_VAMP:
					{
						/* Obvious */
						obvious = TRUE;

						/* Take damage */
						take_hit(damage, ddesc);

						if (p_ptr->hold_life && (rand_int(100) < 50))
						{
							msg_print("You keep hold of your life force!");
							resist_drain = TRUE;
						}
						else
						{
							s32b d =
								damroll(60,
								6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;
							if (p_ptr->hold_life)
							{
								msg_print
									("You feel your life slipping away!");
								lose_exp(d / 10);
							}
							else
							{
								msg_print
									("You feel your life draining away!");
								lose_exp(d);
							}
						}

						/* Heal the attacker? */
						if (!(p_ptr->prace == RACE_ZOMBIE ||
								p_ptr->prace == RACE_VAMPIRE ||
								p_ptr->prace == RACE_SPECTRE ||
								p_ptr->prace == RACE_SKELETON ||
								p_ptr->prace == RACE_GOLEM) && (damage > 2)
							&& !(resist_drain))
						{
							bool did_heal = FALSE;

							if (m_ptr->hp < m_ptr->maxhp)
								did_heal = TRUE;

							/* Heal */
							m_ptr->hp += damroll(4, damage / 6);
							if (m_ptr->hp > m_ptr->maxhp)
								m_ptr->hp = m_ptr->maxhp;

							/* Redraw (later) if needed */
							if (p_ptr->health_who == m_idx)
								p_ptr->redraw |= (PR_HEALTH);

							/* Special message */
							if ((m_ptr->ml) && (did_heal))
							{
								msg_format("%^s appears healthier.",
									m_name);
							}
						}
					}
				}
			}

			/* Hack -- only one of cut or stun */
			if (do_cut && do_stun)
			{
				/* Cancel cut */
				if (rand_int(100) < 50)
				{
					do_cut = 0;
				}

				/* Cancel stun */
				else
				{
					do_stun = 0;
				}
			}

			/* Handle cut */
			if (do_cut)
			{
				int k = 0;

				/* Critical hit (zero if non-critical) */
				tmp = monster_critical(d_dice, d_side, damage);

				/* Roll for damage */
				switch (tmp)
				{
					case 0:
						k = 0;
						break;
					case 1:
						k = randint(5);
						break;
					case 2:
						k = randint(5) + 5;
						break;
					case 3:
						k = randint(20) + 20;
						break;
					case 4:
						k = randint(50) + 50;
						break;
					case 5:
						k = randint(100) + 100;
						break;
					case 6:
						k = 300;
						break;
					default:
						k = 500;
						break;
				}

				/* Apply the cut */
				if (k)
					(void) set_cut(p_ptr->cut + k);
			}

			/* Handle stun */
			if (do_stun)
			{
				int k = 0;

				/* Critical hit (zero if non-critical) */
				tmp = monster_critical(d_dice, d_side, damage);

				/* Roll for damage */
				switch (tmp)
				{
					case 0:
						k = 0;
						break;
					case 1:
						k = randint(5);
						break;
					case 2:
						k = randint(10) + 10;
						break;
					case 3:
						k = randint(20) + 20;
						break;
					case 4:
						k = randint(30) + 30;
						break;
					case 5:
						k = randint(40) + 40;
						break;
					case 6:
						k = 100;
						break;
					default:
						k = 200;
						break;
				}

				/* Apply the stun */
				if (k)
					(void) set_stun(p_ptr->stun + k);
			}

			if (explode)
			{
				if (mon_take_hit(m_idx, m_ptr->hp + 1, &fear, NULL))
				{
					blinked = FALSE;
					alive = FALSE;
				}
			}

			if (touched)
			{
				if (p_ptr->sh_fire && alive)
				{
					if (!(r_ptr->flags3 & RF3_IM_FIRE))
					{
						int dam = damroll(2, 6);

						/* Modify the damage */
						dam = mon_damage_mod(m_ptr, dam, 0);

						msg_format("%^s is suddenly very hot!", m_name);

						if (mon_take_hit(m_idx, dam, &fear,
								" turns into a pile of ash."))
						{
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						if (m_ptr->ml)
							r_ptr->r_flags3 |= RF3_IM_FIRE;
					}
				}

				if (p_ptr->sh_elec && alive)
				{
					if (!(r_ptr->flags3 & RF3_IM_ELEC))
					{
						int dam = damroll(2, 6);

						/* Modify the damage */
						dam = mon_damage_mod(m_ptr, dam, 0);

						msg_format("%^s gets zapped!", m_name);

						if (mon_take_hit(m_idx, dam, &fear,
								" turns into a pile of cinder."))
						{
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						if (m_ptr->ml)
							r_ptr->r_flags3 |= RF3_IM_ELEC;
					}
				}
				touched = FALSE;
			}
		}

		/* Monster missed player */
		else
		{
			/* Analyze failed attacks */
			switch (method)
			{
				case RBM_HIT:
				case RBM_TOUCH:
				case RBM_PUNCH:
				case RBM_KICK:
				case RBM_CLAW:
				case RBM_BITE:
				case RBM_STING:
				case RBM_XXX1:
				case RBM_BUTT:
				case RBM_CRUSH:
				case RBM_ENGULF:
				case RBM_CHARGE:

					/* Visible monsters */
					if (m_ptr->ml)
					{
						/* Disturbing */
						disturb(1, 0);

						/* Message */
						msg_format("%^s misses you.", m_name);
					}

					break;
			}
		}


		/* Analyze "visible" monsters only */
		if (visible)
		{
			/* Count "obvious" attacks (and ones that cause damage) */
			if (obvious || damage || (r_ptr->r_blows[ap_cnt] > 10))
			{
				/* Count attacks of this type */
				if (r_ptr->r_blows[ap_cnt] < MAX_UCHAR)
				{
					r_ptr->r_blows[ap_cnt]++;
				}
			}
		}
	}


	/* Blink away */
	if (blinked)
	{
		msg_print("The thief flees laughing!");
		teleport_away(m_idx, MAX_SIGHT * 2 + 5);
	}


	/* Always notice cause of death */
	if (death && (r_ptr->r_deaths < MAX_SHORT))
	{
		r_ptr->r_deaths++;
	}

	if (m_ptr->ml && fear)
	{
		msg_format("%^s flees in terror!", m_name);
	}

	/* Assume we attacked */
	return (TRUE);
}
