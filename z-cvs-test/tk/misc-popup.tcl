# File: misc-popup.tcl

# Purpose: the Misc Window popup menu/window and related commands

#
# Copyright (c) 1997-2001 Tim Baker
#
# This software may be copied and distributed for educational, research, and
# not for profit purposes provided that this copyright and statement are
# included in all such copies.
#

namespace eval NSMiscPopup {

	variable Priv

# namespace eval NSMiscPopup
}

# NSMiscPopup::InitModule --
#
#	One-time-only-ever initialization.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMiscPopup::InitModule {} {

	MsgCatInit inven misc-win

	InitImageIfNeeded Image_ButtonActivate button-activate.gif
	InitImageIfNeeded Image_ButtonFood button-food.gif
	InitImageIfNeeded Image_ButtonPotion button-potion.gif
	InitImageIfNeeded Image_ButtonScroll button-scroll.gif
	InitImageIfNeeded Image_ButtonRod button-rod.gif
	InitImageIfNeeded Image_ButtonWand button-wand.gif
	InitImageIfNeeded Image_ButtonStaff button-staff.gif
	InitImageIfNeeded Image_ButtonDown button-down.gif
	InitImageIfNeeded Image_ButtonUp button-up.gif

	MInfo oop [NSObject::New NSMiscPopup]

	return
}

# NSMiscPopup::CloseModule --
#
#	One-time-only-ever cleanup.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMiscPopup::CloseModule {} {

	catch {
		set win [MInfo win]
		destroy $win
	}

	return
}

# NSMiscPopup::MInfo --
#
#	Query and modify info.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMiscPopup::MInfo {info args} {

	variable Priv

	# Set info
	if {[llength $args]} {
		set Priv($info) [lindex $args 0]

	# Get info
	} else {
		return $Priv($info)
	}

	return
}

# NSMiscPopup::NSMiscPopup --
#
#	Object constructor called by NSObject::New().
#
# Arguments:
#	oop					OOP ID. See above.
#
# Results:
#	What happened.

proc NSMiscPopup::NSMiscPopup {oop} {

	Info $oop hook ""
	Info $oop current ""
	Info $oop busy 0

	Info $oop win,visible 0
	Info $oop whoHasCursor ""
	Info $oop after ""

	Info $oop nextButton 0

	InitWindow $oop

	set win [Info $oop win]

	# See CloseModule()
	MInfo win $win

	# Update ourself when the list highlight color changes
	Info $oop clientId,listHilite \
		[NSValueManager::AddClient listHilite \
			"$win.frame.text tag configure HOT \
			-background \[Value listHilite]"]

	# Update ourself when the font changes
	Info $oop clientId,font,miscPopup \
		[NSValueManager::AddClient font,miscPopup \
			"NSMiscPopup::ValueChanged_font_miscPopup $oop"]
		
	# Destroy the object along with the toplevel (later)
	NSUtils::DestroyObjectWithWidget NSMiscPopup $oop $win

	return
}

# NSMiscPopup::~NSMiscPopup --
#
#	Object destructor called by NSObject::Delete().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMiscPopup::~NSMiscPopup {oop} {

	set win [Info $oop win]

	NSValueManager::RemoveClient font,miscPopup [Info $oop clientId,font,miscPopup]
	NSValueManager::RemoveClient listHilite [Info $oop clientId,listHilite]

	return
}

# NSMiscPopup::Info --
#
#	Query and modify info.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMiscPopup::Info {oop info args} {

	global NSMiscPopup

	# Set info
	if {[llength $args]} {
		switch -- $info {
			default {
				set NSMiscPopup($oop,$info) [lindex $args 0]
			}
		}

	# Get info
	} else {
		switch -- $info {
			default {
				return $NSMiscPopup($oop,$info)
			}
		}
	}

	return
}

# NSMiscPopup::InitWindow --
#
#	Create the window.
#
# Arguments:
#	oop					OOP ID. See above.
#
# Results:
#	What happened.

