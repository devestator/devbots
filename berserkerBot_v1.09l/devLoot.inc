| ======================================================================================================
| 																		devLoot.inc v1.36
| 																	Written By - Devestator
| 
| Description:
|  devLoot is an include file meant to handle any looting situation.  INI is created and auto populated
|   as new items are found.  Change the settings of an item and that action will be taken automatically
|   the next time the item is found.
| 
| USAGE:  This is primarily intended for use in macros.  /call Loot <corpseID>
|   This will automatically loot the specified corpseID, or you can leave corpseID blank and it will
|    loot all corpses within the INI set CorpseRadius.
| 
| v1.36 Update Notes
|  -Added auto ignoring of lore items that the character already has
| 
| v1.35 Update Notes
|  -Added INI Setting CorpseZRadius=50 and Z Radius checking to corpse detection
|  
| v1.34 Update Notes
|  -Added a delay after looting a no drop item with confirmation dialogue turned on
| 
| v1.33 Update Notes
|  -Added [Settings] ConfirmNoDrop=TRUE/FALSE so that you can disable waiting on the confirmation dialogue
|    if you have it turned off for no drop items
| 
| v1.32 Update Notes
|  -Added the command /lootupdate "ItemName" ACTION, this will add or update ItemName with the specified
|    ACTION
| 
| v1.31 Update Notes
|  -Corrected a bug in sell routine that was not selling the correct items.
|  -Corrected debug statements in the loot routine that were outputting null values
|  -Added setting [Settings] UseValueOnly=TRUE/FALSE, this will cause looting to be based only on the value
|    of the item and your MinValueKeep setting.  It will ignore all other settings in the INI.
| 
| v1.30 Update Notes
|  -Fixed it to not try tokeep looting a corpse that you cannot loot.
| 
| v1.29 Update Notes
|  -Added support for Ignore settings so that NinjaAdvLoot loot.ini's could be used.
| 
| v1.28 Update Notes
|  -Added loot retry settings to support multiple looters
|  -Fixed a typo in LootSell and LootBank routines
| 
| v1.27 Update Notes
|  -Added more echo messages when looting to help with tracking and debugging in the future
| 
| v1.26 Update Notes
|  -Updated echo settings to use devCommon EchoLog if it is loaded
|  
| v1.25 Update Notes
|  -Corrected a problem that could cause some items on a corpse to be ignored
|  -Changed LootInit routine LootInit(bool silentLoad,string devLootINI)
|  -Added a delay after opening the corpse to help account for lag slowing down the loading of all items
| 
| v1.24 Update Notes
|  -Corrected a typo in moving routine that could sometimes cause it to error out and end the macro
| 
| v1.23 Update Notes
|  -Corrected a few typing errors that could cause problems with loading INIs
|  -Changed a few variable names that were conflicting with other macros
|  -Added [Settings] LootInCombat INI setting, if FALSE and you enter combat while looting it will stop looting
|  -Changed move to corpse routine for greater compatability with all macros.
| 
| v1.22 Update Notes
|  -Corrected some bugs with looting
|  -Made looting routine faster
|  -Changed to devMovement.inc movement routines because /moveto sucks arse
| 
| v1.21 Update Notes
|  -Added INI Setting [Settings] MinValueKeep=0, this setting if set to a value in copper will automatically
|   keep any items above this price.  If they are not in the INI alreayd it will set their settings to SELL.
|   Items below the value threshold will follow their setting or the default settingin the INI
|  
| v1.20 Update Notes
|  -Added a check to make sure the include is properly initialized no matter what sub is called first
|  -Fixed a few looting bugs and checks to ensure higher rate of success at looting
| 
| =======================================================================================================

#event cannotLoot "You may not loot this corpse at this time."
#event iniUpdate "#*#LOOT INIUpdate Setting: #1#"

Sub BuildCorpseArray
	/declare corpseCountDL		int local
	/declare cArray						int local
	/declare cID							int local
	/declare cCount						int local 0
	/declare sStr							string local
	/declare mReturn					string local
	
	/for cArray 1 to 100
		/varset cpArray[${cArray}] 0
	/next cArray
	/varset corpseCountDL ${SpawnCount[corpse radius ${corpseRadius} noalert 199]}
	/for cArray 1 to ${corpseCountDL}
		/varset cID ${Me.NearestSpawn[${cArray},corpse radius ${corpseRadius} noalert 199].ID}
		/if (${cID}) {
			/if (!${Spawn[${cID}].Deity.ID} && ${Spawn[${cID}].DistanceZ} < ${corpseZRadius}) {
				/call LootTracker ${cID}
				/varset mReturn ${Macro.Return}
				/if (${mReturn.Equal[FALSE]}) {
					/varcalc cCount ${cCount} + 1
					/varset cpArray[${cCount}] ${cID}
				}
			} else {
				/squelch /alert add 199 ID ${cID}
			}	
		} else {
			/goto :doneBuilding
		}
	/next cArray
	:doneBuilding
