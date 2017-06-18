| ========================================================================================================
| 							 devCast.inc v1.05
|     					Written By: Devestator
| 
| 
| Description:
|  Include file to handle casting without the use of MQ2Cast
| 
| v1.05 Update Notes
|  -Corrected white mq2chatwnd spam when casting an item
| 
| v1.04 Update Notes
|  -Updates for MQ2 patch changing spell timers
| 
| v1.03 Update Notes
|  -Changed return of a forced interrupt to CAST_CANCELLED instead of CAST_INTERRUPTED
| 
| v1.02 Update Notes
|  -Added .Upper to the castSlot Replace call to get around bug of non matching case returning random
|    values.
|  -Corrected a problem with finding a rk 2 or higher spell if the castSlot was passed in as NULL
| 
| ========================================================================================================

#event devCastResult "You are not sufficient level to use this item#*#"
#event devCastResult "Your spell is interrupted#*#"
#event devCastResult "Your gate is too unstable, and collapses#*#"
#event devCastResult "You *CANNOT* cast spells, you have been silenced#*#"
#event devCastResult "You can't cast spells while invulnerable#*#"
#event devCastResult "Your casting has been interrupted#*#"
#event devCastResult "This spell does not work here#*#"
#event devCastResult "You can't cast spells while stunned#*#"
#event devCastResult "Your spell did not take hold#*#"
#event devCastResult "You must be standing to cast a spell#*#"
#event devCastResult "You can only cast this spell in the outdoors#*#"
#event devCastResult "Your ability to use#*#has been disabled#*#do not have the membership#*#"
#event devCastResult "Your target resisted the#*#"
#event devCastResult "Your spell fizzles!"
#event devCastResult "Your spell was mostly successful"
#event devCastResult "Your spell was partially successful."
#event devCastResult "Your spell was slightly successful."
#event devCastResult "You are missing some required components."

#event devCastResult "You cannot see your target#*#"
#event devCastResult "Your target cannot be mesmerized#*#"
#event devCastResult "Your target looks unaffected#*#"
#event devCastResult "You miss a note bringing your song to a close#*#"
#event devCastResult "You must first select a target for this spell#*#"
#event devCastResult "This spell only works on#*#"
#event devCastResult "You must first target a group member#*#"
| #event devCastResult "Spell recast time not yet met#*#"
#event devCastResult "Insufficient Mana to cast this spell#*#"
#event devCastResult "Your target is out of range, get closer#*#"
| #event devCastResult "You haven't recovered yet#*#"
| #event devCastResult "Spell recovery time not yet met#*#"
#event devCastResult "Your song ends."

| Wrapper for MQ2Cast drop in replacement support
Sub MQ2CastInit
	/call devCastInit
/return

Sub devCastInit(bool standAloneRun)
	/if (!${Defined[standAloneRun]}) /declare standAloneRun bool local FALSE
	/declare devCastVersion								float outer 1.05
	/declare CastResult										string outer CAST_INIT
	/declare devCastCasting								bool outer FALSE
	/declare MQ2CastSpellRoutinesVer			float outer 1.03
	/declare devCastStandAlone						bool outer ${standAloneRun}
	
	/if (!${devCastStandAlone}) /call EchoLog "\awInitializing devCast.inc \agv${devCastVersion}" FALSE
	
	/if (!${Defined[defaultGem]}) /declare defaultGem			string outer GEM1
	/if (!${Defined[defaultMaxWaitTime]}) /declare defaultMaxWaitTime	string outer 50

	/if ( !${Defined[interruptFlag]} ) {
		/declare interruptFlag bool outer FALSE	
	} else {
		/varset interruptFlag FALSE
	}	
/return

