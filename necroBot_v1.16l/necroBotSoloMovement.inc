| ========================================================================================
| 														necroSoloMovement
| 														Written By: Devestator
| 
| USAGE:
|  Special movement routines specifically for necro solo mode right now
| 
| 
| ========================================================================================

Sub NecroSoloMovement(int Start,int Finish,bool CombatMove,bool ignoreZ)
	/if (!${Defined[maxMoveTimer]}) /call NecroSoloMovementInit
	/declare Location	int inner 
	/declare locX		float local
	/declare locY		float local
	/declare locZ		float local
	/declare locStr		string local
	/declare locStrZ	string local
	/declare retValue	string local
	/declare forType	string local to
	
	/if (!${Defined[CombatMove]}) /declare CombatMove bool local false
	/if (!${Defined[ignoreZ]}) /declare ignoreZ bool local false
	/if (${Start} > ${Finish}) /varset forType downto
	
	/varset InCombatMove ${CombatMove}
	/varset StopMoving FALSE
	/varset StopDistance 6
	/for Location ${Start} ${forType} ${Finish} 
		/if (${Location}==${Finish}) /varset StopMoving TRUE
		/varset locStr ${LocArray[${Location}]}
		:MoveLoop 
			/if (${Location}==${Start}) {
				| /echo ${LocArray[${Location}]}
				/face nolook loc ${LocArray[${Location}]}
			}
			/if (${Math.Distance[${LocArray[${Location}]}]}>6) { 
				/varset locY ${locStr.Left[${Math.Calc[${locStr.Find[,]}-1]}]}
				/varset locStrZ ${locStr.Right[${Math.Calc[${locStr.Length}-${locStr.Find[,]}]}]}
				/if (${locStrZ.Find[,]}) {
					/varset locX ${locStrZ.Left[${Math.Calc[${locStrZ.Find[,]}-1]}]}
					/varset locZ ${locStrZ.Right[${Math.Calc[${locStrZ.Length}-${locStrZ.Find[,]}]}]}
					/if (!${ignoreZ}) {
						/call NecroSoloMoveToLoc ${locY} ${locX} forward ${locZ}
					} else {
						/call NecroSoloMoveToLoc ${locY} ${locX} forward
					}
				} else {
					/varset locX ${locStr.Right[${Math.Calc[${locStr.Length}-${locStr.Find[,]}]}]}
					/call NecroSoloMoveToLoc ${locY} ${locX} forward
				}
			} 
			/if (${Math.Distance[${LocArray[${Location}]}]}<=6) { 
				/nomodkey /keypress forward
				/goto :next 
			} else {
				/if (${Macro.Return.Length}) {
					/varset retValue ${Macro.Return}
				} else {
					/varset retValue COMPLETED_SUCCESS
				}
				/return ${retValue}
			}
		/goto :MoveLoop 
		:next 
	/next Location 
	/if (${Macro.Return.Length}) {
		/varset retValue ${Macro.Return}
	} else {
		/varset retValue COMPLETED_SUCCESS
	}
/return ${retValue}

Sub NecroSoloMovementCall
	| Default call routine
	/if (${Me.CombatState.Equal[Combat]} && !${InCombatMove}) /return AGGRO
/return CONTINUE

Sub NecroSoloMovementInit
	/if (!${Defined[NecroSoloMovementVer]}) {
		/declare NecroSoloMovementVer 			float outer 2.00
		/declare moveTimer						timer outer 0s
		/declare Moving								bool outer FALSE
		/declare StopDistance					float outer 8.00
		/declare MCallRoutine					string outer NecroSoloMovementCall
		/declare InCombatMove					bool outer FALSE
		/declare StopMoving						bool outer TRUE
		/declare strafeAttempts				int outer 0
		/declare doorAttempt					bool outer FALSE
	}
/return