/return

Sub BuildINI(string iniBuild)
	/if (!${Defined[iniBuild]}) /declare iniBuild				string local ${lootINI}
	
	/ini ${iniBuild} Settings CorpseRadius ${corpseRadius}
	/ini ${iniBuild} Settings LinkChannel ${linkChannel}
	/ini ${iniBuild} Settings LinkLeave	${linkLeave}
	/ini ${iniBuild} Settings LinkNoDrop ${linkNoDrop}
	/ini ${iniBuild} Settings NoDropDefault ${nodropDefault}
	/ini ${iniBuild} Settings UnknownDefault ${unknownDefault}
	/ini ${iniBuild} Settings MinValueKeep ${minValueKeep}
	/ini ${iniBuild} Settings UseValueOnly ${valueOnly}
	/ini ${iniBuild} Settings LootInCombat ${lootInCombat}
	/ini ${iniBuild} Settings LootAttempts ${lootAttempts}
	/ini ${iniBuild} Settings LootAttemptDly ${lootAttemptDly}
	/ini ${iniBuild} Settings ConfirmNoDrop ${nodropConfirm}
	
	/ini ${iniBuild} 0 0 ${unknownDefault}
	/ini ${iniBuild} 1 1 ${unknownDefault}
	/ini ${iniBuild} 2 2 ${unknownDefault}
	/ini ${iniBuild} 3 3 ${unknownDefault}
	/ini ${iniBuild} 4 4 ${unknownDefault}
	/ini ${iniBuild} 5 5 ${unknownDefault}
	/ini ${iniBuild} 6 6 ${unknownDefault}
	/ini ${iniBuild} 7 7 ${unknownDefault}
	/ini ${iniBuild} 8 8 ${unknownDefault}
	/ini ${iniBuild} 9 9 ${unknownDefault}
	/ini ${iniBuild} A A ${unknownDefault}
	/ini ${iniBuild} B B ${unknownDefault}
	/ini ${iniBuild} C C ${unknownDefault}
	/ini ${iniBuild} D D ${unknownDefault}
	/ini ${iniBuild} E E ${unknownDefault}
	/ini ${iniBuild} F F ${unknownDefault}
	/ini ${iniBuild} G G ${unknownDefault}
	/ini ${iniBuild} H H ${unknownDefault}
	/ini ${iniBuild} I I ${unknownDefault}
	/ini ${iniBuild} J J ${unknownDefault}
	/ini ${iniBuild} K K ${unknownDefault}
	/ini ${iniBuild} L L ${unknownDefault}
	/ini ${iniBuild} M M ${unknownDefault}
	/ini ${iniBuild} N N ${unknownDefault}
	/ini ${iniBuild} O O ${unknownDefault}
	/ini ${iniBuild} P P ${unknownDefault}
	/ini ${iniBuild} Q Q ${unknownDefault}
	/ini ${iniBuild} R R ${unknownDefault}
	/ini ${iniBuild} S S ${unknownDefault}
	/ini ${iniBuild} T T ${unknownDefault}
	/ini ${iniBuild} U U ${unknownDefault}
	/ini ${iniBuild} V V ${unknownDefault}
	/ini ${iniBuild} W W ${unknownDefault}
	/ini ${iniBuild} X W ${unknownDefault}
	/ini ${iniBuild} Y Y ${unknownDefault}
	/ini ${iniBuild} Z Z ${unknownDefault}
/return

Sub Close_Packs
	/declare packLoop	int local

	/for packLoop 1 to 10
		/if (${Window[pack${packLoop}].Open}) /itemnotify ${Me.Inventory[pack${packLoop}].InvSlot} rightmouseup
	/next packLoop
/return

Sub Event_cannotLoot
	/varset cannotLoot TRUE
	/if (${Target.ID} && ${Target.Type.Equal[Corpse]}) {
		/squelch /alert add 199 ID ${Target.ID}
	}
/return

Sub Loot(int corpseID)
	/if (!${Defined[devLootFirstRun]}) /call LootInit
	/if (!${Defined[corpseID]}) /declare corpseID					int local 0
	/if (!${Defined[clArray]}) /declare clArray						int outer 0
	
	/if (${Cursor.ID}) {
		/call LootEcho "Cannot loot when something is on the Cursor"
		/return LOOT_CURSORFULL
	}
	/call LootTrackerClean
	/if (${corpseID}) {
		/call LootCorpse ${corpseID}
		/return ${Macro.Return}
	} else {
		/call BuildCorpseArray
		/for clArray 1 to 100
			/if (${cpArray[${clArray}]}) {
				/call LootCorpse ${cpArray[${clArray}]}
				/if (${Macro.Return.Equal[LOOT_ABORT_COMBAT]}) /return LOOT_ABORT_COMBAT
			} else {
				/goto :doneLooting
			}
		/next clArray
		:doneLooting
	}
