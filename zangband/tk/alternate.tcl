# File: alternate.tcl

# Purpose: the Alternate Window and related commands

#
# Copyright (c) 1997-2001 Tim Baker
#
# This software may be copied and distributed for educational, research, and
# not for profit purposes provided that this copyright and statement are
# included in all such copies.
#

namespace eval NSAlternate {

	variable MenuString

# namespace eval NSAlternate
}

# NSAlternate::InitModule --
#
#	One-time-only-ever initialization.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::InitModule {} {

	MsgCatInit

	NSModule::LoadIfNeeded NSIconBrowser

	NSObject::New NSAlternate

	return
}

# NSAlternate::CloseModule --
#
#	One-time-only-ever cleanup.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::CloseModule {} {

	catch {
		destroy [Window alternate]
	}

	return
}

# NSAlternate::NSAlternate --
#
#	Object constructor called by NSObject::New().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::NSAlternate {oop} {

	Info $oop alternate,current -1
	Info $oop frame,current -1
	Info $oop alternate,current2 -1
	Info $oop frame,current2 -1
	Info $oop frame,lastIndex -1

	InitWindow $oop

	set win [Info $oop win]

	NSWindowManager::RegisterWindow alternate $win \
		"GetDefaultGeometry $win reqwidth reqheight" "" "NSAlternate::DisplayCmd $oop"

	# Update ourself when the listBG value changes
	Info $oop listBG,clientId \
		[NSValueManager::AddClient listBG \
		"NSAlternate::ValueChanged_listBG $oop"]

	# Destroy the object along with the toplevel (later)
	NSUtils::DestroyObjectWithWidget NSAlternate $oop $win

	bind $win <KeyPress-Escape> "NSAlternate::Close $oop"
	bind $win <Control-KeyPress-w> "NSAlternate::Close $oop"

	bind $win <KeyPress-n> "NSAlternate::NewAlternate $oop"
#	bind $win <KeyPress-plus> "NSAlternate::NewFrame $oop"
#	bind $win <KeyPress-minus> "NSAlternate::DeleteFrame $oop"

	#
	# Global access
	#

	Window alternate [Info $oop win]
	Global alternate,oop $oop

	return
}

# NSAlternate::~NSAlternate --
#
#	Object destructor called by NSObject::Delete().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::~NSAlternate {oop} {

	NSValueManager::RemoveClient listBG [Info $oop listBG,clientId]

	return
}

# NSAlternate::Info --
#
#	Query and modify info.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::Info {oop info args} {

	global NSAlternate

	# Verify the object
	NSObject::CheckObject NSAlternate $oop

	# Set info
	if {[llength $args]} {
		switch -- $info {
			default {
				set NSAlternate($oop,$info) [lindex $args 0]
			}
		}

	# Get info
	} else {
		switch -- $info {
			alternateCanvas {
				set canvistId [Info $oop alternate,canvistId]
				return [NSCanvist2::Info $canvistId canvas]
			}
			frameCanvas {
				set canvistId [Info $oop frame,canvistId]
				return [NSCanvist2::Info $canvistId canvas]
			}
			default {
				return $NSAlternate($oop,$info)
			}
		}
	}

	return
}

# NSAlternate::InitWindow --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::InitWindow {oop} {

	set win .alternate$oop
	toplevel $win
	wm title $win "Alternate Editor"

	wm transient $win [Window main]

	# Start out withdrawn (hidden)
	wm withdraw $win

	# Do stuff when window closes
	wm protocol $win WM_DELETE_WINDOW "NSAlternate::Close $oop"

	Info $oop win $win

	# Menus
	InitMenus $oop

	# Frame around alternate list, frame list, etc on the left
	set frame $win.frame
	frame $frame \
		-borderwidth 0

	set iconSize [expr {[icon size] + 8}]

	# The list of alternates
	set frameAlternate $frame.frameAlternate
	frame $frameAlternate \
		-borderwidth 1 -relief sunken
	set canvistId [NSObject::New NSCanvist2 $frameAlternate $iconSize $iconSize 240 160]
	NSCanvist2::Info $canvistId newItemCmd \
		"NewItemCmd_Assign"
	NSCanvist2::Info $canvistId highlightCmd \
		"NSIconBrowser::HighlightItemCmd $oop"
	NSCanvist2::Info $canvistId columns 6
	set canvas [NSCanvist2::Info $canvistId canvas]
	scrollbar $frameAlternate.yscroll \
		-command "$canvas yview" -orient vertical

