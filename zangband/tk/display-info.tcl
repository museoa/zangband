# File: display-info.tcl

# Purpose: the Display Info Window and related commands

#
# Copyright (c) 1997-2001 Tim Baker
#
# This software may be copied and distributed for educational, research, and
# not for profit purposes provided that this copyright and statement are
# included in all such copies.
#

namespace eval NSDisplayInfo {

	variable Priv

# namespace eval NSDisplayInfo
}

# NSDisplayInfo::InitModule --
#
#	One-time-only-ever initialization.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSDisplayInfo::InitModule {} {

	NSObject::New NSDisplayInfo

	return
}

# NSDisplayInfo::NSDisplayInfo --
#
#	Object constructor called by NSObject::New().
#
# Arguments:
#	oop					OOP ID. See above.
#
# Results:
#	What happened.

proc NSDisplayInfo::NSDisplayInfo {oop} {

	InitWindow $oop

	set win [Info $oop win]

	NSWindowManager::RegisterWindow displayinfo $win \
		"GetDefaultGeometry $win main main" "" \
		"NSDisplayInfo::DisplayCmd $oop"

	# Update ourself when the font for the Recall Window changes
	NSValueManager::AddClient font,recall \
		"NSDisplayInfo::ValueChanged_font_recall $oop"

	#
	# Global list of application windows
	#

	Global displayinfo,oop $oop
	Window displayinfo $win

	return
}

# NSDisplayInfo::Info --
#
#	Query and modify info.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSDisplayInfo::Info {oop info args} {

	global NSDisplayInfo

	# Set info
	if {[llength $args]} {
		switch -- $info {
			default {
				set NSDisplayInfo($oop,$info) [lindex $args 0]
			}
		}

	# Get info
	} else {
		switch -- $info {
			default {
				return $NSDisplayInfo($oop,$info)
			}
		}
	}

	return
}

# NSDisplayInfo::InitWindow --
#
#	Description.
#
# Arguments:
#	oop					OOP ID. See above.
#
# Results:
#	What happened.

proc NSDisplayInfo::InitWindow {oop} {

	global Angband

	set win .displayinfo$oop
	toplevel $win
	wm title $win "Monster Recall"
	wm resizable $win yes yes
	wm transient $win [Window main]

	# Do stuff when window closes
	wm protocol $win WM_DELETE_WINDOW "NSDisplayInfo::Close $oop"

	# Set instance variables
	Info $oop win $win

	# Create menus
	InitMenus $oop

	set frame $win.recall
	frame $frame \
		-borderwidth 1 -relief sunken -background Black

	# Canvas to display icon
	set iconSize [expr {[icon size] + 8}]
	set canvas $frame.icon
	canvas $canvas \
		-borderwidth 0 -width $iconSize -height $iconSize -background Black \
		-highlightthickness 0

	# Display monster icon here
	$canvas create widget \
		6 6 -tags icon

	text $frame.text \
		-wrap word -width 55 -height 15 -font [Value font,recall] \
		-borderwidth 0 -setgrid no -highlightthickness 0 \
		-padx 4 -pady 2 -background Black -foreground White \
		-cursor "" -yscrollcommand "$frame.yscroll set"
	scrollbar $frame.yscroll \
		-orient vertical -command "$frame.text yview"
	bindtags $frame.text [list $frame.text $win all]

	pack $frame \
		-expand yes -fill both

	grid rowconfig $frame 0 -weight 1
	grid columnconfig $frame 0 -weight 0
	grid columnconfig $frame 1 -weight 1
	grid columnconfig $frame 2 -weight 0

	grid $frame.icon -in $frame \
		-row 0 -column 0 -rowspan 1 -columnspan 1 -sticky ns
	grid $frame.text -in $frame \
		-row 0 -column 1 -rowspan 1 -columnspan 1 -sticky news
	grid $frame.yscroll -in $frame \
		-row 0 -column 2 -rowspan 1 -columnspan 1 -sticky ns

	# Set instance variables
	Info $oop text $frame.text

	bind $win <Control-KeyPress-w> "NSDisplayInfo::Close $oop"

	#
	# Feed Term when keys pressed
	#

	bind $win <KeyPress> {
		angband keypress %A
	}

	return
}

# NSDisplayInfo::InitMenus --
#
#	Description.
#
# Arguments:
#	oop					OOP ID. See above.
#
# Results:
#	What happened.

proc NSDisplayInfo::InitMenus {oop} {
}

# NSDisplayInfo::SetupMenus --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSDisplayInfo::SetupMenus {oop mbarId} {
}

# NSDisplayInfo::DisplayCmd --
#
#	Called by NSWindowManager::Display().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSDisplayInfo::DisplayCmd {oop message first args} {

	set win [Info $oop win]
	set text [Info $oop text]
	
	switch -- $message {
		preDisplay {
			set option [lindex $args 0]
			switch -- $option {
				monster {
					DisplayMonster $oop [lindex $args 1]
				}
				}
				warrior_probe_desc {
					DisplayWPD $oop [lindex $args 1]
				}
			}
		}
		postDisplay {
		}
		reDisplay {
		}
		postWithdraw {
			# Prevent useless animations
			$win.recall.icon itemconfigure icon -assign {icon none 0}
		}
	}

	return
}

# NSDisplayInfo::Close --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSDisplayInfo::Close {oop} {

	angband keypress \033

	return
}

# NSDisplayInfo::ValueChanged_font_recall --
#
#	Called when the font,recall value changes.
#	Updates the Recall Monster Window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSDisplayInfo::ValueChanged_font_recall {oop} {

	[Info $oop text] configure -font [Value font,recall]

	return
}

# NSDisplayInfo::DisplayMonster --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSDisplayInfo::DisplayMonster {oop r_idx} {

	set win [Info $oop win]
	set text [Info $oop text]

	# Set the window title
	wm title $win "Monster Memory"

	if {![angband r_info info $r_idx unique]} {
		set name "The "
	}
	append name [angband r_info info $r_idx name]:
	set icon [angband r_info info $r_idx icon]
	set memory [angband r_info info $r_idx memory]

	$win.recall.icon itemconfigure icon -assign $icon

	$text delete 1.0 end
	$text insert end $name TAG_STYLE
	$text tag configure TAG_STYLE -foreground [Value TERM_L_RED]
	$text insert end \n$memory

	# Hack -- Also display in the Recall Window
	NSRecall::RecallMonster $r_idx

	return
}



# NSDisplayInfo::DisplayWPD --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSDisplayInfo::DisplayWPD {oop message} {

	set win [Info $oop win]
	set text [Info $oop text]

	# Set the window title
	wm title $win "Pseudo-Probe"

	$win.recall.icon itemconfigure icon -assign [angband player icon]

	$text delete 1.0 end
	$text insert end "Warrior Pseudo-Probing Ability:" TAG_STYLE
	$text tag configure TAG_STYLE -foreground [Value TERM_L_BLUE]
	$text insert end \n$message

	return
}