Sub NecroSoloMoveToLoc(float MoveToY, float MoveToX,string MovementType,float MoveToZ)
		/if (!${Defined[maxMoveTimer]}) /call NecroSoloMovementInit
		/if (!${Defined[MovementType]}) /declare MovementType			string local FORWARD
		/if (${MovementType.NotEqual[FORWARD]} && ${MovementType.NotEqual[back]}) /varset MovementType FORWARD
		/if (${Defined[MoveToZ]}) {
			/declare moveUseZ					bool local TRUE
		} else {
			/declare moveUseZ					bool local FALSE
		}
		/declare retValue						string local UNKNOWN
		/declare faceString					string local
		/declare distanceTimer			timer local 0
		/declare locString					string local
		/declare distanceBefore			float local 0
		/declare distanceNow				float local 0
		/declare speedMod						int local 1
		/declare toHeading					float local 0
		
		/varset Moving FALSE
		/varset moveTimer ${maxMoveTimer}
		/varset locString ${MoveToY},${MoveToX}
		/if (${moveUseZ}) /varset locString ${locString},${MoveToZ}
		/varset faceString loc ${locString}
		/if (${MovementType.Equal[back]}) /varset faceString ${faceString} away
		/varset doorAttempt FALSE
		:MoveToLocLoop
			/if (${MCallRoutine.NotEqual[NULL]} && ${MCallRoutine.Length}) {
				/call ${MCallRoutine}
				/if (${Macro.Return.NotEqual[CONTINUE]}) {
					/varset Moving FALSE
					/keypress ${MovementType}
					/return ABORT_${Macro.Return}
				}
			}
			
			/varset distanceNow ${Math.Distance[${locString}]}
			/if (${distanceNow} < ${StopDistance}) {
				/if (${StopMoving}) /keypress ${MovementType}
				/varset Moving FALSE
				/return COMPLETED_SUCCESS
			} else /if (!${Moving}) {
				/keypress ${MovementType} HOLD
				/varset Moving TRUE
			}

			/if (!${moveTimer}) {
				/keypress ${MovementType}
				/varset Moving FALSE
				/return ABORT_TIMEOUT
			}
			
			/varset toHeading ${Me.HeadingToLoc[${MoveToY},${MoveToX}].Degrees}
			/if (${MovemenType.Equal[back]}) {
				/if (${toHeading} > 180) {
					/varcalc toHeading ${toHeading} - 180
				} else {
					/varcalc toHeading ${toHeading} + 180
				}
			}
			/if (${Math.Abs[${Math.Calc[${Me.Heading.Degrees} - ${toHeading}]}]} > .5) {
				/if (${moveUseZ} && (${Me.FeetWet} || ${Me.Underwater} || ${Me.Levitating})) {
					/face ${faceString}
				} else {
					/face ${faceString} nolook
				}
			}

			/if (${Me.FeetWet} || ${Me.Underwater}) {
				/varset speedMod 3
			} else {			
				/if (${Me.Speed} < 100) {
					/varset speedMod 2
				} else {
					/varset speedMod 1
				}
			}
			
			/if (!${distanceTimer}) {
				/if (${Math.Calc[${distanceBefore} - ${distanceNow}]} < ${Math.Calc[6/${speedMod}]} && ${distanceBefore}) {
					| We appear to be stuck
					/keypress ${MovementType}
					/varset Moving FALSE
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
						/if (${strafeAttempts} > 2) {
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
						/return ABORT_ROOT
					}					
				}
				/varset distanceBefore ${Math.Distance[${locString}]}
				/varset distanceTimer 15
			}
			/doevents
		/goto :MoveToLocLoop
/return ${retValue}

