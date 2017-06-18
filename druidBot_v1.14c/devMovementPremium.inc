| ========================================================================================================
| 					 devMovementPremium.inc V1.73
| 					Written By: Devestator
| 
| 
| Description:
|  Movement routines used in bots by Devestator
| 
| v1.73 Update Notes
|  -Tweaked some position check values
| 
| v1.72 Update Notes
|  -Updated to support loading paths from mission INIs
| 
| v1.71 Update Notes
|  -Updated to allow usage without devCommonPremium.inc
| 
| v1.70 Update Notes
|  -Corrected a problem preventing paths record with pathrecord.mac from loading with the new pull routines
| 
| v1.69 Update Notes
|  -Changed Navigate to Spawn logic and los checking to try and improve reliability
| 
| v1.68 Update Notes
|  -Updated ReadINIA to set maxLoc to 0 if it fails to load
| 
| v1.67 Update Notes
|  -Added GetHomeDistance routine to get the distance to your current home location more easily
|  -Corrected typo /delays in NavToLoc routine
|  -Fixed a problem with PosititionCheck failing to return to camp like it should in some situations
| 
| v1.66 Update Notes
|  -Added minimumDistance when using MQ2Navigation to not get any closer than if moving into los
| 
| v1.65 Update Notes
|  -Added support for using MQ2Navigation
| 
| v1.64 Update Notes
|  -Added code to handle facing the same direction as when started when returning to camp
| 
| v1.63 Update Notes
|  -Fixed a bug in the position check routine that could cause characters to break follow
| 
| v1.62 Update Notes
|  -Added PositionCheckDefault(float allowedDistance) routine for returning to camp location
| 
| V1.61 Update Notes
|  -Added the ability to initialize silenty
|  -Made it so that the movement will not get stuck trying to get to a spawn that no longer exists
| 
| V1.60 Update Notes
|  -Fixed a bug that was causing some problems with getting stuck due to not returning a proper values.
|  -Added Event summoned to devMovement.inc
| 
| V1.5 Update Notes
|  -Modified movement routines to return more information, will now exit movement if rooted.
| 
| V1.4 Update Notes
|  -Made it so that you dont have to remember to run in first person mode for doors to work
|   it will change to first person mode automatically if it detects a door.
|  
| V1.3 Update Notes
|  -Fixed a bug with MoveToSpawn that caused it to not recognize being stuck as fast as it should.
|  -Changed SetNearestLoc to attempt to be better at setting a loc nearby but also near your point on the path
|  -Made some other adjustments to stuck detection code
|  -Attempts to recognize when near a door and facing it, if so it will try to open the door, will only
|   work if you are running the macro in first personmode
| 
| V1.2 Update Notes
|  -Added maxMoveTimer.  If it takes longer than this to get to a location the move will cancel.
|  -Made some adjustments to z-axis handling
| 
| V1.1 Update Notes
|  -A few small code adjustments
|  -Changed MoveToLoc to be able to accept a Z axis value.
| 
| ========================================================================================================

#event summoned "You have been summoned!"

Sub MovementInit(bool initSilent,bool requireCommon)
	/if (!${Defined[requireCommon]}) /declare requireCommon bool local TRUE
	/declare moveRequireCommon			bool outer ${requireCommon}
	
	/declare reqDevCommon						float outer 2.48
	/if (${requireCommon} && (!${Defined[devCommonVer]} || ${devCommonVer} < ${reqDevCommon})) {
		/echo Cannot load devMovement.inc due to missing incompatible version: devCommon version: ${devCommonVer}, Needed version: ${reqDevCommon}
		/endm
	}
	/if (!${Defined[initSilent]}) /declare initSilent bool local FALSE
	
	/if (!${Defined[devMovementVer]}) {
		/declare devMovementVer 			float outer 1.73
		/declare moveCanceled 				bool outer FALSE
		/declare curLoc 							int outer 0
		/declare maxMoveTimer 				string outer 5m
		/declare lootReturnNearestLoc	bool outer TRUE
		/declare circuitRestTime			string outer 5m
		/declare allowMove						bool outer TRUE
		/declare LocArray[10000]			string outer	
	  /declare maxLoc								int outer	1
	  /declare movVar								int outer 1
		/declare circularPath 				bool outer FALSE
		
		/if (${missionMode} && ${missionINI.NotEqual[NULL]}) {
			/declare fileSearches[6]			string outer
			/varset fileSearches[1] ${missionINI}
			/varset fileSearches[2] path.ini
			/varset fileSearches[3] ${Zone.ShortName}.ini
			/varset fileSearches[4] ../MQ2AdvPath/${Zone.ShortName}.ini
			/varset fileSearches[5] ../${Zone.ShortName}.ini
			/varset fileSearches[6] advpath.ini
		} else {
			/declare fileSearches[5]			string outer
			/varset fileSearches[1] path.ini
			/varset fileSearches[2] ${Zone.ShortName}.ini
			/varset fileSearches[3] ../MQ2AdvPath/${Zone.ShortName}.ini
			/varset fileSearches[4] ../${Zone.ShortName}.ini
			/varset fileSearches[5] advpath.ini			
		}
	}
	/if (!${Defined[homeX]}) /declare homeX						float outer ${Me.X}
	/if (!${Defined[homeY]}) /declare homeY						float outer ${Me.Y}
	/if (!${Defined[homeZ]}) /declare homeZ						float outer ${Me.Z}
	/if (!${Defined[homeDir]}) /declare homeDir				float outer ${Me.Heading.DegreesCCW}
  /if (!${Defined[groupMode]}) /declare groupMode		bool outer true
  /if (!${Defined[useNavigaton]}) /declare useNavigation		bool outer FALSE
  /if (!${Defined[startZone]}) /declare startZone						string outer ${Zone.ShortName}
  /if (!${Defined[startZoneID]}) /declare startZoneID				int outer ${Zone.ID}
  
  /if (!${initSilent}) /call moveLogWrapper "Initializing devMovementPremium.inc v${devMovementVer} - Written By Devestator"
		
	/squelch /plugin MQ2Debuffs
