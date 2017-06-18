| 07/02/2013 devestator - Changed item casting to use right click instead of MQ2Cast plugin
| 04/26/2011 Modified by devestator - Added ability to automatically convert spells to Rk. II or Rk. III
| 07/11/2011 Changed name to devMQ2Cast_Spell_Routines.inc to avoid conflics with mmoloader updates
| 12/28/2011 v1.03 Added a check to prevent interruptFlag errors if the interrupt routine is called for a spell
|   not cast through MQ2Cast.
| 
|**************************************************************************************************
| MQ2Cast_Spell_Routines.inc
|
| Written by A_Enchanter_00
| Last Modified 20070108
| Last Modification by Kroak.
|
| Features:
| This MQ2Cast_Spell_Routines.inc is much like The normal Spell_Routines.inc by Rusty~ this macro 
| use the MQ2Cast Plugin by s0rcier
|
| This macro also require MQ2ExchangeItem
|
| Note: Easy to Replace with Normal because Syntas is the "same".
|
| - Casts spells, clicks items, or uses AA abilities for you 
| - Allows back to back casting without waiting on spell gems to pop all the way up 
| - Will interrupt spell if target dies while casting. If on a mount, it will dismount and duck.
|    ** IMPORTANT: if you don't want to interrupt a spell while mounted, put this at the top of your macro: ** 
|    **   /declare noInterrupt bool outer TRUE                                                              ** 
| - Allows you to use items in bags. Equips item, clicks it, then returns it to its previous location
|   NOTE: There is a known isue with the MQ2Cast Plugin not always geting swarped back corectly...
| - Lets you set how long you want to keep trying to cast the spell (defaults to 0) 
| - Lets you call a custom subroutine while waiting for spell to finish casting 
|   Try to keep custom subroutines very small. A common use would be to interrupt the spell if a certain condition is true 
| - This file also includes a sub named Interrupt. You can call this to interrupt any spell you're casting instantly. 
| - Note: if you don't want this to cast spells while you're invis, in your main macro have this at the top: 
|      /declare noInvis bool outer TRUE
| - Note: if you don't want to stand up when Feig do the following:
|      /declare noFeigning bool outer TRUE
|   This will make it return CAST_CANCELLED if you're invis asking s0rcier to make it CAST_INVISIBLE insted.
| - Added: defaultGem so that spells can be casted using only /call MQ2Cast "Howl of Tashan"
|     /declare defaultGem string outer gem1
|	Note: If spell is not memed and and NOT seting /call MQ2Cast "Howl of Tashan" gem#
|   Set /declare defaultGem string outer gem# to change default gem#
| - Added: "-targetid|#####" it will find target and cast a MQ2Cast plugin feature.
| - Added: /call /call Interrupt - for use in mySub ect..
| - Added: ${Cast.Timing} - for use in mySub Example below
| - Added: -kill - will keep casting spell untill target is dead
| - Added: -recast|# - recast spells # number of times
| - Added: -maxtries|# - retry casting spell # number of times
| - Changed: -bandolier|setname OR -setin|setname swarp items using MQ2Bandolier
| - Added: -invis for single spell invis checks.
|**************************************************************************************************
|  Cast: the main subroutine that casts spells or items for you 
|     Usage: 
|        /call MQ2Cast ["spell name"|"item name"|"AA name"|"AA#"] [[item|slotname]|alt|gem#] [give up time][s|m] [custom subroutine name] [-targetid|###] [-maxtries|#] [-recast|#] [-setin|setname] [-bandolier|setname]
|     Examples: 
| 
|     To cast Howl of Tashan and mem it in slot 3 if not memmed:
|       /call MQ2Cast "Howl of Tashan" gem3
| 
|     To cast Arcane Rune and keep trying for 7 seconds, in case of interrupts.
|       /call MQ2Cast "Arcane Rune" gem5 7s
|
|     To click Grim Aura earring that's in a bag:
|       /call MQ2Cast "Shrunken Goblin Skull Earring" item
|
|     To click Grim Aura earring that's in a bag:
|       /call MQ2Cast "Shrunken Goblin Skull Earring" slotname
|
|     To use AA ability Eldritch Rune:
|       /call MQ2Cast "Eldritch Rune" alt
|         or
|       /call MQ2Cast "173" alt
| 
|     To call a subroutine that interrupts CH if target gets healed before it lands:
|       /call MQ2Cast "Complete Healing" gem1 0 CheckHP
|     Then in your macro have somewhere:
|       Sub CheckHP
|          /if ( ${Target.PctHPs}>=80 ) /call Interrupt
|       /return
|
|		3000 = 3 sec
|		Sub CheckHP
|		   /if ( ${Me.Casting.Name.Equal[Complete Healing]} && ${Target.PctHPs} < 40 && ${Cast.Timing} > 3000 ) ) /call Interrupt
|		/return
| 
| Returns these values: ${Macro.Return} and ${CastResult} and ${castReturn}
|----------------------+----------------------------------------------------------------------+ 
| CAST_CANCELLED       | Spell was cancelled by ducking (either manually or because mob died) | 
| CAST_CANNOTSEE       | You can't see your target                                            | 
| CAST_IMMUNE          | Target is immune to this spell                                       | 
| CAST_INTERRUPTED     | Casting was interrupted and exceeded the given time limit            | 
| CAST_INVIS           | You were invis, and noInvis is set to true                           | 
| CAST_NOTARGET        | You don't have a target selected for this spell                      | 
| CAST_NOTMEMMED       | Spell is not memmed and you gem to mem was not specified             | 
| CAST_NOTREADY        | AA ability or spell is not ready yet                                 | 
| CAST_OUTOFMANA       | You don't have enough mana for this spell!                           | 
| CAST_OUTOFRANGE      | Target is out of range                                               | 
| CAST_RESIST          | Your spell was resisted!                                             | 
| CAST_SUCCESS         | Your spell was cast successfully! (yay)                              | 
| CAST_UNKNOWN         | Spell/Item/Ability was not found                                     | 
| CAST_COLLAPSE        | Gate Colapsed                                                        | 
| CAST_TAKEHOLD        | Spell not hold                                                       | 
| CAST_FIZZLE          | Spell Fizzle                                                         | 
| CAST_INVISIBLE       | NOT Casting Invis                 									  |
| CAST_RECOVER		   | Spell not Recovered yet!											  |
| CAST_STUNNED		   | Stunned 															  |
| CAST_STANDING		   | Not Standing 														  |
| CAST_DISTRACTED      | To Distracted ( spell book open )                                    |
|----------------------+----------------------------------------------------------------------+
**************************************************************************************************|

