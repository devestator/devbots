#define HIGHERDEBUG /devecholog
| ========================================================================================================
| 					 devCommonPremium.inc v2.50
|     					Written By: Devestator
| 
| 
| Description:
|  A compilation of common subs used in multiple macros by Devestator.
|  Must /call CommonInit before using any subroutines in this include.
|  Call CommonLoad after you load all settings for your macro.
| 
| v2.50 Update Notes
|  -Update respite discs
| 
| v2.49 Update Notes
|  -Moved merc assist check higher in the combat routines so that it will assist before debuffing if it should
| 
| v2.48 Update Notes
|  -Added hidden setting [General] CampReturnRatio=.5  You can set this to a decimal representation of the percentage
|   of camp radius a bot can be away from the central camp location, before it returns to it.  The default is .5 which
|   is 50% of the camp radius, meaning if your camp radius is 100, and your bot is more than 50 units away from the center point
|   it will return to camp.  If you set it to say .25, then it will return if you are more than 25 units away.  1.5 will return
|   if you are more than 150 units away.
| 
| v2.47 Update Notes
|  -Updates for MQ2 patch changing spell timers
| 
| v2.46 Update Notes
|  -Added a global event to force standing if "You must be standing to attack!" is detected
| 
| v2.45 Update Notes
|  -Corrected buffing routine bug that could cause self buffs to be skipped when group pets are around and set to be buffed
| 
| v2.44 Update Notes
|  -Added setting [General] EmuMode=FALSE, if set to TRUE the bots will run with some settings purposefully disabled
|   and some slight code differences in some places so that they will work on ROF2 EMU clients
|  -Added auto hiding of corpses after looting (post combat).  This can be disabled with the hidden setting
|    [General] HideCorpses=FALSE
| 
| v2.43 Update Notes
|  -Added Spawn.Aggressive check when checking for non xtarget adds.  This should help eliminate detecting
|    non aggressive mobs as aggro when not using xtarget.
|  -Added hidden setting [General] CastMaxWaitTime=5s, this is the maximum time casting will wait for a spell
|    to cast before giving up (defaults to 5 seconds).
|  
| v2.42 Update Notes
|  -Added common ClearCursor(string clearType,string maxAttemptTime) routine clearType = AUTO or DEST
|  -Switched to devCast.inc instead of using MQ2Cast
|  -Added [General] MinRezAcceptPercent=0 setting.  A setting of 0 (default) will accept any rez.  You can
|   change this to a minimum rez amount you want your characters to accept.
|   (Added by request to combat low percent rez griefing when AFK)
|  -devCommonPremium.dat v1.02 - Added Ancient Flames as a recognized auto buff for melee characters
|  -Updated gift of mana to new short buff text
| 
| v2.41 Update Notes
|  -Added SpellMaxMana to the Combat section.  This is a hidden setting and defaults to 100.
|  -Added SpellMinHPs to the Combat section.  This is a hidden setting and defaults to 0.
|  -Added MissionMode for auto mission running capabilities, currently does not support missions that
|    change your character (example requires you to select a class, or a story character to do the mission)
|    see more info at http://www.devecoder.com/wiki/missionmode
| 
| v2.40 Update Notes
|  -Updated mod rod and health pot usage to use /useitem instead of MQ2Cast to right click the items.
|  -Set MovementInit to rely on devCommonPremium for some shared functions
|  -Fixed a problem with buff reagent checking
|  -Changed it so that if your MaxRezWaitTimer expires while you are hovering, it will release in order to camp
| 
| 
| v2.3x and below update notes can be found at the end of the file
| ========================================================================================================


| Need to write routines to unequip and equip items to remove MQ2Exchange requirement
| Need to figure out an alternative to /stick to remove requirement for MQ2MoveUtils
| MQ2Debuffs is not required (confirm that not having it won't cause problems)
| MQ2CEcho is not required

#turbo 80

|#include devMQ2Cast_Spell_Routines.inc
#include devCast.inc
#include devMovementPremium.inc
#include devLoot.inc
#include devPull.inc
|#include devCommonPremium.dat

#chat Group

#event pullerChange "#1# is now Puller"
#event pullerChange "#1# is no longer Puller"
#event tankChange "#1# is now Main Tank"
#event tankChange "#1# is no longer Main Tank"
#event assistChange "#1# is now Main Assist"
#event assistChange "#1# is no longer Main Assist"
#event newmember "#1# has joined the group."
#event rampage "#*#YOU#*#(Rampage)"
#event died "You have been slain#*#"
#event rTell "#1# tells you, '#2#'"
#event rTell "#1# told you, '#2#'"
#event MQ2Cmd "[MQ2] #1#"
#event summoned "You have been summoned!"
#event spellFade "Your #1# spell has worn off of #2#."
#event enrage "#1# has become ENRAGED."
#event enrageOff "#1# is no longer enraged."
#event bcTell "[#1#(msg)] #2#"
#event bcChatTell "<#1#> #2# #3#"
#event noMerc "You can not unsuspend your mercenary because there are too many players!"
#event noMerc "You can't unsuspend the selected mercenary#*#"
#event mercStanceChange "Your active mercenary stance is now: #1#."
#event mq2navFail "[MQ2Navigation] No Start Reference"
#event membershipFail "Your ability to use#*#has been disabled#*#do not have the membership#*#"
#event standAttack "You must be standing to attack!"

#event PBCastFail "You are not sufficient level to use this item#*#"
#event PBCastFail "Your spell is interrupted#*#"
#event PBCastFail "Your gate is too unstable, and collapses#*#"
#event PBCastFail "You *CANNOT* cast spells, you have been silenced#*#"
#event PBCastFail "You can't cast spells while invulnerable#*#"
#event PBCastFail "Your casting has been interrupted#*#"
#event PBCastFail "This spell does not work here#*#"
#event PBCastFail "You can't cast spells while stunned#*#"
#event PBCastFail "Your spell did not take hold#*#"
#event PBCastFail "You must be standing to cast a spell#*#"
#event PBCastFail "You can only cast this spell in the outdoors#*#"

Sub CommonInit
	/squelch /alias /devecholog /call EchoLog
	/declare devCommonVer								float outer 2.50
	/declare reqMovementCommon						float outer 1.73
	/declare reqCastCommon								float outer 1.04
	/declare reqPullCommon								float outer 1.06
	/squelch /plugin MQ2Debuffs
	/squelch /plugin MQ2Exchange
	/squelch /plugin MQ2CEcho
	/squelch /plugin MQ2MoveUtils
	
	/declare isMMOBugs									bool outer ${Mem}
	
	| =====================================================================
	| Declare supported bot versions
	| =====================================================================	
	/declare bardbot									float outer 1.13
	/declare beastlordbot								float outer 1.01
	/declare berserkerbot								float outer 1.04
	/declare clericbot2									float outer 2.00
	/declare druidbot									float outer 1.04
	/declare enchanterbot								float outer 1.08
	/declare magebot2									float outer 4.00
	/declare monkbot									float outer 1.07
	/declare necrobot									float outer 1.03
	/declare paladinbot									float outer 1.00
	/declare rangerbot									float outer 1.12
	/declare roguebot									float outer 1.00
	/declare shamanbot									float outer 1.05
	/declare skbot2										float outer 2.00
	/declare warriorbot									float outer 1.00
	/declare wizardbot									float outer 1.03
	
	| =====================================================================
	| Declare general variables
	| =====================================================================
	/declare debugMode									bool outer FALSE
	/declare debugLvl									int outer 0
	/declare hPotName									string outer NULL
	/declare hPotHPs									int outer 50
	/declare defaultEchoColor 							string outer \ao
	/declare startZone									string outer ${Zone.ShortName}
	/declare startZoneID								int outer ${Zone.ID}	
	/declare waitingForRez								bool outer FALSE
	/declare nArray										int outer
	/declare lootCorpses								bool outer FALSE
	/declare lootININame								string outer devLoot.ini
	/declare ignoreMob									bool outer
	/declare itemSwapped								bool outer
	/declare itemPrimary								string outer
	/declare itemSecondary								string outer
	/declare useForceTarg								bool outer TRUE  	| what is this?
	/declare allowSit									bool outer TRUE
	/declare replyToTells								bool outer FALSE
	/declare followMode									bool outer FALSE
	/declare followTarg									string outer	
	/declare followTargID								int outer 0
	/declare outOfCombatSit								bool outer TRUE
	/declare inCombatSit								bool outer TRUE
	/declare anyAllowSit								bool outer TRUE
	/declare iniName									string outer
	/declare sitTimer									timer outer 0s
	/declare sitTimerString								string outer
	/declare useHomeDir									bool outer FALSE
	/declare allowInterrupt								bool outer TRUE
	/declare rampageNoSit								bool outer FALSE
	/declare friendList									string outer
	/declare GMAction									string outer GateCamp
	/declare campAfterGate								bool outer TRUE
	/declare checkForAdds								bool outer TRUE	
	/declare colorEcho									bool outer
	/declare useEQBC									bool outer FALSE
	/declare relayChat									bool outer FALSE
	/declare relayTells									bool outer FALSE
	/declare relayTarget								string outer /bct EQBC_Interface
	/declare useMacroEndTimer							bool outer FALSE
	/declare macroEndTimer								timer outer 0s
	/declare macroEndAction								string outer GateCamp
	/declare enrageID									bool outer FALSE
	/declare enrageAttackOff							bool outer FALSE
	/declare faceWhenCasting							bool outer TRUE
	/declare xTargetOnly								bool outer FALSE
	/declare useXTarget									bool outer TRUE
	/declare maxXTargets								int outer 5
	/declare maTargetID									int outer 0
	/declare mtTargetID									int outer 0
	/declare debuffClasses								string outer BRD,SHM,MAG,ENC,SHD,BST,PAL,NEC,DRU,RNG,CLR
	/declare petClasses									string outer SHM,MAG,NEC,ENC,BST,SHD
	/declare meleeClasses								string outer SHD,BRD,BST,PAL,RNG,MNK,ROG,BER,WAR
	/declare mercCheckTimer								timer outer 0s
	/declare respitePct									int outer 0
	/declare TargetNamed								bool outer FALSE
	/declare alwaysAttemptReturn						bool outer FALSE
	/declare engagePet									bool outer FALSE
	/declare lootOnlyKilled								bool outer TRUE
	/declare devCommonPremiumDatVersion					float outer 0.00
	/declare modRodNameCommon							string outer NULL
	/declare modRodMinHPsCommon							int outer 50
	/declare modRodManaCommon							int outer 50
	/declare noInvis									bool outer FALSE
	/declare TRUE										bool outer TRUE
	/declare FALSE										bool outer FALSE
	/declare minRezPercent								string outer 0
	/declare maxRezWaitTime								string outer 0
	/declare trackKills									bool outer FALSE
	/declare killTrack[200,5]							string outer NULL
	/declare membershipFail								bool outer FALSE
	/declare defaultMaxWaitTime						string outer 50
	/declare campReturnRatio							float outer .5
	
	/if (${Plugin[MQ2CEcho].Version}) {
		/squelch /alias /myecho /cecho \aw[MQ2]
		/varset colorEcho TRUE
	} else {
		/squelch /alias /myecho /echo
		/varset colorEcho FALSE
	}
	/if (${isMMOBugs} && ${Me.AltAbility[Eyes Wide Open]}) {
		/varset maxXTargets ${Me.MaxXTarget}
	} else {
		/if (${Me.AltAbility[Eyes Wide Open]}) /varcalc maxXTargets ${maxXTargets} + 1
		/if (${Me.AltAbility[Eyes Wide Open]} > 5) /varcalc maxXTargets ${maxXTargets} + 1
		/if (${Me.AltAbility[Eyes Wide Open]} > 10) /varcalc maxXTargets ${maxXTargets} + 1
		/if (${Me.AltAbility[Eyes Wide Open]} > 20) /varcalc maxXTargets ${maxXTargets} + 1
		/if (${Me.AltAbility[Eyes Wide Open]} > 30) /varcalc maxXTargets ${maxXTargets} + 1
	}
	
	/declare respiteDiscs[4]						string outer
	/varset respiteDiscs[1] Respite
	/varset respiteDiscs[2] Reprieve
	/varset respiteDiscs[3] Rest
	/varset respiteDiscs[4] Breather
	
	| =====================================================================
	|  Merc Variables
	| =====================================================================
	/declare useMerc										bool outer FALSE	
	/declare manageMerc									bool outer FALSE
	/declare mercID											int outer 0
	/declare mercName										string outer NULL
	/declare mercBurnPct								int outer 0
	/declare mercOOCStance							string outer NULL
	/declare mercICStance								string outer NULL
	/declare mercAssistPct							int outer 90
	/declare mercStance									string outer NULL

	| =====================================================================
	|  Command settings variables (Custom commands)
	| =====================================================================
	/declare eqbcCommands								bool outer TRUE
	/declare groupCommands							bool outer TRUE
	/declare tellCommands								bool outer TRUE
	/declare echoCommands								bool outer TRUE
	/declare commandCount								int outer 0
	/declare commandTriggers[100,2]			string outer
	/declare commandActions[100,50]			string outer
	
	| =====================================================================
	| Declare group variables
	| =====================================================================	
	/declare groupMode									bool outer TRUE	
	/declare campRadius									int outer 0
	/declare useGroupPuller							bool outer FALSE
	/declare useGroupTank								bool outer FALSE
	/declare useGroupAssist							bool outer FALSE
	/declare pullerName									string outer
	
	/declare watchEnabled								bool outer TRUE
	/declare watchCount									int outer 0
	/declare watchList[20]							string outer
	/declare buffWatch									bool outer TRUE
	/declare healWatch									bool outer TRUE
	
	| =====================================================================
	| Declare combat variables
	| =====================================================================	
	/declare targArray[100]							int outer
	/declare corpseArray[100]						int outer
	/declare namedCount									int outer 0
	/declare lTargCount									int outer 0
	/declare targCount									int outer 0
	/declare corpseCount								int outer 0
	/declare targID											int outer 0
	/declare newAddID										int outer 0
	/declare newAdd											bool outer FALSE
	/declare newTarg										bool outer FALSE
	/declare inCombat										bool outer FALSE
	/declare gotAggro										bool outer FALSE
	/declare gotAggroID									int outer 0
	/declare mainAssist 								string outer ${Me.CleanName}
	/declare mainAssistOld							string outer NULL
	/declare mainTank										string outer ${Me.CleanName}
	/declare secondaryAssist						string outer NULL
	/declare assistPct									int outer 97	
	/declare groupRole									string outer dps
	/declare minSafeDistance						int outer 25	
	/declare mobRadius									int outer 60
	/declare minMobLvl									int outer 1
	/declare maxMobLvl									int outer 1000
		
	| =====================================================================
	| Declare health and mana variables
	| =====================================================================	
	/declare healPotName								string outer
	/declare healPotHPs									int outer
	/declare healPotTimer								timer outer 0s
	
	| =====================================================================
	| Declare spell variables
	| =====================================================================	
	/declare resistRetryTime						string outer 1m
	/declare noInterrupt								bool outer TRUE
	/declare defaultGem									string outer GEM1	
	/declare resistTries								int outer 3
	/declare globalCooldown							timer outer
	/declare globalCDStr								string outer 4s
	/declare giftSpell									bool outer FALSE
	/declare fopSpell										bool outer FALSE
	/declare fopgiftSpell								bool outer FALSE
	/declare doCombatSpells							bool outer TRUE
	/declare discTimer									timer outer 0s
	/declare csCount										int outer 0
	/declare combatSpells[100]						string outer
	/declare csminMana[100]							int outer
	/declare csmaxMana[100]							int outer
	/declare csminHPs[100]								int outer
	/declare csmaxHPs[100]								int outer
	/declare csrecastDly[100]						string outer
	/declare csmaxMobs[100]							int outer
	/declare csminMobs[100]							int outer
	/declare csSpellSlot[100]						string outer
	/declare csCondition[100]						string outer
	/declare csminSelfHPs[100]						int outer
	/declare csmaxSelfHPs[100]						int outer
	/declare csGiftSpell[100]						bool outer
	/declare csFlameSpell[100]						bool outer
	/declare csType[100]									string outer
	/for nArray 1 to 100
	 	/declare csTimer${nArray}					timer outer
	/next nArray
	
	| =====================================================================
	| Declare debuff variables
	| =====================================================================	
	/declare debuffSpells[20]						string outer
	/declare debuffMinMana[20]					int outer
	/declare debuffMinHPs[20]						int outer
	/declare debuffMaxHPs[20]						int outer
	/declare debuffMaxMobs[20]					int outer
	/declare debuffMinMobs[20]					int outer
	/declare debuffSpellSlot[20]				string outer
	/declare debuffRecastDly[20]				string outer
	/declare debuffAnnounce[20]					string outer
	/declare debuffCount								int outer
	/declare debuffCheckTime						string outer
	/declare debuffCheckTimer						timer outer 0s
	/declare debuffAdds									bool outer FALSE
	/declare doDebuffs									bool outer FALSE
	/declare debuffAlias[20]						string outer
	/declare debuffCOR[20]							string outer
	/declare debuffEnabled[20]					bool outer

	| =====================================================================
	| Declare alert variables
	| =====================================================================		
	/declare alertList									int outer 101	
	/declare mobAlertsValue							int outer 0
	/declare mobAlertsAudio[100]				string outer
	/declare mobAlertsName[100]					string outer
	/declare alertDeath									string outer NULL
	/declare alertEvac									string outer NULL
	/declare alertUnsafePC							string outer NULL
	/declare alertGM										string outer NULL
	/declare textAlertsValue						int outer 0
	/declare textAlertsAudio[100]				string outer
	/declare textAlertsName[100]				string outer
	/declare alertSent[100]							int outer
			
	| =====================================================================
	| Declare ignore mob variables
	| =====================================================================		
	/declare ignoreMobsCount						int outer 0
	/declare ignoreMobs[100]						string outer
	
	| =====================================================================
	| Declare safePC variables
	| =====================================================================	
	/declare safePCCount								int outer 0
	/declare safePC[100]								string outer
	/declare unsafePCAction							string outer pause
	/declare unsafePCRadius							int outer 300
	/declare campCheckTimer							timer outer 0s
	/declare campCheckTimerStr					string outer 1m
	/declare safeAlertList							int outer 102
	
	| =====================================================================
	| Declare buff variables
	| =====================================================================
	/declare doCombatBuffs							bool outer FALSE
	/declare inCombatBuffRoutine				string outer NULL
	/declare selfBuffCount							int outer 0
	/declare buffName[20] 							string outer
	/declare buffText[20]								string outer
	/declare buffSlot[20]								string outer
	/declare buffAuto[20]								bool outer
	/declare buffEnabled[20]						bool outer
	/declare buffAlias[20]							string outer
	/declare buffCombat[20]							bool outer
	/declare buffDNSCount[20]						int outer 0
	/declare buffDoNotStack[20,20]			string outer
	/declare buffSCount[20]							int outer 0
	/declare buffStack[20,20]						string outer
	/declare petBuffCount								int outer 0
	/declare petBuff[20]								string outer
	/declare petBuffText[20]						string outer
	/declare petBuffSlot[20]						string outer
	/declare petBuffEnabled[20]					bool outer
	/declare petBuffAuto[20]						bool outer
	/declare petBuffAlias[20]						string outer
	/declare petBuffCombat[20]					bool outer
	/declare petBuffDNSCount[20]				int outer 0
	/declare petBuffDoNotStack[20,20]		string outer
	/declare petBuffSCount[20]					int outer 0
	/declare petBuffStack[20,20]				string outer
	/declare groupBuffCount							int outer 0
	/declare groupBuffAlias[20]					string outer
	/declare groupBuffSlot[20]					string outer
	/declare groupBuffGroup[20]					string outer
	/declare groupBuffGroupDisplay[20]	string outer
	/declare groupBuffGroupMin[20]			int outer
	/declare groupBuffSingle[20]				string outer
	/declare groupBuffSingleDisplay[20]	string outer
	/declare groupBuffTargets[20]				string outer
	/declare groupBuffAuto[20]					bool outer
	/declare groupBuffSingleAnnounce[20]	string outer
	/declare groupBuffGroupAnnounce[20]	string outer
	/declare groupBuffCombat[20]				bool outer
	/declare groupBuffEnabled[20]				bool outer
	/declare groupBuffDNSCount[20]			int outer 0
	/declare groupBuffDoNotStack[20,20]	string outer
	/declare groupBuffSCount[20]				int outer 0
	/declare groupBuffStack[20,20]			string outer
	/declare combatBuffs								bool outer FALSE
	/declare doGroupBuffs								bool outer TRUE
	/declare doSelfBuffs								bool outer TRUE
	/declare doPetBuffs									bool outer TRUE
	/declare doWatchBuffs								bool outer TRUE
	/declare doAutoBuffs								bool outer TRUE
	/declare buffOtherPets							bool outer TRUE
	/declare minBuffCheckTimerStr				string outer 1m
	/declare minBuffCheckTimer					timer outer 0s	
	/declare buffMode										string outer BUFF_NONE
	/declare doAnnounce									bool outer FALSE
	/declare announceChan								string outer /g
	/declare autoBuff[50]								string outer
	/declare autoBuffText[50]						string outer
	/declare autoBuffAuto[50]						bool outer
	/declare autoBuffEnabled[50]				bool outer
	/declare autoBuffAlias[50]					string outer
	/declare autoBuffType[50]						string outer
	/declare autoBuffCombat[50]					bool outer
	/declare autoBuffSlot[50]						string outer
	/declare autoBuffTargets[50]				string outer
	/declare autoBuffCount							int outer 0
	/declare autoBuffingEnabled					bool outer TRUE
	/declare emuMode										bool outer FALSE
	/declare xTargType									string outer TargetType
	/declare hideCorpses								bool outer TRUE
	
	| =====================================================================
	| Declare SummonedItems variables
	| =====================================================================
	/declare summonedCount							int outer
	/declare summonedPackNum						int outer 0	
	/declare summonedUnfoldedName				string outer NULL
	/declare summonedSpells[20]					string outer NULL
	/declare summonedItems[20]					string outer NULL
	/declare summonedSlots[20]					string outer Item
	/declare summonedMinCount[20]				int outer 0

	/declare equipmentSlot[23]	string local
	/varset equipmentSlot[1] face
	/varset equipmentSlot[2] neck
	/varset equipmentSlot[3] rightear
	/varset equipmentSlot[4] leftear
	/varset equipmentSlot[5] arms
	/varset equipmentSlot[6] hands
	/varset equipmentSlot[7] rightwrist
	/varset equipmentSlot[8] leftwrist
	/varset equipmentSlot[9] rightfinger
	/varset equipmentSlot[10] leftfinger
	/varset equipmentSlot[11] shoulder
	/varset equipmentSlot[12] back
	/varset equipmentSlot[13] chest
	/varset equipmentSlot[14] waist
	/varset equipmentSlot[15] legs
	/varset equipmentSlot[16] feet
	/varset equipmentSlot[17] mainhand
	/varset equipmentSlot[18] offhand
	/varset equipmentSlot[19] ranged
	/varset equipmentSlot[20] ammo
	/varset equipmentSlot[21] charm
	/varset equipmentSlot[22] powersource
	/varset equipmentSlot[23] head

	| =====================================================================
	| Declare MissionMode variables
	| =====================================================================
	/declare missionMode				bool outer
	/declare missionINI					string outer
	/declare missionLeader			string outer
	/declare missionFollowers		string outer
	/declare missionCurrentAction		int outer 0
	/declare missionName				string outer
	/declare missionRequestZone	string outer
	/declare missionRequestNPC	string outer
	/declare missionRequestText string outer
	/declare missionRequestor		string outer
	/declare missionStartText		string outer
	/declare missionStartNPC		string outer
	/declare missionStartPath		string outer
	/declare missionStartRequestor	string outer
	/declare missionStartDelay	string outer
	/declare missionStartDelayTimer		timer outer 0s
	/declare missionCompleteAction		string outer NULL
	/declare missionRepeatCount				int outer 0
	/declare missionRunCount					int outer 0
	/declare missionRepeatDelay				string outer 30m
	/declare missionRepeatMinDelay		int outer 0
	/declare missionRepeatMaxDelay		int outer 0
	/declare missionRepeatDelayType		string outer m
	/declare missionRequestDelay			string outer 360m
	/declare missionRequestTimer			timer outer 0s
	/declare missionZone				string outer
	/declare missionCommMethod	string outer
	/declare missionStatus			string outer
	/declare missionFollowerStatus[6,5]	string outer
	/declare missionWaitTimer		timer outer 0s
	/declare missionWaitCondition				string outer NULL
	/declare missionActionAttempts			int outer 0s
	/declare missionActionMaxAttempts 	int outer 0s
	/declare missionActionMaxTimer			timer outer 0s
	/declare missionAmLeader						bool outer FALSE
	/declare missionWaitingOn						string outer NULL
	/declare missionStartAtAction				int outer 0
	
	/declare missionActionType					string outer NULL
	/declare missionActionByCommand			bool outer TRUE
	/declare missionActionFollowLeader	bool outer TRUE
	/declare missionActionLocX					string outer NULL
	/declare missionActionLocY					string outer NULL
	/declare missionActionLocZ					string outer NULL
	/declare missionActionPathName			string outer NULL
	/declare missionActionPullPath			string outer NULL
	/declare missionActionLocFuzziness	int outer 10
	/declare missionActionTargetNPC			string outer NULL
	/declare missionActionPullNPC[30,5]	string outer
	/declare missionActionTargetRadius	int outer 20
	/declare missionActionSpeakText			string outer NULL
	/declare missionActionLootMobs			bool outer FALSE
	/declare missionActionLooter				string outer LEADER
	/declare missionActionItem[30,5]		string outer
	/declare missionActionPreCommand[50]	string outer
	/declare missionActionPostCommand[50]	string outer
	/declare missionActionCompleteCondition[50]	string outer
	/declare missionActionCompleteConditionRequire string outer ALL
	/declare missionActionCompleteConditionCount	int outer 0
	/declare missionActionCompleteBy		string outer ALL
	/declare missionActionCompleteDelay	string outer 2s
	/declare missionActionKillCounts		bool outer FALSE
	/declare missionActionPullLimit			bool outer FALSE
	/declare missionPrimaryActionCompleted bool outer FALSE
	
	/declare dataFile			string outer devCommonPremium.dat

	/call EchoLog "Initialized \agdevCommonPremium.inc \awv${devCommonVer} \ao- Written By Devestator"
	/call MovementInit FALSE TRUE
	/call MQ2CastInit
	
	/call EchoLog "Loading Bot specific configuration..."
	
	/declare reqCommon		float outer ${devCommonVer}
	/declare reqMovement  float outer ${devMovementVer}
	/declare reqCast			float outer ${reqCastCommon}
/return

Sub AddCleanUp
	/if (!${targCount}) /return
	/declare acLoop				int local 0
	/declare ccLoop				int local 0
	/declare mobID				int local 0
	/for acLoop 1 to ${targCount}
		/varset mobID ${targArray[${acLoop}]}
		/if (${mobID}) {
			/if (${Defined[addTracking${mobID}]} && !${addTracking${mobID}}) {
				/if (${Spawn[${mobID}].ID} && ${Spawn[${mobID}].Type.NotEqual[Corpse]}) /call EchoLog "Add removed because it is no longer in the camp, ${Spawn[${mobID}].CleanName} | ${mobID}" TRUE
				/varset targArray[${acLoop}] 0
				/varcalc corpseCount ${corpseCount} + 1
				/varset corpseArray[${corpseCount}] ${If[${Spawn[${mobID}].Type.Equal[Corpse]},${mobID},0]}
				/deletevar addTracking${mobID}
			} else /if (!${Defined[addTracking${mobID}]}) {
				/if (!${Spawn[${mobID}].ID} || (${Math.Distance[${Spawn[${mobID}].Y},${Spawn[${mobID}].X}:${homeY},${homeX}]} > ${Math.Calc[${campRadius} * 2]} && ${Spawn[${mobID}].Distance} > ${campRadius})) {
					/declare addTracking${mobID}					timer outer 15s
				}
			} else /if (${addTracking${mobID}}) {
				| /echo FoundTimer
				/if (${Spawn[${mobID}].ID} && (${Math.Distance[${Spawn[${mobID}].Y},${Spawn[${mobID}].X}:${homeY},${homeX}]} < ${Math.Calc[${campRadius} * 2]} || ${Spawn[${mobID}].Distance} <= ${campRadius})) {
					/deletevar addTracking${mobID}
				}
			}
		}
		:skipMob
	/next acLoop

	/varcalc lTargCount ${targCount} - ${corpseCount}
/return

Sub Announce(string announceString) 
	/if (!${doAnnounce} || ${announceString.Equal[NULL]} || !${announceString.Length}) /return
	/if (${announceChan.Find[/bc]} && ${announceString.Find[%t]}) {
		/varset announceString ${announceString.Replace[%t,${Target.CleanName}]}
	}
	/docommand ${announceChan} ${announceString}
/return

Sub AutoBuffCheck(string itemName,string itemType)
	/declare aLoop							int local 0
	/declare aBuffSpellName			string local
	/declare currSpellName			string local
	/declare currSpellRank			int local 0
	/declare aMatch							bool local FALSE
	/declare stackIssues[20]		string local
	/declare stackNum						int local 0
	/declare sLoop							int local 0
	/declare stackCount					int local 0
	/declare stackHigher				bool local FALSE
	/declare stackCheck					bool local FALSE
	/call GetINIArrayCount devCommonPremium.dat BuffStackExceptions Stack NULL
	/varset stackCount ${Macro.Return}

	/if (${itemType.Equal[Item]}) {
		/if (!${FindItem[${itemName}].ID} || ${FindItem[${itemName}].RequiredLevel} > ${Me.Level}) /return
		/call GetSpellName "${FindItem[${itemName}].Spell.Name}"
		/varset currSpellName ${Macro.Return}
		/call GetSpellRank "${FindItem[${itemName}].Spell.Name}"
		/varset currSpellRank ${Macro.Return}
	} else /if (${Select[${itemType},Spell,Disc,Alt]}) {
		/if ((${Select[${itemType},Spell,Disc]} && ${Spell[${itemName}].Level[${Me.Class}]} > ${Me.Level}) || (${itemType.Equal[Alt]} && !${Me.AltAbility[${itemName}]})) /return
		/call GetSpellName "${itemName}"
		/varset currSpellName ${Macro.Return}
		/call GetSpellRank "${itemName}"
		/varset currSpellRank ${Macro.Return}
	}
	
	/varset stackNum 0
	/for aLoop 1 to ${stackCount}
		/if (${Ini[devCommonPremium.dat,BuffStackExceptions,Stack${aLoop},NULL].Find[${currSpellName}]}) {
			/varcalc stackNum ${stackNum} + 1
			/varset stackIssues[${stackNum}] ${Ini[devCommonPremium.dat,BuffStackExceptions,Stack${aLoop},NULL]}
		}
	/next aLoop
	
	/varset aMatch FALSE
	/if (${autoBuffCount}) {
		/for aLoop 1 to ${autoBuffCount}
			/varset stackHigher FALSE
			/varset stackCheck FALSE
			/if (${stackNum}) {
				/call GetSpellName "${autoBuffText[${aLoop}]}"
				/varset aBuffSpellName ${Macro.Return}
				/for sLoop 1 to ${stackNum}
					/if (${stackIssues[${sLoop}].Find[${aBuffSpellName}]}) {
						/varset stackCheck TRUE
						/if (${stackIssues[${sLoop}].Find[${aBuffSpellName}]}>${stackIssues[${sLoop}].Find[${currSpellName}]}) /varset stackHigher TRUE
					}
				/next sLoop
			}
			/if (${autoBuffText[${aLoop}].Find[${currSpellName}]} || ${stackHigher}) {
				/varset aMatch TRUE
				/call GetSpellRank "${autoBuffText[${aLoop}]}"
				/if (${currSpellRank} > ${Macro.Return} || ${stackHigher} || (${autoBuffSlot[${aLoop}].Equal[ITEM]} && !${FindItem[${autoBuff[${aLoop}]}].ID})) {
					/call EchoLog ".....Replacing auto buff ${autoBuff[${aLoop}]} (${autoBuffText[${aLoop}]}) with ${itemName} (${FindItem[${itemName}].Spell.Name})" TRUE
					/ini ${iniName} AutoBuffs AutoBuffName${aLoop} "${itemName}"
					/varset autoBuff[${aLoop}] ${itemName}
					/ini ${iniName} AutoBuffs AutoBuffText${aLoop} "${FindItem[${itemName}].Spell.Name}"
					/varset autoBuffText[${aLoop}] ${FindItem[${itemName}].Spell.Name}
					/ini ${iniName} AutoBuffs AutoBuffSlot${aLoop} ${Ini[devCommonPremium.dat,"${currSpellName}",Slot,ITEM]}
					/varset autoBuffSlot[${aLoop}] ${Ini[devCommonPremium.dat,"${currSpellName}",Slot,ITEM]}
					/ini ${iniName} AutoBuffs AutoBuffType${aLoop} ${Ini[devCommonPremium.dat,"${currSpellName}",Type,SELF]}
					/varset autoBuffType[${aLoop}] ${Ini[devCommonPremium.dat,"${currSpellName}",Type,SELF]}
					/ini ${iniName} AutoBuffs AutoBuffTargets${aLoop} ${Ini[devCommonPremium.dat,"${currSpellName}",Targets,ALL]}
					/varset autoBuffTargets[${aLoop}] ${Ini[devCommonPremium.dat,"${currSpellName}",Targets,ALL]}
					/ini ${iniName} AutoBuffs AutoBuffCombat${aLoop} ${Ini[devCommonPremium.dat,"${currSpellName}",Combat,FALSE]}
					/varset autoBuffCombat[${aLoop}] ${Ini[devCommonPremium.dat,"${currSpellName}",Combat,FALSE]}
				}
			} else /if (${stackCheck} && !${stackHigher}) {
				/varset aMatch TRUE
			}
		/if (!${aMatch}) /next aLoop
	}
	
	/if (!${aMatch}) {
		/if (${Ini[devCommonPremium.dat,"${currSpellName}",Classes,NULL].Find[${Me.Class.ShortName}]} || ${Ini[devCommonPremium.dat,"${currSpellName}",Classes,NULL].Find[ALL]}) {
			/call EchoLog ".....New auto buff found ${itemName} (${FindItem[${itemName}].Spell.Name})" TRUE
			/varcalc autoBuffCount ${autoBuffCount} + 1
			/call GetINISetting ${iniName} AutoBuffs AutoBuffName${autoBuffCount} autoBuff[${autoBuffCount}] "${itemName}"
			/call GetINISetting ${iniName} AutoBuffs AutoBuffText${autoBuffCount} autoBuffText[${autoBuffCount}] "${FindItem[${itemName}].Spell.Name}"
			/call GetINISetting ${iniName} AutoBuffs AutoBuffSlot${autoBuffCount} autoBuffSlot[${autoBuffCount}] ${Ini[devCommonPremium.dat,"${currSpellName}",Slot,ITEM]}
			/call GetINISetting ${iniName} AutoBuffs AutoBuffType${autoBuffCount} autoBuffType[${autoBuffCount}] ${Ini[devCommonPremium.dat,"${currSpellName}",Type,SELF]}
			/call GetINISetting ${iniName} AutoBuffs AutoBuffTargets${autoBuffCount} autoBuffTargets[${autoBuffCount}] ${Ini[devCommonPremium.dat,"${currSpellName}",Targets,ALL]}
			/call GetINISetting ${iniName} AutoBuffs AutoBuffAuto${autoBuffCount} autoBuffAuto[${autoBuffCount}] TRUE FALSE
			/call GetINISetting ${iniName} AutoBuffs AutoBuffAlias${autoBuffCount} autoBuffAlias[${autoBuffCount}] AUTO${autoBuffCount}
			/call GetINISetting ${iniName} AutoBuffs AutoBuffCombat${autoBuffCount} autoBuffCombat[${autoBuffCount}] ${Ini[devCommonPremium.dat,"${currSpellName}",Combat,FALSE]}
			/call GetINISetting ${iniName} AutoBuffs AutoBuffEnabled${autoBuffCount} autoBuffEnabled[${autoBuffCount}] TRUE FALSE						
		}
	}
/return

Sub AutoBuffLoad
| ==========================================================================
| Load common buffs automatically
| ==========================================================================
	/call EchoLog "Loading automatic buffs, this may take a few seconds"
	/call GetINIArrayCount ${iniName} AutoBuffs AutoBuffName NULL
	/varset autoBuffCount ${Macro.Return}
	/call EchoLog ".....${autoBuffCount} current automatic buff(s)" TRUE
	/if (${autoBuffCount}) {
		/for nArray 1 to ${autoBuffCount}
			/call GetINISetting ${iniName} AutoBuffs AutoBuffName${nArray} autoBuff[${nArray}] NULL
			/call GetINISetting ${iniName} AutoBuffs AutoBuffText${nArray} autoBuffText[${nArray}] NULL
			/call GetSpellName "${autoBuffText[${nArray}]}"
			/call GetINISetting ${iniName} AutoBuffs AutoBuffSlot${nArray} autoBuffSlot[${nArray}] ${Ini[devCommonPremium.dat,"${Macro.Return}",Slot,ITEM]}
			/call GetINISetting ${iniName} AutoBuffs AutoBuffType${nArray} autoBuffType[${nArray}] ${Ini[devCommonPremium.dat,"${Macro.Return}",Type,SELF]}
			/call GetINISetting ${iniName} AutoBuffs AutoBuffTargets${nArray} autoBuffTargets[${nArray}] ${Ini[devCommonPremium.dat,"${Macro.Return}",Targets,ALL]}
			/call GetINISetting ${iniName} AutoBuffs AutoBuffAuto${nArray} autoBuffAuto[${nArray}] TRUE FALSE
			/call GetINISetting ${iniName} AutoBuffs AutoBuffAlias${nArray} autoBuffAlias[${nArray}] AUTO${nArray}
			/call GetINISetting ${iniName} AutoBuffs AutoBuffCombat${nArray} autoBuffCombat[${nArray}] ${Ini[devCommonPremium.dat,"${Macro.Return}",Combat,FALSE]}
			/call GetINISetting ${iniName} AutoBuffs AutoBuffEnabled${nArray} autoBuffEnabled[${nArray}] TRUE FALSE
			/call EchoLog "..........NAME: ${autoBuff[${nArray}]} TEXT: ${autoBuffText[${nArray}]} SLOT: ${autoBuffSlot[${nArray}]} TYPE: ${autoBuffType[${nArray}]} AUTO: ${autoBuffAuto[${nArray}]} ALIAS: ${autoBuffAlias[${nArray}]} COMBAT: ${autoBuffCombat[${nArray}]} ENABLED: ${autoBuffEnabled[${nArray}]}" TRUE
		/next nArray
	}
	
	| =====================================================
	| Check for new or upgraded versions of the auto buffs
	| =====================================================

	| loop through inventory and such here to find items, check character level and required level on item to ensure usage
	| then check the autobuff list to see if a buff of the same kind exists, and if it does replace it with the upgraded one
	| if it doesn't exist yet, add it to the end of the list
	
	/declare equippedArray[23]	string local
	
	/varset equippedArray[23] head
	/varset equippedArray[1] face
	/varset equippedArray[2] neck
	/varset equippedArray[3] rightear
	/varset equippedArray[4] leftear
	/varset equippedArray[5] arms
	/varset equippedArray[6] hands
	/varset equippedArray[7] rightwrist
	/varset equippedArray[8] leftwrist
	/varset equippedArray[9] rightfinger
	/varset equippedArray[10] leftfinger
	/varset equippedArray[11] shoulder
	/varset equippedArray[12] back
	/varset equippedArray[13] chest
	/varset equippedArray[14] waist
	/varset equippedArray[15] legs
	/varset equippedArray[16] feet
	/varset equippedArray[17] mainhand
	/varset equippedArray[18] offhand
	/varset equippedArray[19] ranged
	/varset equippedArray[20] ammo
	/varset equippedArray[21] charm
	/varset equippedArray[22] powersource
	
	/for nArray 1 to 23
		/if (${Me.Inventory[${equippedArray[${nArray}]}].Name.NotEqual[NULL]}) {
			/if (${Me.Inventory[${equippedArray[${nArray}]}].Spell.ID} && ${Me.Inventory[${equippedArray[${nArray}]}].EffectType.Find[Click]}) {
				/call AutoBuffCheck "${Me.Inventory[${equippedArray[${nArray}]}].Name}" Item
			}
		}
	/next nArray
	
	/declare bLoop						int local 0	
	/for nArray 1 to 10
		/if (${Me.Inventory[pack${nArray}].Name.NotEqual[NULL]}) {
			/if (!${Me.Inventory[pack${nArray}].Container} && ${Me.Inventory[pack${nArray}].Spell.ID} && ${Me.Inventory[pack${nArray}].EffectType.Find[Click]}) {
				/call AutoBuffCheck "${Me.Inventory[pack${nArray}].Name}" Item
			} else /if (${Me.Inventory[pack${nArray}].Container}) {
				/for bLoop 1 to ${Me.Inventory[pack${nArray}].Container}
					/if (${Me.Inventory[pack${nArray}].Item[${bLoop}].Name.NotEqual[NULL]} && ${Me.Inventory[pack${nArray}].Item[${bLoop}].Spell.ID} && ${Me.Inventory[pack${nArray}].Item[${bLoop}].EffectType.Find[Click]}) {
						/call AutoBuffCheck "${Me.Inventory[pack${nArray}].Item[${bLoop}].Name}" Item
					}
				/next bLoop
			}
		}
	/next nArray
/return

Sub GetSpellName(string spellString)
	/declare paramCount													int local
	/varcalc paramCount ${spellString.Count[ ]} + 1
	/declare sLoop															int local 0
	/declare sName															string local
	/declare sRanks															string local [I]|[II]|[III]|[IV]|[V]|[VI]|[VII]|[VIII]|[IX]|[X]|[XI]|[XII]|[XIII]|[XIV]|[XV]|[XVI]|[XVII]|[XVIII]|[XIX]|[XX]|[XXI]|[XXII]|[XXIII]|[XXIV]|[XXV]
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

Sub GetSpellRank(string spellString)
	/declare paramCount													int local
	/varcalc paramCount ${spellString.Count[ ]} + 1
	/declare sRanks															string local [I]|[II]|[III]|[IV]|[V]|[VI]|[VII]|[VIII]|[IX]|[X]|[XI]|[XII]|[XIII]|[XIV]|[XV]|[XVI]|[XVII]|[XVIII]|[XIX]|[XX]|[XXI]|[XXII]|[XXIII]|[XXIV]|[XXV]
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

Sub CommonLoad
	/clearerrors

	/call EchoLog "Checking include file versions..."
	
	/declare supVersion									float local
	/if (${Defined[myName]}) {
		/varset supVersion ${${myName}}
	} else {
		/varset supVersion ${${Macro.Name.Left[-4].Lower}}
	}
	/if (${meVersion}<${supVersion} || !${supVersion}) {
		/call EchoLog "\ayThe version of \ag${Macro.Name} \awv${meVersion} \ayyou are using is not supported with this version of \agdevCommonPremium.inc \awv${devCommonVer}\ao: Required Version: \aw${supVersion}"
		/endmacro
	}
	/if (${devCommonVer}<${reqCommon}) {
		/call EchoLog "\ayThe version of devCommonPremium.inc v${devCommonVer} is not supported by ${Macro.Name} v${meVersion}, please download a newer version of devCommonPremium.inc"
		/endmacro
	}
	/if (${devMovementVer}<${reqMovement}) {
		/call EchoLog "\ayThe version of devMovement.inc v${devMovementVer} is not supported by ${Macro.Name} v${meVersion}, please download a newer version of devMovement.inc"
		/endmacro
	}
	/if (${devMovementVer}<${reqMovementCommon}) {
		/call EchoLog "\ayThe version of devMovementPremium.inc v${devMovementVer} is not support by devCommonPremium.inc v${devCommonVer}, please download a newer version of devMovementPremium.inc"
		/endmacro
	}
	|**
	/if (!${Defined[MQ2CastSpellRoutinesVer]} || ${MQ2CastSpellRoutinesVer}<${reqCast}) {
		/call EchoLog "\ayThe version of MQ2Cast_Spell_Routines v${MQ2CastSpellRoutinesVer} is not supported by ${Macro.Name} v${meVersion}.  Need version ${reqCast}.  Please make sure you have the latest copy of MQ2Cast_Spell_Routines.inc"
		/endmacro
	}
	**|
	/if (!${Defined[devCastVersion]} || ${devCastVersion}<${reqCast}) {
		/call EchoLog "\ayThe version of devCast v${devCastVersion} is not supported by ${Macro.Name} v${meVersion}.  Need version ${reqCast}.  Please make sure you have the latest copy of devCast.inc"
		/endmacro
	}
	/if (${Defined[myClass]} && !${myClass.Find[${Me.Class.ShortName}]}) {
		/call EchoLog "\ayThis bot does not support the ${Me.Class.Name} class."
		/endmacro
	}

	/call CommonINILoad

	/if (${devPullVer}<${reqPullCommon}) {
		/call EchoLog "\ayThe version of devPull.inc v${devPullVer} is not support by devCommonPremium.inc v${devCommonVer}, please download a newer version of devPull.inc.  Pulling will not be allowed until this is corrected."
		/varset allowPullMode FALSE
	}
				
	/call CreateIgnoreAlert ${alertList}
	/call CreateSafeAlert ${safeAlertList}
	
	/if (!${campRadius}) {
		/if (${mobRadius}) {
			/varset campRadius ${mobRadius}
		} else {
			/varset campRadius 60
			/varset mobRadius 60
		}
	}
	
	/if (${pullerName.Equal[Group]} || ${pullerName.Equal[null]}) {
		/varset useGroupPuller TRUE
		/varset pullerName ${Group.Puller.CleanName}
	}
	/if (${mainTank.Equal[Group]} || ${mainTank.Equal[null]}) {
		/varset useGroupTank TRUE
		/varset mainTank ${Group.MainTank.CleanName}
	}
	/if (${mainAssist.Equal[Group]} || ${mainAssist.Equal[null]}) {
		/varset useGroupAssist TRUE
		/varset mainAssist ${Group.MainAssist.CleanName}
	}
	
	/if (${Me.Pet.Name.Find[familiar]}) {
		/call EchoLog "Destroying familiar pet." TRUE
		/pet get lost
		/delay 1s
	}	
	
	/if (${useEQBC} && ${Plugin[MQ2NetBots].Version}) {
		/squelch /netbots grab=on
		/squelch /netbots send=on
		/squelch /netbots on
	}
	
	/if (${useMerc}) {
		/varset mercID 0
		/for nArray 1 to ${Group.Members}
			/if (${Group.Member[${nArray}].Type.Equal[MERCENARY]} && ${Group.Member[${nArray}].Owner.ID}==${Me.ID}) {
				/varset mercID ${Group.Member[${nArray}].ID}
				/varset mercName ${Group.Member[${nArray}].CleanName}
			}
		/if (!${mercID}) /next nArray
	}
/return

Sub CombatCheck(bool AddCheck)
	/if (!${Defined[AddCheck]}) /declare AddCheck				bool local TRUE
	/if (${AddCheck}) /call CheckForAdds ${campRadius} ${Me.ID} FALSE TRUE
	/if (!${inCombat} && (${lTargCount} > 0 || ${Me.CombatState.Equal[Combat]})) {
		/varset inCombat TRUE
		/varset minBuffCheckTimer ${minBuffCheckTimerStr}
		/if (${Defined[healCheckTimer]}) /varset healCheckTimer 0
		/if (${Defined[engagePet]}) /varset engagePet TRUE
		/if (${followMode}) /squelch /stick off
		/if (${Me.Sitting} && ${meleeClasses.Find[${Me.Class.ShortName}]}) /stand
		/call EchoLog "Entering Combat" TRUE
	}
	/if (${inCombat} && ${lTargCount} < 1 && ${Me.CombatState.NotEqual[Combat]}) {
		/varset inCombat FALSE
		/call AfterCombatRoutines
		/varset targCount 0
		/varset corpseCount 0
		/varset lTargCount 0
		/if (${Defined[healCheckTimer]}) /varset healCheckTimer 20s
		/if (${Defined[engagePet]}) /varset engagePet FALSE
		/if (${minBuffCheckTimer} < 30) /varset minBuffCheckTimer 3s
	}
/return

Sub CommonCombatRoutines(string callRoutine,bool AllowCombatBuff,bool AllowDebuff,bool AllowAddCleanup)
	/if (${Zone.ID}!=${startZoneID}) {
		/call EchoLog "\ayWe have changed zones, setting home location to current location."
		/call SetHome
		/varset startZoneID ${Zone.ID}
		/varset startZone ${Zone.ShortName}
		/if (${pullMode}) {
			/call EchoLog ".....Disabling pull mode due to zone change."
			/varset pullMode FALSE
		}
	}
	
	/if (${Me.State.Equal[HOVER]}) /call Event_died
	
	/if (!${Defined[AllowCombatBuff]}) /declare AllowCombatBuff				bool local TRUE
	/if (!${Defined[AllowDebuff]}) /declare AllowDebuff								bool local TRUE
	/if (!${Defined[AllowAddCleanup]}) /declare AllowAddCleanup				bool local TRUE
	/if (!${Defined[LastMercAssistID]}) /declare LastMercAssistID			int outer 0
	/if (${Defined[callRoutine]}) {
		/varset inCombatBuffRoutine ${callRoutine}
	} else {
		/varset inCombatBuffRoutine NULL
	}
	/if (${followMode} && ${Math.Distance[${homeY},${homeX}]} > ${Math.Calc[${campRadius} / 4]}) /call SetHome
	/if (${Me.Feigning} && !${Me.Class.Name.Equal[Monk]} && !${Me.Class.Name.Equal[Shadow Knight]}) /stand
	/call campCheck

	/if (${manageMerc}) {
		/if (((${Me.Mercenary.State.Equal[ACTIVE]} || (${Group.Member[1].ID} && ${Group.Member[1].Type.Equal[MERCENARY]})) && ${mainAssist.NotEqual[${Group.Member[1].CleanName}]} && ${mainTank.NotEqual[${Group.Member[1].CleanName}]} && ${mainTank.NotEqual[${Me.CleanName}]})) {
			/varset nArray ${Target.ID}
			/assist ${mainAssist}
			/delay 1s ${Target.ID}!=${nArray}
			/if (${Target.ID} && ${Target.Type.Equal[NPC]} && ${Target.PctHPs} < ${mercAssistPct} && ${Target.ID}!=${LastMercAssistID}) {
				/mercassist
				/varset LastMercAssistID ${Target.ID}
			} else /if ((!${Target.ID} || ${Target.Type.NotEqual[NPC]}) && ${LastMercAssistID}) {
				/mercassist off
				/varset LastMercAssistID 0
			}
		}
	}
	/if (${manageMerc} && ${mercID} && ${Group.Member[1].ID}==${mercID}) {
		/if (${mercStance.NotEqual[${mercICStance}]} && ${mercICStance.NotEqual[NULL]} && ${Target.ID} && ${Target.Type.Equal[NPC]} && ${Target.PctHPs} > ${If[${Math.Calc[${mercBurnPct} + 2]}>99,99,${Math.Calc[${mercBurnPct} + 2]}]}) /stance ${mercICStance}
		/if (${Target.ID} && ${Target.Type.Equal[NPC]} && ${Target.PctHPs} <= ${mercBurnPct} && ${mercStance.NotEqual[Burn]}) /stance Burn
	}

	/if (${debuffCount} && ${doDebuffs} && ${AllowDebuff} && ${debuffClasses.Find[${Me.Class.ShortName}]} && !${priorityDebuffsTimer}) /call DebuffRoutine
	/if (${combatBuffs} && ${AllowCombatBuff} && ${doCombatBuffs}) /call buffCheck TRUE TRUE ${callRoutine}
	/if ((${mainAssist.Equal[NULL]} || ${mainAssist.Equal[${Me.CleanName}]})	 && ${Target.ID} && ${Target.Type.Equal[Corpse]}) /squelch /target clear
	
	/call AlertsCheck

	/if (!${medMode} && ((${Me.MaxMana} && ${Me.PctMana} < ${medModeManaThreshold}) || (!${Me.MaxMana} && ${Me.PctEndurance} < ${medModeManaThreshold}))) {
		/call EchoLog "Entering med mode, will not cast combat spells until ${If[${Me.Class.CanCast},mana,endurance]} is above ${medModeMedTo} percent" TRUE
		/varset medMode TRUE
	} else /if (${medMode} && ((${Me.MaxMana} && ${Me.PctMana} >= ${medModeMedTo}) || (!${Me.MaxMana} && ${Me.PctEndurance} >= ${medModeMedTo}))) {
		/call EchoLog "Leaving med mode" TRUE
		/varset medMode FALSE
	}
	
	/if (${enrageID}) {
		/if (${Target.ID}==${enrageID} && !${Stick.Behind} && !${Me.Song[Master's Aura Effect].ID}) {	 
			| ' Just a temp fix for something in ultraedit
			/if (${Me.Combat}) /attack off
		} else {
			| /attack on
			/varset enrageID 0
		}
	}
	
	/if (${Window[CombatAbilityWnd].Open}) {
		/if (${Window[CombatAbilityWnd].Child[CAW_CombatEffectLabel].Text.NotEqual[No Effect]}) {
			/if (!${discTimer}) /varset discTimer ${Spell[${Window[CombatAbilityWnd].Child[CAW_CombatEffectLabel].Text}].Duration.TotalSeconds}s
		} else {
			/varset discTimer 0s
		}
	}
	
	/if (${hPotName.NotEqual[NULL]} && ${FindItem[${hPotName}].ID} && !${FindItem[${hPotName}].Timer} && ${Me.PctHPs} < ${hPotHPs} && !${Me.Casting.ID}) {
		/useitem "${hPotName}"
	}
	/if (${modRodNameCommon.NotEqual[NULL]} && ${Me.PctMana} <= ${modRodManaCommon} && ${Me.PctHPs} >= ${modRodMinHPsCommon} && ${FindItem[${modRodNameCommon}].ID} && !${Me.Casting.ID}) {
		/useitem "${modRodNameCommon}"
	}

	/if ((!${Spawn[${mainAssist}].ID} || ${Spawn[${mainAssist}].Type.Equal[Corpse]}) && ${secondaryAssist.NotEqual[NULL]} && ${Spawn[${secondaryAssist}].ID} && ${Spawn[${secondaryAssist}].Type.NotEqual[Corpse]} && ${mainAssist.NotEqual[${secondaryAssist}]}) {
		/call EchoLog "MainAssist ${mainAssist} appears to be dead or missing, using ${secondaryAssist} instead"
		/varset mainAssistOld ${mainAssist}
		/varset mainAssist ${secondaryAssist}
	} else {
		/if (${mainAssistOld.NotEqual[NULL]} && ${mainAssist.NotEqual[${mainAssistOld}]}) {
			/varset mainAssistOld NULL
			/varset mainAssist ${mainAssistOld}
		}
	}
	/if (${AllowAddCleanup}) /call AddCleanUp
	
	/if (!${pullCombatTimer} && ${pullMode} && ${allowCombatPull}) /call PullCombatCheck
/return

Sub CommonRoutines(bool pullCheck)
	/if (${Zone.ID}!=${startZoneID}) {
		/call EchoLog "\ayWe have changed zones, setting home location to current location."
		/call SetHome
		/varset startZoneID ${Zone.ID}
		/varset startZone ${Zone.ShortName}
		/if (${pullMode}) {
			/call EchoLog ".....Disabling pull mode due to zone change."
			/varset pullMode FALSE
		}		
	}
	
	/if (${Me.State.Equal[HOVER]}) /call Event_died
	
	/if (!${Defined[pullCheck]}) /declare pullCheck bool local TRUE
	/if (${followMode}) /call SetHome
	/call campCheck
	| /call TradeCheck
	
	/if (${Cursor.ID}) /autoinventory
	
	/if (${Group.Member[1].ID} && ${Group.Member[1].Type.Equal[MERCENARY]}) {
		/varset mercID ${Group.Member[1].ID}
		/varset mercName ${Group.Member[1].CleanName}
	}
	
	/if (${useMerc}) {
	  /if (!${mercCheckTimer} && ${Me.CombatState.NotEqual[Combat]} && (((${Me.Mercenary.State.Equal[SUSPENDED]} || ${Group.Member[1].Name.NotEqual[${mercName}]}) &&	${Group.GroupSize} < 6)	|| ${Me.Mercenary.State.Equal[UNKNOWN]} || 	(${Me.Mercenary.State.Equal[NULL]} && ${Group.Member[1].Name.NotEqual[${mercName}]}) || (${Group.Member[1].Name.Equal[${mercName}]} && !${Group.Member[1].ID}))) {
			/notify MMGW_ManageWnd MMGW_SuspendButton leftmouseup
			/delay 3
			/varset mercID 0
			/for nArray 1 to ${Group.Members}
				/if (${Group.Member[${nArray}].Type.Equal[MERCENARY]} && ${Group.Member[${nArray}].Owner.ID}==${Me.ID}) {
					/varset mercID ${Group.Member[${nArray}].ID}
					/varset mercName ${Group.Member[${nArray}].CleanName}
				}
			/if (!${mercID}) /next nArray
			/varset mercCheckTimer 10s
		} else /if (${useMerc} && ${Me.Mercenary.State.NotEqual[ACTIVE]} && (${Group.Member[1].Name.NotEqual[${mercName}]} || (${Group.Member[1].Name.Equal[${mercName}]} && !${Group.Member[1].ID}))) {
			/varset mercID 0
		}
	} else {
		/varset mercID 0
	}
	/if (${manageMerc} && ${mercID} && ${Group.Member[1].ID}==${mercID}) {
		/if (${mercStance.NotEqual[${mercOOCStance}]} && ${mercOOCStance.NotEqual[NULL]}) /stance ${mercOOCStance}
	}
	
	/if (${useMacroEndTimer} && !${macroEndTimer}) {
		/call EchoLog "The macro end timer has expired, taking the action: ${macroEndAction}"
		/if (${macroEndAction.Equal[GateCamp]}) {
			/call Evac TRUE FALSE TRUE
		} else /if (${macroEndAction.Equal[camp]}) {
			/call Evac TRUE TRUE TRUE
		} else /if (${macroEndAction.Equal[gate]}) {
			/call Evac FALSE FALSE TRUE
		} else /if (${macroEndAction.Equal[stop]}) {
			/endmacro
		} else /if (${macroEndAction.Equal[quit]}) {
			/quit
		} else {
			/call EchoLog "The action was not recognized, using the default action of GateCamp"
			/call Evac TRUE
		}
	}
	
	/if (${meleeClasses.Find[${Me.Class.ShortName}]} && ${Me.Class.ShortName.NotEqual[BRD]} && ${Me.PctEndurance} <= ${respitePct} && !${discTimer}) {
		/declare resArr int local 0
		/declare resRkArr int local 0
		/declare resRk string local Rk. III|Rk. II| 
		/for resArr ${respiteDiscs.Size} to 1
			/for resRkArr 1 to 3
				/if (${Me.CombatAbilityReady[${respiteDiscs[${resArr}]} ${resRk.Token[${resRkArr},|]}]}) {
					/doability "${respiteDiscs[${resArr}]} ${resRk.Token[${resRkArr},|]}"
					/varset discTimer ${Spell[${respiteDiscs[${resArr}]} ${resRk.Token[${resRkArr},|]}].Duration.TotalSeconds}s
				}
			/next resRkArr
		/next resArr
	}

	/if (${modRodNameCommon.NotEqual[NULL]} && ${Me.PctMana} <= ${modRodManaCommon} && ${Me.PctHPs} >= ${modRodMinHPsCommon} && ${FindItem[${modRodNameCommon}].ID} && !${Me.Casting.ID}) {
		/call MQ2Cast "${modRodNameCommon}" item
	}
	/if (${hPotName.NotEqual[NULL]} && ${FindItem[${hPotName}].ID} && !${FindItem[${hPotName}].Timer} && ${Me.PctHPs} < ${hPotHPs} && !${Me.Casting.ID}) {
		/call MQ2Cast "${hPotName}" Item
	}

	/call SummonedItemsCheck
	
	/if (${missionMode}) {
		/call MissionModeHandler
		| /echo >> ${Macro.Return}
	}
	
	/if (${pullCheck} && !${pullTimer} && ${pullMode}) {
		/call PullRoutine
		/varset pullResult ${Macro.Return}
	}
/return

Sub AddIgnoreMob(string addName)
	/if (${addName.Equal[null]}) /return
	
	/varcalc ignoreMobsCount ${ignoreMobsCount}+1
	/varset ignoreMobs[${ignoreMobsCount}] ${addName}
	/squelch /alert add ${alertList} ${addName}
	/ini ${iniName} IgnoreMobs Value ${ignoreMobsCount}
	/ini ${iniName} IgnoreMobs MobName${ignoreMobsCount} ${addName}	
	/call EchoLog "\ag${addName} \aoadded to ignore mobs list"
/return

Sub RemoveIgnoreMob(string removeName)
	/declare rLoop 		int local
	/if (${removeName.Equal[null]}) /return
	
	/for rLoop 1 to ${ignoreMobsCount}
		/if (${ignoreMobs[${rLoop}].Equal[${removeName}]}) {
			/varset ignoreMobs[${rLoop}] NULL
			/ini ${iniName} IgnoreMobs MobName${rLoop} NULL
		}
	/next rLoop
	/squelch /alert clear ${alertList}
	/for rLoop 1 to ${ignoreMobsCount}
		/if (${ignoreMobs[${rLoop}].NotEqual[NULL]}) /squelch /alert add ${alertList} ${ignoreMobs[${rLoop}]}
	/next rLoop
/return

Sub AddSafePC(string addName)
	/if (${addName.Equal[null]}) /return
	
	/varcalc safePCCount ${safePCCount}+1
	/varset safePC[${safePCCount}] ${addName}
	/squelch /alert add ${safeAlertList} ${addName}
	/ini ${iniName} SafePCs Value ${safePCCount}
	/ini ${iniName} SafePCs PCName${safePCCount} ${addName}	
	/call EchoLog "\ag${addName} \aoadded to safe pc list"	
/return

Sub RemoveSafePC(string removeName)
	/declare rLoop 		int local
	/if (${removeName.Equal[null]}) /return
	
	/for rLoop 1 to ${safePCCount}
		/if (${safePC[${rLoop}].Equal[${removeName}]}) {
			/varset safePC[${rLoop}] NULL
			/ini ${iniName} SafePCs PCName${rLoop} NULL
		}
	/next rLoop
	/squelch /alert clear ${safeAlertList}
	/for rLoop 1 to ${safePCCount}
		/if (${safePC[${rLoop}].NotEqual[NULL]}) /squelch /alert add ${safeAlertList} ${safePC[${rLoop}]}
	/next rLoop
/return

Sub AfterCombatRoutines(bool lootReturn)
	/if (!${Defined[lootReturn]}) /declare lootReturn			bool local TRUE
	/declare acArray			int local
	
	/if (${Me.Combat}) /attack off
	
	/call EchoLog "Combat completed HPs: ${Me.PctHPs} Mana: ${Me.PctMana} Pet HPs: ${Me.Pet.PctHPs}" TRUE
	/if (${lootCorpses}) {
		/for acArray 1 to ${corpseCount}
			/if (${lootOnlyKilled} && ${corpseArray[${acArray}]}) /call Loot ${corpseArray[${acArray}]}
			/varset corpseArray[${acArray}] 0
		/next acArray
		
		/if (!${lootOnlyKilled}) /call Loot
		
		/if (${groupMode} && !${Debuff.Rooted} && ${lootReturn}) {
			/call PositionCheckDefault
		}
	}

	/if (${targCount} && ${doDebuffs}) {
		/for acArray 1 to ${targCount}
			/call DebuffCleanup ${targArray[${acArray}]}
		/next acArray
	}

	/varset targCount 0
	/varset corpseCount 0
	/varset lTargCount 0
	/varset namedCount 0
	/varset enrageID 0
	/varset enrageAttackOff FALSE
	| /varset rested FALSE
	/varset allowSit TRUE
	/varset rampageNoSit FALSE
	/varset gotAggro FALSE

	/if (${Me.Combat}) /attack off
	/if (${hideCorpses}) {
		/hidecorpse allbutgroup
		/hidecorpse looted
	}
	/if (!${followMode}) {
		/call PositionCheckDefault
	} else {
		/squelch /target ID ${followTargID}
		/delay 2s ${Target.ID}==${followTargID}
		/if (${Target.ID}==${followTargID} && ${Target.Type.NotEqual[Corpse]} && ${Target.Distance} < ${campRadius}) {
			/squelch /stick hold
		}	else /if (${Target.ID}!=${followTargID} || (${Target.Type.Equal[Corpse]} && ${Target.ID}==${followTargID})) {
			/call EchoLog "\ayCould not resume follow because we could not find the follow target.  Resetting home loation to current location."
			/varset followMode FALSE
		} else /if (${Target.ID}==${followTargID} && ${Target.Distance} > ${campRadius}) {
			/call EchoLog "\ayCould not resume follow because the follow target is to far away.  Resetting home loation to current location."
			/varset followMode FALSE
		}
	}
	/varset sitTimer ${Math.Calc[${Math.Rand[9]} + 1]}s
	
	/if (!${pullTimer} && ${pullMode}) {
		/varset pullTimer ${pullRestTime}
	}
/return

Sub AlertsCheck(int mobID)
	/declare aLoop	int local
	/declare bLoop	int local
	/if (!${Defined[cLoop]}) /declare cLoop	int local
	/declare aSent	bool local
	/declare aSet		bool local
	/declare aZero	int local 
	
	/if (!${mobAlertsValue}) /return
	/if (!${Defined[mobID]}) {
		/if (${Target.ID}) {
			/declare mobID	int local ${Target.ID}
		} else {
			/if (${lTargCount}<1) /return
			/declare mobID int local 0
		}
	}

	/for aLoop 1 to 100
		/if (${alertSent[${aLoop}]} && !${Spawn[ID ${alertSent[${aLoop}]}].ID}) /varset alertSent[${aLoop}] 0
	/next aLoop

	/if (!${mobID}) {
		/for cLoop 1 to ${targCount}
			/varset mobID ${targArray[${cLoop}]}
			/for aLoop 1 to ${mobAlertsValue}	
				/if (${Spawn[${mobID}].CleanName.Find[${mobAlertsName[${aLoop}]}]}) {
					/for bLoop 1 to 100
						/if (${alertSent[${bLoop}]}==${mobID}) /return
						/if (${alertSent[${bLoop}]}==0 && !${aZero}) /varset aZero ${bLoop}
					/next bLoop
					/if (${aZero}) {
							/varset alertSent[${aZero}] ${mobID}
							/varset aSet TRUE
					}
					/if (!${aSet}) {
						/call EchoLog "There are currently 100 alert mobs being tracked, no longer playing alerts until some are cleared"
					} else {
						/if (!${mobAlertsAudio[${aLoop}].Equal[NULL]}) {
							/call EchoLog "Playing ${mobAlertsAudio[${aLoop}]} for ${Spawn[${mobID}].CleanName} | ${mobID}" TRUE
							/squelch /exec "${mobAlertsAudio[${aLoop}]}" bg
							/return
						}
					}
				}
			/next aLoop		
		/next cLoop
	} else {
		/for aLoop 1 to ${mobAlertsValue}	
			/if (${Spawn[${mobID}].CleanName.Find[${mobAlertsName[${aLoop}]}]}) {
				/for bLoop 1 to 100
					/if (${alertSent[${bLoop}]}==0) {
						/varset alertSent[${bLoop}] ${mobID}
						/varset aSet TRUE
					} else {
						/next bLoop
					}
				/if (!${aSet}) {
					/call EchoLog "There are currently 100 alert mobs being tracked, no longer playing alerts until some are cleared"
				} else {
					/if (!${mobAlertsAudio[${aLoop}].Equal[NULL]}) {
						/call EchoLog "Playing ${mobAlertsAudio[${aLoop}]} for ${Spawn[${mobID}].CleanName} | ${mobID}" TRUE
						/squelch /exec "${mobAlertsAudio[${aLoop}]}" bg
						/return
					}
				}
			}
		/next aLoop
	}
/return

Sub CastBuff(int buffTargID, int buffNum, string buffType, string singGroup, bool silentMode, string castCallRoutine)
	/if (!${Defined[singGroup]}) /declare singGroup			string local Single
	/if (!${Defined[silentMode]}) /declare silentMode		bool local FALSE
	/if (!${Defined[castCallRoutine]}) /declare castCallRoutine	string local CheckForAggro
	/if (!${castCallRoutine.Length} || ${castCallRoutine.Equal[NULL]}) /varset castCallRoutine CheckForAggro
	
	/declare buffString					string local groupBuff${singGroup}
	/declare shortBuffString		string local groupBuff
	/declare retValue						string local COMPLETED_SUCCESS
	/declare pbItem							bool local FALSE
	/declare iLoop							int local 0
	
	/if (${buffType.Equal[Group]}) {
		/varset buffString groupBuff${singGroup}
		/varset shortBuffString groupBuff
	} else /if (${buffType.Equal[SELF]}) {
		/varset buffString buffName
		/varset shortBuffString buff
	} else /if (${buffType.Equal[Pet]}) {
		/varset buffString petBuff
		/varset shortBuffString petBuff
	} else /if (${buffType.Equal[AUTO]}) {
		/varset buffString autoBuff
		/varset shortBuffString autoBuff
	}
	

	| /if (${${shortBuffString}Enabled[${buffNum}].Equal[FALSE]}) /return COMPLETED_DISABLED
	/call EchoLog "Attempting to cast buff ${${buffString}[${buffNum}]} on ${Spawn[ID ${buffTargID}].CleanName}" TRUE
	/if (${Me.Casting.ID} && (${Me.Casting.Skill.Find[Instrument]} || ${Me.Casting.Skill.Find[Sing]} || ${Twist.Twisting})) {
		/melody
		/squelch /twist stop
		/squelch /twist clear
		/delay 5s !${Me.Casting.ID}
	}
	/if (${${shortBuffString}Slot[${buffNum}].Find[Gem]}) {
		/if (${Me.Gem[${${buffString}[${buffNum}]}]}) {
			/if (${Me.GemTimer[${${buffString}[${buffNum}]}]} && ${Me.GemTimer[${${buffString}[${buffNum}]}].Float} > 5) {
				/call EchoLog ".....Cannot cast buff, it has a cooldown timer on it" TRUE
				/return ABORT_COOLDOWN
			}
		} else {
			/if (${Spell[${${buffString}[${buffNum}]}].RecoveryTime.Float} > 5) {
				/call EchoLog ".....Cannot cast buff, it is not memmed yet and has a long recovery time, will attempt to mem the buff in order to attempt buffing next buff check." TRUE
				/memspell ${${shortBuffString}Slot[${buffNum}].Right[-3]} "${${buffString}[${nArray}]}"
				/delay 5
			}
		}
		
		/call timer buffTimer ${buffNum}
		/if (${Macro.Return[TRUE]}) {
			/call EchoLog ".....Cannot cast buff, it has a cooldown timer on it" TRUE
			/return ABORT_COOLDOWN
		}
		
		/if (${isMMOBugs}) {
			/for iLoop 1 to 4
				/if (${Spell[${${buffString}[${buffNum}]}].ReagentCount[${iLoop}]} && ${Spell[${${buffString}[${buffNum}]}].ReagentID[${iLoop}]} != -1 && (!${FindItem[${Spell[${${buffString}[${buffNum}]}].ReagentID[${iLoop}]}].ID} || ${FindItemCount[${Spell[${${buffString}[${buffNum}]}].ReagentID[${iLoop}]}]} < ${Spell[${${buffString}[${buffNum}]}].ReagentCount[${iLoop}]})) {
					/call EchoLog ".....Cannot cast buff, missing required component ID ${Spell[${${buffString}[${buffNum}]}].ReagentID[${iLoop}]}" TRUE
					/return ABORT_MISSINGCOMPONENT
				}
			/next iLoop
		}
		
		/if (${Me.CurrentMana} < ${Spell[${${buffString}[${buffNum}]}].Mana}) /return ABORT_MANALOW
		/if (${Spell[${${buffString}[${buffNum}]}].RecoveryTime.Float} > 5 && !${Me.Gem[${${buffString}[${buffNum}]}]}) {
			/memspell ${${shortBuffString}Slot[${buffNum}].Right[-3]} "${${buffString}[${buffNum}]}"
			/call EchoLog ".....Cannot cast buff, recovery time is to long and it's not memmed.  Memming spell and canceling cast." TRUE
			/call timer buffTimer ${buffNum} ${Spell[${${buffString}[${buffNum}]}].RecoveryTime.Float}s
			/return ABORT_RECOVERY
		}
	} else /if (${${shortBuffString}Slot[${buffNum}].Equal[disc]}) {
		/if (!${Me.AbilityReady[${${buffString}[${buffNum}]}]} && !${Me.CombatAbilityReady[${${buffString}[${buffNum}]}]}) {
			/call EchoLog "The buff ${${buffString}[${buffNum}]} is not yet ready to cast." TRUE
			/varset retValue ABORT_NOTREADY
			/goto :cBuffEnd
		} else /if (${discTimer}) {
			/call EchoLog "Cannot cast the buff ${${buffString}[${buffNum}]} because there is already a disc running." TRUE
			/varset retValue ABORT_NOTREADY
			/goto :cBuffEnd
		}
	} else /if (${${shortBuffString}Slot[${buffNum}].Left[1].Equal[/]}) {
		/docommand ${${shortBuffString}Slot[${buffNum}]}
		/varset retValue COMPLETED_SUCCESS
		/goto :cBuffEnd
	}
	/if (${buffTargID}!=${Target.ID}) {
		/squelch /target clear
		/delay 1s !${Target.ID}
		/squelch /target id ${buffTargID}
		/delay 1s ${Target.ID}
	}
	/if (${buffType.Equal[Group]} && !${silentMode}) /call Announce "${${buffString}Announce[${buffNum}]}"
	/if (${${shortBuffString}Slot[${buffNum}].Equal[Item]}) {
		/call FindPotionBelt "${${buffString}[${buffNum}]}"
		/varset pbItem ${Macro.Return}
	}
	/if (!${pbItem}) {
		/if (${${shortBuffString}Slot[${buffNum}].Equal[disc]}) {
			/doability "${${buffString}[${buffNum}]}"
			:abCastWait
				/call CheckForAggro
				/if (${gotAggro}) /goto :cBuffEnd
				/delay 5
			/if (${Me.Casting.ID}) /goto :abCastWait
			| /varset discTimer ${Spell[${${buffString}[${buffNum}]}].Duration.TotalSeconds}s
			/varset discTimer 2s
			/varset retValue COMPLETED_SUCCESS
		} else {
			/if (${${shortBuffString}Slot[${buffNum}].Equal[Item]}) /call ItemSwapCheck "${${buffString}[${buffNum}]}"
			/varset membershipFail FALSE
			/call MQ2Cast "${${buffString}[${buffNum}]}" ${${shortBuffString}Slot[${buffNum}]} 0 ${castCallRoutine} -targetID|${Target.ID} -maxtries|${resistTries}
			/delay 2
			/if (${CastResult.Equal[CAST_PRESTIGE]}) {
				/call EchoLog "\ayInsufficient membership to use ${${buffString}[${buffNum}]}, disabling this ${buffType} buff."
				/varset ${shortBuffString}Enabled[${buffNum}] FALSE
			}
			/if (${itemSwapped}) /call ItemSwapCheck
			/if (${CastResult.Equal[CAST_SUCCESS]}) {
				/if (${${shortBuffString}Slot[${buffNum}].Find[Gem]} && ${Spell[${${buffString}[${buffNum}]}].RecastTime.Float} > 5) {
					/call timer buffTimer ${buffNum} ${Spell[${${buffString}[${buffNum}]}].RecastTime.Float}s
				}
				/varset retValue COMPLETED_SUCCESS
			} else {
				/call EchoLog "Attempt to buff failed, reason: ${CastResult}" TRUE
				/varset retValue ${CastResult}
			}
		}
	} else {
		/call CastPotionBelt "${${buffString}[${buffNum}]}" ${castCallRoutine}
		/if (${Macro.Return.Equal[POTIONCAST_SUCCESS]}) {
			/varset retValue COMPLETED_SUCCESS
		} else {
			/varset retValue ${Macro.Return}
			/call EchoLog "Attempt to buff potion belt item failed, reason: ${retValue}" TRUE
		}
	}
	/if (${retValue.Find[OUTDOORS]}) {
		/call EchoLog "The spell ${${buffString}[${buffNum}]} can only be cast outdoors.  Disabling it for the duration of this run." TRUE
		/varset ${shortBuffString}Enabled[${buffNum}] FALSE
	}
	:cBuffEnd
	/doevents flush spellFade
	/doevents
	/if (${gotAggro}) /varset retValue ABORT_AGGRO
/return ${retValue}

Sub FindPotionBelt(string PotionName)
	/declare pbArray						int local 0

	/if (!${Defined[PotionName]}) /return FALSE
	/for pbArray 0 to 4
		/if (${Window[PotionBeltWnd].Child[PW_PotionSlot${pbArray}_Button].Tooltip.Equal[${PotionName}]} && ${Window[PotionBeltWnd].Child[PW_PotionSlot${pbArray}_Label].Text}>0) {
			/return TRUE
		}
	/next pbArray
/return FALSE

Sub FindItemWithEffect(string effectName)
	/declare iLoop			int local 0
	/declare ibLoop			int local 0
	/for iLoop 1 to 23
		/if (${Me.Inventory[${equippedSlot[${iLoop}]}].Name.NotEqual[NULL]} && ${Me.Inventory[${equippedSlot[${iLoop}]}].Spell.Name.Equal[${effectName}]}) /return ${Me.Inventory[${equippedSlot[${iLoop}]}].Name}
	/next iLoop
	/for iLoop 1 to 10
		/if (${Me.Inventory[pack${iLoop}].Name.NotEqual[NULL]} && ${Me.Inventory[pack${iLoop}].Spell.Name.Equal[${effectName}]}) {
			/return ${Me.Inventory[pack${iLoop}].Name}
		} else /if (${Me.Inventory[pack${iLoop}].Container}) {
			/for ibLoop 1 to ${Me.Inventory[pack${iLoop}].Container}
				/if (${Me.Inventory[pack${iLoop}].Item[${ibLoop}].Name.NotEqual[NULL]} && ${Me.Inventory[pack${iLoop}].Item[${ibLoop}].Spell.Name.Equal[${effectName}]}) /return ${Me.Inventory[pack${iLoop}].Item[${ibLoop}].Name}
			/next ibLoop
		}
	/next iLoop
/return NULL

Sub CastPotionBelt(string PotionName, string CallRoutine) {
	/declare pbArray						int local 0
	/declare retValue						string local POTIONCAST_UNKNOWN
	
	/if (!${Defined[PotionName]}) /return POTIONCAST_NOTFOUND
	/if (!${Defined[CallRoutine]}) /declare CallRoutine		string local NULL
	/if (!${Defined[PBCasting]}) /declare PBCasting				bool outer FALSE
	/if (!${Defined[PBFailReason]}) /declare PBFailReason	string outer SUCCESS
	/for pbArray 0 to 4
		/if (${Window[PotionBeltWnd].Child[PW_PotionSlot${pbArray}_Button].Tooltip.Equal[${PotionName}]}) {
			/if (${Window[PotionBeltWnd].Child[PW_PotionSlot${pbArray}_Label].Text}>0) {
				/potionbelt activate ${Math.Calc[${pbArray}+1]}
				/delay 1s ${Me.Casting.ID}
				/varset PBCasting TRUE
				:potionCasting
					/doevents PBCastFail
					/if (${CallRoutine.NotEqual[NULL]}) /call ${CallRoutine}
				/if (${Me.Casting.ID}) /goto :potionCasting
				/delay 1
				/doevents PBCastFail
				/doevents outDoors
				
				/varset PBCasting FALSE
				/varset retValue POTIONCAST_${PBFailReason}
				/varset PBFailReason SUCCESS
				/return ${retValue}
			} else {
				/return POTIONCAST_NOPOTIONS
			}
		}
	/next pbArray	
}
/return POTIONCAST_NOTFOUND

Sub Event_PBCastFail(string rLine)
	/if (!${Defined[PBCasting]}) /declare PBCasting				bool outer FALSE
	/if (!${Defined[PBFailReason]}) /declare PBFailReason	string outer SUCCESS
	/if (!${PBCasting}) /return
	
	/if (${rLine.Find[You are not sufficient level to use this Item]}) {
		/varset PBFailReason LOWLEVEL
	} else /if (${rLine.Find[Your spell is interrupted]}) {
		/varset PBFailReason INTERRUPTED
	} else /if (${rLine.Find[Your gate is too unstable, and collapses]}) {
		/varset PBFailReason COLLAPSE
	} else /if (${rLine.Find[You *CANNOT* cast spells, you have been silenced]}) {
		/varset PBFailReason SILENCED
	} else /if (${rLine.Find[You can't cast spells while invulnerable]}) {
		/varset PBFailReason INVULNERABLE
	} else /if (${rLine.Find[Your casting has been interrupted]}) {
		/varset PBFailReason INTERRUPTED
	} else /if (${rLine.Find[This spell does not work here]}) {
		/varset PBFailReason OUTDOORS
	} else /if (${rLine.Find[You can't cast spells while stunned]}) {
		/varset PBFailReason STUNNED
	} else /if (${rLine.Find[Your spell did not take hold]}) {
		/varset PBFailReason TAKEHOLD
	} else /if (${rLine.Find[You must be standing to cast a Spell]}) {
		/varset PBFailReason DUCKING
	} else /if (${rLine.Find[You can only cast this spell in the outdoors]}) {
		/varset PBFailReason OUTDOORS
	}
/return

Sub buffCheck(bool useTimer, bool combatBuff, string onCallRoutine)
	/if (!${castWhileInvis} && ${Me.Invis}) /return ABORT_INVIS
	/if (!${Defined[useTimer]}) /declare useTimer bool local TRUE
	/if (!${Defined[combatBuff]}) /declare combatBuff bool local FALSE
	/if (!${Defined[onCallRoutine]}) /declare onCallRoutine string local NULL
	/if (${useTimer} && ${minBuffCheckTimer}) /return ABORT_TIMER
	/if (${selfBuffCount} == 0 && (!${Me.Pet.ID} || ${petBuffCount} == 0) && (${groupBuffCount} == 0 || !${Group.Members}) && (!${watchCount} || !${groupBuffCount}) && !${autoBuffCount}) /return ABORT_NOBUFFS

	/declare checkTypes					string local
	
	/if (!${priorityAutoBuffsTimer} && ${autoBuffCount} && ${doAutoBuffs}) /varset checkTypes ${checkTypes}AUTO|
	/if (!${prioritySelfBuffsTimer} && ${selfBuffCount} && ${doSelfBuffs}) /varset checkTypes ${checkTypes}SELF|
	/if (!${priorityGroupBuffsTimer} && ${groupBuffCount} && ${doGroupBuffs} && ${Group.Members}) /varset checkTypes ${checkTypes}GROUP|
	/if (!${priorityWatchBuffsTimer} && ${groupBuffCount} && ${watchCount} && ${doWatchBuffs}) /varset checkTypes ${checkTypes}WATCH|
	/if (!${priorityGroupPetBuffsTimer} && ${groupBuffCount} && ${doGroupBuffs} && ${Group.Members}) /varset checkTypes ${checkTypes}GROUPPET|
	/if (!${priorityPetBuffsTimer} && ${petBuffCount} && ${doPetBuffs} && ${Me.Pet.ID} && !${Me.Pet.Name.Find[familiar]}) /varset checkTypes ${checkTypes}PET|
	
	/if (${checkTypes.Length}) {
		/varset checkTypes |${checkTypes}
		/call buffCheckRoutine ${checkTypes} ${combatBuff} ${onCallRoutine}
	}
/return

Sub buffCheckNeeded(string checkBuffName, string checkBuffSpell, string targVariable, float gbMod)
	/if (!${Defined[gbMod]}) /declare gbMod int local 0
	/if (${checkBuffName.Equal[NULL]} && ${checkBuffSpell.Equal[NULL]}) /return FALSE
	/if (${checkBuffName.Equal[NULL]} && ${checkBuffSpell.NotEqual[NULL]}) /varset checkBuffName ${checkBuffSpell}
	/if (!${${targVariable}[${checkBuffName}].ID} || ${${targVariable}[${checkBuffName}].Duration.TotalSeconds} < ${Math.Calc[${Spell[${checkBuffSpell}].Duration.TotalSeconds}/10+${gbMod}]}) {
		| Does not have buff or less than 10% spell duration left
		/return TRUE
	}
/return FALSE

Sub buffCheckRoutine(string checkTypes, bool combatBuff, string onCallRoutine)
	/if (!${Defined[combatBuff]}) /declare combatBuff bool local FALSE
	/if (!${Defined[onCallRoutine]}) /declare onCallRoutine string local NULL
	/if (!${Defined[checkTypes]}) /declare checkTypes string local ALL
	
	/call EchoLog "Checking buffs (${checkTypes})- In Combat: ${combatBuff}" TRUE

	/declare bArray							int local 0
	/declare gArray							int local 0
	/declare needBuffCount[20]	int local
	/declare needBuffID[20]			int local
	/declare needBuffStack[20]	bool local
	/declare singWillStack			bool local
	/declare targVariable				string local Target.Buff
	/declare gbMod							int local 0
	/declare xArray							int local 0
	/declare aggroDetection			bool local FALSE
	/declare needsBuff					bool local FALSE
	/declare needsSingleBuff		bool local FALSE
	
	| ======================
	| Check for auto buffs
	| ======================
	| supports self only buffs right now
	/if (${doAutoBuffs} && ${autoBuffCount} && ${checkTypes.Find[AUTO]}) {
		/call EchoLog "Checking auto buffs (SELF only currently)" TRUE
		/varset buffMode BUFF_AUTO
		/for nArray 1 to ${autoBuffCount}
			/if (${useXTarget}) {
				/for xArray 1 to ${maxXTargets}
					/if (${Me.XTarget[${xArray}].${xTargType}.Equal[Auto Hater]} && ${Me.XTarget[${xArray}].ID}  && !${combatBuff}) {
						/varset aggroDetection TRUE
						/goto :exitBuffRoutine
					}
				/next xArray
			}
			/if (${autoBuffSlot[${nArray}].Find[Gem]}) {
				/if (${Me.Gem[${autoBuff[${nArray}]}]}) {
					/if (${Me.GemTimer[${autoBuff[${nArray}]}]} && ${Me.GemTimer[${autoBuff[${nArray}]}].Float} > 5) /goto :nextAutoBuff
				} else {
					/if (${Spell[${autoBuff[${nArray}]}].RecoveryTime.Float} > 5) {
						/memspell ${autoBuffSlot[${nArray}].Right[-3]} "${autoBuff[${nArray}]}"
						/delay 5
						/goto :nextAutoBuff
					}
				}
			}
			/if (${autoBuffSlot[${nArray}].Equal[Item]} && (!${FindItem[${autoBuff[${nArray}]}].ID} || ${FindItem[${autoBuff[${nArray}]}].Timer})) /goto :nextAutoBuff
			/if (${autoBuffSlot[${nArray}].Equal[alt]} && !${Me.AltAbilityReady[${autoBuff[${nArray}]}]}) /goto :nextAutoBuff
			/if (${autoBuffType[${nArray}].Equal[SELF]}) {
				/if (${autoBuff[${nArray}].NotEqual[NULL]} && ${autoBuffEnabled[${nArray}]} && ${autoBuffAuto[${nArray}]} && ${autoBuffCombat[${nArray}]}==${combatBuff}) {
					/if (${Spell[${autoBuff[${nArray}]}].Stacks} || ${Spell[${autoBuffText[${nArray}]}].Stacks}) {
						/if (${autoBuffText[${nArray}].NotEqual[NULL]}) {
							/if (${Me.Aura[1].Equal[${autoBuffText[${nArray}]}]} || ${Me.Aura[1].Equal[${autoBuff[${nArray}]}]} || ${Me.Aura[2].Equal[${autoBuffText[${nArray}]}]} || ${Me.Aura[2].Equal[${autoBuff[${nArray}]}]}) {
								| Work around for MQ2 aura bug
							} else {
								/if ((!${Me.Buff[${autoBuffText[${nArray}]}].ID} || ${Me.Buff[${autoBuffText[${nArray}]}].Duration} <= ${Math.Calc[${Spell[${autoBuff[${nArray}]}].Duration.TotalSeconds}/10]}) && !${Me.Song[${autoBuffText[${nArray}]}].ID}) {
									/call CastBuff ${Me.ID} ${nArray} AUTO
									/if (${Macro.Return.Equal[CAST_COMPONENTS]}) {
										/call EchoLog "Disabling auto buff ${autoBuff[${nArray}]} because you either don't have the subscription status for it, or don't have the components for it." TRUE
										/varset autoBuffEnabled[${nArray}] FALSE
									}
								}
							}
						} else {
							/if (${Me.Aura[1].Equal[${autoBuff[${nArray}]}]} || ${Me.Aura[2].Equal[${autoBuff[${nArray}]}]}) {
								| Work around for MQ2 aura bug
							} else {
								/if ((!${Me.Buff[${autoBuff[${nArray}]}].ID} ||  ${Me.Buff[${autoBuff[${nArray}]}].Duration} <= ${Math.Calc[${Spell[${autoBuff[${nArray}]}].Duration.TotalSeconds}/10]})) {
									/call CastBuff ${Me.ID} ${nArray} AUTO
									/if (${Macro.Return.Equal[CAST_COMPONENTS]}) {
										/call EchoLog "Disabling auto buff ${autoBuff[${nArray}]} because you either don't have the subscription status for it, or don't have the components for it." TRUE
										/varset autoBuffEnabled[${nArray}] FALSE
									}									
								}
							}
						}
						/if (${gotAggro}) /goto :exitBuffRoutine
					}
				}
			}
			:nextAutoBuff
			/doevents flush spellFade
			/doevents
		/next nArray
		/varset buffMode BUFF_NONE
		/varset priorityAutoBuffsTimer ${priorityAutoBuffs}
	}
		
	| ======================
	| Check for Group Buffs
	| ======================	
	/if (${Group.Members} && ${groupBuffCount} && ${doGroupBuffs} && ${checkTypes.Find[GROUP|]}) {
		/varset buffMode BUFF_GROUP
		/call EchoLog "Checking group members" TRUE
		/for nArray 1 to ${groupBuffCount}
			/if (${useXTarget}) {
				/for xArray 1 to ${maxXTargets}
					/if (${Me.XTarget[${xArray}].${xTargType}.Equal[Auto Hater]} && ${Me.XTarget[${xArray}].ID}  && !${combatBuff}) {
						/varset aggroDetection TRUE
						/goto :exitBuffRoutine
					}
				/next xArray
			}
			/if (${groupBuffSlot[${nArray}].Equal[Item]} && (${FindItem[${groupBuffGroup[${nArray}]}].Timer} || ${FindItem[${groupBuffSingle[${nArray}]}].Timer} || (!${FindItem[${groupBuffGroup[${nArray}]}].InvSlot} && !${FindItem[${groupBuffSingle[${nArray}]}].InvSlot}))) /goto :nextBuff
			/if (${groupBuffSlot[${nArray}].Equal[Alt]} && !${Me.AltAbilityReady[${groupBuffGroup[${nArray}]}]} && !${Me.AltAbilityReady[${groupBuffSingle[${nArray}]}]}) /goto :nextBuff
			/if (${groupBuffCombat[${nArray}]}==${combatBuff} && ${groupBuffAuto[${nArray}]} && ${groupBuffEnabled[${nArray}]}) {
				/varset gbMod 0
				/for gArray 0 to ${Group.Members}
					/if (${Group.Member[${gArray}].ID} && ${Group.Member[${gArray}].State.NotEqual[HOVER]} && ${Group.Member[${gArray}].Type.NotEqual[Corpse]} && ${Group.Member[${gArray}].Distance} < ${campRadius} && (!${Group.Member[${gArray}].Linkdead} || ${Group.Member[${gArray}].Type.Equal[MERCENARY]})) {
						/if (${groupBuffTargets[${nArray}].Find[${Group.Member[${gArray}].Class.ShortName}]} || ${groupBuffTargets[${nArray}].Find[${Group.Member[${gArray}].Class.Name}]} || ${groupBuffTargets[${nArray}].Find[${Group.Member[${gArray}].CleanName}]} || ((${groupBuffTargets[${nArray}].Find[mt]} || ${groupBuffTargets[${nArray}].Find[maintank]}) && (${Group.Member[${gArray}].MainTank} || ${Group.Member[${gArray}].CleanName.Equal[${mainTank}]}))) {
							/if (${useEQBC} && ${NetBots[${Group.Member[${gArray}].CleanName}].Zone}) {
								/varset targVariable NetBots[${Group.Member[${gArray}].CleanName}].Buff
							} else {
								/squelch /target id ${Group.Member[${gArray}].ID}
								/delay 1s ${Target.ID}==${Group.Member[${gArray}].ID}
								/delay 3
								/delay 2s ${Target.BuffCount}
								/varset targVariable Target.Buff
							}
							/call buffCheckNeeded "${groupBuffGroupDisplay[${nArray}]}" "${groupBuffGroup[${nArray}]}" ${targVariable} ${gbMod}
							/varset needsBuff ${Macro.Return}
							/call buffCheckNeeded "${groupBuffSingleDisplay[${nArray}]}" "${groupBuffSingle[${nArray}]}" ${targVariable} ${gbMod}
							/varset needsSingleBuff FALSE
							/if (${needsBuff}) {
								/call EchoLog "${Group.Member[${gArray}].CleanName} is missing ${groupBuffGroup[${nArray}]}" TRUE
								/if (${groupBuffSingleDisplay[${nArray}].NotEqual[NULL]}) {
									/call buffCheckStacking groupBuff ${nArray} ${targVariable} Single
									/varset singWillStack ${Macro.Return}
								} else {
									/varset singWillStack FALSE
								}
								/call buffCheckStacking groupBuff ${nArray} ${targVariable} Group
								/if (${Macro.Return.Equal[FALSE]}) /goto :nextGroupMember
								
								/varcalc needBuffCount[${nArray}] ${needBuffCount[${nArray}]} + 1
								/if (${needBuffCount}==1) /varcalc gbMod (${Spell[${groupBuffGroup[${nArray}]}].Duration.TotalSeconds} / 10) * 2
								/varset needBuffID[${needBuffCount[${nArray}]}] ${Group.Member[${gArray}].ID}
								/varset needBuffStack[${needBuffCount[${nArray}]}] ${singWillStack}
								/if (${groupBuffSingle[${nArray}].Equal[NULL]} || ${needBuffCount[${nArray}]} >= ${groupBuffGroupMin[${nArray}]}) {
									/call CastBuff ${Group.Member[${gArray}].ID} ${nArray} Group Group FALSE ${onCallRoutine}
									/if (${Macro.Return.Equal[ABORT_AGGRO]}) /goto :exitBuffRoutine
									/varset needBuffCount[${nArray}] 0
									/if (${combatBuff} && ${onCallRoutine.NotEqual[NULL]}) /call ${onCallRoutine}
									/goto :nextBuff
								}
							} else /if (${needsSingleBuff}) {
								/call EchoLog "${Group.Member[${gArray}].CleanName} is missing ${groupBuffSingle[${nArray}]}" TRUE
								/call buffCheckStacking groupBuff ${nArray} ${targVariable} Single
								/if (${Macro.Return.Equal[FALSE]}) /goto :nextGroupMember

								/call CastBuff ${Group.Member[${gArray}].ID} ${nArray} Group Single FALSE ${onCallRoutine}
								/if (${Macro.Return.Equal[ABORT_AGGRO]}) /goto :exitBuffRoutine
								/if (${combatBuff} && ${onCallRoutine.NotEqual[NULL]}) /call ${onCallRoutine}
							}
						}
					}
					/if (!${groupBuffEnabled[${nArray}]}) /goto :nextBuff
					:nextGroupMember
				/next gArray
				/if (${needBuffCount[${nArray}]}) {
					/for bArray 1 to ${needBuffCount[${nArray}]}
						/if (${needBuffStack[${bArray}]}) {
							/call CastBuff ${needBuffID[${bArray}]} ${nArray} Group Single FALSE ${onCallRoutine}
							/if (${Macro.Return.Equal[ABORT_AGGRO]}) /goto :exitBuffRoutine
						}
					/next bArray
					/if (${combatBuff} && ${onCallRoutine.NotEqual[NULL]}) /call ${onCallRoutine}
				}
			}
			/doevents flush spellFade
			/doevents
			:nextBuff
		/next nArray
		/varset buffMode BUFF_NONE
		/varset priorityGroupBuffsTimer ${priorityGroupBuffs}
	}

	| ==========================
	| Check for Group Pet Buffs
	| ==========================
	/if (${Group.Members} && ${groupBuffCount} && ${doGroupBuffs} && ${checkTypes.Find[GROUPPET]}) {
		/varset buffMode BUFF_GROUPPET
		/call EchoLog "Checking group member's pets" TRUE
		/for nArray 1 to ${groupBuffCount}
			/if (${useXTarget}) {
				/for xArray 1 to ${maxXTargets}
					/if (${Me.XTarget[${xArray}].${xTargType}.Equal[Auto Hater]} && ${Me.XTarget[${xArray}].ID}  && !${combatBuff}) {
						/varset aggroDetection TRUE
						/goto :exitBuffRoutine
					}
				/next xArray
			}
			/if (${groupBuffSlot[${nArray}].Equal[Item]} && (${FindItem[${groupBuffGroup[${nArray}]}].Timer} || ${FindItem[${groupBuffSingle[${nArray}]}].Timer})) /goto :nextGroupPetBuff
			/if (${groupBuffSlot[${nArray}].Equal[Alt]} && !${Me.AltAbilityReady[${groupBuffGroup[${nArray}]}]} && !${Me.AltAbilityReady[${groupBuffSingle[${nArray}]}]}) /goto :nextGroupPetBuff
			/if (${groupBuffCombat[${nArray}]}==${combatBuff} && ${groupBuffAuto[${nArray}]} && ${groupBuffEnabled[${nArray}]}) {
				/for gArray 0 to ${Group.Members}
					/if (${Group.Member[${gArray}].Pet.ID} && !${Group.Member[${gArray}].Pet.Name.Find[familiar]}) {
						/if (${groupBuffTargets[${nArray}].Find[Pet]} || ${groupBuffTargets[${nArray}].Find[${Group.Member[${gArray}].CleanName}-Pet]} || ${groupBuffTargets[${nArray}].Find[${Group.Member[${gArray}].Class}-Pet]} || ${groupBuffTargets[${nArray}].Find[${Group.Member[${gArray}].Class.ShortName}-Pet]}) {
							/if (${useEQBC} && ${NetBots[${Group.Member[${gArray}].CleanName}].Zone}) {
								/varset targVariable NetBots[${Group.Member[${gArray}].CleanName}].PetBuff
							} else {
								/squelch /target id ${Group.Member[${gArray}].Pet.ID}
								/delay 1s ${Target.ID}==${Group.Member[${gArray}].Pet.ID}
								/delay 3
								/delay 2s ${Target.BuffCount}
								/varset targVariable Target.Buff
							}
							| /delay 4
							/call buffCheckNeeded "${groupBuffGroupDisplay[${nArray}]}" "${groupBuffGroup[${nArray}]}" ${targVariable}
							/varset needsBuff ${Macro.Return}
							/call buffCheckNeeded "${groupBuffSingleDisplay[${nArray}]}" "${groupBuffSingle[${nArray}]}" ${targVariable}
							/varset needsSingleBuff FALSE

							/if (${needsBuff}) {
									/call EchoLog "${Group.Member[${gArray}].Pet.CleanName} is missing ${groupBuffGroup[${nArray}]}" TRUE
									
									/varset singWillStack FALSE
									/call buffCheckStacking groupBuff ${nArray} ${targVariable} Group
									/if (${Macro.Return.Equal[FALSE]}) /goto :nextGroupMemberPet

									/call CastBuff ${Group.Member[${gArray}].Pet.ID} ${nArray} Group Group FALSE ${onCallRoutine}
									/if (${Macro.Return.Equal[ABORT_AGGRO]}) /goto :exitBuffRoutine
									/varset needBuffCount[${nArray}] 0
									/if (${combatBuff} && ${onCallRoutine.NotEqual[NULL]}) /call ${onCallRoutine}
									/goto :nextGroupPetBuff
							} else /if (${needsSingleBuff}) {
									/call EchoLog "${Group.Member[${gArray}].Pet.CleanName} is missing ${groupBuffSingle[${nArray}]}" TRUE
									/call buffCheckStacking groupBuff ${nArray} ${targVariable} Single
									/if (${Macro.Return.Equal[FALSE]}) /goto :nextGroupMemberPet

									/call CastBuff ${Group.Member[${gArray}].Pet.ID} ${nArray} Group Single FALSE ${onCallRoutine}
									/if (${Macro.Return.Equal[ABORT_AGGRO]}) /goto :exitBuffRoutine
									/if (${combatBuff} && ${onCallRoutine.NotEqual[NULL]}) /call ${onCallRoutine}
							}
						}
					}
					/if (!${groupBuffEnabled[${nArray}]}) /goto :nextGroupPetBuff
					:nextGroupMemberPet
				/next gArray
			}
			/doevents flush spellFade
			/doevents
			:nextGroupPetBuff
		/next nArray
		/varset buffMode BUFF_NONE
		/varset priorityGroupPetBuffsTimer ${priorityGroupPetBuffs}
	}
		
	| =========================
	| Check for watched buffs
	| =========================	
	/if (${watchCount} && ${groupBuffCount} && ${doWatchBuffs} && ${checkTypes.Find[WATCH]}) {
		/varset buffMode BUFF_WATCH
		/call EchoLog "Checking watch list" TRUE
		/for gArray 1 to ${watchCount}
			/if (${useXTarget}) {
				/for xArray 1 to ${maxXTargets}
					/if (${Me.XTarget[${xArray}].${xTargType}.Equal[Auto Hater]} && ${Me.XTarget[${xArray}].ID} && !${combatBuff}) {
						/varset aggroDetection TRUE
						/goto :exitBuffRoutine
					}
				/next xArray
			}
			/if (${Spawn[${watchList[${gArray}]}].ID}) {
				/if (${useEQBC} && ${NetBots[${watchList[${gArray}]}].Zone}) {
					/varset targVariable NetBots[${watchList[${gArray}]}].Buff
				} else {
					/squelch /target ID ${Spawn[${watchList[${gArray}]}].ID}
					/delay 1s ${Target.ID}==${Spawn[${watchList[${gArray}]}].ID}
					/delay 3
					/delay 2s ${Target.BuffCount}
					/varset targVariable Target.Buff
				}
				/if (${Spawn[${watchList[${gArray}]}].Type.NotEqual[Corpse]} && ${Spawn[${watchList[${gArray}]}].Distance} < ${campRadius} && (!${Spawn[${watchList[${gArray}]}].Linkdead} || ${Spawn[${watchList[${gArray}]}].Type.Equal[MERCENARY]})) {
					/for nArray 1 to ${groupBuffCount}
						/if (${groupBuffTargets[${nArray}].Find[${Spawn[${watchList[${gArray}]}].Class.ShortName}]} || ${groupBuffTargets[${nArray}].Find[${Spawn[${watchList[${gArray}]}].Class.Name}]} || ${groupBuffTargets[${nArray}].Find[${Spawn[${watchList[${gArray}]}].CleanName}]} || ((${groupBuffTargets[${nArray}].Find[mt]} || ${groupBuffTargets[${nArray}].Find[maintank]}) && ${Spawn[${watchList[${gArray}]}].CleanName.Equal[${mainTank}]})) {
							/if (${groupBuffSlot[${nArray}].Equal[Item]} && (${FindItem[${groupBuffGroup[${nArray}]}].Timer} || ${FindItem[${groupBuffSingle[${nArray}]}].Timer})) /goto :nextWBuff
							/if (${groupBuffSlot[${nArray}].Equal[Alt]} && !${Me.AltAbilityReady[${groupBuffGroup[${nArray}]}]} && !${Me.AltAbilityReady[${groupBuffSingle[${nArray}]}]}) /goto :nextWBuff
							/if (${groupBuffCombat[${nArray}]}==${combatBuff} && ${groupBuffAuto[${nArray}]} && ${groupBuffEnabled[${nArray}]}) {
								/call buffCheckNeeded "${groupBuffGroupDisplay[${nArray}]}" "${groupBuffGroup[${nArray}]}" ${targVariable}
								/varset needsBuff ${Macro.Return}
								/call buffCheckNeeded "${groupBuffSingleDisplay[${nArray}]}" "${groupBuffSingle[${nArray}]}" ${targVariable}
								/varset needsSingleBuff FALSE
								/if (${needsBuff}) {
								| /if (${groupBuffGroup[${nArray}].NotEqual[NULL]}) {
									| /if (((${groupBuffGroupDisplay[${nArray}].NotEqual[NULL]} && (!${${targVariable}[${groupBuffGroupDisplay[${nArray}]}].ID} || ${${targVariable}[${groupBuffGroupDisplay[${nArray}]}].Duration.Float} < ${Math.Calc[${Spell[${groupBuffGroup[${nArray}]}].Duration.Float}/10]})) || (${groupBuffGroupDisplay[${nArray}].Equal[NULL]} && (!${${targVariable}[${groupBuffGroup[${nArray}]}].ID} || (${${targVariable}[${groupBuffGroup[${nArray}]}].Duration.Float} < ${Math.Calc[${Spell[${groupBuffGroup[${nArray}]}].Duration.Float}/10]} || ${${targVariable}[${groupBuffGroupDisplay[${nArray}]}].Duration.Float} < ${Math.Calc[${Spell[${groupBuffGroupDisplay[${nArray}]}].Duration.Float}/10]})))) && (${groupBuffSingle[${nArray}].Equal[NULL]} || (${groupBuffSingleDisplay[${nArray}].NotEqual[NULL]} && (!${${targVariable}[${groupBuffSingleDisplay[${nArray}]}].ID} || ${${targVariable}[${groupBuffSingleDisplay[${nArray}]}].Duration.Float} < ${Math.Calc[${Spell[${groupBuffGroup[${nArray}]}].Duration.Float}/10]})) || (${groupBuffSingleDisplay[${nArray}].Equal[NULL]} && (!${${targVariable}[${groupBuffSingle[${nArray}]}].ID} || (${${targVariable}[${groupBuffSingle[${nArray}]}].Duration.Float} < ${Math.Calc[${Spell[${groupBuffSingle[${nArray}]}].Duration.Float}/10]} || ${${targVariable}[${groupBuffSingleDisplay[${nArray}]}].Duration.Float} < ${Math.Calc[${Spell[${groupBuffSingleDisplay[${nArray}]}].Duration.Float}/10]}))))) {
										/call EchoLog "${Spawn[${watchList[${gArray}]}].CleanName} is missing ${groupBuffGroup[${nArray}]}" TRUE
										/call buffCheckStacking groupBuff ${nArray} ${targVariable} Group
										/if (${Macro.Return.Equal[FALSE]}) /goto :nextWatchMember

										/call CastBuff ${Spawn[${watchList[${gArray}]}].ID} ${nArray} Group Group FALSE ${onCallRoutine}
										/if (${Macro.Return.Equal[ABORT_AGGRO]}) /goto :exitBuffRoutine
										/if (${combatBuff} && ${onCallRoutine.NotEqual[NULL]}) /call ${onCallRoutine}
										/goto :nextWBuff
									| }
								} else /if (${needsSingleBuff}) {
								| } else /if (${groupBuffSingle[${nArray}].NotEqual[NULL]}) {
									| /if ((${groupBuffSingleDisplay[${nArray}].NotEqual[NULL]} && (!${${targVariable}[${groupBuffSingleDisplay[${nArray}]}].ID} || ${${targVariable}[${groupBuffSingleDisplay[${nArray}]}].Duration.Float} < ${Math.Calc[${Spell[${groupBuffSingle[${nArray}]}].Duration.Float}/10]})) || (${groupBuffSingleDisplay[${nArray}].Equal[NULL]} && (!${${targVariable}[${groupBuffSingle[${nArray}]}].ID} || (${${targVariable}[${groupBuffSingle[${nArray}]}].Duration.Float} < ${Math.Calc[${Spell[${groupBuffSingle[${nArray}]}].Duration.Float}/10]} || ${${targVariable}[${groupBuffSingleDisplay[${nArray}]}].Duration.Float} < ${Math.Calc[${Spell[${groupBuffSingleDisplay[${nArray}]}].Duration.Float}/10]})))) {
										/call EchoLog "${Spawn[${watchList[${gArray}]}].CleanName} is missing ${groupBuffSingle[${nArray}]}" TRUE
										/call buffCheckStacking groupBuff ${nArray} ${targVariable} Single
										/if (${Macro.Return.Equal[FALSE]}) /goto :nextWatchMember

										/call CastBuff ${Spawn[${watchList[${gArray}]}].ID} ${nArray} Group Single FALSE ${onCallRoutine}
										/if (${Macro.Return.Equal[ABORT_AGGRO]}) /goto :exitBuffRoutine
										/if (${combatBuff} && ${onCallRoutine.NotEqual[NULL]}) /call ${onCallRoutine}
									| }							
								}						
							}
						}
						/doevents flush spellFade
						/doevents
						:nextWBuff
					/next nArray
				}
			}
			:nextWatchMember
		/next gArray
		/varset buffMode BUFF_NONE
		/varset priorityWatchBuffsTimer ${priorityWatchBuffs}
	}
	
	| ======================
	| Check for self buffs
	| ======================
	/if (${doSelfBuffs} && ${selfBuffCount}>0 && ${checkTypes.Find[SELF]}) {
		/call EchoLog "Checking self buffs" TRUE
		/varset buffMode BUFF_SELF
		/for nArray 1 to ${selfBuffCount}
			/if (${useXTarget}) {
				/for xArray 1 to ${maxXTargets}
					/if (${Me.XTarget[${xArray}].${xTargType}.Equal[Auto Hater]} && ${Me.XTarget[${xArray}].ID}  && !${combatBuff}) {
						/varset aggroDetection TRUE
						/goto :exitBuffRoutine
					}
				/next xArray
			}
			/if (${buffSlot[${nArray}].Equal[Item]} && ${FindItem[${buffName[${nArray}]}].Timer}) /goto :nextSelfBuff
			/if (${buffSlot[${nArray}].Equal[alt]} && !${Me.AltAbilityReady[${buffName[${nArray}]}]}) /goto :nextSelfBuff
			/if (${buffName[${nArray}].NotEqual[NULL]} && ${buffEnabled[${nArray}]} && ${buffAuto[${nArray}]} && ${buffCombat[${nArray}]}==${combatBuff}) {
				/call buffCheckStacking buff ${nArray} Me.Buff
				/if (${Macro.Return.Equal[TRUE]}) {
					/if (${buffText[${nArray}].NotEqual[NULL]}) {
						/if (${Me.Aura[1].Equal[${buffText[${nArray}]}]} || ${Me.Aura[1].Equal[${buffName[${nArray}]}]} || ${Me.Aura[2].Equal[${buffText[${nArray}]}]} || ${Me.Aura[2].Equal[${buffName[${nArray}]}]}) {
							| Work around for MQ2 aura bug
						} else {
							/if ((!${Me.Buff[${buffText[${nArray}]}].ID} || ${Me.Buff[${buffText[${nArray}]}].Duration} <= ${Math.Calc[${Spell[${buffName[${nArray}]}].Duration.TotalSeconds}/10]}) && !${Me.Song[${buffText[${nArray}]}].ID}) /call CastBuff ${Me.ID} ${nArray} SELF
						}
					} else {
						/if (${Me.Aura[1].Equal[${buffName[${nArray}]}]} || ${Me.Aura[2].Equal[${buffName[${nArray}]}]}) {
							| Work around for MQ2 aura bug
						} else {
							/if ((!${Me.Buff[${buffName[${nArray}]}].ID} ||  ${Me.Buff[${buffName[${nArray}]}].Duration} <= ${Math.Calc[${Spell[${buffName[${nArray}]}].Duration.TotalSeconds}/10]})) /call CastBuff ${Me.ID} ${nArray} SELF
						}
					}
					/if (${gotAggro}) /goto :exitBuffRoutine
				}
			}
			:nextSelfBuff
			/doevents flush spellFade
			/doevents
		/next nArray
		/varset buffMode BUFF_NONE
		/varset prioritySelfBuffsTimer ${prioritySelfBuffs}
	}

	| ======================
	| Check for pet buffs
	| ======================
	/if (${doPetBuffs} && ${petBuffCount}>0 && ${Me.Pet.ID} && !${Me.Pet.Name.Find[familiar]} && ${checkTypes.Find[|PET|]}) {
		/varset buffMode BUFF_PET
		/call EchoLog "Checking pet buffs" TRUE
		/for nArray 1 to ${petBuffCount}
			/for xArray 1 to ${maxXTargets}
				/if (${Me.XTarget[${xArray}].${xTargType}.Equal[Auto Hater]} && ${Me.XTarget[${xArray}].ID}  && !${combatBuff}) {
					/varset aggroDetection TRUE
					/goto :exitBuffRoutine
				}
			/next xArray
			/if (${petBuffSlot[${nArray}].Equal[Item]} && ${FindItem[${petBuff[${nArray}]}].Timer}) /goto :nextPetBuff
			/if (${petBuffSlot[${nArray}].Equal[alt]} && !${Me.AltAbilityReady[${petBuff[${nArray}]}]}) /goto :nextPetBuff				
			/if (!${petBuff[${nArray}].Equal[NULL]} && ${petBuffEnabled[${nArray}]} && ${petBuffAuto[${nArray}]} && ${petBuffCombat[${nArray}]}==${combatBuff}) {
				/call buffCheckStacking petBuff ${nArray} Me.PetBuff
				/if (${Macro.Return.Equal[TRUE]}) {
					/if (!${petBuffText[${nArray}].Equal[NULL]}) {
						/if (!${Me.PetBuff[${petBuffText[${nArray}]}]}) /call CastBuff ${Me.Pet.ID} ${nArray} Pet
					} else {
						/if (!${Me.PetBuff[${petBuff[${nArray}]}]}) /call CastBuff ${Me.Pet.ID} ${nArray} Pet
					}
					/if (${gotAggro}) /goto :exitBuffRoutine
				}
			}
			:nextPetBuff
			/doevents flush spellFade
			/doevents
		/next nArray
		/varset buffMode BUFF_NONE
		/varset priorityPetBuffsTimer ${priorityPetBuffs}
	}
	
	:exitBuffRoutine
	| /varset minBuffCheckTimer ${minBuffCheckTimerStr}
	/if (!${gotAggro} && !${aggroDetection}) {
		/call EchoLog "Buff check Completed" TRUE
	} else {
		/varset minBuffCheckTimer 30s
		/call EchoLog "Buff check aborted due to aggro, all buff checking will be delayed for 30 seconds" TRUE
		/varset buffMode BUFF_NONE
		/return ABORT_AGGRO
	}
	/doevents flush spellFade
/return COMPLETED_SUCCESS

Sub buffCheckStacking(string buffType, int buffID, string buffTargetVar, string buffSubType)
	/declare bArray					int local 0
	/declare sArray					int local 0
	/declare buffNameSpell	string local NULL
	/declare buffNameBaseSpell	string local NULL
	/declare buffNameRank		int local 0
	/declare buffTextSpell	string local NULL
	/declare buffTextBaseSpell	string local NULL
	| /declare buffTextRank		int local 0
	/declare currSpell 			string local NULL
	/declare currRank				int local 0
	/declare sBuffName			string local
	/declare sBuffText			string local
	/declare sBuffSlot			string local
	/declare textVar				string local Text
	/declare nameVar				string local 
	/declare sSpell					string local NULL
	/declare sRank					int local 0
	/declare thisStack			bool local FALSE
	/declare thisSpellName	string local
	/declare currBaseSpell	string local NULL
	
	/if (!${Defined[buffSubType]}) /declare buffSubType string local 
	/if (${buffType.Find[group]}) /varset textVar Display
	/if (${buffType.Equal[buff]}) /varset nameVar Name
	
	/varset sBuffName ${${buffType}${buffSubType}${nameVar}[${buffID}]}
	/varset sBuffText ${${buffType}${buffSubType}${textVar}[${buffID}]}
	/varset sBuffSlot ${${buffType}Slot[${buffID}]}

	/if (${sBuffSlot.Find[Gem]} && !${sBuffName.Find[Rk]}) /varset sBuffName ${sBuffName} Rk. I	
	/call GetSpellName "${sBuffName}"
	/varset buffNameSpell ${Macro.Return}
	/call GetSpellName "${buffNameSpell}"
	/varset buffNameBaseSpell ${Macro.Return}
	| Make sure we have the right spell from the book if this is a casting spell
	/if (${sBuffSlot.Find[Gem]}) {
		/if (${Me.Book[${buffNameSpell}]}) /varset sBuffName ${buffNameSpell} Rk. I
		/if (${Me.Book[${buffNameSpell} Rk. II]}) /varset sBuffName ${buffNameSpell} Rk. II
		/if (${Me.Book[${buffNameSpell} Rk. III]}) /varset sBuffName ${buffNameSpell} Rk. III
		/if (${Me.Book[${buffNameSpell} Rk II]}) /varset sBuffName ${buffNameSpell} Rk II
		/if (${Me.Book[${buffNameSpell} Rk III]}) /varset sBuffName ${buffNameSpell} Rk III
	}
	/call GetSpellRank "${sBuffName}"
	/varset buffNameRank ${Macro.Return}
	
	/if (!${sBuffText.Length} || ${sBuffText.Equal[NULL]}) /varset sBuffText ${sBuffName}
	/if (${sBuffSlot.Find[Gem]} && !${sBuffText.Find[Rk]}) /varset sBuffText ${sBuffText} Rk. I	
	/call GetSpellName "${sBuffText}"
	/varset buffTextSpell ${Macro.Return}
	/call GetSpellName "${buffTextSpell}"
	/varset buffTextBaseSpell ${Macro.Return}

	/for bArray 1 to 50
		/if (${${buffTargetVar}[${bArray}].ID}) {
			/varset thisSpellName ${${buffTargetVar}[${bArray}].Name}
			/call GetSpellName "${thisSpellName}"
			/varset currSpell ${Macro.Return}
			/call GetSpellName "${currSpell}"
			/varset currBaseSpell ${Macro.Return}
			/call GetSpellRank "${thisSpellName}"
			/varset currRank ${Macro.Return}
			
			/if (${currBaseSpell.NotEqual[${currSpell}]}) {
				/if (${currSpell.Equal[${buffTextSpell}]} && ${currRank} > ${buffNameRank}) {
					/call EchoLog ".....${sBuffName} will not stack because there is a higher version of it present" TRUE
					/return FALSE
				} else /if (${currSpell.Equal[${buffTextSpell}]} && ${currRank} <= ${buffNameRank}) {
					/return TRUE
				}
			} else /if (${currBaseSpell.Equal[${buffTextBaseSpell}]} && ${buffTextBaseSpell.Equal[${buffTextSpell}]} && ${currRank} > ${buffNameRank}) {
				/call EchoLog ".....${sBuffName} will not stack because there is a higher version of it present" TRUE
				/return FALSE
			} else /if (${currBaseSpell.Equal[${buffTextBaseSpell}]} && ${buffTextBaseSpell.Equal[${buffTextSpell}]} && ${currRank} <= ${buffNameRank}) {
				/return TRUE
			}
			
			/if (${${buffType}DNSCount[${buffID}]}) {
				/for sArray 1 to ${${buffType}DNSCount[${buffID}]}
					/call GetSpellName "${${buffType}DoNotStack[${buffID},${sArray}]}"
					/varset sSpell ${Macro.Return}
					/if (${currSpell.Equal[${sSpell}]} || ${currSpell.Equal[${${buffType}DoNotStack[${buffID},${sArray}]}]} || ${currBaseSpell.Equal[${sSpell}]} || ${currBaseSpell.Equal[${${buffType}DoNotStack[${buffID},${sArray}]}]}) {
						/call EchoLog ".....${sBuffName} will not stack because ${${buffType}DoNotStack[${buffID},${sArray}]} is in the Do Not Stack settings for this buff" TRUE
						/return FALSE
					}
				/next sArray
			}
			
			/if ((${sBuffSlot.NotEqual[Item]} && ${Spell[${buffNameSpell}].WillStack[${thisSpellName}]}) || (${sBuffSlot.Equal[Item]} && ${FindItem[${sBuffName}].Spell.WillStack[${thisSpellName}]})) {
			} else {
				| Check the stack override list
				/if (${${buffType}SCount[${buffID}]}) {
					/varset thisStack FALSE
					/for sArray 1 to ${${buffType}SCount[${buffID}]}
						/call GetSpellName "${${buffType}Stack[${buffID},${sArray}]}"
						/varset sSpell ${Macro.Return}
						/call GetSpellRank "${${buffType}Stack[${buffID},${sArray}]}"
						/varset sRank ${Macro.Return}
						/if (${currSpell.Equal[${sSpell}]} || ${currSpell.Equal[${${buffType}DoNotStack[${buffID},${sArray}]}]} || ${currBaseSpell.Equal[${sSpell}]} || ${currBaseSpell.Equal[${${buffType}DoNotStack[${buffID},${sArray}]}]}) {
							/call EchoLog ".....MQ2 reports ${sBuffName} will not stack with ${sSpell}, but it is in the Stack list so allowing it." TRUE
							/varset thisStack TRUE
						}
					/next sArray
					/if (!${thisStack}) {
						/call EchoLog ".....${sBuffName} will not stack with ${thisSpellName} according to MQ2" TRUE
						/return FALSE
					}
				} else {
					/call EchoLog ".....${sBuffName} will not stack with ${thisSpellName} according to MQ2" TRUE
					/return FALSE
				}
			}
		}
	/next bArray
/return TRUE

Sub campCheck(bool pauseCheck)
	/declare inum 		int local
	/declare isSafe 	bool local
	/declare pcID			int local
	/declare pcNum		int local 0
	/declare campSafe	bool local
	/if (!${Defined[pauseCheck]}) /declare pauseCheck bool local FALSE
	/if (!${Defined[unsafePCLastCheckCombat]}) /declare unsafePCLastCheckCombat bool outer ${inCombat}
	
	/if (${Spawn[GM].ID}) {
		/varset campSafe FALSE
		/if (${GMAction.NotEqual[continue]}) /call EchoLog "\arGM Detected in zone!"
		/squelch /exec "${alertGM}" bg
		/if (${pauseCheck}) /return ${campSafe}
		/if (${GMAction.Equal[pause]}) {
			/if (!${priorityUnsafePCTimer}) /call EchoLog ".....Action is set to pause, will pause all actions until clear." TRUE
			/call pauseActions
			/varset campSafe TRUE
		} else /if (${GMAction.Equal[stop]}) {
			/call EchoLog ".....Action is set to stop, will stop the macro now." TRUE
			/endmacro
		} else /if (${GMAction.Equal[Gate]}) {
			/call EchoLog ".....Action is set to gate, gating.  Camping: ${campAfterGate}" TRUE
			/call Evac ${campAfterGate}
		} else /if (${GMAction.Equal[GateCamp]}) {
			/call EchoLog ".....Action is set to gate and camp.  Gating and camping now." TRUE
			/call Evac TRUE
		} else /if (${GMAction.Equal[Camp]}) {
			/call EchoLog ".....Action is set to camp out, camping out now." TRUE
			/call Evac TRUE TRUE
		} else /if (${GMAction.Equal[Quit]}) {
			/call EchoLog ".....Action is set to quit, quiting out now." TRUE
			/q
			/endm
		} else /if (${GMAction.Equal[Continue]}) {
			/if (!${priorityUnsafePCTimer}) /call EchoLog ".....Action is set to continue." TRUE
		}
	}
	
	/if (${priorityUnsafePCTimer} && ((${inCombat} && ${unsafePCLastCheckCombat}) || (!${inCombat} && !${unsafePCLastCheckCombat}))) /return
	/varset unsafePCLastCheckCombat ${inCombat}
	
	/varset campSafe TRUE
	:safePCCheck
	/varset pcNum ${Math.Calc[${pcNum}+1]}
	/varset pcID ${NearestSpawn[${pcNum},pc loc ${homeX} ${homeY} radius ${unsafePCRadius} notid ${Me.ID}].ID}
	/if (${pcID}) {
		/call SafePC ${Spawn[${pcID}].CleanName}
		/varset isSafe ${Macro.Return}
		/if (!${isSafe}) {
			/varset campSafe FALSE
			/call EchoLog "Camp check detected ${Spawn[${pcID}].Name} near us." TRUE
			/squelch /exec "${alertUnsafePC}" bg
			/if (${pauseCheck}) /return ${campSafe}
			/if (${inCombat} && !${unsafePCImmediate} && ${unsafePCAction.NotEqual[continue]}) {
				/varset priorityUnsafePCTimer ${priorityUnsafePC}
				/call EchoLog ".....In combat, finishing combat first" TRUE
				/return ${campSafe}
			}
			/if (${pullMode} && ${unsafePCReturn} && ${pullState.NotEqual[NULL]}) {
				/call EchoLog ".....Currently pulling (PullState: ${pullState}) and set to return to camp first, attempting to do so" TRUE
				/return FALSE
			}
			/if (${unsafePCAction.Equal[stop]}) {
				/call EchoLog ".....Action is set to stop, will stop the macro now" TRUE
				/endmacro
			} else /if (${unsafePCAction.Equal[gate]}) {
				/call EchoLog ".....Action is set to gate, attempting to gate and camp" TRUE
				/call Evac ${campAfterGate}
			} else /if (${unsafePCAction.Equal[continue]}) {
				/call EchoLog ".....Action is set to continue" TRUE
			} else /if (${unsafePCAction.Equal[camp]}) {
				/call EchoLog ".....Action is set to camp out" TRUE
				/call Evac ${campAfterGate} TRUE
			} else {
				/call EchoLog ".....Action is set to pause, will pause all actions until clear." TRUE
				/call pauseActions
				/varset campSafe TRUE
			}
		} else {
			/goto :safePCCheck
		}
	}
	/varset priorityUnsafePCTimer ${priorityUnsafePC}
/return ${campSafe}

Sub CastSpell(string spellType, bool combatMem, bool faceTarg, bool AllowDiscs) 
	/if (${medMode}) {
		/varset priorityCombatSpellsTimer ${priorityCombatSpells}
		/return ABORT_MEDMODE
	}
	/if (!${castWhileInvis} && ${Me.Invis}) /return ABORT_INVIS
	/if (${priorityCombatSpellsTimer}) /return ABORT_TIMER
	/if (!${Defined[faceTarg]}) /declare faceTarg				bool local ${faceWhenCasting}
	/if (!${doCombatSpells}) /return ABORT_DISABLED
	/if (${Me.Moving}) /return ABORT_MOVING
	/declare cCondition		bool local
	/declare retValue			string local
	/declare sTypeNum			int local 0
	/declare hKeyNum			int local 0
	/declare hKeyBar			int local 0
	/declare hKeyNumStr		string local
	/declare hKeyBarStr		string local
	/declare hKeyTemp			string local
	/declare restartTwist	bool local FALSE
	/declare iLoop				int local 0
	
	/if (!${Defined[AllowDiscs]}) /declare AllowDiscs bool local TRUE
	/if (!${Defined[spellType]} || ${spellType.Equal[NULL]}) {
		/if (!${Defined[spellType]}) /declare spellType string local
		/if (${spellTypeValue}) {
			/varset sTypeNum 1
			/varset spellType ${spellTypes[${sTypeNum}]}
		} else {
			/varset spellType ALL
		}
	}
	/if (!${Defined[combatMem]}) /declare combatMem bool local TRUE
	/if (!${csCount}) /return ABORT_NOSPELLS
	/if (${globalCooldown}) /return ABORT_COOLDOWN
	/if (${giftSpell}) {
		/if (${Me.Song[Gift of Mana (70)].ID}) /varset spellType GOM70
		/if (${Me.Song[Gift of Mana (75)].ID}) /varset spellType GOM75
		/if (${Me.Song[Gift of Mana (80)].ID}) /varset spellType GOM80
		/if (${Me.Song[Gift of Mana (85)].ID}) /varset spellType GOM85
		/if (${Me.Song[Gift of Mana (90)].ID}) /varset spellType GOM90
		/if (${Me.Song[Gift of Mana (95)].ID}) /varset spellType GOM95
		/if (${Me.Song[Gift of Mana (100)].ID}) /varset spellType GOM100
		/if (${Me.Song[Gift of Mana (105)].ID}) /varset spellType GOM105
		/if (${Me.Song[Gift of Mana (110)].ID}) /varset spellType GOM110
		/if (${Me.Song[Gift of Mana (115)].ID}) /varset spellType GOM115
		/if (${Me.Song[Gift of Mana (120)].ID}) /varset spellType GOM120
	}
	/if (${Me.Song[Flames of Power].ID}) {
		/if (${Me.Song[Gift of].ID} && ${fopgiftSpell}) {
			/varset spellType ${spellType}FOP
		} else /if (${fopSpell} && !${Me.Song[Gift of].ID}) {
			/varset spellType FOP
		}
	}
	:restartCast
	/for nArray 1 to ${csCount}
		/if (${csType[${nArray}].Equal[${spellType}]}) {
			/if (${combatSpells[${nArray}].NotEqual[NULL]} && ${Target.ID} && ${Target.Type.NotEqual[Corpse]}) {
				/if (${csSpellSlot[${nArray}].Equal[item]}) {
					/if (!${FindItem[${combatSpells[${nArray}]}].ID}) {
						/call EchoLog "${combatSpells[${nArray}]} could not be found in your inventory.  Please check the spelling in your INI." TRUE
						/goto :nextnArray
					}
					/if (${FindItem[${combatSpells[${nArray}]}].Timer}) /goto :nextnArray
				}
				/if (!${combatMem}) {
					/if (${csSpellSlot[${nArray}].Find[Gem]} && !${Me.SpellReady[${combatSpells[${nArray}]}]}) /goto :nextnArray
				}
				| Check GemTimer for spell readiness
				/if (${csSpellSlot[${nArray}].Find[Gem]}) {
					/if (${Me.Gem[${combatSpells[${nArray}]}]}) {
						/if (${Me.GemTimer[${combatSpells[${nArray}]}]} && ${Me.GemTimer[${combatSpells[${nArray}]}].Float} > 5) /goto :nextnArray
					} else {
						/if (${Spell[${combatSpells[${nArray}]}].RecoveryTime.Float} > 5) {
							/memspell ${csSpellSlot[${nArray}].Right[-3]} "${combatSpells[${nArray}]}"
							/delay 5
							/goto :nextnArray
						}
					}
				}
				/if (${csSpellSlot[${nArray}].Equal[alt]} && !${Me.AltAbilityReady[${combatSpells[${nArray}]}]}) /goto :nextnArray
				/if (${csSpellSlot[${nArray}].Equal[disc]} && !${Me.CombatAbilityReady[${combatSpells[${nArray}]}]}) /goto :nextnArray
				/if ((${csSpellSlot[${nArray}].Equal[disc]} || ${combatSpells[${nArray}].Find[Untamed Rage]}) && ${discTimer} && ${Spell[${csSpellSlot[${nArray}]}].Duration.TotalSeconds}) /goto :nextnArray
				/if (${csTimer${nArray}} && !${Me.SpellReady[${combatSpells[${nArray}]}]}) /goto :nextnArray
				/if (${Spell[${combatSpells[${nArray}]}].TargetType.Find[AE]} && ${Spell[${combatSpells[${nArray}]}].TargetType.NotEqual[PB AE]} && ${Spell[${combatSpells[${nArray}]}].AERange} && ${Spell[${combatSpells[${nArray}]}].Range} > 50 && ${Target.Distance} < ${Spell[${combatSpells[${nArray}]}].AERange}) /goto :nextnArray
				/if ((${Me.MaxMana} && ${Me.PctMana}>=${csminMana[${nArray}]} && ${Me.PctMana}<=${csmaxMana[${nArray}]}) || (!${Me.MaxMana} && ${Me.PctEndurance}>=${csminMana[${nArray}]} && ${Me.PctEndurance}<=${csmaxMana[${nArray}]})) {
					/call timer cs ${Target.ID}${nArray}
					/if (${Macro.Return.Equal[FALSE]}) {
						/if (${lTargCount}>=${csminMobs[${nArray}]} && ${lTargCount}<=${csmaxMobs[${nArray}]}) {
							/if (${Target.PctHPs}>=${csminHPs[${nArray}]} && ${Target.PctHPs}<=${csmaxHPs[${nArray}]}) {
								/if ((${csmaxSelfHPs[${nArray}]}==0 || ${Me.PctHPs}<=${csmaxSelfHPs[${nArray}]}) && ${Me.PctHPs}>=${csminSelfHPs[${nArray}]}) {
									/if (${Target.Name.Find[#]}) {
										/varset TargetNamed TRUE
									} else {
										/varset TargetNamed FALSE
									}
									/varset cCondition FALSE
									/if (${csCondition[${nArray}].Equal[NULL]}) {
										/varset cCondition TRUE
									} else {
										/if (${csCondition[${nArray}].Left[1].Equal[!]}) {
											| /varset cCondition ${${csCondition[${nArray}].Right[${Math.Calc[${csCondition[${nArray}].Length}-1]}]}}
											/varset cCondition ${${Ini[${iniName},Combat,SpellCondition${nArray},TRUE].Right[-1]}}
											/if (${cCondition}) {
												/varset cCondition FALSE
											} else {
												/varset cCondition TRUE
											}
										} else {
											| /varset cCondition ${${csCondition[${nArray}]}}
											/varset cCondition ${${Ini[${iniName},Combat,SpellCondition${nArray},TRUE]}}
										}
									}
									/if (${cCondition}) {
										/if (${Me.Class.ShortName.Equal[BRD]} && (${Me.Casting.ID} || ${Me.Casting.Skill.Find[instrument]} || ${Me.Casting.Skill.Find[Sing]} || ${Twist.Twisting})) {
											/if (${Spell[${combatSpells[${nArray}]}].CastTime} || ${FindItem[${combatSpells[${nArray}]}].CastTime}) {
												:stopCast
													/melody
													/stop
													/squelch /twist stop
													/varset restartTwist TRUE
													/delay 1s !${Me.Casting.ID}
												/if (${Me.Casting.ID}) /goto :stopCast
											}
										}
										/if (${csSpellSlot[${nArray}].Find[Gem]} && ${Spell[${combatSpells[${nArray}]}].RecoveryTime.Float} > 3 && !${Me.Gem[${combatSpells[${nArray}]}]}) {
											/memspell ${csSpellSlot[${nArray}].Right[-3]} "${combatSpells[${nArray}]}"
											/call EchoLog ".....Cannot complete casting, spell recovery time to long and it's not memmed.  Memming spell and canceling cast." TRUE
											/varset csTimer${nArray} ${Spell[${combatSpells[${nArray}]}].RecoveryTime.Float}s
											/goto :exitFor
										}
										/if (${faceTarg} && ${Target.ID}) /face nolook
										/if (${csSpellSlot[${nArray}].Equal[item]}) /call ItemSwapCheck "${combatSpells[${nArray}]}"
										/call EchoLog "Attempting to cast ${combatSpells[${nArray}]} on ${Target.CleanName} | ${Target.ID}" TRUE
										/if (${csSpellSlot[${nArray}].Find[disc]} && ${AllowDiscs}) {
											/if (!${Defined[PBCasting]}) /declare PBCasting				bool outer FALSE
											/if (!${Defined[PBFailReason]}) /declare PBFailReason	string outer SUCCESS
											/varset PBFailReason SUCCESS
											/varset PBCasting TRUE
											/if (${Me.CombatAbility[${Me.CombatAbility[${combatSpells[${nArray}]}]}].Name.NotEqual[${combatSpells[${nArray}]}]}) /varset combatSpells[${nArray}] ${Me.CombatAbility[${Me.CombatAbility[${combatSpells[${nArray}]}]}].Name}
											/doability "${combatSpells[${nArray}]}"
											:discWait
												/doevents PBCastFail
											/if (${Me.Casting.ID} && ${Me.Casting.ID}==${Spell[${combatSpells[${nArray}]}].ID}) /goto :discWait
											/varset PBCasting FALSE
											/varset globalCooldown ${globalCDStr}
											/varset sitTimer ${sitTimerString}
											/if (${PBFailReason.Equal[SUCCESS]}) {
												/varset csTimer${nArray} ${csrecastDly[${nArray}]}
												/call timer cs ${Target.ID}${nArray} ${csrecastDly[${nArray}]}
												/varset discTimer ${Math.Calc[${Me.CombatAbility[${Me.CombatAbility[${combatSpells[${nArray}]}]}].Duration.TotalSeconds} + 1]}s
											} else {
												/call EchoLog "Spell cast unsuccessful: ${PBFailReason}" TRUE
											}
										} else /if (${csSpellSlot[${nArray}].Left[6].Equal[hotkey]}) {
											| =========================================================================
											|  This should make it so you can use hotkeys as slots in combat spells
											|   Format HotkeyN-B  where N is the hotkey number and b is the bar number
											|   Just hotkeyN should default to the first bar
											| =========================================================================
											/varset hKeyTemp ${csSpellSlot[${nArray}].Right[-6]}
											/if (${hKeyTemp.Find[-]}) {
												/varset hKeyNum ${hKeyTemp.Left[${Math.Calc[${hKeyTemp.Find[-]} - 1]}]}
												/varset hKeyBar ${hKeyTemp.Right[-${Math.Calc[${hKeyNum.Length} - 1]}]}
											} else {
												/varset hKeyNum ${hKeyTemp}
												/varset hKeyBar 1
											}
											/if (${hKeyNum} != 0 && ${hKeyBar} != 0) {
												/varset hKeyBarStr hotbuttonwnd											
												/varset hKeyNumStr HB
												/if (${hKeyBar} > 1) {
													/varset hKeyBarStr hotbuttonwnd${hKeyBar}
													/varset hKeyNumStr HB${hKeyBar}
												}
												/varset hKeyNumStr ${hKeyNumStr}_Button${hKeyNum}
												
												/call EchoLog "Attempting to click ${csSpellSlot[${nArray}]} HotButton: ${hKeyNumStr} HotBar: ${hKeyBarStr}" TRUE
												/notify ${hKeyBarStr} ${hKeyNumStr} leftmouseup
												/call timer cs ${Target.ID}${nArray} ${csrecastDly[${nArray}]}
												/varset csTimer${nArray} ${csrecastDly[${nArray}]}
											}
										} else {
											/if (${Me.Class.Name.Equal[Bard]} && ${csSpellSlot[${nArray}].Find[Gem]} && (${Spell[${combatSpells[${nArray}]}].Skill.Find[instrument]} || ${Spell[${combatSpells[${nArray}]}].Skill.Find[Sing]})) {
												/call CastSong "${combatSpells[${nArray}]}" ${csSpellSlot[${nArray}].Right[-3]} ${resistTries} TRUE checkTargHPs
												/varset globalCooldown ${globalCDStr}
												/varset sitTimer ${sitTimerString}
												/if (${CastResult.NotEqual[CAST_SUCCESS]}) {
													/call EchoLog "Spell cast unsuccessfull: ${CastResult}" TRUE
													/if (${CastResult.Equal[CAST_IMMUNE]}) /call timer cs ${Target.ID}${nArray} 30m
													/if (${CastResult.Equal[CAST_RESIST]}) /call timer cs ${Target.ID}${nArray} ${resistRetryTime}
													/if (!${Select[${CastResult},CAST_CANCELLED,CAST_INTERRUPTED,CAST_STUNNED,CAST_CANNOTSEE,CAST_ABORTED]}) /varset csTimer${nArray} ${Me.Gem[${Me.Gem[${combatSpells[${nArray}]}]}].RecastTime}s
												} else {
													/call timer cs ${Target.ID}${nArray} ${csrecastDly[${nArray}]}
													/varset csTimer${nArray} ${Me.Gem[${Me.Gem[${combatSpells[${nArray}]}]}].RecastTime}s
												}
											} else {
												/if (${Me.Class.Name.Equal[Bard]} && ${Me.Casting.ID}) {
													/if (${csSpellSlot[${nArray}].Equal[alt]}) {
														/alt act ${Me.AltAbility[${combatSpells[${nArray}]}].ID}
													} else /if (${csSpellSlot[${nArray}].Equal[item]}) {
														/itemnotify ${FindItem[${combatSpells[${nArray}]}].InvSlot} rightmouseup
													}
													/varset globalCooldown ${globalCDStr}
													/varset sitTimer ${sitTimerString}
													/call timer cs ${Target.ID}${nArray} ${csrecastDly[${nArray}]}
													/if (${combatSpells[${nArray}].Equal[Untamed Raged]}) /varset discTimer ${Math.Calc[${Me.CombatAbility[${Me.CombatAbility[${combatSpells[${nArray}]}]}].Duration.TotalSeconds} + 1]}s
													/varset csTimer${nArray} ${Me.Gem[${Me.Gem[${combatSpells[${nArray}]}]}].RecastTime}s
												} else {
													
													/if (${isMMOBugs} && ${Spell[${combatSpells[${nArray}]}].ID}) {
														/for iLoop 1 to 4
															/if (${Spell[${combatSpells[${nArray}]}].ReagentCount[${iLoop}]} && ${Spell[${combatSpells[${nArray}]}].ReagentID[${iLoop}]} != -1 && (!${FindItem[${Spell[${combatSpells[${nArray}]}].ReagentID[${iLoop}]}].ID} || ${FindItem[${Spell[${combatSpells[${nArray}]}].ReagentID[${iLoop}]}].StackCount} < ${Spell[${combatSpells[${nArray}]}].ReagentCount[${iLoop}]})) {
																/call EchoLog ".....Cannot cast spell, missing required component ID ${Spell[${combatSpells[${nArray}]}].ReagentID[${iLoop}]}" TRUE
																/goto :nextnArray
															}
														/next iLoop
													}
													
													/call MQ2Cast "${combatSpells[${nArray}]}" ${csSpellSlot[${nArray}]} 0 checkTargHPs -targetID|${Target.ID} -maxtries|${resistTries}
													/varset globalCooldown ${globalCDStr}
													/varset sitTimer ${sitTimerString}
													/if (${CastResult.NotEqual[CAST_SUCCESS]}) {
														/call EchoLog "Spell cast unsuccessfull: ${CastResult}" TRUE
														/if (${CastResult.Equal[CAST_IMMUNE]}) /call timer cs ${Target.ID}${nArray} 30m
														/if (${CastResult.Equal[CAST_RESIST]}) /call timer cs ${Target.ID}${nArray} ${resistRetryTime}
														/if (!${Select[${CastResult},CAST_CANCELLED,CAST_INTERRUPTED,CAST_STUNNED,CAST_CANNOTSEE,CAST_ABORTED]}) /varset csTimer${nArray} ${Me.Gem[${Me.Gem[${combatSpells[${nArray}]}]}].RecastTime}s
													} else {
														/call timer cs ${Target.ID}${nArray} ${csrecastDly[${nArray}]}
														/if (${combatSpells[${nArray}].Equal[Untamed Raged]}) /varset discTimer ${Math.Calc[${Me.CombatAbility[${Me.CombatAbility[${combatSpells[${nArray}]}]}].Duration.TotalSeconds} + 1]}s
														/varset csTimer${nArray} ${Me.Gem[${Me.Gem[${combatSpells[${nArray}]}]}].RecastTime}s
													}
												}
											}
										}
										/if (${itemSwapped}) /call ItemSwapCheck
										/varset retValue CAST_COMPLETE
										/goto :exitFor
									}
								}
							}
						}
					}
				}
			}
		}
		:nextnArray
	/next nArray
	/if (${sTypeNum}) {
		/varcalc sTypeNum ${sTypeNum} + 1
		/if (${sTypeNum}>${spellTypeValue}) {
			/varset sTypeNum 0
			/varset spellType ALL
			/goto :restartCast
		} else {
			/varset spellType ${spellTypes[${sTypeNum}]}
			/goto :restartCast
		}
	}
	/if (${spellType.Find[GOM]} && !${spellType.Find[FOP]}) {
		/if (${spellType.Equal[GOM120]}) {
			/varset spellType GOM115			
		} else /if (${spellType.Equal[GOM115]}) {
			/varset spellType GOM110
		} else /if (${spellType.Equal[GOM110]}) {
			/varset spellType GOM105
		} else /if (${spellType.Equal[GOM105]}) {
			/varset spellType GOM100
		} else /if (${spellType.Equal[GOM100]}) {
			/varset spellType GOM95
		} else /if (${spellType.Equal[GOM95]}) {
			/varset spellType GOM90
		} else /if (${spellType.Equal[GOM90]}) {
			/varset spellType GOM85
		} else /if (${spellType.Equal[GOM85]}) {
			/varset spellType GOM80
		} else /if (${spellType.Equal[GOM80]}) {
			/varset spellType GOM75
		} else /if (${spellType.Equal[GOM75]}) {
			/varset spellType GOM70
		} else /if (${spellType.Equal[GOM70]}) {
			/if (${Me.Song[Flames of Power].ID}) {
				/varset spellType FOP
			} else {
				/varset spellType ALL
			}
		}
		/goto :restartCast
	}
	/if (${spellType.Equal[FOP]}) {
		/varset spellType ALL
		/goto :restartCast
	}
	/if (${spellType.Find[GOM]} && ${spellType.Find[FOP]}) {
		/if (${spellType.Equal[GOM120]}) {
			/varset spellType GOM115			
		} else /if (${spellType.Equal[GOM115]}) {
			/varset spellType GOM110
		} else /if (${spellType.Equal[GOM110]}) {
			/varset spellType GOM105
		} else /if (${spellType.Equal[GOM105]}) {
			/varset spellType GOM100
		} else /if (${spellType.Find[GOM100]}) {
			/varset spellType GOM95
		} else /if (${spellType.Find[GOM95]}) {
			/varset spellType GOM90
		} else /if (${spellType.Find[GOM90]}) {
			/varset spellType GOM85
		} else /if (${spellType.Find[GOM85]}) {
			/varset spellType GOM80
		} else /if (${spellType.Find[GOM80]}) {
			/varset spellType GOM75
		} else /if (${spellType.Find[GOM75]}) {
			/varset spellType GOM70
		} else /if (${spellType.Find[GOM70]}) {
			/varset spellType GOM120
		}
		/if (${spellType.NotEqual[GOM120]}) /varset spellType ${spellType}FOP
		/goto :restartCast
	}	
	/varset retValue CAST_NONE
	:exitFor
	| /if (${restartTwist}) /squelch /twist start
	/if (${restartTwist}) {
		/if (${useMelody}) {
			/squelch /melody ${combatGem1} ${combatGem2} ${combatGem3} ${combatGem4}
		} else /if (${useTwist}) {
			/squelch /twist start
		}
	}
	
	/varset priorityCombatSpellsTimer ${priorityCombatSpells}
/return ${retValue}

Sub CheckFriendList(int friendID)
	/if (!${Defined[friendID]}) /return FALSE
	/call CreateFriendList
	/if (${friendList.Find[|${friendID}|]}) /return TRUE
/return FALSE

Sub IsDetectedAdd(int checkID)
	/declare idLoop				int local 0
	
	/if (${targCount}) {
		/for idLoop 1 to ${targCount}
			/if (${targArray[${idLoop}]}==${checkID}) /return TRUE
		/next idLoop
	}
/return FALSE

Sub CheckForXTargetAdds(int cDist, int checkID, bool checkAll, bool silentRun)
	/if (!${Defined[cDist]}) /declare cDist int local ${campRadius}
	/if (!${Defined[checkAll]}) /declare checkAll bool local TRUE
	/if (!${Defined[silentRun]}) /declare silentRun bool local FALSE
	/if (!${Defined[checkID]}) /declare checkID int local 0
	
	/declare xTargLoop			int local 0
	/declare freeXTarget		int local 0
	/declare xID						int local 0
	/declare checkY					float local ${If[!${checkID},${homeY},${Spawn[${checkID}].Y}]}
	/declare checkX					float local ${If[!${checkID},${homeX},${Spawn[${checkID}].X}]}
	
	/for xTargLoop 1 to ${maxXTargets}
		/if (${Me.XTarget[${xTargLoop}].${xTargType}.Equal[Auto Hater]} && !${Me.XTarget[${xTargLoop}].ID}) /varcalc freeXTarget ${freeXTarget} + 1
		/if (${Me.XTarget[${xTargLoop}].ID}) {
			/varset xID ${Me.XTarget[${xTargLoop}].ID}
			/if ((${Spawn[${xID}].Type.Equal[NPC]} || (${Spawn[${xID}].Type.Equal[Pet]} && ${Spawn[${xID}].Master.Type.NotEqual[PC]})) && ${Math.Distance[${checkY},${checkX}:${Spawn[${xID}].Y},${Spawn[${xID}].X}]} <= ${cDist} && ${Spawn[${xID}].LineOfSight}) {
				/call IsDetectedAdd ${Me.XTarget[${xTargLoop}].ID}
				/if (${Macro.Return.Equal[FALSE]}) {
					/varcalc targCount ${targCount} + 1
					/varset targArray[${targCount}] ${xID}
					/if (!${silentRun}) /call EchoLog "It Appears we have an add from XTarget ${xTargLoop} ${Me.XTarget[${xTargLoop}].${xTargType}}, ${Spawn[${xID}].CleanName} | ${xID}  Total: ${targCount}, Current: ${Math.Calc[${targCount}-${corpseCount}].Int}" TRUE
					/if (${Spawn[${xID}].Name.Left[1].Equal[#]}) {
						/varcalc namedCount ${namedCount} + 1
						/if (!${silentRun}) /call EchoLog ".....The add is a named, Total Named: ${namedCount}" TRUE
					}
					/varset newAddID ${xID}
					/if (!${checkAll}) /return 0
				}
			}
		}	
	/next xTargLoop
/return ${freeXTarget}

Sub CheckForAdds(int cDist, int checkID,  bool useChoose, bool checkAll, bool silentRun)
	/if (${priorityAddCheckTimer}) /return ABORT_TIMER
	/if (!${Defined[cDist]}) /declare cDist int local ${campRadius}
	/if (!${Defined[checkID]}) /declare checkID int local 0
	/if (!${Defined[useChoose]}) /declare useChoose bool local FALSE
	/if (!${Defined[checkAll]}) /declare checkAll bool local TRUE
	/if (!${Defined[silentRun]}) /declare silentRun bool local FALSE

	/if (${checkID} && !${Spawn[${checkID}].ID}) /varset checkID 0
	/if (!${checkID} && (${followMode} || (${Defined[soloMode]} && ${soloMode}))) /varset checkID ${Me.ID}
	/declare checkY					float local ${If[!${checkID},${homeY},${Spawn[${checkID}].Y}]}
	/declare checkX					float local ${If[!${checkID},${homeX},${Spawn[${checkID}].X}]}

	/if (!${checkForAdds}) {
		/if (${Target.ID} && ${Target.Type.Equal[NPC]}) {
			/varset inCombat TRUE
			/varset targID ${Target.ID}
			/varset lTargCount 1
			/varset targCount 1
			/varset targArray[1] ${Target.ID}
		}
		/return
	}			

	/declare sID 								int local 0
	/declare cID								int local 0
	/declare mToTID							int local 0
	/declare nCount 						int local	0
	/declare isAdd							bool local FALSE
	/declare detectType					string local npc
	/declare fLoop							int local 0
	/declare aHeading						float local
	/declare aToFriendHeading		float local		
	/declare freeXTargets				int local 0
	/declare spawnAggressive		bool local FALSE
	/if (${useXTarget}) {
		/varset newAddID 0
		/call CheckForXTargetAdds ${cDist} ${checkID} ${silentRun}
		/varset freeXTargets ${Macro.Return}
		/if (${newAddID} && !${checkAll}) /goto :endDetection
	}
	
	/if (!${useXTarget} || (!${xTargetOnly} && !${freeXTargets})) {
		/call CreateFriendList
		:nextAdd
			/varcalc nCount ${nCount} + 1
			/varset sID ${NearestSpawn[${nCount},${detectType} radius ${cDist} loc ${checkX} ${checkY} Range ${minMobLvl} ${maxMobLvl} los noalert ${alertList}].ID}
			/varset spawnAggressive TRUE
			/if (!${emuMode}) {
				/varset spawnAggressive ${Spawn[${sID}].Aggressive}
			}
			/if (${sID} && ${spawnAggressive} && (${Spawn[${sID}].Type.Equal[NPC]} || (${Spawn[${sID}].Type.Equal[Pet]} && ${Spawn[${sID}].Master.Type.NotEqual[PC]})) && (!${checkID} || ${Spawn[${sID}].FeetWet}==${Spawn[${checkID}].FeetWet})) {
				/call IsDetectedAdd ${sID}
				/if (${Macro.Return.Equal[FALSE]}) {
					/if (${Me.LAHoTT} && ${Group.GroupSize} >= 3 && !${Me.Combat} && ${useLAHoTT}) {
						| ====================================================================================
						| Check the target of the mob to ensure it's targetting a friend
						| ====================================================================================						
						/varset mToTID ${Me.TargetOfTarget.ID}
						/squelch /target id ${sID}
						/delay 1s ${Target.ID}==${sID}
						/delay 1s ${Me.TargetOfTarget.ID}!=${mToTID}
						/if (${Me.TargetOfTarget.ID}) {
							/call CheckFriendList ${Me.TargetOfTarget.ID}
							/varset isAdd ${Macro.Return}
						} else {
							/varset isAdd FALSE
						}
					} else {
						| ====================================================================================
						| Check the heading of the mob to see if it's facing a friend if LA HoTT is not available
						| ====================================================================================
						/varset isAdd FALSE
						
						/for fLoop 1 to ${Math.Calc[${friendList.Count[|]}+1]}
							/if (${friendList.Token[${fLoop},|].NotEqual[NULL]} && ${friendList.Token[${fLoop},|].Length}) {
								/varset cID ${friendList.Token[${fLoop},|]}
								/varset aHeading ${Spawn[${sID}].Heading.Degrees}
								/varset aToFriendHeading ${Spawn[${sID}].HeadingToLoc[${Spawn[${cID}].Y},${Spawn[${cID}].X}].Degrees}
								/if (${Math.Abs[${aHeading}-${aToFriendHeading}]} < 4) /varset isAdd TRUE
							}
						/if (!${isAdd}) /next fLoop
					}
					
					/if (${isAdd}) {
						/varcalc targCount ${targCount} + 1
						/varset targArray[${targCount}] ${sID}
						/if (!${silentRun}) /call EchoLog "It Appears we have an add, ${Spawn[${sID}].CleanName} | ${sID}  Total: ${targCount}, Current: ${Math.Calc[${targCount}-${corpseCount}].Int}" TRUE
						/if (${Spawn[${sID}].Name.Left[1].Equal[#]}) {
							/varcalc namedCount ${namedCount} + 1
							/if (!${silentRun}) /call EchoLog ".....The add is a named, Total Named: ${namedCount}" TRUE
						}
						/varset newAddID ${sID}
						/if (!${checkAll}) /goto :endDetection
					}
				}
				/goto :nextAdd
			} else /if (!${sID} && ${detectType.Equal[NPC]}) {
				/varset detectType Pet
				/varset nCount 0
				/goto :nextAdd
			}		
	}
	:endDetection
	/if (${useChoose}) {
		/call ChooseTarget ${newAddID}
	} else {
		/call CorpseCheck
	}
	/varset priorityAddCheckTimer ${priorityAddCheck}
/return

Sub checkTargHPs
	/if (${Target.Type.Equal[Corpse]} && ${allowInterrupt}) /call Interrupt
	/call CheckForAggro
/return

Sub CheckForAggro(int mobCheck)
	/declare nearID 				int local 0
	/declare nearNum 				int local 0
	/declare mobHeading 		float local
	/declare mobToMeHeading float local
	/declare mToTID					int local 0
	/if (!${Defined[mobCheck]}) /declare mobCheck				int local 0

	/if (!${checkForAdds}) /varset mobCheck ${targID}
	
	/varset gotAggro FALSE
	/varset gotAggroID 0
	/if (${useXTarget} && !${Me.XTarget}) /return FALSE
	:nextNearNPC
	/if (!${mobCheck}) {
		/varset nearNum ${Math.Calc[${nearNum}+1]}
		/varset nearID ${Me.NearestSpawn[${nearNum}, npc radius ${Math.Calc[${minSafeDistance} - .5]} los noalert ${alertList}].ID}
	} else {
		/if (${Spawn[${mobCheck}].Distance} > ${minSafeDistance} || ${Math.Abs[${Spawn[${mobCheck}].Heading.Degrees}-${Spawn[${mobCheck}].HeadingToLoc[${Me.Y},${Me.X}].Degrees}]}>5) {
			/if (!${rampageNoSit}) /varset allowSit TRUE
			/return ${gotAggroID}
		}
		/varset nearID ${mobCheck}
	}
	/if (${nearID} && ${Spawn[${nearID}].Type.NotEqual[Untargetable]}) {
		/varset mobHeading ${Spawn[${nearID}].Heading.Degrees}
		/varset mobToMeHeading ${Spawn[${nearID}].HeadingToLoc[${Me.Y},${Me.X}].Degrees}
		/varset allowSit FALSE
		/if (${Me.LAHoTT} && ${Group.GroupSize}>=3 && !${Me.Combat} && !${Me.Casting.ID}) {
			/varset mToTID ${Me.TargetOfTarget.ID}
			/squelch /target id ${sID}
			/delay 1s ${Target.ID}==${nearID}
			/delay 1s ${Me.TargetOfTarget.ID}!=${mToTID}
			/if (${Me.TargetOfTarget.ID}==${Me.ID}) {
				/if (${Me.Sitting}) /stand
				/varset gotAggro TRUE
				/varset gotAggroID ${Target.ID}
				/if (${sitTimer} < 500) /varset sitTimer 5s
			} else {
				/if (!${mobCheck}) /goto :nextNearNPC
			}
		} else {
			/if (${Math.Abs[${mobHeading} - ${mobToMeHeading}]} < 3) {
				/if (${Me.Sitting}) /stand
				/varset gotAggro TRUE
				/varset gotAggroID ${nearID}
				/if (${sitTimer} < 500) /varset sitTimer 5s
				/if (${Me.Casting.ID} && ${allowInterrupt}) {
					/call EchoLog "Casting aborted due to aggro" TRUE
					/call Interrupt
				}
			} else {
				/if (!${mobCheck}) /goto :nextNearNPC
			}
		}
	} else /if (${Spawn[${nearID}].Type.Equal[Untargetable]}) {
		/if (!${mobCheck}) /goto :nextNearNPC
	} else /if (${nearNum}<=1 && !${rampageNoSit}) {
		/varset allowSit TRUE
	}
/return ${gotAggroID}

Sub CommonINILoad
	/declare ctArray										int local 0
	
	/call EchoLog "Common INI Load started, INI Name: ${iniName}"

| ========================================
|  Load command settings (custom commands)
| ========================================
	/call GetINISetting ${iniName} Commands EQBCCommands eqbcCommands TRUE
	/call GetINISetting ${iniName} Commands GroupCommands groupCommands TRUE
	/call GetINISetting ${iniName} Commands TellCommands tellCommands TRUE
	/call GetINISetting ${iniName} Commands echoCommands echoCommands TRUE

	/call GetINIArrayCount ${iniName} Commands CommandTrigger NULL
	/varset commandCount ${Macro.Return}
	/call EchoLog "Loading ${commandCount} custom command(s)." TRUE
	/if (${commandCount}) {
		/for nArray 1 to ${commandCount}
			/call GetINISetting ${iniName} Commands CommandTrigger${nArray} commandTriggers[${nArray},1] NULL
			/call GetINIArrayCount ${iniName} Commands CommandTrigger${nArray}Action NULL
			/varset commandTriggers[${nArray},2] ${Macro.Return}
			/call EchoLog ".....Trigger ${nArray}: ${commandTriggers[${nArray},1]} Actions: ${commandTriggers[${nArray},2]}" TRUE
			/if (${commandTriggers[${nArray},2]}) {
				/for ctArray 1 to ${commandTriggers[${nArray},2]}
					/call GetINISetting ${iniName} Commands CommandTrigger${nArray}Action${ctArray} commandActions[${nArray},${ctArray}] NULL
					/call EchoLog "..........Action ${ctArray}: ${commandActions[${nArray},${ctArray}]}" TRUE
				/next ctArray
			}
			/call GetINISetting ${iniName} Commands CommandTrigger${nArray}Action${Math.Calc[${commandTriggers[${nArray},2]} + 1].Int} NULL NULL
		/next nArray
	}
	/call GetINISetting ${iniName} Commands CommandTrigger${Math.Calc[${commandCount}+1].Int} NULL NULL
	/call GetINISetting ${iniName} Commands CommandTrigger${Math.Calc[${commandCount}+1].Int}Action1 NULL NULL
		
| ========================================
|  Load watch list
| ========================================
	/call GetINIArrayCount ${iniName} WatchList WatchName NULL
	/varset watchCount ${Macro.Return}
	/call EchoLog "Loading ${watchCount} watched PCs" TRUE
	
	
	/if (${watchCount}) {
		/for nArray 1 to ${watchCount} 
			/call GetINISetting ${iniName} WatchList WatchName${nArray} watchList[${nArray}] NULL
			/call EchoLog ".....NAME ${nArray}: ${watchList[${nArray}]} added to the watch list" TRUE
		/next nArray
	}
	/call GetINISetting ${iniName} WatchList WatchName${Math.Calc[${watchCount}+1].Int} NULL NULL
	
| ========================================
|  Load Debuffs
| ========================================
	/declare debuffAll						bool outer FALSE
	/declare debuffAllOneTarget		bool outer TRUE
	
	/if (${debuffClasses.Find[${Me.Class.ShortName}]}) {
		/call GetINIArrayCount ${iniName} Debuffs DebuffSpell NULL
		/varset debuffCount ${Macro.Return}
		/call EchoLog "Loading ${debuffCount} debuff(s):" TRUE
		/call GetINISetting ${iniName} Debuffs DebuffAdds debuffAdds FALSE
		/call GetINISetting ${iniName} Debuffs DebuffAllDebuffsAtOnce debuffAll TRUE
		/call GetINISetting ${iniName} Debuffs DebuffAllDebuffsOneTarget debuffAllOneTarget TRUE FALSE
		
		/for nArray 1 to ${Math.Calc[${debuffCount}+1]}
			/if (${nArray} <= ${debuffCount}) {
				/call GetINISetting ${iniName} Debuffs DebuffSpell${nArray} debuffSpells[${nArray}] NULL
				/call GetINISetting ${iniName} Debuffs DebuffMinMana${nArray} debuffMinMana[${nArray}] 1
				/call GetINISetting ${iniName} Debuffs DebuffMinHPs${nArray} debuffMinHPs[${nArray}] 1
				/call GetINISetting ${iniName} Debuffs DebuffMaxHPs${nArray} debuffMaxHPs[${nArray}] 100
				/call GetINISetting ${iniName} Debuffs DebuffMaxMobs${nArray} debuffMaxMobs[${nArray}] 100
				/call GetINISetting ${iniName} Debuffs DebuffMinMobs${nArray} debuffMinMobs[${nArray}] 1
				/call GetINISetting ${iniName} Debuffs DebuffSpellSlot${nArray} debuffSpellSlot[${nArray}] gem1
				/call GetINISetting ${iniName} Debuffs DebuffRecastDly${nArray} debuffRecastDly[${nArray}] 1m
				/call GetINISetting ${iniName} Debuffs DebuffAnnounce${nArray} debuffAnnounce[${nArray}] NULL
				/call GetINISetting ${iniName} Debuffs DebuffAlias${nArray} debuffAlias[${nArray}] debuff${nArray}
				/call GetINISetting ${iniName} Debuffs DebuffCastOnResist${nArray} debuffCOR[${nArray}] NULL
				/call GetINISetting ${iniName} Debuffs DebuffEnabled${nArray} debuffEnabled[${nArray}] TRUE
				/call EchoLog ".....DEBUFF ${nArray}: NAME: ${debuffSpells[${nArray}]}" TRUE
			} else {
				/call GetINISetting ${iniName} Debuffs DebuffSpell${nArray} NULL NULL
				/call GetINISetting ${iniName} Debuffs DebuffMinMana${nArray} NULL 1
				/call GetINISetting ${iniName} Debuffs DebuffMinHPs${nArray} NULL 1
				/call GetINISetting ${iniName} Debuffs DebuffMaxHPs${nArray} NULL 100
				/call GetINISetting ${iniName} Debuffs DebuffMaxMobs${nArray} NULL 100
				/call GetINISetting ${iniName} Debuffs DebuffMinMobs${nArray} NULL 1
				/call GetINISetting ${iniName} Debuffs DebuffSpellSlot${nArray} NULL gem1
				/call GetINISetting ${iniName} Debuffs DebuffRecastDly${nArray} NULL 1m
				/call GetINISetting ${iniName} Debuffs DebuffAnnounce${nArray} NULL NULL
				/call GetINISetting ${iniName} Debuffs DebuffAlias${nArray} NULL debuff${nArray}
				/call GetINISetting ${iniName} Debuffs DebuffCastOnResist${nArray} NULL NULL
				/call GetINISetting ${iniName} Debuffs DebuffEnabled${nArray} NULL TRUE				
			}
		/next nArray
	}
| ====================
| Load selfbuffs
| ====================
	/call EchoLog "Loading buffs" TRUE
	/call GetINIArrayCount ${iniName} SelfBuffs SelfBuffName NULL
	/varset selfBuffCount ${Macro.Return}
	/call EchoLog ".....Loading ${selfBuffCount} self buff(s)" TRUE
	/for nArray 1 to ${Math.Calc[${selfBuffCount}+1]}
		/if (${nArray} <= ${selfBuffCount}) {
			/call GetINISetting ${iniName} SelfBuffs SelfBuffName${nArray} buffName[${nArray}] NULL
			/call GetINISetting ${iniName} SelfBuffs SelfBuffText${nArray} buffText[${nArray}] "${buffName[${nArray}]}"
			/call GetINISetting ${iniName} SelfBuffs SelfBuffSlot${nArray} buffSlot[${nArray}] ${If[${Me.Class.CanCast},GEM1,ITEM]}
			/call GetINISetting ${iniName} SelfBuffs SelfBuffAuto${nArray} buffAuto[${nArray}] TRUE
			/call GetINISetting ${iniName} SelfBuffs SelfBuffAlias${nArray} buffAlias[${nArray}] SELF${nArray}
			/call GetINISetting ${iniName} SelfBuffs SelfBuffCombat${nArray} buffCombat[${nArray}] FALSE
			/varset buffEnabled[${nArray}] TRUE
			/call EchoLog "..........${nArray} - NAME: ${buffName[${nArray}]} TEXT: ${buffText[${nArray}]} SLOT: ${buffSlot[${nArray}]} AUTO: ${buffAuto[${nArray}]} COMBAT: ${buffCombat[${nArray}]} ALIAS: ${buffAlias[${nArray}]}" TRUE
			/if (${buffCombat[${nArray}]}) /varset combatBuffs TRUE
			
			| Load do not stack buffs
			/call GetINIArrayCount ${iniName} SelfBuffs SelfBuff${nArray}DoNotStack NULL
			/varset buffDNSCount[${nArray}] ${Macro.Return}
			/if (${buffDNSCount[${nArray}]}) {
				/for ctArray 1 to ${buffDNSCount[${nArray}]}
					/call GetINISetting ${iniName} SelfBuffs SelfBuff${nArray}DoNotStack${ctArray} buffDoNotStack[${nArray},${ctArray}] NULL
					/call EchoLog "...............Do Not Stack ${ctArray}: ${buffDoNoStack[${nArray},${ctArray}]}" TRUE
				/next ctArray
			}
			/call GetINISetting ${iniName} SelfBuffs SelfBuff${nArray}DoNotStack${Math.Calc[${buffDNSCount[${nArray}]}+1].Int} NULL NULL
			
			| Load stack buffs
			/call GetINIArrayCount ${iniName} SelfBuffs SelfBuff${nArray}Stack NULL
			/varset buffSCount[${nArray}] ${Macro.Return}
			/if (${buffSCount[${nArray}]}) {
				/for ctArray 1 to ${buffSCount[${nArray}]}
					/call GetINISetting ${iniName} SelfBuffs SelfBuff${nArray}Stack${ctArray} buffStack[${nArray},${ctArray}] NULL
					/call EchoLog "...............Allow Stack ${ctArray}: ${buffStack[${nArray},${ctArray}]}" TRUE
				/next ctArray
			}
			/call GetINISetting ${iniName} SelfBuffs SelfBuff${nArray}Stack${Math.Calc[${buffSCount[${nArray}]}+1].Int} NULL NULL
		} else {
			/call GetINISetting ${iniName} SelfBuffs SelfBuffName${nArray} NULL NULL
			/call GetINISetting ${iniName} SelfBuffs SelfBuffText${nArray} NULL NULL
			/call GetINISetting ${iniName} SelfBuffs SelfBuffSlot${nArray} NULL ${If[${Me.Class.CanCast},GEM1,ITEM]}
			/call GetINISetting ${iniName} SelfBuffs SelfBuffAuto${nArray} NULL TRUE
			/call GetINISetting ${iniName} SelfBuffs SelfBuffAlias${nArray} NULL SELF${nArray}
			/call GetINISetting ${iniName} SelfBuffs SelfBuffCombat${nArray} NULL FALSE
			/call GetINISetting ${iniName} SelfBuffs SelfBuff${nArray}DoNotStack1 NULL NULL
			/call GetINISetting ${iniName} SelfBuffs SelfBuff${nArray}Stack1 NULL NULL
		}
	/next nArray

| ====================
| Load petbuffs
| ====================
	/if (${petClasses.Find[${Me.Class.ShortName}]}) {
		/if (!${petBuffCount}) {
			/call GetINIArrayCount ${iniName} PetBuffs PetBuffName NULL
			/varset petBuffCount ${Macro.Return}
			/call EchoLog ".....Loading ${petBuffCount} pet buff(s)" TRUE
			/for nArray 1 to ${Math.Calc[${petBuffCount}+1]}
				/if (${nArray} <= ${petBuffCount}) {
					/call GetINISetting ${iniName} PetBuffs PetBuffName${nArray} petBuff[${nArray}] NULL
					/call GetINISetting ${iniName} PetBuffs PetBuffText${nArray} petBuffText[${nArray}] "${petBuff[${nArray}]}"
					/call GetINISetting ${iniName} PetBuffs PetBuffSlot${nArray} petBuffSlot[${nArray}] gem1
					/call GetINISetting ${iniName} PetBuffs PetBuffAuto${nArray} petBuffAuto[${nArray}] TRUE
					/call GetINISetting ${iniName} PetBuffs PetBuffAlias${nArray} petBuffAlias[${nArray}] PET${nArray}
					/call GetINISetting ${iniName} PetBuffs PetBuffCombat${nArray} petBuffCombat[${nArray}] FALSE					
					/varset petBuffEnabled[${nArray}] TRUE
					/call EchoLog "..........${nArray} - NAME: ${petBuff[${nArray}]} TEXT: ${petBuffText[${nArray}]} SLOT: ${petBuffSlot[${nArray}]} AUTO: ${petBuffAuto[${nArray}]} ALIAS: ${petBuffAlias[${nArray}]}" TRUE
					/if (${petBuffCombat[${nArray}]}) /varset combatBuffs TRUE

					| Load do not stack petbuffs
					/call GetINIArrayCount ${iniName} PetBuffs PetBuff${nArray}DoNotStack NULL
					/varset petBuffDNSCount[${nArray}] ${Macro.Return}
					/if (${petBuffDNSCount[${nArray}]}) {
						/for ctArray 1 to ${petbuffDNSCount[${nArray}]}
							/call GetINISetting ${iniName} PetBuffs PetBuff${nArray}DoNotStack${ctArray} petBuffDoNotStack[${nArray},${ctArray}] NULL
							/call EchoLog "...............Do Not Stack ${ctArray}: ${petBuffDoNoStack[${nArray},${ctArray}]}" TRUE
						/next ctArray
					}
					/call GetINISetting ${iniName} PetBuffs PetBuff${nArray}DoNotStack${Math.Calc[${petBuffDNSCount[${nArray}]}+1].Int} NULL NULL
					
					| Load stack petbuffs
					/call GetINIArrayCount ${iniName} PetBuffs PetBuff${nArray}Stack NULL
					/varset petBuffSCount[${nArray}] ${Macro.Return}
					/if (${petBuffSCount[${nArray}]}) {
						/for ctArray 1 to ${petbuffSCount[${nArray}]}
							/call GetINISetting ${iniName} PetBuffs PetBuff${nArray}Stack${ctArray} petBuffStack[${nArray},${ctArray}] NULL
							/call EchoLog "...............Allow Stack ${ctArray}: ${petBuffStack[${nArray},${ctArray}]}" TRUE
						/next ctArray
					}
					/call GetINISetting ${iniName} PetBuffs PetBuff${nArray}Stack${Math.Calc[${petBuffSCount[${nArray}]}+1].Int} NULL NULL					
				} else {
					/call GetINISetting ${iniName} PetBuffs PetBuffName${nArray} NULL NULL
					/call GetINISetting ${iniName} PetBuffs PetBuffText${nArray} NULL NULL
					/call GetINISetting ${iniName} PetBuffs PetBuffSlot${nArray} NULL GEM1
					/call GetINISetting ${iniName} PetBuffs PetBuffAuto${nArray} NULL TRUE
					/call GetINISetting ${iniName} PetBuffs PetBuffAlias${nArray} NULL PET${nArray}
					/call GetINISetting ${iniName} PetBuffs PetBuffCombat${nArray} NULL FALSE		
					/call GetINISetting ${iniName} PetBuffs PetBuff${nArray}DoNotStack1 NULL NULL	
					/call GetINISetting ${iniName} PetBuffs PetBuff${nArray}Stack1 NULL NULL			
				}
			/next nArray
		}
	}
	
| ====================
| Load Group Buffs
| ====================
	/varset nArray 0
	:groupBuffCountLoop
		/varcalc nArray ${nArray} + 1
	/if (${Ini[${iniName},GroupBuffs,GroupBuffGroup${nArray},NULL].NotEqual[NULL]} || ${Ini[${iniName},GroupBuffs,GroupBuffSingle${nArray},NULL].NotEqual[NULL]}) /goto :groupBuffCountLoop
	/varcalc nArray ${nArray} - 1
	/varset groupBuffCount ${nArray}
	/call EchoLog ".....Loading ${groupBuffCount} group buff(s)" TRUE
	/for nArray 1 to ${Math.Calc[${groupBuffCount}+1]}
		/if (${nArray} <= ${groupBuffCount}) {
			/call GetINISetting ${iniName} GroupBuffs GroupBuffGroup${nArray} groupBuffGroup[${nArray}] NULL
			/call GetINISetting ${iniName} GroupBuffs GroupBuffSingle${nArray} groupBuffSingle[${nArray}] NULL
			/call GetINISetting ${iniName} GroupBuffs GroupBuffGroupDisplayName${nArray} groupBuffGroupDisplay[${nArray}] "${groupBuffGroup[${nArray}]}"
			/call GetINISetting ${iniName} GroupBuffs GroupBuffSingleDisplayName${nArray} groupBuffSingleDisplay[${nArray}] "${groupBuffSingle[${nArray}]}"
			/call GetINISetting ${iniName} GroupBuffs GroupBuffSlot${nArray} groupBuffSlot[${nArray}] gem1
			/call GetINISetting ${iniName} GroupBuffs GroupBuffAlias${nArray} groupBuffAlias[${nArray}] GROUP${nArray}
			/call GetINISetting ${iniName} GroupBuffs GroupBuffTargets${nArray} groupBuffTargets[${nArray}] Pet|WAR|SHD|PAL|RNG|MNK|ROG|BRD|BST|BER|SHM|CLR|DRU|WIZ|MAG|ENC|NEC
			/call GetINISetting ${iniName} GroupBuffs GroupBuffGroupAnnounce${nArray} groupBuffGroupAnnounce[${nArray}] NULL
			/call GetINISetting ${iniName} GroupBuffs GroupBuffSingleAnnounce${nArray} groupBuffSingleAnnounce[${nArray}] NULL
			/call GetINISetting ${iniName} GroupBuffs GroupBuffCombat${nArray} groupBuffCombat[${nArray}] FALSE
			/call GetINISetting ${iniName} GroupBuffs GroupBuffAuto${nArray} groupBuffAuto[${nArray}] TRUE
			/call GetINISetting ${iniName} GroupBuffs GroupBuffGroupMin${nArray} groupBuffGroupMin[${nArray}] ${If[${groupBuffSingle[${nArray}].NotEqual[NULL]} || (${groupBuffSingle[${nArray}].Equal[NULL]} && ${groupBuffGroup[${nArray}].Equal[NULL]}),3,1]}
			/varset groupBuffEnabled[${nArray}] TRUE
			/call EchoLog "..........${nArray} - GROUP NAME: ${groupBuffGroup[${nArray}]} GROUP TEXT: ${groupBuffGroupDisplay[${nArray}]} SINGLE NAME: ${groupBuffSingle[${nArray}]} SINGLE TEXT: ${groupBuffSingleDisplay[${nArray}]} SLOT: ${groupBuffSlot[${nArray}]} ALIAS: ${groupBuffAlias[${nArray}]} TARGETS: ${groupBuffTargets[${nArray}]} COMBAT: ${groupBuffCombat[${nArray}]} AUTO: ${groupBuffAuto[${nArray}]} MIN GROUP: ${groupBuffGroupMin[${nArray}]}" TRUE
			/if (${groupBuffCombat[${nArray}]}) /varset combatBuffs TRUE
			
			| Load do not stack groupbuffs
			/call GetINIArrayCount ${iniName} GroupBuffs GroupBuff${nArray}DoNotStack NULL
			/varset groupBuffDNSCount[${nArray}] ${Macro.Return}
			/if (${groupBuffDNSCount[${nArray}]}) {
				/for ctArray 1 to ${groupBuffDNSCount[${nArray}]}
					/call GetINISetting ${iniName} GroupBuffs GroupBuff${nArray}DoNotStack${ctArray} groupBuffDoNotStack[${nArray},${ctArray}] NULL
					/call EchoLog "...............Do Not Stack ${ctArray}: ${groupBuffDoNotStack[${nArray},${ctArray}]}" TRUE
				/next ctArray
			}
			/call GetINISetting ${iniName} GroupBuffs GroupBuff${nArray}DoNotStack${Math.Calc[${groupBuffDNSCount[${nArray}]}+1].Int} NULL NULL
			
			| Load stack groupbuffs
			/call GetINIArrayCount ${iniName} GroupBuffs GroupBuff${nArray}Stack NULL
			/varset groupBuffSCount[${nArray}] ${Macro.Return}
			/if (${groupBuffSCount[${nArray}]}) {
				/for ctArray 1 to ${groupBuffSCount[${nArray}]}
					/call GetINISetting ${iniName} GroupBuffs GroupBuff${nArray}Stack${ctArray} groupBuffStack[${nArray},${ctArray}] NULL
					/call EchoLog "...............Allow Stack ${ctArray}: ${groupBuffStack[${nArray},${ctArray}]}" TRUE
				/next ctArray
			}
			/call GetINISetting ${iniName} GroupBuffs GroupBuff${nArray}Stack${Math.Calc[${groupBuffSCount[${nArray}]}+1].Int} NULL NULL
			
			| Set display names
			/if (${groupBuffGroupDisplay[${nArray}].Equal[NULL]} || ${groupBuffGroup[${nArray}].Equal[NULL]}) /varset groupBuffGroupDisplay[${nArray}] ${groupBuffGroup[${nArray}]}
			/if (${groupBuffSingleDisplay[${nArray}].Equal[NULL]} || ${groupBuffSingle[${nArray}].Equal[NULL]}) /varset groupBuffSingleDisplay[${nArray}] ${groupBuffSingle[${nArray}]}
		} else {
			/if (${Me.Class.CanCast}) {
				/call GetINISetting ${iniName} GroupBuffs GroupBuffGroup${nArray} NULL NULL
				/call GetINISetting ${iniName} GroupBuffs GroupBuffSingle${nArray} NULL NULL
				/call GetINISetting ${iniName} GroupBuffs GroupBuffGroupDisplayName${nArray} NULL NULL
				/call GetINISetting ${iniName} GroupBuffs GroupBuffSingleDisplayName${nArray} NULL NULL
				/call GetINISetting ${iniName} GroupBuffs GroupBuffSlot${nArray} NULL gem1
				/call GetINISetting ${iniName} GroupBuffs GroupBuffAlias${nArray} NULL GROUP${nArray}
				/call GetINISetting ${iniName} GroupBuffs GroupBuffTargets${nArray} NULL mt|Pet|WAR|SHD|PAL|RNG|MNK|ROG|BRD|BST|BER|SHM|CLR|DRU|WIZ|MAG|ENC|NEC
				/call GetINISetting ${iniName} GroupBuffs GroupBuffGroupAnnounce${nArray} NULL NULL
				/call GetINISetting ${iniName} GroupBuffs GroupBuffSingleAnnounce${nArray} NULL NULL
				/call GetINISetting ${iniName} GroupBuffs GroupBuffCombat${nArray} NULL FALSE
				/call GetINISetting ${iniName} GroupBuffs GroupBuffAuto${nArray} NULL TRUE
				/call GetINISetting ${iniName} GroupBuffs GroupBuffGroupMin${nArray} NULL ${If[${groupBuffSingle[${nArray}].NotEqual[NULL]} || (${groupBuffSingle[${nArray}].Equal[NULL]} && ${groupBuffGroup[${nArray}].Equal[NULL]}),3,1]}			
				/call GetINISetting ${iniName} GroupBuffs GroupBuff${nArray}DoNotStack1 NULL NULL
				/call GetINISetting ${iniName} GroupBuffs GroupBuff${nArray}Stack1 NULL NULL
			}
		}
	/next nArray

 /if (${combatBuffs}) /varset doCombatBuffs TRUE
 
| ====================
| Load SafePCs
| ====================
	/call GetINIArrayCount ${iniName} SafePCs PCName NULL
	/varset safePCCount ${Macro.Return}
	/call EchoLog "Loading safe pc list:  ${safePCCount} safe pc(s)" TRUE
	/for nArray 1 to ${Math.Calc[${safePCCount}+1]}
		/if (${nArray} <= ${safePCCount}) {
			/call GetINISetting ${iniName} SafePCs PCName${nArray} safePC[${nArray}] NULL
			/call EchoLog ".....${nArray} - ${safePC[${nArray}]} added." TRUE
		} else {
			/call GetINISetting ${iniName} SafePCs PCName${nArray} NULL NULL
		}
	/next nArray
	
| ====================
| Load Mobs to ignore
| ====================
	/call GetINIArrayCount ${iniName} IgnoreMobs MobName NULL
	/varset ignoreMobsCount ${Macro.Return}
	/call EchoLog "Loading mobs to ignore:  ${ignoreMobsCount} mob(s)" TRUE
	
	/for nArray 1 to ${Math.Calc[${ignoreMobsCount}+1]}
		/if (${nArray}<=${ignoreMobsCount}) {
			/call GetINISetting ${iniName} IgnoreMobs MobName${nArray} ignoreMobs[${nArray}] NULL
			/call EchoLog ".....${nArray} - Ignoring: ${ignoreMobs[${nArray}]}" TRUE
		} else {
			/call GetINISetting ${iniName} IgnoreMobs MobName${nArray} NULL NULL
		}
	/next nArray

| ====================
| Load Alerts
| ====================
	/call EchoLog "Loading Alerts" TRUE
	/call GetINISetting ${iniName} Alerts AlertDeathAudioFile alertDeath NULL
	/call GetINISetting ${iniName} Alerts AlertEvac alertEvac NULL
	/call GetINISetting ${iniName} Alerts AlertUnsafePC alertUnsafePC NULL
	/call GetINISetting ${iniName} Alerts AlertGM alertGM NULL
	
	/call GetINIArrayCount ${iniName} Alerts AlertMobName NULL
	/varset mobAlertsValue ${Macro.Return}
	/call EchoLog ".....Death: ${alertDeath}" TRUE
	/call EchoLog ".....Evac: ${alertEvac}" TRUE
	/call EchoLog ".....UnsafePC: ${alertUnsafePC}" TRUE
	/call EchoLog ".....Loading ${mobAlertsValue} mob alert(s)" TRUE
	
	/for nArray 1 to ${Math.Calc[${mobAlertsValue}+1]}
		/if (${nArray}<=${mobAlertsValue}) {
			/call GetINISetting ${iniName} Alerts AlertMobName${nArray} mobAlertsName[${nArray}] NULL
			/call GetINISetting ${iniName} Alerts AlertAudioFile${nArray} mobAlertsAudio[${nArray}] NULL
			/call EchoLog "..........${nArray} - MOBNAME: ${mobAlertsName[${nArray}]} ALERT: ${mobAlertsAudio[${nArray}]}" TRUE
		} else {
			/call GetINISetting ${iniName} Alerts AlertMobName${nArray} NULL NULL
			/call GetINISetting ${iniName} Alerts AlertAudioFile${nArray} NULL NULL			
		}
	/next nArray

	/call GetINIArrayCount ${iniName} Alerts AlertsText NULL
	/varset textAlertsValue ${Macro.Return}
	/call EchoLog ".....Loading ${textAlertsValue} text alert(s)" TRUE
	/if (${textAlertsValue}) {
		/for nArray 1 to ${textAlertsValue}
			/call GetINISetting ${iniName} Alerts AlertText${nArray} textAlertsName[${nArray}] NULL
			/call GetINISetting ${iniName} Alerts AlertTextFile${nArray} textAlertsAudio[${nArray}] NULL
			/call EchoLog "..........${nArray} - TEXT: ${textAlertsName[${nArray}]} ALERT: ${textAlertsAudio[${nArray}]}" TRUE
		/next nArray
	}

| ====================
| Load CombatSpell Settings
| ====================
	/declare spellTypeValue							int outer
	/declare spellTypes[100]						string outer
	/call GetINIArrayCount ${iniName} Combat SpellSetType NULL
	/varset spellTypeValue ${Macro.Return}
	/call EchoLog "Load Spell Types: ${spellTypeValue} spell type(s)" TRUE
	/for nArray 1 to ${Math.Calc[${spellTypeValue}+1]}
		/if (${nArray}<=${spellTypeValue}) {
			/call GetINISetting ${iniName} Combat SpellSetType${nArray} spellTypes[${nArray}] NULL
			/call EchoLog "   ${nArray} - TYPE: ${spellTypes[${nArray}]}" TRUE
		} else {
			/call GetINISetting ${iniName} Combat SpellSetType${nArray} NULL NULL ${If[${spellSetTypeValue},TRUE,FALSE]}
		}
	/next nArray

	/call GetINIArrayCount ${iniName} Combat SpellName NULL
	/varset csCount ${Macro.Return}
	| /if (${csCount}>30) /varset csCount 30
	/call EchoLog "Loading combat spells:  ${csCount} combat spell(s)" TRUE

	/for nArray 1 to ${Math.Calc[${csCount}+1]}
		/if (${nArray}<=${csCount}) {
			/call GetINISetting ${iniName} Combat SpellName${nArray} combatSpells[${nArray}] NULL
			/call GetINISetting ${iniName} Combat SpellMinMana${nArray} csminMana[${nArray}] 0
			/call GetINISetting ${iniName} Combat SpellMaxMana${nArray} csmaxMana[${nArray}] 100 FALSE
			/call GetINISetting ${iniName} Combat SpellMinMobHPs${nArray} csminHPs[${nArray}] 1
			/call GetINISetting ${iniName} Combat SpellMaxMobHPs${nArray} csmaxHPs[${nArray}] 100
			/call GetINISetting ${iniName} Combat SpellRecastDelay${nArray} csrecastDly[${nArray}] 0s
			/call GetINISetting ${iniName} Combat SpellMaxMobs${nArray} csmaxMobs[${nArray}] 100
			/call GetINISetting ${iniName} Combat SpellMinMobs${nArray} csminMobs[${nArray}] 1
			/call GetINISetting ${iniName} Combat SpellSlot${nArray} csSpellSlot[${nArray}] ${If[${Me.Class.CanCast},gem1,DISC]}
			/call GetINISetting ${iniName} Combat SpellCondition${nArray} csCondition[${nArray}] NULL
			/call GetINISetting ${iniName} Combat SpellMinHPs${nArray} csminSelfHPs[${nArray}] 0 FALSE
			/call GetINISetting ${iniName} Combat SpellMaxHPs${nArray} csmaxSelfHPs[${nArray}] 100 FALSE
			/call GetINISetting ${iniName} Combat SpellType${nArray} csType[${nArray}] NULL ${If[${Me.Class.ShortName.Equal[SHD]} || ${Me.Class.ShortName.Equal[PAL]},TRUE,FALSE]}
			/call GetINISetting ${iniName} Combat SpellGOM${nArray} csGiftSpell[${nArray}] ${Me.Class.CanCast}
			/call GetINISetting ${iniName} Combat SpellFOP${nArray} csFlameSpell[${nArray}] ${Me.Class.ShortName.Equal[MAG]}
			/if (${csType[${nArray}].Equal[null]}) /varset csType[${nArray}] ALL
			/call EchoLog ".....${nArray} - NAME: ${combatSpells[${nArray}]} SLOT: ${csSpellSlot[${nArray}]} MAXMOBS: ${csmaxMobs[${nArray}]} MINMOBS: ${csminMobs[${nArray}]} MAXMOBHPS: ${csmaxHPs[${nArray}]} MINMOBHPS: ${csminHPs[${nArray}]} MINMANA: ${csminMana[${nArray}]} MAXMANA: ${csmaxMana[${nArray}]} RECAST: ${csrecastDly[${nArray}]} CONDITION: ${csCondition[${nArray}]} MINHPS: ${csminSelfHPs} MAXHPS: ${csmaxSelfHPs[${nArray}]} TYPE: ${csType[${nArray}]}" TRUE

			/if (${csGiftSpell[${nArray}]}) {
				/varset giftSpell TRUE
				/if (${Spell[${combatSpells[${nArray}]}].Level} <= 70) {
					/varset csType[${nArray}] GOM70
				} else /if (${Spell[${combatSpells[${nArray}]}].Level} <= 75) {
					/varset csType[${nArray}] GOM75
				} else /if (${Spell[${combatSpells[${nArray}]}].Level} <= 80) {
					/varset csType[${nArray}] GOM80
				} else /if (${Spell[${combatSpells[${nArray}]}].Level} <= 85) {
					/varset csType[${nArray}] GOM85
				} else /if (${Spell[${combatSpells[${nArray}]}].Level} <= 90) {
					/varset csType[${nArray}] GOM90
				} else /if (${Spell[${combatSpells[${nArray}]}].Level} <= 95) {
					/varset csType[${nArray}] GOM95
				} else /if (${Spell[${combatSpells[${nArray}]}].Level} <= 100) {
					/varset csType[${nArray}] GOM100
				} else /if (${Spell[${combatSpells[${nArray}]}].Level} <= 105) {
					/varset csType[${nArray}] GOM105
				} else /if (${Spell[${combatSpells[${nArray}]}].Level} <= 110) {
					/varset csType[${nArray}] GOM110
				} else /if (${Spell[${combatSpells[${nArray}]}].Level} <= 115) {
					/varset csType[${nArray}] GOM115
				} else /if (${Spell[${combatSpells[${nArray}]}].Level} <= 120) {
					/varset csType[${nArray}] GOM120
				}
			}
			/if (${csFlameSpell[${nArray}]}) {
				/if (${csGiftSpell[${nArray}]}) {
					/varset fopgiftSpell TRUE
					/varset csType[${nArray}] ${csType[${nArray}]}FOP
				} else {
					/varset fopSpell TRUE
					/varset csType[${nArray}] FOP
				}
			}
		} else {
			/call GetINISetting ${iniName} Combat SpellName${nArray} NULL NULL
			/call GetINISetting ${iniName} Combat SpellMinMana${nArray} NULL 0
			/call GetINISetting ${iniName} Combat SpellMinMobHPs${nArray} NULL 1
			/call GetINISetting ${iniName} Combat SpellMaxMobHPs${nArray} NULL 100
			/call GetINISetting ${iniName} Combat SpellRecastDelay${nArray} NULL 0s
			/call GetINISetting ${iniName} Combat SpellMaxMobs${nArray} NULL 100
			/call GetINISetting ${iniName} Combat SpellMinMobs${nArray} NULL 1
			/call GetINISetting ${iniName} Combat SpellSlot${nArray} NULL ${If[${Me.Class.CanCast},gem1,DISC]}
			/call GetINISetting ${iniName} Combat SpellCondition${nArray} NULL NULL
			/call GetINISetting ${iniName} Combat SpellMaxHPs${nArray} NULL 100 FALSE
			/call GetINISetting ${iniName} Combat SpellType${nArray} NULL NULL FALSE
			/call GetINISetting ${iniName} Combat SpellGOM${nArray} NULL FALSE ${Me.Class.CanCast}
			/call GetINISetting ${iniName} Combat SpellFOP${nArray} NULL FALSE ${Me.Class.ShortName.Equal[MAG]}			
		}
	/next nArray

| ========================================
|  Load General Settings
| ========================================	
	/declare castWhileInvis				bool outer TRUE
	/declare useLAHoTT						bool outer TRUE
	/call EchoLog "Loading common general settings" TRUE
	
	
	| /call GetINISetting ${iniName} General SpellCoolDown globalCDStr ${globalCDStr}
	| Need to remove references to this setting
	/varset globalCDStr 0s

	/call GetINISetting ${iniName} General CampRadius campRadius 70
	/call GetINISetting ${iniName} General ResistTries resistTries 3 ${Me.Class.CanCast}
	/call GetINISetting ${iniName} General TimeToSitAfterCast sitTimerString 4s ${Me.Class.CanCast}
	/call GetINISetting ${iniName} General ReplyToTells replyToTells FALSE
	/call GetINISetting ${iniName} General Announcements doAnnounce TRUE
	/call GetINISetting ${iniName} General AnnounceChan announceChan /g
	/call GetINISetting ${iniName} General MinSafeDistance minSafeDistance 30
	/call GetINISetting ${iniName} General LootCorpses lootCorpses FALSE
	/call GetINISetting ${iniName} General LootINI lootININame devLoot.ini FALSE
	/call GetINISetting ${iniName} General DebuffAtStart doDebuffs ${If[${debuffClasses.Find[${Me.Class.ShortName}]},TRUE,FALSE]} ${debuffClasses.Find[${Me.Class.ShortName}]}
	/call GetINISetting ${iniName} General MinDebuffCheckTimer debuffCheckTime 5s
	/call GetINISetting ${iniName} General GroupBuffAtStart doGroupBuffs ${Me.Class.CanCast} ${Me.Class.CanCast}
	/call GetINISetting ${iniName} General SelfBuffAtStart doSelfBuffs TRUE
	/call GetINISetting ${iniName} General PetBuffAtStart doPetBuffs ${If[${petClasses.Find[${Me.Class.ShortName}]},TRUE,FALSE]} ${petClasses.Find[${Me.Class.ShortName}]}
	/call GetINISetting ${iniName} General WatchBuffAtStart doWatchBuffs ${Me.Class.CanCast} ${Me.Class.CanCast}
	| /call GetINISetting ${iniName} General AutoBuffAtStart doAutoBuffs TRUE
	| /call GetINISetting ${iniName} General MinBuffCheckTimer minBuffCheckTimerStr 1m
	
	/call GetINISetting ${iniName} General PullerName pullerName Group
	/call GetINISetting ${iniName} General MainTank mainTank Group
	/call GetINISetting ${iniName} General MainAssist mainAssist Group
	/call GetINISetting ${iniName} General SecondaryAssist secondaryAssist NULL
	
	/call GetINISetting ${iniName} General AssistPct assistPct ${assistPct}
	/call GetINISetting ${iniName} General AllowInterrupt allowInterrupt TRUE ${Me.Class.CanCast}
	/call GetINISetting ${iniName} General UnsafePCAction unsafePCAction continue
	/call GetINISetting ${iniName} General UnsafePCRadius unsafePCRadius 300 FALSE
	/call GetINISetting ${iniName} General UnsafePCImmediateAction unsafePCImmediate FALSE bool
	/call GetINISetting ${iniName} General GMAction GMAction stop
	/call GetINISetting ${iniName} General UseMerc useMerc FALSE
	/call GetINISetting ${iniName} General ResistRetryTime resistRetryTime 0s
	/call GetINISetting ${iniName} General CampAfterGate campAfterGate TRUE ${If[${Me.Book[Gate]} || ${Me.AltAbility[Gate]},TRUE,FALSE]}
	/call GetINISetting ${iniName] General CheckForAdds checkForAdds TRUE FALSE
	/call GetINISetting ${iniName} General UseEQBC useEQBC TRUE
	/call GetINISetting ${iniName} General RelayChat relayChat FALSE FALSE
	/call GetINISetting ${iniName} General RelayTells relayTells FALSE FALSE
	/call GetINISetting ${iniName} General RelayTarget relayTarget /bca FALSE
	/call GetINISetting ${iniName} General OutOfCombatSit outOfCombatSit TRUE
	/call GetINISetting ${iniName} General InCombatSit inCombatSit ${If[!${meleeClasses.Find[${Me.Class.ShortName}]},TRUE,FALSE]} ${If[!${meleeClasses.Find[${Me.Class.ShortName}]},TRUE,FALSE]}
	/call GetINISetting ${iniName} General AllowSit anyAllowSit TRUE FALSE
	/call GetINISetting ${iniName} General FaceWhenCasting faceWhenCasting TRUE FALSE
	/call GetINISetting ${iniName} General UseXTargetOnly xTargetOnly FALSE FALSE
	/call GetINISetting ${iniName} General UseXTarget useXTarget TRUE FALSE
	/call GetINISetting ${iniName} General UseLAHoTT useLAHoTT TRUE FALSE
	/call GetINISetting ${iniName} General MercAssistPct mercAssistPct ${assistPct} FALSE FALSE
	/call GetINISetting ${iniName} General DebugMode debugMode ${debugMode} FALSE
	/call GetINISetting ${iniName} General DebugLevel debugLvl ${debugLvl} FALSE
	/call GetINISetting ${iniName} General RespitePct respitePct ${If[${meleeClasses.Find[${Me.Class.ShortName}]},25,0]} ${If[${meleeClasses.Find[${Me.Class.ShortName}]},TRUE,FALSE]}
	/call GetINISetting ${iniName} General HealPotName hPotName NULL
	/call GetINISetting ${iniName} General HealPotHPs hPotHPs 50
	/call GetINISetting ${iniName} General ModRodName modRodNameCommon NULL ${If[${Me.Class.CanCast} && ${Me.Class.ShortName.NotEqual[MAG]},TRUE,FALSE]}
	/call GetINISetting ${iniName} General ModRodMinHPs modRodMinHPsCommon 50 ${If[${Me.Class.CanCast} && ${Me.Class.ShortName.NotEqual[MAG]},TRUE,FALSE]}
	/call GetINISetting ${iniName} General ModRodMana modRodManaCommon 50 ${If[${Me.Class.CanCast} && ${Me.Class.ShortName.NotEqual[MAG]},TRUE,FALSE]}
	/call GetINISetting ${iniName} General FaceStartupDirectionAtCamp useHomeDir FALSE FALSE
	/call GetINISetting ${iniName} General AlwaysAttemptReturn alwaysAttemptReturn FALSE FALSE
	/call GetINISetting ${iniName} General AutoBuffsEnabled autoBuffingEnabled FALSE	
	/call GetINISetting ${iniName} General CastWhileInvis castWhileInvis TRUE
	/call GetINISetting ${iniName} General MinRezAcceptPercent minRezPercent 0
	/call GetINISetting ${iniName} General MaxRezWaitTime maxRezWaitTime 0
	/call GetINISetting ${iniName} General DefaultGem defaultGem GEM1 FALSE
	/call GetINISetting ${iniName} General LootOnlyKilledCorpses lootOnlyKilled TRUE FALSE
	/call GetINISetting ${iniName} General CastMaxWaitTime defaultMaxWaitTime 5s FALSE
	/call GetINISetting ${iniName} General EmuMode emuMode FALSE
	/call GetINISetting ${iniName} General HideCorpses hideCorpses TRUE FALSE
	/call GetINISetting ${iniName} General CampReturnRatio campReturnRatio .5 FALSE

	/declare medModeManaThreshold		int outer 0
	/declare medModeMedTo			int outer 0
	/declare medMode			bool outer FALSE
	/call GetINISetting ${iniName} General MedModeManaThreshold medModeManaThreshold 0
	/call GetINISetting ${iniName} General MedModeMedTo medModeMedTo 80
	
	/varset doAutoBuffs ${autoBuffingEnabled}
	
	/if (${castWhileInvis}) {
		/varset noInvis FALSE
	} else {
		/varset noInvis TRUE
	}
	/call EchoLog ".....GMAction: ${GMAction}" TRUE
	/if (${Me.Book[Gate]} || ${Me.AltAbility[Gate]}) /call EchoLog ".....Camping after gate: ${campAfterGate}" TRUE
	/call EchoLog ".....Facing When Casting: ${faceWhenCasting}" TRUE
	/call EchoLog ".....Casting while invis: ${castWhileInvis}" TRUE
	/if (!${unsafePCAction.Length}) /varset unsafePCAction pause
	/if (!${announceChan.Length} || ${announceChan.Equal[NULL]}) /varset announceChan /g
	/if (${announceChan.Left[1].NotEqual[/]}) /varset announceChan /${announceChan}	

	/if (${autoBuffingEnabled}) /call AutoBuffLoad
	/call GetINISetting devCommonPremium.dat General Version devCommonPremiumDatVersion
	/call EchoLog "Using \agdevCommonPremium.dat \awv${devCommonPremiumDatVersion}"
	
	/if (${useEQBC}) {
		/squelch /plugin MQ2NetBots
		/squelch /netbots on
		/squelch /netbots grab on
		/squelch /netbots send on
	} else {
		/squelch /plugin MQ2NetBots unload
	}
	
| ========================================
|  Load Priority Settings
| ========================================
	|**/declare priorityDebuffs					string outer 20s
	/declare priorityAutoBuffs				string outer 60s
	/declare prioritySelfBuffs				string outer 60s
	/declare priorityGroupBuffs				string outer 30s
	/declare priorityGroupPetBuffs		string outer 120s
	/declare priorityPetBuffs					string outer 120s
	/declare priorityWatchBuffs				string outer 60s
	/declare priorityCombatSpells			string outer 0s
	/declare priorityAddCheck					string outer 5s
	/declare priorityAssist						string outer 3s
	/declare priorityUnsafePC					string outer 30s**|
	/declare priorityDebuffsTimer				timer outer 0s
	/declare priorityAutoBuffsTimer			timer outer 0s
	/declare prioritySelfBuffsTimer			timer outer 0s
	/declare priorityGroupBuffsTimer		timer outer 0s
	/declare priorityGroupPetBuffsTimer	timer outer 0s
	/declare priorityPetBuffsTimer			timer outer 0s
	/declare priorityWatchBuffsTimer		timer outer 0s
	/declare priorityCombatSpellsTimer	timer outer 0s
	/declare priorityAddCheckTimer			timer outer 0s
	/declare priorityAssistTimer				timer outer 0s
	/declare priorityUnsafePCTimer			timer outer 0s
	/call GetINISetting ${iniName} Priorities Debuffs priorityDebuffs 20s string
	/call GetINISetting ${iniName} Priorities AutoBuffs priorityAutoBuffs 60s string
	/call GetINISetting ${iniName} Priorities SelfBuffs prioritySelfBuffs 60s string
	/call GetINISetting ${iniName} Priorities PetBuffs priorityPetBuffs 120s string
	/call GetINISetting ${iniName} Priorities GroupBuffs priorityGroupBuffs 30s string
	/call GetINISetting ${iniName} Priorities GroupPetBuffs priorityGroupPetBuffs 120s string
	/call GetINISetting ${iniName} Priorities WatchBuffs priorityWatchBuffs 60s string
	/call GetINISetting ${iniName} Priorities CombatBuffDelay minBuffCheckTimerStr 10s string
	/call GetINISetting ${iniName} Priorities CombatSpells priorityCombatSpells 0s string
	/call GetINISetting ${iniName} Priorities AddCheck priorityAddCheck 0s string
	/call GetINISetting ${iniName} Priorities AssistFrequency priorityAssist 3s string
	/call GetINISetting ${iniName} Priorities UnsafePCCheck priorityUnsafePC 30s string

| ========================================
|  Load MQ2Navigation settings
| ========================================
	/call EchoLog "Checking for MQ2Navigation for movement" TRUE	
	/call GetINISetting ${iniName} General UseNavigation useNavigation TRUE FALSE
	/if (${useNavigation} && !${Plugin[MQ2Navigation].Version}) /squelch /plugin MQ2Navigation
	/if (${Navigation}) {
		/if (${useNavigation}) {
			/if (!${Navigation.MeshLoaded}) {
				/nav 0 0 0
				/delay 2
				/nav stop
			}
			/if (${Navigation.MeshLoaded}) {
				/call EchoLog ".....MQ2Navigation detected, using it for movements" TRUE
				/if (!${Plugin[MQ2EQDraw].Version}) /call EchoLog "\ayWarning, MQ2Navigation is being used but MQ2EQDraw could not be loaded.  Will not be able to navigate through doors."
			} else {
				/call EchoLog "\ayMQ2Navigation detected and set to be used, but you have no mesh for the current zone, not using MQ2Navigation"
				/varset useNavigation FALSE
			}
		} else {
			/call EchoLog ".....MQ2Navigation detected but set to not be used in the INI" TRUE
		}
	} else {
		/varset useNavigation FALSE
	}
	
	/if (${useNavigation} && !${Plugin[MQ2EQDraw].Version}) /squelch /plugin MQ2EQDraw
	
| ========================================
|  Load Merc Management Settings
| ========================================	
	/call GetINISetting ${iniName} MercSettings BurnPct mercBurnPct 0
	/call GetINISetting ${iniName} MercSettings ManageMerc manageMerc FALSE
	/call GetINISetting ${iniName} MercSettings AssistPct mercAssistPct ${mercAssistPct} FALSE
	/call GetINISetting ${iniName} MercSettings OutOfCombatStance mercOOCStance NULL
	/call GetINISetting ${iniName} MercSettings InCombatStance mercICStance NULL
	/varset mercStance NULL
	
| ====================
| Load Summoned Items
| ====================
	/call GetINIArrayCount ${iniName} Summoned SummonedSpell NULL
	/varset summonedCount ${Macro.Return}
	/call EchoLog "Loading ${summonedCount} Summoned Items" true
	
	/if (${summonedCount}) {
		/for nArray 1 to ${summonedCount}
			/call GetINISetting ${iniName} Summoned SummonedSpell${nArray} summonedSpells[${nArray}] NULL
			/call GetINISetting ${iniName} Summoned SummonedItem${nArray} summonedItems[${nArray}] NULL
			/call GetINISetting ${iniName} Summoned SummonedSlot${nArray} summonedSlots[${nArray}] Item
			/call GetINISetting ${iniName} Summoned SummonedCount${nArray} summonedMinCount[${nArray}] 1
			/call EchoLog ".....${nArray}  ITEM: ${summonedItems[${nArray}]}  SPELL: ${summonedSpells[${nArray}]}  SLOT: ${summonedSlots[${nArray}]}  COUNT: ${summonedMinCount[${nArray}]}" TRUE
		/next nArray
	}
	/call GetINISetting ${iniName} Summoned SummonedSpell${Math.Calc[${summonedCount}+1].Int} NULL NULL
	/call GetINISetting ${iniName} Summoned SummonedItem${Math.Calc[${summonedCount}+1].Int} NULL NULL
	/call GetINISetting ${iniName} Summoned SummonedSlot${Math.Calc[${summonedCount}+1].Int} NULL Item
	/call GetINISetting ${iniName} Summoned SummonedCount${Math.Calc[${summonedCount}+1].Int} NULL 0

| ====================
| Load Mission Settings
| ====================
	/call GetINISetting ${iniName} Mission MissionMode missionMode FALSE bool
	/call GetINISetting ${iniName} Mission MissionINI missionINI NULL string
	| The PC that is the leader for the mission, can be SELF for the PC running the current bot, or GROUP for the group leader
	/call GetINISetting ${iniName} Mission MissionLeader missionLeader GROUP string
	| Mission followers is either a comma separated list of names, or GROUP for all PCs in your group, only the leader will look at this setting
	/call GetINISetting ${iniName} Mission MissionFollowers missionFollowers GROUP string
	/if (!${missionINI.Find[.ini]} && ${missionINI.NotEqual[NULL]}) /varset missionINI ${missionINI}.ini
	| /if (${missionLeader.Equal[GROUP]}) /varset missionLeader ${Group.Leader.Name}
	
	/if (${emuMode}) {
		/varset useMerc FALSE
		/varset manageMerc FALSE
		/varset xTargType Type
	}
	
	/call LootInit FALSE ${lootININame}
	/if (!${Defined[soloMode]} || !${soloMode}) /call PullInit
/return

Sub CorpseCheck
	/declare corpseLoop		int local
	/declare cocLoop		int local
	/declare corpseFound		bool local
	/declare cKillCount			int local 0
	
	/varset lTargCount ${targCount}

	/if (${targCount}) {
		/for corpseLoop 1 to ${targCount}
			/if (!${Spawn[${targArray[${corpseLoop}]}].ID} || (${Spawn[${targArray[${corpseLoop}]}].Type.Equal[Pet]} && ${Spawn[${targArray[${corpseLoop}]}].Master.Type.Equal[PC]}) || ${Spawn[${targArray[${corpseLoop}]}].Type.Equal[Corpse]}) {
				/varset corpseFound FALSE
				/if (${corpseCount} && ${Spawn[${targArray[${corpseLoop}]}].Type.NotEqual[Pet]}) {
					/for cocLoop 1 to ${corpseCount}
						/if (${corpseArray[${cocLoop}]}==${targArray[${corpseLoop}]}) {
							/varset corpseFound TRUE
							/goto :addCorpseOnly
						}
					/next cocLoop
					:addCorpseOnly
				}
				/if (!${corpseFound} && ${targArray[${corpseLoop}]}) {
					/if (${Spawn[${targArray[${corpseLoop}]}].Named}) /varcalc namedCount ${namedCount} - 1
					/varcalc corpseCount ${corpseCount} + 1
					/varset corpseArray[${corpseCount}] ${targArray[${corpseLoop}]}
					/if (${enrageID}==${targArray[${corpseLoop}]}) {
						/varset enrageID 0
						/varset enrageAttackOff FALSE
					}
					/if (${Spawn[${targArray[${corpseLoop}]}].Type.Equal[Pet]} && ${Spawn[${targArray[${corpseLoop}]}].Master.Type.Equal[PC]}) {
						/call EchoLog "Spawn ${Spawn[${targArray[${corpseLoop}]}].CleanName} | ${targArray[${corpseLoop}]}, is now a pc pet removing from target list" TRUE
					} else {
						/if (${trackKills}) {
							/call TrackKill "${Spawn[${targArray[${corpseLoop}]}].CleanName}"
							/call TrackKillCount "${Spawn[${targArray[${corpseLoop}]}].CleanName}"
							/varset cKillCount ${Macro.Return}
						}
						/call EchoLog "Corpse added ${Spawn[${targArray[${corpseLoop}]}].CleanName} | ${targArray[${corpseLoop}]}, Corpse number ${corpseCount}${If[${trackKills}, Kill Count: ${cKillCount},]}" TRUE
					}
					/varset targArray[${corpseLoop}] 0
				}
			}
		/next corpseLoop
		/varset lTargCount ${Math.Calc[${targCount} - ${corpseCount}]}
	}
/return

Sub ChooseTarget(int forceTarg, int baseAround)
	/declare lowHealth 					int local
	/declare lowDistance				float local
	/declare mHealth 						int local
	/declare mDistance 					int local
	/declare mFleeing 					int local
	/declare Flee 							bool local
	/declare lnewAdd 						bool local
	/declare mNew 							int local
	/declare coLoop 						int local
	/declare corpseFound 				bool local
	/declare oTargID 						int local
	/declare tDist							float local
	/declare tLoop							int local
	/declare cMob								int local
	/declare waitingOnAssist		bool local FALSE
	/declare mercDistance				float local 5000
	/declare merclowDistance		int local 0
	/declare baID								int local 0
	
	/varset oTargID ${targID}
	/varset lowHealth 100
	/varset lowDistance 5000
	/varset Flee FALSE
	/varset newAdd FALSE
	/varset targID 0
	/varset mHealth 0
	/varset mFleeing 0
	/varset mDistance 0
	/varset cMob 0
	/varset mNew 0
	/varset lTargCount ${targCount}
	/varset newAdd FALSE
	/varset newTarg FALSE

	/if (!${Defined[forceTarg]}) /declare forceTarg int local 0
	/if (!${Defined[baseAround]}) /declare baseAround int local ${Me.ID}
	
	/if ((!${forceTarg} || !${useForceTarg}) && (!${groupMode} || !${Spawn[${mainAssist}].ID} || ${Spawn[${mainAssist}].Type.Equal[Corpse]} || (${Spawn[${mainAssist}].Linkdead} && ${Spawn[${mainAssist}].Type.NotEqual[mercenary]}) || ${mainAssist.Equal[${Me.CleanName}]} || ${mainAssist.Equal[NULL]})) {
		/for tLoop 1 to ${targCount}
			/if (${Spawn[${targArray[${tLoop}]}].Type.Equal[NPC]}) {
				/varset tDist ${Math.Distance[${Spawn[${targArray[${tLoop}]}].Y},${Spawn[${targArray[${tLoop}]}].X}:${Spawn[${baseAround}].Y},${Spawn[${baseAround}].X}]}
				/if (${tDist}<${lowDistance}) {
					/varset mDistance ${targArray[${tLoop}]}
					/varset lowDistance ${tDist}
				}
				/if (${Spawn[${targArray[${tLoop}]}].PctHPs}<=${lowHealth}) {
					/varset mHealth ${targArray[${tLoop}]}
					/varset lowHealth ${Spawn[${targArray[${tLoop}]}].PctHPs}
				}
				/if (${Spawn[${targArray[${tLoop}]}].Fleeing}) {
					/varset mFleeing ${targArray[${tLoop}]}
					/varset Flee TRUE
				}
				/if (${Spawn[${targArray[${tLoop}]}].PctHPs}==100) {
					/varset mNew ${targArray[${tLoop}]}
					/varset lnewAdd TRUE
				}
				/if (${Spawn[${targArray[${tLoop}]}].Class.HealerType}) /varset cMob ${targArray[${tLoop}]}	
				/if (${useMerc} && ${mercID}) {
					/if (${Math.Distance[${Spawn[${mercID}].Y},${Spawn[${mercID}].X}:${Spawn[${targArray[${tLoop}]}].Y},${Spawn[${targArray[${tLoop}]}].X}]} < ${mercDistance}) {
						/varset merclowDistance ${Math.Distance[${Spawn[${mercID}].Y},${Spawn[${mercID}].X}:${Spawn[${targArray[${tLoop}]}].Y},${Spawn[${targArray[${tLoop}]}].X}]}
						/varset mercDistance ${targArray[${tLoop}]}
					}
				}
			} 
		/next tLoop
	
		/if (${baseAround} != ${Me.ID}) {
			/if ((${lowDistance}<${minSafeDistance} && ${Me.Class.PureCaster}) || ${Spawn[${oTargID}].Type.NotEqual[NPC]}) {
				/varset targID ${mDistance}
			} else {
				/varset targID ${oTargID}
			}
		} else {
			/if (${lowDistance}<${minSafeDistance} && ${Me.Class.PureCaster}) {
				/varset targID ${mDistance}
				/varset newTarg TRUE
			} else /if (${lnewAdd} && ${Me.Class.PetClass} && ${Me.Class.PureCaster}) {
				/varset targID ${mNew}
			} else /if (${useMerc} && ${mercID} && ${merclowDistance}<${minSafeDistance}) {
				/varset targID ${mercDistance}
				/varset newTarg TRUE
			} else /if (${Flee}) {
				/varset targID ${mFleeing}
			} else /if (${cMob}) {
				/varset targID ${cMob}
			} else {
				/if (${lowHealth}<=20) {
					/varset targID ${mHealth}
				} else {
					/if (${Me.Class.PureCaster} || ${Spawn[${oTargID}].Type.NotEqual[NPC]}) {
						/varset targID ${mDistance}
					} else {
						/varset targID ${oTargID}
					}
				}
			}
		}
	} else /if (${forceTarg} && ${useForceTarg} && (!${groupMode} || !${Spawn[${mainAssist}].ID} || ${Spawn[${mainAssist}].Type.Equal[Corpse]} || (${Spawn[${mainAssist}].Linkdead} && ${Spawn[${mainAssist}].Type.NotEqual[mercenary]}) || ${mainAssist.Equal[${Me.CleanName}]} || ${mainAssist.Equal[NULL]})) {
		/varset targID ${forceTarg}
		/varset newAdd TRUE
		/varset newTarg TRUE
	}

	/if (${forceTarg} && !${targID}) /varset targID ${forceTarg}
	/if (${targID} != ${oTargID} || ${Target.PctHPs} > 99) /varset newTarg TRUE
/return

Sub ClearCursor(string clearType,string maxAttemptTime)
	/if (!${Defined[clearType]}) /declare clearType string local AUTOINVENTORY
	/if (!${Defined[maxAttemptTime]}) /declare maxAttemptTime string local 15s
	/if (!${Select[${clearType},AUTOINV,AUTO,AUTOINVENTORY,DESTROY,DEST]}) /varset clearType AUTOINVENTORY
	/declare clearCursorTimer		timer local ${maxAttemptTime}

	:clearCursorLoop
	/if (${Cursor.ID}) {
		/if (${clearType.Find[AUTO]}) {
			/autoinventory
		} else /if (${clearType.Find[DEST]}) {
			/destroy
		}
		/delay 5 !${Cursor.ID}
	}
	/if (${Cursor.ID} && ${clearCursorTimer}) /goto :clearCursorLoop
	
	/if (${Cursor.ID} && !${clearCursorTimer}) {
		/call EchoLog "\ayFailed to clear ${Cursor.Name} from the cursor in the allotted time: ${maxAttemptTime}"
	}
/return

Sub CreateFriendList
	/declare cFLInt					int local
	/declare watchID				int local
	
	/varset friendList |${Me.ID}|
	/if (${Group.Members}) {
		| /varset friendList ${friendList}|
		/for cFLInt 1 to ${Group.Members}
			/if (${Group.Member[${cFLInt}].ID}) {
				/varset friendList ${friendList}|${Group.Member[${cFLInt}].ID}|
				/if (${Group.Member[${cFLInt}].Pet.ID} && !${Group.Member[${cFLInt}].Pet.Name.Find[familiar]}) /varset friendList ${friendList}|${Group.Member[${cFLInt}].Pet.ID}|
				| /if (${cFLInt}<${Group.Members}) /varset friendList ${friendList}|
			}
		/next cFLInt
	}
		
	/if (${watchCount}) {
		/varset friendList ${friendList}|
		/for cFLInt 1 to ${watchCount}
			/if (${watchList[${cFLInt}].NotEqual[NULL]}) {
				/varset watchID ${Spawn[${watchList[${cFLInt}]}].ID}
				/if (${Spawn[${watchID}].Type.Equal[PET]} && ${Spawn[${watchID}].Master.CleanName.Equal[${watchList[${cFLInt}]}]}) /varset watchID ${Spawn[${watchID}].Master.ID}
				/if (${watchID}) {
					/varset friendList ${friendList}|${watchID}|
					/if (${Spawn[${watchID}].Pet.ID} && !${Spawn[${watchID}].Pet.Name.Find[familiar]}) /varset friendList ${friendList}|${Spawn[${watchID}].Pet.ID}|
				}
			}
			/if (${cFLInt}<${watchCount}) /varset friendList ${friendList}|
		/next cFLInt
	}
	
	/if (${mainAssist.NotEqual[NULL]} && !${useGroupAssist} && ${Spawn[${mainAssist}].ID}) /varset friendList ${friendList}|${If[${Spawn[${mainAssist}].Type.NotEqual[Pet]},${Spawn[${mainAssist}].ID},${Spawn[${mainAssist}].Master.ID}]}|
	/if (${mainTank.NotEqual[NULL]} && !${useGroupTank} && ${Spawn[${mainTank}].ID}) /varset friendList ${friendList}|${If[${Spawn[${mainTank}].Type.NotEqual[Pet]},${Spawn[${mainTank}].ID},${Spawn[${mainTank}].Master.ID}]}|
	/if (${pullerName.NotEqual[NULL]} && !${useGroupPuller} && ${Spawn[${pullerName}].ID}) /varset friendList ${friendList}|${If[${Spawn[${pullerName}].Type.NotEqual[Pet]},${Spawn[${mainAssist}].ID},${Spawn[${pullerName}].Master.ID}]}|
/return

Sub CreateIgnoreAlert(int aListNum)
	/if (!${Defined[aListNum]}) /declare aListNum int local ${alertList}
	/varset alertList ${aListNum}
	
	/squelch /alert clear ${aListNum}
	/call EchoLog "\aoCreating ignore alert list number \ag${alertList}"	
	/if (${ignoreMobsCount} > 0) {
		/for nArray 1 to ${ignoreMobsCount}
			/if (${ignoreMobs[${nArray}].NotEqual[NULL]} && ${ignoreMobs[${nArray}].Length}) {
				/squelch /alert add ${aListNum} ${ignoreMobs[${nArray}]}
				/call EchoLog ".....\ag${ignoreMobs[${nArray}]} \aoadded to ignore alert list"
			}
		/next nArray
	}
	
	/call EchoLog ".....Adding untargetable mobs in the zone to the ignore list" TRUE
	/squelch /alert add ${aListNum} Untargetable
/return

Sub CreateSafeAlert(int aListNum)
	/if (!${Defined[aListNum]}) /declare aListNum int local ${safeAlertList}
	/varset safeAlertList ${aListNum}
	
	/squelch /alert clear ${aListNum}
	/if (${Group.Members} || ${safePCCount}) /call EchoLog "Creating safe pc alert list number \ag${safeAlertList}"
	/if (${Group.Members}) {
		/for nArray 1 to ${Group.Members}
			/if (${Group.Member[${nArray}].Type.Equal[PC]}) {
				/squelch /alert add ${aListNum} ${Group.Member[${nArray}].CleanName}
				/call EchoLog "\ag${Group.Member[${nArray}].CleanName} \aoadded to safe pc alert list (from group)"
			}
		/next nArray
	}
	/if (${safePCCount}) {
		/for nArray 1 to ${safePCCount}
			/squelch /alert add ${aListNum} ${safePC[${nArray}]}
			/call EchoLog "\ag${safePC[${nArray}]} \aoadded to safe pc alert list (from ini)"
		/next nArray
	}
/return

Sub Debuff(int debuffNum, int mobID)
	/if (${debuffSlot[${debuffNum}].Equal[Item]} && ${FindItem[${debuffSpells[${debuffNum}]}].Timer}) /return
	/if (${debuffSlot[${debuffNum}].Equal[alt]} && !${Me.AltAbilityReady[${debuffSpells[${debuffNum}]}]}) /return

	/squelch /target clear
	/squelch /target ID ${mobID}
	/delay 1s ${Target.ID}==${mobID}
	/if (${Target.ID}!=${mobID}) /return DEBUFF_NOTARGET
	
	/declare retValue							string local DEBUFF_CONDITIONS_FAIL
	/declare dResTries						int local 1
	
	/if (${debuffMinMobs[${debuffNum}]}==1) /varset dResTries ${resistTries}

	/call CheckForAggro
	/if (${Target.ID} && ${Target.Type.Equal[NPC]} && ${Target.LineOfSight} && ${gotAggroID} != ${Target.ID}) {
		/if (${Target.PctHPs}<=${debuffMaxHPs[${debuffNum}]} && ${Target.PctHPs}>=${debuffMinHPs[${debuffNum}]}) {
			/call EchoLog "Attempting to cast debuff ${debuffSpells[${debuffNum}]} on ${Target.CleanName} | ${Target.ID}" TRUE
			/if (${debuffSpellSlot[${debuffNum}].Find[Gem]}) {
				/if (${Me.Gem[${debuffSpells[${debuffNum}]}]}) {
					/if (${Me.GemTimer[${debuffSpells[${debuffNum}]}]} && ${Me.GemTimer[${debuffSpells[${debuffNum}]}].Float} > 5) {
						/call EchoLog ".....Cannot cast debuff because it has a cooldown on it." TRUE
						/return ${retValue}
					}
				} else {
					/if (${Spell[${debuffSpells[${debuffNum}]}].RecoveryTime.Float} > 5) {
						/call EchoLog ".....Cannot cast debuff because it is not memmed and has a long recovery time, will mem and attempt to cast later." TRUE
						/memspell ${debuffSpellSlot[${debuffNum}].Right[-3]} "${debuffSpells[${debuffNum}]}"
						/delay 5
						/return ${retValue}
					}
				}
			}
			/if (${faceWhenCasting}) /face fast nolook
			/call MQ2Cast "${debuffSpells[${debuffNum}]}" ${debuffSpellSlot[${debuffNum}]} 0 CheckForAggro -targetid|${Target.ID} -maxtries|${dResTries}
			/if (!${Defined[debuffTimer${debuffNum}Mob${mobID}]} && ${debuffMinMobs[${debuffNum}]}==1) /declare debuffTimer${debuffNum}Mob${mobID} timer outer 0s
			/if (!${Defined[debuffTimer${debuffNum}]} && ${debuffMinMobs[${debuffNum}]}>1) /declare debuffTimer${debuffNum} timer outer 0s
			/if (${CastResult.Equal[CAST_SUCCESS]} || ${CastResult.Equal[CAST_PARTIAL]}) {
				/if (${debuffMinMobs[${debuffNum}]}>1) {
					/varset debuffTimer${debuffNum} ${debuffRecastDly[${debuffNum}]}
				} else {
					/varset debuffTimer${debuffNum}Mob${mobID} ${debuffRecastDly[${debuffNum}]}
				}
				/call Announce "${debuffAnnounce[${debuffNum}]}"
				/varset retValue DEBUFF_SUCCESS
			} else /if (${CastResult.Equal[CAST_IMMUNE]}) {
				/if (${debuffMinMobs[${debuffNum}]}>1) {
					/varset debuffTimer${debuffNum} ${debuffRecastDly[${debuffNum}]}
				} else {
					/varset debuffTimer${debuffNum}Mob${mobID} 60m
				}
				/varset retValue DEBUFF_IMMUNE
			} else /if (${CastResult.Equal[CAST_RESIST]}) {
				/if (${debuffMinMobs[${debuffNum}]}>1) {
					/varset debuffTimer${debuffNum} ${debuffRecastDly[${debuffNum}]}
				} else {
					/varset debuffTimer${debuffNum}Mob${mobID} ${resistRetryTime}
					
					/if (${debuffCOR[${debuffNum}].NotEqual[NULL]}) {
						/declare corArray					int local
						/for corArray 1 to ${debuffCount}
							/if (${debuffAlias[${corArray}].Equal[${debuffCOR[${debuffNum}]}]}) {
								/if (!${Defined[debuffTimer${corArray}Mob${mobID}]} || !${debuffTimer${corArray}Mob${mobID}}) {
									/call EchoLog "Debuff ${debuffSpells[${debuffNum}]} was resisted, attempting to use CastOnResist spell ${debuffSpells[${corArray}]}" TRUE
									/call Debuff ${corArray} ${mobID}
									/return ${Macro.Return}
								}
							}
						/next corArray
					}
				}
				/varset retValue DEBUFF_RESIST
			} else /if (${CastResult.Equal[CAST_TAKEHOLD]}) {
				/if (${debuffMinMobs[${debuffNum}]}>1) {
					/varset debuffTimer${debuffNum} ${debuffRecastDly[${debuffNum}]}
				} else {
					/varset debuffTimer${debuffNum}Mob${mobID} ${debuffRecastDly[${debuffNum}]}
					/call EchoLog "Debuff was unable to take hold.  Using regular recast delay to try again. ${debuffRecastDly[${debuffNum}]}" TRUE
				}
				/varset retValue DEBUFF_RESIST				
			} else {
				/call EchoLog "Debuff cast unsuccessfull, reason: ${CastResult}" TRUE
				/varset retValue DEBUFF_${CastResult}
			}
			/varset sitTimer ${sitTimerString}
			/varset globalCooldown ${globalCDStr}
		}
	}
/return ${retValue}

Sub DebuffCheck(int mobID)
	/if (!${Spawn[${mobID}].ID} || ${Spawn[${mobID}].Type.NotEqual[NPC]}) {
		/call DebuffCleanUp ${mobID}
		/return DEBUFF_NOMOB
	}
	/if (${Spawn[${mobID}].Distance} > ${campRadius}) /return DEBUFF_TOOFAR

	/declare dArray				int local
	/for dArray 1 to ${debuffCount}
		/if (${debuffEnabled[${dArray}]}) {
			/if (${lTargCount}>=${debuffMinMobs[${dArray}]} && ${lTargCount}<=${debuffMaxMobs[${dArray}]}) {		
				/if (${debuffMinMobs[${dArray}]}>1) {
					/if (!${Defined[debuffTimer${dArray}]} || (${Defined[debuffTimer${dArray}]} && !${debuffTimer${dArray}})) {
						/call Debuff ${dArray} ${mobID}
						/if (!${debuffAll}) /return ${Macro.Return}
					}
				} else {
					/if (!${Defined[debuffTimer${dArray}Mob${mobID}]} || (${Defined[debuffTimer${dArray}Mob${mobID}]} && !${debuffTimer${dArray}Mob${mobID}})) {
						/call Debuff ${dArray} ${mobID}
						/if (!${debuffAll}) /return ${Macro.Return}
					}
				}
			}
		}
	/next dArray
/return DEBUFF_SUCCESS

Sub DebuffCleanUp(int mobID)
	/declare dArray				int local
	
	/for dArray 1 to ${debuffCount}
		/if (${Defined[debuffTimer${dArray}Mob${mobID}]}) /deletevar debuffTimer${dArray}Mob${mobID}
		/if (${Defined[debuffTimer${dArray}]}) /varset debuffTimer${dArray} 0s
	/next dArray
/return

Sub DebuffRoutine(int mobID)
	/if (${priorityDebuffsTimer}) /return ABORT_TIMER
	/if (${debuffCheckTimer}) /return ABORT_TIMER
	/if (${Me.Moving}) /return ABORT_MOVING
	/if (!${Defined[mobID]}) /declare mobID		int local 0
	/declare currID				int local
	/declare dArray				int local
	
	/if (!${mobID}) {
		/if (${debuffAdds}) {
			/if (${targCount}) {
				/for dArray ${targCount} downto 1
					/if (${targArray[${dArray}]}) {
						/call DebuffCheck ${targArray[${dArray}]}
						/if ((${debuffAll} && ${debuffAllOneTarget}) || (!${debuffAll} && (${Macro.Return.Equal[DEBUFF_SUCCESS]} || ${Macro.Return.Equal[DEBUFF_IMMUNE]} || ${Macro.Return.Equal[DEBUFF_RESIST]}))) /return
					}
				/next dArray
			}
		} else {
			/if (${groupMode} && ${mainAssist.NotEqual[null]} && ${mainAssist.Length}) {
				/varset currID ${Target.ID}
				/assist ${mainAssist}
				/delay 1s ${Target.ID}!=${currID}
			}
			/call DebuffCheck ${Target.ID}
		}
	} else {
		/call DebuffCheck ${mobID}
	}
	/if (!${Select[${Macro.Return},DEBUFF_SUCCESS,DEBUFF_IMMUNE,DEBUFF_RESIST]}) {
		/varset debuffCheckTimer ${debuffCheckTime} 
		/varset priorityDebuffsTimer ${priorityDebuffs}s
	}
/return

Sub DoNotAttackCheck
	/declare dnaBool				bool local TRUE
	/declare dnaArray				int local 0
	
	/if (${Target.ID} && ${Target.BuffCount}) {
		/for dnaArray 1 to 30
			/if (${dnaString.Find[${Target.Buff[${dnaArray}]}]}) /varset dnaBool FALSE
		/next dnaArray
	}
/return ${dnaBool}

Sub EchoLog(string eMsg, bool logOnly, int msgLevel)
	/declare sMsg			string local

	/if (!${Defined[eMsg]})	/return
	/if (!${Defined[logOnly]}) /declare logOnly	bool local FALSE
	/if (!${Defined[msgLevel]}) /declare msgLevel int local 0

	/if (${msgLevel} <= ${debugLvl}) {
		/call StripText "${eMsg}"
		/varset sMsg ${Macro.Return}
		/varset eMsg ${defaultEchoColor}${eMsg}
		/if (!${colorEcho}) /varset eMsg ${sMsg}
		/if (!${logOnly} || ${debugMode}) /myecho >> ${eMsg}
	
		/mqlog ${sMsg}
	}
/return

Sub Evac(bool campOut, bool campOutOnly, bool silentEvac)
	/if (!${Defined[campOut]}) /declare campOut bool local TRUE
	/if (!${Defined[campOutOnly]}) /declare campOutOnly bool local FALSE
	/if (!${Defined[silentEvac]}) /declare silentEvac bool local FALSE
	
	/if (!${campOutOnly}) {
		/call EchoLog "Attempting to gate out"
		/if (!${silentEvac}) /squelch /exec "${alertEvac}" bg
		/if (${Me.AltAbilityReady[Gate]}) {
			/call MQ2Cast "Gate" alt
		} else {
			/if (${Me.Book[Gate]}) {
				/call MQ2Cast "Gate" gem1
			} else {
				/call EchoLog "This character does not have the gate spell." TRUE
			}
		}
		/doevents
		/delay 1m ${Zone.ShortName.NotEqual[${startZone}]}
		/delay 5s
		/doevents
	} else {
		/varset campOut TRUE
	}
	/if (${campOut}) {
		/if (${Me.Pet.ID} && ${Me.AltAbilityReady[Suspended Minion]})	/call MQ2Cast "Suspended Minion" alt
		/call EchoLog "Camping"
		/camp desktop
	}
	/endmacro
/return

Sub GetINISetting(string ININame,string INISection,string INIKey,string VariableName,string defaultValue,string createSettingStr,string variableType)
	/if (!${Defined[ININame]}) /return INVALIDINI
	/if (!${Defined[INISection]}) /return INVALIDSECTION
	/if (!${Defined[INIKey]}) /return INVALIDKEY
	/if (!${Defined[VariableName]}) /return INVALIDVARIABLE	
	/if (${Defined[createSettingStr]} && ${createSettingStr.NotEqual[TRUE]} && ${createSettingStr.NotEqual[FALSE]} && !${Defined[variableType]}) {
		/declare variableType		string local ${createSettingStr}
		/declare createSetting	bool local TRUE
	} else /if (${Defined[createSettingStr]}) {
		/declare createSetting	bool local ${createSettingStr}
	}
	/if (!${Defined[createSetting]}) /declare createSetting 	bool local TRUE
	/if (!${Defined[defaultValue]}) /declare defaultValue 		string local NULL
	/if (!${Defined[variableType]}) /declare variableType			string local NULL
	/declare iniValue						string local NULL
	
	/varset iniValue ${Ini[${ININame},${INISection},${INIKey},DOES_NOT_EXIST]}
	/if (${iniValue.Equal[DOES_NOT_EXIST]}) {
		/if (${createSetting}) /ini "${ININame}" "${INISection}" "${INIKey}" "${defaultValue}"
		/varset iniValue ${defaultValue}
	}
	/if (!${Defined[${VariableName}]} && ${variableType.NotEqual[NULL]}) /declare ${VariableName} ${variableType} outer
	/if (${VariableName.NotEqual[NULL]}) /varset ${VariableName} ${iniValue}
/return COMPLETED

| Needs to be worked on if you want to use!!  Is ok for simple applications
Sub GetINIArray(string ININame,string INISection,string INIKeyPrefix,string ArrayName,string ArrayType,string defaultValue,int createNum)
	/if (!${Defined[ININame]}) /return INVALIDINI
	/if (!${Defined[INISection]}) /return INVALIDSECTION
	/if (!${Defined[INIKeyPrefix]}) /return INVALIDKEYPREFIX
  /if (!${Defined[ArrayName]}) /return INVALIDARRAY
  /if (!${Defined[ArrayType]}) /declare ArrayType						string local string
	/if (!${Defined[createNum]}) /declare createNum 					int local 1
	/if (!${Defined[defaultValue]}) /declare defaultValue 		string local NULL
	/declare iniValue						string local NULL
	/declare iniArrayCount			int local 0
	/declare arraySize					int local 0
	
	:iniCountArrayLoop
		/varcalc iniArrayCount ${iniArrayCount} + 1
		/varset iniValue ${Ini[${ININame},${INISection},${INIKeyPrefix}${iniArrayCount},DOES_NOT_EXIST]}
	/if (${iniValue.NotEqual[DOES_NOT_EXIST]}) /goto :iniCountArrayLoop
	/varcalc iniArrayCount ${iniArrayCount} - 1
	/varset arraySize ${iniArrayCount}
	/if (${iniArrayCount} < ${createNum}) /varset iniArrayCount ${createNum}
	
	/if (!${Defined[${ArrayName}]} && ${iniArrayCount}) {
		/declare ${ArrayName}[${arraySize}]					${ArrayType} outer
	}
	
	/for nArray 1 to ${iniArrayCount}
		/varset iniValue ${Ini[${ININame},${INISection},${INIKeyPrefix}${nArray},DOES_NOT_EXIST]}
		/if (${iniValue.Equal[DOES_NOT_EXIST]}) {
			/ini "${ININame}" "${INISection}" "${INIKeyPrefix}${nArray}" "${defaultValue}"
			/varset iniValue ${defaultValue}
		}
		/if (${nArray} <= ${arraySize}) /varset ${ArrayName}[${nArray}] ${iniValue}
	/next nArray
/return COMPLETED

Sub GetINIArrayCount(string ININame,string INISection,string INIKeyPrefix,string invalidValue)
	/if (!${Defined[ININame]}) /return 0
	/if (!${Defined[INISection]}) /return 0
	/if (!${Defined[INIKeyPrefix]}) /return 0
	/if (!${Defined[invalidValue]}) /declare invalidValue				string local NULL
	/declare iniCount					int local 0
	/declare nullCount				int local 0
	
	:iniArrayLoop
		/varcalc iniCount ${iniCount} + 1
		/if (${Ini[${ININame},${INISection},${INIKeyPrefix}${iniCount},${invalidValue}].Equal[${invalidValue}]}) {
			/varcalc nullCount ${nullCount} + 1
		} else {
			/varset nullCount 0
		}
	| /if (${Ini[${ININame},${INISection},${INIKeyPrefix}${iniCount},${invalidValue}].NotEqual[${invalidValue}]}) /goto :iniArrayLoop
	/if (${nullCount} < 5) /goto :iniArrayLoop
	/varcalc iniCount ${iniCount} - ${nullCount}
/return ${iniCount}

Sub GetItemCount(string itemName)
	/declare itemCount		int local 0
	/declare primaryLoop	int local 0
	/declare bagLoop			int local 0
	
	/if (${FindItem[${itemName}].ID}) {
		/if (!${FindItem[${itemName}].Stackable}) {
			/for primaryLoop 1 to 10
				/if (${Me.Inventory[pack${primaryLoop}].Name.Equal[${itemName}]}) /varcalc itemCount ${itemCount} + 1
				/if (${Me.Inventory[pack${primaryLoop}].Container}) {
					/for bagLoop 1 to ${Me.Inventory[pack${primaryLoop}].Container}
						/if (${Me.Inventory[pack${primaryLoop}].Item[${bagLoop}].Name.Equal[${itemName}]}) /varcalc itemCount ${itemCount} + 1
					/next bagLoop
				}
			/next primaryLoop
		} else {
			/varset itemCount ${FindItem[${itemName}].StackCount}
		}
	}
/return ${itemCount}

Sub GetMATarget()
	/declare myID					int local 0
	/declare xtNum				int local 0
	/declare oMaTargetID	int local ${maTargetID}
	/declare maTargLoop		int local 0
	
	/varset maTargetID 0
	/if (${mainAssist.Equal[NULL]} && !${Group.MainAssist.ID}) {
		/varset maTargetID 0
		/return 0
	}
	/if (${mainAssist.Equal[${Me.CleanName}]}) /varset maTargetID ${Target.ID}
	
	/if (${mainAssist.Left[7].Equal[XTARGET]}) {
		/varset xtNum ${mainAssist.Replace[XTARGET,]}
		/varset maTargetID ${Me.XTarget[${xtNum}].ID}		
	}
	
	/if (!${maTargetID}) {
		/for maTargLoop 1 to ${maxXTargets}
			/if (${Me.XTarget[${maTargLoop}].${xTargType}.Equal[Group Assist Target]}) {
				/varset maTargetID ${Me.XTarget[${maTargLoop}].ID}
			}
		/if (!${maTargetID}) /next maTargLoop
	}
			
	/if (!${maTargetID} && ${Group.MainAssist.ID} && ${Group.MainAssist.Type.NotEqual[Corpse]} && (${Group.MainAssist.CleanName.Equal[${mainAssist}]} || ${Group.MainAssist.CleanName.Equal[${mainTank}]})) {
		/for maTargLoop 1 to ${targCount}
			/if (${Spawn[${targArray[${maTargLoop}]}].Assist}) /varset maTargetID ${targArray[${maTargLoop}]}
		/if (!${maTargetID}) /next maTargLoop
	}
	
	/if (!${maTargetID} && ${Spawn[${mainAssist}].ID}) {
		/varset myID ${Target.ID}
		/assist ${mainAssist}
		/delay 1s ${Target.ID}!=${myID}
		/if (${Target.ID}!=${myID} || ${oMaTargetID}==${myID}) /varset maTargetID ${Target.ID}
	}
/return ${maTargetID}

Sub GetMTTarget()
	/declare myID					int local 0
	/declare oMtTargetID	int local ${mtTargetID}
	/declare mtTargLoop		int local 0
	
	/varset mtTargetID 0
	/if (${mainTank.Equal[NULL]} && !${Group.MainTank.ID}) {
		/varset mtTargetID 0
		/return 0
	}
	/if (${mainTank.Equal[${Me.CleanName}]}) /varset mtTargetID ${Target.ID}
	
	/if (!${mtTargetID}) {
		/for mtTargLoop 1 to ${maxXTargets}
			/if (${Me.XTarget[${mtTargLoop}].${xTargType}.Equal[Group Tank's Target]}) {
				| '
				/varset mtTargetID ${Me.XTarget[${mtTargLoop}].ID}
			}
		/if (!${mtTargetID}) /next mtTargLoop
	}
	
	/if (!${mtTargetID}) {
		/varset myID ${Target.ID}
		/assist ${mainTank}
		/delay 1s ${Target.ID}!=${myID}
		/if (${Target.ID}!=${myID} || ${oMtTargetID}==${myID}) /varset mtTargetID ${Target.ID}
	}		
/return ${mtTargetID}

Sub CommonGetTarget(bool targNew)
	/if (!${Defined[targNew]}) /declare targNew bool local TRUE
	/declare newTargID						int local 0
	/declare targLoop							int local 0

	/if (${Target.ID} && ${Target.Type.Equal[Corpse]}) /squelch /target clear	
	/if (${mainAssist.NotEqual[${Me.CleanName}]} && ${mainAssist.NotEqual[NULL]}) {
		/if (!${priorityAssistTimer}) {
			/call GetMATarget
			/varset priorityAssistTimer ${priorityAssist}
			/if (${Target.ID}!=${maTargetID}) /varset newTargID ${maTargetID}
		}
	} else {
		/if (!${Target.ID} || (${Target.Type.NotEqual[NPC]} && (${Target.Type.NotEqual[Pet]} || ${Target.Master.Type.Equal[PC]}))) {
			/if (${mainTank.NotEqual[NULL]} && ${Spawn[${mainTank}].ID} && ${mainTank.NotEqual[${Me.CleanName}]}) {
				/call GetMTTarget
				/if (${mtTargetID} && ${mtTargetID}!=${Target.ID} && ${Spawn[${mtTargetID}].Type.NotEqual[Corpse]} && ${Math.Distance[${homeY},${homeX}:${Spawn[${mtTargetID}].Y},${Spawn[${mtTargetID}].X}]} <= ${campRadius}) /varset newTargID ${mtTargetID}
			}
			/if (!${newTargID} && ${targCount}) {
				/if (${namedCount}) {
					/for targLoop 1 to ${targCount}
						/if (${targArray[${targLoop}]} && ${Spawn[${targArray[${targLoop}]}].Name.Left[1].Equal[#]} && ${Spawn[${targArray[${targLoop}]}].Type.NotEqual[Corpse]} && ${Math.Distance[${homeY},${homeX}:${Spawn[${targArray[${targLoop}]}].Y},${Spawn[${targArray[${targLoop}]}].X}]} <= ${campRadius}) /varset newTargID ${targArray[${targLoop}]}
					/next targLoop
				}
				/if (!${newTargID}) {
					/for targLoop 1 to ${targCount}
						/if (${targArray[${targLoop}]} && ${Spawn[${targArray[${targLoop}]}].Type.NotEqual[Corpse]} && ${Math.Distance[${homeY},${homeX}:${Spawn[${targArray[${targLoop}]}].Y},${Spawn[${targArray[${targLoop}]}].X}]} <= ${campRadius}) /varset newTargID ${targArray[${targLoop}]}
					/next targLoop
				}
			}
		}
	}
	/if (${newTargID} && ${targNew} && ${newTargID}!=${Target.ID}) {
		/squelch /target id ${newTargID}
		/delay 2s ${Target.ID}==${newTargID}
	}	
/return ${newTargID}

Sub GlobalCommands(string comFrom, string comText, bool fromEQBC)
	/if (!${Defined[comFrom]} || !${Defined[comText]}) /return ABORT_PARAMS
	/if (!${Defined[fromEQBC]}) /declare fromEQBC				bool local FALSE
	/declare retValue									string local COMPLETED_NOTFOUND
	/declare paramCount								int local
	/varcalc paramCount ${comText.Count[ ]} + 1
	/declare cParams[${paramCount}]		string local
	/declare strCommand								string local
	/declare ctArray									int local 0
	
	/varset strCommand ${comText.Token[1, ]}
	/call EchoLog "Global Command: ${strCommand} | ${comText}" TRUE
	/if (${paramCount}) {
		/for nArray 2 to ${paramCount}
			/varset cParams[${Math.Calc[${nArray}-1]}] ${comText.Token[${nArray}, ]}
		/if (${nArray} < 100) /next nArray
	}
	
	| ======================================================
	|  Check custom commands first
	| ======================================================
	/if (!${Defined[commandFrom]}) {
		/declare commandFrom							string outer ${comFrom}
		/declare commandText							string outer ${comText}
		/declare commandFromEQBC					string outer ${fromEQBC}
	} else {
		/varset commandFrom ${comFrom}
		/varset commandText ${comText}
		/varset commandFromEQBC ${fromEQBC}
	}
	/if (${commandCount}) {
		/for nArray 1 to ${commandCount}
			/if (${commandTriggers[${nArray},1].Equal[${strCommand}]} || ${commandTriggers[${nArray},1].Equal[${comText}]}) {
				| Custom command found for the trigger, execute it
				/call EchoLog "Custom command ${commandTriggers[${nArray},1]} executing ${commandTriggers[${nArray},2]} action(s)." TRUE
				/if (${commandTriggers[${nArray},2]}) {
					/for ctArray 1 to ${commandTriggers[${nArray},2]}
						/call GetINISetting ${iniName} Commands CommandTrigger${nArray}Action${ctArray} commandActions[${nArray},${ctArray}] NULL
						/call EchoLog "...Action[${ctArray}]:  ${commandActions[${nArray},${ctArray}]}" TRUE
						/docommand ${commandActions[${nArray},${ctArray}]}
					/next ctArray
				}
				/varset retValue Custom command ${commandTriggers[${nArray},1]} executed
			} else {
				/next nArray
			}
	}
	
	/if (${strCommand.Equal[watch]}) {
		| ======================================================
		| watch <name>
		| ======================================================
		/if (${cParams[1].NotEqual[NULL]}) {
			/declare watchAdd							bool local TRUE
			/if (${watchCount}) {
				/for nArray 1 to ${watchCount}
					/if (${watchList[${nArray}].Equal[${cParams[1]}]}) {
						/varset watchList[${nArray}] NULL
						/varset watchAdd FALSE
						/ini "${iniName}" "WatchList" "watchName${nArray}" "NULL"
					}
				/if (${watchAdd}) /next nArray
			} else {
				/varset watchAdd TRUE
			}
			/if (${watchAdd}) {
				/varcalc watchCount ${watchCount} + 1
				/varset watchList[${watchCount}] ${cParams[1]}
				/ini "${iniName}" "WatchList" "WatchName${watchCount}" "${cParams[1]}"
				/varset retValue \ag${cParams[1]} \aoadded to the watch List.
			} else {
				/varset retValue \ag${cParams[1]} \aoremoved fromt he watch List.
			}
		} else {
			/if (${watchEnabled}) {
				/varset watchEnabled FALSE
			} else {
				/varset watchEnabled TRUE
			}
			/varset retValue Watching people on the watch list: \ag${watchEnabled}
		}
	} else /if (${strCommand.Equal[loot]}) {
		| =====================================================
		| loot <state>
		| =====================================================
		/if (${cParams[1].Equal[ON]} || ${cParams[1].Equal[TRUE]}) {
			/varset lootCorpses TRUE
		} else /if (${cParams[1].Equal[OFF]} || ${cParams[1].Equal[FALSE]}) {
			/varset lootCorpse FALSE
		} else {
			/if (${lootCorpses}) {
				/varset lootCorpses FALSE
			} else {
				/varset lootCorpses TRUE
			}
		}
		/varset retValue Looting set to: ${lootCorpses}
	} else /if (${strCommand.Equal[safepc]}) {
		| =====================================================
		| safepc <name>
		| =====================================================
		/declare actAdd									bool local TRUE
		/if (${cParams[1].NotEqual[NULL]}) {
			/for nArray 1 to ${safePCCount}
				/if (${safePC[${nArray}].Equal[${cParams[1]}]}) /varset actAdd FALSE
			/if (${actAdd}) /next nArray
			/if (${actAdd}) {
				/call AddSafePC ${cParams[1]}
				/varset retValue Successfully added the pc \ag${cParams[1]} \aofrom the safe pc List.
			} else {
				/call RemoveSafePC ${cParams[1]}
				/varset retValue Successfully removed the pc \ag${cParams[1]} \aofrom the safe pc List.
			}
		} else {
			/varset retValue ABORT_SYNTAX
		}
	} else /if (${strCommand.Equal[ignoremob]}) {
		| =====================================================
		| ignoremob <name>
		| =====================================================
		/declare actAdd									bool local TRUE
		/declare tmpIgnoreString			string local NULL
		/for ctArray 1 to ${Math.Calc[${paramCount}	- 1]}
			/if (${cParams[${ctArray}].NotEqual[NULL]}) { 
				/if (${tmpIgnoreString.Length} && ${tmpIgnoreString.NotEqual[NULL]}) {
					/varset tmpIgnoreString ${tmpIngoreString} ${cParams[${ctArray}]}
				} else {
					/varset tmpIgnoreString ${cParams[${ctArray}]}
				}
			}
		/next ctArray		
		/if (${tmpIgnoreString.NotEqual[NULL]} && ${tmpIgnoreString.Length}) {
			/for nArray 1 to ${ignoreMobsCount}
				/if (${ignoreMobs[${nArray}].Equal[${tmpIgnoreString}]}) /varset actAdd FALSE
			/if (${actAdd}) /next nArray
			/if (${actAdd}) {
				/call AddIgnoreMob ${tmpIgnoreString}
				/varset retValue Successfully added the mob \ag${tmpIgnoreString} \aoto the ignore mob List.
			} else {
				/call RemoveIgnoreMob ${tmpIgnoreString}
				/varset retValue Successfully removed the mob \ag${cParams[1]} \aofrom the ignore mob List.
			}
		} else {
			/varset retValue ABORT_SYNTAX
		}		
	} else /if (${strCommand.Equal[docmd]}) {
		| =====================================================
		| docmd <command>
		| =====================================================
		/declare strDoCmd							string local
		/if (${cParams[1].NotEqual[NULL]}) {
			/for nArray 1 to ${paramCount}
				/varset strDoCmd ${strDoCmd} ${cParams[${nArray}]}
			/next nArray
			/if (${strDoCmd.Left[1].Equal[ ]}) /varset strDoCmd ${strDoCmd.Right[${Math.Calc[${strDoCmd.Length}-1]}]}
			/if (${strDoCmd.Left[1].NotEqual[/]}) /varset strDoCmd /${strDoCmd}
			/docommand ${strDoCmd}
			/varset retValue COMPLETED_SUCCESS
		} else {
			/varset retValue ABORT_SYNTAX
		}
	} else /if (${strCommand.Equal[follow]}) {
		| =====================================================
		| follow <target>
		| =====================================================		
		/declare folTarg							string local
		/declare fID									int local
		/if (${groupMode}) {
			/varset folTarg ${cParams[1]}
			/if (${cParams[1].Equal[Me]} || ${cParams[1].Equal[NULL]} || !${cParams[1].Length}) /varset folTarg ${comFrom}
			/varset fID ${Spawn[${folTarg}].ID}
			/if (${fID} && ${Spawn[${fID}].Type.Equal[Pet]}) /varset fID ${Spawn[${fID}].Master.ID}
			/if (${fID} && ${Spawn[${fID}].Type.NotEqual[Corpse]}) {
				/squelch /target clear
				/squelch /target ID ${fID}
				/delay 1s ${Target.ID}
				/if (${Target.ID} && ${Target.Distance}<150) {
					/if (${Me.Sitting}) /stand
					/squelch /stick hold 15
					/varset retValue Following \ag${folTarg}
					/varset followMode TRUE
					/varset followTarg ${folTarg}
					/varset followTargID ${fID}
					/varset curLoc 1
				} else /if (!${Target.ID}) {
					/varset retValue \aoI cannot follow \ag${folTarget} \aobecause I do not see them around
				} else {
					/varset retValue \aoI cannot follow \ag${folTarget} \aobecause they are too far away
					/squelch /target clear
				}
			} else {
				/varset retValue \aoI cannot follow \ag${folTarget} \aobecause I do not see them around
			}
		} else {
			/varset retValue Cannot use follow command when groupmode is FALSE.
		}
	} else /if (${strCommand.Equal[Assist]}) {
		| =====================================================
		| assist <target>
		| =====================================================			
		/declare assistTarg						string local
		/if (${cParams[1].NotEqual[NULL]}) {
			/varset assistTarg ${cParams[1]}
			/if (${cParams[1].Equal[Me]}) /varset assistTarg ${comFrom}
			
			/if (${assistTarg.Equal[Group]}) {
				/varset mainAssist ${Group.MainAssist}
				/varset useGroupAssist TRUE
			} else {
				/varset mainAssist ${assistTarg}
				/varset useGroupAssist FALSE
			}
			/varset retValue Assist has been changed to \ag${mainAssist}
		} else {
			/varset retValue ABORT_SYNTAX
		}
	} else /if (${strCommand.Equal[puller]}) {
		| =====================================================
		| puller <target>
		| =====================================================			
		/declare pullerTarg						string local
		/if (${cParams[1].NotEqual[NULL]}) {
			/varset pullerTarg ${cParams[1]}
			/if (${cParams[1].Equal[Me]}) /varset pullerTarg ${comFrom}
			
			/if (${pullerTarg.Equal[Group]}) {
				/varset pullerName ${Group.Puller}
				/varset useGroupPuller TRUE
			} else {
				/varset pullerName ${pullerTarg}
				/varset useGroupPuller FALSE
			}
			/varset retValue Puller has been changed to \ag${pullerName}
		} else {
			/varset retValue ABORT_SYNTAX
		}
	} else /if (${strCommand.Equal[maintank]} || ${strCommand.Equal[tank]}) {
		| =====================================================
		| maintank / tank <target>
		| =====================================================			
		/declare tankTarg						string local
		/if (${cParams[1].NotEqual[NULL]}) {
			/varset tankTarg ${cParams[1]}
			/if (${cParams[1].Equal[Me]}) /varset tankTarg ${comFrom}
			
			/if (${assistTarg.Equal[Group]}) {
				/varset mainTank ${Group.MainTank}
				/varset useGroupTank TRUE
			} else {
				/varset mainTank ${tankTarg}
				/varset useGroupTank FALSE
			}
			/varset retValue MainTank has been changed to \ag${mainTank}
		} else {
			/varset retValue ABORT_SYNTAX
		}
	} else /if (${strCommand.Equal[stay]}) {
		| =====================================================
		| stay
		| =====================================================
		/if (${groupMode}) {
			/squelch /stick off
			/call SetHome
			/varset LocArray[1] ${Me.Y},${Me.X},${Me.Z}
			/varset followMode FALSE
			/varset retValue Home location reset where I am, follow is now off
		} else {
			/varset retValue Cannot use stay command while groupmode is FALSE
		}
	} else /if (${strCommand.Equal[groupbuff]}) {
		| =====================================================
		| groupbuff
		| =====================================================		
		/if (${groupMode}) {
			/if (${doGroupBuffs}) {
				/varset doGroupBuffs FALSE
			} else {
				/varset doGroupBuffs TRUE
				| /varset minBuffCheckTimer 0s
			}
			/varset retValue Group Buffing: ${doGroupBuffs}
		}
	} else /if (${strCommand.Equal[selfbuff]}) {
		| =====================================================
		| selfbuff
		| =====================================================		
		/if (${doSelfBuffs}) {
			/varset doSelfBuffs FALSE
		} else {
			/varset doSelfBuffs TRUE
			| /varset minBuffCheckTimer 0s
		}
		/varset retValue Self Buffing: ${doSelfBuffs}
	} else /if (${strCommand.Equal[PetBuff]}) {
		| =====================================================
		| petbuff
		| =====================================================				
		/if (${doPetBuffs}) {
			/varset doPetBuffs FALSE
		} else {
			/varset doPetBuffs TRUE
			| /varset minBuffCheckTimer 0s
		}
		/varset retValue Pet Buffing: ${doPetBuffs}
	} else /if (${strCommand.Equal[watchbuff]}) {
		| =====================================================
		| watchbuff
		| =====================================================				
		/varset doWatchBuffs ${If[${doWatchBuffs},FALSE,TRUE]}
		/varset retValue Watch Buffing: ${doWatchBuffs}
	} else /if (${strCommand.Equal[debuff]}) {
		| =====================================================
		| debuff [alias]
		| =====================================================				
		/if (${cParams[1].Equal[NULL]}) {
			/if (${doDebuffs}) {
				/varset doDebuffs FALSE
			} else {
				/varset doDebuffs TRUE
			}
			/varset retValue Debuffing: ${doDebuffs}
		} else {
			/declare eArray						int local
			/for eArray 1 to ${debuffCount}
				/if (${debuffAlias[${eArray}].Equal[${cParams[1]}]}) {
					/if (${debuffEnabled[${eArray}]}) {
						/varset debuffEnabled[${eArray}] FALSE
					} else {
						/varset debuffEnabled[${eArray}] TRUE
					}
					/varset retValue Debuff ${debuffSpells[${eArray}]} - Enabled: ${debuffEnabled[${eArray}]}
				}
			/next eArray
		}
	} else /if (${strCommand.Equal[Buff]}) {
		| =====================================================
		| buff <alias> <target>
		| =====================================================
		/if (${groupBuffCount}) {
			/if (${cParams[1].NotEqual[NULL]} && ${cParams[2].NotEqual[NULL]}) {
				/declare buffTarg 					string local ${cParams[2]}
				/declare buffFound					bool local FALSE
				/if (${buffTarg.Equal[Me]}) /varset buffTarg ${comFrom}
				/if (${Spawn[${buffTarg}].ID}) {
					/if (${cParams[1].NotEqual[all]}) {
						/for nArray 1 to ${groupBuffCount}
							/if (${groupBuffAlias[${nArray}].Equal[${cParams[1]}]}) {
								/if (${groupBuffSingle[${nArray}].NotEqual[NULL]}) {
									/varset buffFound TRUE
									/if (${Me.CurrentMana} > ${Math.Calc[${Spell[${groupBuffSingle[${nArray}]}].Mana} + 100]}) {
										/call CastBuff ${Spawn[${buffTarg}].ID} ${nArray} Group Single TRUE
									} else {
										/varset retValue Insufficient mana to do that Buff
									}
								} else /if (${groupBuffGroup[${nArray}].NotEqual[NULL]}) {
									/varset buffFound TRUE
									/if (${Me.CurrentMana} > ${Math.Calc[${Spell[${groupBuffGroup[${nArray}]}].Mana} + 100]}) {
										/call CastBuff ${Spawn[${buffTarg}].ID} ${nArray} Group Group TRUE
									} else {
										/varset retValue Insufficient mana to do that Buff
									}									
								} else {
									/varset retValue The Buff with that alias could not be found									
								}
								/if (${retValue.Equal[COMPLETED_NOTFOUND]} && ${Macro.Return.Equal[COMPLETED_SUCCESS]}) {
									/varset retValue COMPLETED_SUCCESS
								} else {
									/call EchoLog "Attempt to buff failed: ${Macro.Return}" TRUE
									/varset retValue COMLETED_FAIL
								}
							}
						/next nArray
						/if (!${buffFound}) /varset retValue The Buff with that alias could not be found
					} else {
						/varset buffFound TRUE
						/varset retValue BUFFING
						/for nArray 1 to ${groupBuffCount}
							/if (${groupBuffSingle[${nArray}].NotEqual[NULL]}) {
								/if (${Me.CurrentMana} > ${Math.Calc[${Spell[${groupBuffSingle[${nArray}]}].Mana} + 100]}) {
									/call CastBuff ${Spawn[${buffTarg}].ID} ${nArray} Group Single TRUE
								} else {
									/varset retValue Insufficient mana to complete buffs
								}
							} else /if (${groupBuffGroup[${nArray}].NotEqual[NULL]}) {
								/if (${Me.CurrentMana} > ${Math.Calc[${Spell[${groupBuffGroup[${nArray}]}].Mana} + 100]}) {
									/call CastBuff ${Spawn[${buffTarg}].ID} ${nArray} Group Group TRUE
								} else {
									/varset retValue Insufficient mana to complete buffs
								}								
							}
						/if (${retValue.Equal[BUFFING]}) /next nArray
						/if (${retValue.Equal[BUFFING]}) /varset retValue COMPLETED_SUCCESS
					}
				} else {
					/varset retValue The Target, ${buffTarg}, could not be found to Buff
				}
			} else {
				/if (${cParams[1].NotEqual[NULL]}) {
					/if (${groupBuffCount}) {
						/for nArray 1 to ${groupBuffCount}
							/if (${groupBuffAlias[${nArray}].Equal[${cParams[1]}]}) {
								/if (${groupBuffEanbled[${nArray}]}) {
									/varset groupBuffEnabled[${nArray}] FALSE
								} else {
									/varset groupBuffEnabled[${nArray}] TRUE
								}
							}
						/next nArray
					}
				} else {
					/varset retValue ABORT_SYNTAX
				}
			}
		} else {
			/varset retValue There are no Group buffs loaded.
		}
	} else /if (${strCommand.Equal[invite]}) {
		| =====================================================
		| invite <target>
		| =====================================================		
		/declare invTarget			string local
		/if (${cParams[1].Equal[NULL]} || ${cParams[1].Equal[Me]} || !${cParams[1].Length}) {
			/varset invTarget ${rFrom}
		} else {
			/varset invTarget ${cParams[1]}
		}
		/if (${groupMode}) {
			/if ((${Group.GroupSize} > 1 && ${Group.Leader.CleanName.Equal[${Me.CleanName}]}) || !${Group.GroupSize}) {
				/invite ${invTarget}
				/varset retValue \ag${invTarget} has been Invited.
			} else {
				/varset retValue Cannot invite because I am not the Group Leader
			}
		} else {
			/varset retValue Cannot invite people when I'm not Running in Group mode'
		}
	} else /if (${strCommand.Equal[buffcheck]}) {
		| =====================================================
		| buffcheck
		| =====================================================	
		/call buffCheck FALSE ${inCombat} ${inCombatBuffRoutine}
		/varset retValue COMPLETED_SUCCESS
	} else /if (${strCommand.Equal[combatspells]}) {
		| =====================================================
		| combatspells
		| =====================================================	
		/if (${doCombatSpells}) {
			/varset doCombatSpells FALSE
		} else {
			/varset doCombatSpells TRUE
		}
		/varset retValue Combat spells Enabled; ${doCombatSpells}
	} else /if (${strCommand.Equal[EndTimer]}) {
		| =====================================================
		| EndTimer [time/disable] [action] 
		| (action can be gate, gatecamp, camp, stop, or quit)
		| =====================================================	
		/if (${cParams[1].NotEqual[NULL]}) {
			/if (${cParams[1].NotEqual[disable]}) {
				/varset useMacroEndTimer TRUE
				/varset macroEndTimer ${cParams[1]}
				/if (${cParams[2].NotEqual[NULL]} && ${cParams[2].Length}) /varset macroEndAction ${cParams[2]}
				/varset retValue End timer has been Enabled with a Time of ${cParams[1]}, Action is set to ${macroEndAction}
				/call EchoLog "Macro end timer has been Enabled with a Time of ${cParams[1]}, Action is set to ${macroEndAction}"
			} else {
				/if (${useMacroEndTimer}) {
					/varset useMacroEndTimer FALSE
					/varset macroEndTimer 0s
					/varset retValue End timer has been disabled
					/call EchoLog "Macro end timer has been disabled"
				}
			}
		} else {
			/if (${useMacroEndTimer}) {
				/declare tempMinutes			int local
				/declare tempSeconds			int local
				/varcalc tempSeconds ${macroEndTimer} / 10
				/varcalc tempMinutes ${tempSeconds} / 60
				/varcalc tempSeconds ${tempSeconds} - (${tempMinutes} * 60)
				/varset retValue End timer, Time remaining approixmately: ${tempMinutes} Minute(s) ${tempSeconds} Second(s), Action: ${macroEndAction}
			} else {
				/varset retValue End timer is not current Enabled.
			}
		}
	} else /if (${strCommand.Equal[combatbuff]}) {
		| =====================================================
		| combatbuff
		|  Enables / Disables combat buffing
		| =====================================================		
		/if (${doCombatBuffs}) {
			/varset doCombatBuffs FALSE
		} else {
			/varset doCombatBuffs TRUE
		}
		/varset retValue Combat Buffing: ${doCombatBuffs}
	} else /if (${strCommand.Equal[campradius]}) {
		| =====================================================
		| CampRadius [Value]
		|  Changes the camp radius
		| =====================================================			
		/if (${cParams[1].NotEqual[NULL]} && ${cParams[1].Length}) {
			/varset campRadius ${cParams[1]}
			/varset retValue CampRadius has been set to ${cParams[1]}
		} else {
			/varset retValue You must specify a camp radius: CampRadius [Value]
		}
	} else /if (${strCommand.Equal[campreturn]}) {
		| =====================================================
		| campreturn
		|  Forces bots to move back to their camp location
		| =====================================================		
		/call MoveToLoc ${homeY} ${homeX} ${inCombat}
		/varset retValue Move to home result: ${Macro.Return}
	} else /if (${strCommand.Equal[cast]}) {
		| =====================================================
		| cast [target] [spell]
		|  Will have the bot cast the specified spell on the target
		| =====================================================
		/declare spTarget						int local 0
		/declare spName							string local
		/declare spInt							int local 0
		/if (${cParams[1].NotEqual[NULL]} && ${cParams[1].Length}) {
			/if (${cParams[1].Equal[me]}) /varset cParams[1] ${rFrom}
			/if (${Spawn[${cParams[1]}].ID}) {
				/varset spTarget ${Spawn[${cParams[1]}].ID}
				/for spInt 2 to 20
					/if (${cParams[${spInt}].NotEqual[NULL]} && ${cParams[${spInt}].Length}) {
						/varset spName ${spName} ${cParams[${spInt}]}
					}
				/next spInt
				/varset spName ${spName.Right[-1]}
				/if (${spName.NotEqual[NULL]} && ${spName.Length}) {
					/squelch /target ID ${spTarget}
					/delay 1s ${Target.ID}==${spTarget}
					/if (${Target.ID}==${spTarget}) {
						/call MQ2Cast "${spName}"
						/varset retValue Cast Result: ${CastResult}
					} else {
						/varset retValue Could not Target ${Spawn[${spTarget}].CleanName})
					}
				} else {
					/varset retValue No Spell Name provided.
				}
			} else {
				/varset retValue I do not see that Target around here
			}
		}
	} else /if (${strCommand.Equal[castwhileinvis]}) {
		| =====================================================
		| castwhileinvis
		|  toggles casting while invis true / false
		| =====================================================
		/if (${castWhileInvis}) {
			/varset castWhileInvis FALSE
			/varset noInvis TRUE
		} else {
			/varset castWhileInvis TRUE
			/varset noInvis FALSE
		}
		/varset retValue Casting While Invis: ${castWhileInvis}
	} else /if (${strCommand.Equal[pull]}) {
		| ==========================
		| Pull, Toggles pulling on and off
		| ==========================
		/if ((${cParams[1].Equal[TRUE]} || ${cParams[1].Equal[ON]}) && ${allowPullMode}) {
			/varset pullMode TRUE
		} else /if (${cParams[1].Equal[FALSE]} || ${cParams[1].Equal[OFF]}) {
			/varset pullMode FALSE
		} else /if (${cParams[1].NotEqual[NULL]}) {
			/call PullLoadPath ${cParams[1]} FALSE
			/varset pullPath ${cParams[1]}
			/varset multiPaths FALSE
			/varset usePullPath TRUE
			/varset pullMode TRUE
		} else {
			/if (${pullMode} || !${allowPullMode}) {
				/varset pullMode FALSE
			} else {
				/varset pullMode TRUE
			}
		}
		/varset retValue ${If[!${allowPullMode},Pull mode is not allowed currently,PullMode set to: \ag${pullMode}]}
	} else /if (${strCommand.Equal[pullradius]}) {
		| ==========================
		| PullRadius <radius>
		| ==========================		
		/if (${cParams[1].NotEqual[NULL]}) {
			/varset pullRadius ${cParams[1]}
			/varset retValue Pulling Radius set to: \ag${pullRadius}
		} else {
			/varset retValue You must specify a new radius, CMD: \agPullRadius <Radius>
		}
 	} else /if (${strCommand.Equal[pulllimit]}) {
		| ==========================
		| PullLimit <action> <param>
		| ==========================		
		/if (${cParams[1].Equal[CLEAR]}) {
			/squelch /alert clear ${pullTargetAlert}
		} else /if (${cParams[1].Equal[ADD]}) {
			/declare plLoop	int local
			/declare plParam	string local NULL
			/for plLoop 2 to ${cParams.Size[1]}
				/if (${cParams[${plLoop}].NotEqual[NULL]}) /varset plParam ${plParam}${If[${plParam.Length}, ,]}${cParams[${plLoop}]}
			/next plLoop
			/squelch /alert add ${pullTargetAlert} ${plParam}
		}
  } else /if (${strCommand.Equal[MissionMode]}) {
		| ==========================
		| MissionMode <MissionINI>
		|  starts / stops mission mode, optional MissionINI sets the mission INI
		| ==========================
		/if ((!${missionMode} || ${cParams[1].NotEqual[NULL]}) && ${cParams[1].NotEqual[OFF]}) {
			/if (${cParams[1].NotEqual[NULL]}) {
				/varset missionINI ${cParams[1]}
				/if (!${missionINI.Find[.ini]}) /varset missionINI ${missionINI}.ini
			}
			/if (${missionINI.NotEqual[NULL]}) {
				/varset missionMode TRUE
				/varset retValue MissionMode started
			} else {
				/varset retValue You tried to enable mission mode but there is no MissionINI specicfied.  Specify it with \agMissionMode MissionINI
			}
		} else {
			/varset missionMode FALSE
			/varset missionStatus NULL
			/varset retValue MissionMode disabled.  If you are currently in a mission you will have to manually exit the mission.
		}
	} else /if (${strCommand.Equal[warp]}) {
		| =====================================================
		| warp [loc y x z] [t|target] [wp wpname] [me]
		|  Will have the bot warp using the specified parameters
		| =====================================================	
		/if (${cParams[1].NotEqual[NULL]}) {
			/if (${cParams[1].Equal[loc]}) {
				/if (${cParams[2].NotEqual[NULL]} && ${cParams[3].NotEqual[NULL]} && ${cParams[4].NotEqual[NULL]}) {
					/warp loc ${cParams[2]} ${cParams[3]} ${cParams[4]}
				} else {
					/varset retValue You must specify a y x z Value when using warp loc
				}
			} else /if (${cParams[1].Equal[t]} || ${cParams[1].Equal[Target]}) {
				/warp t
			} else /if (${cParams[1].Equal[wp]}) {
				/if (${cParams[2].NotEqual[NULL]}) {
					/warp wp ${cParams[2]}
				} else {
					/varset retValue You must specify a waypoint Name when using warp wp
				}
			} else /if (${cParams[1].Equal[Me]}) {
				/squelch /target ${comFrom}
				/delay 1s ${Target.CleanName.Equal[${comFrom}]}
				/if (${Target.CleanName.Equal[${comFrom}]}) {
					/warp t
				} else {
					/varset retValue Could not Find you in the current Zone to warp to.
				}
			} else {
				/varset retValue The warp parameter ${cParams[1]} was not understood.  Valid parameters are /ag[loc y x z] [t|target] [wp wpname] [me]
			}
			/if (${retValue.Equal[COMPLETED_NOTFOUND]}) {
					/delay 5
					/varset retValue Warp complete and home location reset to current location
					/call SetHome
					/if (${followMode}) {
						/varset followMode FALSE
						/squelch /stick off
					}				
			}
		} else {
			/varset retValue You did not pass a parameter for the Type of warp.  /ayUSAGE: 	/agwarp [loc y x z] [t|target] [wp wpname] [me]
		}
	}
/return ${retValue}

| Does this need to be updated for HoT inventory changes?
Sub ItemSwapCheck(string itemName)
	/declare needToSwap		bool local FALSE
	/declare slotsReq		int local
	/if (!${Defined[itemName]} && !${itemSwapped}) {
		/call EchoLog "No item specified to check for, aborting swap check." TRUE
		/return
	}

	/if (!${Plugin[MQ2Exchange].Version} && ${FindItem[${itemName}].EffectType.Equal[Click Worn]}) {
		/call EchoLog "MQ2Exchange is unavailable, cannot cast spells that are required to be worn to cast and are not currently worn."
		/return
	}
	
	/if (!${itemSwapped}) {
		/if (${FindItem[${itemName}].EffectType.Equal[Click Worn]}) {
			/if (${FindItem[${itemName}].WornSlot[1]} == 13 || ${FindItem[${itemName}].WornSlot[1]} == 14) {
				/if (${FindItem[${itemName}].Type.Find[2h]}) {
					/if (${FindItem[${itemName}].InvSlot} != 13 && !${InvSlot[13].Item.Type.Find[2h]}) /varset needToSwap TRUE
				} else /if (${FindItem[${itemName}].WornSlot[1]} == 14 && ${FindItem[${itemName}].InvSlot} != 14 && ${InvSlot[13].Item.Type.Find[2h]}) {
					/varset needToSwap TRUE
				}
			}
		}
		/if (${needToSwap}) {
			/varset slotsReq 2
			/if (${InvSlot[13].Item.Type.Find[2h]}) /varset slotsReq 1
			/if (${Me.FreeInventory[4]} >= ${slotsReq}) {
				/varset itemPrimary ${InvSlot[13].Item}
				/varset itemSecondary ${InvSlot[14].Item}
				/squelch /unequip mainhand
				/delay 2
				/squelch /unequip offhand
				/delay 2
				/if ((${FindItem[${itemPrimary}].Name.NotEqual[null]} && ${FindItem[${itemPrimary}].InvSlot}<23) || (${FindItem[${itemSecondary}].Name.NotEqual[null]} && ${FindItem[${itemSecondary}].InvSlot}<23)) {
					/call EchoLog "Failed to unequip items, item swap aborted." TRUE
					/return
				}
				/if (${FindItem[${itemName}].WornSlot[1]}==13) {
					/exchange "${itemName}" mainhand
				} else {
					/exchange "${itemName}" offhand
				}
				/varset itemSwapped TRUE
			} else {
				/call EchoLog "You must have at least ${slotsReq} empty giant sized inventory slots to do this item swap for safety of not loosing items." TRUE
			}
		}
	} else {
		/squelch /unequip mainhand
		/delay 2
		/squelch /unequip offhand
		/delay 2
		/if (${itemPrimary.NotEqual[null]}) /exchange "${itemPrimary}" mainhand
		/delay 2
		/if (${itemSecondary.NotEqual[null]}) /exchange "${itemSecondary}" offhand
		/delay 2
		/varset itemPrimary null
		/varset itemSecondary null
		/varset itemSwapped FALSE
	}
/return

Sub pauseActions
	:pauseLoop
		/call campCheck TRUE
	/if (${Macro.Return.Equal[FALSE]}) /goto :pauseLoop
/return

Sub SafePC(string pcName)
	/declare isSafe 	bool local FALSE
	/declare inum			int local
	/if (!${Defined[pcName]}) {
		/call EchoLog "Pc name not specified for safe pc check, aborting and returning as unsafe" TRUE
		/return FALSE
	}
	
	/if (${Group.Members}) {
		/for inum 1 to ${Group.Members}
			/if (${pcName.Equal[${Group.Member[${inum}].DisplayName}]}) /varset isSafe TRUE
		/next inum
	}
	/if (${safePCCount}) { 
		/for inum 1 to ${safePCCount}
			/if (${pcName.Equal[${safePC[${inum}]}]}) /varset isSafe TRUE
		/next inum
	}
	/if (${missionLeader.Equal[${pcName}]}) /varset isSafe TRUE
	
	/if (!${isSafe} && ${pcName.Equal[${Me.CleanName}]}) /varset isSafe TRUE
/return ${isSafe}

Sub SendTell(string tTo, string tMsg)
	/if (${tMsg.Length}) {
		/if (${tTo.Equal[MQ2]}) { 
			/call EchoLog "${tMsg}"
		} else /if (${tTo.Left[6].Equal[(EQBC)]}) {
			/call StripText "${tMsg}"
			/varset tMsg ${Macro.Return}
			/bct ${tTo.Right[-6]} ${tMsg}
			/call EchoLog "BC Tell sent to ${tTo} saying ${tMsg}" TRUE
		} else {
			/call StripText "${tMsg}"
			/varset tMsg ${Macro.Return}
			/if (${replyToTells}) {	
				/tell ${tTo} ${tMsg}
				/call EchoLog "Tell sent to ${tTo} saying ${tMsg}" TRUE
			} else {
				/call EchoLog "Reply to tells set to FALSE, not sending: ${tTo}  The message: ${tMsg}" TRUE
			}
		}
	}
/return

Sub StripText(string cString)
	/declare argNum			int local
	/declare wString		string local
	/declare argString	string local
	
	/if (!${Defined[cString]}) /return
	/if (!${cString.Find[\]}) /return ${cString}
	/varset wString ${cString}
	
	:nextArg
		/varset argNum ${cString.Find[\a]}
		/varset argString ${cString.Mid[${argNum},3]}
		/if (${argString.Left[1].Equal[\]} && ${argString.Length} == 3) {
			/if (${argString.Right[1].Equal[#]}) /varset argString ${wString.Mid[${argNum},9]}
			/if (${cString.Find[${argString}]}) /varset cString ${cString.Replace[${argString},]}
		}
		/if (${cString.Find[\a]}) /goto :nextArg
/return ${cString}

Sub SummonItem(string spellName, string spellSlot, string targetName, string itemName, bool putInInv, bool unfold)
	/if (!${Defined[itemName]}) /declare itemName string local NULL
	/if (!${Defined[putInInv]}) /declare putInInv bool local TRUE
	/if (!${Defined[unfold]}) /declare unfold bool local TRUE
	/declare pLoop		int local
	/declare pFound		bool local FALSE
	/declare emptyPrimary		int local 0
	/declare summonedID			int local 0
	/declare maxCursorWait	timer local 0s
	
	/varset summonedPackNum 0
	/varset summonedUnfoldedName NULL
	/if (${Cursor.ID}) {
		/autoinv
		/delay 2s !${Cursor.ID}
		/if (${Cursor.ID}) {
			/call EchoLog "There is something on our cursor and we can't put it into inventory.  Unable to summon with an item on the cursor"
			/return
		}
	}
	/if (${targetName.Equal[NULL]}) {
		/call EchoLog "Target name is somehow NULL, Aborting summmoning of ${spellName}" TRUE
		/return
	}
	
	/if (!${Target.ID} || ${Target.CleanName.NotEqual[${targetName}]}) {
		/squelch /target clear
		/delay 1s !${Target.ID}
		/squelch /target ${targetName}
		/delay 2s ${Target.CleanName.Equal[${targetName}]}
		/if (${Target.CleanName.NotEqual[${targetName}]}) {
			/call EchoLog "Unable to target summoning target.  Aborting summon." TRUE
			/return
		}
	}
	/call EchoLog "Attempting to summon ${If[${itemName.NotEqual[NULL]},${itemName} , ]}with spell: ${spellName} to target: ${Target.CleanName}" TRUE
	/if (${spellSlot.Equal[Item]}) /call ItemSwapCheck "${spellName}"
	/if (${spellSlot.Equal[disc]}) {
		 /varset discTimer ${Spell[${spellName}].Duration.TotalSeconds}s
		/doability "${spellName}"
		:discSummonCast
			/call CheckForAggro
			/if (${Window[CombatAbilityWnd].Open}) {
				/if (${Window[CombatAbilityWnd].Child[CAW_CombatEffectLabel].Text.NotEqual[No Effect]}) {
					/if (!${discTimer}) /varset discTimer ${Spell[${Window[CombatAbilityWnd].Child[CAW_CombatEffectLabel].Text}].Duration.TotalSeconds}s			
				} else {
					/varset discTimer 0s
				}
			}
		/if (${discTimer}) /goto :discSummonCast
	} else {
		/call MQ2Cast "${spellName}" ${spellSlot} 0 CheckForAggro -maxtries|${resistTries}
	}
	| /if (${itemSwapped}) /call ItemSwapCheck

	/if (${Target.CleanName.Equal[${Me.CleanName}]} || ${targetName.Equal[NULL]} || ${targetName.Equal[${Me.CleanName}]}) {
		/delay 1s ${Cursor.ID}

		/if (${Cursor.ID}) {
			/varset summonedID ${Cursor.ID}
			/if (!${Window[InventoryWindow].Open}) /windowstate InventoryWindow open
			/delay 5
			/for pLoop 1 to 10
				/if (!${Me.Inventory[pack${pLoop}].Container}) /varset emptyPrimary ${pLoop}
			/if (!${emptyPrimary}) /next pLoop
			/if (${Cursor.Name.Find[Folded]} && ${unfold} && ${emptyPrimary}) {
				/itemnotify pack${pLoop} leftmouseup
				/delay 1s ${Cursor.ID}!=${summonedID}
				/if (${Cursor.ID}) {
					/varset maxCursorWait 10s
					:summonItemFreeCursor
						/autoinv
						/delay 1s !${Cursor.ID}
					/if (${Cursor.ID} && ${maxCursorWait}) /goto :summonItemFreeCursor
					/if (${Cursor.ID}) {
						/call EchoLog ".....Unable to get item off the cursor, unable to fully complete summoning." TRUE
						/return
					}
				}
				/if (${Me.Inventory[pack${emptyPrimary}].Name.Find[Folded]}) {
					/if (${Me.Casting.ID}) /delay 10s !${Me.Casting.ID}
					/itemnotify pack${emptyPrimary} rightmouseup
					/delay 5
					/delay 5s !${Me.Casting.ID}
					/delay 1s ${Cursor.ID}
					/if (${Cursor.ID}) {
						/varset summonedUnfoldedName ${Cursor.Name}
					} else {
						/varset summonedUnfoldedName NULL
						/call EchoLog ".....Unfolded an item, but unable to determine it's name." TRUE
					}
				}
			} else /if (${Cursor.Name.Find[Folded]} && ${unfold} && !${emptyPrimary}) {
				/call EchoLog "You have summoned a folded bag but do not have an empty primary slot to unfold it into, summoning not fully completed"
				/autoinv
				/delay 2s !${Cursor.ID}
				/return
			}
			/if (${Cursor.Container} && ${emptyPrimary}) {
				/varset summonedPackNum ${emptyPrimary}
				/itemnotify pack${emptyPrimary} leftmouseup
				/delay 1s !${Cursor.ID}
				/if (${Cursor.ID}) {
					/varset maxCursorWait 10s
					:summonItemFreeCursor
						/autoinv
						/delay 1s !${Cursor.ID}
					/if (${Cursor.ID} && ${maxCursorWait}) /goto :summonItemFreeCursor
					/if (${Cursor.ID}) {
						/call EchoLog ".....Unable to get item off the cursor, unable to fully complete summoning." TRUE
						/return
					}
				}
				/if (!${Cursor.ID}) /call EchoLog ".....Container summoned and placed in pack${emptyPrimary}" TRUE

				/if (!${emptyPrimary}) { 
					/if (${Cursor.Name.Equal[${itemName}]} || ${Cursor.Name.Equal[Folded ${itemName}]}) {
						/call EchoLog ".....Could not find a place to put the summoned container, destroying it" TRUE
						/dest
					} else {
						/autoinv
					}
				}
			} else {
				/if (${Cursor.Container} && !${emptyPrimary}) {
					/call EchoLog "You have summoned a bag but have no free primary slots, summoning not fully completed"
				} else {
					/if (${Cursor.ID} && ${putInInv}) /autoinv	
					/call EchoLog ".....Item successfully summoned" TRUE
				}
			}
		}
	}
	/if (${itemSwapped}) /call ItemSwapCheck
	/if (${Window[InventoryWindow].Open}) /windowstate InventoryWindow close
/return

Sub SummonedItemsCheck
	/declare sLoop		int local
	/declare pFree		bool local false
	/declare gFree		bool local false
	/declare maxCleanTime	timer local 0s
	
	/if (${myName.Equal[magebot2]} && ${modRodSpell.NotEqual[NULL]}) {
		/if (!${FindItem[${modRodName}].ID} && (${Me.CurrentMana} > ${Math.Calc[${Spell[${modRodSpell}].Mana}+200]} || !${modRodSlot.Find[Gem]})) {
			/call SummonItem "${modRodSpell}" ${modRodSlot} ${Me.CleanName} "${modRodName}" TRUE
		}
	}
	/if (${summonedCount}) {
		|**/for sLoop 1 to 10
			/if (!${Me.Inventory[pack${sLoop}].Container}) /varset pFree true
		/next sLoop**|

		/if (${Cursor.ID}) {
			/varset maxCleanTime 10s
			:summonCleanCursor
				/autoinv
				/delay 2s !${Cursor.ID}
			/if (${Cursor.ID} && ${maxCleanTime}) /goto :summonCleanCursor
			/if (${Cursor.ID} && !${maxCleanTimer}) {
				/call EchoLog "Unable to clean the cursor to summon items, aborting"
				/return
			}
		}
		/if (${Me.FreeInventory[4]}) /varset gFree true

		| /if (${pFree} && ${gFree}) {
		/if (${gFree}) {
			/for sLoop 1 to ${summonedCount}
				/if (!${FindItem[${summonedItems[${sLoop}]}].ID} && ((${Me.CurrentMana} > ${Math.Calc[${Spell[${summonedSpells[${sLoop}]}].Mana}+200]} && ${Me.Book[${summonedSpells[${sLoop}]}]} && ${summonedSlots[${sLoop}].Find[Gem]}) || (${FindItem[${summonedSpells[${sLoop}]}].ID} && ${summonedSlots[${sLoop}].Equal[Item]}) || (${Me.CombatAbilityReady[${summonedSpells[${sLoop}]}]} && ${summonedSlots[${sLoop}].Equal[DISC]}))) {
					:checkItem
						/call SummonItem "${summonedSpells[${sLoop}]}" ${summonedSlots[${sLoop}]} ${Me.CleanName} "${summonedItems[${sLoop}]}" TRUE
						/if (${gotAggro}) /return
					/if (!${FindItem[${summonedItems[${sLoop}]}].ID} || (${FindItem[${summonedItems[${sLoop}]}].Stackable} && ${FindItem[${summonedItems[${sLoop}]}].StackCount} < ${summonedMinCount[${sLoop}]})) /goto :checkItem
				}
			/next sLoop		
		} else {
			| /if (!${pFree}) /call EchoLog "You must have at least one open primary inventory slot to summon items" TRUE
			/if (!${gFree}) /call EchoLog "You must have at least one open giant sized bag slot to summon items" TRUE
		}
	}
/return

Sub Timer(string timerPrefix, int timerID, string setTo, bool delTimer)
	/if (${Defined[delTimer]} && ${delTimer}) {
		/if (${timerID} >= 0) {
			/if (!${Defined[${timerPrefix}Timer${timerID}]}) /deletevar ${timerPrefix}Timer${timerID}
		} else {
			/if (!${Defined[${timerPrefix}Timer]}) /deletevar ${timerPrefix}Timer
		}
	} else {
		/if (${Defined[setTo]}) {
			/if (${timerID} >= 0) {
				/if (!${Defined[${timerPrefix}Timer${timerID}]}) /declare ${timerPrefix}Timer${timerID} timer outer 0s
				/varset ${timerPrefix}Timer${timerID} ${setTo}
			} else {
				/if (!${Defined[${timerPrefix}Timer]}) /declare ${timerPrefix}Timer timer outer 0s
				/varset ${timerPrefix}Timer ${setTo}			
			}
		} else {
			/if (${Defined[timerID]} && ${timerID} >= 0 && (!${Defined[${timerPrefix}Timer${timerID}]} || !${${timerPrefix}Timer${timerID}})) {
				/return FALSE
			} else /if ((!${Defined[timerID]} || ${timerID} < 0) && (!${Defined[${timerPrefix}Timer]} || !${${timerPrefix}Timer})) {
				/return FALSE
			} else {
				/return TRUE
			}		
		}
	}
/return FALSE

Sub TrackKill(string killName)
	/declare tkLoop			int local 0
	/declare knFound		bool local FALSE
	/declare firstNull	int local 0
	
	/for tkLoop 1 to 200
		/if (${killTrack[${tkLoop},1].Equal[killName]}) {
			/if (${killTarck[${tkLoop},2].Equal[NULL]}) {
				/varset killtrack[${tkLoop},2] 1
			} else {
				/varcalc killTrack[${tkLoop},2] ${killTrack[${tkLoop},2]} + 1
			}
			/varset knFound TRUE
		}
		/if (${killTrack[${tkLoop},1].Equal[NULL]} && !${firstNull}) /varset firstNull ${tkLoop}
	/if (!${knFound} && ${killTrack[${tkLoop}].NotEqual[NULL]}) /next tkLoop
	/if (!${knFound}) {
		/varset killTrack[${firstNull},1] ${killName}
		/varset killTrack[${firstNull},2] 1
	}
/return

Sub TrackKillCount(string killName,bool allMatches)
	/declare tkLoop			int local 0
	/declare knFound		bool local FALSE
	/declare tkCount		int local 0
	/if (!${Defined[allMatches]}) /declare allMatches bool FALSE
	
	/for tkLoop 1 to 200
		/if ((!${allMatches} && ${killTrack[${tkLoop},1].Equal[${killName}]}) || (${allMatches} && ${killTrack[${tkLoop},1].Find[${killName}]})) {
			/varcalc tkCount ${tkCount} + ${killTrack[${tkLoop},2]}
			/varset knFound TRUE
		}
	/if ((!${knFound} || ${allMatches}) && ${killTrack[${tkLoop}].NotEqual[NULL]}) /next tkLoop
/return ${tkCount}

Sub TrackKillReset(string killName,bool allMatches)
	/declare tkLoop			int local 0
	/declare knFound		bool local FALSE
	/if (!${Defined[allMatches]}) /declare allMatches bool FALSE
	
	/for tkLoop 1 to 200
		/if ((!${allMatches} && ${killTrack[${tkLoop},1].Equal[${killName}]}) || (${allMatches} && ${killTrack[${tkLoop},1].Find[${killName}]})) {
			/varset killTrack[${tkLoop},2] 0
			/varset knFound TRUE
		}
	/if ((!${knFound} || ${allMatches}) && ${killTrack[${tkLoop}].NotEqual[NULL]}) /next tkLoop
/return

| Needs to be updated for HoT inventory changes
Sub TradeCheck
	/declare intContainer								int local 0
	/declare intLoop										int local 0
	/declare intILoop										int local 0
	/declare openSize[80]								int local
	/declare openTrack									int local 0
	/declare slotFound									bool local

	/if (${Window[TradeWnd].Open}) {
		/if (${Window[TradeWnd].HisTradeReady}) {
			/call EchoLog "Trade detected, checking if we can accept" TRUE
			/for intLoop 1 to 80
				/varset openSize[${intLoop}] -1
			/next intLoop
			/for intLoop 23 to 30
				/if (!${InvSlot[${intLoop}].Item.ID}) {
					/varcalc openTrack ${openTrack} + 1
					/varset openSize[${openTrack}] 5
				}
			/next intLoop
			/for intLoop 23 to 30
				/if (${InvSlot[${intLoop}].Item.ID} && ${InvSlot[${intLoop}].Item.Container}) {
					/varcalc intContainer ${InvSlot[${intLoop}].Item.Container} - ${InvSlot[${intLoop}].Item.Items}
					/if (${intContainer}) {
						/for intILoop 1 to ${intContainer}
							/varcalc openTrack ${openTrack} + 1
							/varset openSize[${openTrack}] ${InvSlot[${intLoop}].Item.SizeCapacity}
						/next intILoop
					}
				}
			/next intLoop
			/for intLoop 9 to 16
				/if (${InvSlot[trade${intLoop}].Item.ID}) {
					/if (${InvSlot[trade${intLoop}].Item.Container}) {
						/if (${openSize[${Math.Calc[${intLoop} - 8]}]} < 5) {
							/call EchoLog "Cannot complete trade because there aren't enough primary inventory slots open, or the items are in a bad order." TRUE
							/goto :cancelTrade							
						} else {
							/varset openSize[${Math.Calc[${intLoop} - 8]}] -1
						}
					} else {
						/varset slotFound FALSE
						/for intILoop 1 to 80
							/if (${openSize[${Math.Calc[${intLoop} - 8]}]} >= ${InvSlot[trade${intLoop}].Item.Size}) {
								/varset openSize[${Math.Calc[${intLoop} - 8]}] -1
								/varset slotFound TRUE
							}
						/if (!${slotFound}) /next intILoop
						/if (!${slotFound}) {
							/call EchoLog "Cannot complete trade because there aren't enough open inventory slots of size ${InvSlot[trade${intLoop}].Item.Size}, or the items are in a bad order" TRUE
							/goto :cancelTrade
						}
					}
				}
			/next intLoop
			/goto :acceptTrade
		} else {
			/return ABORT_NOTREADY
		}
	} else {
		/return ABORT_NOTRADE
	}
	
	:cancelTrade
		/notify TradeWnd TRDW_Cancel_Button leftmouseup
		/delay 2s !${Window[TradeWnd].Open}
		/if (${Window[TradeWnd].Open}) /goto :cancelTrade
		/return ABORT_CANCEL
	:acceptTrade
		/notify TradeWnd TRDW_Trade_Button leftmouseup
		/delay 2s !${Window[TradeWnd].Open}
		/if (${Window[TradeWnd].Open}) /goto :acceptTrade
/return COMPLETED_SUCCESS

Sub WaitForRez(string maxWaitTime)
	/declare deathZone			int local ${Zone.ID}
	/declare deathX					float local ${Me.X}
	/declare deathY					float local ${Me.Y}
	/declare deathZ					float local ${Me.Z}
	/declare dText					string local NULL
	/declare rezPercent			string local 0
		
	/if (${deathZone}!=${startZoneID}) {
		/varset deathZone ${startZoneID}
		/varset deathX ${homeX}
		/varset deathY ${homeY}
		/varset deathZ ${homeZ}
	}
	/declare waitTimer			bool local FALSE
	/if (${Defined[maxWaitTime]} && !${Select[${maxWaitTime},0,0s,0m,00,0h]}) {
		/declare maxWaitTimer timer local ${maxWaitTime}
		/varset waitTimer TRUE
	}
		
	/call EchoLog "\aoWe have died and are waiting for a rez"
	/varset waitingForRez TRUE
	/if ((!${Defined[rezSelf]} || !${rezSelf}) && ${Zone.ID}==${deathZone}) {
		:rezWait
			/if (${waitTimer} && !${maxWaitTimer}) {
				/call EchoLog "\arWe have waited the maximum amount of time for a rez and have not received one, ending macro and camping."
				/if (${Window[RespawnWnd].Open}) {
					/nomodkey /notify RespawnWnd RW_OptionsList listselect 1
					/delay 5
					/nomodkey /notify RespawnWnd RW_SelectButton leftmouseup 
					/delay 30s
				}
				/varset waitingForRez FALSE
				/camp desktop
				/endmacro
			}
			/doevents
			/if (${Me.State.NotEqual[HOVER]} && ${Zone.ID}==${deathZone} && ${Math.Distance[${deathY},${deathX},${deathZ}]} < ${Math.Calc[${campRadius}*2]}) {
				/call EchoLog "\agIt appears we have accepted the rez by other means, attempting to continue operations."
				/varset waitingForRez FALSE
				/return
			}
			/delay 10s ${Window[ConfirmationDialogBox].Open} 
		/if (!${Window[ConfirmationDialogBox].Open}) {
			/goto :rezWait
		} else {
			/varset dText ${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text}
			/if (${dText.Find[wants to cast]} && ${dText.Find[percent]}) {
				| Probably a rez box
				/varset rezPercent ${dText.Mid[${Math.Calc[${dText.Find[(]}+1]},${Math.Calc[${dText.Find[)]} - ${dText.Find[(]} - 1]}].Replace[ percent,]}
				/if (!${minRezPercent} || ${rezPercent} >= ${minRezPercent}) {
					/nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
				}
			}
		}
		/delay 5
		/if (${Window[RespawnWnd].Open}) {
			/nomodkey /notify RespawnWnd RW_OptionsList listselect 2  
			/delay 5
			/nomodkey /notify RespawnWnd RW_SelectButton leftmouseup 
		} else {
			:zoneWait
				/delay 10s ${Zone.ShortName.Equal[${startZone}]}
			/if (!${Zone.ShortName.Equal[${startZone}]}) /goto :zoneWait
		}
		/call EchoLog "\agWe have been resurrected, continuing opertaions"
	} else {
		/delay 30s
		/if (${Window[RespawnWnd].Open}) {
			/nomodkey /notify RespawnWnd RW_OptionsList listselect 1
			/delay 5
			/nomodkey /notify RespawnWnd RW_SelectButton leftmouseup 
		}
		/if (${Zone.ID}!=${startZoneID}) {
			/call SetHome
			/varset startZoneID ${Zone.ID}
			/varset startZone ${Zone.ShortName}
		}
	}
	/varset waitingForRez FALSE
/return

Sub Event_pullerChange(rLine,string pName)
	/if (${useGroupPuller}) {
		/varset pullerName ${Group.Puller}
	}
/return

Sub Event_tankChange(rLine,string pName)
	/if (${useGroupTank}) {
		/varset mainTank ${Group.MainTank}
	}
/return

Sub Event_assistChange(rLine,string pName)
	/if (${useGroupAssist}) {
		/varset mainAssist ${Group.MainAssist}
	}
/return

Sub Event_newmember(rLine, string rName)
	/if (${Spawn[${rName}].Distance} < ${campRadius} && ${Me.PctMana} > 50 && ${doGroupBuffs} && ${groupBuffCount}) /call buffCheck FALSE ${inCombat} ${inCombatBuffRoutine}
/return

Sub Event_rampage
	/if (${allowSit}) /call EchoLog "A mob we are fighting is rampaging on me, will not sit for the duration of this fight" TRUE
	/varset allowSit FALSE
	/varset rampageNoSit TRUE
/return

Sub Event_died(eLine)
	/if (!${Defined[eLine]}) /declare eLine string local Died by unknown source
	
	/varset inCombat FALSE
	/varset targCount 0
	/varset lTargCount 0
	/varset corpseCount 0
	/varset targID 0
	/attack off
	/squelch /target clear
	/if (${useNavigation} && ${Plugin[MQ2Naviagion].Version}) /nav stop
	/doevents flush died
	/call EchoLog "${eLine}" TRUE
	/squelch /exec "${alertDeath}" bg
	/if (${groupMode}) {
		/call WaitForRez ${maxRezWaitTime}
		/if (${Math.Distance[${Me.Y},${Me.X}:${homeY},${homeX}]}>${campRadius}) {
			/call SetHome
			/call EchoLog "Home location has been reset to the point of resurrection"
		} else {
			/call MoveToLoc ${homeY} ${homeX} FALSE
			/if (${moveCanceled} && !${Me.CombatState.Equal[COMBAT]}) {
				/call EchoLog "\ayCould not get back to home location after rez, resetting home location for where we are now"
				/call SetHome
			}
		}
	} else {
		/notify RespawnWnd RW_SelectButton leftmouseup		
	}
	/doevents flush died
	/call AfterDeath
/return

Sub Event_bcTell(rLine,string rFrom,string rMsg)
	/if (${rMsg.Left[2].NotEqual[//]} && ${eqbcCommands}) /call Event_rTell "${rLine}" "(EQBC)${rFrom}" "${rMsg}" BCTELL
/return

Sub Event_bcChatTell(rLine,string rFrom,strng rMsgTo,string rMsg)
	/if (${rMsgTo.Equal[${Me.CleanName}]} && ${eqbcCommands}) {
		/if (${rMsg.Left[2].NotEqual[//]}) /call Event_rTell "${rLine}" "(EQBC)${rFrom}" "${rMsg}" BCCTELL
	} 
/return

Sub Event_MQ2Cmd(rLine,string rMsg)
	/if (${rMsg.Left[2].NotEqual[>>]} && ${rMsg.Left[1].NotEqual[[]} && ${echoCommands}) /call Event_rTell "${rLine}" MQ2 "${rMsg}" MQ2
/return

Sub Event_mq2navFail
	/call EchoLog "\aySetting UseNavigation to \arFALSE \aybecause MQ2Navigation does not work in this zone."
	/varset useNavigation FALSE
/return

Sub Event_Chat(ChatChannel,ChatSender,ChatText)
	/declare taArray					int local 0
	/if (${textAlertsValue}) {
		/for taArray 1 to ${textAlertsValue}
			/if (${ChatText.Find[${textAlertsName[${taArray}]}]} && ${textAlertsAudio[${taArray}].NotEqual[NULL]}) {
				/squelch /exec "${textAlertsAudio[${taArray}]}" bg
				/goto :alertSent
			}
		/next taArray
		:alertSent
	}
	/if (${ChatSender.Left[1].Compare[ ]} < 0) /varset ChatSender ${ChatSender.Right[-2].Left[-1]}
	/if (${relayChat} && ${relayTarget.Left[1].Equal[/]}) /docommand ${relayTarget} >> [${ChatChannel}]  ${ChatSender}: '${ChatText}'
	/if (${ChatChannel.Equal[Group]} && ${groupCommands}) /call Event_rTell NULL ${ChatSender} "${ChatText}" Group
/return

Sub Event_rTell(rLine,string rFrom,string rMsg, string rType)
	/declare risSafe bool local
	/declare rnum int local
	/declare hCommand	string local
	/declare hDoes		string local
	/declare hMsg		string local
	/declare rFromID	int local 0
	/declare rFromEQBC	bool local FALSE
	/if (!${Defined[rType]}) /declare rType				string local TELL
	
	/if (${rFrom.Equal[${Me.Pet.CleanName}]}) /return
	/if (${rType.Equal[TELL]} && !${tellCommands}) /return
	/if (${rMsg.Find[Command not recognized]}) /return
	
	/if (${rFrom.NotEqual[MQ2]}) {
		/if (${rFrom.Left[6].Equal[(EQBC)]}) {
			/varset risSafe TRUE
			/varset rFromEQBC TRUE
			/varset rFrom ${rFrom.Right[-6]}
			/varset rFromID ${Spawn[pc ${rFrom}].ID}
		} else {
			/if (${rFrom.Left[1].Compare[ ]} < 0) /varset rFrom ${rFrom.Right[-2].Left[-1]}
			/if (${rType.Equal[TELL]}) {
				/call EchoLog "\au${Time} \aoReceived a tell from \ag${rFrom} \aosaying: \ay${rMsg}"
				/if (${relayTells} && ${relayTarget.Left[1].Equal[/]}) /docommand ${relayTarget} >> [Tell]  ${rFrom}: '${rMsg}'
			}
							
			/varset rFromID ${Spawn[pc ${rFrom}].ID}
			
			/call SafePC ${rFrom}
			/varset risSafe ${Macro.Return}
		}
	} else {
		/varset risSafe TRUE
	}

	/if (${risSafe}) {
		/if (${rMsg.Left[12].Equal[MISSIONCOMM ]} && ${missionMode}) {
			/call MissionCommandHandler ${rFrom} "${rMsg}" ${rFromEQBC}
			/if (${Macro.Return.NotEqual[COMPLETED_NOTFOUND]}) {
				/if (${Macro.Return.Equal[ABORT_SYNTAX]}) {
					/varset hMsg \ayYour command could not be completed due to improper syntax
				} else /if (${Macro.Return.Equal[COMPLETED_SUCCESS]}) {
					/varset hMsg \agYour command was completed successfully.
				} else {
					/varset hMsg \ag${Macro.Return}
				}
			} else {
				/varset hMsg \ayMission command not recognized
			}
		}	else {
			/call BotCommands ${rFrom} "${rMsg}" ${rFromEQBC}
			/if (${Macro.Return.NotEqual[COMPLETED_NOTFOUND]}) {
				/if (${Macro.Return.Equal[ABORT_SYNTAX]}) {
					/varset hMsg \ayYour command could not be completed due to improper syntax
				} else /if (${Macro.Return.Equal[COMPLETED_SUCCESS]}) {
					/varset hMsg \agYour command was completed successfully.
				} else {
					/varset hMsg \ag${Macro.Return}
				}
			} else {
				/call GlobalCommands ${rFrom} "${rMsg}" ${rFromEQBC}
				/if (${Macro.Return.NotEqual[COMPLETED_NOTFOUND]}) {
					/if (${Macro.Return.Equal[ABORT_SYNTAX]}) {
						/varset hMsg \ayYour command could not be completed due to improper syntax
					} else /if (${Macro.Return.Equal[COMPLETED_SUCCESS]}) {
						/varset hMsg \agYour command was completed successfully.
					} else {
						/varset hMsg \ag${Macro.Return}
					}
				} else {
					/varset hMsg \ayCommand not recognized
				}	
			}
		}
		/if (${hMsg.NotEqual[NULL]}) {
			/if (${rFromEQBC}) /varset rFrom (EQBC)${rFrom}
			/if (${rType.NotEqual[Group]}) /call SendTell ${rFrom} "${hMsg}"
		}
	}
/return

| Need to update for using mq2navigation and always attempting return
Sub Event_summoned
	/if (${Math.Distance[${Me.Y},${Me.X}:${homeY},${homeX}]}<= ${Math.Calc[${campRadius}*2]} && ${mainTank.NotEqual[${Me.CleanName}]}) {
		/call MoveToLoc ${homeY} ${homeX} TRUE
		/if (${Macro.Return.NotEqual[COMPLETED_SUCCESS]} && ${Macro.Return.NotEqual[ABORT_ROOT]}) {
			/call EchoLog "\ayWe were summoned and cannot get back to camp location, resetting camp to current location."
			/call SetHome
		}
	}
/return

Sub Event_standAttack
	/if (${Me.Sitting}) /stand
/return

Sub Event_spellFade(rLine,string spellName,string charName)
	/if (${Spell[${spellName}].SpellType.Find[Bene]}) {
		/doevents flush spellFade
		/if (${inCombat}) {
			/if (${Me.PctMana} > 30) /call buffCheck FALSE TRUE ${inCombatBuffRoutine}
		} else {
			/if (${Me.PctMana} > 50) /call buffCheck TRUE FALSE NULL
		}
		/doevents flush spellFade
	}
/return

Sub Event_enrage(rLine,string eName)
	/if (${Spawn[${eName}].Type.Equal[NPC]}) {
		/if (${Me.Pet.ID} && !${Me.Pet.Name.Find[familiar]} && !${Me.PetBuff[Master's Aura Effect]}) {
			|'
			/pet back off
			/pet hold on
		}
		/if (${Target.ID} && !${Me.Song[Master's Aura Effect].ID} && ${Me.Combat}) {
			| Need to find a way to check if behind the mob or not and verify it is the mob we are fighting that enraged'
			/varset enrageID ${Target.ID}
			/varset enrageAttackOff TRUE
			/attack off
		}
	}
/return

Sub Event_enrageOff(rLine, string eName)
	/if (${Spawn[${eName}].Type.Equal[NPC]}) {
		/if (${Me.Pet.ID} && !${Me.Pet.Name.Find[familiar]} && !${Me.PetBuff[Master's Aura Effect]}) { 
			| '
			/pet attack
			/pet hold off
		}
		/if (${enrageAttackOff} && !${Me.Combat} && ${Spawn[${enrageID}].CleanName.Equal[${eName}]}) {
			/attack on
			/varset enrageAttackOff FALSE
			/varset enrageID 0
		}
	}
/return

Sub Event_noMerc
	/varset useMerc FALSE
	/call EchoLog "Merc usage disabled because either the group is full or your account status is not high enough to use the mercenary." TRUE
/return

Sub Event_membershipFail
	/varset membershipFail TRUE
/return

Sub Event_mercStanceChange(line,string newStance)
	/varset mercStance ${newStance}
/return

Sub MissionCommandHandler(string comFrom, string comText, bool fromEQBC)
	/if (!${Defined[comFrom]} || !${Defined[comText]}) /return ABORT_PARAMS
	/if (!${Defined[fromEQBC]}) /declare fromEQBC				bool local FALSE
	/declare retValue									string local COMPLETED_NOTFOUND
	/declare paramCount								int local
	/varcalc paramCount ${comText.Count[ ]} + 1
	/declare cParams[${paramCount}]		string local
	/declare strCommand								string local
	/declare ctArray									int local 0
	/declare mcLoop										int local 0
	/declare mcWhile									bool local TRUE
	
	/varset comText ${comText.Replace[MISSIONCOMM ,]}
	
	/varset strCommand ${comText.Token[1, ]}
	/call EchoLog "Global Command: ${strCommand} | ${comText}" TRUE
	/if (${paramCount}) {
		/for nArray 2 to ${paramCount}
			/varset cParams[${Math.Calc[${nArray}-1]}] ${comText.Token[${nArray}, ]}
		/if (${nArray} < 100) /next nArray
	}	
	
	/if (${strCommand.Equal[GETSTATUS]}) {
		| ======================================================
		|  GETSTATUS
		|   Used to have all bots report their status
		| ======================================================
		/call MissionCommandSend ${comFrom} "STATUS ${missionStatus}"
	} else /if (${strCommand.Equal[STATUS]}) {
		| =====================================================
		|  STATUS
		|   Status update, RUNNING,EXECUTINGACTION,ACTIONCOMPLETE,READY,NOTREADY,...
		| =====================================================
		| Need to add trackers for followers to make sure they are checked in
		/call MissionFollowerStatus ${comFrom} ${cParams[1]}
	} else /if (${strCommand.Equal[SETSTATUS]}) {
		/varset missionStatus ${cParams[1]}
		/varset missionActionAttempts 0
		/varset missionActionMaxAttempts 0
	} else /if (${strCommand.Equal[SETACTION]}) {
		/varset missionCurrentAction ${cParams[1]}
		/varset missionStatus STARTACTION
		/varset missionActionAttmempts 0
		/varset missionActionMaxAttempts 0
	} else /if (${strCommand.Equal[GETSTATUS]}) {
		/call MissionCommandSend ${comFrom} ${missionStatus}
	} else /if (${strCommand.Equal[GETSTATS]}) {
		/call MissionCommandSend ${comFrom} "STATS ${Me.PctHPs} ${If[!${Me.MaxMana},100,${Me.PctMana}]} ${If[!${Me.MaxEndurance},100,${Me.PctEndurance}]}"
	} else /if (${strCommand.Equal[STATS]}) {
		/while (${missionFollowerStatus[${mcLoop},1].NotEqual[NULL]} && ${mcLoop} <= ${missionFollowerStatus.Size[1]} && ${mcWhile}) {
			/if (${missionFollowerStatus[${mcLoop},1].Equal[${comFrom},1]}) {
				/varset missionFollowerStatus[${mcLoop},3] ${cParams[2]}
				/varset missionFollowerStatus[${mcLoop},4] ${cParams[3]}
				/varset missionFollowerStatus[${mcLoop},5] ${cParams[4]}
				/varset mcWhile FALSE
			}
		}
	}
/return ${retValue}

Sub MissionCommandSend(string sendTo,string sendText,bool missionCommTagged)
	/declare commandMethod		string local /bct
	/declare mcLoop						int local 0
	/declare postSendDelay		int local 2
	/if (${missionCommMethod.NotEqual[BC]}) {
		/varset commandMethod /tell
		/varset postSendDelay 5
	}
	/if (!${Defined[missionCommTagged]}) /declare missionCommTagged bool local TRUE
	/if (${sendTo.Equal[LEADER]} && ${missionLeader.Equal[GROUP]}) {
		/varset sendTo ${Group.Leader.Name}
	} else /if (${sendTo.Equal[LEADER]}) {
		/varset sendTo ${missionLeader}
	}
	
	/if (${sendTo.Equal[ALL]}) {
		| /bca ${sendText}
		/if (${missionFollowers.Equal[GROUP]}) {
			/for mcLoop 1 to ${Group.Members}
				/if (${Group.Member[${mcLoop}].Type.Equal[PC]}) {
					/docommand ${commandMethod} ${Group.Member[${mcLoop}].Name} ${If[${missionCommTagged},MISSIONCOMM ,]}${sendText}
					/delay ${postSendDelay}
				}
			/next mcLoop
		} else {
			/varset mcLoop 0
			:mcSendLoop
				/varcalc mcLoop ${mcLoop} + 1
				/if (${missionFollowers.Arg[${mcLoop},,].Length}) {
					/docommand ${commandMethod} ${missionFollowers.Arg[${mcLoop},,]} ${If[${missionCommTagged},MISSIONCOMM ,]}${sendText}
					/delay ${postSendDelay}
				} else {
					/goto :mcSendLoopEnd
				}
			/goto :mcSendLoop
			:mcSendLoopEnd
		}
	} else {
		/docommand ${commandMethod} ${sendTo} ${If[${missionCommTagged},MISSIONCOMM ,]}${sendText}
		/delay ${postSendDelay}
	}	
/return

Sub MissionFollowerStatus(string followerName,string statusString,bool setStatus)
	/declare mfLoop			int local 1
	/declare mfWhile		bool local TRUE
	/declare mfRetValue	string local TRUE
	/if (!${Defined[setStatus]}) /declare setStatus bool local FALSE
	
	/if (!${setStatus}) {
		/if (${followerName.Equal[ANY]}) /varset mfRetValue FALSE
		/if (${followerName.Equal[FIND]}) /varset mfRetValue NULL
		/while (${missionFollowerStatus[${mfLoop},1].NotEqual[NULL]} && ${mfLoop} <= ${missionFollowerStatus.Size[1]} && ${mfWhile}) {
			/if (${followerName.Equal[ALL]}) {
				/if ((${statusString.Left[4].Equal[NOT ]} && ${missionFollowerStatus[${mfLoop},2].Equal[${statusString}]}) || (${statusString.Left[4].NotEqual[NOT ]} && ${missionFollowerStatus[${mfLoop},2].NotEqual[${statusString}]})) {
					/varset mfRetValue FALSE
					/varset mfWhile FALSE
				}
			} else /if (${followerName.Equal[ANY]}) {
				/if ((${statusString.Left[4].Equal[NOT ]} && ${missionFollowerStatus[${mfLoop},2].NotEqual[${statusString}]}) || (${statusString.Left[4].NotEqual[NOT ]} && ${missionFollowerStatus[${mfLoop},2].Equal[${statusString}]})) {
					/varset mfRetValue TRUE
					/varset mfWhile FALSE
				}
			} else /if (${followerName.Equal[FIND]}) {
				/if ((${statusString.Left[4].Equal[NOT ]} && ${missionFollowerStatus[${mfLoop},2].NotEqual[${statusString}]}) || (${statusString.Left[4].NotEqual[NOT ]} && ${missionFollowerStatus[${mfLoop},2].Equal[${statusString}]})) {
					/varset mfRetValue ${missionFollowerStatus[${mfLoop},1]}
					/varset mfWhile FALSE
				}
			} else /if (${followerName.Left[4].Equal[STAT]} && ${Select[${followerName.Replace[STAT,]},HP,MANA,ENDURANCE]}) {
				/varset mfRetValue FALSE
				/if (${followerName.Replace[STAT,].Equal[HP]} && ${missionFollowerStatus[${mfLoop},3]} >= ${statusString}) /varset mfRetValue TRUE
				/if (${followerName.Replace[STAT,].Equal[MANA]} && ${missionFollowerStatus[${mfLoop},4]} >= ${statusString}) /varset mfRetValue TRUE
				/if (${followerName.Replace[STAT,].Equal[ENDURANCE]} && ${missionFollowerStatus[${mfLoop},5]} >= ${statusString}) /varset mfRetValue TRUE
				/if (${mfRetValue.Equal[FALSE]}) /varset mfWhile FALSE
			} else /if (${missionFollowerStatus[${mfLoop},1].Equal[${followerName}]}) {
				/if (!${Defined[statusString]} || ${statusString.Equal[NULL]}) {
					/varset mfRetValue ${missionFollowerStatus[${mfLoop},2]}
				} else /if ((${statusString.Left[4].Equal[NOT ]} && ${missionFollowerStatus[${mfLoop},2].Equal[${statusString}]}) || (${statusString.Left[4].NotEqual[NOT ]} && ${missionFollowerStatus[${mfLoop},2].NotEqual[${statusString}]})) {
					/varset mfRetValue FALSE
				}
				/varset mfWhile FALSE
			}
			/varcalc mfLoop ${mfLoop} + 1
		}
	} else {
		/while (${missionFollowerStatus[${mfLoop},1].NotEqual[NULL]} && ${mfLoop} <= ${missionFollowerStatus.Size[1]} && ${mfWhile}) {
			/if (${followerName.Equal[ALL]} || ${missionFollowerStatus[${mfLoop},1].Equal[${followerName}]}) {
				/varset missionFollowerStatus[${mfLoop},2] ${statusString}
				/if (${followerName.NotEqual[ALL]}) /varset mfWhile FALSE
			}
			/varcalc mfLoop ${mfLoop} + 1
		}
		/if (${mfWhile} && ${followerName.NotEqual[ALL]}) {
			/varset ${missionFollowerStatus[${mfLoop},1]} ${followerName}
			/varset ${missionFollowerStatus[${mfLoop},2]} ${statusString}
		}
	}
/return ${mfRetValue}

Sub MissionLoad
	/declare mlLoop				int local 0
	/declare mlWhile			bool local TRUE
	/declare mlInt				int local 0
	
	/call EchoLog "Attempting to load mission information from ${missionINI}" TRUE
	
	/call GetINISetting ${missionINI} General MissionName missionName NULL
	/call GetINISetting ${missionINI} General RequestZone missionRequestZone NULL
	/call GetINISetting ${missionINI} General RequestNPC missionRequestNPC NULL
	/call GetINISetting ${missionINI} General RequestText missionRequestText NULL
	/call GetINISetting ${missionINI} General Requestor missionRequestor LEADER
	/call GetINISetting ${missionINI} General StartText missionStartText NULL
	/call GetINISetting ${missionINI} General StartNPC missionStartNPC NULL
	/call GetINISetting ${missionINI} General StartNPCPath missionStartPath NULL
	/call GetINISetting ${missionINI} General StartDelay missionStartDelay 0s
	/call GetINISetting ${missionINI} General StartRequestor missionStartRequestor LEADER
	/call GetINISetting ${missionINI} General MissionZone missionZone NULL
	/call GetINISetting ${missionINI} General CommunicationMethod missionCommMethod BC
	/call GetINISetting ${missionINI} General CompleteAction missionCompleteAction CAMP
	/call GetINISetting ${missionINI} General RepeatCount missionRepeatCount 0
	/call GetINISetting ${missionINI} General RepeatDelay missionRepeatDelay 30m 
	/call GetINISetting ${missionINI} General RepeatRequestDelay missionRequestDelay 360m
	/call GetINISetting ${missionINI} General MissionStartAtAction missionStartAtAction 1 FALSE
	
	/if (${missionRequestZone.NotEqual[${Zone.ShortName}]}) {
		| Not in the correct zone for this mission
		/call EchoLog ".....We are not in the correct zone to start this mission."
		/varset missionStatus LOADFAILED
		/return FALSE
	}

	/declare repPer				int local
	/declare repRawDelay	int local
	/varset missionRepeatDelayType 
	/if (${missionRepeatDelay.Find[m]}) /varset missionRepeatDelayType m
	/if (${misisonRepeatDelay.Find[s]}) /varset missionRepeatDelayType s
	/varset repRawDelay ${missionRepeatDealy.Replace[${missionRepeatDelayType},]}
	/varcalc repPer ${repRawDelay} * .15
	/varcalc missionRepeatMinDelay ${repRawDelay} - ${repPer}
	/varcalc missionRepeatMaxDelay ${repRawDelay} + ${repPer}
	
	/for mlLoop 1 to ${missionFollowerStatus.Size[1]}
		/varset missionFollowerStatus[${mlLoop},1] NULL
		/varset missionFollowerStatus[${mlLoop},2] NULL
	/next mlLoop
	
	/varset mlInt 1
	/if (${missionFollowers.Equal[GROUP]}) {
		/for mlLoop 1 to ${Group.Members}
			/if (${Group.Member[${mlLoop}].Type.Equal[PC]}) {
				/varset missionFollowerStatus[${mlInt},1] ${Group.Member[${mlLoop}].Name}
				/varset missionFollowerStatus[${mlInt},2] UNKNOWN
				/varset missionFollowerStatus[${mlInt},3] 100
				/varset missionFollowerStatus[${mlInt},4] 100
				/varset missionFollowerStatus[${mlInt},5] 100
				/varcalc mlInt ${mlInt} + 1
			}
		/next mlLoop
	} else {
		/varset mlLoop 1
		/while (${missionFollowers.Arg[${mlLoop},,].Length}) {
			/varset missionFollowerStatus[${mlLoop},1] ${missionFollowers.Arg[${mlLoop},,]}
			/varset missionFollowerStatus[${mlLoop},2] UNKNOWN			
			/varset missionFollowerStatus[${mlLoop},3] 100
			/varset missionFollowerStatus[${mlLoop},4] 100
			/varset missionFollowerStatus[${mlLoop},5] 100
			/varcalc mlLoop ${mlLoop} + 1
		}
	}
	
	/varset pullMode FALSE
	/varset lootCorpses FALSE

	/varset missionCurrentAction ${Math.Calc[${missionStartAtAction}-1].Int}
	/varset missionWaitTimer 0s
	/varset missionWaitCondition NULL	
	/varset missionActionAttempts 0
	/varset missionActionMaxAttempts 0
	
	/if (${missionLeader.Equal[${Me.CleanName}]} || (${missionLeader.Equal[GROUP]} && ${Me.GroupLeader})) /varset missionAmLeader TRUE
	/varset missionStatus LOADED
/return TRUE

Sub MissionLoadAction(int actionNumber,bool buildINI)
	/declare mlLoop				int local 0
	/declare mlWhile			bool local TRUE
	/declare mlInt				int local 0
	/declare iniSection		string local Action${actionNumber}
	/declare actionMainTank	string local NULL
	/declare actionPuller		string local NULL
	/declare actionMainAssist string local NULL
	/if (!${Defined[buildINI]}) /declare buildINI bool local FALSE
	
	/call EchoLog "Attempting to load mission action ${actionNumber} from INI ${missionINI}" TRUE
	
	/call GetINISetting ${missionINI} ${iniSection} Action missionActionType NULL
	
	/if (${missionActionType.Equal[NULL]}) {
		| No action found, mission completed?
		/return ACTION_NOT_FOUND
	}
	
	/if (!${Select[${missionActionType},HAIL,SPEAK,MOVE,MAKECAMP,PULL,IDLE,GIVE,USEITEM]} && !${buildINI}) {
		/call EchoLog ".....${missionActionType} is not a valid Action.  Aborting action load."
		/return INVALID_ACTION_TYPE
	}
	
	| ======================================
	|  ActionConditions
	| ======================================
	/call GetINIArrayCount ${missionINI} ${iniSection} ActionCondition NULL
	/varset mlInt ${Macro.Return}
	/call EchoLog ".....Checking ${mlInt} ActionConditions" TRUE
	/if (${mlInt}) {
		/declare actionConditions[${mlInt}] string local
		/for mlLoop 1 to ${actionConditions.Size[1]}
			/call GetINISetting ${missionINI} ${iniSection} ActionCondition${mlLoop} actionConditions[${mlLoop}] NULL
			/if (!${buildINI} && !${${actionConditions[${mlLoop}]}}) {
				/call EchoLog "........Failed ActionCondition${mlLoop}, skipping action."
				/return ACTION_CONDITION_FAIL
			}
		/next mlLoop
	} else {
		/call GetINISetting ${missionINI} ${iniSection} ActionCondition1 NULL NULL
	}
	
	/call GetINISetting ${missionINI} ${iniSection} DoActionByCommand missionActionByCommand TRUE
	/call GetINISetting ${missionINI} ${iniSection} FollowLeader missionActionFollowLeader TRUE
	/call GetINISetting ${missionINI} ${iniSection} LocX missionActionLocX NULL FALSE
	/call GetINISetting ${missionINI} ${iniSection} LocY missionActionLocY NULL FALSE
	/call GetINISetting ${missionINI} ${iniSection} LocZ missionActionLocZ NULL FALSE
	/call GetINISetting ${missionINI} ${iniSection} PathName missionActionPathName NULL
	/call GetINISetting ${missionINI} ${iniSection} PullPathName missionActionPullPath NULL
	/call GetINISetting ${missionINI} ${iniSection} LocFuzziness missionActionLocFuzziness 10
	/call GetINISetting ${missionINI} ${iniSection} TargetNPC missionActionTargetNPC NULL

	| ======================================
	|  TargetPullNPCs
	| ======================================	
	/varset missionActionKillCounts FALSE
	/call GetINIArrayCount ${missionINI} ${iniSection} TargetPullNPC NULL
	/varset mlInt ${Macro.Return}
	/if (${mlInt}) {
		/varset missionActionPullLimit TRUE
		/for mlLoop 1 to ${mlInt}
			/call GetINISetting ${missionINI} ${iniSection} TargetPullNPC${mlLoop} missionActionPullNPC[${mlLoop},1] NULL
			/call GetINISetting ${missionINI} ${iniSection} TargetPullNPC${mlLoop}KillCount missionActionPullNPC[${mlLoop},2] 0
			/if (${missionActionPullNPC[${mlLoop},2]}) /vaset missionActionKillCounts TRUE
		/next mlLoop
	} else {
		/call GetINISetting ${missionINI} ${iniSection} TargetPullNPC1 NULL NULL
		/call GetINISetting ${missionINI} ${iniSection} TargetPullNPC1KillCount NULL 0
	}
	
	/call GetINISetting ${missionINI} ${iniSection} TargetRadius missionActionTargetRadius 20
	/call GetINISetting ${missionINI} ${iniSection} SpeakText missionActionSpeakText NULL
	/call GetINISetting ${missionINI} ${iniSection} SetMainTank actionMainTank NULL
	/call GetINISetting ${missionINI} ${iniSection} SetPuller actionPuller NULL
	/call GetINISetting ${missionINI} ${iniSection} SetMainAssist actionMainAssist NULL
	/call GetINISetting ${missionINI} ${iniSection} LootMobs missionActionLootMobs TRUE
	/call GetINISetting ${missionINI} ${iniSection} Looter missionActionLooter LEADER

	| ======================================
	|  ActionItems
	| ======================================
	/call GetINIArrayCount ${missionINI} ${iniSection} ActionItem NULL
	/varset mlInt ${Macro.Return}
	/if (${mlInt}) {
		/for mlLoop 1 to ${mlInt}
			/call GetINISetting ${missionINI} ${iniSection} ActionItem${mlLoop} missionActionItem[${mlLoop},1] NULL
			/call GetINISetting ${missionINI} ${iniSection} ActionItem${mlLoop}Count missionActionItem[${mlLoop},2] 0
		/next mlLoop
	} else {
		/call GetINISetting ${missionINI} ${iniSection} ActionItem1 NULL NULL
		/call GetINISetting ${missionINI} ${iniSection} ActionItem1Count NULL 0		
	}

	| ======================================
	|  PreExecuteCommands
	| ======================================
	/call GetINIArrayCount ${missionINI} ${iniSection} PreExecuteCommand NULL
	/varset mlInt ${Macro.Return}
	/if (${mlInt}) {
		/for mlLoop 1 to ${mlInt}
			/call GetINISetting ${missionINI} ${iniSection} PreExecuteCommand${mlLoop} missionActionPreCommand[${mlLoop}] NULL
		/next mlLoop
	} else {
		/call GetINISetting ${missionINI} ${iniSection} PreExecuteCommand1 NULL NULL
	}

	| ======================================
	|  PostExecuteCommands
	| ======================================
	/call GetINIArrayCount ${missionINI} ${iniSection} PostExecuteCommand NULL
	/varset mlInt ${Macro.Return}
	/if (${mlInt}) {
		/for mlLoop 1 to ${mlInt}
			/call GetINISetting ${missionINI} ${iniSection} PostExecuteCommand${mlLoop} missionActionPostCommand[${mlLoop}] NULL
		/next mlLoop
	} else {
		/call GetINISetting ${missionINI} ${iniSection} PostExecuteCommand1 NULL NULL
	}
	
	/call GetINISetting ${missionINI} ${iniSection} ActionMaxTime missionActionMaxTime 999m

	| ======================================
	|  CompleteConditions
	| ======================================
| These should probably loaded and checked during the action so that any variables are correctly parsed
	/call GetINIArrayCount ${missionINI} ${iniSection} CompleteCondition NULL
	/varset mlInt ${Macro.Return}
	/varset missionActionCompleteConditionCount ${mlInt}
	/if (${mlInt}) {
		/for mlLoop 1 to ${mlInt}
			/call GetINISetting ${missionINI} ${iniSection} CompleteCondition${mlLoop} missionActionCompleteCondition[${mlLoop}] NULL
		/next mlLoop
	} else {
		/call GetINISetting ${missionINI} ${iniSection} CompleteCondition1 NULL NULL
	}	
	/call GetINISetting ${missionINI} ${iniSection} CompleteConditionRequire missionActionCompleteConditionRequire ALL
	
	/call GetINISetting ${missionINI} ${iniSection} CompleteBy missionActionCompleteBy ALL
	/call GetINISetting ${missionINI} ${iniSection} CompleteDelay missionActionCompleteDelay 2s
	
	/if (${actionMainTank.NotEqual[NULL]}) {
		/if (${actionMainTank.Equal[group]}) {
			/varset mainTank ${Group.MainTank}
			/varset useGroupTank TRUE
		} else {
			/varset mainTank ${actionMainTank}
			/varset useGroupTank FALSE
		}
	}
	/if (${actionPuller.NotEqual[NULL]}) {
		/if (${actionPuller.Equal[group]}) {
			/varset pullerName ${Group.Puller}
			/varset useGroupPuller TRUE
		} else {
			/varset pullerName ${actionPuller}
			/varset useGroupPuller FALSE
		}
	}
	/if (${actionMainAssist.NotEqual[NULL]}) {
		/if (${actionMainAssist.Equal[group]}) {
			/varset mainAssist ${Group.MainAssist}
			/varset useGroupAssist TRUE
		} else {
			/varset mainAssist ${actionMainAssist}
			/varset useGroupAssist FALSE
		}
	}
/return TRUE

Sub MissionModeEnd(string endReason)
	/if (${Defined[endReason]}) /call EchoLog "${endReason}"
	/if (${missionAmLeader}) /call MissionCommandSend ALL "MissionMode OFF" FALSE
	/varset missionMode FALSE
	/varset missionStatus NULL	
/return

Sub MissionModeHandler
	/if (!${missionMode}) /return NOT_MISSION_MODE
	/declare mmLoop				int local 0
	/declare mmInt				int local 0
	/declare mmWhile			bool local FALSE
	
	/if (${missionActionMaxAttempts}!=0 && ${missionActionAttempts}>=${missionActionMaxAttempts}) {
		| Reached maximum attempts for the current action
		/varset missionStatus ACTIONFAILED
		/varset missionActionMaxAttempts 0
		/if (!${missionAmLeader})) /call MissionCommandSend LEADER "STATUS ACTIONFAILED"
	}	
	/if (${missionStatus.NotEqual[ACTIONFAILED]} && ${missionAmLeader}) {
		/call MissionFollowerStatus ANY ACTIONFAILED
		/if (${Macro.Return.Equal[TRUE]}) {
			/varset missionStatus ACTIONFAILED
			/varset missionWaitTimer 0s
		}
	}
		
	/if (${missionWaitTimer}) {
		/if (${missionWaitCondition.NotEqual[NULL]}) {
			| /varset missionWaitTimer 0s
			/if (${missionWaitCondition.Left[6].Equal[STATUS]}) {
				/call MissionCommandSend ALL "GETSTATUS"
				/declare statusCheck string local ${missionWaitCondition.Replace[STATUS ,]}
				/call MissionFollowerStatus ${statusCheck}
				/if (${Macro.Return.Equal[TRUE]}) /varset missionWaitTimer 0s
			} else /if (${${missionWaitCondition}}) {
				/varset missionWaitTimer 0s
			}
		}
	}
	/if (${missionWaitTimer}) /return MISSION_WAIT_TIMER

	/if (${missionStatus.Equal[NULL]} || !${missionStatus.Length}) {
		| Load the mission
		/call MissionLoad
	} else /if (${missionStatus.Equal[LOADED]}) {
		| Check in with followers, wait for all to be running
		/if (${missionAmLeader}) {
			/varset missionActionMaxAttempts 6
			/varcalc missionActionAttempts ${missionActionAttempts} + 1
			
			/call MissionFollowerStatus ALL LOADED
			/if (${Macro.Return.Equal[FALSE]}) {
				/call MissionFollowerStatus ANY LOADFAILED
				/if (${Macro.Return.Equal[TRUE]}) {
					/call MissionModeEnd "One of the followers failed to load the mission."
				} else {
					/call MissionCommandSend ALL "MissionMode ${missionINI}" FALSE
					/delay 5s
					/call MissionCommandSend ALL "GETSTATUS"
					/varset missionWaitTimer 30s
					/varset missionWaitCondition STATUS ALL LOADED
				}
			} else {
				/if (${missionStartAtAction} > 0 ) {
					/if (${missionAmLeader}) {
						/call MissionCommandSend ALL "SETSTATUS IDLEACTION"
						/varset missionCurrentAction ${Math.Calc[${missionStartAtAction}+1].Int}
						/varset missionStatus STARTACTION
						/varset missionWaitTimer 1m
						/varset missionWaitCondition STATUS IDLEACTION
					}
				} else {
					/if (${missionRequestor.Equal[ALL]}) /call MissionCommandSend ALL "SETSTATUS REQUEST"
					/varset missionStatus REQUEST
				}
			}
		}
	} else /if (${missionStatus.Equal[REQUEST]}) {
		/if (${missionRequestTimer}) {
			/varset missionWaitTimer ${missionRequestTimer}
			/call EchoLog "Waiting on mission request lockout timer to expire."
		} else {
			/if (!${Window[TaskWnd].Open}) /windowstate TaskWnd open
			/delay 1s ${Window[TaskWnd].Open}
			/if (${Window[TaskWnd].Open}) {
				/for mmLoop 1 to ${Window[TaskWnd].Child[TASK_TaskList].Items}
					/if (${Window[TaskWnd].Child[TASK_TaskList].List[${mmLoop},2].Equal[${missionName}]}) {
						| Already have the mission
						/windowstate TaskWnd close
						/delay 5s !${Window[TaskWnd].Open}
						/varset missionStatus REQUESTDONE
					}
				/next mmLoop
			}
			/if (${missionStatus.Equal[REQUEST]}) {
				/varset missionActionMaxAttempts 5
				/varcalc missionActionAttempts ${missionActionAttempts} + 1
				/squelch /target npc ${missionRequestNPC}
				/delay 2s ${Target.CleanName.Find[${missionRequestNPC}]}
				/if (${Target.CleanName.Find[${missionRequestNPC}]}) {
					/if (${Target.Distance} > 20) {
						| To far from the NPC
						/call EchoLog "To far from ${missionRequestNPC} to request the mission."
						/varset missionActionAttempts 0
						/varset missionActionMaxAttempts 0
						/varset missionStatus ACTIONFAILED
					} else {
						/delay ${Math.Rand[21]}
						/say ${missionRequestText}
						/delay 5s ${Window[TaskSelectWnd].Open}
						/if (${Window[TaskSelectWnd].Open}) {
							| Mission names with commas will cause problems here, should probably fix that
							/for mmLoop 1 to ${Window[TaskSelectWnd].Child[TSEL_TaskList].Items}
								/if (${Window[TaskSelectWnd].Child[TSEL_TaskList].List[${mmLoop}].Equal[${missionName}]}) {
									/notify TaskSelectWnd TSEL_TaskList listselect ${mmLoop}
									/delay 5
									/notify TaskSelectWnd TSEL_AcceptButton leftmouseup
									/delay 2s ${Window[TaskWnd].Open}
									/if (!${Window[TaskWnd].Open}) /windowstate TaskWnd open
									/delay 3
								}
							/next mmLoop
							/if (${Window[TaskWnd].Open}) {
								/for mmLoop 1 to ${Window[TaskWnd].Child[TASK_TaskList].Items}
									/if (${Window[TaskWnd].Child[TASK_TaskList].List[${mmLoop},2].Equal[${missionName}]}) {
										/windowstate TaskWnd close
										/delay 2s !${Window[TaskWnd].Open}
										/varset missionStatus REQUESTDONE
									}
								/next mmLoop
							}
						}
						/if (${missionStatus.Equal[REQUEST]}) {
							/varset missionWaitTimer 20s
						} else {
							/varcalc missionRunCount ${missionRunCount} + 1
							/varset missionActionMaxAttempts 0
							/varset missionActionAttempts 0
							/varset missionWaitTimer 0s
						}
					}
				}
			}
		}
	} else /if (${missionStatus.Equal[REQUESTDONE]}) {
		/if (${missionAmLeader}) {
			/if (${missionRequestor.Equal[ALL]}) {
				/call MissionCommandSend ALL "GETSTATUS"
				/delay 5
				/call MissionFollowerStatus ALL "REQUESTDONE"
				/if (${Macro.Return.Equal[TRUE]}) {
					/varset missionWaitTimer ${missionStartDelay}
					/varset missionStatus START
				} else {
					/varset missionActionMaxAttempts 5
					/varcalc missionActionAttempts ${missionActionAttempts} + 1
					/varset missionWaitTimer 20s
					/varset missionWaitCondition STATUS ALL REQUESTDONE
				}
			} else {
				/varset missionRequestTimer ${missionRequestDelay}
				/varset missionWaitTimer ${missionStartDelay}
				/varset missionStatus START
			}
		}
	} else /if (${missionStatus.Equal[START]}) {
		/if (${missionStartRequestor.NotEqual[NONE]}) {
			/if (${missionStartRequestor.Equal[ALL]} && ${missionAmLeader}) /call MissionCommandSend ALL "SETSTATUS START"
			/if (${missionStartPath.NotEqual[NULL]}) {
				/call LoadPath ${missionStartPath}
				/if (${Macro.Return.Equal[TRUE]}) {
					/call SetNearestLoc TRUE
					/call Movement ${curLoc} ${maxLoc}
					/if (${Macro.Return.Equal[COMPLETED_SUCCESS]}) {
						/if (${missionStartNPC.NotEqual[NULL]} && ${missionStartText.NotEqual[NULL]}) {
							/squelch /target npc ${missionStartNPC}
							/delay 2s ${Target.CleanName.Find[${missionStartNPC}]}
							/if (${Target.CleanName.Find[${missionStartNPC}]} && ${Target.Distance} <= 20) {
								/say ${missionStartText}
							} else {
								/call EchoLog "Unable to find or get close enough to start npc"
								/varset missionStatus ACTIONFAILED
							}
						}
						/if (${missionAmLeader}) {
							/varset missionCurrentAction 1
							/varset missionStatus STARTACTION
							/if (${missionStartRequestor.Equal[LEADER]}) /call MissionCommandSend ALL "SETSTATUS WAITINGACTION"
							/varset missionWaitTimer 1m
							/varset missionWaitCondition STATUS WAITINGACTION
						} else {
							/varset missionStatus WAITINGACTION
						}
					} else /if (${Select[${Macro.Return},ABORT_ZAXIS,ABORT_STUCK,ABORT_TIMEOUT,ABORT_WRONG_ZONE]}) {
						/call EchoLog "Movement on the StartPath ${missionStartPath} failed"
						/varset missionStatus ACTIONFAILED
					}
				} else {
					/call EchoLog "Could not load mission StartPath ${missionStartPath}"
					/varset missionStatus ACTIONFAILED
				}
			}
		} else /if (${missionAmLeader}) {
			/varset missionCurrentAction ${Math.Calc[${missionStartAtAction}+1].Int}
			/varset missionStatus STARTACTION
			/varset missionWaitTimer 1m
			/varset missionWaitCondition STATUS WAITINGACTION
		} else {
			/varset missionStatus WAITINGACTION
		}
	} else /if (${missionStatus.Equal[STARTACTION]}) {
		/call MissionLoadAction ${missionCurrentAction}
		/if (${Macro.Return.Equal[ACTION_CONDITION_FAIL]}) {
			/varset missionStatus COMPLETEACTION
		} else /if (${Macro.Return.Equal[ACTION_NOT_FOUND]}) {
			| Mission must be completed
			/varset missionStatus COMPLETEMISSION
		} else {
			
		}
		
		/varset missionPrimaryActionCompleted FALSE
		
		/if (${missionActionLootMobs}) {
			/for mmLoop 1 to ${missionActionItem.Size[1]}
				/if (${missionActionItem[${mmLoop},1].NotEqual[NULL]}) /ini "${lootININame}" ${missionActionItem[${mmLoop},1].Left[1]} "${missionActionItem[${mmLoop},1]}" KEEP
				/call GetItemCount "${missionActionItem[${mmLoop},1]}"
				/varset missionActionItem[${mmLoop},3] ${Macro.Return}
			/if (${missionActionItem[${mmLoop},1].NotEqual[NULL]}) /next mmLoop
		}
		
		/if (${missionActionKillCounts}) {
			/varset trackKills TRUE
			/for mmLoop 1 to ${missionActionPullNPC.Size[1]}
				/if (${missionActionPullNPC[${mmLoop},1].NotEqual[NULL]}) /call TrackKillReset "${missionActionPullNPC[${mmLoop},1]}" TRUE
			/if (${missionActionPullNPC[${mmLoop},1].NotEqual[NULL]}) /next mmLoop
		}
		
		| Execute mission leader only stuff
		/if (${missionAmLeader}) {
			/varset missionActionMaxTimer ${missionActionMaxTime}
			/if (${missionActionLootMobs}) {
				/call MissionCommandSend ALL "Loot OFF" FALSE
				/delay 2
				/if (!${Select[${missionActionLooter},NULL,ALL,LEADER]}) {
					/call MissionCommandSend ${missionActionLooter} "Loot ON" FALSE
				} else /if (${missionActionLooter.Equal[LEADER]} || ${missionActionLooter.Equal[NULL]}) {
					/varset lootCorpses TRUE
				}
			}

			/call missionCommandSend ALL "Pull OFF" FALSE
			/delay 2
			/if (${missionActionType.Equal[PULL]}) {
				/if (${pullerName.NotEqual[${Me.CleanName}]}) {
					/call MissionCommandSend ${pullerName} "pullradius ${missionActionTargetRadius}" FALSE
					/delay 2
					/call MissionCommandSend ${pullerName} "pull ${missionActionPullPath}" FALSE
					
					/if (${missionActionPullLimit}) {
						/delay 2
						/call MissionCommandSend ${pullerName} "pulllimit clear" FALSE
						/delay 2
						/for mmLoop 1 to ${missionActionPullNPC.Size[1]}
							/if (${missionActionPullNPC[${mmLoop},1].NotEqual[NULL]}) /call MissionCommandSend ${pullerName} "pulllimit add ${missionActionPullNPC[${mmLoop},1]}" FALSE
							/delay 2
						/if (${missionActionPullNPC[${mmLoop},1].NotEqual[NULL]}) /next mmLoop
					}
				} else {
					/varset pullRadius ${missionActionTargetRadius}
					/squelch /alert clear ${pullTargetAlert}
					/for mmLoop 1 to ${missionActionPullNPC.Size[1]}
						/if (${missionActionPullNPC[${mmLoop},1].NotEqual[NULL]}) /squelch /alert add ${pullTargetAlert} ${missionActionPullNPC[${mmLoop},1]}
					/if (${missionActionPullNPC[${mmLoop},1].NotEqual[NULL]}) /next mmLoop
					/echo pull ${missionActionPullPath}
				}
			}
			
			/if (${missionActionCompleteBy.Equal[ALL]} && !${missionActionByCommand}) {
				/call MissionCommandSend ALL "SETACTION ${missionCurrentAction}"
			} else {
				/call MissionCommandSend ALL "SETSTATUS IDLEACTION"
			}
		}
		
		| Do Pre Commands
		/for mmLoop 1 to ${missionActionPreCommand.Size[1]}
			/if (${missionActionPreCommand.NotEqual[NULL]}) /docommand ${missionActionPreCommand[${mmLoop}]}
		/next mmLoop
		
		| If we need to move for this mission
		/if (${missionActionPathName.NotEqual[NULL]} || (${missionActionLocX.NotEqual[NULL]} && ${missionActionLocY.NotEqual[NULL]})) {
			/if (${missionActionFollowLeader} && ${missionAmLeader}) {
				/call MissionSendCommand ALL "Follow me" FALSE
				/delay 1s
			}
			/if (${missionAmLeader} || !${missionActionFollowLeader}) {
				/if (${missionActionPathName.NotEqual[NULL]}) {
					/call LoadPath ${missionActionPathName}
					/call SetNearestLoc
					/call Movement ${curLoc} ${maxLoc}
				} else /if (${missionActionLocX.NotEqual[NULL]} && ${missionActionLocY.NotEqual[NULL]}) {
					/call MoveToLoc ${missionActionLocY} ${missionActionLocX}${If[${missionActionLocZ.NotEqual[NULL]},FALSE ${missionActionLocZ},]}
				}
			}
			/if (${missionActionFollowLeader} && ${missionAmLeader}) /call MissionSendCommand ALL "stay" FALSE
		}
		
		/if (${missionActionLooter.Equal[ALL]}) /echo loot on

		/varset missionStatus DOACTION
		| Possibly look into having CompleteBy as a comma separated list so that only certian characters have to complete it?
	} else /if (${missionStatus.Equal[DOACTION]}) {
		/declare killCountsComplete			bool local TRUE
		/declare lootCountsComplete			bool local TRUE
		/declare giveItemCount					int local 0
		| HAIL - hail a specified NPC 
		| SPEAK - speak to a specified NPC saying specified text
		| MOVE - move
		| MAKECAMP - make a camp at the area, usually for the purpose of pulling mobs to
		| PULL - pull mobs (more on filters for what to pull later, also, only the bot configured as puller will PULL during a PULL action)
		| IDLE - just stand around and wait (bots will engage in combat if they are engaged), use this also if the group should med / heal up before moving to the next action
		| GIVE - give items to an NPC
		| USEITEM - Use specified items (right click them, from your inventory, if you need to interact with world objects that can be handled in the recorded path and set as PathName)
		/if (!${missionPrimaryActionCompleted}) {		
			/if (${Select[${missionActionType},HAIL,SPEAK,GIVE]}) {
				/squelch /target npc ${missionActionTargetNPC}
				/delay 2s ${Target.CleanName.Find[${missionActionTargetNPC}]}
				/if (${Target.CleanName.Find[${missionActionTargetNPC}]}) {
					/squelch /stick 15
					/delay 5s ${Target.Distance} <= 20
					/squelch /stick off
					/if (${missionActionType.Equal[HAIL]}) {
						/hail
					} else /if (${missionActionType.Equal[SPEAK]}) {
						/say ${missionActionSpeakText}
					} else /if (${missionActionType.Equal[GIVE]}) {
						/call Open_Packs
						/for mmLoop 1 to ${missionActionItem.Size[1]}
							/if (${missionActionItem[${mmLoop},1].NotEqual[NULL]}) {
								/for mmInt 1 to ${If[${missionActionItem[${mmLoop},2]}==0,1,${missionActionItem[${mmLoop},2]}]}
									/if (${FindItem[${missionActionItem[${mmLoop},1]}].ID}) {
										:mmPickupItem
											/ctrlkey /itemnotify "${missionActionItem[${mmLoop},1]}" leftmouseup
											/delay 2s ${Cursor.ID}
											/if (${Cursor.ID}) {
												/click left target
												/delay 2s ${Window[GiveWnd].Open}
												/varcalc giveItemCount ${giveItemCount} + 1
											} else {
												/goto :mmPickupItem
											}
									} else {
										| Don't have the item??
									}
									/if (${giveItemCount}>=4) {
										:mmWaitForGive
											/notify GiveWnd GVW_Give_Button leftmouseup 
											/delay 1s !${Window[GiveWnd].Open}
										/if (${Window[GiveWnd].Open}) /goto :mmWaitForGive
										/varset giveItemCount 0
									}
								/next mmInt
							}
							/if (${giveItemCount}>=4) {
								:mmWaitForGive1
									/notify GiveWnd GVW_Give_Button leftmouseup 
									/delay 1s !${Window[GiveWnd].Open}
								/if (${Window[GiveWnd].Open}) /goto :mmWaitForGive1
								/varset giveItemCount 0
							}							
						/next mmLoop
						/if (${Window[GiveWnd].Open}) {
							:mmWaitForGive2
								/notify GiveWnd GVW_Give_Button leftmouseup 
								/delay 1s !${Window[GiveWnd].Open}
							/if (${Window[GiveWnd].Open}) /goto :mmWaitForGive2
							/varset giveItemCount 0
						}
					}
					/varset missionPrimaryActionCompleted TRUE
				}
			} else /if (${missionActionType.Equal[MAKECAMP]}) {
				| Perhaps change this to actually emulating the stay command instead of echoing it?				
				/echo stay
				/varset missionPrimaryActionCompleted TRUE
			} else /if (${missionActionType.Equal[USEITEM]}) {
				/for mmLoop 1 to ${missionActionItem.Size[1]}
					/if (${missionActionItem[${mmLoop},1].NotEqual[NULL]}) {
						/for mmInt 1 to ${If[${missionActionItem[${mmLoop},2]}==0,1,${missionActionItem[${mmLoop},2]}]}
							/call MQ2Cast "${missionActionItem[${mmLoop},1]}" ITEM
							/delay 3
						/next mmInt
					}
				/next mmLoop
				/varset missionPrimaryActionCompleted TRUE
			} else /if (${missionActionType.Equal[PULL]}) {
				| Track Kill count / Loot count
				/if (${missionActionKillCounts}) {
					/for mmLoop 1 to ${missionActionPullNPC.Size[1]}
						/if (${missionActionPullNPC[${mmLoop},1].NotEqual[NULL]}) {
							/call TrackKillCount "${missionActionPullNPC[${mmLoop},1]}"
							/if (${Macro.Return} < ${missionActionPullNPC[${mmLoop},2]}) /varset killCountsComplete FALSE
						}
					/if (${missionActionPullNPC[${mmLoop},1].NotEqual[NULL]} && ${killCountsComplete}) /next mmLoop
				}
				/if (${missionActionLootMobs}) {
					/for mmLoop 1 to ${missionActionItem.Size[1]}
						/call GetItemCount "${missionActionItem[${mmLoop},1]}"
						/if (${Math.Calc[${Macro.Return}-${missionActionItem[${mmLoop},3]}]} < ${missionActionItem[${mmLoop},2]}) /varset lootCountsComplete FALSE
					/if (${missionActionItem[${mmLoop},1].NotEqual[NULL]} && ${lootCountsComplete}) /next mmLoop
				}				
				
				/if (${lootCountsComplete} && ${killCountsComplete}) /varset missionPrimaryActionCompleted TRUE
			} else {
				| MOVE,IDLE, unrecognized, these don't require any further actual actions right now
				/varset missionPrimaryActionCompleted TRUE
			}
		}
		
		| Check complete rules for setting missionStatus to COMPLETEACTION
		/if (${missionActionCompleteConditionCount}) {
			/declare completeConditionsMet		int local 0
			/declare completeConditionRequire	int local 0
			/declare groupMeets								bool local FALSE
			
			/call MissionCommandSend ALL GETSTATS
			/if (${missionActionCompleteConditionRequire.Equal[ALL]}) {
				/varset completeConditionRequire ${missionActionCompleteConditionCount}
			} else /if (${missionActionCompleteConditionRequire.Equal[ANY]}) {
				/varset completeConditionRequire 1
			} else {
				/varset completeConditionRequire ${missionActionCompleteConditionRequire}
			}
			| KILLCOUNT - this condition will be true when it has met the KillCount specified for all PullNPCs.  If any KillCounts are set to 0 this will never be TRUE.
			| LOOTCOUNT - this condition will be true when all the LootItemCounts have been met for the LootItems.  If any LootItemCounts are 0 this will never be TRUE.
			| MANA>XX - condition is completed if mana is greater than or equal to XX, so if you want to say a completed condition is when mana is at 100% you would use MANA>100
			| HP>XX - same as mana, except for hitpoints
			| ENDURANCE>XX - same as mana and HP, except for endurance			
			/for mmLoop 1 to ${missionActionCompleteConditionCount}
				/if (${missionActionCompleteCondition[${mmLoop}].Equal[KILLCOUNT]}) {
					| Track Kill count / Loot count
					/if (${missionActionKillCounts}) {
						/varset killCountsComplete TRUE
						/for mmLoop 1 to ${missionActionPullNPC.Size[1]}
							/if (${missionActionPullNPC[${mmLoop},1].NotEqual[NULL]}) {
								/call TrackKillCount "${missionActionPullNPC[${mmLoop},1]}"
								/if (${Macro.Return} < ${missionActionPullNPC[${mmLoop},2]}) /varset killCountsComplete FALSE
							}
						/if (${missionActionPullNPC[${mmLoop},1].NotEqual[NULL]} && ${killCountsComplete}) /next mmLoop
						/if (${killCountsComplete}) /varcalc completeConditionsMet ${completeConditionsMet} + 1
					} else {
						/varcalc completeConditionsMet ${completeConditionsMet} + 1
					}
				} else /if (${missionActionCompleteCondition[${mmLoop}].Equal[LOOTCOUNT]}) {
					/if (${missionActionLootMobs}) {
						/varset lootCountsComplete TRUE
						/for mmLoop 1 to ${missionActionItem.Size[1]}
							/call GetItemCount "${missionActionItem[${mmLoop},1]}"
							/if (${Math.Calc[${Macro.Return}-${missionActionItem[${mmLoop},3]}]} < ${missionActionItem[${mmLoop},2]}) /varset lootCountsComplete FALSE
						/if (${missionActionItem[${mmLoop},1].NotEqual[NULL]} && ${lootCountsComplete}) /next mmLoop
							/if (${lootCountsComplete}) /varcalc completeConditionsMet ${completeConditionsMet} + 1
					}	else {
						/varcalc completeConditionsMet ${completeConditionsMet} + 1
					}
				} else /if (${missionActionCompleteCondition[${mmLoop}].Left[5].Equal[MANA>]}) {
					/if (${missionCompleteBy.Equal[LEADER]} && ${missionAmLeader}) {
						/call MissionFollowerStatus STATMANA ${missionActionCompleteCondition[${mmLoop}].Replace[MANA>,]}
						/if (${Macro.Return.Equal[TRUE]}) /varcalc completeConditionsMet ${completeConditionsMet} + 1
					} else {
						/if (${Me.PctMana} >= ${missionActionCompleteCondition[${mmLoop}].Replace[MANA>,]}) /varcalc completeConditionsMet ${completeConditionsMet} + 1
					}
				} else /if (${missionActionCompleteCondition[${mmLoop}].Left[3].Equal[HP>]}) {
					/if (${missionCompleteBy.Equal[LEADER]} && ${missionAmLeader}) {
						/call MissionFollowerStatus STATHP ${missionActionCompleteCondition[${mmLoop}].Replace[HP>,]}
						/if (${Macro.Return.Equal[TRUE]}) /varcalc completeConditionsMet ${completeConditionsMet} + 1						
					} else {
						/if (${Me.PctHPs} >= ${missionActionCompleteCondition[${mmLoop}].Replace[HP>,]}) /varcalc completeConditionsMet ${completeConditionsMet} + 1
					}					
				} else /if (${missionActionCompleteCondition[${mmLoop}].Left[10].Equal[ENDURANCE>]}) {
					/if (${missionCompleteBy.Equal[LEADER]} && ${missionAmLeader}) {
						/call MissionFollowerStatus STATENDURANCE ${missionActionCompleteCondition[${mmLoop}].Replace[ENDURANCE>,]}
						/if (${Macro.Return.Equal[TRUE]}) /varcalc completeConditionsMet ${completeConditionsMet} + 1						
					} else {
						/if (${Me.PctEndurance} >= ${missionActionCompleteCondition[${mmLoop}].Replace[ENDURANCE>,]}) /varcalc completeConditionsMet ${completeConditionsMet} + 1
					}					
				} else {
					/if (${${Ini[${missionINI},Action${missionCurrentAction},CompleteCondition${mmLoop},FALSE]}}) /varcalc completeConditionsMet ${completeConditionsMet} + 1
				}
			/next mmLoop
			
			/if (${completeConditionsMet} >= ${completeConditionRequire}) {
				| CompleteConditions met, Action is completed
				/varset missionPrimaryActionCompleted TRUE
				/varset missionStatus COMPLETEACTION
			} else {
				/varset missionWaitTimer 30s
			}
		} else /if (${missionPrimaryActionCompleted}) {
			| No CompleteConditions, primary action completed so move to COMPLETEACTION
			/varset missionStatus COMPLETEACTION
		}
	} else /if (${missionStatus.Equal[COMPLETEACTION]}) {
		/if (${missionPrimaryActionCompleted}) {
			| Do Post Commands
			/for mmLoop 1 to ${missionActionPostCommand.Size[1]}
				/if (${missionActionPostCommand.NotEqual[NULL]}) /docommand ${missionActionPostCommand[${mmLoop}]}
			/next mmLoop
		}
				
		/if (${missionAmLeader}) {
			/if (${missionActionCompleteBy.Equal[LEADER]}) {
				/varcalc missionCurrentAction ${missionCurrentAction} + 1
				/varset missionWaitTimer ${missionActionCompleteDelay}
				/varset missionStatus STARTACTION
			} else {
				/varset missionStatus COMPLETEDWAITACTION
			}
		} else {
			/call MissionCommandSend LEADER "STATUS COMPLETEDWAITACTION"
			/varset missionStatus COMPLETEDWAITACTION
		}
	} else /if (${missionStatus.Equal[COMPLETEDWAITACTION]}) {
		/declare allCompleted				bool local FALSE
		/declare waitingOnCompleted	bool local FALSE

		/if (${missionAmLeader}) {
			/if (${missionActionByCommand}) {
				/if (${missionWaitingOn.NotEqual[NULL]}) {
					/call MissionFollowerStatus ${missionWaitingOn} COMPLETEDWAITACTION
					/if (${Macro.Return.Equal[TRUE]}) /varset waitingOnCompleted TRUE
				}
				/if (!${waitingOnCompleted}) {
					/call MissionFollowerStatus FIND IDLEACTION
					/if (${Macro.Return.NotEqual[NULL]}) {
						| Should return the first follower that is not currently on IDLEACTION
						/call MissionCommandSend ${Macro.Return} "SETACTION ${missionCurrentAction}"
						/call MissionFollowerStatus ${Macro.Return} "DOACTION" TRUE
						/varset missionWaitingOn ${Macro.Return}
					} else {
						/call MissionFollowerStatus ALL COMPLETEDWAITACTION
						/if (${Macro.Return.NotEqual[TRUE]}) /varset allCompleted TRUE
					}
				}
			} else {
				| Wait for everyone to complete the action
				/call MissionFollowerStatus ALL COMPLETEDWAITACTION
				/if (${Macro.Return.Equal[FALSE]}) /varset allCompleted TRUE
			}
			/if (${allCompleted}) {
				| Move on to the next action
				/varset missionWaitingOn NULL
				/varcalc missionCurrentAction ${missionCurrentAction} + 1
				/varset missionWaitTimer ${missionActionCompleteDelay}
				/varset missionWaitCondition NULL
				/varset missionStatus STARTACTION								
			} else {
				/varset missionWaitTimer 30s
				/varset missionWaitCondition NULL
			}
		}
	} else /if (${missionStatus.Equal[IDLEACTION]}) {
		| Idle
	} else /if (${missionStatus.Equal[COMPLETEMISSION]}) {
		| Mission completed, clean up and do reset or camp depending on CompleteAction
		/if (${missionAmLeader} && ${Select[${missionCompleteAction},CAMP,CAMPDESKTOP,ENDMACRO]}) /call MissionCommandSend ALL "SETSTATUS COMPLETEMISSION"
		
		/varset missionCurrentAction 0
		/varset missionWaitTimer 0s
		/varset missionWaitCondition NULL
		/if (${missionCompleteAction.Equal[CAMP]}) {
			/camp
			/endmacro
		} else /if (${missionCompleteAction.Equal[CAMPDESKTOP]}) {
			/camp desktop
			/endmacro
		} else /if (${missionCompleteAction.Equal[ENDMACRO]}) {
			/endmacro
		} else /if (${missionCompleteAction.Equal[IDLE]}) {
			/call MissionModeEnd "Mission Completed, CompleteAction is set to IDLE"
		} else /if (${missionCompleteAction.Equal[REPEAT]}) {
			/if (${missionRepeatCount} > ${missionRunCount} || !${misisonRepeatCount}) {
				/call MissionModeEnd "Ending the current mission in order to repeat it"
				/varset missionMode TRUE
				/varset missionWaitTimer ${Math.Calc[${missionRepeatMinDelay}+${Math.Rand[${missionRepeatMaxDelay} - ${missionRepeatMinDelay}]}]}${If[${missionRepeatDelayType.Length},${missionRepeatDelayType},]}
			} else {
				/call MissionModeEnd "We have ran the mission ${missionRunCount} times, repeat count set to ${missionRepeatCount}, going into IDLE mode now."
			}
		} else {
			| CompleteAction not recognized
			/call MissionModeEnd "Complete action ${missionCompleteAction} is not recognized, defaulting to IDLE"
		}
	} else /if (${missionStatus.Equal[ACTIONFAILED]}) {
		/if (${missionAmLeader}) /call MissionModeEnd "${If[${missionCurrentAction}!=0,Action${missionCurrentAction},An action]} has been failed."
	}
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

Sub Close_Packs
	/declare packLoop	int local

	/for packLoop 1 to 10
		/if (${Window[pack${packLoop}].Open}) /itemnotify ${Me.Inventory[pack${packLoop}].InvSlot} rightmouseup
	/next packLoop
/return

| ========================================================================================================
| v2.39 Update Notes
|  -Updated summoning routine to stop Inventory Item 'null' messages when summoning
|  -Added support for recognizing what a summoned item unfolds to for bots that destroy extra / empty unfolded packs after using whats in them.
|  -Disabled TradeCheck routine.  It hasn't worked correctly in a while and so was just pointlessly eating up cycles checking for trade window.
|    I hope to restor this functionality in the future, but the Invetory changes made a couple expansions ago may make that impossible without
|    some changes to MQ2 to fix it.
|  -Fixed a timing issue that was causing Unsafe pc checks to run without delay (essentially ignoring the priority setting for it) when out of combat.
|  -Added a #define HIGHERDEBUG /devecholog at the very top of devCommonPremium.  This will be used for any debug statements with a level higher 
|    than 0 (default level).  The purpose of this is for people that are seeing sluggishness, can disable higher level debug calls 
|    (which can be numerous and slower) by changing this line to #define HIGHERDEBUG /squelch
| 
| v2.38 Update Notes
|  -Updated typo in xtarget add check routine for npc pets
|  -Updated corpse check routine to only remove PC pets
|  -Updated references of XTarget[].Type to XTarget[].TargetType due to changes in MQ2.
| 
| v2.37 Update Notes
|  -Corrected typo in section name for UnsafePCImmediateAction setting (General vs Genearl)
|  -Updated mercenary state checks for TLO changes
| 
| v2.36 Update Notes
|  -Fixed a bug with group pet buffing that could prevent own pet buffing from working
|  -Added the ability to use an XTarget for a main assist
|  -New add detection routine that supports usage of any kind of xtarget, should be a little more efficient
|    New Hidden INI Setting [General] UseLAHoTT=TRUE, if this is set to FALSE it will not use the 
|     leadership ability health of targets target to determine if a mob is a true add (this method can be slow waiting on the hott)
|     Note: disabling it may slightly lower add detection reliability if not using xtargets, or xtargets are full
|  -Add detection routine will now detect NPC pets as well
|  -New Setting [General] UnsafePCImmediateAction=FALSE, if TRUE will take UnsafePCAction immediately in combat rather than waiting
|    until after combat
|  -New setting [Priorities] AssistFrequency=3s, determines how long to wait between checking assist targets (if bot is not the main assist)
|    and the bot is using the new CommonGetTarget routine
|  -New setting [Priorities] UnsafePCCheck=30s, how long between unsafe pc checks around your camp area.
|  -Added debugLvl to allow higher levels of debugging (not every where right now).  This defaults to 0 (debug outputs the same as currently)
|    It is set using [General] DebugLevel=0 (a hidden setting).  Currently only the pull routine is using anything beyond debug level 0
|    Note: [General] DebugMode=FALSE if set to TRUE will still simply make debug messages that would log only, appear in the MQ2ChatWnd
|  -Added some additional checks to handle zone changes.  Will auto set home to the location they are at when a zone change is detected
|    and will disable pull mode if enabled when a zone change is detected.
|  -Added a 30 second delay to buff checking if a buff check is exited early due to aggro
|  -New GetINISetting routine to allow auto create of variables if they do not exist rather than having to declare them seperately
|    (uses less memory this way)
|  -Unsafe PC Checks tweaked a little for better detection and handling.  Pull routine now checks back at camp location while pulling
|    and can be set to return to camp if an unsafe pc is detected, or to take the unsafe pc action where it is.  Added options to
|    allow for immediate action, even if in combat.  Be warned this will likely result in character deaths if you have it set to pause
|    or camp while you are in combat.  Don't forge the old hidden setting [General] UnsafePCRadius=300 for the radius to look for unsafe pcs
|  -Added version requirement checking to devPull.inc, pulling will be disabled if the version does not match the required version
| 
| v2.35 Update Notes
|  -Tweaked buff stacking to automatically allow stacking of equal rks of the same spell (mq2 reporst this as
|    unable to stack)
|  -Corrected a typo preventing pet buffs from working
|  -Added pull commands for global pull routine ('pull' enables / disables pulling, 'pullradius RADIUS' sets pull radius to RADIUS)
| 
| v2.34 Update Notes
|  -Corrected a problem with reagent detection for some spells causing them not to cast if they required no reagent
| 
| v2.33 Update Notes
|  -Changed it so classes with endurance will check endurance for combat spells
|  -Added a med mode that will not cast combat spells if it's in med mode (falls below a % mana until it gets
|    back up to a % mana) Settings: [General] MedModeManaThreshold=10 MedModeMedTo=80
|    If you do not want to use this setting just set the ManaThreshold to 0 and it will never hit it.
|    This will user Endurance in place of mana for endurance users
|  -Added Priority settings to make it so some things will be checked more often than others
|    [General] SpellCoolDown and MinBuffCheckTime are now obsolete and no longer used
|  -Added option [Debuffs] DebuffAllDebuffsAtOnce=TRUE/FALSE, if TRUE will cast all debuffs in the debuff routine
|    before going to another routine.  If FALSE will only cast one debuff each debuff check.  Note if you have
|    multiple debuffs and a long debuff priority then it will not cast the next debuff until the priority time
|  -Added option [Debuffs] DebuffAllDebuffsOneTarget=TRUE/FALSE.  This is a hidden setting and defaults to TRUE.
|    This option is only used if DebuffAllDebuffsAtOnce=TRUE.  If this option is TRUE it will only perform all debuffs
|    on one target regardless of the DebuffAdds setting.  Then it will wait to do the next target during the next debuff check.
|    If FALSE, it will debuff all debuffs, on all targets in camp before moving on to another routine (not recommend for healers).
|  -Added command 'castwhileinvis' to toggle casting while invis on / off
|  -Made some tweaks to the buff stacking checks to correct some problems in some situations like buffing from items
|  -Added new pull routine for all classes
|  -Added detection for MMOBugs compile to use some more advanced stuff from MMOBugs
|  -MMOBugs Only: Added auto detection of required reagents on buffs and combat spells.
|  
| v2.32 Update Notes
|  -Added disabling of merc usage if your account status is not sufficient to unsuspend the merc
|  -Corrected typo in add check routine that caused XTarget adds to be double checked with direction
|  -Updated friend list creation to be sure the pet master is being added if a pet is named after it's master
|  -Corrected a typo in the summoning routine that prevented spell based summoning from working
|  -Fixed a watchlist error dealing with Pet Type.
|  -Corrected a problem with Gift of Mana on spells higher than 95
|  -Added a check to disable autobuffs that you don't have the account status for (ie prestige items).
|  -Added additional corpse check to group buff routine
|  -MainAssist, MainTank, and Puller are now automatically considered friends if not set to group
|  -Added a cursor check to the common routine to auto inventory anything that may be on the cursor
|  -Buff stacking checks on ranks for spells that have a different Text than their buff name should now work
|  -Added new Buff settings for all buff types BuffXDoNotStack and BuffXStack.
|    BuffXDoNotStack - spell will not be allowed to stack with spells in this list
|    BuffXStack - spells that MQ2 reports will not stack, will still be allowed to stack if in this list
| 
| v2.31 Update Notes
|  -Updated summon item routine to work with discs and items to summon items
|  -Corrected a bug that prevented Target of Target being used when UseXTarget=FALSE but UseXTargetOnly=TRUE
|  -Updated summon item routine to check mana when summoning items
|  -Added hidden setting [General] LootOnlyKilledCorpses=TRUE/FALSE - if true will only loot corpses that
|    were detected ass adds and killed.  If false will attempt to loot all nearby corpses.  Defaults to TRUE
|  -Fixed a problem in add detection that could let the bot check an XTarget's target for add detection.
|  -Changed XTarget.Spawn to use Spawn[XTarget.ID] instead.
| 
| v2.30 Update Notes
|  -Updated ignore command to support multiple words in the ignore name
|  -Removed extra defaultGem declare
|  -Added checking to prevent adding a NULL value to the ignore list sometimes causing it to ignore all mobs
|  -Added a force target clear in the common combat routine if the target is a corpse.
|  -Added a SummonedItems section so that any class can use it to summon any regularly used and summoned items
| 
| v2.29 Update Notes
|  -Corrected a typo in GM detection routine that created some spam
|  -Added support for reprieve (still uses [General] RespitePct which defaults to 25)
|  -Added setting [General] MaxRezWaitTime=0 for the maximum amount of time to wait for a rez after death
|    before camping.  If 0 it will wait indefinitely.
|  -Added hidden ini setting [General] DefaultGem=GEM1 so you can change the default spell gem
|    if for any reason a spell needs to be memmed without a slot being specified
|  -Untargetable mobs in the zone will automatically be added to the ignore list now (added by name)
| 
| v2.28 Update Notes
|  -Changed merc managemenet code to account for MQ2 bug not properly reporting ${Me.Mercenary} state.
|  -Modified spell casting routine to better work with bards
|  -Changed INI loading to allow for up to 4 NULL values between entries in the INI so that it will not
|    stop loading a section for one NULL setting.
|  -All bots that can pull have had [Pull Settings] DefaultMinEnd added.
| 
| v2.27 Update Notes
|  -Corrected a typo when loading that prevented correct logging of CastWhileInvis state (did not affect
|    function of setting).
|  -Corrected a typo affecting rank 25 autobuff recognition
| 
| v2.26 Update Notes
|  -Added a /nav stop to the death detection routine to account for nav sometimes not stopping after death
|  -Added [General] CastWhileInvis=TRUE, set to FALSE to prevent casting while invis
|  -Updated xtarget limit to 10 to account for VoA AAs
|  -Fixed WatchBuffAtStart setting applying to the group buff variable instead of the watch buff variable
|  -Updated Gift of mana to support Ascendant (VoA level of GOM)
|  -Added support for GemTimer to combat spells, buffs, and debuffs
|  -Updated custom command triggers to support multiple word commands
| 
| v2.25 Update Notes
|  -Made some changes to make follow mode more dependable when used as an on the move bot that
|    regularly encounters combat.
|  -Fixed a problem with mob alerts not firing correctly
| 
| v2.24 Update Notes
|  -Added event to catch when MQ2Navigation can't work in a zone due to No Start reference error.
|  -Allowed discs with 0s duration to fire even when there is a disctimer
|  -Added equipmentSlot array for searching inventory with Me.Inventory.
|  -Added FindItemWithEffect(string effectName) routine to find items equipped with a specific effect.
|  -Fixed a problem with spell conditions parsing at load time instead of at the time of running.
|  -Fixed buff recast timer so that some single target spells like SoW will more reliably cast each buff check
| 
| v2.23 Update Notes
|  -Changed it so home location is updated while following to the current location to enable
|    better support for engaging in combat while following.
|  -Added setting [General] UnsafePCRadius=300, this is hidden by default.  Will change the range at
|    which unsafe pcs are detected
| 
| v2.22 Update Notes
|  -Fixed another condition check that could prevent adds from being detected while following on a char
|    or while soloing on the mageBot
|  -devLoot.inc updated to have /lootupdate aliased command
|  -corrected a problem that could prevent some beneficial ae combat spells from casting
|  -Fixed a problem preventing out of group characters from assisting due to inability to detect adds
|  -Prevented autobuff from trying to use an item that wasn't present on the character
|  -Added checks for recovery time in casting spells and buffs, also added checks for recast time in buffs
|  -Custom command actions should now parse variables in them when the command is issued
|  -Updated CheckForAggro routine to return the ID of the mob that you have aggro on
| 
| v2.21 Update Notes
|  -Fixed conflicting variable name dealing with modRods and the magebot2.
|  -Added setting [General] SecondaryAssist
|  -Added checks for higher ranks of buffs in the buff routine
| 
| v2.20 Update Notes
|  -Corrected WatchList loading to auto create the section if it is not created yet.
|  -Fixed add detection while in follow mode to detect around the character instead of around the home location.
|  -Corrected a problem with add detection when running solo mode on the mageBot.
|  -Corrected a problem preventing PB AE spells from being cast if the target was to close.
|  -Added support for using mod rods
|  -Fixed it so that MQ2Navigation would not attempt to load if UseNavigation was set to false
|  -Updated to devMQ2Cast_Spell_Routines.int to avoid future mmobugs update issues
|  -Made some changes to start adding support for running on emu servers
|  -Added a new INI setting [General] MinDebuffCheckTimer=5s
|  -Added variables ${TRUE} and ${FALSE} so that in line Ifs used in conditions with TRUE / FALSE being
|    returned will parse their conditions correctly.
| 
| v2.19 Update Notes
|  -Fixed a typo in casting routines with discs.
| 
| v2.18 Update Notes
|  -Added support for auto loading and using common spells / items / aa for buffing
|  -Moved EQDraw loading so that it will not auto load if Navigation is not being used.
|  -Modified add check to check distance from camp location instead of player location.
|  -Modified to automatically convert spells to Rk. II or Rk. III (requires MQ2Cast_Spell_Routines.inc change)
|    This means spells should no longer require Rk at the end of them in INIs to function correctly
| 
| v2.17 Update Notes
|  -Changed devMovement.inc to devMovementPremium.inc to avoid conflics with free bots from mmobugs
|  -Updated spell casting to support lvl 85 to 90 GOM spells
|  -Corrected a problem with the stay command not breaking follow
|  -Updated add check to accomodate checking for adds on a pet that is out of line of sight
| 
| v2.16 Update notes
|  -Added support for premium mageBot, warriorBot, rangerBot, and necroBot
| 
| v2.15 Update Notes
|  -Added support for new paladinbot
|  -Added support for healing potions for all bots [General] HealPotName=NULL HealPotPct=50
|  -Corrected debuff announcements not parsing %t for /bc
|  -Changed namedCount to work off # instead of Spawn.Named
|  
| v2.14 Update Notes
|  -Added INI setting [General] WatchBuffAtStart=TRUE/FALSE
|  -Corrected problem with some classes targetting watchlist members during buff routine unnecessarily
|  -Added maxXTargets variable and fixed all xtarget references to work with up to 7 XTargets
| 
| v2.13 Update Notes
|  -Changed spell casting to use endurance on classes that don't have mana for the SpellMinManaX setting.
|  -Updated the log message when buffing to correctly refelect the intended target
|  -Added a discTimer check in CommonCombatRoutines so that if a disc is no longer running it will not
|    prevent another disc from starting.
| 
| v2.12 Update Notes
|  -Added custom commands
|  -Added settings to turn off specific types of commands (bc, group, tell, echo)
|  -Added a check in WaitForRez routine, to continue operations if rez is accepted by another method.
|  -Changed EQBC announce messages to parse %t in announcemnts as your targets name
|  -Added respite ability support for all melee classes that have it, new INI setting [General] RespitePct=25
|  -Added TargetNamed variable to use in place of Target.Named
| 
| v2.11 Update Notes
|  -Corrected some combat buff issues
| 
| v2.10 Update Notes
|  -Updated for HoT changes
| 
| v2.09 Update Notes
|  -Improved compatability checks
|  -Corrected a problem with default merc burn % setting
| 
| v2.08 Update Notes
|  -Corrected a problem in the corpseCheck routine that would not remove charmed mobs that broke
|    more than once per combat.
| 
| v2.07 Update Notes
|  -Added aggro aborting to buff check routine
|  -Added merc management code
| 
| v2.06 Update Notes
|  -Fixed a phantom corpse getting added causing bots to not recognize the last mob in camp
|  -Fixed a bug where mercs where being revived and suspended immediately after
| 
| v2.05 Update Notes
|  -Added routine CombatCheck(bool AddCheck) for generic combat checking
| 
| v2.04 Update Notes
|  -Changed corpse check routine to also check for missing or charmed mobs
| 
| v2.03 Update Notes
|  -Fixed a problem that could prevent merc from assisting
| 
| v2.02 Update Notes
|  -Corrected a bug that could prevent a buff from auto buffing on a watch list member, do to FALSEly
|    thinking it would not stack with NULL.
| 
| v2.01 Update Notes
|  -Corrected a couple typos in the INI load routine that was preventing group buffs from working
|    as well as buff commands
| 
| v2.00 Update Notes
|  -New premium only version of devCommon.inc
| 
| ========================================================================================================


