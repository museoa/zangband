# File: choose-monster.tcl

# Purpose: the Choose-Monster Window and related commands

#
# Copyright (c) 1997-2001 Tim Baker
#
# This software may be copied and distributed for educational, research, and
# not for profit purposes provided that this copyright and statement are
# included in all such copies.
#

namespace eval NSChooseMonster {

	variable Priv

# namespace eval NSChooseMonster
}

# NSChooseMonster::InitModule --
#
#	One-time-only-ever initialization.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::InitModule {} {

	variable Priv

	MsgCatInit know

	set Priv(find,string) ""
	set Priv(find,fromStart) 1

	NSObject::New NSChooseMonster

	return
}

# NSChooseMonster::NSChooseMonster --
#
#	Object constructor called by NSObject::New().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::NSChooseMonster {oop} {

	InitWindow $oop

	Info $oop group,current -1
	Info $oop member,current -1

	set win [Info $oop win]

	# The user can type ESCAPE to cancel
	# The user can type ENTER to use the selected monster (if any)
	# The user can type the d_char of any monster race (if valid)
	bind $win <KeyPress-Escape> \
		"NSChooseMonster::Close $oop"
	bind $win <KeyPress-Return> \
		"NSChooseMonster::Accept $oop"
	bind $win <KeyPress> \
		"NSChooseMonster::TrySymbol $oop %A"

	# Searching
	bind $win <Control-KeyPress-f> \
		"NSChooseMonster::Find $oop 0"
	bind $win <Control-KeyPress-g> \
		"NSChooseMonster::Find $oop 1"

	NSWindowManager::RegisterWindow choosemonster $win \
		"GetDefaultGeometry $win reqwidth main" "" \
		"NSChooseMonster::DisplayCmd $oop"		

	#
	# Global list of application windows
	#

	Global choosemonster,oop $oop
	Window choosemonster $win

	return
}

# NSChooseMonster::~NSChooseMonster --
#
#	Object destructor called by NSObject::Delete().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::~NSChooseMonster {oop} {

	NSValueManager::RemoveClient listBG [Info $oop listBG,clientId]
	NSValueManager::RemoveClient font,knowledge [Info $oop font,clientId]

	return
}

# NSChooseMonster::Info --
#
#	Query and modify info.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::Info {oop info args} {

	global NSChooseMonster

	# Verify the object
	NSObject::CheckObject NSChooseMonster $oop

	# Set info
	if {[llength $args]} {
		switch -- $info {
			default {
				set NSChooseMonster($oop,$info) [lindex $args 0]
			}
		}

	# Get info
	} else {
		switch -- $info {
			groupCanvas {
				set canvistId [Info $oop group,canvistId]
				return [NSCanvist::Info $canvistId canvas]
			}
			memberCanvas {
				set canvistId [Info $oop member,canvistId]
				return [NSCanvist::Info $canvistId canvas]
			}
			default {
				return $NSChooseMonster($oop,$info)
			}
		}
	}

	return
}

# NSChooseMonster::InitWindow --
#
#	Create the window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::InitWindow {oop} {

	set win .choosemonster$oop
	toplevel $win
	wm title $win "Choose Monster"

	wm transient $win [Window main]

	# Do stuff when window closes
	wm protocol $win WM_DELETE_WINDOW "NSChooseMonster::Close $oop"

	Info $oop win $win

	InitMenus $oop

	MakeDivider $win.divider1 x

	#
	# Group List
	#

	set font [Value font,knowledge]
	set cw [font measure $font "W"]
#	set width [expr {$cw * 26}]
	set width [CalcGroupListWidth $oop]
	set iconSize [expr {[icon size] + 8}]

	frame $win.frameGroup \
		-borderwidth 1 -relief sunken
	set canvistId [NSObject::New NSCanvist $win.frameGroup $iconSize $width 300 \
		"NSChooseMonster::NewItemCmd $oop" "NSChooseMonster::HighlightItemCmd $oop"]
	set canvas [NSCanvist::Info $canvistId canvas]
	$canvas configure -height [expr {40 * 5}]
	$canvas configure -background [Value listBG]
	$canvas configure -yscrollcommand "$win.frameGroup.scroll set"
	scrollbar $win.frameGroup.scroll \
		-command "$canvas yview" -orient vert