#	bind $frameAlternate.yscroll <Map> "eval %W set \[$canvas yview]"
	NSUtils::SynchScrollBar $canvas $frameAlternate.yscroll

	$canvas configure -background [Value listBG]
	$canvas configure -yscrollcommand "$frameAlternate.yscroll set"

	# When a alternate is selected, show the frames of it
	NSCanvist2::Info $canvistId selectionCmd \
		"NSAlternate::SelectionChanged_Alternate $oop"

	Info $oop alternate,canvistId $canvistId

	pack $canvas \
		-side left -expand yes -fill both -anchor nw
	pack $frameAlternate.yscroll \
		-side left -fill y

	# Divider
	MakeDivider $frame.divider2 x

	# The list of frames in a alternate
	set frameFrame $frame.frameFrame
	frame $frameFrame \
		-borderwidth 1 -relief sunken
	set canvistId [NSObject::New NSCanvist2 $frameFrame $iconSize $iconSize 240 $iconSize]
	NSCanvist2::Info $canvistId newItemCmd \
		"NSIconBrowser::NewItemCmd $oop"
	NSCanvist2::Info $canvistId highlightCmd \
		"NSIconBrowser::HighlightItemCmd $oop"
	NSCanvist2::Info $canvistId columns 1
	set canvas [NSCanvist2::Info $canvistId canvas]
	scrollbar $frameFrame.xscroll \
		-command "$canvas xview" -orient horizontal

#	bind $frameFrame.xscroll <Map> "eval %W set \[$canvas xview]"
	NSUtils::SynchScrollBar $canvas $frameFrame.xscroll

	$canvas configure -background [Value listBG]
	$canvas configure -xscrollcommand "$frameFrame.xscroll set"

	NSCanvist2::Info $canvistId selectionCmd \
		"NSAlternate::SelectionChanged_Frame $oop"

	bindtags $canvas [concat [bindtags $canvas] NSAlternateBindTag$oop]
	bind NSAlternateBindTag$oop <Motion> \
		"NSAlternate::Motion $oop frame %x %y"
	bind NSAlternateBindTag$oop <Leave> \
		"NSAlternate::Leave $oop frame"

	Info $oop frame,canvistId $canvistId

	pack $canvas \
		-side top -expand yes -fill both -anchor nw
	pack $frameFrame.xscroll \
		-side top -fill x

	# Geometry of stuff on the left
	grid rowconfig $frame 0 -weight 1 -minsize 0
	grid rowconfig $frame 1 -weight 0 -minsize 0
	grid rowconfig $frame 2 -weight 0 -minsize 0
	grid columnconfig $frame 0 -weight 1 -minsize 0
	grid $frameAlternate \
		-row 0 -column 0 -rowspan 1 -columnspan 1 -sticky news
	grid $frame.divider2 \
		-row 1 -column 0 -rowspan 1 -columnspan 1 -sticky ew \
		-padx 2 -pady 2
	grid $frameFrame \
		-row 2 -column 0 -rowspan 1 -columnspan 1 -sticky ew

	# An NSIconBrowser lets the user examine all icon types
	set browserId [NSObject::New NSIconBrowser $win]
	set canvistId [NSIconBrowser::Info $browserId member,canvistId]
	NSCanvist2::Info $canvistId selectionCmd \
		"NSAlternate::SelectionChanged_Icon $oop"

	# Display progress while listing an icon type
	NSIconBrowser::Info $browserId clientCmd \
		"NSAlternate::BrowserCmd $oop"

	# Display the icon type when the mouse is over a group icon
	NSIconBrowser::Info $browserId group,motionCmd \
		"NSAlternate::BrowserMotionCmd $oop group"
	NSIconBrowser::Info $browserId group,leaveCmd \
		"NSAlternate::BrowserMotionCmd $oop group"

	# Display the icon index when the mouse is over an icon
	NSIconBrowser::Info $browserId member,motionCmd \
		"NSAlternate::BrowserMotionCmd $oop member"
	NSIconBrowser::Info $browserId member,leaveCmd \
		"NSAlternate::BrowserMotionCmd $oop member"

	Info $oop browserId $browserId

	#
	# Statusbar
	#

	MakeStatusBar $win.statusBar 20

	# Progress bar used to display progress of listing icons
	set label [$win.statusBar itemcget t1 -label]
	set progId [NSObject::New NSProgress2 $label 225 10]
	[NSProgress2::Info $progId frame] configure -borderwidth 0
	Info $oop progId $progId

	#
	# Geometry
	#

	grid rowconfig $win 0 -weight 0 -minsize 0
	grid rowconfig $win 1 -weight 1 -minsize 0
	grid rowconfig $win 2 -weight 0 -minsize 0
	grid columnconfig $win 0 -weight 1 -minsize 0
	grid columnconfig $win 1 -weight 0 -minsize 0
	grid columnconfig $win 2 -weight 1 -minsize 0
 
	if {[Platform windows]} {
		grid [MakeDivider $win.divider1 x] \
			-row 0 -column 0 -rowspan 1 -columnspan 3 -sticky ew \
			-padx 0 -pady 2
	}
	grid $frame \
		-row 1 -column 0 -rowspan 1 -columnspan 1 -sticky news
	grid [MakeDivider $win.divider2 y] \
		-row 1 -column 1 -rowspan 1 -columnspan 1 -sticky ns \
		-padx 4 -pady 2
	grid [NSIconBrowser::Info $browserId frame] \
		-row 1 -column 2 -rowspan 1 -columnspan 1 -sticky news
	grid $win.statusBar \
		-row 2 -column 0 -rowspan 1 -columnspan 3 -sticky ew

	return
}