/return

Sub MoveToSpawn(int MoveToID, int StopDistance, bool inCombatMove, bool overRideNavigate, bool waitForLOS, int minimumDistance) 
	/if (!${Defined[MoveToID]} || !${Spawn[${MoveToID}].ID}) { 
		/mqlog MoveToSpawn:  Spawn ID not found or no ID provided. Aborting... 
		/return ABORT_NOSPAWN
	} 
  /if (!(${Defined[StopDistance]})) { 
		/mqlog MoveToSpawn:  Stopping point not defined, using default distance of 20
    /declare StopDistance int local 
    /varset StopDistance 20
  }
  /if (!${Defined[inCombatMove]}) /declare inCombatMove bool local FALSE
	/if (!${Defined[overRideNavigate]}) /declare overRideNavigate bool local FALSE
	/if (!${Defined[waitForLOS]}) /declare waitForLOS bool local FALSE
  /if (!${Defined[minimumDistance]}) /declare minimumDistance float local 0
  
  /declare running						int local 0
  /declare distanceNow				float local 
  /declare distanceBefore			float local ${Spawn[${MoveToID}].Distance} 
  /declare distanceModifier		int local 1
  /declare distanceTimer			timer 15 
  /declare strafeAttempts			int local 0
  /declare doorAttempt				bool local FALSE
  /declare moveTimer					timer local
  /declare spawnType					string local ${Spawn[${MoveToID}].Type}
  
  /varset moveCanceled FALSE

  /if (${useNavigation} && !${overRideNavigate} && (${Spawn[${MoveToID}].Distance} > 70 || !${Spawn[${MoveToID}].LineOfSight})) {
  	/call NavigateToSpawn ${MoveToID} ${StopDistance} ${inCombatMove} ${waitForLOS} ${minimumDistance}
  	/return ${Macro.Return}
  }  
  
  /varset moveTimer ${maxMoveTimer}
  
  :moveToSpawn
  /if (!${Spawn[${MoveToID}].ID} || ${Spawn[${MoveToID}].Type.NotEqual[${spawnType}]}) {
  	/keypress forward
  	/return COMPLETED_SUCCESS
  }
	/if (${Me.Underwater} || ${Me.FeetWet}) { 
		/squelch /face fast id ${MoveToID}
  } else {
		/squelch /face fast nolook id ${MoveToID} 
	}

  /if (${Spawn[${MoveToID}].Distance}<${StopDistance} && (!${waitForLOS} || (${waitForLOS} && ${Spawn[${MoveToID}].LineOfSight}))) { 
    /keypress forward 
    /return COMPLETED_SUCCESS
  } else {
		/if (!${moveTimer}) { 
			/mqlog MoveToSpawn:  It has taken to long to get to the spawn, move canceled | ${Spawn[${MoveToID}].CleanName} | ${MoveToID}
			/varset moveCanceled true
			/keypress forward
			/return ABORT_TIMEOUT
		}
  }
  
	/if (${Me.CombatState.Equal[COMBAT]} && !${inCombatMove}) {
		/mqlog MoveToLoc:  Something attacked us while moving, stopping movement.
		/keypress forward
		/varset moveCanceled true
		/return ABORT_COMBAT
	}  
    
  /if (!${distanceTimer}) { 
    /if (${Me.Sneaking}) { 
        /varset distanceModifier 2 
    } else { 
        /varset distanceModifier 1 
    } 

    /varset distanceNow ${Spawn[${MoveToID}].Distance} 
    /if (${Math.Calc[${distanceBefore}-${distanceNow}]}<${Math.Calc[5/${distanceModifier}]} && ${distanceBefore} && ${distanceNow} > ${StopDistance}) {
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
					/mqlog MoveToSpawn:  Cannot get to spawn | ${Spawn[${MoveToID}].CleanName} | ${MoveToID}
					/varset moveCanceled TRUE
					/keypress forward
					/mqlog MoveToSpawn:  Movement aborted due to being stuck.
					/return ABORT_STUCK
				} else /if (!${doorAttempt} && ${DoorTarget.Name.Length} && ${DoorTarget.Distance} < 15) {
					/keypress FIRST_PERSON_CAMERA
					/squelch /doortarget
					/nomodkey /keypress U hold
					/delay 2
					/nomodkey /keypress U
					/varset doorAttempt TRUE
				} else {
					/call strafePastObstacle 
					/varcalc strafeAttempts ${strafeAttempts}+1
				}
			} else /if (${Debuff.Rooted}) {
				/mqlog MoveToSpawn:  Movement aborted due to being rooted
				/return ABORT_ROOT
			}         
    }
    
    /varset distanceBefore ${Spawn[${MoveToID}].Distance} 
    /varset distanceTimer 20 
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
	/goto :moveToSpawn 
