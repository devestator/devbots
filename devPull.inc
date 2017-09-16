| ======================================================================================================
| 									devPull.inc v1.10
| 									Written By - Devestator
| 
| Description:
|  devPull is a include file for dev's bots to handle pulling in any situation any class.
| 
| v1.10 Update Notes
|  -Updated for 09082017 MQ2 Patch that makes a lot of changes with undefined variables and breaks function calls relying on Not defined to set defaults
| 
| v1.09 Update Notes
|  -Added PullPathMaxTime and PullPathMaxTimeN to set maximum time for a pull path even if it is still finding mobs
| 
| v1.08 Update Notes
|  -Added the ability to use a pet to pull, should only be used currenty for characters that have pet hold
| 
| v1.07 Update Notes
|  -Added the ability to use +/- on the PullMinLevel and PullMaxLevel to signifiy differences from character level
|  -Changed AltAbility Range checking to use Me.AltAbility.Spell.Range instead of Spell.Range
| 
| v1.06 Update Notes
|  -Corrected a problem with multiple pull paths being unable to load path 'FALSE' after
|    pulling on all paths.
|  -Added the ability to have a list of target pull NPCs (all others will be ignored unless they aggro)
|    INI Setting for this is [Pull Settings] PullTargetx, there can be up to 100 and they can be partial names
| 
| v1.05 Update Notes
|  -Corrected a problem causing some Discs and AAs to not cast correctly when pulling
|  -Modified Unsafe PC check while pulling to be more consistent with what it does
|  -Added hidden setting [Pull Settings] UnsafePCCheck=TRUE, you can set this to FALSE
|    in order to completely disable checking for unsafe pcs while pulling.  This can
|    speed up path movement while pulling if it is running slow for you.
| 
| v1.04 Update Notes
|  -Corrected Group.Member reference for group TLO change in the compile
|  -Made some tweaks to switching from ranged to melee if to close
|  -Adjusted XTarget aggro detection
|  -Updated pull result return to get information on pull failures back to the bot
| 
| v1.03 Update Notes
|  -Added some additional level 2 debuggin messages
| 
| v1.02 Update Notes
|  -Added some additional checking to the pull routine to prevent the bot from getting stuck out 
|    while pulling.
|  -Added setting [Pull Settings] IncomingMessage=NULL, will announce this message when it aggros
|    a mob and heads back toward camp
|  -Added option to specify a specific zone for a path PullPathZoneN=NULL, it should be set
|    equal to the zones short name if you want to limit that path to be used only in one zone
|    This should only ever need to be used if it's not a MQ2AdvPath zone, and you are changing
|    zones with very similar coordinates.
|  -Added camp check call while pulling to check camp for unsafe pcs / gms
|  -New setting [General] UnsafePCReturnToCamp=FALSE if an unsafe pc is detected at camp, TRUE will return to camp
|    with or without any mobs currently in tow before taking the specified UnsafePCAction
|    FALSE will take the specified UnsafePCAction where it is
|    if action is continue, then it will continue with the pull as normal
|  
| v1.01 Update Notes
|  -Corrected a problem with bards pulling with anything other than a song
|  -Changed healer requirements to be able to specify number of healers required to pull.
|    Setting: [Pull Settings] RequiredHealers=1  Defaults to 1, set it to the number of healers
|    required in group and or watchlist (present in the zone and not dead), required to pull.
|  -Added hidden setting [Pull Settings] MaxTargetRange=250  This is the default maximum range
|    it will attempt to use a spell / ability to pull.  It will still auto detect if a spell
|    ability or item require a shorter range.
|  
| v1.00 Update Notes
|  -initial release
|  -Multiple Pull Paths
|  -Pulling while in combat before the last mob dies (Only when using xtarget)
|  -MedTo settings for better med periods
|  -Face the direction you came from at camp
|  -Supports all classes
|  -Required buffs to have before going to pull
|  -Supports MQ2AdvPath paths
|  -Integrates better with MMOBugs target range restrictions
|  
| ======================================================================================================

|#include pathrecord.mac