# NSAlternate::InitMenus --
#
#	Create menus in the toplevel associated with this object.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::InitMenus {oop} {

	variable MenuString

	set win [Info $oop win]
	set mod "Ctrl"

	#
	# Menu bar
	#

	set mbar [NSObject::New NSMenu $win -tearoff 0 \
		-postcommand "NSAlternate::SetupMenus $oop" -identifier MENUBAR]
	Info $oop mbarId $mbar

	# Context-sensitive help
	NSMenu::Info $mbar menuSelectCmd "NSAlternate::MenuSelect $oop"

	# Call our command when an entry is invoked
	NSMenu::Info $mbar invokeCmd "NSAlternate::MenuInvoke $oop"

	#
	# Alternate Menu
	#

	NSObject::New NSMenu $mbar -tearoff 0 -identifier MENU_ALTERNATE
	NSMenu::MenuInsertEntry $mbar -end MENUBAR -type cascade \
		-menu MENU_ALTERNATE -label "Alternate" -underline 0 \
		-identifier M_ALTERNATE

	set entries {}
	lappend entries [list -type command -label "New Alternate" \
		-accelerator n -identifier E_NEW]
#	lappend entries "-type command -label \"Insert Frame\" -accelerator plus
#		-command \"NSAlternate::NewFrame $oop\" -identifier E_INSERT_FRAME"
#	lappend entries "-type command -label \"Delete Frame\" -accelerator minus
#		-command \"NSAlternate::DeleteFrame $oop\" -identifier E_DELETE_FRAME"
	lappend entries [list -type separator]
	lappend entries [list -type command -label [mc Close] \
		-underline 0 -accelerator $mod+W -identifier E_CLOSE]

	NSMenu::MenuInsertEntries $mbar -end MENU_ALTERNATE $entries

	#
	# Reason Menu
	#

	NSObject::New NSMenu $mbar -tearoff 0 -identifier MENU_REASON
	NSMenu::MenuInsertEntry $mbar -end MENUBAR -type cascade \
		-menu MENU_REASON -label "Reason" -underline 0 \
		-identifier M_REASON

	set entries {}
	lappend entries [list -type radiobutton -label "None" \
		-variable NSAlternate($oop,radio,reason) -value none \
		-identifier E_REASON_NONE]
	lappend entries [list -type radiobutton -label "Feature" \
		-variable NSAlternate($oop,radio,reason) -value feature \
		-identifier E_REASON_FEATURE]
	lappend entries [list -type radiobutton -label "Ident" \
		-variable NSAlternate($oop,radio,reason) -value ident \
		-identifier E_REASON_IDENT]
	lappend entries [list -type radiobutton -label "Number" \
		-variable NSAlternate($oop,radio,reason) -value number \
		-identifier E_REASON_NUMBER]

	NSMenu::MenuInsertEntries $mbar -end MENU_REASON $entries

	set MenuString(M_ALTERNATE) \
		"Contains commands for creating alternates."
	set MenuString(E_NEW) \
		"Creates a new alternate."
	set MenuString(E_CLOSE) \
		"Closes the window."

	set MenuString(M_REASON) \
		"Contains options for configuring alternates."
	set MenuString(E_REASON_NONE) \
		"The second frame is ignored."
	set MenuString(E_REASON_FEATURE) \
		"Feature is a door or pillar."
	set MenuString(E_REASON_IDENT) \
		"Object icon depends on identified status."
	set MenuString(E_REASON_NUMBER) \
		"Object icon depends on size of stack."

	return
}