/return COMPLETED_SUCCESS

Sub MoveToLoc(float MoveToY, float MoveToX, bool inCombatMove, float MoveToZ, bool overRideNavigate)
	/declare running 			int local  0
	/declare distanceNow 		float local 
	/declare distanceBefore 	float local 
	/declare distanceModifier 	int local 1
	/declare strafeAttempts		int local 0
	/declare doorAttempt			bool local false
	/declare distanceTimer		timer 0 
	/declare meZ				float local
	/declare moveZ				float local
	/declare useZAxis			bool local
	/declare moveTimer			timer local
	
	/if (!${Defined[inCombatMove]}) /declare inCombatMove bool local false
	/if (!${Defined[MoveToY]} || !${Defined[MoveToX]}) {
		/mqlog The y and or x coordinates were not specified to move to, not attempting movement.
		/return ABORT_NOCOORDS
	}
	/if (!${Defined[MoveToZ]} || ${MoveToZ}==0) {
		/if (!${Defined[MoveToZ]}) /declare MoveToZ float local 0
		/varset useZAxis FALSE
		/varset meZ 0
		/varset moveZ 0
	} else {
		/varset useZAxis TRUE
		/varset meZ ${Me.Z}
		/varset moveZ ${MoveToZ}
	}
	/if (!${Defined[overRideNavigate]}) /declare overRideNavigate bool local FALSE
	
  /varset moveCanceled false
  
  /if (${useNavigation} && !${overRideNavigate}) {
  	/call NavigateToLoc ${MoveToX} ${MoveToY} ${MoveToZ} ${inCombatMove}
  	/return ${Macro.Return}
  }
  
	/varset moveTimer ${maxMoveTimer}
	
  :moveToLocation 
	/if (${Me.CombatState.Equal[COMBAT]} && !${inCombatMove}) {
		/mqlog MoveToLoc:  Something attacked us while moving, stopping movement.
		/keypress forward
		/varset moveCanceled true
		/return ABORT_COMBAT
	}
	/if (!${useZAxis}) {
		/face fast nolook loc ${MoveToY},${MoveToX} 
	} else {
		/if (${Me.Underwater} || ${Me.FeetWet}) {
			/face fast loc ${MoveToY},${MoveToX},${MoveToZ}
		} else {
			/face fast nolook loc ${MoveToY},${MoveToX},${MoveToZ}
		}
		/varset meZ ${Me.Z}
	}

  /if (${Math.Distance[${Me.Y},${Me.X},${meZ}:${MoveToY},${MoveToX},${moveZ}]}<6) { 
    /keypress forward 
    /return COMPLETED_SUCCESS
  } else /if (!${moveTimer}) {
		/mqlog We have been trying to get to the same location for ${maxMoveTimer}, giving up.
		/keypress forward
		/varset moveCanceled true
		/return ABORT_TIMEOUT
	}

  /if (!${distanceTimer}) { 
    /if (${Me.Sneaking}) { 
        /varset distanceModifier 2 
    } else { 
        /varset distanceModifier 1 
    } 

		/varset distanceNow ${Math.Distance[${Me.Y},${Me.X},${meZ}:${MoveToY},${MoveToX},${moveZ}]} 
		/if (${Math.Calc[${distanceBefore}-${distanceNow}]}<${Math.Calc[6/${distanceModifier}]} && ${distanceBefore}) { 
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
					/mqlog MoveToLoc:  Movement aborted due to being stuck.
					/keypress forward
					/varset moveCanceled true
					/return ABORT_STUCK
				} else /if (!${doorAttempt}) {
					/keypress FIRST_PERSON_CAMERA
					/squelch /doortarget
					/nomodkey /keypress u hold
					/delay 2
					/nomodkey /keypress u
					/varset doorAttempt true
				} else {				
					/call strafePastObstacle 
					/varcalc strafeAttempts ${strafeAttempts}+1
				}
			} else /if (${Debuff.Rooted}) {
				/mqlog MoveToLoc:  Movement aborted due to being rooted.
				/return ABORT_ROOT
			}
		}
		
		/if (${useZAxis} && ${Math.Calc[${distanceBefore}-${distanceNow}]}>30 && ${distanceBefore}) {
			/mqlog MoveToLoc:  Movement aborted due to large Z change.
			/keypress forward
			/varset moveCanceled true
			/return ABORT_ZAXIS
		}
		
		/varset distanceBefore ${Math.Distance[${Me.Y},${Me.X},${meZ}:${MoveToY},${MoveToX},${moveZ}]} 
    /varset distanceTimer 15 
  } 

  /if (!${running}) { 
    /keypress forward 
    /if (${Math.Distance[${Me.Y},${Me.X},${meZ}:${MoveToY},${MoveToX},${moveZ}]}>6) { 
      /varset running 1 
      /keypress forward hold 
    } 
  } else { 
    /if (${Math.Distance[${Me.Y},${Me.X},${meZ}:${MoveToY},${MoveToX},${moveZ}]}<=6) { 
      /varset running 0 
      /keypress forward 
    } 
  }
	/goto :moveToLocation 