#	bind $win.frameGroup.scroll <Map> "eval %W set \[$canvas yview]"
	NSUtils::SynchScrollBar $canvas $win.frameGroup.scroll

	# Do something when a group item is selected
	NSCanvist::Info $canvistId selectionCmd \
		"NSChooseMonster::SelectionChanged_Group $oop"

	# Update ourself when the listBG value changes
	Info $oop listBG,clientId \
		[NSValueManager::AddClient listBG \
		"NSChooseMonster::ValueChanged_listBG $oop"]

	# Update ourself when the font,knowledge value changes
	Info $oop font,clientId \
		[NSValueManager::AddClient font,knowledge \
		"NSChooseMonster::ValueChanged_font_knowledge $oop"]

	pack $win.frameGroup.scroll -side right -fill y
	pack $canvas -side left -expand yes -fill both -anchor nw

	Info $oop group,canvistId $canvistId

	#
	# Member List
	#

#	set width [expr {$cw * 40}]
	set width 350

	frame $win.frameMember \
		-borderwidth 1 -relief sunken
	set canvistId [NSObject::New NSCanvist $win.frameMember $iconSize $width 300 \
		"NSChooseMonster::NewItemCmd $oop" "NSChooseMonster::HighlightItemCmd $oop"]
	set canvas [NSCanvist::Info $canvistId canvas]
#	$canvas configure -height [expr {40 * 5}]
	$canvas configure -background [Value listBG]
	$canvas configure -yscrollcommand "$win.frameMember.scroll set"
	scrollbar $win.frameMember.scroll \
		-command "$canvas yview" -orient vert

#	bind $win.frameMember.scroll <Map> "eval %W set \[$canvas yview]"
	NSUtils::SynchScrollBar $canvas $win.frameMember.scroll

	# Do something when a member item is invoked.
	NSCanvist::Info $canvistId invokeCmd \
		"NSChooseMonster::Invoke_Member $oop"

	# Do something when a member item is selected.
	NSCanvist::Info $canvistId selectionCmd \
		"NSChooseMonster::SelectionChanged_Member $oop"

	Info $oop member,canvistId $canvistId

	pack $win.frameMember.scroll -side right -fill y
	pack $canvas -side left -expand yes -fill both -anchor nw

	#
	# Geometry
	#

	grid rowconfig $win 0 -weight 0
	grid rowconfig $win 1 -weight 1
	grid columnconfig $win 0 -weight 0
	grid columnconfig $win 1 -weight 1

	grid $win.divider1 \
		-row 0 -column 0 -rowspan 1 -columnspan 2 -sticky ew
	grid $win.frameGroup \
		-row 1 -column 0 -rowspan 1 -columnspan 1 -sticky ns
	grid $win.frameMember \
		-row 1 -column 1 -rowspan 1 -columnspan 1 -sticky news

	return
}

# NSChooseMonster::InitMenus --
#
#	Create the menus.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::InitMenus {oop} {

	global NSMenu

	set win [Info $oop win]
	set mod "Ctrl"

	#
	# Menu bar
	#

	set mbar [NSObject::New NSMenu $win -tearoff 0 \
		-postcommand "NSChooseMonster::SetupMenus $oop" -identifier MENUBAR]
	Info $oop mbar $mbar

	#
	# Choose
	#

	NSObject::New NSMenu $mbar -tearoff 0 -identifier MENU_CHOOSE
	NSMenu::MenuInsertEntry $mbar -end MENUBAR -type cascade \
		-menu MENU_CHOOSE -label "Choose" -underline 0 \
		-identifier M_CHOOSE

	set entries {}
	lappend entries [list -type command -label [mc Find...] \
		-command "NSChooseMonster::Find $oop 0" \
		-accelerator $mod+F -underline 0 -identifier E_FIND]
	lappend entries [list -type command -label [mc "Find Again"] \
		-command "NSChooseMonster::Find $oop 1" \
		-accelerator $mod+G -underline 6 -identifier E_FIND_AGAIN]
	lappend entries [list -type separator]
	lappend entries [list -type command -label [mc Close] \
		-command "NSChooseMonster::Close $oop" -underline 0 \
		-accelerator $mod+W -identifier E_CLOSE]
	
	NSMenu::MenuInsertEntries $mbar -end MENU_CHOOSE $entries

	return
}