proc NSMiscPopup::InitWindow {oop} {

	Info $oop canvas [Global misc,toolbar]

	#
	# Buttons
	#

	set x 1
	NewButton $oop -image Image_ButtonActivate -popup 1 -hook item \
		-args [list equipment A -activate yes] \
		-message [mc "Activate an equipment item"]
	NewButton $oop -image Image_ButtonFood -popup 1 -hook item \
		-args [list inventory E -tval TV_FOOD] \
		-message [mc "Eat an inventory item"]
	NewButton $oop -image Image_ButtonPotion -popup 1 -hook item \
		-args [list inventory q -tval TV_POTION] \
		-message [mc "Drink a potion in inventory"]
	NewButton $oop -image Image_ButtonScroll -popup 1 -hook item \
		-args [list inventory r -tval TV_SCROLL] \
		-message [mc "Read a scroll in inventory"]
	NewButton $oop -image Image_ButtonRod -popup 1 -hook item \
		-args [list inventory z -tval TV_ROD] \
		-message [mc "Zap a rod in inventory"]
	NewButton $oop -image Image_ButtonWand -popup 1 -hook item \
		-args [list inventory a -tval TV_WAND] \
		-message [mc "Aim a wand in inventory"]
	NewButton $oop -image Image_ButtonStaff -popup 1 -hook item \
		-args [list inventory u -tval TV_STAFF] \
		-message [mc "Use a staff in inventory"]

	NewButton $oop -image Image_ButtonUp -popup 0 \
		-command "DoUnderlyingCommand <" \
		-message [mc "Go up a dungeon level"]
	NewButton $oop -image Image_ButtonDown -popup 0 \
		-command "DoUnderlyingCommand >" \
		-message [mc "Go down a dungeon level"]

	#
	# Popup window of choices
	#

	set win [Global misc,canvas].miscpopup$oop
	toplevel $win -borderwidth 1 -relief flat -background gray60

	wm overrideredirect $win yes
	wm transient $win [Window misc]

	if {[Platform unix]} {
		$win configure -cursor arrow
	}

	# Start out withdrawn (hidden)
	wm withdraw $win

	# Set instance variables
	Info $oop win $win

	set wText $win.text
	text $wText \
		-wrap none -font [Value font,miscPopup] \
		-borderwidth 0 -setgrid no -highlightthickness 0 \
		-padx 4 -pady 2 -background Black -foreground White -cursor ""

	# Bypass default Text bindings
	bindtags $wText [list $wText $win all]

	pack $wText \
		-expand yes -fill both

	Info $oop text $wText

	# Fiddle with the selection for list behaviour
	$wText tag configure HOT -foreground White \
		-background [Value listHilite]

	$wText tag bind HOT <ButtonPress-1> \
		"NSMiscPopup::Invoke $oop"
	$wText tag bind TEXT <Motion> \
		"NSMiscPopup::Motion $oop \[$wText index {@%x,%y linestart}]"
	$wText tag bind HOT <Leave> \
		"NSMiscPopup::Motion $oop {}"

	bind $win <Enter> \
		"NSMiscPopup::Event $oop enter-win"
	bind $win <Leave> "
		NSMiscPopup::Motion $oop {}
		NSMiscPopup::Event $oop leave-win
	"

	return
}