/return COMPLETED_SUCCESS

Sub strafePastObstacle
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

Sub Movement(int Start,int Finish,bool CombatMove,bool ignoreZ,bool fastFace) 
	/declare Location	int inner 
	/declare locX		float local
	/declare locY		float local
	/declare locZ		float local
	/declare locStr		string local
	/declare locStrZ	string local
	/declare retValue	string local
	/declare moveZoneID	int local ${Zone.ID}
	/declare movePauseTimer	timer local 0s
	/declare reverseMove		bool local FALSE
	
	/if (!${Defined[CombatMove]}) /declare CombatMove bool local FALSE
	/if (!${Defined[ignoreZ]}) /declare ignoreZ bool local FALSE
	/if (!${Defined[fastFace]}) /declare fastFace bool local FALSE
	/if (${curLoc} > ${Finish}) /varset reverseMove TRUE
	/if (${Me.Feigning} || ${Me.Sitting}) /stand
	
	/for Location ${Start} to ${Finish}
		:reverseInstruct
			/varset locStr ${LocArray[${Location}]}
			/if (${reverseMove} && (${locStr.Arg[1,,].Equal[INSTRUCTION]} || ${locStr.Equal[IGNORE]})) {
				/if (${locStr.Arg[2,,].Equal[DOOR]}) {
					/doortarget
					/face fast door
					/click left door
					/if (${LocArray[${Math.Calc[${Location}+1]}].Arg[2,,].Equal[PAUSE]}) {
						/varset movePauseTimer ${LocArray[${Math.Calc[${Location}+1]}].Arg[3,,]}
						:reverseMovePauseLoop
							| /doevents
							/if (${Me.CombatState.Equal[COMBAT]} && !${CombatMove}) {
								/call moveLogWrapper "MoveToLoc:  Something attacked us while moving, stopping movement."
								/return ABORT_COMBAT
							}
						/if (${movePauseTimer}) /goto :reverseMovePauseLoop						
					}
				}
				/varcalc Location ${Location} - 1
				/goto :reverseInstruct
			}

		/if (${locStr.Arg[1,,].Equal[INSTRUCTION]} || ${locStr.Equal[IGNORE]}) {
			/if (${locStr.Arg[2,,].Equal[DOOR]}) {
				/doortarget
				/face fast door
				/click left door
			} else /if (${locStr.Arg[2,,].Equal[ITEM]}) {
				/itemtarget "${locStr.Arg[3,,]}"
				/face fast item
				/click left item
			} else /if (${locStr.Arg[2,,].Equal[ZONECHANGE]}) {
				/delay 30s ${Zone.ID}!=${locStr.Arg[3,,]}
				/delay 30s ${Zone.ID}==${locStr.Arg[4,,]}
				/if (${Zone.ID}==${locStr.Arg[4,,]}) {
					/varset moveZoneID ${Zone.ID}
				} else {
					/call moveLogWrapper "Zoned into an unexpected zone, aborting movement"
					/return ABORT_WRONG_ZONE
				}
			} else /if (${locStr.Arg[2,,].Equal[PAUSE]}) {
				/varset movePauseTimer ${locStr.Arg[3,,]}
				:movePauseLoop
					| /doevents
					/if (${Me.CombatState.Equal[COMBAT]} && !${CombatMove}) {
						/call moveLogWrapper "MoveToLoc:  Something attacked us while moving, stopping movement."
						/return ABORT_COMBAT
					}
				/if (${movePauseTimer}) /goto :movePauseLoop
			} else /if (${locStr.Arg[2,,].Equal[FACE]}) {
				/face heading ${locStr.Arg[3,,]}
			} else /if (${locStr.Arg[2,,].Equal[UI]}) {
				/notify ${locStr.Arg[3,,]} ${locStr.Arg[4,,]} ${locStr.Arg[5,,]} ${locStr.Arg[6,,]} ${locStr.Arg[7,,]}
			} else /if (${locStr.Arg[2,,].Equal[CLICK]}) {
				/click left center
			}
			/if (${Finish}<${maxLoc}) /varcalc Finish ${Finish} + 1
		} else {
			:MoveLoop 
				/face ${If[${fastFace},fast ,]}nolook loc ${LocArray[${Location}]} 
				/if (${Math.Distance[${LocArray[${Location}]}]}>6) { 
					/varset locY ${locStr.Left[${Math.Calc[${locStr.Find[,]}-1]}]}
					/varset locStrZ ${locStr.Right[${Math.Calc[${locStr.Length}-${locStr.Find[,]}]}]}
					/if (${locStrZ.Find[,]}) {
						/varset locX ${locStrZ.Left[${Math.Calc[${locStrZ.Find[,]}-1]}]}
						/varset locZ ${locStrZ.Right[${Math.Calc[${locStrZ.Length}-${locStrZ.Find[,]}]}]}
						/if (!${ignoreZ}) {
							/call MoveToLoc ${locY} ${locX} ${CombatMove} ${locZ}
						} else {
							/call MoveToLoc ${locY} ${locX} ${CombatMove}
						}
					} else {
						/varset locX ${locStr.Right[${Math.Calc[${locStr.Length}-${locStr.Find[,]}]}]}
						/call MoveToLoc ${locY} ${locX} ${CombatMove}
					}
				} 
				/if (${Math.Distance[${LocArray[${Location}]}]}<=6) {
					/varset curLoc ${Location}
					/nomodkey /keypress forward 
					/goto :nextMoveLoc
				} else {
					/if (${Macro.Return.Length}) {
						/varset retValue ${Macro.Return}
					} else {
						/varset curLoc ${Location}
						/varset retValue COMPLETED_SUCCESS
					}
					/return ${retValue}
				}
			/goto :MoveLoop 
		}
		:nextMoveLoc
	/next Location 
	/if (${Macro.Return.Length}) {
		/varset retValue ${Macro.Return}
	} else {
		/varset retValue COMPLETED_SUCCESS
	}