# NSChooseMonster::SetupMenus --
#
#	Prepare to post the menus.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::SetupMenus {oop mbarID} {

	lappend identList E_FIND E_FIND_AGAIN E_CLOSE
		
	NSMenu::MenuEnable $mbarID $identList

	return
}

# NSChooseMonster::DisplayCmd --
#
#	Called by NSWindowManager::Display().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::DisplayCmd {oop message first args} {

	variable Priv
	
	set canvas [Info $oop groupCanvas]

	switch -- $message {
		preDisplay {
			Info $oop allowUnique 0
			Info $oop returnIndex 0
			if {[llength $args]} {
				if {[lsearch -exact $args -unique] != -1} {
					Info $oop allowUnique 1
				}
				if {[lsearch -exact $args -r_idx] != -1} {
					Info $oop returnIndex 1
				}
			}

			# Hmmm. Do this because the canvas was resized above.
			update idletasks
			
			SetHook $oop choose_hook_monster
		}
		postDisplay {
			focus $canvas
		}
		postWithdraw {
			Unmap $oop
		}
	}

	return
}

# NSChooseMonster::Close --
#
#	Do something when closing the window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::Close {oop} {

	angband keypress \033

	return
}

# NSChooseMonster::Unmap --
#
#	Do something when unmapping the window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::Unmap {oop} {

	set canvistId [Info $oop group,canvistId]
	NSCanvist::DeleteAll $canvistId

	set canvistId [Info $oop member,canvistId]
	NSCanvist::DeleteAll $canvistId

	return
}

# NSChooseMonster::SetHook --
#
#	Set the hook..
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::SetHook {oop hook} {

	Info $oop hook $hook
	SetList_Group $oop

	return
}

# NSChooseMonster::CallHook --
#
#	Call the hook.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::CallHook {oop message args} {

	return [uplevel #0 NSChooseMonster::[Info $oop hook] $oop $message $args]
}

# NSChooseMonster::SelectionChanged_Group --
#
#	When a "group" list item is selected, display members in the
#	"member" list.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::SelectionChanged_Group {oop canvistId select deselect} {

	# If nothing was selected, clear the member list
	if {![llength $select]} {
		set canvistId [Info $oop member,canvistId]
		NSCanvist::DeleteAll $canvistId
		Info $oop group,current -1
		return
	}

	# Get the (first) row
	set row [lindex $select 0]
	Info $oop group,current $row

	# Display members in that group
	SetList_Member $oop $row

	return
}

# NSChooseMonster::SelectionChanged_Member --
#
#	Do something when a "member" list item is selected.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::SelectionChanged_Member {oop canvistId select deselect} {

	# Do nothing if no new row was selected
	if {![llength $select]} {
		Info $oop member,current -1
		return
	}

	# Get the (first) row
	set row [lindex $select 0]
	Info $oop member,current $row

	# Call the hook to do stuff
	CallHook $oop select_member $row

	return
}

# NSChooseMonster::Invoke_Member --
#
#	Do something when a "member" list item is invoked.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::Invoke_Member {oop canvistId x y} {

	set row [NSCanvist::PointToRow $canvistId $x $y]
	if {$row == -1} return

	# Sometimes return r_info[] index
	set r_idx [lindex [Info $oop member,match] $row]

	# Sometimes return r_info[] index
	if {[Info $oop returnIndex]} {
		angband keypress [format "%d\n" $r_idx]
		return
	}

	angband keypress [angband r_info set $r_idx d_char]

	return
}