| Wrapper function to make this include a drop in replacement to MQ2Cast
Sub MQ2Cast(string castName, string castSlot, timer castMaxTryTime, string castCallRoutine, string MQ2Feature1, string MQ2Feature2, string MQ2Feature3, string MQ2Feature4, string MQ2Feature5, string MQ2Feature6, string MQ2Feature7, string MQ2Feature8)
	/declare cResult	string local CAST_INCOMPLETE
	/declare f				int local 0
	/declare feature	string local NULL
	/declare data			string local NULL
	/declare rRetryCount 	int local 0
	/declare tryCount			int local 0
	
	/varset interruptFlag FALSE
	/for f 1 to 8
		/if (${MQ2Feature${f}.NotEqual[NULL]}) {
			/varset feature ${MQ2Feature${f}.Arg[1,|].Right[-1].Upper}
			/varset data ${MQ2Feature${f}.Arg[2,|].Upper}
			
			/if (${feature.Equal[TARGETID]}) {
				/if (${Target.ID} != ${data}) {
					/squelch /target ID ${Target.ID}
					/delay 2s ${Target.ID}==${data}
				}
			} else /if (${feature.Equal[MAXTRIES]}) {
				/varset rRetryCount ${data}
			}
		}
	/next f
	
	:resistRetryLoop
		/varcalc tryCount ${tryCount} + 1
		/call Cast "${castName}" ${castSlot} ${castMaxTryTime} ${castCallRoutine}
		/varset cResult ${Macro.Return}
	/if (${tryCount} < ${rRetryCount} && ${Select[${cResult},CAST_RESIST,CAST_INTERRUPTED,CAST_STUNNED,CAST_FIZZLE]} && !${interruptFlag}) /goto :resistRetryLoop
/return ${cResult}