# NSAlternate::SetupMenus --
#
#	Prepare to post the menus.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::SetupMenus {oop mbarID} {

	set alternateIndex [Info $oop alternate,current]
	set frameIndex [Info $oop frame,current]

	lappend identList E_NEW E_CLOSE
	if {$alternateIndex != -1} {
		lappend identList E_REASON_NONE E_REASON_NUMBER \
			E_REASON_IDENT E_REASON_FEATURE
#		if {($frameIndex != -1) && ([alternate count $alternateIndex] > 2)} {
#			lappend identList E_DELETE_FRAME
#		}
	}

	NSMenu::MenuEnable $mbarID $identList

	[Info $oop win].statusBar cover show

	return
}

# NSAlternate::MenuSelect --
#
#	Displays a help string associated with a menu entry.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::MenuSelect {oop menuId index ident} {

	variable MenuString

	switch -- $ident {
		{} {
			set desc {}
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

# NSAlternate::MenuInvoke --
#
#	Called when a menu entry is invoked.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::MenuInvoke {oop menuId ident} {

	switch -glob -- $ident {
		E_NEW {NewAlternate $oop}
		E_CLOSE {Close $oop}
		E_REASON_* {
			scan $ident "E_REASON_%s" reason
			set reason [string tolower $reason]
			ConfigAlternate $oop reason $reason
		}
	}

	return
}

# NSAlternate::DisplayCmd --
#
#	Called by NSWindowManager::Display().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::DisplayCmd {oop message first} {

	switch -- $message {
		preDisplay {
			Map $oop
		}
		postDisplay {
			if {$first} {
				set browserId [Info $oop browserId]
				set canvistId [NSIconBrowser::Info $browserId group,canvistId]
				NSIconBrowser::SetList_Group $browserId
				NSCanvist2::UpdateSelection $canvistId 0 ""
			}
		}
		postWithdraw {
			Unmap $oop
		}
	}

	return
}

# NSAlternate::Close --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::Close {oop} {

	NSWindowManager::Undisplay alternate

	return
}

# NSAlternate::Map --
#
#	Do something when the toplevel is mapped.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::Map {oop} {

	SetAlternateList $oop

	# Restore the selected alternate
	set current [Info $oop alternate,current2]
	set canvistId [Info $oop alternate,canvistId]
	if {$current != -1} {
		NSCanvist2::UpdateSelection $canvistId $current {}
		NSCanvist2::See $canvistId $current
	}

	# Restore the selected frame
	set current [Info $oop frame,current2]
	set canvistId [Info $oop frame,canvistId]
	if {$current != -1} {
		NSCanvist2::UpdateSelection $canvistId $current {}
		NSCanvist2::See $canvistId $current
	}

	return
}

