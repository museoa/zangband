# File: building.tcl

# Purpose: the Building Window and related commands

#
# Copyright (c) 1997-2001 Tim Baker
#
# This software may be copied and distributed for educational, research, and
# not for profit purposes provided that this copyright and statement are
# included in all such copies.
#

namespace eval NSBuilding {

	variable MenuString
	variable Priv

# namespace eval NSBuilding
}

# NSBuilding::InitModule --
#
#	One-time-only-ever initialization.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::InitModule {} {

	global Angband
	variable Image
	variable Action2Image

	MsgCatInit building

	# Symbolic names for images in the townactions.gif file
	set names [list \
		Sword Fruit1 Fruit2 CompareWeapons Fruit4 ResearchMonster Fruit6 Throne \
		Shield Fruit7 Fruit8 Fruit9 Fruit10 Fruit11 Fruit12 \
		Quest Armor Dice1 Dice2 Dice3 Dice4 Dice5 Dice6 King \
		Skull Card1 Card2 Card3 Card4 Card5 Card6 Exit \
		Wheel Rules Rest Food Rumors Research RaceLegends TownHistory \
		Lord Plaque EnchantWeapon EnchantArmor \
			MasterThief ShareOfGold Identify LearnSpell \
		Warden Healing Shrine Poster \
			EnterArena InBetween Craps DruidLord \
		MasterRanger Priest Wizard ShadowLord \
			Restoration EnchantBow EnchantArrow LearnPrayer \
		Wall Recharge Trees Spires \
			Slots Bust EnchantWeapon2 EnchantArmor2 \
	]

	InitImageIfNeeded Image_TownActions townactions.gif

	set columns [expr {[image width Image_TownActions] / 32}]

	# Calculate the location in townactions.gif of each sub-image.
	# The sub-images are only created when needed.
	set col 0
	set row 0
	foreach name $names {
		set Image($name,x) [expr {$col * 32}]
		set Image($name,y) [expr {$row * 32}]
		set Image($name,exists) 0
		if {[incr col] == $columns} {
			set col 0
			incr row
		}
	}

	# This command is called when we actually create a graphical
	# button for a building action. We do this on demand to speed
	# things up.
	proc GetImage {name} {

		variable Image

		# Set the Tk image name
		set image Image_Town$name

		# Has this image been created?
		if {!$Image($name,exists)} {

			# Create the new image
			image create photo $image -width 32 -height 32

			# Copy image data from townactions.gif
			set x $Image($name,x)
			set y $Image($name,y)
			$image copy Image_TownActions -from $x $y \
				[expr {$x + 32}] [expr {$y + 32}]

			# Create the mask by hand, because Tk is too slow
			photomask $image

			# The image was created
			set Image($name,exists) 1
		}

		# Return the Tk image name
		return $image
	}

	# Map images to building commands.
	set a 0
	set Action2Image([incr a]) Research
	set Action2Image([incr a]) TownHistory
	set Action2Image([incr a]) RaceLegends
	set Action2Image([incr a]) Throne
	set Action2Image([incr a]) King
	set Action2Image([incr a]) Quest
	set Action2Image([incr a]) ShareOfGold
	set Action2Image([incr a]) Poster
	set Action2Image([incr a]) Rules
	set Action2Image([incr a]) EnterArena
	set Action2Image([incr a]) ArenaLegends
	set Action2Image([incr a]) InBetween
	set Action2Image([incr a]) Rules
	set Action2Image([incr a]) Craps
	set Action2Image([incr a]) Wheel
	set Action2Image([incr a]) Slots
	set Action2Image([incr a]) Rest
	set Action2Image([incr a]) Food
	set Action2Image([incr a]) Rumors
	set Action2Image([incr a]) ResearchMonster
	set Action2Image([incr a]) CompareWeapons
	set Action2Image([incr a]) RaceLegends
	set Action2Image([incr a]) EnchantWeapon
	set Action2Image([incr a]) EnchantArmor
	set Action2Image([incr a]) Recharge
	set Action2Image([incr a]) Identify
	set Action2Image([incr a]) LearnPrayer
	set Action2Image([incr a]) Healing
	set Action2Image([incr a]) Restoration
	set Action2Image([incr a]) EnchantArrow
	set Action2Image([incr a]) EnchantBow

	# BACT_GREET -> Ranger, Wizard, etc
	set Action2Image([incr a]) MasterRanger

	# BACT_RECALL
	set Action2Image([incr a]) ""

	# BACT_TELEPORT_LEVEL
	set Action2Image([incr a]) ""

	# BACT_CURE_MUTATION
	set Action2Image([incr a]) Restoration

	NSObject::New NSBuilding

	return
}

