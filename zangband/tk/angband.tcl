# File: angband.tcl

# Purpose: script commands called by Angband

#
# Copyright (c) 1997-2001 Tim Baker
#
# This software may be copied and distributed for educational, research, and
# not for profit purposes provided that this copyright and statement are
# included in all such copies.
#

# angband_close_game --
#
#	Called by Angband when the game is about to exit.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc angband_close_game {} {

	global Angband

	if {[catch {

		if {[Value window,autosave]} {
			WriteGeometryFile
		}

		Config::Alternate::Write
		Config::Assign::Write
		Config::Feature::Write
		Config::Sprite::Write
		Config::CharIcon::Write

		NSValueManager::CloseModule
		
		if {[info exists ::Global(photoText)]} {
			set tempFile [Global photoText]
			if {[string length $tempFile] && [file exists $tempFile]} {
				file delete $tempFile
			}
		}

	} result]} {
		set message "An error occured during shutdown:\n    $result"
		tk_messageBox -title "ZAngband Error" -message $message \
			-icon error
	}

	return
}

# angband_display --
#
#	Called by Angband to hide/show a window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc angband_display {window action args} {

	global Angband
	global Display
	global PYPX

	if {[string equal $action hide]} {

		switch -- $window {

			floor -
			inven -
			equip {
				if {[string equal [Value inventory,style] new]} {
					if {[Inventory2Obj Info browsing]} return
					set window inventory2
				} else {
					if {[InventoryObj Info browsing]} return
					set window inventory
				}
#eval RequestDisplay $window $action $args
#return
			}

			map {
				set mainId [Global main,oop]
				set mapId [Global bigmap,mapId]
				place forget [NSMap::Info $mapId frame]
				pack forget [Global mapdetail,widget]
				place [Global micromap,widget] \
					-relx 0.5 -rely 0.5 -anchor center
					
				# Focus for KeyPress bindings
				focus [Window main]
				return
			}

			message {
				set window messages
			}

			player {
				set window character
			}

			store {
				if {[string equal [Value store,style] new]} {
					set window store2
				} else {
					set window store
				}
			}
		}

		# Undisplay the window
		if {[info exists NSWindowManager::Priv($window,win)]} {
			eval NSWindowManager::Undisplay $window $args
		} else {
			wm withdraw [Window $window]
		}

		# Restore focus
		catch {focus $Display($window,oldFocus)}

		set Display(window) none

	} else {

		switch -- $window {

			kingly {
				if {[catch {
					NSModule::LoadIfNeeded NSTomb
					NSTomb::KinglyWindow
				} result]} {
					HandleError $result
				}
				return
			}
	
			tomb {
				if {[catch {
					NSModule::LoadIfNeeded NSTomb
					NSTomb::TombWindow
				} result]} {
					HandleError $result
				}
				return
			}

			book {
				NSModule::LoadIfNeeded NSBookWindow
#eval RequestDisplay $window $action $args
#return
			}

			building {
				NSModule::LoadIfNeeded NSBuilding
			}

			choosemonster {
				NSModule::LoadIfNeeded NSChooseMonster
			}

			displayinfo {
				NSModule::LoadIfNeeded NSDisplayInfo
			}

			equip {
				if {[string equal [Value inventory,style] new]} {
					NSModule::LoadIfNeeded NSInventory2
					set window inventory2
				} else {
					NSModule::LoadIfNeeded NSInventory
					set window inventory
				}
				set args [concat [list equipment ""] $args]
#eval RequestDisplay $window $action $args
#return
			}

			floor {
				if {[string equal [Value inventory,style] new]} {
					NSModule::LoadIfNeeded NSInventory2
					set window inventory2
				} else {
					NSModule::LoadIfNeeded NSInventory
					set window inventory
				}
				set args [list floor ""]
#eval RequestDisplay $window $action $args
#return
			}	
	
			help {
				NSModule::LoadIfNeeded NSHelp
			}
	
			highscore {

				if {[catch {
					set loaded [NSModule::LoadIfNeeded NSHighScore]
				} result]} {
					HandleError $result
				}

				# Hack -- If the character-record option window is showing,
				# then it means the game is over, and we are about to
				# display the High Scores Window. To ensure there is
				# always an active frontmost window, we don't hide the
				# c-r'opt window until after the High Scores Window is
				# displayed.
				if {[angband player is_dead]} {

					if {[catch {

						# The High Scores Window was previously loaded
						if {!$loaded} {

							# No longer transient to the Main Window
							wm transient [Window highscore] ""
							unset ::WindowIcon([Window highscore])
							wm geometry [Window highscore] ""

							# Make sure the dead-character record is read
							NSRecord::LoadRecords
						}

						# Important for positioning and yview'ing
						NSToplevel::NaturalSize [Window highscore] ""

						NSHighScore::SetList [Global highscore,oop]
	
						# Position the window
						WindowPosition [Window highscore] 2 3

						# Make sure the window is active
						focus [Window highscore]

						# Now destroy the Character Record Options Window
						destroy .record

						# XXX Hack -- The Character Record Window
						# is centered after character death.
						set NSWindowManager::Priv(record,setup) 0
						set NSWindowManager::Priv(record,geomRequest) ""
						wm geometry [Window record] ""

						focus [NSHighScore::Info [Global highscore,oop] canvas]

						# Show the window, and wait
						tkwait window [Window highscore]
	
					} result]} {
						set prompt "The following error occured while trying to display the High Scores Window"
						HandleError $result $prompt
					}

					# Done
					return
				}
				wm title [Window highscore] [lindex $args 0]
				set args {}
			}

			info {
				NSModule::LoadIfNeeded NSInfoWindow
				NSInfoWindow::SetList [Global info,oop] [lindex $args 0] \
					[lindex $args 1]
			}

			inven {
				if {[string equal [Value inventory,style] new]} {
					NSModule::LoadIfNeeded NSInventory2
					set window inventory2
				} else {
					NSModule::LoadIfNeeded NSInventory
					set window inventory
				}
				set args [concat [list inventory ""] $args]
#eval RequestDisplay $window $action $args
#return
			}

			knowledge {
				NSModule::LoadIfNeeded NSKnowledge
			}
	
			map {
				set mainId [Global main,oop]
				set mapId [Global bigmap,mapId]
				place [NSMap::Info $mapId frame] -x 0 -y 0 \
					-relwidth 1.0 -relheight 1.0
				place forget [Global micromap,widget]
				pack [Global mapdetail,widget] \
					-expand yes -fill both
				update idletasks

				# We must map the widget so it may resize to fit its parent
				# before setting the view. Also, the scrollbars must be
				# mapped before they will update properly.
				eval NSMap::SetView $mapId $PYPX
				eval [Global mapdetail,widget] center $PYPX

				# Focus for KeyPress bindings
				focus [Global bigmap,widget]
				return
			}
	
			message {
				NSModule::LoadIfNeeded NSMessageHistory
				set window messages
			}

			macros {
				NSModule::LoadIfNeeded NSMacros
			}

			mindcraft {
				NSModule::LoadIfNeeded NSMindcraftWindow
			}

			options {
				NSModule::LoadIfNeeded NSOptions
			}

			pets {
				NSModule::LoadIfNeeded NSPets
			}

			player {
				NSModule::LoadIfNeeded NSCharacterWindow
				set window character
			}

			playerflags {
				NSModule::LoadIfNeeded NSPlayerFlags
			}

			power {
				NSModule::LoadIfNeeded NSPower
			}

			store {
				if {[string equal [Value store,style] new]} {
					NSModule::LoadIfNeeded NSStore2
					set window store2
				} else {
					NSModule::LoadIfNeeded NSStore
					set window store
				}
			}

			default {
				error "unknown window \"$window\""
			}
		}

		# The window isn't already displayed
		if {$Display(window) != $window} {

			# Save current focus
			set Display($window,oldFocus) [focus]

			set Display(window) $window
		}

		# Display the window
		if {[info exists NSWindowManager::Priv($window,win)]} {
			eval NSWindowManager::Display $window $args
		} else {
			WindowBringToFront [Window $window]
		}
	}

	# Update the windows now
	update

	return
}