Sub MQ2CastInit
	/declare MQ2CastSpellRoutinesVer			float outer 1.03
/return

Sub MQ2Cast(string spellName,string spellType,timer giveUpTimer,string mySub,string MQ2Feature1,string MQ2Feature2,string MQ2Feature3,string MQ2Feature4,string MQ2Feature5,string MQ2Feature6,string MQ2Feature7,string MQ2Feature8)
	/if ( !${Plugin[MQ2Cast].Name.Length} ) {
		/squelch /plugin MQ2Cast noauto
		/if ( !${Plugin[MQ2Cast].Name.Length} ) {
			/echo You must have MQ2Cast plugin to use this macro!!
			/endmacro
		}
	}
	/if ( !${Plugin[MQ2Exchange].Name.Length} ) {
		/squelch /plugin MQ2Exchange noauto
		/if ( !${Plugin[MQ2Exchange].Name.Length} ) {
			/echo You must have MQ2Exchange plugin to use this macro!!
			/endmacro
		}
	}

	/if ( !${Defined[castReturn]} ) /declare castReturn string outer NULL
	/if ( !${Defined[defaultGem]} ) /declare defaultGem string outer gem1
	/if ( !${Defined[spellType]} ) /declare spellType string local NULL
	/if ( !${Defined[giveUpTimer]} ) /declare giveUpTimer timer local 0
	/if ( !${Defined[MQ2Feature1]} ) /declare MQ2Feature1 string local
	/if ( !${Defined[MQ2Feature2]} ) /declare MQ2Feature2 string local
	/if ( !${Defined[MQ2Feature3]} ) /declare MQ2Feature3 string local
	/if ( !${Defined[MQ2Feature4]} ) /declare MQ2Feature4 string local
	/if ( !${Defined[MQ2Feature5]} ) /declare MQ2Feature5 string local
	/if ( !${Defined[MQ2Feature6]} ) /declare MQ2Feature6 string local
	/if ( !${Defined[MQ2Feature7]} ) /declare MQ2Feature7 string local
	/if ( !${Defined[MQ2Feature8]} ) /declare MQ2Feature8 string local
	
	/if ( !${Defined[interruptFlag]} ) {
		/declare interruptFlag bool outer FALSE	
	} else {
		/varset interruptFlag FALSE
	}

	/if ( ${mySub.Find[-targetid|]} || ${mySub.Find[-maxtries|]} || ${mySub.Find[-kill]} || ${mySub.Find[-recast|]} || ${mySub.Find[-setin]} || ${mySub.Find[-setout]} || ${mySub.Find[-bandolier|]} || ${mySub.Find[-invis]}) {
		/varset MQ2Feature8 ${MQ2Feature7}
		/varset MQ2Feature7 ${MQ2Feature6}
		/varset MQ2Feature6 ${MQ2Feature5}
		/varset MQ2Feature5 ${MQ2Feature4}
		/varset MQ2Feature4 ${MQ2Feature3}
		/varset MQ2Feature3 ${MQ2Feature2}
		/varset MQ2Feature2 ${MQ2Feature1}
		/varset MQ2Feature1 ${mySub}
		/varset mySub
	}
	