# NSBuilding::CloseModule --
#
#	One-time-only-ever cleanup.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::CloseModule {} {

	catch {
		destroy [Window building]
	}

	return
}

# NSBuilding::NSBuilding --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::NSBuilding {oop} {

	InitWindow $oop

	set win [Info $oop win]
	
	NSWindowManager::RegisterWindow building [Info $oop win] \
		"GetDefaultGeometry $win main2 main" "" \
		"NSBuilding::DisplayCmd $oop"

	# Destroy the object along with the toplevel (later)
	NSUtils::DestroyObjectWithWidget NSBuilding $oop $win

	#
	# Global list of application windows
	#

	Global building,oop $oop
	Window building $win

	return
}

# NSBuilding::~NSBuilding --
#
#	Object destructor called by NSObject::Delete().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::~NSBuilding {oop} {

	NSValueManager::RemoveClient listBG [Info $oop listBG,clientId]

	return
}

# NSBuilding::Info --
#
#	Query and modify info.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::Info {oop info args} {

	global NSBuilding

	# Verify the object
	NSObject::CheckObject NSBuilding $oop

	# Set info
	if {[llength $args]} {
		switch -- $info {
			default {
				set NSBuilding($oop,$info) [lindex $args 0]
			}
		}

	# Get info
	} else {
		switch -- $info {
			default {
				return $NSBuilding($oop,$info)
			}
		}
	}

	return
}

# NSBuilding::InitWindow --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::InitWindow {oop} {

	set win .building$oop
	toplevel $win
	wm title $win Building

	wm transient $win [Window main]

	# Do stuff when window closes
	wm protocol $win WM_DELETE_WINDOW "NSBuilding::Close $oop"

	# Start out withdrawn (hidden)
	wm withdraw $win

	Info $oop win $win

	InitMenus $oop

	#
	# Divider + Building Info
	#

	if {[Platform unix]} {
		set font {Courier 12}
	}
	if {[Platform windows]} {
		set font {Courier 9}
	}

	set frame $win.info
	frame $frame \
		-borderwidth 0
	MakeDivider $frame.divider1 x
	label $frame.playerGold \
		-font $font -text [mc "Gold Remaining:"]
	label $frame.gold \
		-font $font -text [angband player gold]

	#
	# Canvas
	#

	frame $win.frame \
		-borderwidth 1 -relief sunken
	canvas $win.frame.canvas \
		-width 400 -height 300 -background [Value listBG] \
		-borderwidth 0 -highlightthickness 0

	# Update ourself when the listBG value changes
	Info $oop listBG,clientId \
		[NSValueManager::AddClient listBG \
		"NSBuilding::ValueChanged_listBG $oop"]

	Info $oop canvas $win.frame.canvas

	#
	# Statusbar
	#

	MakeStatusBar $win.statusBar 12

	#
	# Geometry
	#

	pack $win.frame.canvas -expand yes -fill both

	grid rowconfig $win 0 -weight 0
	grid rowconfig $win 1 -weight 1
	grid rowconfig $win 2 -weight 0
	grid columnconfig $win 0 -weight 1

	grid $win.info \
		-row 0 -column 0 -rowspan 1 -columnspan 1 -sticky ew
	grid $win.frame \
		-row 1 -column 0 -rowspan 1 -columnspan 1 -sticky news
	grid $win.statusBar \
		-row 2 -column 0 -rowspan 1 -columnspan 2 -sticky ew

	#
	# Context Menu
	#
	
	set m $win.context
	menu $m -tearoff 0
	bind [Info $oop canvas] <Button-3> \
		"NSBuilding::ContextMenu $oop $m %X %Y"

	#
	# Feed Term when keys pressed
	#

	Term_KeyPress_Bind $win
	Term_KeyPress_Bind [Info $oop canvas]

	return
}