# NSMiscPopup::NewButton --
#
#	Add a new button.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMiscPopup::NewButton {oop args} {

	set canvas [Info $oop canvas]

	# Get the next unique id for this button
	set num [incr ::NSMiscPopup($oop,nextButton)]

	set config(-command) ""
	array set config $args

	set image $config(-image)
	set command $config(-command)
	set message $config(-message)

	# The buttons are positioned in ShowBar().
	set x [expr {1 + ($num - 1) * 20}]
	set y 1

	Info $oop button,x,$num $x

	# Focus rectangle
	$canvas create rectangle $x $y [expr {$x + 17}] [expr {$y + 17}] \
		-tags [list button button$num border$num]

	# Image
	$canvas create image [expr {$x + 1}] [expr {$y + 1}] -image $image \
		-anchor nw -tags "button button$num img$num"

	# Show popup on mouse-over
	if {![string length $command]} {

		$canvas bind img$num <Enter> "
			$canvas itemconfigure border$num -outline gray60
			NSMainWindow::StatusText $oop [list $message]
			NSMiscPopup::EnterButton $oop $num
		"

		Info $oop button,hook,$num $config(-hook)
		Info $oop button,args,$num $config(-args)

	# Click to invoke command
	} else {
		$canvas bind img$num <Enter> "
			$canvas itemconfigure border$num -outline gray60
			NSMainWindow::StatusText $oop [list $message]
			NSMiscPopup::Event $oop enter-button2
		"
		$canvas bind img$num <ButtonPress-1> "
			$canvas move button$num 1 1
			$command
		"
		$canvas bind img$num <ButtonRelease-1> \
			"$canvas move button$num -1 -1"

		Info $oop button,hook,$num ""
	}

	$canvas bind img$num <Leave> "
		$canvas itemconfigure border$num -outline Black
		NSMiscPopup::Event $oop leave-button
		NSMainWindow::StatusText $oop {}
	"

	return
}

# NSMiscPopup::SetHook --
#
#	Set the hook.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMiscPopup::SetHook {oop hook} {

	if {[string length $hook]} {
		Info $oop hook $hook
		CallHook $oop open
	} elseif {[string length [Info $oop hook]]} {
		Info $oop hook ""
	}

	return
}

# NSMiscPopup::CallHook --
#
#	Call the hook.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMiscPopup::CallHook {oop message args} {

	return [uplevel #0 NSMiscPopup::[Info $oop hook] $oop $message $args]
}

# NSMiscPopup::EnterButton --
#
#	Display popup of choices.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMiscPopup::EnterButton {oop buttonNum} {

	set win [Info $oop win]
	set canvas [Info $oop canvas]
	set wText [Info $oop text]

	if {[lsearch -exact [angband inkey_flags] INKEY_CMD] == -1} return

	# Set the hook for this button
	set hookArgs [Info $oop button,args,$buttonNum]
	SetHook $oop hook_[Info $oop button,hook,$buttonNum]

	# See if there are any valid choices
	if {![eval CallHook $oop has_cmd $hookArgs]} {
		HideWin $oop
		return
	}

	# Set the list
	$wText delete 1.0 end
	eval CallHook $oop set_list $hookArgs

	set x [Info $oop button,x,$buttonNum]
	incr x [expr {[winfo rootx $canvas] + 9}]
	set y [expr {[winfo rooty $canvas] + [winfo height $canvas]}]

	set width [Info $oop maxWidth]
	incr width [expr {[$wText cget -padx] * 2}]

	set height [Info $oop maxHeight]
	incr height [expr {[$wText cget -pady] * 2}]

	incr width [expr {[$win cget -borderwidth] * 2}]
	incr height [expr {[$win cget -borderwidth] * 2}]

	# x is middle
	incr x [expr {0 - $width / 2}]
	if {$x < [winfo rootx $canvas]} {
		set x [winfo rootx $canvas]
	}

	set screenWidth [winfo screenwidth .]
	if {$x + $width > $screenWidth} {
		incr x [expr {$screenWidth - ($x + $width)}]
	}
	set screenHeight [winfo screenheight .]
	if {$y + $height > $screenHeight} {
		incr y [expr {$screenHeight - ($y + $height)}]
	}

	wm geometry $win ${width}x${height}+${x}+$y

	# Perhaps show the window later
	Event $oop enter-button

	return
}