Sub PullInit
	/declare devPullVer						float outer 1.10
	/declare devPullReqCommon			float outer 2.51
	/declare devPullReqMovement		float outer 1.74

	/call EchoLog "Initializing devPull.inc v${devPullVer}"	TRUE
	/if (${devCommonVer}<${devPullReqCommon}) {
		/call EchoLog "\ayThe version of devCommonPremium.inc v${devCommonVer} is not supported by devPull.inc v${devPullVer}, please download a newer version of devCommonPremium.inc"
		/endmacro
	}
	/if (${devMovementVer}<${devPullReqMovement}) {
		/call EchoLog "\ayThe version of devMovementPremium.inc v${devMovementVer} is not supported by devPull.inc v${devPullVer}, please download a newer version of devMovementPremium.inc"
		/endmacro		
	}
		
	| ===================================
	|  Load pull settings
	| ===================================
	/call EchoLog "Loading pull settings" TRUE
	/declare allowPullMode				bool outer TRUE
	/declare pullMode							bool outer FALSE
	/declare pullWith							string outer
	/declare pullType							string outer
	/declare pullPath							string outer
	/declare pullPathZone					string outer
	/declare pullPathMaxTime			string outer
	/declare pullPaths[20]				string outer
	/declare defPullPaths[20]			string outer
	/declare pullPathZones[20]		string outer
	/declare pullPathMaxTimes[20]	string outer
	/declare pullPathNum					int outer 1
	/declare pullPathTimer				timer outer 0s
	/declare multiPaths						bool outer FALSE
	/declare pullTimeOut					string outer
	/declare pullRadius						float outer
	/declare pullZRadius					float outer
	/declare pullMinLevelLoad			string outer
	/declare pullMaxLevelLoad			string outer
	/declare pullMinLevel					int outer
	/declare pullMaxLevel					int outer
	/declare lullSpell						string outer
	/declare lullGem							string outer 2
	/declare lullGemInt						int local 2
	/declare lullSong							string outer NULL
	/declare lullSlot							string outer
	/declare pullGem							string outer 1
	/declare pullGemInt						int local 0
	/declare lullRadius						float outer	
	/declare bandolierAtCamp			bool outer FALSE
	/declare defaultMinMana				int outer
	/declare defaultMinHP					int outer
	/declare defaultMinEnd				int outer
	/declare defaultMedToMana			int outer
	/declare defaultMedToHP				int outer
	/declare defaultMedToEnd			int outer
	/declare pullHoldMedding			bool outer FALSE
	/declare pullRestTime					string outer
	/declare pullTimer						timer outer 0s
	/declare pullCombatTimer			timer outer 0s
	/declare pullBandolier				string outer
	/declare normalBandolier			string outer
	/declare returnToCamp					bool outer
	/declare requiredPullBuffs[20]		string outer
	/declare fadeOnPull						bool outer	
	/declare faceAtCamp						bool outer FALSE
	/declare pullCampMobs					int outer 0
	/declare pullMobHP						int outer 0
	/declare allowCombatPull			bool outer TRUE
	/declare requireHealer				int outer 1
	/declare healerMsgTimer				timer outer 0s
	/declare maxTargetRange				float outer 0
	/declare incMessage						string outer NULL
	/declare pathsCount						int local 0
	/declare reqBuffsCount				int local 0
	/declare usePullPath					bool outer TRUE
	/declare pullState						string outer NULL
	/declare pullMedToMsgTimer		timer outer 0s
	/declare pullMsgTimer					timer outer 0s
	/declare pullResult						string outer 
	/declare fastFaceOnAggro			bool outer TRUE
	/declare pullNPCList[100]			string outer
	/declare pullNPCCount					int outer 0
	/declare pullTargetAlert			int outer 250
	
	/call GetINISetting ${iniName} "Pull Settings" PullAtStart pullMode FALSE
	/call GetINISetting ${iniName} "Pull Settings" PullWith pullWith NULL
	/call GetINISetting ${iniName} "Pull Settings" PullGem pullGem 1 FALSE
	/varset pullGemInt ${pullGem}
	/call GetINISetting ${iniName} "Pull Settings" PullType pullType ${If[${Me.Class.ShortName.Equal[BRD]},${If[${pullGemInt},GEM${pullGem},${pullGem}]},MELEE]}
	
	/call GetINISetting ${iniName} "Pull Settings" PullPathName pullPath Default FALSE
	/call GetINISetting ${iniName} "Pull Settings" PullPathMaxTime pullPathMaxTime 0 FALSE 
	
	/call GetINIArrayCount ${iniName} "Pull Settings" PullPathName NULL
	/varset pathsCount ${Macro.Return}
	
	/if (${pathsCount}) {
		/for nArray 1 to ${pathsCount}
			/call GetINISetting ${iniName} "Pull Settings" PullPathName${nArray} defPullPaths[${nArray}] ${If[${nArray}==1,${pullPath},NULL]}
			/call GetINISetting ${iniName} "Pull Settings" PullPathZone${nArray} pullPathZones[${nArray}] NULL FALSE
			/call GetINISetting ${iniName} "Pull Settings" PullPathMaxTime${nArray} pullPathMaxTimes[${nArray}] ${pullPathMaxTime} FALSE
		/next nArray
	}
	/call GetINISetting ${iniName} "Pull Settings" PullPathName${Math.Calc[${pathsCount}+1].Int} NULL ${If[!${pathsCount},${pullPath},NULL]}
		
	
	/call GetINISetting ${iniName} "Pull Settings" PullTimeout pullTimeOut 30s
	/call GetINISetting ${iniName} "Pull Settings" PullRadius pullRadius 100
	/call GetINISetting ${iniName} "Pull Settings" PullZRadius pullZRadius 100
	/call GetINISetting ${iniName} "Pull Settings" PullMinLevel pullMinLevelLoad 1
	/call GetINISetting ${iniName} "Pull Settings" PullMaxLevel pullMaxLevelLoad +0
	/call GetINISetting ${iniName} "Pull Settings" LullSong lullSong NULL FALSE
	/call GetINISetting ${iniName} "Pull Settings" Lull lullSpell "${If[${Me.Class.ShortName.Equal[BRD]},${lullSong},NULL]}"
	/call GetINISetting ${iniName} "Pull Settings" LullGem lullGem 2 FALSE
	/varset lullGemInt ${lullGem}
	/call GetINISetting ${iniName} "Pull Settings" LullSlot lullSlot ${If[${Me.Class.ShortName.Equal[BRD]},${If[${lullGemInt},GEM${lullGem},${lullGem}]},GEM2]}
	/call GetINISetting ${iniName} "Pull Settings" LullRadius lullRadius 50	
	/call GetINISetting ${iniName} "Pull Settings" DefaultMinMana defaultMinMana 50
	/call GetINISetting ${iniName} "Pull Settings" DefaultMinHP defaultMinHP 75
	/call GetINISetting ${iniName} "Pull Settings" DefaultMinEnd defaultMinEnd 50
	/call GetINISetting ${iniName} "Pull Settings" DefaultMedToMana defaultMedToMana ${defaultMinMana} FALSE
	/call GetINISetting ${iniName} "Pull Settings" DefaultMedToHP defaultMedToHP ${defaultMinHP} FALSE
	/call GetINISetting ${iniName} "Pull Settings" DefaultMedToEnd defaultMedToEnd ${defaultMinEnd} FALSE
	/call GetINISetting ${iniName} "Pull Settings" PullRestTime pullRestTime 10s
	/call GetINISetting ${iniName} "Pull Settings" CircuitRestTime circuitRestTime 1m
	/call GetINISetting ${iniName} "Pull Settings" PullBandolier pullBandolier NULL
	/call GetINISetting ${iniName} "Pull Settings" NormalBandolier normalBandolier NULL
	/call GetINISetting ${iniName} "Pull Settings" BandolierSwapAtCamp bandolierAtCamp FALSE
	/call GetINISetting ${iniName} "Pull Settings" ReturnToCamp returnToCamp TRUE
	/call GetINISetting ${iniName} "Pull Settings" FadeOnPull fadeOnPull ${If[${Me.Class.ShortName.Equal[BRD]},TRUE,FALSE]} ${If[${Me.Class.ShortName.Equal[BRD]},TRUE,FALSE]} 
	/call GetINISetting ${iniName} "Pull Settings" FaceAtCamp faceAtCamp FALSE
	/call GetINISetting ${iniName} "Pull Settings" PullAtMobsLeftInCamp pullCampMobs 0
	/call GetINISetting ${iniName} "Pull Settings" PullAtAssistMobHP pullMobHP 15
	/call GetINISetting ${iniName} "Pull Settings" RequiredHealers requireHealer 1
	/call GetINISetting ${iniName} "Pull Settings" MaxTargetRange maxTargetRange ${If[${isMMOBugs} && ${MMOBugs.TargetRange},${MMOBugs.TargetRange},250]} FALSE
	/call GetINISetting ${iniName} "Pull Settings" IncomingMessage incMessage NULL
	/call GetINISetting ${iniName} "Pull Settings" UnsafePCReturnToCamp unsafePCReturn FALSE bool
	/call GetINISetting ${iniName} "Pull Settings" UnsafePCCheck unsafePCPullCheck TRUE FALSE bool
	/call GetINISetting ${iniName} "Pull Settings" UseFastFaceOnAggro fastFaceOnAggro TRUE FALSE bool
	
	/call GetINIArrayCount ${iniName} "Pull Settings" RequiredPullBuff NULL
	/varset reqBuffsCount ${Macro.Return}	
	/for nArray 1 to ${Math.Calc[${reqBuffCounts}+1]}
		/call GetINISetting ${iniName} "Pull Settings" RequiredPullBuff${nArray} requiredPullBuffs[${nArray}] NULL
		/if (${requiredPullBuffs[${nArray}].NotEqual[NULL]}) /call EchoLog ".....Required Pull Buff ${requiredPullBuffs[${nArray}]}" TRUE
	/next nArray
	
	
	/call GetINISetting ${iniName} "Pull Settings" PullTargetAlert pullTargetAlert 250 FALSE
	/call GetINIArrayCount ${iniName} "Pull Settings" PullTarget NULL
	/varset pullNPCCount ${Macro.Return}
	/squelch /alert clear ${pullTargetAlert}
	/if (${pullNPCCount}) {
		/for nArray 1 to 100
			/call GetINISetting ${iniName} "Pull Settings" PullTarget${nArray} pullNPCList[${nArray}] NULL FALSE
			/squelch /alert add ${pullNPCList[${nArray}]}
		/next nArray
	} else {
		/squelch /alert add ${pullTargetAlert} npc
	}
	| /call GetINISetting ${iniName} "Pull Settings" PullTarget${Math.Calc[${pullNPCCount}+1].Int} NULL NULL
	
	/if (${pullMinLevelLoad.Left[1].Equal[-]} || ${pullMinLevelLoad.Left[1].Equal[+]}) {
		/varset pullMinLevel ${Math.Calc[${Me.Level} ${pullMinLevelLoad}].Int}
	} else {
		/varset pullMinLevel ${pullMinLevelLoad}
	}
	/if (${pullMinLevel} < 1) /varset pullMinLevel 1
	/if (${pullMaxLevelLoad.Left[1].Equal[-]} || ${pullMaxLevelLoad.Left[1].Equal[+]}) {
		/varset pullMaxLevel ${Math.Calc[${Me.Level} ${pullMaxLevelLoad}].Int}
	} else {
		/varset pullMaxLevel ${pullMaxLevelLoad}
	}
	/call EchoLog ".....PULLATSTART: ${pullMode} PULLWITH: ${pullWith}  PULLTYPE: ${pullType} PULLRADIUS: ${pullRadius} CAMPRETURN: ${returnToCamp} PULLMINLEVEL: ${pullMinLevel} PULLMAXLEVEL: ${pullMaxLevel}" TRUE

	/call SetHome

	| ==================	
	| Load Path(s)
	| ==================
	/declare pathLoadZone						int outer ${Zone.ID}	
	/call PullBuildPathList
	
	/if (${pullCampMobs} && ${pullMobHP} && !${useXTarget} && ${allowCombatPull}) {
		/varset allowCombatPull FALSE
		/call EchoLog "\aySettings are configured to pull when there are \ag${pullCampMobs}\ay or less mobs left in camp and the current assist target is below \ag${pullMobHP}\ay, however UseXTarget is set to \agFALSE\ay so the bot will be unable to pull while in combat."
	}
	/call EchoLog "\awdevPull.inc \agv${devPullVer} \awloaded"