/return COMPLETED_SUCCESS

Sub LootBank(string bankerName)
	/if (!${Defined[devLootFirstRun]}) /call LootInit
	/if (!${Defined[bankerName]}) /declare bankerName					string local NULL
	/declare invItem						int local 0
	/declare cItemAction				string local
	/declare bankType						string local BIGB
	/declare bankPrefix					string local Big
	
	/if (${bankerName.NotEqual[NULL]}) {
		/squelch /target ${bankerName}
		/delay 1s ${Target.Name.Find[${bankerName}]}
	} else /if (!${Target.ID} || ${Target.Type.NotEqual[NPC]} || ${Target.Distance} > 19) {
		/squelch /target clear
		/delay 1s !${Target.ID}
		/squelch /target npc
		/delay 1s ${Target.ID}
	}
	/if (${Target.ID} && ${Target.Distance} > 19) {
		/call LootEcho "You are to far away from the banker to Bank"
		/return LOOTBANK_TOOFAR
	} else /if (!${Target.ID}) {
		/call LootEcho "A banker Target could not be found"
		/return LOOTBANK_NOTARGET
	}
	/if (!${Window[BankWnd].Open} && !${Window[BigBankWnd].Open}) {
		/click Right Target
		/delay 5s (${Window[BankWnd].Open} || ${Window[BigBankWnd].Open})
		/if (!${Window[BankWnd].Open} && !${Window[BigBankWnd].Open}) {
			/call LootEcho "Failed to Open the Bank Window"
			/return LOOTBANK_NOWINDOW
		}
	}
	/if (${Cursor.ID}) {
		/autoinv
		/delay 1s !${Cursor.ID}
		/if (${Cursor.ID}) {
			/call LootEcho "There is something on your Cursor and we cannot get it off, cannot Bank with an Item on your Cursor."
			/return LOOTBANK_CURSORFULL
		}
	}
	/if (${Window[BankWnd].Open}) {
		/varset bankType BW
		/varset bankPrefix
	}
	/for invItem 1 to 10
		/varset cItemAction ${Ini[${lootINI},${Me.Inventory[pack${invItem}].Name.Left[1]},${Me.Inventory[pack${invItem}].Name},ITEM_NOTFOUND]}
		/if (${cItemAction.Equal[KEEP]}) {
			/shiftkey /itemnotify ${Me.Inventory[pack${invItem}].InvSlot} leftmouseup
			/delay 1s ${Cursor.ID}
			/call LootEcho "Banking ${Cursor.Name}"
			/notify ${bankPrefix}BankWnd ${bankType}_AutoButton leftmouseup
			/delay 1s !${Cursor.ID}
			/if (${Cursor.ID}) {
				/call LootEcho "There is still an Item on your Cursor, the Bank must be full."
				/autoinv
				/delay 1s !${Cursor.ID}
				/return LOOTBANK_BANKFULL
			}
			/delay 3
		}
	/next invItem
	
	/call Open_Packs
	/declare pLoop					int local
	/for pLoop 1 to 10
		/if (${Me.Inventory[pack${pLoop}].Container}) {
			/for invItem 1 to ${Me.Inventory[pack${pLoop}].Container}
				/varset cItemAction ${Ini[${lootINI},${Me.Inventory[pack${pLoop}].Item[${invItem}].Name.Left[1]},${Me.Inventory[pack${pLoop}].Item[${invItem}].Name},ITEM_NOTFOUND]}
				/if (${cItemAction.Equal[KEEP]}) {
					/shiftkey /itemnotify ${Me.Inventory[pack${pLoop}].Item[${invItem}].InvSlot} leftmouseup
					/delay 1s ${Cursor.ID}
					/call LootEcho "Banking ${Cursor.Name}"
					/notify ${bankPrefix}BankWnd ${bankType}_AutoButton leftmouseup
					/delay 1s !${Cursor.ID}
					/if (${Cursor.ID}) {
						/call LootEcho "There is still an Item on your Cursor, the Bank must be full."
						/autoinv
						/delay 1s !${Cursor.ID}
						/return LOOTBANK_BANKFULL
					}
					/delay 3
				}
			/next invItem
		}
	/next pLoop
	/call Close_Packs
	/notify ${bankPrefix}BankWnd ${bankType}_DoneButton leftmouseup
/return LOOTBANK_SUCCESS