# NSMiscPopup::Event --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMiscPopup::Event {oop event} {

	set who [Info $oop whoHasCursor]

	switch -- $event {
		enter-win {
			set who win
		}
		leave-win {
			set who ""
		}
		enter-button {
			set who button
		}
		enter-button2 {
			set who button2
		}
		leave-button {
			set who ""
		}
	}

	Info $oop whoHasCursor $who
	if {[string match enter-* $event]} {
		set delay 10
	} else {
		set delay 200
	}
	after cancel [Info $oop after]
	Info $oop after [after $delay NSMiscPopup::CheckWhoHasCursor $oop]

	return
}

# NSMiscPopup::CheckWhoHasCursor --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMiscPopup::CheckWhoHasCursor {oop} {

	set who [Info $oop whoHasCursor]

	switch -- $who {
		button {
			ShowWin $oop
		}
		button2 {
			HideWin $oop
		}
		win {
		}
		default {
			HideWin $oop
		}
	}

	Info $oop after ""

	return
}

# NSMiscPopup::ShowWin --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMiscPopup::ShowWin {oop} {

	if {[Info $oop win,visible]} return

	set win [Info $oop win]

	wm deiconify $win
	if {[Platform unix]} {
		raise $win
	}

	Info $oop win,visible 1

	return
}

# NSMiscPopup::HideWin --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMiscPopup::HideWin {oop} {

	if {![Info $oop win,visible]} return

	set win [Info $oop win]

	wm withdraw $win

	Info $oop win,visible 0

	return
}

# NSMiscPopup::SetList --
#
#	Clears the text and calls the hook to set the text.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMiscPopup::SetList {oop} {

	set win [Info $oop win]
	set textBox [Info $oop text]

	# Clear the text
	$textBox delete 1.0 end

	# Call the hook to set the list
	CallHook $oop set_list

	# Something is displayed
	Info $oop display something

	# No item is highlighted
	Info $oop current ""	

	return
}

# NSMiscPopup::Invoke --
#
#	Called when a list item is clicked. 
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMiscPopup::Invoke {oop} {

	set textBox [Info $oop text]
	set index [Info $oop current]
	set row [expr {[lindex [split $index .] 0] - 1}]

	HideWin $oop

	CallHook $oop invoke $row

	return
}

# NSMiscPopup::Motion --
#
#	Called when the mouse moves in a list item. 
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMiscPopup::Motion {oop index} {

	set textBox [Info $oop text]

	# If you invoke an item, hold down the mouse, and drag...
	if {![string length [Info $oop hook]]} return

	# No tracking while menu is up
	if {[Info $oop busy]} return

	# See if the item has changed
	if {$index == [Info $oop current]} return

	# An item is highlighted
	if {[string length [Info $oop current]]} {

		# Remove highlighting
		UnhighlightItem $oop [Info $oop current]
	}

	# An item is under the pointer
	if {[string length $index]} {

		# Highlight the item
		HighlightItem $oop $index
	}

	# Remember which item is highlighted
	Info $oop current $index

	return
}

# NSMiscPopup::HighlightItem --
#
#	Highlights a list item. 
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMiscPopup::HighlightItem {oop index} {

	set textBox [Info $oop text]
	set row [expr {[lindex [split $index .] 0] - 1}]

	# Highlight the item
	$textBox tag add HOT $index "$index lineend"
	$textBox tag raise HOT

	# Call the hook (to set the icon, for example)
	CallHook $oop highlight $row

	return
}

# NSMiscPopup::UnhighlightItem --
#
#	Removes highlighting from a list item. 
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMiscPopup::UnhighlightItem {oop index} {

	set win [Info $oop win]
	set textBox [Info $oop text]

	# Unhighlight the item
	$textBox tag remove HOT 1.0 end

	return
}

# NSMiscPopup::HasCursor --
#
#	See if the cursor is over the window. 
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMiscPopup::HasCursor {oop} {

	set pointerx [winfo pointerx .]
	set pointery [winfo pointery .]
	set window [winfo containing $pointerx $pointery]
	if {![string length $window]} {
		return 0
	}
	if {[string compare [winfo toplevel $window] [Info $oop win]]} {
		return 0
	}
	return 1
}