:wait_for_stop
	/if ( ${Me.Casting.ID} ) {
		/goto :wait_for_stop
	} else /if ( ${Corpse.Open} ) {
		/notify LootWnd DoneButton leftmouseup
		/goto :wait_for_stop
	}
	/if ( !${noFeigning} && ${Me.Feigning} ) /stand
		
	/if ( ${Window[SpellBookWnd].Open} ) /keypress spellbook
	/declare SaveItem string local NULL
	/declare SaveItemSlot string local NULL

	/if ( ${spellName.Find[|]} ) {
		/varset spellType ${spellName.Right[${spellName.Length:Dec[]}]}
		/varset spellName ${spellName.Left[${spellName.Find[|]:Dec}]}
	}

:cast_spell
	/if ( ${FindItem[${spellName}].ID} ) {
		/if ( ${Cursor.ID} && ${Me.FreeBuffSlots} ) /autoinventory
		/if ( ${FindItem[${spellName}].InvSlot.ID} && ${FindItem[${spellName}].InvSlot.ID} > 21 ) {			
			/if ( ${FindItem[${spellName}].InvSlot.ID} > 30 && ${FindItem[${spellName}].EffectType.Equal[Click Inventory]} || ${FindItem[${spellName}].EffectType.Equal[Click Unknown]} ) {
				/if ( !${InvSlot[pack8].Item.Container} && ${InvSlot[pack8].Item.ID} != ${FindItem[${spellName}].ID} ) /varset spellType pack8
			}
		}
	} else /if ( ${Me.AltAbility[${spellName}].ID} ) {
		/varset spellType alt
	} else /if ( ${Spell[${spellName}].ID} ) {
		/if ( !${spellType.Find[gem]} ) /varset spellType ${defaultGem}
	}
	/if (${spellType.Find[Gem]} && !${Me.Book[${spellName}]}) {
		/if (${Me.Book[${spellName} Rk. II]}) /varset spellName ${spellName} Rk. II
		/if (${Me.Book[${spellName} Rk II]}) /varset spellName ${spellName} Rk II
		/if (${Me.Book[${spellName} Rk. III]}) /varset spellName ${spellName} Rk. III
		/if (${Me.Book[${spellName} Rk III]}) /varset spellName ${spellName} Rk III
	}
	| /if (${spellType.NotEqual[item]}) {
		/if ( ${noInvis} ) {
			/squelch /casting "${spellName}" ${spellType} "-invis" "${MQ2Feature1}" "${MQ2Feature2}" "${MQ2Feature3}" "${MQ2Feature4}" "${MQ2Feature5}" "${MQ2Feature6}" "${MQ2Feature7}" "${MQ2Feature8}"
		} else {
			/squelch /casting "${spellName}" ${spellType} "${MQ2Feature1}" "${MQ2Feature2}" "${MQ2Feature3}" "${MQ2Feature4}" "${MQ2Feature5}" "${MQ2Feature6}" "${MQ2Feature7}" "${MQ2Feature8}"
		}
	|**} else {
		/if (!${noInvis} || !${Me.Invis}) {
			/squelch /itemnotify ${FindItem[${spellName}].InvSlot} rightmouseup
			/delay 5 ${Me.Casting.ID}
			:item_cast_pending
				/if ( !${interruptFlag} && ${mySub.Length} ) /call ${mySub}
			/if (${Me.Casting.ID}) /goto :item_cast_pending
			/varset castReturn CAST_SUCCESS
			/return ${castReturn}
		}
	}**|
:cast_spell_pending
  /if (${Cast.Result.Equal[CAST_NOTARGET]}) /goto :CastEnd
	/if ( ${Cast.Status.Find[C]} ) {
		/if ( !${interruptFlag} && ${mySub.Length} ) /call ${mySub}
		/goto :cast_spell_pending
	} else /if ( ${Cast.Result.Equal[CAST_RESIST]} ) {
		/if ( ( ${FindItem[${spellName}].ID} && ${giveUpTimer} && !${FindItem[${spellName}].Timer} ) || ( ${Spell[${spellName}].ID} && ${giveUpTimer} > ${Spell[${spellName}].RecoveryTime} ) ) /goto :cast_spell
	} else /if ( ${Select[${Cast.Result},CAST_FIZZLE,CAST_STUNNED,CAST_INTERRUPTED]} ) {
		/if ( !${interruptFlag} && ( ${FindItem[${spellName}].ID} && ${giveUpTimer} && !${FindItem[${spellName}].Timer} ) || ( ${Me.AltAbility[${spellName}].ID} && ${Me.AltAbilityReady[${spellName}]} ) || ( ${Spell[${spellName}].ID} && ${giveUpTimer} > ${Spell[${spellName}].RecoveryTime} ) ) /goto :cast_spell
	}
	:CastEnd
	/varset castReturn ${Cast.Result}
/return ${CastResult}

Sub Interrupt
	/if (${Defined[interruptFlag]}) /varset interruptFlag TRUE
	/interrupt
/return