/return ${retValue}

Sub CheckPathExists(PathName,FileName)
	/if (!${Defined[FileName]}) /declare FileName		string local xxx
	
	/call LoadPath ${PathName} ${FileName} FALSE
/return ${Macro.Return}

Sub LoadPath(PathName,FileName,ReadInPath)
	/declare fpArray				int local 0
	/declare pathFound			bool local FALSE
	/if (!${Defined[ReadInPath]}) /declare ReadInPath		bool local TRUE
	
	| Attempt to find the path
	/if (${Defined[FileName]} && ${FileName.NotEqual[NULL]}) {
		/if (${Ini[${FileName},${PathName},1,NULL].NotEqual[NULL]} || ${Ini[${FileName},${PathName},Loc1,NULL].NotEqual[NULL]}) {
			/varset pathFound TRUE
			/if (${ReadInPath}) {
				/call ReadPath ${FileName} ${PathName}
				/return ${Macro.Return}
			}
		}	
	}
	
	/if (!${pathFound}) {
		/for fpArray 1 to ${fileSearches.Size}
			/if (${Ini[${fileSearches[${fpArray}]},${PathName},1,NULL].NotEqual[NULL]} || ${Ini[${fileSearches[${fpArray}]},${PathName},Loc1,NULL].NotEqual[NULL]}) {
				/varset pathFound TRUE
				/if (${ReadInPath}) {
					/call ReadPath ${fileSearches[${fpArray}]} ${PathName}
					/return ${Macro.Return}
				}
			}			
		/if (!${pathFound}) /next fpArray
	}
	
	/if (!${ReadInPath}) /return ${pathFound}
	/if (!${pathFound}) /call moveLogWrapper "/ag${PathName} /aycould not be found to load."
/return FALSE

