/*	SCCS Id: @(#)write.c	3.4	2001/11/29	*/
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

STATIC_DCL int FDECL(cost,(struct obj *));

/*
 * returns basecost of a scroll or a spellbook
 */
STATIC_OVL int
cost(otmp)
register struct obj *otmp;
{

	if (otmp->oclass == SPBOOK_CLASS)
		return(10 * objects[otmp->otyp].oc_level);

	/* KMH, balance patch -- restoration of marker charges */
	switch (otmp->otyp) {
# ifdef MAIL
	case SCR_MAIL:
		return(2);
/*		break; */
# endif
	case SCR_STANDARD_ID:
	case SCR_WOUNDS:
	case SCR_RUMOR:
	case SCR_MESSAGE:
		return(5);
	case SCR_HEALING:
	case SCR_LIGHT:
	case SCR_GOLD_DETECTION:
	case SCR_FOOD_DETECTION:
	case SCR_TRAP_DETECTION:
	case SCR_MAGIC_MAPPING:
	case SCR_AMNESIA:
	case SCR_INSTANT_AMNESIA:
	case SCR_FIRE:
	case SCR_SLEEP:
	case SCR_EARTH:
	case SCR_CURE_BLINDNESS:
	case SCR_ROOT_PASSWORD_DETECTION:
		return(8);
/*		break; */
	case SCR_MANA:
	case SCR_DESTROY_ARMOR:
	case SCR_DESTROY_WEAPON:
	case SCR_BAD_EFFECT:
	case SCR_CREATE_MONSTER:
	case SCR_CREATE_VICTIM:
	case SCR_SUMMON_UNDEAD:
	case SCR_PUNISHMENT:
	case SCR_NASTINESS:
	case SCR_SYMMETRY:
	case SCR_CREATE_CREATE_SCROLL:
	case SCR_PROOF_ARMOR:
	case SCR_PROOF_WEAPON:
		return(10);
/*		break; */
	case SCR_CONFUSE_MONSTER:
	case SCR_PHASE_DOOR:
		return(12);
/*		break; */
	case SCR_IDENTIFY:
	case SCR_STONING:
	case SCR_BULLSHIT:
	case SCR_REVERSE_IDENTIFY:
	case SCR_SCARE_MONSTER:
		return(14);
/*		break; */
	case SCR_TAMING:
	case SCR_TELEPORTATION:
	case SCR_FLOOD:
	case SCR_LAVA:
	case SCR_BARRHING:
	case SCR_GROWTH:
	case SCR_ICE:
	case SCR_ENRAGE:
	case SCR_FROST:
	case SCR_CLOUDS:
	case SCR_DETECT_WATER:
	case SCR_CHAOS_TERRAIN:
	case SCR_TELE_LEVEL:
	case SCR_WARPING:
	case SCR_IMMOBILITY:
	case SCR_MASS_MURDER:
	case SCR_TRAP_CREATION:
	case SCR_CREATE_TRAP:
	case SCR_GROUP_SUMMONING:
	case SCR_UNDO_GENOCIDE:
	case SCR_RANDOM_ENCHANTMENT:
		return(20);
/*		break; */
	/* KMH, balance patch -- more useful scrolls cost more */
	case SCR_STINKING_CLOUD:
	case SCR_ENCHANT_ARMOR:
	case SCR_REMOVE_CURSE:
	case SCR_ENCHANT_WEAPON:
	case SCR_CHARGING:
	case SCR_GIRLINESS:
	case SCR_FLOODING:
	case SCR_EGOISM:
	case SCR_ERASURE:
	case SCR_ANTIMATTER:
	case SCR_MEGALOAD:
	case SCR_GEOLYSIS:
	case SCR_SECURE_IDENTIFY:
	case SCR_REPAIR_ITEM:
		return(24);
/*		break; */
	case SCR_RESISTANCE:
	case SCR_GENOCIDE:
	case SCR_CURE:
	case SCR_SIN:
	case SCR_ARMOR_SPECIALIZATION:
	case SCR_SUMMON_BOSS:
	case SCR_SUMMON_ELM:
	case SCR_DEMONOLOGY:
	case SCR_ELEMENTALISM:
	case SCR_TRAP_DISARMING:
	case SCR_FLOOD_TIDE:
	case SCR_EBB_TIDE:
	case SCR_CREATE_FACILITY:
	case SCR_SUMMON_GHOST:
		return(30);
/*		break; */
	case SCR_GAIN_MANA:
	case SCR_LOCKOUT:
	case SCR_WARD:
	case SCR_CREATE_ALTAR:
	case SCR_WARDING:
	case SCR_RELOCATION:
	case SCR_VILENESS:
	case SCR_CREATE_FAMILIAR:
	case SCR_ITEM_GENOCIDE:
	case SCR_POWER_HEALING:
		return(40);
/*		break; */
	case SCR_CONSECRATION:
	case SCR_BOSS_COMPANION:
	case SCR_ANTIMAGIC:
	case SCR_INVENTORY_ID:
	case SCR_SKILL_UP:
	case SCR_ALTER_REALITY:
	case SCR_WORLD_FALL:
		return(50);
/*		break; */
	case SCR_BLANK_PAPER:
	case SCR_COPYING:
	case SCR_WISHING:
	case SCR_ARTIFACT_CREATION:
	case SCR_ARTIFACT_JACKPOT:
	case SCR_RESURRECTION:
	case SCR_ACQUIREMENT:
	case SCR_ENTHRONIZATION:
	case SCR_FOUNTAIN_BUILDING:
	case SCR_SINKING:
	case SCR_CREATE_SINK:
	case SCR_WC:
	default:
		/*impossible*/pline("You can't write such a weird scroll!");
	}
	return(1000);
}