# NSAlternate::Unmap --
#
#	Do something when unmapping the window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::Unmap {oop} {

	# Because we are clearing the lists here, and don't want to
	# upset the user, we save the selected alternate/frame so it can
	# be restored in Map() below.
	Info $oop alternate,current2 [Info $oop alternate,current]
	Info $oop frame,current2 [Info $oop frame,current]

	NSCanvist2::DeleteAll [Info $oop alternate,canvistId]

	return
}

# NSAlternate::SetAlternateList --
#
#	.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::SetAlternateList {oop} {

	set win [Info $oop win]
	set canvistId [Info $oop alternate,canvistId]

	# Clear the list
	NSCanvist2::DeleteAll $canvistId
	
	# Get the number of alternates */
	set max [alternate count]

	# Add each alternate to the list
	for {set i 0} {$i < $max} {incr i} {

		# Append match to the list
		NSCanvist2::Append $canvistId "alternate $i"
	}

	return
}

# NSAlternate::SelectionChanged_Alternate --
#
#	When a alternate is selected, display the frames (icons) in that alternate.
#	Otherwise clear the frames list.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::SelectionChanged_Alternate {oop canvistId select deselect} {

	set win [Info $oop win]

	# No alternate was selected
	if {![llength $select]} {

		# Clear the frame list
		set canvistId [Info $oop frame,canvistId]
		NSCanvist2::DeleteAll $canvistId

		# No alternate is selected now
		Info $oop alternate,current -1
		return
	}

	# Get the (first) cell
	set index [lindex $select 0]

	# Radiobutton menu entries
	Info $oop radio,reason [alternate configure $index -reason]

	# Display icons in that alternate
	SetFrameList $oop $index 0

	# Remember which alternate is selected
	Info $oop alternate,current $index

	return
}

# NSAlternate::SelectionChanged_Frame --
#
#	When a frame is selected, display it in the icon browser.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::SelectionChanged_Frame {oop canvistId select deselect} {

	set win [Info $oop win]

	if {![llength $select]} {

		$win.statusBar itemconfigure t2 -text ""

		# No frame is selected now
		Info $oop frame,current -1
		return
	}

	# Remember which frame is selected
	set frameIndex [lindex $select 0]
	Info $oop frame,current $frameIndex

	# Get the icon for that frame
	set icon [lindex [alternate get [Info $oop alternate,current] $frameIndex] 0]
	scan $icon "%s %d" iconType iconIndex

	# Select the frame in the icon browser, if the type is shown
	set browserId [Info $oop browserId]
	set iconTypeBrowser [NSIconBrowser::Info $browserId iconType]
	if {[string equal $iconTypeBrowser $iconType]} {
		NSIconBrowser::SeeIcon $browserId $iconType $iconIndex
	}

	# Display the type of icon
	$win.statusBar itemconfigure t2 -text $icon

	return
}

# NSAlternate::SelectionChanged_Icon --
#
#	When an icon is selected, change the icon assigned to the selected
#	frame.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::SelectionChanged_Icon {oop canvistId select deselect} {

	# Do nothing if no new cell was selected
	if {![llength $select]} return

	set selectedAlternate [Info $oop alternate,current]
	if {$selectedAlternate < 0} return
	set selectedFrame [Info $oop frame,current]
	if {$selectedFrame < 0} return

	# Get the (first) cell
	set index [lindex $select 0]

	# Get the type of icon to assign
	set browserId [Info $oop browserId]
	set iconType [NSIconBrowser::Info $browserId iconType]

	# Assign the icon to the frame
	alternate assign $selectedAlternate $selectedFrame -type $iconType -index $index

	# Scroll the frame into view if it isn't already
	NSCanvist2::See [Info $oop frame,canvistId] $selectedFrame

	# Update the alternate list if needed
	if {!$selectedFrame} {
		set icon [lindex [alternate get $selectedAlternate 0] 0]
		UpdateList $oop alternate $selectedAlternate -assign "icon $icon"
	}

	# Update the frame list
	UpdateList $oop frame $selectedFrame -assign "icon $iconType $index"

	return
}