# NSBuilding::InitMenus --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::InitMenus {oop} {

	variable MenuString

	set win [Info $oop win]

	#
	# Menu bar
	#

	set mbar [NSObject::New NSMenu $win -tearoff 0 \
		-postcommand "NSBuilding::SetupMenus $oop" -identifier MENUBAR]
	Info $oop mbarId $mbar

	# Context-sensitive help
	NSMenu::Info $mbar menuSelectCmd "NSBuilding::MenuSelect $oop"

	#
	# Building Menu
	#

	NSObject::New NSMenu $mbar -tearoff 0 -identifier MENU_BUILDING
	NSMenu::MenuInsertEntry $mbar -end MENUBAR -type cascade \
		-menu MENU_BUILDING -label [mc Building] -underline 0 \
		-identifier M_BUILDING

	set entries {}
	lappend entries [list -type command -label [mc Leave] \
		-command "DoCommandIfAllowed \033" -identifier E_EXIT]
	
	NSMenu::MenuInsertEntries $mbar -end MENU_BUILDING $entries

	return
}

# NSBuilding::SetupMenus --
#
#	Description
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::SetupMenus {oop mbarID} {

	set identList {}
	
	if {[string equal [angband inkey_flags] INKEY_CMD]} {
		lappend identList E_EXIT
	}

	NSMenu::MenuEnable $mbarID $identList

	[Info $oop win].statusBar cover show

	return
}

# NSBuilding::MenuSelect --
#
#	Displays a help string associated with a menu entry.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::MenuSelect {oop menuId index ident} {

	variable MenuString

	switch -glob -- $ident {
		{} {
			set desc {}
		}				
		E_EXIT {
			set buildingname [angband building buildingname]
			set desc "Leave the $buildingname."
		}
		default {
			if {[info exists MenuString($ident)]} {
				set desc $MenuString($ident)
			} else {
				set menu [NSMenu::Info $menuId menu]
				set desc [$menu entrycget $index -label]
			}
		}
	}

	[Info $oop win].statusBar cover set $desc
	if {![string length $desc]} {
		if {$menuId == [Info $oop mbarId]} {
			[Info $oop win].statusBar cover hide
		}
	}

	return
}

# NSBuilding::DisplayCmd --
#
#	Called by NSWindowManager::Display().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::DisplayCmd {oop message first} {

	switch -- $message {
		preDisplay {
			set win [Info $oop win]
			set buildingname [angband building buildingname]
			set ownername [angband building ownername]
			set ownerrace [angband building ownerrace]
			wm title $win [format [mc "The %s run by %s the %s"] \
				$buildingname $ownername $ownerrace]

			ConfigureWindow $oop
			DisplayCommands $oop
		}
		reDisplay {
			DisplayCommands $oop
		}
		postDisplay {
		}
	}

	return
}

# NSBuilding::Close --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::Close {oop} {

	angband keypress \033

	return
}

# NSBuilding::ConfigureWindow --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::ConfigureWindow {oop} {

	set win [Info $oop win]
	set frame $win.info

	eval pack forget [winfo children $frame]

	pack $frame.divider1 \
		-side top -expand yes -fill x

	# Character's Gold
	pack $frame.gold \
		-side right -expand no -padx 2
	pack $frame.playerGold \
		-side right -expand no -padx 2

	return
}