static NEARDATA const char write_on[] = { SCROLL_CLASS, SPBOOK_CLASS, 0 };

int
dowrite(pen)
register struct obj *pen;
{
	register struct obj *paper;
	char namebuf[BUFSZ], *nm, *bp;
	register struct obj *new_obj;
	int basecost, actualcost;
	int curseval;
	char qbuf[QBUFSZ];
	int first, last, i;
	boolean by_descr = FALSE;
	const char *typeword;

	if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) ) {
	    You("need hands to be able to write!");
	    return 0;
	} else if (IsGlib) {
	    pline("%s from your %s.",
		  Tobjnam(pen, "slip"), makeplural(body_part(FINGER)));
	    dropx(pen);
	    return 1;
	}

	/* get paper to write on */
	paper = getobj(write_on,"write on");
	if(!paper)
		return(0);
	typeword = (paper->oclass == SPBOOK_CLASS) ? "spellbook" : "scroll";
	if(Blind && !paper->dknown) {
		You("don't know if that %s is blank or not!", typeword);
		return(1);
	}
	paper->dknown = 1;
	if(paper->otyp != SCR_BLANK_PAPER && paper->otyp != SPE_BLANK_PAPER) {
		pline("That %s is not blank!", typeword);
		exercise(A_WIS, FALSE);
		return(1);
	}

	/* what to write */
	Sprintf(qbuf, "What type of %s do you want to write?", typeword);
	getlin(qbuf, namebuf);
	(void)mungspaces(namebuf);	/* remove any excess whitespace */
	if(namebuf[0] == '\033' || !namebuf[0])
		return(1);
	nm = namebuf;
	if (!strncmpi(nm, "scroll ", 7)) nm += 7;
	else if (!strncmpi(nm, "spellbook ", 10)) nm += 10;
	if (!strncmpi(nm, "of ", 3)) nm += 3;

	if ((bp = strstri(nm, " armour")) != 0) {
		(void)strncpy(bp, " armor ", 7);	/* won't add '\0' */
		(void)mungspaces(bp + 1);	/* remove the extra space */
	}

	first = bases[(int)paper->oclass];
	last = bases[(int)paper->oclass + 1] - 1;
	for (i = first; i <= last; i++) {
		/* extra shufflable descr not representing a real object */
		if (!OBJ_NAME(objects[i])) continue;

		if (!strcmpi(OBJ_NAME(objects[i]), nm))
			goto found;
		if (!strcmpi(OBJ_DESCR(objects[i]), nm)) {
			by_descr = TRUE;
			goto found;
		}
	}

	There("is no such %s!", typeword);
	return 1;