/return

Sub PullBuildPathList
	/declare firstPathLoaded			bool local FALSE
	/declare validPathCount				int local 0
	/for nArray 1 to 20
		/varset pullPaths[${nArray}] ${If[${defPullPaths[${nArray}].NotEqual[NULL]} && ${defPullPaths[${nArray}].Length},${defPullPaths[${nArray}]},NULL]}
		/if (${pullPaths[${nArray}].NotEqual[NULL]} && ${pullPaths[${nArray}].Length}) {
			/if (${pullPaths[${nArray}].Equal[default]}) /varset pullPaths[${nArray}] ${Zone.ShortName}
			/call CheckPathExists ${pullPaths[${nArray}]}
			/if (${Macro.Return.Equal[TRUE]} && ${pullPaths[${nArray}].NotEqual[FALSE]} && (${pullPathZones[${nArray}].Equal[NULL]} || ${pullPathZones[${nArray}].Equal[${Zone.ShortName}]})) {
				/varcalc validPathCount ${validPathCount} + 1
				/if (!${firstPathLoaded}) {
					/call PullLoadPath ${pullPaths[${nArray}]}
					/if (${Macro.Return.NotEqual[FALSE]}) {
						/varset firstPathLoaded TRUE
						/varset pullPathNum ${nArray}
					}
				}
			} else {
				/call EchoLog "\ayUnable to find the pull path \ag${pullPaths[${nArray}]}\ay, excluding it from the pull paths list."
				/varset pullPaths[${nArray}] NULL
			}
		}
	/next nArray
	
	/if (${validPathCount}>1) /varset multiPaths TRUE
	/if (!${firstPathLoaded}) {
		/varset usePullPath FALSE
		/call EchoLog "\ayUnable to load any pull paths, pulling will default to pulling without a path within the pull radius of \ag${pullRadius}"
	}
	
	/varset pathLoadZone ${Zone.ID}
/return

Sub PullAggroCheck
	/declare xtArray			int local 0
	/declare tArray				int local 0
	/declare newAddP			bool local FALSE
	
	/if (${useXTarget}) {
		/call EchoLog "Checking pull aggro with xtarget" TRUE	5
		/for xtArray 1 to ${maxXTargets}
			/varset newAddP FALSE
			/if (${Me.XTarget[${xtArray}].ID} && (${Spawn[${Me.XTarget[${xtArray}].ID}].Type.Equal[NPC]} || (${Spawn[${Me.XTarget[${xtArray}].ID}].Type.Equal[Pet]} && ${Spawn[${Me.XTarget[${xtArray}].ID}].Owner.Type.Equal[NPC]}))) {
				/varset newAddP TRUE
				/if (${targCount}) {
					/for tArray 1 to ${targCount}
						/if (${targArray[${tArray}]}==${Me.XTarget[${xtArray}].ID}) /varset newAddP FALSE
					/if (${newAddP}) /next tArray
				}
			}
		HIGHERDEBUG "XTarget ${xtArray} is new add result ${newAddP}" TRUE 5
		/if (!${newAddP}) /next xtArray
		/varset gotAggro ${newAddP}
	} else /if (${Me.CombatState.Equal[Combat]}) {
		/varset gotAggro TRUE
	}
	/if (${Me.Casting.ID} && ${allowInterrupt} && ${gotAggro}) {
		HIGHERDEBUG "Attempting to interrupt casting" TRUE 3
		/if (${Me.Class.ShortName.Equal[BRD]}) {
			/stop
			/melody			
		}
		/call Interrupt
	}
	HIGHERDEBUG "PullAggroCheck returning ${gotAggro}" TRUE	3
/return ${gotAggro}

Sub PullCheckBuffs
	/declare pbArray						int local 0
	
	/for pbArray 1 to 20
		/if (${requiredPullBuffs[${pbArray}].NotEqual[NULL]} && !${Me.Buff[${requiredPullBuffs[${pbArray}]}].ID} && !${Me.Song[${requiredPullBuffs[${pbArray}]}].ID} && ${Me.Aura[1].NotEqual[${requiredPullBuffs[${pbArray}]}]} && ${Me.Aura[2].NotEqual[${requiredPullBuffs[${pbArray}]}]}) {
			/if (!${pullMsgTimer}) {
				HIGHERDEBUG ".....Not pulling because we are missing a required buff: ${requiredPullBuffs[${pbArray}]}" TRUE 2
				/varset pullMsgTimer 30s
			}
			/return FALSE
		}
	/next pbArray
/return TRUE