# NSBuilding::DisplayCommands --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::DisplayCommands {oop} {

	variable Action2Image

	set canvas [Info $oop canvas]

	$canvas delete all

	set buildingname [angband building buildingname]
	set b_idx [angband building index]

	set font {Times 18 bold}
	set fh [font metrics $font -linespace]
	$canvas create text 10 10 -text [format [mc "The %s"] $buildingname] \
		-fill White -font $font -anchor nw
	
	set x 10
	set top [expr {10 + $fh + 6}]
	set y $top
	set row 0
	set height [expr {4 + 2 + 32 + 2 + 4 + 4}]
	set width [expr {200 + 8}]

	# Get a list of building actions
	set actionList [angband building actions]

	# Append Exit action
	lappend actionList [list char ESC label [mc "Exit building"] info "" action xxx]

	# Create a new button for each action
	foreach action $actionList {

		# List -> Array -> Variables
		array unset data
		array set data $action
		set cmdChar $data(char)
		set desc $data(label)
		set extra $data(info)
		set actionNum $data(action)

		# Assume the action is available to the character
		set closed 0

		# See if the action costs money or is not available
		if {[string match "*gp)" $extra]} {
			scan $extra " (%dgp)" cost
			set extra [format [mc "Cost: %d gp"] $cost]
		} elseif {[string match "*(closed)" $extra]} {
			set closed 1
			set extra CLOSED
		}

		# Get an image for the action
		set image ""
		if {[string equal $cmdChar ESC]} {
			set image [GetImage Exit]
		} elseif {[info exists Action2Image($actionNum,$b_idx)]} {
			set image [GetImage $Action2Image($actionNum,$b_idx)]
		} elseif {[info exists Action2Image($actionNum)]} {
			if {[string length $Action2Image($actionNum)]} {
				set image [GetImage $Action2Image($actionNum)]
			}
		}

		# Create a button
		CreateButton $canvas $x $y $cmdChar $image $desc $extra $closed

		# Next row/column
		incr y $height
		if {[incr row] == 4} {
			incr x $width
			set y $top
		}
	}

	return
}

# NSBuilding::ContextMenu --
#
#	If the game is waiting for the user to choose an item, then
#	pop up a menu of choices.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::ContextMenu {oop menu x y} {

	# Waiting for an inventory item
	if {[string equal [angband inkey_flags] INKEY_ITEM]} {

		# Display equipment/inventory items
		NSRecall::PopupSelect_Item $menu $x $y

		# Done
		return
	}

	return	
}

# NSBuilding::FruitColor --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::FruitColor {slot} {

	set colors [list "" Yellow Orange Gray Gray Violet Red]
	return [lindex $colors $slot]
}

# NSBuilding::CreateButton --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::CreateButton {canvas x y char image text extra closed} {

	if {$closed} {
		set fill #804040
	} else {
		set fill Brown
	}
	set width 200
	set height [expr {4 + 2 + 32 + 2 + 4}]
	$canvas create rectangle [expr {$x + 2}] [expr {$y + 2}] \
		[expr {$x + $width - 2}] \
		[expr {$y + $height - 2}] -outline $fill -fill $fill -width 4 \
		-tags "button$char button$char,rect"
	$canvas create image [expr {$x + 4 + 2 + 16}] \
		[expr {$y + 4 + 2 + 16}] -image $image -anchor center \
		-tags button$char

	set font [Global font,sys,normal]

	# Char
	$canvas create text [expr {$x + 4 + 2 + 32 + 2}] \
		[expr {$y + 4 + 2 + 16}] -text "$char)" -anchor w -fill White \
		-tags button$char -font $font
	set offset [font measure $font "$char) "]

	# Text + Extra
	if {[string length $extra]} {
		append text \n$extra
	}
	$canvas create text [expr {$x + 4 + 2 + 32 + 2 + $offset}] \
		[expr {$y + 4 + 2 + 16}] -text $text -anchor w -fill White \
		-tags button$char -font $font

	if {$closed} {
		foreach sequence [$canvas bind button$char] {
			$canvas bind button$char $sequence ""
		}
	} else {
		if {[string equal $char ESC]} {
			set string "\033"
		} else {
			set string $char
		}
		$canvas bind button$char <ButtonPress-1> \
			"DoCommandIfAllowed $string"
		$canvas bind button$char <Enter> \
			"%W itemconfigure button$char,rect -outline Gold"
		$canvas bind button$char <Leave> \
			"%W itemconfigure button$char,rect -outline Brown"
	}

	return
}