found:

	if (i == SCR_BLANK_PAPER || i == SPE_BLANK_PAPER) {
		You_cant("write that!");
		pline("It's obscene!");
		return 1;
	} else if (i == SPE_BOOK_OF_THE_DEAD) {
		pline("No mere dungeon adventurer could write that.");
		return 1;
	} else if (i == SCR_COPYING) {
		You("don't know how to break copy protect.");
		if(Hallucination) 
			pline("(I know it, but not tell to you.)");
		return 1;
	} else if (i == SCR_WISHING || i == SCR_ARTIFACT_CREATION || i == SCR_ARTIFACT_JACKPOT || i == SCR_RESURRECTION || i == SCR_ACQUIREMENT || i == SCR_ENTHRONIZATION || i == SCR_FOUNTAIN_BUILDING || i == SCR_SINKING || i == SCR_CREATE_SINK || i == SCR_WC) {
		pline("This scroll refuses to be written.");
		return 1;
	} else if (by_descr && paper->oclass == SPBOOK_CLASS &&
		    !objects[i].oc_name_known) {
		/* can't write unknown spellbooks by description */
		pline(
		  "Unfortunately you don't have enough information to go on.");
		return 1;
	}

	/* KMH, conduct */
	u.uconduct.literate++;

	new_obj = mksobj(i, FALSE, FALSE);
	if (!new_obj) {
		pline("Scroll creation failed!");
		return(1);
	}
	new_obj->bknown = (paper->bknown && pen->bknown);
	new_obj->oinvis = paper->oinvis;
	new_obj->oinvisreal = paper->oinvisreal;

	/* shk imposes a flat rate per use, not based on actual charges used */
	check_unpaid(pen);

	/* see if there's enough ink */
	basecost = cost(new_obj);
	if(pen->spe < basecost/2)  {
		Your("marker is too dry to write that!");
		obfree(new_obj, (struct obj *) 0);
		return(1);
	}

	/* we're really going to write now, so calculate cost
	 */
	actualcost = rn1(basecost/2,basecost/2);

	if (!(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_DEVICES)) {
			default: break;
			case P_BASIC: actualcost = (actualcost * 14 / 15); break;
			case P_SKILLED: actualcost = (actualcost * 13 / 15); break;
			case P_EXPERT: actualcost = (actualcost * 12 / 15); break;
			case P_MASTER: actualcost = (actualcost * 11 / 15); break;
			case P_GRAND_MASTER: actualcost = (actualcost * 10 / 15); break;
			case P_SUPREME_MASTER: actualcost = (actualcost * 9 / 15); break;
		}
	}

	curseval = bcsign(pen) + bcsign(paper);
	exercise(A_WIS, TRUE);
	/* dry out marker */
	if (pen->spe < actualcost) {
		pen->spe -= rnd(pen->spe);
		if (issoviet) pen->spe = 0;
		if (!pen->spe && !issoviet) Your("marker dries out!");
		if (issoviet) pline("Medved' khar khar, Sovet reshil, chto markery stanovyatsya pustymi, eto zdorovo. Poproshchaytes' s nim!");
		else pline("Unfortunately, after writing for a bit you notice that there's not enough ink left. You stop writing to salvage at least some of the precious ink.");
		/* scrolls disappear, spellbooks don't */
		if (paper->oclass == SPBOOK_CLASS) {
			pline_The(
		       "spellbook is left unfinished and your writing fades.");
			update_inventory();	/* pen charges */
		} else {
			pline_The("scroll is now useless and disappears!");
			useup(paper);
		}
		obfree(new_obj, (struct obj *) 0);
		return(1);
	}
	pen->spe -= actualcost;

	/* can't write if we don't know it - unless we're lucky */
	if(!(objects[new_obj->otyp].oc_name_known) &&
	   !(objects[new_obj->otyp].oc_uname) &&
	   (rnl(Role_if(PM_WIZARD) ? 3 : Role_if(PM_SAGE) ? 2 : 15))) {
		You("%s to write that!", by_descr ? "fail" : "don't know how");
		/* scrolls disappear, spellbooks don't */
		if (paper->oclass == SPBOOK_CLASS) {
			You(
       "write in your best handwriting:  \"My Diary\", but it quickly fades.");
			update_inventory();	/* pen charges */
		} else {
			if (by_descr) {
			    Strcpy(namebuf, OBJ_DESCR(objects[new_obj->otyp]));
			    wipeout_text(namebuf, (6+MAXULEV - u.ulevel)/6, 0);
			} else
			    Sprintf(namebuf, "%s was here!", plname);
			You("write \"%s\" and the scroll disappears.", namebuf);
			useup(paper);
		}
		obfree(new_obj, (struct obj *) 0);
		return(1);
	}

	/* useup old scroll / spellbook */
	useup(paper);
	use_skill(P_DEVICES,10);
	if (Race_if(PM_FAWN)) {
		use_skill(P_DEVICES,10);
	}
	if (Race_if(PM_SATRE)) {
		use_skill(P_DEVICES,10);
		use_skill(P_DEVICES,10);
	}

	/* success */
	if (new_obj->oclass == SPBOOK_CLASS) {
		/* acknowledge the change in the object's description... */
		pline_The("spellbook warps strangely, then turns %s.",
		      OBJ_DESCR(objects[new_obj->otyp]));
	}
	new_obj->blessed = (curseval > 0);
	new_obj->cursed = (curseval < 0);
	if (pen && pen->oartifact == ART_PEN_OF_RANDOMNESS) {
		new_obj->blessed = 0;
		new_obj->cursed = 0;
		if (!rn2(3)) new_obj->blessed = 1;
		else if (!rn2(2)) new_obj->cursed = 1;
	}
	new_obj->selfmade = TRUE;
#ifdef MAIL
	if (new_obj->otyp == SCR_MAIL) new_obj->spe = 1;
#endif
	new_obj = hold_another_object(new_obj, "Oops!  %s out of your grasp!",
					       The(aobjnam(new_obj, "slip")),
					       (const char *)0);
	return(1);
}

/*write.c*/