Sub PullCheckGroup
	/declare classMinMana				int local
	/declare classMinEnd				int local
	/declare classMinHP					int local
	/declare healerFound				int local 0
	/declare hArray							int local
	/declare arrayType					string local Min
	/if (!${pullHoldMedding}) {
		/varset arrayType Min
	} else {
		/varset arrayType MedTo
	}
	/varset healerFound FALSE
	/for nArray 0 to ${Group.Members}
		/if (${Group.Member[${nArray}].ID}) {
			/if (${Group.Member[${nArray}].Type.Equal[Corpse]}) {
				/if (${arrayType.Equal[MedTo]} && !${pullMedToMsgTimer}) HIGHERDEBUG ".....${Group.Member[${nArray}].CleanName} is dead, not pulling" TRUE 2
				/return FALSE
			}
			/varset classMinMana ${Ini[${iniName},Pull Settings,${Group.Member[${nArray}].Class.ShortName}${arrayType}Mana,${default${arrayType}Mana}]}
			/varset classMinEnd ${Ini[${iniName},Pull Settings,${Group.Member[${nArray}].Class.ShortName}${arrayType}End,${default${arrayType}End}]}
			/varset classMinHP ${Ini[${iniName},Pull Settings,${Group.Member[${nArray}].Class.ShortName}${arrayType}HP,${default${arrayType}HP}]}
			| /if (!${Group.Member[${nArray}].Class.CanCast}) /varset classMinMana ${Ini[${iniName},Pull Settings,${Group.Member[${nArray}].Class.ShortName}${arrayType}End,${classMinMana}]}
			
			/if (!${nArray} && (${Me.PctHPs} < ${classMinHP} || (${Me.Class.CanCast} && ${Me.PctMana} < ${classMinMana}) || (!${Me.Class.CanCast} && ${Me.PctEndurance} < ${classMinEnd}))) {
				/if (${arrayType.Equal[MedTo]} && !${pullMedToMsgTimer}) HIGHERDEBUG ".....I do not meat minimum conditions to pull CLASS: ${Me.Class.Name} HP: ${Me.PctHPs} MANA: ${Me.PctMana} Endurance: ${Me.PctEndurance}" TRUE 2
				/return FALSE
			}
			/if (${nArray} && (${Group.Member[${nArray}].PctHPs} < ${classMinHP} || (${Group.Member[${nArray}].Class.CanCast} && ${Group.Member[${nArray}].PctMana} < ${classMinMana}) || (!${Group.Member[${nArray}].Class.CanCast} && ${Group.Member[${nArray}].PctEndurance} < ${classMinEnd}))) {
				/if (${arrayType.Equal[MedTo]} && !${pullMedToMsgTimer}) HIGHERDEBUG ".....${Group.Member[${nArray}].CleanName} does not meat minimum conditions to pull CLASS: ${Group.Member[${nArray}].Class.Name} HP: ${Group.Member[${nArray}].PctHPs} MANA: ${Group.Member[${nArray}].PctMana} Endurance: ${Group.Member[${nArray}].PctEndurance}" TRUE 2
				/return FALSE
			}
			/if (${Group.Member[${nArray}].Class.HealerType}) /varcalc healerFound ${healerFound} + 1
		} else {
			/if (${Group.Member[${nArray}].DisplayName.Equal[${mainTank}]}) {
				/if (${arrayType.Equal[MedTo]} && !${pullMedToMsgTimer}) HIGHERDEBUG ".....${Group.Member[${nArray}].CleanName} is designated as the main tank and not present, not pulling." TRUE 2
				/return FALSE
			}
		}
	/next nArray
	
	/if (${watchCount}) {
		/for nArray 1 to ${watchCount}
			/if (!${Group.Member[${watchList[${nArray}]}].ID} && ${Spawn[=${watchList[${nArray}]}].ID}) {
				/if (${useEQBC} && ${Spawn[${watchList[${nArray}]}].Type.Equal[PC]} && ${NetBots[${watchList[${nArray}]}]} && ${NetBots[${watchList[${nArray}]}].Zone}==${Zone.ID} && ${Plugin[MQ2NetBots].Version} && ${NetBots.Listen}) {
					/varset classMinMana ${Ini[${iniName},Pull Settings,${NetBots[${watchList[${nArray}]}].Class.ShortName}${arrayType}Mana,${default${arrayType}Mana}]}
					/varset classMinEnd ${Ini[${iniName},Pull Settings,${NetBots[${watchList[${nArray}]}].Class.ShortName}${arrayType}End,${default${arrayType}End}]}
					/varset classMinHP ${Ini[${iniName},Pull Settings,${NetBots[${watchList[${nArray}]}].Class.ShortName}${arrayType}HP,${default${arrayType}HP}]}
					
					| NetBots PctEndurance always returns 0 can't do endurance checks....
					/if ((${NetBots[${watchList[${nArray}]}].Class.CanCast} && ${NetBots[${watchList[${nArray}]}].PctMana} < ${classMinMana}) || ${NetBots[${watchList[${nArray}]}].PctHPs} < ${classMinHP}) {
						/if (${arrayType.Equal[MedTo]} && !${pullMedToMsgTimer}) HIGHERDEBUG ".....${watchList[${nArray}]} does not meat minimum conditions to pull CLASS: ${NetBots[${watchList[${nArray}]}].Class.Name} HP: ${NetBots[${watchList[${nArray}]}].PctHPs} MANA: ${NetBots[${watchList[${nArray}]}].PctMana} Endurance: ${NetBots[${watchList[${nArray}]}].PctEndurance}" TRUE 2
						/return FALSE
					}
				} else {
					/varset classMinMana ${Ini[${iniName},Pull Settings,${Spawn[=${watchList[${nArray}]}].Class.ShortName}${arrayType}Mana,${default${arrayType}Mana}]}
					/varset classMinEnd ${Ini[${iniName},Pull Settings,${Spawn[=${watchList[${nArray}]}].Class.ShortName}${arrayType}End,${default${arrayType}End}]}
					/varset classMinHP ${Ini[${iniName},Pull Settings,${Spawn[=${watchList[${nArray}]}].Class.ShortName}${arrayType}HP,${default${arrayType}HP}]}

					| Can't check mana or end of non group members without netbots, only check hitpoints					
					/if (${Spawn[=${watchList[${nArray}]}].PctHPs} < ${classMinHP}) {
						/if (${arrayType.Equal[MedTo]} && !${pullMedToMsgTimer}) HIGHERDEBUG ".....${watchList[${nArray}]} does not meat minimum conditions to pull CLASS: ${Spawn[=${watchList[${nArray}]}].Class.Name} HP: ${Spawn[=${watchList[${nArray}]}].PctHPs} MANA: -- Endurance: --" TRUE 2
						/return FALSE
					}
				}
				/if (${Spawn[=${watchList[${nArray}]}].Class.HealerType}) /varcalc healerFound ${healerFound} + 1
			} else /if (!${Spawn[=${watchList[${nArray}]}].ID}) {
				/if (${watchList[${nArray}].Equal[${mainTank}]}) {
					/if (${arrayType.Equal[MedTo]} && !${pullMedToMsgTimer}) HIGHERDEBUG ".....${watchList[${nArray}]} is designated as the main tank and not present, not pulling." TRUE 2
					/return FALSE
				}
			}
		/next nArray
	}
	
	/if (${healerFound} < ${requireHealer}) {
		/if (!${healerMsgTimer}) {
			/call EchoLog "\ayNot enough living healers found in group or watchlist, cannot pull.  Required Healers: ${requireHealer} Found Healers: ${healerFound}  If needed you can change this setting [Pull Settings] RequiredHealers= to the number of healers required to pull (0 for none)"
			/varset healerMsgTimer 30s
		}
		/return FALSE
	}	
/return TRUE

Sub PullCheckLocation(float cDist)
	/if (${cDist} == NULL || ${cDist} == 0) /varset cDist ${pullRadius}
	/declare mobID							int local 0
	/declare sNum								int local 0
	/declare nearestPC					int local 0
	/declare nNum								int local 0
	
	:checkNextSpawn
	/varset mobID 0
	/varcalc sNum ${sNum} + 1
	/varset mobID ${Me.NearestSpawn[${sNum},npc Range ${pullMinLevel} ${pullMaxLevel} radius ${cDist} ${If[${useNavigation},,los ]}noalert ${alertList} alert ${pullTargetAlert}].ID}
	/if (${mobID}) {
		/if (${Spawn[${mobID}].DistanceZ} > ${pullZRadius}) /goto :checkNextSpawn
		/varset nNum 0
		:nextNearestPC
			/varcalc nNum ${nNum} + 1
			/varset nearestPC 0
			/varset nearestPC ${Spawn[${mobID}].NearestSpawn[${nNum},pc radius 50].ID}
			/if (${nearestPC} && ${Math.Distance[${Spawn[${mobID}].Y},${Spawn[${mobID}].X},${Spawn[${mobID}].Z}:${Spawn[${nearestPC}].Y},${Spawn[${nearestPC}].X},${Spawn[${nearestPC}].Z}]} < 50) {
				/call CheckFriendList ${nearestPC}
				/if (${Macro.Return.Equal[TRUE]}) /goto :nextNearestPC
				/if (${Macro.Return.Equal[FALSE]}) /goto :checkNextSpawn
			}
	}
	/if (!${mobID} && !${pullMsgTimer}) {
		HIGHERDEBUG "No mobs within range that meet specifications to pull: npc Range ${pullMinLevel} ${pullMaxLevel} radius ${cDist} ${If[${useNavigation},,los ]}noalert ${alertList}" TRUE 2
		/varset pullMsgTimer 30s
	}
/return ${mobID}

Sub PullCombatCheck
	/if (${pullCombatTimer}) /return
	HIGHERDEBUG "Starting PullCombatCheck" TRUE	4
	/varset pullCombatTimer 10s
	
	/call PullCheckGroup
	/if (${Macro.Return.Equal[FALSE]}) {
		/if (!${pullHoldMedding}) /varset pullHoldMedding TRUE
		/return
	} else {
		/varset pullHoldMedding FALSE
	}
	/call PullCheckBuffs
	/if (${Macro.Return.Equal[FALSE]}) /return
	
	/if (!${pullHoldMedding} && ${lTargCount} <= ${pullCampMobs} && ${mainAssist.NotEqual[${Me.CleanName}]} && ${mainTank.NotEqual[${Me.CleanName}]}) {
		/call GetMATarget
		/if (${Target.ID}!=${maTargetID}) {
			/assist ${mainAssist}
			/delay 2s ${Target.ID}==${maTargetID}
		}
		/if (${Target.ID}==${maTargetID} && ${Target.PctHPs} <= ${pullMobHP}) {
			/if (${Me.Combat}) /attack off
			/if (${Stick.Active}) /squelch /stick off
			HIGHERDEBUG "Starting pull routine from PullCombatCheck routine" TRUE 3
			/call PullRoutine
		}
	}