# NSBuilding::CreateCard --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::CreateCard {canvas x y face} {

	$canvas create image $x $y -image [GetImage Card$face] -anchor nw \
		-tags temp

	return
}

# NSBuilding::CreateDie --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::CreateDie {canvas x y face} {

	$canvas create image $x $y -image [GetImage Dice$face] -anchor nw \
		-tags temp

	return
}

# NSBuilding::CreateFruit --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::CreateFruit {canvas x y slot} {

	set font [Global font,fixed,normal]

	set gif(Lemon) Fruit10
	set gif(Orange) Fruit11
	set gif(Sword) Sword
	set gif(Shield) Shield
	set gif(Grape) Fruit12
	set gif(Cherry) Fruit9

	set name [lindex [list "" Lemon Orange Sword Shield Grape Cherry] $slot]

	$canvas create image $x $y -image [GetImage $gif($name)] -anchor n \
		-tags temp
	$canvas create text $x [expr {$y + 36}] \
		-font $font -fill [FruitColor $slot] -text $name -tags temp -anchor n

	return
}

# NSBuilding::HandleGamblingHouse --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::HandleGamblingHouse {oop message args} {

	set canvas [Info $oop canvas]

	switch -- $message {

		craps {
			$canvas delete temp
			set font [Global font,fixed,normal]
			set fh [font metrics $font -linespace]
			set fw [font measure $font W]
			NSMiscWindow::CanvasAddTextItem $canvas $font 2 1 -1 Green "Craps" left temp
			set size 32
			set x [expr {$fw * (54 / 2) - (($size + 8) * 3 + 8) / 2}]
			set y [expr {$fh * 6 - $size / 2}]
			$canvas create rectangle $x $y [expr {$x + ($size + 8) * 3 + 8}] [expr {$y + $size + 16}] \
				-fill Green -outline "" -tags temp
			incr x 8 ; incr y 8
			set faces [lindex $args 0]
			CreateDie $canvas $x $y [lindex $faces 0]
			CreateDie $canvas [incr x [expr {$size + 8}]] $y [lindex $faces 1]
			set font {Times 18}
			$canvas create text [incr x [expr {$size + 8 + $size / 2}]] [expr {$y + $size / 2}] \
				-text [lindex $faces 2] -font $font -fill Black -tags temp -anchor center
		}

		diceslots {
			$canvas delete temp
			set font [Global font,fixed,normal]
			set fh [font metrics $font -linespace]
			set fw [font measure $font W]
			NSMiscWindow::CanvasAddTextItem $canvas $font 2 1 -1 Green "Dice Slots" left temp
			set x [expr {$fw * (54 / 2 - 3 * 10 / 2 + 10 / 2)}]
			set y [expr {$fh * 6}]
			scan [lindex $args 0] "%d %d %d" slot1 slot2 slot3
			CreateFruit $canvas $x $y $slot1
			CreateFruit $canvas [incr x [expr {$fw * 10}]] $y $slot2
			CreateFruit $canvas [incr x [expr {$fw * 10}]] $y $slot3
		}

		inbetween {
			$canvas delete temp
			set font [Global font,fixed,normal]
			set fh [font metrics $font -linespace]
			set fw [font measure $font W]
			NSMiscWindow::CanvasAddTextItem $canvas $font 2 1 -1 Green "In Between" left temp
			set size 32
			set x [expr {$fw * (54 / 2) - (($size + 8) * 3 + 8) / 2}]
			set y [expr {$fh * 6 - $size / 2}]
			$canvas create rectangle $x $y [expr {$x + ($size + 8) * 3 + 8}] [expr {$y + $size + 16}] \
				-fill Green -outline "" -tags temp
			incr x 8 ; incr y 8
			set faces [lindex $args 0]
			CreateCard $canvas $x $y [lindex $faces 0]
			CreateCard $canvas [incr x [expr {$size + 8}]] $y [lindex $faces 1]
			CreateCard $canvas [incr x [expr {$size + 8}]] $y [lindex $faces 2]
		}

		wheel {
			$canvas delete temp
			set font [Global font,fixed,normal]
			set fh [font metrics $font -linespace]
			set fw [font measure $font W]
			NSMiscWindow::CanvasAddTextItem $canvas $font 2 1 -1 Green "Wheel" left temp
			set radius 48
			set x [expr {$fw * 25}]
			set y [expr {$fh * 6}]
			for {set i 1} {$i <= 10} {incr i} {
				if {$i & 1} {
					set fill Blue
					set tag pieTag1
				} else {
					set fill Green
					set tag pieTag2
				}
				$canvas create arc [expr {$x - $radius}] \
					[expr {$y - $radius}] [expr {$x + $radius}] \
					[expr {$y + $radius}] -extent 36 \
					-start [expr {($i - 1) * 36}] \
					-fill $fill -tags "temp $tag"
			}
			scan [lindex $args 0] "%d %d" choice wheel
			set font2 {Times 18}

			set x2 [expr {$x - ($radius + 48)}]
			$canvas create text $x2 $y \
				-text $choice -font $font2 -fill White -anchor center -tags "temp choice"
			set y2 [lindex [$canvas bbox choice] 1]
			$canvas create text $x2 [expr {$y2 - 4}] \
				-text "You bet on:" -font $font -fill White -anchor s -tags temp

			set x2 [expr {$x + ($radius + 48)}]
			$canvas create text $x2 $y \
				-text $wheel -font $font2 -fill White -anchor center -tags "temp wheel"
			$canvas create text $x2 [expr {$y2 - 4}] \
				-text "This spin:" -font $font -fill White -anchor s -tags temp

			# Animation
			set fill1 Green ; set fill2 Blue
			for {set i 1} {$i <= 4} {incr i} {
				$canvas itemconfigure pieTag1 -fill $fill1
				$canvas itemconfigure pieTag2 -fill $fill2
				set temp $fill1 ; set fill1 $fill2 ; set fill2 $temp
				$canvas itemconfigure wheel -text [string index " 4792" $i]
				update idletasks ; after 20
			}
			$canvas itemconfigure wheel -text $wheel
		}

		loser {
			set wager [lindex $args 0]
			set font [Global font,fixed,normal]
			NSMiscWindow::CanvasAddTextItem $canvas $font 37 13 -1 Red "You lost $wager gp" left temp
			NSMiscWindow::CanvasAddTextItem $canvas $font 37 15 -1 White "Play again? (y/n)" left temp
		}

		wager {

			# This is the first thing called for any gambling screen
			# Clear display completely here
			$canvas delete all

			set wager [lindex $args 0]
			set font [Global font,fixed,normal]
			NSMiscWindow::CanvasAddTextItem $canvas $font 2 15 -1 White [format "Current Wager:    %9ld" $wager] left {}

			set gold [angband player gold]
			NSMiscWindow::CanvasAddTextItem $canvas $font 2 14 -1 White [format "Gold before game: %9ld" $gold] left {}
		}

		winner {
			scan [lindex $args 0] "%d %d" wager odds
			set font [Global font,fixed,normal]
			NSMiscWindow::CanvasAddTextItem $canvas $font 37 13 -1 Gold "YOU WON $wager gp" left temp
			NSMiscWindow::CanvasAddTextItem $canvas $font 37 14 -1 White "Payoff $odds-to-1" left temp
			NSMiscWindow::CanvasAddTextItem $canvas $font 37 15 -1 White "Play again? (y/n)" left temp
		}
	}

	return
}