Sub LootCorpse(int corpseID)
	/if (!${Defined[devLootFirstRun]}) /call LootInit
	/declare cItems									int local
	/declare cItem									int local
	/declare cItemAction						string local
	/declare linkItems							bool local false
	/declare tItems									int local 0
	/declare lAttempt								int local 0
	/declare nA											int local 0

	/squelch /target ID ${corpseID}
	/delay 1s ${Target.ID}==${corpseID}
	/if (${Target.ID} && ${Target.Type.Equal[Corpse]} && ${Target.ID}==${corpseID}) {
		/varset cannotLoot FALSE
		/call LootEcho "LOOT:  Attempting to loot ${Target.CleanName} | ${Target.ID}" TRUE
		/if (${Target.Distance} > 15) {
			/call MoveToCorpse ${corpseID} 12
			/if (${Macro.Return.NotEqual[COMPLETED_SUCCESS]}) {
				/return LOOT_${Macro.Return}
			}
			/delay 3
		}
		:lootAttempt
			/varcalc lAttempt ${lAttempt} + 1
			/loot
			/delay 2s ${Window[LootWnd].Open}
			/doevents
			/if (${cannotLoot}) /return LOOT_CANNOTLOOT
			/if (!${Window[LootWnd].Open} && ${lAttempt} < ${lootAttempts}) {
				/delay ${lootAttemptDly}
				/goto :lootAttempt
			}
			/if (!${Window[LootWnd].Open}) /return LOOT_NOLOOTWINDOW
		
		:lagDelay
		/varset tItems ${Corpse.Items}
		/delay 1s ${tItems}!=${Corpse.Items}
		/if (${tItems}!=${Corpse.Items}) /goto :lagDelay
		
		/for nA 1 to 100
			/varset leftItems[${nA}] NULL
		/next nA
		/varset leftCount 0
		/varset cItems ${Corpse.Items}
		/call LootEcho "LOOT: ${cItems} item${If[${cItems}!=1,s,]} on the corpse" TRUE
		/if (${cItems}) {
			/for cItem 1 to ${cItems}
				/if (${Corpse.Item[${cItem}].Lore} && ${FindItem[${Corpse.Item[${cItem}].Name}].ID} && ${FindItem[${Corpse.Item[${cItem}].Name}].ID} == ${Corpse.Item[${cItem}].ID}) {
					/call LootEcho "LOOT:  Item ${cItem} - ${Corpse.Item[${cItem}].Name} | Lore: ${Corpse.Item[${cItem}].Lore} | No Drop: ${Corpse.Item[${cItem}].NoDrop} | Action: IGNORING DUPLICATE LORE ITEM" TRUE
					/next cItem
				}
				/varset cItemAction NULL 
				/for nA 1 to 1000
					/if (${overrides[1,${nA}].Equal[${Corpse.Item[${cItem}].Name}]}) {
						/varset cItemAction ${overrides[2,${nA}]}
					}
				/if (${overrides[1,${nA}].NotEqual[NULL]}) /next nA
				/if (${cItemAction.Equal[NULL]}) {
					/if (!${valueOnly} || !${minValueKeep}) {
						/varset cItemAction ${Ini[${lootINI},${Corpse.Item[${cItem}].Name.Left[1]},${Corpse.Item[${cItem}]},ITEM_NOTFOUND]}
					} else {
						/if (${Corpse.Item[${cItem}].Value} >= ${minValueKeep}) {
							/varset cItemAction SELL
						} else {
							/varset cItemAction LEAVE
						}
					}
				}
				/call LootEcho "LOOT:  Item ${cItem} - ${Corpse.Item[${cItem}].Name} | Lore: ${Corpse.Item[${cItem}].Lore} | No Drop: ${Corpse.Item[${cItem}].NoDrop} | Action: ${cItemAction} ${If[${minValueKeep},| Value: ${Corpse.Item[${cItem}].Value} | Min Keep Value: ${minValueKeep},]}" TRUE
				/if (${Select[${cItemAction},ITEM_NOTFOUND,KEEP,SELL]}) {
					/if (${cItemAction.Equal[ITEM_NOTFOUND]}) {
						/if (${Corpse.Item[${cItem}].NoDrop}) {
							| /if (${linkNoDrop}) /varset linkItems true
							/varset cItemAction ${nodropDefault}					
						} else {
							/if (${minValueKeep} && ${Corpse.Item[${cItem}].Value}>=${minValueKeep}) {
								/varset cItemAction SELL
							} else {
								/varset cItemAction ${unknownDefault}
							}
						}
						/ini ${lootINI} ${Corpse.Item[${cItem}].Name.Left[1]} "${Corpse.Item[${cItem}]}" ${cItemAction}
						/if (${cItemAction.Equal[DESTROY]}) {
							/call LootItem ${cItem} TRUE left
						} else /if (${cItemAction.Equal[LEAVE]} || ${cItemAction.Equal[IGNORE]}) {
							/varcalc leftCount ${leaveCount} + 1
							/varset leftItems[${leftCount}] ${Corpse.Item[${cItem}].Name}
							/if (${linkLeave} || (${Corpse.Item[${cItem}].NoDrop} && ${linkNoDrop})) /varset linkItems true
						} else {
							/call LootItem ${cItem}
						}
					} else {
						/call LootItem ${cItem}
					}
				} else {
					/if (${minValueKeep} && ${Corpse.Item[${cItem}].Value}>=${minValueKeep}) {
						/call LootItem ${cItem}
					} else /if (${cItemAction.Equal[DESTROY]}) {
						/call LootItem ${cItem} TRUE Left
					} else /if (${cItemAction.Equal[LEAVE]} || ${cItemAction.Equal[IGNORE]}) {
						/varcalc leftCount ${leaveCount} + 1
						/varset leftItems[${leftCount}] ${Corpse.Item[${cItem}].Name}
						/if ((${Corpse.Item[${cItem}].NoDrop} && ${linkNoDrop}) || ${linkLeave}) /varset linkItems TRUE
						| /if (${linkLeave}) /varset linkItems true
					}
				}
				:nextcItem
				/delay 3
			/next cItem
		}
		/if (${linkItems}) {
			/keypress enter
			/delay 2
			/call Type ${linkChannel}
			/delay 2
			/keypress space chat
			/delay 2
			/nomodkey /notify LootWnd LW_BroadcastButton leftmouseup
			/delay 2
			/keypress enter chat
		}
		:closeLootWnd
		/if (${Window[LootWnd].Open}) /nomodkey /notify LootWnd LW_DoneButton leftmouseup
		/delay 2s !${Window[LootWnd].Open}
		/if (${Window[LootWnd].Open}) /goto :closeLootWnd
		/call LootTracker ${corpseID} TRUE
		/call LootEcho "LOOT:  Looting attempt finished on ${corpseID}" TRUE
		/return LOOT_SUCCESS
	} else {
		/return LOOT_NOTARGET
	}