/return

Sub PullLoadPath(string pullLoadPathName, bool changeMode)
	/if (${pullLoadPathName.Equal[NULL]}) /return FALSE
	/if (${changeMode} == NULL) /varset changeMode TRUE
	
	/call EchoLog "Attempting to load pull path ${pullLoadPathName}..." TRUE
	/if (${pullLoadPathName.Equal[default]}) /varset pullLoadPathName ${Zone.ShortName}
	/call LoadPath ${pullLoadPathName}
	| /call ReadINIA Path.ini ${pullLoadPathName}
	/if (${Macro.Return.Equal[FALSE]}) {
		/call EchoLog "\ayUnable to load pull path ${pullLoadPathName}${If[${changeMode}, will not be pulling,]}."
		/if (${changeMode}) /varset pullMode FALSE
		/return FALSE
	} else {
		/call EchoLog "Pull path ${pullLoadPathName} loaded successfully" TRUE
	}
	/call SetNearestLoc TRUE
/return TRUE

Sub PullMob(int pullID)
	/if (${pullID} == NULL || !${pullID} || !${Spawn[${pullID}].ID}) /return FALSE
	/declare lullTarg						int local 0
	/declare lullCount					int local 0
	/declare lArray							int local 0
	/declare maxPRange 					float local ${maxTargetRange}
	/declare maxLRange					float local ${maxPRange}
	/declare retValue						string local NULL
	/declare lullNum						int local 0
	/declare lastLullTarg				int local 0
	/declare pullSpellTimeout		timer local 0s
	/declare aggroAttempted			bool local FALSE
	/declare aggroInterrupted		bool local FALSE
	/declare aggroTimeout				timer local 0s
	
	/varset gotAggro FALSE

	/if (${pullWith.NotEqual[NULL]}) {
		/if (!${Select[${pullType},ITEM,DISC,ALT,RANGE,MELEE,PET]} && !${pullType.Find[Gem]}) {
			/call EchoLog "\ayPullType is currently set to ${pullType} which is not a valid setting (ITEM,DISC,ALT,RANGE,MELEE,PET,GEMx).  Defaulting to MELEE."
			/varset pullType MELEE
		}
		/if (${pullType.Equal[Item]} && ${FindItem[${pullWith}].Spell.Range} < ${maxPRange}) /varcalc maxPRange ${FindItem[${pullWith}].Spell.Range} - 20
		/if (${pullType.Equal[DISC]} && ${Me.CombatAbility[${Me.CombatAbility[${pullWith}]}].Range} < ${maxPRange}) /varcalc maxPRange ${Me.CombatAbility[${Me.CombatAbility[${pullWith}]}].Range} - 20
		/if (${pullType.Find[Gem]} && ${Spell[${pullWith}].Range} < ${maxPRange}) /varcalc maxPRange ${Spell[${pullWith}].Range} - 20
		/if (${pullType.Find[alt]} && ${Me.AltAbility[${pullWith}].Spell.Range} < ${maxPRange}) /varcalc maxPRange ${Me.AltAbility[${pullWith}].Spell.Range} - 20
		/if (${pullType.Equal[Range]} && ${FindItem[${pullWith}].Range} < ${maxPRange}) /varcalc maxPRange ${FindItem[${pullWith}].Range} - 20
		/if (${pullType.Equal[PET]} && ${Me.Pet.ID} && !${Me.Pet.Name.Find[familiar]}) /varset maxPRange 200
	} 
	/varset maxLRange ${maxPRange}
	/if (${lullSpell.NotEqual[NULL]}) {
		/if (${lullSlot.Equal[Item]} && ${FindItem[${lullSpell}].Spell.Range} < ${maxLRange}) /varcalc maxLRange ${FindItem[${lullSpell}].Spell.Range} - 20
		/if (${lullSlot.Equal[DISC]} && ${Me.CombatAbility[${Me.CombatAbility[${lullSpell}]}].Range} < ${maxLRange}) /varcalc maxLRange ${Me.CombatAbility[${Me.CombatAbility[${lullSpell}]}].Range} - 20
		/if (${lullSlot.Find[Gem]} && ${Spell[${lullSpell}].Range} < ${maxLRange}) /varcalc maxLRange ${Spell[${lullSpell}].Range} - 20
		/if (${lullSlot.Equal[alt]} && ${Me.AltAbility[${lullSpell}].Spell.Range} < ${maxLRange}) /varcalc maxLRange ${Me.AltAbility[${lullSpell}].Spell.Range} < ${maxLRange} - 20
	}
	
	| /echo >> ${pullWith} : ${maxPRange} - ${lullSpell} : ${maxLRange} | ${Spawn[${pullID}].Distance}
	/if ((${Spawn[${pullID}].Distance} > ${maxPRange} && ${pullWith.NotEqual[fartaunt]}) || !${Spawn[${pullID}].LineOfSight}) {
		/call MoveToSpawn ${pullID} ${maxPRange} ${Me.CombatState.Equal[Combat]} FALSE TRUE
	}
	/call PullAggroCheck
	HIGHERDEBUG "PrePull Aggro: ${gotAggro}" TRUE	3
	/if (!${gotAggro}) {
		/if (${Me.Casting.ID} && ${Me.Class.ShortName.Equal[BRD]}) {
			/melody
			/stop
			/if (${Twist.Twisting}) /squelch /twist clear
			/delay 5s !${Me.Casting.ID}
		}
				
		/varset lullCount ${SpawnCount[npc radius ${lullRadius} loc ${Spawn[${pullID}].X} ${Spawn[${pullID}].Y} NOTID ${pullID} noalert ${alertList}]}
		/if (${lullCount} && ${lullSpell.NotEqual[NULL]}) {
			/varset pullState LULLING
			/call EchoLog "There are ${lullCount} potential add(s)." TRUE
			/varset lullNum 0
			:nextLullTarget
				/varset lastLullTarg ${lullTarg}
				/varcalc lullNum ${lullNum} + 1
				/varset lullTarg ${Spawn[${pullID}].NearestSpawn[${lullNum},npc radius ${lullRadius} notid ${pullID} noalert ${alertList}].ID}
				/if (${lullTarg} && ${lullTarg}!=${lastLullTarg} && ${Math.Distance[${Spawn[${lullTarg}].Y},${Spawn[${lullTarg}].X},${Spawn[${lullTarg}].Z}:${Spawn[${pullID}].Y},${Spawn[${pullID}].X},${Spawn[${pullID}].Z}]} > ${lullRadius}) /goto :nextLullTarget
				/if (${lullTarg} && ${lullTarg}!=${lastLullTarg} && ${Spawn[${lullTarg}].Distance3D} > ${Math.Calc[${pullRadius} + ${lullRadius}]}) /goto :nextLullTarget
				/if (${lullTarg}==${lastLullTarg}) /varset lullTarg 0
				/if (${lullTarg}) {
					/squelch /target ID ${lullTarg}
					/delay 1s ${Target.ID}==${lullTarg}
					/call PullAggroCheck
					/if (${gotAggro}) /return TRUE
					/if (${Target.ID} && ${Target.ID}==${lullTarg}) {
						/call EchoLog "Attempting to lull potential add, ${Target.CleanName} | ${Target.ID}" TRUE
						:waitLullReady
							/doevents
							/call PullAggroCheck
							/if (${gotAggro}) /goto :skipLull
						/if (${lullSlot.Equal[DISC]} && !${Me.CombatAbilityReady[${lullSpell}]}) /goto :waitLullReady
						
						| Not close enough to lull, move in range first
						/if (${Target.Distance} > ${maxLRange}) /call MoveToSpawn ${Target.ID} ${maxLRange} ${Me.CombatState.Equal[Combat]} FALSE TRUE

						/if (${lullSlot.Equal[DISC]}) {
							/doability "${lullSpell}"
						} else {
							/if (${Me.Class.ShortName.NotEqual[BRD]} || !${lullSlot.Find[Gem]}) {
								/if (${Me.Class.ShortName.Equal[BRD]} && ${Me.Casting.ID}) {
									/stop
									/melody
								}
								/call MQ2Cast "${lullSpell}" ${lullSlot}
							} else {
								/call CastSong "${lullSpell}" ${lullGem} 2 TRUE PullAggroCheck
							}
						}
						/delay 3
					}
					/if (${Me.Class.ShortName.NotEqual[MNK]}) /goto :nextLullTarget
				}
		}
		:skipLull

		/if (${pullType.Equal[ALT]} || ${pullType.Find[Gem]} || ${pullType.Equal[Item]}) /delay 1s !${Me.Moving}
		
		HIGHERDEBUG "PreAttempt Aggro: ${gotAggro}" TRUE 3
		/call EchoLog "Attempting to pull ${Spawn[${pullID}].CleanName} | ${pullID}" TRUE	
		/squelch /target ID ${pullID}
		/delay 2s ${Target.ID}==${pullID}
		/if (${Target.ID} && (${Target.ID}!=${pullID} || (${Target.ID}==${pullID} && ${Target.PctHPs} < 100))) {
			/call EchoLog "${If[${Target.ID}==${pullID},${Spawn[${pullID}].CleanName} | ${pullID} is not at full hitpoints,Unable to target ${Spawn[${pullID}].CleanName} | ${pullID}]}.  Aborting pull."
			/squelch /alert add ${alertList} ID ${pullID}
			/squelch /target clear
			/delay 2s !${Target.ID}
			/return FALSE
		}
		/varset pullState AGGROING		
		/if (${pullBandolier.NotEqual[NULL]} && !${bandolierAtCamp}) {
			/delay 5
			/bandolier activate ${pullBandolier}
			/delay 5
			/if (${pullType.Equal[Range]} && ${Me.Inventory[ranged].Range} && ${Target.Distance} > ${Me.Inventory[ranged].Range}) /call MoveToSpawn ${Target.ID} ${Math.Calc[${Me.Inventory[ranged].Range} - 10]} ${Me.CombatState.Equal[Combat]}
		}
		/if (${pullType.Equal[DISC]}) {
			/doability "${pullWith}"
			/delay ${Me.CombatAbility[${Me.CombatAbility[${pullWith}]}].MyCastTime.Float}s
			/delay 2
			/varset aggroAttempted TRUE
		} else /if (${pullType.Equal[PET]} && ${Me.Pet.ID} && !${Me.Pet.Name.Find[familiar]}) {
			/pet attack
			:waitForPet
			| should add a max pet wait time here
				/delay 1s
			/if (!${Target.Aggressive}) /goto :waitForPet
			/if (${Target.Aggressive}) {
				/pet back off
				/varset aggroAttempted TRUE
			}
		} else /if (${pullType.Equal[Range]} && ${Target.Distance} > 20) {
			/face fast nolook
			/delay 1
			/autofire on
			/delay 3
			/autofire off
			/varset aggroAttempted TRUE
		} else /if (${pullType.Equal[melee]} || (${pullType.Equal[Range]} && ${Target.Distance} <= 20)) {
			/if (${pullType.Equal[Range]} && ${Target.Distance} <= 20) /call EchoLog "PullType is set to Range but we are to close to the mob, using melee instead" TRUE 1
			/attack on
			/call MoveToSpawn ${Target.ID} 8 ${Me.CombatState.Equal[Combat]}
			/if (${Target.Moving}) /squelch /stick 10 hold
			/if (!${Defined[emuMode]} || !${emuMode}) {
				/delay 2s ${Target.Aggressive}
				/if (${returnToCamp} || !${Target.Aggressive}) /attack off
			} else {
				/delay 5
				/if (${returnToCamp}) /attack off
			}
			/squelch /stick off
			/varset aggroAttempted TRUE
		} else {
			/if (${Me.Class.ShortName.Equal[BRD]} && ${pullType.Find[Gem]}) {
				HIGHERDEBUG "Attempting to pull with song ${pullWith}, Gem ${pullGem}" TRUE 2
				/call CastSong "${pullWith}" ${pullGem} 1 TRUE PullAggroCheck
			} else {
				/if (${Me.Class.ShortName.Equal[BRD]} && ${Me.Casting.ID}) {
					/stop
					/melody
				}
				/varset pullSpellTimeout 5s
				:pullWaitForSpell
				/if ((${pullType.Find[gem]} && !${Me.SpellReady[${pullWith}]}) || (${pullType.Equal[ALT]} && !${Me.AltAbilityReady[${pullWith}]})) {
					/call PullAggroCheck
					/if (!${gotAggro} && ${pullSpellTimeout}) /goto :pullWaitForSpell
				}
				/if ((${pullType.Find[gem]} && ${Me.SpellReady[${pullWith}]}) || (${pullType.Equal[ALT]} && ${Me.AltAbilityReady[${pullWith}]})) {
					HIGHERDEBUG "Attempting to pull with ${pullType} ${pullWith}" TRUE 2
					/varset aggroAttempted TRUE
					/call MQ2Cast "${pullWith}" ${pullType} 0 PullAggroCheck
					/if (!${Select[${CastResult},CAST_SUCCESS,CAST_TAKEHOLD,CAST_RESIST]}) /varset aggroInterrupted TRUE
				} else {
					HIGHERDEBUG "Unable to pull with ${pullType} ${pullWith}, it is not ready." TRUE 2
				}
			}
			/if (${CastResult.Equal[CAST_CANNOTSEE]}) {
				HIGHERDEBUG "Cannot see error, adding ${Target.ID} to alert ${alertList}" TRUE 2
				/squelch /alert add ${alertList} NPC ID ${Target.ID}
				/squelch /target clear
			}
		}
		HIGHERDEBUG "PostAttempt Aggro: ${gotAggro}" TRUE	3	
		/if (!${gotAggro} && ${Me.CombatState.NotEqual[Combat]}) /delay 2s ${Me.CombatState.Equal[Combat]}
		/varset aggroTimeout 2s
		:postPullAggroCheck
			/call PullAggroCheck
		/if (!${gotAggro} && ${aggroAttempted} && !${aggroInterrupted} && ${aggroTimeout}) /goto :postPullAggroCheck
	}
	/if (${gotAggro}) {
		/if (${pullBandolier.NotEqual[NULL]} && ${normalBandolier.NotEqual[NULL]} && !${bandolierAtCamp}) {
			/delay 3
			/bandolier activate ${normalBandolier}
			/delay 3
		}			
	}
	HIGHERDEBUG "PostPull Aggro: ${gotAggro}" TRUE 3