Sub ReadPath(FileName,SectionName)
	/declare nValues     	int local 
	/declare nMArray      int local  0
	/declare varPrefix		string local Loc
	/declare iniValue			string local
	
	/varset nValues ${Ini[${FileName},${SectionName},Value,0]}
	
	| It is an AdvPath path
	/if (!${nValues}) /varset varPrefix 

	:MakeArraylo 
	/varset maxLoc 0
	/for nMArray 1 to 10000
		/if (${Ini[${FileName},${SectionName},${varPrefix}${nMArray},NULL].Equal[NULL]}) /varcalc maxLoc ${nMArray} - 1
	/if (!${maxLoc}) /next nMArray
	
	/for nMArray 1 to ${maxLoc}
		/varset LocArray[${nMArray}] ${Ini[${FileName},${SectionName},${varPrefix}${nMArray}].Replace[ ,,]}
		|**/varset iniValue ${Ini[${FileName},${SectionName},${varPrefix}${nMArray}]}
		/if (${iniValue.Arg[1,,].Equal[INSTRUCTION]}) {
			/varset LocArray[${nMArray}] ${iniValue}
		} else {
			/varset LocArray[${nMArray}] ${iniValue.Replace[ ,,]}
		}**|
	/next nMArray
	
	/varset circularPath ${If[${Math.Distance[${LocArray[1]}:${LocArray[${maxLoc}]}]} < 75,TRUE,FALSE]}
/return TRUE

Sub ReadINIA(FileName,SectionName)
	/if (${Ini[${FileName},${SectionName},-1,NO].Equal[NO]}) { 
	  /mqlog "${SectionName}" is not a Valid Section for FILE:${FileName}, ending read
	  /return 
	} 
	/declare nValues     int local 
	/declare nMArray      int local  0
	
	/varset nValues ${Ini[${FileName},${SectionName},Value]} 
	:MakeArraylo 
	/varset maxLoc ${nValues}
	/if (!${nValues}) /return 
	/for nMArray 1 to ${nValues} 
	  /varset LocArray[${nMArray}] ${Ini[${FileName},${SectionName},Loc${nMArray}]} 
	/next nMArray
	/if (${Math.Distance[${LocArray[1]}:${LocArray[${maxLoc}]}]} < 50) /varset circularPath true
/return

Sub SetNearestLoc(bool searchAll)
	/declare locLoop	int local
	/declare cLoc		int local
	/declare cDistance	int local
	/declare mDistance	int local
	/declare maLoc		int local
	/declare miLoc		int local
	/if (!${Defined[searchAll]}) /declare searchAll bool local false
	
	/varset cDistance 10000
	/varset cLoc ${curLoc}
	/if (!${searchAll}) {
		/varset maLoc ${Math.Calc[${curLoc}+5]}
		/varset miLoc ${Math.Calc[${curLoc}-5]}
		/if (${maLoc} > ${maxLoc}) /varset maLoc ${maxLoc}
		/if (${miLoc} < 1) /varset miLoc 1
	} else {
		/varset miLoc 1
		/varset maLoc ${maxLoc}
	}
	

	/for locLoop ${miLoc} to ${maLoc}
		/if (${LocArray[${locLoop}].Arg[1,,].NotEqual[INSTRUCTION]} && ${LocArray[${locLoop}].Arg[1,,].NotEqual[IGNORE]}) {
			/varset mDistance ${Math.Distance[${LocArray[${locLoop}]}]}
			/if (${mDistance} < ${cDistance}) {
				/varset cDistance ${mDistance}
				/varset cLoc ${locLoop}
			}
		}
	/next locLoop

	/varset curLoc ${cLoc}
/return

Sub SetFurthestLoc(bool searchAll)
	/declare locLoop	int local
	/declare cLoc		int local
	/declare cDistance	int local
	/declare mDistance	int local
	/declare maLoc		int local
	/declare miLoc		int local
	/if (!${Defined[searchAll]}) /declare searchAll bool local TRUE
	
	/varset cDistance 0
	/varset cLoc ${curLoc}
	/if (!${searchAll}) {
		/varset maLoc ${Math.Calc[${curLoc}+5]}
		/varset miLoc ${Math.Calc[${curLoc}-5]}
		/if (${maLoc} > ${maxLoc}) /varset maLoc ${maxLoc}
		/if (${miLoc} < 1) /varset miLoc 1
	} else {
		/varset miLoc 1
		/varset maLoc ${maxLoc}
	}
	

	/for locLoop ${miLoc} to ${maLoc}
		/varset mDistance ${Math.Distance[${LocArray[${locLoop}]}]}
		/if (${mDistance} > ${cDistance}) {
			/varset cDistance ${mDistance}
			/varset cLoc ${locLoop}
		}
	/next locLoop

	/varset curLoc ${cLoc}
/return