# NSBuilding::EnchantItem --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::EnchantItem {oop message args} {

	variable Priv

	set canvas [Info $oop canvas]

	set font [Global font,fixed,normal]
	set fh [font metrics $font -linespace]
	set fw [font measure $font W]

	switch -- $message {
		init {
			$canvas delete all
			NSMiscWindow::CanvasAddTextItem $canvas $font 2 1 -1 \
				Green "Enchant Item" left temp
			set prompt [lindex $args 0]
			NSMiscWindow::CanvasAddTextItem $canvas $font 2 3 -1 \
				White $prompt left temp
		}
		init2 {
			set prompt [lindex $args 0]
			NSMiscWindow::CanvasAddTextItem $canvas $font 2 5 -1 \
				White $prompt left temp
		}
	}

	return
}

# NSBuilding::RechargeItem --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::RechargeItem {oop message args} {

	variable Priv

	set canvas [Info $oop canvas]

	set font [Global font,fixed,normal]
	set fh [font metrics $font -linespace]
	set fw [font measure $font W]

	switch -- $message {
		init {
			$canvas delete all
			NSMiscWindow::CanvasAddTextItem $canvas $font 2 1 -1 \
				Green "Recharge Item" left temp
		}
		info {
			scan [lindex $args 0] "%d %d %d" k_idx cost charges
			set name [lindex $args 1]
			NSMiscWindow::CanvasAddTextItem $canvas $font 2 3 -1 \
				White "Recharging $name." left temp
			set name [angband k_info info $k_idx object_desc]
			set pval [angband k_info set $k_idx pval]
			set string "Each $name can accept $pval charges."
			NSMiscWindow::CanvasAddTextItem $canvas $font 2 5 -1 \
				White $string left temp
			set string "A single charge will cost $cost gold."
			NSMiscWindow::CanvasAddTextItem $canvas $font 2 7 -1 \
				Gold $string left temp
			set max_cost [expr {$charges * $cost}]
			set string "A full recharge will cost $max_cost gold."
			NSMiscWindow::CanvasAddTextItem $canvas $font 2 8 -1 \
				Gold $string left temp
		}
	}

	return
}