proc NSMiscPopup::hook_item {oop message args} {

	switch -- $message {

		open {
		}

		fresh {
			SetList $oop
		}

		close {
		}

		has_cmd {
			set where [lindex $args 0]
			set char [lindex $args 1]
			set other [lrange $args 2 end]
			set itemList [eval angband $where find $other]
			return [llength $itemList]
		}

		set_list {

			set where [lindex $args 0]
			set char [lindex $args 1]
			set other [lrange $args 2 end]

			Info $oop hook,where $where
			Info $oop hook,char $char
			
			set textBox [Info $oop text]
			set font [$textBox cget -font]
		
			# Get the list of matching item indexes
			set itemList [eval angband $where find $other]

			set maxWidth 0

			# Process each item
			foreach index $itemList {

				# Get item info
				angband $where info $index attrib

				if {[string equal $where floor]} {
					set attrib(char) [string index "abcdefghijklmnopqrstuvw" \
						[lsearch -exact $itemList $index]]
				}

				# Get the color
				set color [default_tval_to_attr $attrib(tval)]
		
				# Append the character and description
				$textBox insert end "$attrib(char)\) " TEXT \
					$attrib(name) [list ITEM_$index TEXT] "\n"
				$textBox tag configure ITEM_$index -foreground $color

				# Remember longest line
				set width [font measure $font "$attrib(char)\) $attrib(name)"]
				if {$width > $maxWidth} {
					set maxWidth $width
				}
			}
		
			# Delete trailing newline
			$textBox delete "end - 1 chars"
		
			# Keep a list of item indexes
			Info $oop match $itemList

			Info $oop maxWidth $maxWidth
			set fontHeight [font metrics $font -linespace]
			Info $oop maxHeight [expr {[llength $itemList] * $fontHeight}]
		}

		invoke {
			set row [lindex $args 0]
			set index [lindex [Info $oop match] $row]
			set where [Info $oop hook,where]
			set charCmd [Info $oop hook,char]
			if {[string equal $where floor]} {
				set attrib(char) [string index "abcdefghijklmnopqrstuvw" $row]
			} else {
				angband $where info $index attrib
			}
			angband keypress $charCmd$attrib(char)
		}

		highlight {
			set row [lindex $args 0]
			set index [lindex [Info $oop match] $row]
			NSRecall::RecallObject [Info $oop hook,where] $index
		}
	}

	return
}


proc NSMiscPopup::hook_cmd_pet {oop message args} {

	switch -- $message {

		open {
		}

		fresh {
			SetList $oop
		}

		close {
		}

		set_list {
			set textBox [Info $oop text]

			# Keep a list of invoke chars
			set match {}
			
			# Process each command
			foreach {char label} [NSRecall::PetCmdInfo mode] {

				if {[string equal $char $mode]} {
					set color [Value TERM_L_BLUE]
				} else {
					set color White
				}

				# Append the character and description
				$textBox insert end "$char\) " TEXT $label \
					[list ITEM_$char TEXT] "\n"
				$textBox tag configure ITEM_$char -foreground $color

				# Keep a list of chars and colors
				lappend match $char
				lappend colors $color
			}
		
			# Delete trailing newline
			$textBox delete "end - 1 chars"
		
			# Keep a list of chars and colors
			Info $oop match $match
			Info $oop color $colors
		}

		get_color {
			set row [lindex $args 0]
			return [lindex [Info $oop color] $row]
		}

		invoke {
			if {![Info $oop choosing]} return
			set row [lindex $args 0]
			set char [lindex [Info $oop match] $row]
			angband keypress $char
		}

		highlight {
		}
	}

	return
}