Sub Cast(string castName, string castSlot, timer castMaxTryTime, string castCallRoutine)
	/declare baseName				string local NULL
	/declare baseRank				int local 1
	/declare myRank					string local NULL
	/declare mySpell				string local NULL
	/declare ranks					string local \\Rk. II\Rk. III\Rk II\Rk III
	/declare typeCheck			string local NULL
	/declare castInt				int local 0
	/declare gemNum					int local 0
	/declare castingResultTimeout		timer local 0s
	/declare castingStartResult			string local NULL
	
 | Add support for maxtries, targetid, etc?  If so, reset castMaxTryTime on retries?
 	/varset interruptFlag FALSE
 	
	/if (${noInvis} && ${Me.Invis}) {
		/varset CastResult CAST_INVIS
		/return CastResult
	}
	
	/varset CastResult CAST_INCOMPLETE
	/if (!${Defined[castName]} || ${castName.Equal[NULL]} || !${castName.Length}) {
		/varset CastResult CAST_NOTFOUND
		/return ${CastResult}
	}
	/call Cast_GetSpellName "${castName}"
	/varset baseName ${Macro.Return}
	/call Cast_GetSpellRank "${castName}"
	/varset baseRank ${Macro.Return}
	
	/if (!${Defined[castSlot]} || (!${Select[${castSlot},ALT,ITEM,ABILITY,DISC]} && !${castSlot.Find[GEM]})) {
		/if (!${Defined[castSlot]}) /declare castSlot	string local NULL
		/varset mySpell ${castName}
		/if (${Me.AltAbility[${castName}]}) {
			/varset castSlot ALT
		} else /if (${FindItem[${castName}].ID}) {
			/varset castSlot ITEM
		} else /if (${Me.Ability[${castName}]}) {
			/varset castSlot Ability
		} else {
			/varset typeCheck CombatAbility
			:nextType
			| /for castInt 1 to ${Math.Calc[${ranks.Count[|]}+1]}
			/if (${Me.${typeCheck}[${Spell[${castName}].RankName}]}) {
				/varset myRank ${ranks.Token[${castInt},|]}
				/if (${typeCheck.Equal[CombatAbility]}) /varset castSlot DISC
				/if (${typeCheck.Equal[Book]}) /varset castSlot ${defaultGem}
			}
			| /if (${castSlot.Equal[NULL]}) /next castInt
			/if (${castSlot.Equal[NULL]} && ${typeCheck.Equal[CombatAbility]}) {
				/varset typeCheck Book
				/goto :nextType
			}
			/varset mySpell ${Spell[${castName}].RankName}
		}
		/if (${castSlot.Equal[NULL]}) {
			/varset CastResult CAST_NOTFOUND
			/return ${CastResult}
		}
	} else {
		/varset typeCheck NULL
		/if (${castSlot.Equal[DISC]}) {
			/varset typeCheck CombatAbility
		} else /if (${castSlot.Find[GEM]}) {
			/varset typeCheck Book
		}
		/if (${typeCheck.NotEqual[NULL]}) {
			/varset myRank NULL
			/for castInt 1 to ${Math.Calc[${ranks.Count[\]}+1]}
				/if (${Me.${typeCheck}[${baseName} ${ranks.Token[${castInt},\]}]}) /varset myRank ${ranks.Token[${castInt},\]}
				| /echo >> ${baseName} ${ranks.Token[${castInt},\]} ${Me.${typeCheck}[${baseName} ${ranks.Token[${castInt},\]}]} ${castInt} ${ranks.Token[${castInt},\]}
			/if (${myRank.Equal[NULL]}) /next castInt
			/if (!${myRank.Length} || ${myRank.Equal[NULL]}) {
				/varset mySpell ${castName}
			} else {
				/varset mySpell ${baseName}${If[${myRank.Length} && ${myRank.NotEqual[NULL]}, ${myRank},]}
			}
			| /echo >> ${mySpell}
		} else {
			/varset mySpell ${castName}
		}
	}


	/if (!${Defined[castMaxTryTime]}) /declare castMaxTryTime	timer local ${defaultMaxWaitTime}
	/if (!${castMaxTryTime}) /varset castMaxTryTime ${defaultMaxWaitTime}
	
	/if (${castSlot.Find[GEM]}) {
		/if (!${Me.Book[${mySpell}]}) {
			/varset CastResult CAST_SPELL_MISSING
			/return ${CastResult}
		}
		/varset gemNum ${castSlot.Upper.Replace[GEM, ]}
		/if (!${Me.Gem[${mySpell}]}) /call MemSpell "${mySpell}" ${gemNum}
		/varset castMaxTryTime ${castMaxTryTime.OriginalValue}
		/if (${Me.Gem[${mySpell}]}) {
			/if (${Me.GemTimer[${mySpell}]} && ${Me.GemTimer[${mySpell}].Float} > ${Math.Calc[${castMaxTryTime}/10]}) {
				/varset CastResult CAST_NOTREADY
				/return ${CastResult}
			} else {
				| It's ok to cast
				:startGemCast
					| /echo Casting Gem ${Me.Gem[${mySpell}]}
					/cast ${Me.Gem[${mySpell}]}
					/if (${Spell[${mySpell}].CastTime}) /delay 5 ${Me.Casting.ID}
					/if (!${castMaxTryTime}) {
						/varset CastResult CAST_TIMEOUT
						/return CAST_TIMEOUT
					}
				/if (!${Me.Casting.ID} && ${Spell[${mySpell}].CastTime}) /goto :startGemCast
			}
		}
	} else /if (${castSlot.Equal[ABILITY]} || ${castSlot.Equal[DISC]} || ${castSlot.Equal[ALT]}) {
		/if ((${castSlot.Equal[Ability]}) || ((${Me.CombatAbilityReady[${mySpell}]} || ${Me.CombatAbilityTimer.Float} <= ${Math.Calc[${castMaxTryTime.OriginalValue}/10]}) && ${castSlot.Equal[DISC]}) || (${castSlot.Equal[ALT]} && (${Me.AltAbilityReady[${mySpell}]} || ${Me.AltAbilityTimer[${mySpell}].Float} <= ${Math.Calc[${castMaxTryTime.OriginalValue}/10]}))) {
			/varset castMaxTryTime ${castMaxTryTime.OriginalValue}
			/delay ${castMaxTryTime} ${Me.${If[${castSlot.Equal[DISC]},Combat,${If[${castSlot.Equal[ALT]},Alt,]}]}AbilityReady[${mySpell}]}
			/if (!${Me.${If[${castSlot.Equal[DISC]},Combat,${If[${castSlot.Equal[ALT]},Alt,]}]}AbilityReady[${mySpell}]}) {
				/varset CastResult CAST_TIMEOUT
				/return CAST_TIMEOUT
			}
			:startAbilityCast
				/if (${castSlot.Equal[ABILITY]} || ${castSlot.Equal[DISC]}) {
					/doability "${mySpell}"
				} else {
					/alt activate ${Me.AltAbility[${mySpell}].ID}
				}
				/if (${Spell[${mySpell}].CastTime}) /delay 5 ${Me.Casting.ID}
				/if (!${castMaxTryTime}) {
					/varset CastResult CAST_TIMEOUT
					/return CAST_TIMEOUT
				}
			/if (!${Me.Casting.ID} && ${Spell[${mySpell}].CastTime}) /goto :startAbilityCast
		} else {
			/return CAST_NOTREADY
		}
	} else /if (${castSlot.Equal[ITEM]}) {
		| Need to check if it needs to swap and then right click it.  If swapped swap back.  Check item timer for ready to cast
		/if (${FindItem[${mySpell}].ID} && (!${FindItem[${mySpell}].Timer} || ${FindItem[${mySpell}].Timer} < ${Math.Calc[${castMaxTryTime.OriginalValue}/10]})) {
			/varset castMaxTryTime ${castMaxTryTime.OriginalValue}
			/delay ${castMaxTryTime} !${FindItem[${mySpell}].Timer}
			:startItemCast
				| /if (${castMaxTryTime}) /itemnotify ${FindItem[${mySpell}].InvSlot} rightmouseup
				| are the quotes needed here?
				/if (${castMaxTryTime}) {
					/if (!${Defined[emuMode]} || !${emuMode} ) {
						/useitem "${mySpell}"
					} else {
						| /useitem ${FindItem[${mySpell}].InvSlot}
						/itemnotify ${FindItem[${mySpell}].InvSlot} rightmouseup
					}
				}
				/if (${FindItem[${mySpell}].CastTime}) {
					/if (${castMaxTryTime}) /delay 5 ${Me.Casting.ID}
					/if (!${castMaxTryTime}) {
						/varset CastResult CAST_TIMEOUT
						/return CAST_TIMEOUT
					}
				}
			/if (!${Me.Casting.ID} && ${FindItem[${mySpell}].CastTime} && (!${emuMode} || ${FindItem[${mySpell}].CastTime})) /goto :startItemCast
		} else {
			/return CAST_NOTREADY
		}
	}

	/if (!${Defined[castCallRoutine]}) /declare castCallRoutine		string local Cast_DefaultCall
	/if (!${castCallRoutine.Length} || ${castCallRoutine.Equal[NULL]}) /varset castCallRoutine Cast_DefaultCall
	
	/varset devCastCasting TRUE
	/varset castingStartResult ${CastResult}
	/if (${Me.Casting.ID}) {
		:castingLoop
			/call ${castCallRoutine}
			/doevents devCastResult
		/if (${Me.Casting.ID} && !${interruptFlag}) /goto :castingLoop
	}
	/varset castingResultTimeout 1s
	:castingResultLoop
		/doevents devCastResult
	/if (${CastResult.Equal[${castingStartResult}]} && ${castingResultTimeout}) /goto :castingResultLoop
	/varset devCastCasting FALSE
	/if (${CastResult.Equal[CAST_INCOMPLETE]}) /varset CastResult CAST_SUCCESS
	/if (${interruptFlag}) /varset CastResult CAST_CANCELLED