# angband_prompt --
#
#	Called by Angband to prompt the user. Rather complex interface to
#	handle multiple messages per line, and getting a string from the
#	user. This routine builds the string to be displayed in the
#	Message Window. It is not actually displayed until a Term-fresh
#	quasi-event occurs, at which point Fresh_Prompt() is called.
#	This behaviour prevents unwanted redraws during macro invocations.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

set AngbandPriv(prompt,new) {}
set AngbandPriv(prompt,old) {}
set AngbandPriv(prompt,fresh) 0

qebind PROMPT <Term-fresh> Fresh_Prompt

proc Fresh_Prompt {} {

	global AngbandPriv

	if {!$AngbandPriv(prompt,fresh)} return

	if {[string compare $AngbandPriv(prompt,new) $AngbandPriv(prompt,old)]} {
		set wText [Global message,message]
		$wText delete 1.0 end
		if {[llength $AngbandPriv(prompt,new)]} {
			eval $wText insert end $AngbandPriv(prompt,new)
		}
		set AngbandPriv(prompt,old) $AngbandPriv(prompt,new)
	}

	set AngbandPriv(prompt,fresh) 0

	return
}

proc angband_prompt {action args} {

	global AngbandPriv

	switch -- $action {

		open {
			set AngbandPriv(prompt,new) $args
			set AngbandPriv(prompt,prefix) $args
		}

		update {
			set AngbandPriv(prompt,new) [concat $AngbandPriv(prompt,prefix) $args]
		}

		set {
			set AngbandPriv(prompt,new) $args
		}

		append {
			set AngbandPriv(prompt,new) [concat $AngbandPriv(prompt,new) $args]
		}

		wipe {
			set AngbandPriv(prompt,new) {}
		}
	}

	set AngbandPriv(prompt,fresh) 1

	return
}

# NSModule::AddModule NSMessageLine [file join $Angband(dirTK) message-line.tcl]
# NSModule::LoadIfNeeded NSMessageLine

# angband_store --
#
#	A bit of ugliness to handle the haggle prompts in a store.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc angband_store {action {arg1 ""}} {

	if {[string equal [Value store,style] new]} {
		set command Store2Obj
		set window store2
	} else {
		set command StoreObj
		set window store
	}

	set win [Window $window]

	switch -- $action {

		haggle_open -
		haggle_close {
			$command HaggleSetup $action
		}

		price_character -
		price_owner {
			$win.info.$action configure -text $arg1
		}
	}

	return
}


proc angband_command {command} {

	global Display
	switch -- $command {
		e -
		i {
			return 0
		}
		default {
			if {[string equal $Display(window) inventory]} {
				eval NSWindowManager::Undisplay inventory
				catch {focus $Display($window,oldFocus)}
				set Display(window) none
			}
			return 0
		}
	}
}