# NSChooseMonster::Accept --
#
#	If the user types ENTER, and a monster is selected, then
#	"angband keypress" the monster's d_char.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::Accept {oop} {

	set row [Info $oop member,current]
	if {$row == -1} return

	# Get the r_info[] index
	set r_idx [lindex [Info $oop member,match] $row]

	# Sometimes return r_info[] index
	if {[Info $oop returnIndex]} {
		angband keypress [format "%d\n" $r_idx]
		return
	}
	
	angband keypress [angband r_info set $r_idx d_char]

	return
}

# NSChooseMonster::TrySymbol --
#
#	If the given symbol is a valid monster d_char, then
#	"angband keypress" it.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::TrySymbol {oop symbol} {

	if {[Info $oop returnIndex]} return

	if {[string length $symbol] != 1} return

	# This is to overcome a limitation of the "quasi-boolean"
	# implementation, which must be one of "yes", "no" or "ignore"
	if {[Info $oop allowUnique]} {
		set allowUnique yes
	} else {
		set allowUnique no
	}
	
	set match [angband r_info find -d_char $symbol -limit 1 \
		-unique $allowUnique]
	if {![llength $match]} return
	angband keypress $symbol

	return
}

# NSChooseMonster::SetList_Group --
#
#	Set the group list.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::SetList_Group {oop} {

	set win [Info $oop win]
	set canvistId [Info $oop group,canvistId]

	# Clear the list
	NSCanvist::DeleteAll $canvistId

	# Calculate row height. This actually only needs to be done
	# when the font,knowledge font changes.
	set rowHgt [font metrics [Value font,knowledge] -linespace]
	if {[icon size] > $rowHgt} {
		set rowHgt [icon size]
	}
	incr rowHgt 8

	# Set row height of group list
	set canvistId [Info $oop group,canvistId]
	set canvas [NSCanvist::Info $canvistId canvas]
	NSCanvist::Info $canvistId rowHgt $rowHgt
	$canvas configure -yscrollincrement $rowHgt

	# Set row height of member list
	set canvistId [Info $oop member,canvistId]
	set canvas [NSCanvist::Info $canvistId canvas]
	NSCanvist::Info $canvistId rowHgt $rowHgt
	$canvas configure -yscrollincrement $rowHgt

	# Call hook to set the group list
	CallHook $oop set_list_group

	Info $oop group,current -1

	# Hack -- Clear the "member" list
	NSCanvist::DeleteAll [Info $oop member,canvistId]

	return
}

# NSChooseMonster::SetList_Member --
#
#	Set the member list.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::SetList_Member {oop group} {

	set canvistId [Info $oop member,canvistId]

	# Clear the list
	NSCanvist::DeleteAll $canvistId

	Info $oop member,current -1

	# Call hook to set the member list
	CallHook $oop set_list_member $group

	return
}

# NSChooseMonster::NewItemCmd --
#
#	Called by NSCanvist::InsertItem() to create a list row.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::NewItemCmd {oop canvistId y icon text} {

	set c [NSCanvist::Info $canvistId canvas]
	set lineHeight [NSCanvist::Info $canvistId rowHgt]
	set font [Value font,knowledge]

	set cw [font measure $font "W"]
	set fw [font measure $font $text]
	set fh [font metrics $font -linespace]
	set diff [expr {int([expr {($lineHeight - $fh) / 2}])}]

	# Selection rectangle around everything
	lappend itemIdList [$c create rectangle 2 [expr {$y + 2}] \
		[expr {([winfo width $c] - 1) - 2}] [expr {$y + $lineHeight - 2}] \
		-fill {} -outline {} -tags enabled -width 2.0]

	# Image
	set iw [icon size]
	set ih [icon size]
	set wid [expr {[icon size] + 8}]
	set xdiff [expr {int([expr {($wid - $iw) / 2}])}]
	set ydiff [expr {int([expr {($lineHeight - $ih) / 2}])}]
	lappend itemIdList [$c create widget $xdiff [expr {$y + $ydiff}] \
		-assign $icon -tags enabled]

	# Text
	set fill White
	lappend itemIdList [$c create text [expr {$wid + 1}] [expr {$y + $diff}] \
		-text $text -anchor nw -font $font -fill $fill -tags enabled]

	return $itemIdList
}