proc NSMiscPopup::hook_mindcraft {oop message args} {

	switch -- $message {

		open {
		}

		fresh {
			SetList $oop
		}

		close {
		}

		set_list {
		
			set textBox [Info $oop text]
		
			# Get the list of mindcraft powers
			set powerList [angband mindcraft get]

			# Keep a list of chars
			set match {}

			# Process each power
			foreach power $powerList {

				angband mindcraft info $power attrib
				if {!$attrib(okay)} continue

				# Append the character and description
				$textBox insert end "$attrib(char)\) " TEXT \
					$attrib(name) [list POWER_$attrib(char) TEXT] "\n"
				$textBox tag configure POWER_$attrib(char) -foreground White

				# Keep a list of chars
				lappend match $attrib(char)
			}
		
			# Delete trailing newline
			$textBox delete "end - 1 chars"
		
			# Keep a list of chars
			Info $oop match $match
		}

		get_color {

			return White
		}

		invoke {

			if {![Info $oop choosing]} return
			set row [lindex $args 0]
			set char [lindex [Info $oop match] $row]
			angband keypress $char
		}

		highlight {
		}
	}

	return
}

proc NSMiscPopup::hook_power {oop message args} {

	set powerChars "abcdefghijklmnopqrstuvwxyz0123456789"

	switch -- $message {

		open {
		}

		fresh {
			SetList $oop
		}

		close {
		}

		set_list {
		
			set textBox [Info $oop text]
		
			# Get the list powers
			set powerList [angband power get]

			set i 0

			# Check each power
			foreach power $powerList {

				# Get information about this power
				angband power info $power attrib

				# Get the power char
				set attrib(char) [string index $powerChars $i]

				set fill White
				if {$attrib(chance) == 100} {
					set fill gray70
				}

				# Append the character and description
				$textBox insert end "$attrib(char)\) " TEXT \
					$attrib(name) [list POWER_$attrib(char) TEXT] "\n"
				$textBox tag configure POWER_$attrib(char) -foreground $fill

				incr i
			}
		
			# Delete trailing newline
			$textBox delete "end - 1 chars"
		
			# Keep a list of inventory indexes
			Info $oop match $powerList
		}

		get_color {

			set row [lindex $args 0]
			set index [lindex [Info $oop match] $row]
			angband power info $index attrib
			set fill White
			if {$attrib(chance) == 100} {
				set fill gray70
			}
			return $fill
		}

		invoke {

			if {![Info $oop choosing]} return
			set row [lindex $args 0]
			angband keypress [string index $powerChars $row]
		}

		highlight {
		}

		context_menu {

			set menu [lindex $args 0]
			set row [lindex $args 1]
			set font [$menu cget -font]
						
			# No row is hit
			if {$row == -1} {
		
				$menu add command -label [mc Cancel] \
					-command {angband keypress \033}
				return 1
			}

			# Append a command to select the item
			set command "angband keypress [string index $powerChars $row]"
			$menu add command -label "Select This Power" -command $command \
				-font [BoldFont $font]
			$menu add separator
			$menu add command -label [mc Cancel] \
				-command {angband keypress \033}

			return 1
		}
	}

	return
}