# NSBuilding::Quest --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::Quest {oop message args} {

	set canvas [Info $oop canvas]

	set font [Global font,fixed,normal]
	set fh [font metrics $font -linespace]
	set fw [font measure $font W]

	switch -- $message {
		info {
			$canvas delete all
			set qname [lindex $args 0]
			set qlevel [lindex $args 1]
			set qtext [lindex $args 2]
			NSMiscWindow::CanvasAddTextItem $canvas $font 2 1 -1 Green "Quest information (Danger level: $qlevel)" left temp
			NSMiscWindow::CanvasAddTextItem $canvas $font 2 3 -1 White $qname left temp
			NSMiscWindow::CanvasAddTextItem $canvas $font 4 5 -1 Yellow $qtext left "temp qtext"
			set y2 [lindex [$canvas bbox qtext] 3]
			set row [expr {$y2 / $fh + 1}]
			if {$y2 % $fh} {incr row}
		}
	}

	return
}

# NSBuilding::FlagText --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::FlagText {oop flag} {

	switch -- $flag {
		SLAY_ANIMAL {return "vs Animals"}
		SLAY_EVIL {return "vs Evil"}
		SLAY_UNDEAD {return "vs Undead"}
		SLAY_DEMON {return "vs Demons"}
		SLAY_ORC {return "vs Orcs"}
		SLAY_TROLL {return "vs Trolls"}
		SLAY_GIANT {return "vs Giants"}
		SLAY_DRAGON {return "vs Dragons"}
		KILL_DRAGON {return "vs Dragons"}
		BRAND_ACID {return "Acid brand"}
		BRAND_ELEC {return "Lightning brand"}
		BRAND_FIRE {return "Fire brand"}
		BRAND_COLD {return "Frost brand"}
		BRAND_POIS {return "Poison brand"}
		default {
			return $flag
		}
	}

	return
}