Sub Event_summoned
	/echo >> We have been summoned, attempting to get to safety.
	/mqlog We have been summoned, attempting to get to safety.
	/if (${groupMode}) {
		/if (${Math.Distance[${Me.Y},${Me.X}:${homeY},${homeX}]}<${Math.Calc[${campRadius}*2]} || ${useNavigation}) {
			/call MoveToLoc ${homeY} ${homeX} true
			/if (${moveCanceled} && !${Me.CombatState.Equal[COMBAT]}) {
				/call moveLogWrapper "Could not get back to home location after being summoned, resetting home location to where we are now"
				/call SetHome
			}
		}
	} else {
		/call SetNearestLoc
	}
/return

Sub PositionCheckDefault(float allowedDistance,bool ignorePuller,float maxAllowed)
	/declare returnTimer 			timer local 0s
	/if (${Zone.ID}!=${startZoneID}) {
		/call moveLogWrapper "\ayWe have changed zones, setting home location to current location."
		/call SetHome
		/varset startZoneID ${Zone.ID}
		/varset startZone ${Zone.ShortName}
		/if (${Defined[pullMode]} && ${pullMode}) {
			/call moveLogWrapper ".....Disabling pull mode due to zone change."
			/varset pullMode FALSE
		}
	}
	/if (!${Defined[ignorePuller]}) /declare ignorePuller		bool local TRUE
	/if (!${Defined[allowedDistance]}) /declare allowedDistance		float local ${Math.Calc[${campRadius}*${campReturnRatio}]}
	/if (!${Defined[maxAllowed]}) /declare maxAllowed				float local 300 
	| ${Math.Calc[${allowedDistance} * 3]}
	/if (${maxAllowed}<${allowedDistance}) /varcalc maxAllowed ${allowDistance}*2
	/if (!${followMode} && (${ignorePuller} || !${Defined[pullMode]} || !${pullMode}) && ${Math.Distance[${homeY},${homeX}]} > ${allowedDistance}) {
		/if (${Math.Distance[${homeY},${homeX}]} < ${maxAllowed} || ${useNavigation} || ${alwaysAttemptReturn}) {
			/call moveLogWrapper "We are to far away from our home location, returning to it." TRUE
			/if (!${inCombat}) {
				/varset returnTimer ${Math.Calc[${Math.Rand[5]}+1]}s
				:returnWait
					/doevents
					/call CheckForAdds ${campRadius} ${Me.ID} FALSE
				/if (${returnTimer} && !${lTargCount}) /goto :returnWait
			}
			/call MoveToLoc ${homeY} ${homeX} TRUE
			/if (${Defined[useHomeDir]} && ${useHomeDir}) /face heading ${homeDir}
		} else {
			/call moveLogWrapper "\ayWe are to far away from our home location to return to it.  Setting a new home location to current location"
			/call SetHome
		}
	} else /if (!${followMode} && !${inCombat} && (${ignorePuller} || ${pullerName.NotEqual[${Me.CleanName}]} || (${Defined[pullPath]} && ${Defined[maxLoc]} && ${maxLoc} > 1 && ${pullPath.NotEqual[NULL]}))) {
		/if (${Defined[useHomeDir]} && ${useHomeDir}) /face Heading ${homeDir}
	}
/return

Sub NavigateToLoc(float navX, float navY, float navZ, bool combatMove)
	/nav ${navX} ${navY} ${navZ} dist 6
	/delay 1s ${Navigation.Active}
	:navToLocation
		/doevents
		/if (${Me.CombatState.Equal[Combat]} && !${combatMove}) {
			/nav stop
			/varset moveCanceled TRUE
			/return ABORT_COMBAT
		}
	/if (${Navigation.Active}) {
		/goto :navToLocation
	} else /if ((${navZ}!=0 && ${Math.Distance[${navY},${navX},${navZ}]} > 20) || (${navZ}==0 && ${Math.Distance[${navY},${navX}]} > 20)) {
		/call moveLogWrapper "\ayNavigateToLoc: MQ2Navigation Aborted move early for unknown reason, attempting to complete move with old style movement."
		/call MoveToLoc ${navY} ${navX} ${combatMove} ${navZ} TRUE
	}
	/if (${Navigation.Active}) {
		/nav stop
		/delay 5
	}
/return NAV_COMPETED