proc NSMiscPopup::hook_spell {oop message args} {

	switch -- $message {

		open {
		}

		fresh {
			SetList $oop
		}

		close {
		}

		set_list {
			set textBox [Info $oop text]
		
			# Get the book number
			set bookNum [Info $oop display,what]

			# Get a list of legal spells
			set spellList [angband spell find $bookNum -tester yes]

			# Process each spell
			foreach spell $spellList {

				# Get information about this spell
				angband spell info $bookNum $spell attrib

				# Colorize the spell name (from OAngband)
				switch -- $attrib(info) {
					unknown {
						set fill gray70
						
						# The character can learn this spell
						if {[angband player new_spells] && 
							($attrib(level) <= [angband player level])} {
							set fill [Value TERM_L_GREEN]
						}
					}
					untried {
						set fill [Value TERM_L_BLUE]
					}
					default {
						set fill White
					}
				}
	
				# Append the character and description
				$textBox insert end "$attrib(char)\) " TEXT \
					$attrib(name) [list SPELL_$attrib(char) TEXT] "\n"
				$textBox tag configure SPELL_$attrib(char) -foreground $fill

				# Keep a list of spell chars
				lappend match $attrib(char)
			}
		
			# Delete trailing newline
			$textBox delete "end - 1 chars"
		
			# Keep a list of spell chars
			Info $oop match $spellList
		}

		get_color {

			set row [lindex $args 0]
			set index [lindex [Info $oop match] $row]
			set bookNum [Info $oop display,what]
			angband spell info $bookNum $spell attrib

			# Colorize the spell name (from OAngband)
			switch -- $attrib(info) {
				unknown {
					set fill gray70
					
					# The character can learn this spell
					if {[angband player new_spells] && 
						($attrib(level) <= [angband player level])} {
						set fill [Value TERM_L_GREEN]
					}
				}
				untried {
					set fill [Value TERM_L_BLUE]
				}
				default {
					set fill White
				}
			}
			return $fill
		}

		invoke {

			if {![Info $oop choosing]} return
			set row [lindex $args 0]
			set index [lindex [Info $oop match] $row]
			set bookNum [Info $oop display,what]
			angband spell info $bookNum $index attrib
			angband keypress $attrib(char)
		}

		doubleclick {

			if {[string compare [angband inkey_flags] INKEY_CMD]} return
			set row [lindex $args 0]
			set spellNum [lindex [Info $oop match] $row]
			set bookNum [Info $oop display,what]
			set index [angband inventory find -limit 1 -k_idx $bookNum]
			if {![llength $index]} return
			angband inventory info $index attrib
			set charBook $attrib(char)
			angband spell info $bookNum $spellNum attrib
			set charSpell $attrib(char)
			GetMagicInfo charCmd spellType
			if {$attrib(known)} {
				DoUnderlyingCommand $charCmd$charBook$charSpell
			} else {
				if {[angband player new_spells] && ($attrib(level) <= \
					[angband player level])} {
					set command G
					append command {\n}
					append command $charBook
					if {[string equal $spellType spell]} {
						append command $charSpell
					}
					DoUnderlyingCommand $command
				}
			}
		}

		highlight {

			if {![Info $oop choosing] && \
				[string compare [angband inkey_flags] INKEY_CMD]} return
			set row [lindex $args 0]
			set index [lindex [Info $oop match] $row]
			set bookNum [Info $oop display,what]
			NSRecall::RecallSpell $bookNum $index
		}

		context_menu {

			set menu [lindex $args 0]
			set row [lindex $args 1]
			set font [$menu cget -font]
						
			# No row is hit
			if {$row == -1} {
		
				$menu add command -label [mc Cancel] \
					-command {angband keypress \033}
				return 1
			}

			if {[string compare [angband inkey_flags] INKEY_SPELL]} {
				return 0
			}

			set bookNum [Info $oop display,what]
			set spellNum [lindex [Info $oop match] $row]
			angband spell info $bookNum $spellNum attrib

			GetMagicInfo charCmd spellType

			# Append a command to select the item
			set command "angband keypress $attrib(char)"
			$menu add command -label "Select This [string totitle $spellType]" \
				-command $command -font [BoldFont $font]
			$menu add separator
			$menu add command -label [mc Cancel] \
				-command {angband keypress \033}

			return 1
		}
	}

	return
}

# NSMiscPopup::OptionChanged --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMiscPopup::OptionChanged {oop info keyword} {

	set setting [Info $oop $info]
	Value choicewindow,$keyword $setting
	switch -- $keyword {
		showicon {
			if {$setting} {
				grid [Info $oop icon]
			} else {
				grid remove [Info $oop icon]
			}
		}
	}

	return
}

# NSMiscPopup::ValueChanged_font_miscPopup --
#
#	Called when the font,miscPopup value changes.
#	Updates the Recall Window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMiscPopup::ValueChanged_font_miscPopup {oop} {

	set text [Info $oop text]
	$text configure -font [Value font,miscPopup]

	return
}