/return LOOT_UNKNOWN

Sub LootEcho(string echoMsg,bool logOnly)
	/if (!${Defined[logOnly]}) /declare logOnly			bool local FALSE
	/if (${Defined[devCommonVer]}) {
		/call EchoLog "${echoMsg}" ${logOnly}
	} else {
		/if (!${logOnly}) /echo >> ${echoMsg}
		/mqlog ${echoMsg}
	}
/return

Sub LootInit(bool silentLoad, string devLootIniName)
	/declare devLootVersion			float outer 1.36
	/if (!${Defined[devLootIniName]}) /declare devLootIniName					string local devLoot.ini
	/declare lootINI						string outer ${devLootIniName}
	/declare linkNoDrop					bool outer true
	/declare linkLeave					bool outer true
	/declare unknownDefault			string outer KEEP
	/declare nodropDefault			string outer LEAVE
	/declare nodropConfirm			bool outer TRUE
	/declare corpseRadius				int outer 100
	/declare corpseZRadius			int outer 50
	/declare linkChannel				string outer /g
	/declare devLootFirstRun		bool outer FALSE
	/declare minValueKeep				int outer 0
	/declare cpArray[100]				int outer
	/declare lootInCombat				bool outer FALSE
	/declare lootAttempts				int outer 0
	/declare lootAttemptDly			string outer 0s
	/declare cannotLoot					bool outer FALSE
	/declare valueOnly					bool outer FALSE
	/declare iniUpdateStr			string outer
	/declare overrides[2,1000]		string outer NULL
	/declare leftItems[100]			string outer NULL
	/declare leftCount					int outer 0
	
	/if (${Ini[${lootINI},Settings,CorpseRadius,NULL].Equal[NULL]}) {
		/call BuildINI ${lootINI}
	} else {
		/varset linkNoDrop ${Ini[${lootINI},Settings,LinkNoDrop,${linkNoDrop}]}
		/varset linkLeave ${Ini[${lootINI},Settings,LinkLeave,${linkLeave}]}
		/varset unknownDefault ${Ini[${lootINI},Settings,UnknownDefault,${unknownDefault}]}
		/varset nodropDefault ${Ini[${lootINI},Settings,NoDropDefault,${nodropDefault}]}
		/varset corpseRadius ${Ini[${lootINI},Settings,CorpseRadius,${corpseRadius}]}
		/varset corpseZRadius ${Ini[${lootINI},Settings,CorpseZRadius,${corpseZRadius}]}
		/varset linkChannel ${Ini[${lootINI},Settings,LinkChannel,${linkChannel}]}
		/varset minValueKeep ${Ini[${lootINI},Settings,MinValueKeep,${minValueKeep}]}
		/varset valueOnly ${Ini[${lootINI},Settings,UseValueOnly,${valueOnly}]}
		/varset lootInCombat ${Ini[${lootINI},Settings,LootInCombat,${lootInCombat}]}
		/varset lootAttempts ${Ini[${lootINI},Settings,LootAttempts,1]}
		/varset lootAttemptDly ${Ini[${lootINI},Settings,LootAttemptDly,5s]}
		/varset nodropConfirm ${Ini[${lootINI},Settings,ConfirmNoDrop,TRUE]}
	}
	/if (!${silentLoad}) {
		/call LootEcho "Initialized devLoot.inc v${devLootVersion} - Written by Devestator"
		/call LootEcho "devLoot.inc settings loaded from ${devLootIniName}"
	}

	/squelch /alias /lootupdate /echo >> LOOT INIUpdate Setting: 