# NSChooseMonster::HighlightItemCmd --
#
#	Called by NSCanvist::Select() to highlight a row.
#
# Arguments:
#	oop					OOP ID. See above.
#	canvistId					OOP ID of NSCanvist object.
#	state					1 or 0 highlight state.
#	args					List of canvas item ids
#
# Results:
#	What happened.

proc NSChooseMonster::HighlightItemCmd {oop canvistId state args} {

	set canvas [NSCanvist::Info $canvistId canvas]
	set itemIdList $args

	set idRect [lindex $itemIdList 0]

	if {[NSUtils::HasFocus $canvas]} {
		set fill [Value listHilite]
	} else {
		set fill [Value listInactive]
	}

	if {$state} {
		$canvas itemconfigure $idRect -outline $fill

	} else {
		$canvas itemconfigure $idRect -outline {}
	}

	return
}

# NSChooseMonster::Find --
#
#	Simple search routine to look for a member by name.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::Find {oop again} {

	variable Priv

	# Repeat the last search
	if {$again && [string length $Priv(find,string)]} {
		set string $Priv(find,string)

	# Enter a string to find, start from the beginning
	} else {

		# Ask the user for a name
		set string [NSUtils::StringBox -title [mc Find] \
			-initial $Priv(find,string) -prompt Name \
			-buttons [list [mc Find] [mc Cancel]] -parent [Info $oop win]]
		if {![string length $string]} return

		# Clean up after the dialog
		update

		# Reset search parameters
		set Priv(find,string) $string
	}

	# Default to searching from the beginning
	set group 0
	set member 0

	# Search in selected group, if any
	if {!$Priv(find,fromStart)} {	
		set groupCurr [Info $oop group,current]
		set memberCurr [Info $oop member,current]
		if {$groupCurr != -1} {
			set group $groupCurr
		}
		if {$memberCurr != -1} {
			set member [expr {$memberCurr + 1}]
		}
	}

	set max [NSCanvist::Info [Info $oop group,canvistId] count]

	# Compare lowercase
	set string [string tolower $string]

	for {set i $group} {$i < $max} {incr i} {

		set match [lrange [CallHook $oop member_list $i] $member end]

		foreach index $match {
	
			# Get the member name
			set name2 [CallHook $oop member_name $index]

			# Compare lowercase
			set name2 [string tolower $name2]
	
			# Found a match
			if {[string first $string $name2] != -1} {
	
				set canvistId [Info $oop group,canvistId]
	
				# The new group is not displayed
				if {![NSCanvist::IsRowSelected $canvistId $i]} {

					# Clear the current selection
					NSCanvist::RemoveSelection $canvistId

					# Select the new group. As a side effect, the
					# SetList_Member() is called to display the monsters in
					# the group.
					NSCanvist::UpdateSelection $canvistId $i {}
					NSCanvist::See $canvistId $i

				# The new group is already selected
				} else {
				}
	
				set canvistId [Info $oop member,canvistId]
	
				# Select the matching member, deselecting all others
				NSCanvist::UpdateSelection $canvistId $member all
				NSCanvist::See $canvistId $member
	
				# Don't search from start next time
				set Priv(find,fromStart) 0

				# Done
				return
			}
			incr member
		}
		set member 0
	}

	# If we didn't search from the start, then wrap around
	if {!$Priv(find,fromStart)} {
		set Priv(find,fromStart) 1
		Find $oop 1
		return
	}

	return
}