/return ${gotAggro}

Sub PullRoutine
	/if (!${pullMode} || !${allowPullMode}) /return PULL_MODE_NOT_ALLOWED
	/if (${pullTimer}) /return PULL_FAIL_TIMER
	/if (${Zone.ID}!=${pathLoadZone}) /call PullBuildPathList
	/call PullCheckGroup
	/if (${Macro.Return.Equal[FALSE]}) {
		/if (!${pullHoldMedding}) /varset pullHoldMedding TRUE
		/if (!${pullMedToMsgTimer}) {
			HIGHERDEBUG ".....The group is not ready for pulls yet.  Waiting for them to be ready before pulling." TRUE 1
			/varset pullMedToMsgTimer 30s
		}
		/return PULL_FAIL_GROUP
	} else {
		/varset pullHoldMedding FALSE
		/varset pullMedToMsgTimer 0s
	}
	/call PullCheckBuffs
	/if (${Macro.Return.Equal[FALSE]}) /return MISSING_REQUIRED_BUFF
	/declare pArray							int local
	/declare moveVar						int local 1
	/declare pullReturn					bool local FALSE
	| /declare usePullPath				bool local TRUE
	/declare pgArray						int local
	/declare revDir							bool local FALSE
	/declare noMobAtEnd					bool local FALSE
	/declare pullMobReturn			bool local FALSE
	/declare pullCheckID				int local 0
	/declare mobPullAttempt			int local 0
	/declare unsafeReturn				bool local FALSE
	/declare fastFace						bool local FALSE
	/if (${pullBandolier.NotEqual[NULL]} && ${bandolierAtCamp}) {
		/delay 5
		/bandolier activate ${pullBandolier}
		/delay 1s
		| /if (${pullType.Equal[Range]} && ${Me.Inventory[ranged].Range} && ${Target.Distance} > ${Me.Inventory[ranged].Range}) /call MoveToSpawn ${Target.ID} ${Math.Calc[${Me.Inventory[ranged].Range} - 10]} ${Me.CombatState.Equal[Combat]}
	}

	/if (${pullMinLevelLoad.Left[1].Equal[-]} || ${pullMinLevelLoad.Left[1].Equal[+]}) {
		/varset pullMinLevel ${Math.Calc[${Me.Level} ${pullMinLevelLoad}].Int}
	} else {
		/varset pullMinLevel ${pullMinLevelLoad}
	}
	/if (${pullMinLevel} < 1) /varset pullMinLevel 1
	/if (${pullMaxLevelLoad.Left[1].Equal[-]} || ${pullMaxLevelLoad.Left[1].Equal[+]}) {
		/varset pullMaxLevel ${Math.Calc[${Me.Level} ${pullMaxLevelLoad}].Int}
	} else {
		/varset pullMaxLevel ${pullMaxLevelLoad}
	}
	
	/call SetNearestLoc
	/if (${usePullPath} && (${Math.Distance[${LocArray[${curLoc}]}]} > 100 || ${pullPath.Equal[NULL]} || ${maxLoc} < 2 || (${pullPathMaxTimes[${pullPathNum}].NotEqual[0]} && !${pullPathTimer}))) {
		/if (!${multiPaths}) {
			/varset usePullPath FALSE
		} else {
			/declare initPullPathNum			int local ${pullPathNum}
			:nextInitialPath
				/varcalc pullPathNum ${pullPathNum} + 1
				/if (${pullPathNum}>20) /varset pullPathNum 1
				/if (${pullPathNum}==${initPullPathNum}) {
					/call EchoLog "All pull paths are too far away, switching to pulling within the pull radius ${pullRadius}"
					/varset usePullPath FALSE
				}
				/if (${usePullPath}) {
					/if (${pullPaths[${pullPathNum}].Equal[NULL]}) /goto :nextInitialPath
					/call PullLoadPath ${pullPaths[${pullPathNum}]} FALSE
					/if (${Math.Distance[${LocArray[${curLoc}]}]} > 100 || ${pullPath.Equal[NULL]} || ${maxLoc} < 2) /goto :nextInitialPath
					/if (!${maxLoc}) /goto :nextInitialPath
					/if (${pullPathMaxTimes[${pullPathNum}].NotEqual[0]}) /varset pullPathTimer ${pullPathMaxTimes[${pullPathNum}]}
				}
		}
	}

	/varset pullState HUNTING		
	:pullPathLoop
		/doevents
		/if (${unsafePCPullCheck} && !${unsafeReturn}) {
			| This one calls and only finds out if the camp is safe or not, takes not action
			/call campCheck TRUE
			/if (${Macro.Return.Equal[FALSE]}) {
				/if (${unsafePCReturn}) {
					/varset unsafeReturn TRUE
					/if (!${pullReturn}) {
						/if (${returnToCamp}) {
							HIGHERDEBUG "Unsafe PC detected, returning to camp" TRUE 1
							/varset pullReturn TRUE
							/varset pullState RETURNING
							/if (!${revDir} && ${usePullPath}) /varcalc moveVar ${moveVar} * -1
						} else {
							HIGHERDEBUG "Unsafe PC detected, set to not return home so setting camp location to here" TRUE 1
							/call SetHome
							| /call PullWait
							/varset pullState NULL
							/return
						}
					}
				} else {
					| If the camp is unsafe, and UnsafePCCampReturn=FALSE, this one will take the specified unsafepcation where it is
					| If the action is set to continue, then it will still proceed with returning to camp.
					/call campCheck
					/if (${Macro.Return.Equal[FALSE]}) {
						/varset unsafeReturn TRUE
						/if (!${pullReturn}) {
							/if (${returnToCamp}) {
								HIGHERDEBUG "Unsafe PC detected, returning to camp" TRUE 1
								/varset pullReturn TRUE
								/varset pullState RETURNING
								/if (!${revDir} && ${usePullPath}) /varcalc moveVar ${moveVar} * -1
							} else {
								HIGHERDEBUG "Unsafe PC detected, set to not return home so setting camp location to here" TRUE 1
								/call SetHome
								| /call PullWait
								/varset pullState NULL
								/return
							}
						}						
					}
				}
			}
		}
		/if (!${pullReturn}) {
			/varset pullState HUNTING		
			HIGHERDEBUG "Checking for a mob to pull within a radius of ${pullRadius} from X: ${Me.X} Y: ${Me.Y}" TRUE	2
			:pullLocationCheck
			/call PullCheckLocation
			/varset pullCheckID ${Macro.Return}
			HIGHERDEBUG "Location check returned ${Macro.Return}" TRUE 3
			:pullAttempt
			/if (${pullCheckID} && ${Spawn[ID ${pullCheckID}].ID} && ${Spawn[ID ${pullCheckID}].Type.NotEqual[CORPSE]}) {
				/if (${Me.Sitting}) /stand
				/call PullMob ${pullCheckID}
				/varset pullMobReturn ${Macro.Return}
				HIGHERDEBUG "PullMob returned ${pullMobReturn}" TRUE 3
				/if (${pullMobReturn}) {
					/call Announce "${incMessage}"
					/if (${returnToCamp}) {
						HIGHERDEBUG "Setting pullReturn to TRUE" TRUE	4
						/varset pullReturn TRUE
						/if (${fastFaceOnAggro}) /varset fastFace TRUE
						/varset pullState RETURNING
						/if (!${revDir} && ${usePullPath}) /varcalc moveVar ${moveVar} * -1
					} else {
						HIGHERDEBUG "Return to camp: ${returnToCamp} Calling SetHome then waiting" TRUE 3
						/call SetHome
						/call PullWait
						/return
					}
				} else /if (!${pullMobReturn} && !${usePullPath}) {
					/varcalc mobPullAttempt ${mobPullAttempt} + 1
					/if (${mobPullAttempt} < 3) {
						HIGHERDEBUG ".....The mob with ID ${pullCheckID} failed to aggro when attempting to pull it again" TRUE 1
						/if (${pullCheckID} && ${Spawn[${pullCheckID}].ID}) {
							/goto :pullAttempt
						} else {
							/if (!${usePullPath} && ${returnToCamp}) {
								HIGHERDEBUG ".....The mob we were attempting to pull with ID ${pullCheckID} has disappeared.  Attempting to find another nearby mob" TRUE 1
								/call PullCheckLocation ${Math.Calc[${pullRadius} / 4]}
								/varset pullCheckID ${Macro.Return}
								/if (${pullCheckID}) {
									/varset mobPullAttempt 0
									/goto :pullAttempt
								} else {
									HIGHERDEBUG ".....Failed to find anymore nearby mobs, returning to camp." TRUE 1
									/varset pullReturn TRUE
									/varset pullState RETURNING
								}
							} else {
								/varset mobPullAttempt 0
								/goto :pullLocationCheck
							}
						}
					} else {
						/call EchoLog ".....Failed to aggro the mob with ID ${pullCheckID} three times.  Adding to the ignore list." TRUE
						/squelch /alert add ${alertList} NPC ID ${pullCheckID}
						/squelch /target clear
						/if (${returnToCamp}) {
							HIGHERDEBUG "Setting pullReturn to TRUE" TRUE 4
							/varset pullReturn TRUE
							/varset pullState RETURNING
							/if (!${revDir} && ${usePullPath}) /varcalc moveVar ${moveVar} * -1
						} else {
							HIGHERDEBUG "Return to camp: ${returnToCamp} Calling SetHome then moving on" TRUE 3
							/call SetHome
							/return
						}
					}
				}
			} else /if (!${usePullPath}) {
				/if (${pullReturn} && ${Math.Distance[${homeY},${homeX}]} > ${campRadius}) {
					/varset pullState RETURNING
					HIGHERDEBUG "Returning to home location ${homeY} ${homeX} InCombat: ${inCombat} - ${Me.CombatState}" TRUE 2
					/call MoveToLoc ${homeY} ${homeX} TRUE
					HIGHERDEBUG "Move to Loc completed at ${Me.Y} ${Me.X} Return: ${Macro.Return}" 3
				}
				/varset pullState NULL
				/return PULL_FAIL_NO_MOBS
			}
		}
		/if (${usePullPath}) {
			/varcalc curLoc ${curLoc} + ${moveVar}
			HIGHERDEBUG "Using pull path moving to loc ${curLoc} delta ${moveVar}" TRUE 4
			/if (${curLoc} > ${maxLoc}) {
				/if (${circularPath}) {
					/varset pullTimer ${circuitRestTime}
					/if (${multiPaths}) {
						:nextPathCircular
							/varcalc pullPathNum ${pullPathNum} + 1
							/if (${pullPathNum}>20) /varset pullPathNum 1
							/if (${pullPaths[${pullPathNum}].Equal[NULL]}) /goto :nextPathCircular
							/call PullLoadPath ${pullPaths[${pullPathNum}]} FALSE
							/if (!${maxLoc}) /goto :nextPathCircular
					}
					/return
				} else {
					/varset revDir TRUE
					/varcalc moveVar ${moveVar} * -1
					/varcalc curLoc ${maxLoc} - 1
					/if (!${pullReturn}) /varset noMobAtEnd TRUE
				}
			} else /if (${curLoc} < 1) {
				/if (${pullReturn}) {
					/if (${Math.Distance[${homeY},${homeX}]} > 15) {
						HIGHERDEBUG "Returning to home location from path ${homeY} ${homeX} InCombat: ${inCombat} - ${Me.CombatState}" TRUE 2
						/call MoveToLoc ${homeY} ${homeX} TRUE
					}
					/if (${noMobAtEnd} && ${multiPaths}) {
						:nextPathNoMob
							/varcalc pullPathNum ${pullPathNum} + 1
							/if (${pullPathNum}>20) /varset pullPathNum 1
							/if (${pullPaths[${pullPathNum}].Equal[NULL]}) /goto :nextPathNoMob
							/call PullLoadPath ${pullPaths[${pullPathNum}]} FALSE
							/if (!${maxLoc}) /goto :nextPathNoMob							
					}
					/if (!${unsafeReturn}) {
						/call PullWait
					} else {
						/varset pullState NULL
					}
				} else {
					/varset pullTimer ${circuitRestTime}
					/if (${multiPaths}) {
						:nextPath
							/varcalc pullPathNum ${pullPathNum} + 1
							/if (${pullPathNum}>20) /varset pullPathNum 1
							/if (${pullPaths[${pullPathNum}].Equal[NULL]}) /goto :nextPath
							/call PullLoadPath ${pullPaths[${pullPathNum}]} FALSE
							/if (!${maxLoc}) /goto :nextPath
					}
					/return PULL_FAIL_NO_MOBS
				}
				/return	
			}
			/call Movement ${curLoc} ${curLoc} ${Me.CombatState.Equal[Combat]} FALSE ${fastFace}
			/varset fastFace FALSE
			/goto :pullPathLoop
			:exitPullPath
		} else /if (${pullReturn}) {
			/declare faceAtCampPre	bool local ${faceAtCamp}
			/if (${Math.Distance[${homeY},${homeX}]} > ${campRadius}) {
				/varset pullState RETURNING
				HIGHERDEBUG "Returning to home location ${homeY} ${homeX} InCombat: ${inCombat} - ${Me.CombatState}" TRUE 2
				/call MoveToLoc ${homeY} ${homeX} TRUE
				HIGHERDEBUG "Move to Loc completed at ${Me.Y} ${Me.X} Return: ${Macro.Return}" 3
			} else {
				/varset faceAtCamp FALSE
			}
			/if (!${unsafeReturn}) {
				/call PullWait
			} else {
				/varset pullState NULL
			}
			/varset faceAtCamp ${faceAtCampPre}
		} else /if (${Spawn[ID ${pullCheckID}].ID} && ${Spawn[ID ${pullCheckID}].Type.Equal[CORPSE]}) {
			/call EchoLog "Pull target has died somehow." TRUE 0
			/return PULL_FAIL_TARGET_DEAD
		}