/return

Sub LootItem(int itemNum,bool destItem, string mouseKey)
	/if (!${Defined[itemNum]}) /return
	/if (!${Defined[mouseKey]}) /declare mouseKey			string local right
	/if (!${Defined[destItem]}) /declare destItem			bool local false
	/if (${destItem}) /varset mouseKey Left
	/declare isNoDrop						bool local false

	/if (${Corpse.Item[${itemNum}].Lore} && ${FindItem[${Corpse.Item[${itemNum}].Name}].ID}) {
		/call LootEcho ".....Cannot loot because item is lore and we already have one." TRUE
		/return
	}
	/varset isNoDrop ${Corpse.Item[${itemNum}].NoDrop}
	| /delay 3
	/shiftkey /itemnotify ${Corpse.Item[${itemNum}].InvSlot} ${mouseKey}mouseup
	/delay 3
	/if (${isNoDrop} && ${nodropConfirm}) {
		:confWindow
		/delay 5s ${Window[ConfirmationDialogBox].Open}
		/nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
		/delay 1s !${Window[ConfirmationDialogBox]}
		/if (${Window[ConfirmationDialogBox].Open}) /goto :confWindow
		/if (${mouseKey.Equal[Left]}) /delay 2s ${Cursor.ID}
	} else /if (${mouseKey.Equal[Left]}) {
		/delay 2s ${Cursor.ID}
	}	else {
		/delay 2s !${Corpse.Item[${itemNum}].ID}
	}
	/if (${Cursor.ID} && ${destItem}) {
		/destroy
	} else /if (${Cursor.ID}) {
		/autoinv
	}
	/delay 2s !${Cursor.ID}
	/delay 2
	/if (${Cursor.ID}) {
		/call LootEcho "LOOT:  Your Inventory appears to be full"
		/autoinv
	}
/return

Sub LootSell(string merchName)
	/if (!${Defined[devLootFirstRun]}) /call LootInit
	/if (!${Defined[merchName]}) /declare merchName					string local NULL
	/declare invItem						int local 0
	/declare cItemAction				string local
	
	/if (${merchName.NotEqual[NULL]}) {
		/squelch /target ${merchName}
		/delay 1s ${Target.Name.Find[${merchName}]}
	} else /if (!${Target.ID} || ${Target.Type.NotEqual[NPC]} || ${Target.Distance} > 19) {
		/squelch /target clear
		/delay 1s !${Target.ID}
		/squelch /target npc
		/delay 1s ${Target.ID}
	}
	/if (${Target.ID} && ${Target.Distance} > 19) {
		/call LootEcho "You are to far away from the Merchant to sell"
		/return LOOTSELL_TOOFAR
	} else /if (!${Target.ID}) {
		/call LootEcho "A Merchant Target could not be found"
		/return LOOTSELL_NOTARGET
	}
	/if (!${Window[MerchantWnd].Open}) {
		/click Right Target
		/delay 5s ${Window[MerchantWnd].Open}
		/if (!${Window[MerchantWnd].Open}) {
			/call LootEcho "Failed to Open the Merchant Window"
			/return LOOTSELL_NOWINDOW
		}
	}
	/if (${Cursor.ID}) {
		/autoinv
		/delay 1s !${Cursor.ID}
		/if (${Cursor.ID}) /call LootEcho "There is something on your Cursor and we cannot get it off, If it is meant to be sold it may not get sold."
	}
	/for invItem 1 to 10
		/if (${Me.Inventory[pack${invItem}].ID}) {
			/varset cItemAction ${Ini[${lootINI},${Me.Inventory[pack${invItem}].Name.Left[1]},${Me.Inventory[pack${invItem}].Name},ITEM_NOTFOUND]}
			/if (${cItemAction.Equal[SELL]} || (${valueOnly} && ${Me.Inventory[${invItem}].Value}>=${minValueKeep} && !${Me.Inventory[pack${invItem}].Container})) {
				/itemnotify ${Me.Inventory[pack${invItem}].InvSlot} leftmouseup
				/delay 5
				/call LootEcho "Selling ${Me.Inventory[pack${invItem}].Name}"
				/shiftkey /notify MerchantWnd MW_Sell_Button leftmouseup
				/delay 5s !${Me.Inventory[pack${invItem}].Item.ID}
				/delay 3
			}
		}
	/next invItem
	
	/call Open_Packs
	/declare pLoop					int local
	/for pLoop 1 to 10
		/if (${Me.Inventory[pack${pLoop}].Container}) {
			/for invItem 1 to ${Me.Inventory[pack${pLoop}].Container}
				/if (${Me.Inventory[pack${pLoop}].Item[${invItem}].ID}) {
					/varset cItemAction ${Ini[${lootINI},${Me.Inventory[pack${pLoop}].Item[${invItem}].Name.Left[1]},${Me.Inventory[pack${pLoop}].Item[${invItem}].Name},ITEM_NOTFOUND]}
					/if (${cItemAction.Equal[SELL]} || (${valueOnly} && ${Me.Inventory[pack${pLoop}].Item[${invItem}].Value}>=${minValueKeep} && !${Me.Inventory[pack${pLoop}].Item[${invItem}].Container})) {
						/itemnotify ${Me.Inventory[pack${pLoop}].Item[${invItem}].InvSlot} leftmouseup
						/delay 5
						/call LootEcho "Selling ${Me.Inventory[pack${pLoop}].Item[${invItem}].Name}"
						/shiftkey /notify MerchantWnd MW_Sell_Button leftmouseup
						/delay 5s !${Me.Inventory[pack${pLoop}].Item[${invItem}].ID}
						/delay 3
					}
				}
			/next invItem
		}
	/next pLoop
	/call Close_Packs
	/notify MerchantWnd MW_Done_Button leftmouseup