# NSBuilding::CompareWeapons --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::CompareWeapons {oop message args} {

	variable Priv

	set canvas [Info $oop canvas]

	set font [Global font,fixed,normal]
	set fh [font metrics $font -linespace]
	set fw [font measure $font W]

	switch -- $message {
		init {
			$canvas delete all
			set prompt [lindex $args 0]
			NSMiscWindow::CanvasAddTextItem $canvas $font 2 1 -1 Green $prompt left temp
			set Priv(compare,count) 0
		}
		list_weapon {
			set desc [lindex $args 0]
			scan [lindex $args 1] "%d %d %d %d %d %d %d" Priv(compare,to_h) \
				Priv(compare,to_d) Priv(compare,dd) Priv(compare,ds) \
				Priv(compare,num_blow) Priv(compare,min_dam) \
				Priv(compare,max_dam)

			incr Priv(compare,count)
			if {$Priv(compare,count) == 1} {
				set col 2
			} else {
				set col 40
			}
			set row 2
			NSMiscWindow::CanvasAddTextItem $canvas $font $col [incr row] -1 Yellow $desc left temp

			set string "To Hit: $Priv(compare,to_h)   Deadliness: $Priv(compare,to_d)"
			NSMiscWindow::CanvasAddTextItem $canvas $font $col [incr row] -1 White $string left temp

			set string "Dice: $Priv(compare,dd)   Sides: $Priv(compare,ds)"
			NSMiscWindow::CanvasAddTextItem $canvas $font $col [incr row] -1 White $string left temp

			set string "Number of Blows: $Priv(compare,num_blow)"
			NSMiscWindow::CanvasAddTextItem $canvas $font $col [incr row] -1 White $string left temp

			incr row
			set string "Possible Damage:"
			NSMiscWindow::CanvasAddTextItem $canvas $font $col [incr row] -1 Yellow $string left temp

			incr col
			set dam1 [expr {$Priv(compare,min_dam) / 100}]
			set dam2 [expr {$Priv(compare,max_dam) / 100}]
			set string "One Strike: $dam1-$dam2 damage"
			NSMiscWindow::CanvasAddTextItem $canvas $font $col [incr row] -1 White $string left temp

			set dam1 [expr {($Priv(compare,min_dam) * $Priv(compare,num_blow)) / 100}]
			set dam2 [expr {($Priv(compare,max_dam) * $Priv(compare,num_blow)) / 100}]
			set string "One Attack: $dam1-$dam2 damage"
			NSMiscWindow::CanvasAddTextItem $canvas $font $col [incr row] -1 White $string left temp

			set Priv(compare,flagCount) 0
		}
		flag {
			set flag [lindex $args 0]
			scan [lindex $args 1] "%d %d" min_dam max_dam

			if {$Priv(compare,count) == 1} {
				set col 2
			} else {
				set col 40
			}
			set row [expr {11 + [incr Priv(compare,flagCount)]}]

			if {$Priv(compare,flagCount) == 1} {
				set string "Damage Multipliers: "
				NSMiscWindow::CanvasAddTextItem $canvas $font $col 12 -1 Yellow $string left temp
			}

			incr col
set mult 1
			set string "[FlagText $oop $flag] (x$mult): "
set string "[FlagText $oop $flag]: "
			NSMiscWindow::CanvasAddTextItem $canvas $font $col [incr row] -1 White $string left temp

			incr col [string length $string]
			set dam1 $min_dam
			set dam2 $max_dam
			set string "$dam1-$dam2 damage"
			NSMiscWindow::CanvasAddTextItem $canvas $font $col $row -1 White $string left temp
		}
	}

	return
}


# NSBuilding::ValueChanged_listBG --
#
#	Called when the listBG value changes.
#	Updates the Building Window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBuilding::ValueChanged_listBG {oop} {

	set color [Value listBG]
	[Info $oop canvas] configure -background $color

	return
}

# angband_building --
#
#	A bit of ugliness to handle building irregularites.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc angband_building {message args} {

	set oop [Global building,oop]
	set win [NSBuilding::Info $oop win]
	set canvas [NSBuilding::Info $oop canvas]

	set font [Global font,fixed,normal]

	switch -- $message {
		compare_weapons {
			eval NSBuilding::CompareWeapons $oop $args
			return
		}
		enchant_item {
			eval NSBuilding::EnchantItem $oop $args
			return
		}
		quest {
			eval NSBuilding::Quest $oop $args
			return
		}
		recharge_item {
			eval NSBuilding::RechargeItem $oop $args
			return
		}
		words {
			$canvas delete all
			set text [lindex $args 0]
			NSMiscWindow::CanvasAddTextItem $canvas $font 2 1 -1 White $text left temp
			return
		}
		craps -
		diceslots -
		inbetween -
		wheel -
		loser -
		wager -
		winner	{
			eval NSBuilding::HandleGamblingHouse $oop $message $args
		}
	}

	return
}