/return

Sub PullWait
	/declare pullTimeoutTimer					timer local 0s
	/declare currID										int local 0
	/declare currDir									float local 0

	HIGHERDEBUG "PullWait routine started, pullTimeoutTimer: ${pullTimeOut}" TRUE 2
	/if (${faceAtCamp}) /face Heading ${If[${Me.Heading.DegreesCCW}<180,${Math.Calc[${Me.Heading.DegreesCCW} + 180]},${Math.Calc[${Me.Heading.DegreesCCW} - 180]}]}
	/if (${pullBandolier.NotEqual[NULL]} && ${normalBandolier.NotEqual[NULL]} && ${bandolierAtCamp}) {
		/delay 5
		/bandolier activate ${normalBandolier}
		/delay 5
	}
	/varset pullTimeoutTimer ${pullTimeOut}
	/varset pullState WAITING
	:pullWait
		/varset sitTimer 5s
		/doevents
		/if (${checkForAdds}) {
			/call CheckForAdds ${campRadius} ${Me.ID} FALSE TRUE
			/if (${lTargCount} > 0) {
				/if (${fadeOnPull} && ${Me.Class.ShortName.Equal[BRD]} && ${Me.CurrentMana} > 800) {
					/if (${Me.Casting.ID}) {
						/melody
						/stop
						/if (${Twist.Twisting}) /squelch /twist clear
						/delay 1s !${Me.Casting.ID}
					}
					/call MQ2Cast "Fading Memories" alt				
				}
				/if (${mainAssist.Equal[${Me.CleanName}]}) /delay 2s ${Math.Distance[${Target.Y},${Target.X}:${homeY},${homeX}]}<${Math.Calc[${campRadius}/2]}
				/varset pullState NULL
				/return
			}
		} else {
			/if (${mainAssist.NotEqual[NULL]} && ${mainAssist.NotEqual[${Me.CleanName}]}) {
				/varset currID ${Target.ID}
				/assist ${mainAssist}
				/delay 1s ${Target.ID}!=${currID}
			} else {
				/varset currID ${Target.ID}
				/squelch /target npc radius ${campRadius} los noalert ${alertList}
				/delay 1s ${Target.ID}!=${currID}
			}
			/if (${Target.ID} && ${Target.Distance} < ${campRadius} && (${Target.Type.Equal[NPC]} || (${Target.Type.Equal[Pet]} && ${Target.Master.Type.NotEqual[PC]}))) {
				/squelch /face fast nolook
				/if (${fadeOnPull} && ${Me.Class.ShortName.Equal[BRD]} && ${Me.CurrentMana} > 800) {
					/if (${Me.Casting.ID}) {
						/melody
						/stop
						/if (${Twist.Twisting}) /squelch /twist clear
						/delay 1s !${Me.Casting.ID}
					}
					/call MQ2Cast "Fading Memories" alt					
				}				
				/varset pullState NULL
				/return
			}
		}
	/if (${pullTimeoutTimer}) /goto :pullWait
	/call EchoLog "\ayAttempt at pulling failed due to time out waiting on mob."
	/squelch /target clear
	/delay 1s !${Target.ID}
	/varset pullState NULL
/return