/return LOOTSELL_SUCCESS

Sub lootstrafePastObstacle
  /keypress forward
  /keypress back hold
  /delay 3
  /keypress back

  /if (${Math.Rand[99]}>50 ) { 
      /keypress strafe_right hold
  } else {
      /keypress strafe_left hold 
  } 
  /delay 1s 
  /keypress strafe_right 
  /keypress strafe_left 
  /keypress forward hold 
/return 

Sub LootTracker(int corpseID,bool addTracker)
	/if (!${Defined[corpseArray]}) /declare corpseArray[1000]							int outer
	/if (!${Defined[addTracker]}) /declare addTracker											bool local false
	/declare ltArray								int local
	
	/if (!${addTracker}) {
		/if (!${Defined[corpseLooted${corpseID}]}) {
			/return FALSE
		} else {
			/return TRUE
		}
	} else {
		/if (!${Defined[corpseLooted${corpseID}]}) /declare corpseLooted${corpseID}			bool outer
		/varset corpseLooted${corpseID} true
		/for ltArray 1 to 1000
			/if (!${corpseArray[${ltArray}]}) {
				/varset corpseArray[${ltArray}] ${corpseID}
				/goto :trackAdded
			}
		/next ltArray
		:trackAdded
		/return TRUE
	}
/return TRUE

Sub LootTrackerClean
	/if (!${Defined[corpseArray]}) {
		/declare corpseArray[1000]							int outer
		/return
	}
	/declare ltcArray								int local
	
	/for ltcArray 1 to 1000
		/if (${corpseArray[${ltcArray}]}) {
			/if (!${Spawn[${corpseArray[${ltcArray}]}].ID} || ${Spawn[${corpseArray[${ltcArray}]}].Type.NotEqual[Corpse]}) {
				/if (${Defined[corpseLooted${corpseArray[${ltcArray}]}]}) /deletevar corpseLooted${corpseArray[${ltcArray}]}
				/varset corpseArray[${ltcArray}] 0
			}
		}
	/next ltcArray
/return