Sub NavigateToSpawn(int navSpawnID, float stopDistance, bool combatMove, bool waitForLOS, float minimumDistance)
	/declare updateTimer			timer local 2s
	/declare lastX						float local ${Spawn[${navSpawnID}].X}
	/declare lastY						float local ${Spawn[${navSpawnID}].Y}
	/declare lastZ						float local ${Spawn[${navSpawnID}].Z}
	/declare losTimer					timer local 10m
	/declare losTimerSet			bool local FALSE
	/declare spawnDistance		float local ${Math.Distance[${Spawn[${navSpawnID}].Y},${Spawn[${navSpawnID}].X},${Spawn[${navSpawnID}].Z}]}
	/nav ${Spawn[${navSpawnID}].X} ${Spawn[${navSpawnID}].Y} ${Spawn[${navSpawnID}].Z}
	| /nav id ${navSpawnID} dist 10
	:navToSpawn
		/doevents
		/if (!${Spawn[${navSpawnID}].ID}) {
			/nav stop
			/return ABORT_NOSPAWN
		}
		/if (!${updateTimer} && ${Math.Distance[${lastY},${lastX},${lastZ}:${Spawn[${navSpawnID}].Y},${Spawn[${navSpawnID}].X},${Spawn[${navSpawnID}].Z}]} > 15) {
			/nav ${Spawn[${navSpawnID}].X} ${Spawn[${navSpawnID}].Y} ${Spawn[${navSpawnID}].Z}
			| /nav id ${navSpawnID} dist ${stopDistance}
			/varset lastX ${Spawn[${navSpawnID}].X}
			/varset lastY ${Spawn[${navSpawnID}].Y}
			/varset lastZ ${Spawn[${navSpawnID}].Z}
			/varset updateTimer 2s
		}
		/if (${Me.CombatState.Equal[Combat]} && !${combatMove}) {
			/nav stop
			/varset moveCanceled TRUE
			/return ABORT_COMBAT
		}
		
		/varset spawnDistance ${Math.Distance[${Spawn[${navSpawnID}].Y},${Spawn[${navSpawnID}].X},${Spawn[${navSpawnID}].Z}]}
		/if (${Navigation.Active}) {
			/if (${spawnDistance} < ${stopDistance}) {
				/if (${waitForLOS}) {
					/if (${Spawn[${navSpawnID}].LineOfSight} && !${losTimer}) {
						/nav stop
						/return NAV_COMPLETED
					} else /if (${Spawn[${navSpawnID}].LineOfSight} && !${losTimerSet}) {
						/varset losTimer 2s
						/varset losTimerSet TRUE
					}
				} else {
					/nav stop
					/return NAV_COMPLETED
				}
			}
		} else {
			/if (${spawnDistance} > ${stopDistance}) {
				/if (${Spawn[${navSpawnID}].LineOfSight}) {
					/call MoveToSpawn ${navSpawnID} ${stopDistance} ${combatMove} TRUE
					/varset spawnDistance ${Math.Distance[${Spawn[${navSpawnID}].Y},${Spawn[${navSpawnID}].X},${Spawn[${navSpawnID}].Z}]}
					/if (${spawnDistance} > ${stopDistance}) {
						/call moveLogWrapper "\ayNavigateToSpawn: MQ2Navigation aborted move early for unknown reason.  And unable to get within range using non navigation movement.  Movement aborted."
						/return NAV_ABORTED
					} else {
						/return NAV_COMPLETED
					}
				} else {
					/call moveLogWrapper "\ayNavigateToSpawn: MQ2Navigation aborted move early for unknown reason.  Aborting movement due to not being able to see the spawn we are trying to move to."
					/return NAV_ABORTED
				}
			} else {
				/if (!${waitForLOS} || (${waitForLOS} && ${Spawn[${navSpawnID}].LineOfSight})) {
					/return NAV_COMPLETED
				} else {
					/call MoveToSpawn ${navSpawnID} 5 ${combatMove} TRUE
					/if (${Spawn[${navSpawnID}].LineOfSight}) /return NAV_COMPLETED
				}
			}
		}
	/goto :navToSpawn
/return NAV_UNKNOWN

Sub SetHome(float newHomeX,float newHomeY,float newHomeZ,float newHomeDir)
	/if (!${Defined[newHomeX]}) /declare newHomeX float local ${Me.X}
	/if (!${Defined[newHomeY]}) /declare newHomeY float local ${Me.Y}
	/if (!${Defined[newHomeZ]}) /declare newHomeZ float local ${Me.Z}
	/if (!${Defined[newHomeDir]}) /declare newHomeDir float local ${Me.Heading.DegreesCCW}

	/varset homeX ${newHomeX}
	/varset homeY ${newHomeY}
	/varset homeZ ${newHomeZ}
	/varset homeDir ${newHomeDir}
/return

Sub GetHomeDistance
	/declare homeDistance
	/varset homeDistance ${Math.Distance[${homeY},${homeX},${homeZ}]}
/return ${homeDistance}

Sub moveLogWrapper(string eMsg, bool logOnly, int msgLevel)
	/if (!${Defined[logOnly]}) /declare logOnly bool local FALSE
	/if (!${Defined[msgLevel]}) /declare msgLevel int local 0
	/if (${moveRequireCommon}) {
		/call EchoLog "${eMsg}" ${logOnly} ${msgLevel}
	} else {
		/if (!${logOnly}) /echo ${eMsg}
		/mqlog ${eMsg}
	}
/return