Sub NecroSoloMoveToSpawn(int MoveToID, float stopDistance, bool CombatMove)
		/if (!${Defined[maxMoveTimer]}) /call NecroSoloMovementInit
		/if (!${Spawn[${MoveToID}].ID}) /return ABORT_NOSPAWN
		/if (${Defined[stopDistance]}) /varset StopDistance ${stopDistance}
		/if (!${Defined[CombatMove]}) {
			/varset InCombatMove FALSE
		} else {
			/varset InCombatMove TRUE
		}
		/declare MovementType			string local FORWARD
		/varset StopMoving TRUE
		
		/declare MoveToX ${Spawn[${MoveToID}].X}
		/declare MoveToY ${Spawn[${MoveToID}].Y}
		/if (${Spawn[${MoveToID}].FeetWet} || ${Spawn[${MoveToID}].Underwater}) /declare MoveToZ ${Spawn[${MoveToID}].Z}
		/if (${Defined[MoveToZ]}) {
			/declare moveUseZ					bool local TRUE
		} else {
			/declare moveUseZ					bool local FALSE
		}
		/declare retValue						string local UNKNOWN
		/declare faceString					string local
		/declare distanceTimer			timer local 0
		/declare locString					string local
		/declare distanceBefore			float local 0
		/declare distanceNow				float local 0
		/declare speedMod						int local 1
		/declare toHeading					float local 0
		
		/varset Moving FALSE
		/varset moveTimer ${maxMoveTimer}
		/varset locString ${MoveToY},${MoveToX}
		/if (${moveUseZ}) /varset locString ${locString},${MoveToZ}
		/varset faceString loc ${locString}
		| /if (${MovementType.Equal[back]}) /varset faceString ${faceString} away
		/varset doorAttempt FALSE		
		:MoveToSpawnLoop
			/varset MoveToX ${Spawn[${MoveToID}].X}
			/varset MoveToY ${Spawn[${MoveToID}].Y}
			/if (${moveUseZ}) /varset MoveToZ ${Spawn[${MoveToID}].Z}
			/varset locString ${MoveToY},${MoveToX}
			/if (${moveUseZ}) /varset locString ${locString},${MoveToZ}
			/varset faceString loc ${locString}
			| /if (${MovementType.Equal[back]}) /varset faceString ${faceString} away
			/if (!${moveUseZ}) /varset faceString ${faceString} nolook
			
				
			/if (${MCallRoutine.NotEqual[NULL]} && ${MCallRoutine.Length}) {
				/call ${MCallRoutine}
				/if (${Macro.Return.NotEqual[CONTINUE]}) {
					/varset Moving FALSE
					/keypress ${MovementType}
					/return ABORT_${Macro.Return}
				}
			}
			
			/varset distanceNow ${Math.Distance[${locString}]}
			/if (${distanceNow} < ${StopDistance}) {
				/if (${StopMoving}) /keypress ${MovementType}
				/varset Moving FALSE
				/return COMPLETED_SUCCESS
			} else /if (!${Moving}) {
				/keypress ${MovementType} HOLD
				/varset Moving TRUE
			}

			/if (!${moveTimer}) {
				/keypress ${MovementType}
				/varset Moving FALSE
				/return ABORT_TIMEOUT
			}
			
			/varset toHeading ${Me.HeadingToLoc[${MoveToY},${MoveToX}].Degrees}
			/if (${MovemenType.Equal[back]}) {
				/if (${toHeading} > 180) {
					/varcalc toHeading ${toHeading} - 180
				} else {
					/varcalc toHeading ${toHeading} + 180
				}
			}
			/if (${Math.Abs[${Math.Calc[${Me.Heading.Degrees} - ${toHeading}]}]} > 0) {
				/if (${moveUseZ} && (${Me.FeetWet} || ${Me.Underwater} || ${Me.Levitating})) {
					/face ${faceString}
				} else {
					/face ${faceString} nolook
				}
			}
			
			/if (${Me.FeetWet} || ${Me.Underwater}) {
				/varset speedMod 4
			} else {
				/if (${Me.Speed} < 100) {
					/varset speedMod 2
				} else {
					/varset speedMod 1
				}
			}
			
			/if (!${distanceTimer}) {
				/if (${Math.Calc[${distanceBefore} - ${distanceNow}]} < ${Math.Calc[6/${speedMod}]} && ${distanceBefore}) {
					| We appear to be stuck
					/keypress ${MovementType}
					/varset Moving FALSE
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
						/if (${strafeAttempts} > 2) {
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
						/return ABORT_ROOT
					}	
				}
				/varset distanceBefore ${Math.Distance[${locString}]}
				/varset distanceTimer 15
			}
			/doevents
		/goto :MoveToSpawnLoop
/return ${retValue}

Sub NecroSoloReadINIA(FileName,SectionName) 
   /if (${Ini[${FileName},${SectionName},-1,NO].Equal[NO]}) { 
      /mqlog "${SectionName}" is not a Valid Section for FILE:${FileName}, ending read
      /return 
   } 
   /declare nValues     int local 
   /declare nMArray      int local  0
	/declare varPrefix			string local Loc
	
  /varset nValues ${Ini[${FileName},${SectionName},Value,0]} 
  
 
	/if (!${nValues}) {
		| It is an AdvPath path
		/echo Using Advpath ${SectionName}
		/varset varPrefix 
	}
   :MakeArraylo 
   /varset maxLoc 0
   /for nMArray 1 to 10000
   		/if (${Ini[${FileName},${SectionName},${varPrefix}${nMArray},NULL].Equal[NULL]}) /varcalc maxLoc ${nMArray} - 1
   /if (!${maxLoc}) /next nMArray
   	
   /for nMArray 1 to ${maxLoc}
      /varset LocArray[${nMArray}] ${Ini[${FileName},${SectionName},${varPrefix}${nMArray}].Replace[ ,,]}
   /next nMArray
   
   /if (${Math.Distance[${LocArray[1]}:${LocArray[${maxLoc}]}]} < 50) /varset circularPath true
/return