Sub MoveToCorpse(int MoveToID, int StopDistance) 
	/if (!${Defined[MoveToID]} || !${Spawn[${MoveToID}].ID} || ${Spawn[${MoveToID}].Type.NotEqual[Corpse]}) /return
  /if (!(${Defined[StopDistance]})) /declare StopDistance int local 20

  /declare running						int local 0
  /declare distanceNow				float local 
  /declare distanceBefore			float local ${Spawn[${MoveToID}].Distance} 
  /declare distanceModifier		int local 1
  /declare distanceTimer			timer 5 
  /declare strafeAttempts			int local 0
  /declare doorAttempt				bool local false
	/declare moveTimer					timer local 30s
	  
  :moveToCorpse
	/if (${Me.Underwater} || ${Me.FeetWet}) { 
		/squelch /face fast id ${MoveToID}
  } else {
		/squelch /face fast nolook id ${MoveToID} 
	}

  /if (${Spawn[${MoveToID}].Distance}<${StopDistance}) { 
    /keypress forward 
    /return COMPLETED_SUCCESS
  } else /if (!${moveTimer}) {
  	/keypress forward
  	/return ABORT_TIMEOUT
  }
  /if (!${Defined[emuMode]} || !${emuMode}) {
		/if (${Me.CombatState.Equal[COMBAT]} && !${lootInCombat}) {
			/keypress forward
			/return ABORT_COMBAT
		}  
	}
	    
  /if (!${distanceTimer}) { 
    /if (${Me.Sneaking}) { 
        /varset distanceModifier 2 
    } else { 
        /varset distanceModifier 1 
    } 

    /varset distanceNow ${Spawn[${MoveToID}].Distance} 
    /if (${Math.Calc[${distanceBefore}-${distanceNow}]}<${Math.Calc[10/${distanceModifier}]} && ${distanceBefore}) {
			/if (!${Me.Stunned} && !${Debuff.Rooted}) {
			  /squelch /doortarget
			  /if (${DoorTarget.Name.Length} && ${DoorTarget.Distance} < 12) {
			  	/if (${Math.Abs[${Me.Heading.Degrees} - ${DoorTarget.HeadingTo.Degrees}]} < 20) {
			  		/keypress FIRST_PERSON_CAMERA
			    	/nomodkey /keypress U hold
			    	/delay 2
			    	/nomodkey /keypress U
			    }
				}
				/if (${strafeAttempts} > 3) {
					/keypress forward
					/return ABORT_STUCK
				} else /if (!${doorAttempt}) {
					/keypress FIRST_PERSON_CAMERA
					/squelch /doortarget
					/nomodkey /keypress U hold
					/delay 2
					/nomodkey /keypress U
					/varset doorAttempt true
				} else {
					/call lootstrafePastObstacle 
					/varcalc strafeAttempts ${strafeAttempts}+1
				}
			} else /if (${Debuff.Rooted}) {
				/mqlog MoveToSpawn:  Movement aborted due to being rooted
				/return ABORT_ROOT
			}         
    }
    
    /varset distanceBefore ${Spawn[${MoveToID}].Distance} 
    /varset distanceTimer 15 
 	} 

  /if (!${running}) { 
      /keypress forward 
      /if (${Spawn[${MoveToID}].Distance}>=${StopDistance}) { 
          /varset running 1 
          /keypress forward hold 
      } 
  } else { 
      /if (${Spawn[${MoveToID}].Distance}<${StopDistance}) { 
          /varset running 0 
          /keypress forward 
      } 
  }
	/doevents
	/goto :moveToCorpse
/return

Sub Open_Packs
	/declare packLoop	int local

	/for packLoop 1 to 10
		/if (${Me.Inventory[pack${packLoop}].Container} && !${Window[pack${packLoop}].Open}) /itemnotify ${Me.Inventory[pack${packLoop}].InvSlot} rightmouseup
		/if (${Window[TradeSkillWnd].Open}) {
			/notify TradeSkillWnd COMBW_ExperimentButton leftmouseup
			/delay 3
		}
	/next packLoop
/return

Sub Type(string InStr) 
   /declare char string local 
   /declare loopctr int local 
   /for loopctr 1 to ${String[${InStr}].Length} 
      /varset char ${String[${InStr}].Mid[${loopctr},1]} 
      /if (!${String[${char}].Length}) { 
         /keypress space chat 
      } else { 
         /keypress ${char} chat 
      } 
   /next loopctr 
/return 

Sub Event_iniUpdate(line,string varString)
	/mqlog LOOT INIUpdate Setting: ${varString}
	/call INIUpdate ${varString}
/return

Sub INIUpdate(string itemName,string itemAction)
	/ini ${lootINI} ${itemName.Left[1]} "${itemName}" ${itemAction}
/return

Sub AddOverride(string itemName, string itemAction)
	/declare nA		int local 0
	/for nA 1 to 1000
		/if (${overrides[1,${nA}].Equal[NULL]}) {
			/varset overrides[1,${nA}] ${itemName}
			/varset overrides[2,${nA}] ${itemAction}
		} else {
			/next nA
		}
/return

Sub RemoveOverride(string itemName)
	/declare nA			int local 0
	/declare nAMod			int local 0
	/declare tmpArray[2,1000]	string local NULL

	/for nA 1 to 1000
		/if (${overrides[1,${nA}].NotEqual[${itemName}]}) {
			/varset tmpArray[1,${Math.Calc[${nA}-${nAMod}]}] ${overrides[1,${nA}]}
			/varset tmpArray[2,${Math.Calc[${nA}-${nAMod}]}] ${overrides[2,${nA}]}
		} else {
			/varset nAMod 1
		}
	/if (${overrides[1,${nA}].NotEqual[NULL]}) /next nA
	/for nA 1 to 1000
		/varset overrides[1,${nA}] ${tmpArray[1,${nA}]}
		/varset overrides[2,${nA}] ${tmpArray[2,${nA}]}
	/next nA
/return