/return ${CastResult}

Sub Cast_DefaultCall
	
/return

Sub Cast_GetSpellName(string spellString)
	/declare paramCount													int local
	/varcalc paramCount ${spellString.Count[ ]} + 1
	/declare sLoop															int local 0
	/declare sName															string local
	/declare sRanks															string local [I]|[II]|[III]|[IV]|[V]|[VI]|[VII]|[VIII]|[IX]|[X]|[XI]|[XII]|[XIII]|[XIV]|[XV]|[XVI]|[XVII]|[XVIII]|[XIX]|[XX]|[XXI]|[XXII]|[XXIII]|[XXIV]|[XXV]|[XXVI]|[XXVII]|[XXVIII]|[XXIX]|XXX]
	/if (${paramCount}) {
		/for sLoop 1 to ${paramCount}
			/if (${spellString.Token[${sLoop}, ].NotEqual[Rk]} && ${spellString.Token[${sLoop}, ].NotEqual[Rk.]}) {
				/if (${sLoop} < ${paramCount} || (${sLoop}==${paramCount} && !${sRanks.Find[[${spellString.Token[${sLoop}, ]}]]})) {
					/varset sName ${If[${sName.Length},${sName} ,]}${spellString.Token[${sLoop}, ]}
				}
			} else {
				/return ${sName}
			} 
		/next sLoop
	} else {
		/varset sName ${spellString}
	}
/return ${sName}

Sub Cast_GetSpellRank(string spellString)
	/declare paramCount													int local
	/varcalc paramCount ${spellString.Count[ ]} + 1
	/declare sRanks															string local [I]|[II]|[III]|[IV]|[V]|[VI]|[VII]|[VIII]|[IX]|[X]|[XI]|[XII]|[XIII]|[XIV]|[XV]|[XVI]|[XVII]|[XVIII]|[XIX]|[XX]|[XXI]|[XXII]|[XXIII]|[XXIV]|[XXV]|[XXVI]|[XXVII]|[XXVIII]|[XXIX]|XXX]
	/declare sLoop															int local
	
	/if (${paramCount}) {
		/if (${sRanks.Find[[${spellString.Token[${paramCount}, ]}]]}) {
			/for sLoop 1 to ${Math.Calc[${sRanks.Count[|]} + 1]}
				/if (${sRanks.Token[${sLoop},|].Equal[[${spellString.Token[${paramCount}, ]}]]}) /return ${sLoop}
			/next sLoop
		} else {
			/return 1
		}
	} else {
		/return 1
	}