proc NSChooseMonster::choose_hook_monster {oop message args} {

	switch -- $message {

		set_list_group {

			set canvistId [Info $oop group,canvistId]

			foreach {title findSpec} [Global groups,r_info] {

				# Skip unique monster group
				if {![Info $oop allowUnique]} {
					if {[string first "-unique yes" $findSpec] != -1} continue
				}

				# Find the last monster in the group
				set match [eval angband r_info find -limit 1 \
					-backwards -unique no $findSpec]

				# Get the icon
				set icon [angband r_info info [lindex $match 0] icon]
		
				# Add this group to the list
				NSCanvist::Insert $canvistId end $icon [mc $title]
			}
		}

		set_list_member {

			set canvistId [Info $oop member,canvistId]

			set group [lindex $args 0]
			set findSpec [lindex [Global groups,r_info] [expr {$group * 2 + 1}]]

			# Get a list of monsters in the group
			set match [eval angband r_info find -unique no $findSpec]

			# Add each match to the list
			foreach index $match {
		
				# Get the icon and name
				set d_char [angband r_info set $index d_char]
				set icon [angband r_info info $index icon]
				set name [angband r_info info $index name]
		
				# Append match to the list
				if {[Info $oop returnIndex]} {
					NSCanvist::Insert $canvistId end $icon $name
				} else {
					NSCanvist::Insert $canvistId end $icon "$name ($d_char)"
				}
			}

			# Keep a list of matching indexes
			Info $oop member,match $match
		}

		select_member {
			set row [lindex $args 0]
			set r_idx [lindex [Info $oop member,match] $row]
			NSRecall::RecallMonster $r_idx
		}

		group_names {
			set names {}
			foreach {title findSpec} [Global groups,r_info] {
				lappend names $title
			}
			return $names
		}

		member_name {
			return [angband r_info info [lindex $args 0] name]
		}

		member_list {
			set group [lindex $args 0]
			set findSpec [lindex [Global groups,r_info] [expr {$group * 2 + 1}]]
			return [eval angband r_info find -unique no $findSpec]
		}
	}

	return
}

# NSChooseMonster::CalcGroupListWidth --
#
#	Returns the desired width of the group list.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::CalcGroupListWidth {oop} {

	variable Priv

	# Get the desired font
	set font [Value font,knowledge]

	set maxWidth 100
	
	foreach hook choose_hook_monster {
		foreach name [$hook $oop group_names] {
			set width [font measure $font $name]
			if {$width > $maxWidth} {
				set maxWidth $width
			}
		}
	}

	return [expr {[icon size] + 8 + $maxWidth + 20 + 4}]
}

# NSChooseMonster::ValueChanged_font_knowledge --
#
#	Called when the font,knowledge value changes.
#	Updates the Knowledge Window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::ValueChanged_font_knowledge {oop} {

	# Get the desired font
	set font [Value font,knowledge]

	# Calculate row height
	set rowHgt [font metrics $font -linespace]
	if {[icon size] > $rowHgt} {
		set rowHgt [icon size]
	}
	incr rowHgt 8

	# Set row height of group list
	set canvistId [Info $oop group,canvistId]
	set canvas [NSCanvist::Info $canvistId canvas]
	NSCanvist::Info $canvistId rowHgt $rowHgt
	$canvas configure -yscrollincrement $rowHgt

	# Set the width of the group list
	set oldWidth [winfo width $canvas]
	set newWidth [CalcGroupListWidth $oop]
	$canvas configure -width $newWidth

	# Set the scrollregion to prevent horizontal scrolling
	scan [$canvas cget -scrollregion] "%s %s %s %s" x1 y1 x2 y2
	$canvas configure -scrollregion "$x1 $y1 $newWidth $y2"

	# Hack -- Resize the toplevel so the member list is not resized
	set diff [expr {$newWidth - $oldWidth}]
	if {$diff} {
		set win [Info $oop win]
		set newWidth [expr {[winfo width $win] + $diff}]
		NSToplevel::SetTotalWidth $win $newWidth
	}

	return
}

# NSChooseMonster::ValueChanged_listBG --
#
#	Called when the listBG value changes.
#	Updates the Knowledge Window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseMonster::ValueChanged_listBG {oop} {

	set color [Value listBG]
	
	[Info $oop groupCanvas] configure -background $color
	[Info $oop memberCanvas] configure -background $color

	return
}