# NSAlternate::UpdateList --
#
#	Configure the Widget canvas item on the row of a list.
#	When the user assigns an icon to a frame, we want to update
#	the frame list, and sometimes the alternate list too.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::UpdateList {oop list cell args} {

	set canvistId [Info $oop $list,canvistId]
	set canvas [Info $oop ${list}Canvas]
	set cellTag [lindex [NSCanvist2::Info $canvistId cellTags] $cell]
	foreach itemId [$canvas find withtag $cellTag] {
		if {[string equal [$canvas type $itemId] widget]} break
	}
	eval $canvas itemconfigure $itemId $args

	return
}

# NSAlternate::SetFrameList --
#
#	.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::SetFrameList {oop alternateIndex preserve} {

	set win [Info $oop win]
	set canvistId [Info $oop frame,canvistId]
	set canvas [Info $oop frameCanvas]

	# Sometimes maintain the selection and scroll position
	if {$preserve} {
		set columnWidth [$canvas cget -xscrollincrement]
		set colLeft [expr {int([$canvas canvasx 0 $columnWidth] / $columnWidth)}]
		set rowHeight [$canvas cget -yscrollincrement]
		set rowTop [expr {int([$canvas canvasy 0 $rowHeight] / $rowHeight)}]
		set selection [NSCanvist2::Selection $canvistId]
	}

	# Clear the list
	NSCanvist2::DeleteAll $canvistId

	NSCanvist2::Info $canvistId columns [alternate count $alternateIndex]

	# Add each frame to the list
	foreach frame [alternate get $alternateIndex] {

		# Append icon to the list
		NSCanvist2::Append $canvistId [lindex $frame 0] [lindex $frame 1]
	}

	if {$preserve} {
		$canvas xview scroll $colLeft units
		$canvas yview scroll $rowTop units
		NSCanvist2::UpdateSelection $canvistId $selection {}
	}

	return
}

# NSAlternate::StatusBar --
#
#	Display text in the status bar, perhaps clearing it later.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::StatusBar {oop text zap} {

	set win [Info $oop win]
	set label [$win.statusBar itemcget t1 -label]
	$label configure -text $text
	if {$zap} {
		NSUtils::ZapLabel $label
	}

	return
}

# NSAlternate::NewAlternate --
#
#	Creates a new alternate with 2 frames, and selects the alternate
#	and the first frame in the alternate.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::NewAlternate {oop} {

	set canvistId [Info $oop alternate,canvistId]

	# Every alternate must have at least 2 frames, or else!
	set alternateIndex [alternate create none]
	alternate insert $alternateIndex 0 -type default -index 0
	alternate insert $alternateIndex 1 -type default -index 0

	# Display the new alternate and select it
	SetAlternateList $oop
	NSCanvist2::UpdateSelection $canvistId $alternateIndex {}
	NSCanvist2::See $canvistId $alternateIndex

	# Select the first new frame
	set canvistId [Info $oop frame,canvistId]
	NSCanvist2::UpdateSelection $canvistId 0 {}
	NSCanvist2::See $canvistId 0

	return
}

# NSAlternate::NewFrame --
#
#	Insert a new (blank) frame into the selected alternate.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::NewFrame {oop} {

	set selectedAlternate [Info $oop alternate,current]
	if {$selectedAlternate < 0} return

	# Currently, only 2 frames are used
	if {[alternate count $selectedAlternate] == 2} {
		tk_messageBox -icon warning -title "Insert Frame" \
			-message "An alternate only requires 2 frames!"
		return
	}

	set canvistId [Info $oop frame,canvistId]

	set frameIndex [Info $oop frame,current]
	if {$frameIndex == -1} {
		set frameIndex [alternate count $selectedAlternate]
	}
	alternate insert $selectedAlternate $frameIndex -type default -index 0

	# Display icons in that alternate
	SetFrameList $oop $selectedAlternate 0

	NSCanvist2::UpdateSelection $canvistId $frameIndex {}
	NSCanvist2::See $canvistId $frameIndex

	return
}