/return

Sub Event_devCastResult(string rLine)
	/if (!${devCastCasting}) /return
	
	/if (${interruptFlag}) {
		/varset CastResult CAST_CANCELLED
	} else /if (${rLine.Find[You are not sufficient level to use this Item]}) {
		/varset CastResult CAST_LOWLEVEL
	} else /if (${rLine.Find[Your spell is interrupted]}) {
		/varset CastResult CAST_INTERRUPTED
	} else /if (${rLine.Find[Your gate is too unstable, and collapses]}) {
		/varset CastResult CAST_COLLAPSE
	} else /if (${rLine.Find[You *CANNOT* cast spells, you have been silenced]}) {
		/varset CastResult CAST_SILENCED
	} else /if (${rLine.Find[You can't cast spells while invulnerable]}) {
		| '
		/varset CastResult CAST_INVULNERABLE
	} else /if (${rLine.Find[Your casting has been interrupted]}) {
		/varset CastResult CAST_INTERRUPTED
	} else /if (${rLine.Find[This spell does not work here]}) {
		/varset CastResult CAST_OUTDOORS
	} else /if (${rLine.Find[You can't cast spells while stunned]}) {
		| '
		/varset CastResult CAST_STUNNED
	} else /if (${rLine.Find[Your spell did not take hold]}) {
		/varset CastResult CAST_TAKEHOLD
	} else /if (${rLine.Find[You must be standing to cast a Spell]}) {
		/varset CastResult CAST_DUCKING
	} else /if (${rLine.Find[You can only cast this spell in the outdoors]}) {
		/varset CastResult CAST_OUTDOORS
	} else /if (${rLine.Find[Your ability to use]}) {
		/varset CastResult CAST_PRESTIGE
	} else /if (${rLine.Find[Your spell was]} && ${rLine.Find[ly successful]}) {
		/varset CastResult CAST_PARTIAL
	} else /if (${rLine.Find[Your target resisted the]}) {
		/varset CastResult CAST_RESIST
	} else /if (${rLine.Find[You are missing some required components]}) {
		/varset CastResult CAST_MISSINGREAGENT
	} else /if (${rLine.Find[Your spell fizzles]}) {
		/varset CastResult CAST_FIZZLE
	} else /if (${rLine.Find[You cannot see your target]}) {
		/varset CastResult CAST_CANNOTSEE
	} else /if (${rLine.Find[Your target cannot be mesmerized]}) {
		/varset CastResult CAST_IMMUNE
	} else /if (${rLine.Find[Your target looks unaffected]}) {
		/varset CastResult CAST_IMMUNE
	} else /if (${rLine.Find[You miss a note bringing your song to a close]}) {
		/varset CastResult CAST_INTERRUPTED
	} else /if (${rLine.Find[You must first select a target for this spell]}) {
		/varset CastResult CAST_NOTARGET
	} else /if (${rLine.Find[This spell only works on]}) {
		/varset CastResult CAST_NOTARGET
	| } else /if (${rLine.Find[Spell recast time not yet met]}) {
	| 	/varset CastResult CAST_RECOVERY
	} else /if (${rLine.Find[Insufficient Mana to cast this spell]}) {
		/varset CastResult CAST_OUTOFMANA
	} else /if (${rLine.Find[Your target is out of range get closer]}) {
		/varset CastResult CAST_OUTOFRANGE
	| } else /if (${rLine.Find[You haven't recovered yet]}) {
		| '
	| 	/varset CastResult CAST_RECOVERY
	| } else /if (${rLine.Find[Spell recovery time not yet met]}) {
	| 	/varset CastResult CAST_RECOVERY
	}	else /if (${rLine.Find[Your song ends]}) {
		/varset CastResult CAST_INTERRUPTED
	} else {
		/varset CastResult CAST_UNKNOWN
	}
/return

Sub Interrupt
	/if (${Defined[interruptFlag]}) /varset interruptFlag TRUE
	/stopcast
/return

Sub MemSpell(string spellName,int gemNumber)
	/declare memAttemptNum		int local 0
	:memAttempt
		/varcalc memAttemptNum ${memAttemptNum} + 1
		/memspell ${gemNumber} "${spellName}"
		/delay 5s ${Me.Gem[${spellName}]}
	/if (!${Me.Gem[${spellName}]} && ${memAttemptNum} < 3) /goto :memAttempt
	
	/if (${Window[SpellBookWnd].Open}) /book
/return