# NSAlternate::DeleteFrame --
#
#	Delete the selected frame.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::DeleteFrame {oop} {

	set alternateIndex [Info $oop alternate,current]
	if {$alternateIndex < 0} return
	set frameIndex [Info $oop frame,current]
	if {$frameIndex < 0} return

	# Program bombs if a alternate has less than 2 frames
	if {[alternate count $alternateIndex] == 2} {
		tk_messageBox -icon warning -title "Delete Frame" \
			-message "An alternate must have 2 frames or else!"
		return
	}

	set canvistId [Info $oop frame,canvistId]

	alternate delete $alternateIndex $frameIndex

	if {$frameIndex >= [alternate count $alternateIndex] - 1} {
		incr frameIndex -1
	}

	# Display icons in that alternate
	SetFrameList $oop $alternateIndex 0
	NSCanvist2::UpdateSelection $canvistId $frameIndex {}
	NSCanvist2::See $canvistId $frameIndex

	return
}

# NSAlternate::ConfigAlternate --
#
#	Sets the frame delay of the selected alternate.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::ConfigAlternate {oop option value} {

	set selectedAlternate [Info $oop alternate,current]
	if {$selectedAlternate < 0} return

	alternate configure $selectedAlternate -$option $value

	return
}

# NSAlternate::Motion --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::Motion {oop which x y} {

	set win [Info $oop win]
	set canvistId [Info $oop $which,canvistId]

	# Get the hit cell
	set index [NSCanvist2::PointToCell $canvistId $x $y]

	if {$index == [Info $oop $which,lastIndex]} {
		return
	}
	Info $oop $which,lastIndex $index

	if {$index == -1} {
		StatusBar $oop "" 0
		return
	}
	
	switch -- $which {
		frame {
		
			# Get the icon for that frame
			set icon [lindex [alternate get [Info $oop alternate,current] $index] 0]
		
			# Display the type of icon
			StatusBar $oop $icon 0
		}
	}
	
	return
}

# NSAlternate::Leave --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::Leave {oop which} {

	set win [Info $oop win]
	
	set index -1

	if {$index == [Info $oop $which,lastIndex]} {
		return
	}
	Info $oop $which,lastIndex $index

	StatusBar $oop "" 0

	return
}

# NSAlternate::BrowserCmd --
#
#	Called by NSIconBrowser when displaying an icon type. Display
#	the progress of listing the icons. Note that this can
#	actually slow down listing the icons.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::BrowserCmd {oop action args} {

	set win [Info $oop win]
	set progId [Info $oop progId]
	
	switch -- $action {
		open {
			StatusBar $oop {} 0
			NSProgress2::Zero $progId
			pack [NSProgress2::Info $progId frame] -pady 1 -fill x
			update idletasks
			Info $oop oldLabel2 [$win.statusBar itemcget t2 -text]
		}
		update {
			set cur [lindex $args 0]
			set max [lindex $args 1]
			set bump [expr {(($max / 20) > 40) ? ($max / 20) : 40}]
			if {$cur && ($cur % $bump) == 0} {
				NSProgress2::SetDoneRatio $progId [expr {$cur / double($max)}]
				$win.statusBar itemconfigure t2 -text "$cur/$max"
				update idletasks
			}
		}
		close {
			NSProgress2::SetDoneRatio $progId 1.0
			update idletasks
			pack forget [NSProgress2::Info $progId frame]
			$win.statusBar itemconfigure t2 -text [Info $oop oldLabel2]
		}
	}

	return
}

# NSAlternate::BrowserMotionCmd --
#
#	Called by NSIconBrowser.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::BrowserMotionCmd {oop which index} {

	set win [Info $oop win]
	set browserId [Info $oop browserId]

	# No icon was hit
	if {$index == -1} {
		StatusBar $oop "" 0
		return
	}

	# The mouse moved over the group canvas (ie, icon types)
	if {[string equal $which group]} {
		set iconType [lindex [NSIconBrowser::Info $browserId iconTypes] $index]
		StatusBar $oop  "Icon type \"$iconType\"." 0
		return
	}

	set iconType [NSIconBrowser::Info $browserId iconType]
	set iconSpec "$iconType $index"

	StatusBar $oop "$iconSpec" 0

	return
}

# NSAlternate::ValueChanged_listBG --
#
#	Called when the listBG value changes.
#	Updates the Assign Window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAlternate::ValueChanged_listBG {oop} {

	set color [Value listBG]
	
	[Info $oop alternateCanvas] configure -background $color
	[Info $oop frameCanvas] configure -background $color

	return
}

