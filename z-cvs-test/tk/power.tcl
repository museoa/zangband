# File: power.tcl

# Purpose: The Power Window and related commands

#
# Copyright (c) 1997-2001 Tim Baker
#
# This software may be copied and distributed for educational, research, and
# not for profit purposes provided that this copyright and statement are
# included in all such copies.
#

namespace eval NSPower {

	variable Priv

# namespace eval NSPower
}

# NSPower::InitModule --
#
#	One-time-only-ever initialization.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSPower::InitModule {} {

	MsgCatInit power

	InitImageIfNeeded Image_ButtonOptions button-options.gif
	InitImageIfNeeded Image_ButtonHelp button-help.gif

	NSModule::LoadIfNeeded NSToolbar

	NSObject::New NSPower

	return
}

# NSPower::CloseModule --
#
#	One-time-only-ever cleanup.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSPower::CloseModule {} {
	
	catch {
		destroy [Window power]
	}

	return
}

# NSPower::NSPower --
#
#	Object constructor called by NSObject::New().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSPower::NSPower {oop} {

	InitWindow $oop

	set win [Info $oop win]
	
	NSWindowManager::RegisterWindow power $win \
		"GetDefaultGeometry $win reqwidth reqheight" "" \
		"NSPower::DisplayCmd $oop"

	#
	# Global list of application windows
	#

	Global power,oop $oop
	Window power $win

	return
}

# NSPower::~NSPower --
#
#	Object destructor called by NSObject::Delete().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSPower::~NSPower {oop} {

	NSValueManager::RemoveClient font,magic [Info $oop clientId,font]

	return
}

# NSPower::Info --
#
#	Query and modify info.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSPower::Info {oop info args} {

	global NSPower

	# Verify the object
	NSObject::CheckObject NSPower $oop

	# Set info
	if {[llength $args]} {
		switch -- $info {
			default {
				set NSPower($oop,$info) [lindex $args 0]
			}
		}

	# Get info
	} else {
		switch -- $info {
			default {
				return $NSPower($oop,$info)
			}
		}
	}

	return
}

# NSPower::InitWindow --
#
#	Create the window associated with this object.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSPower::InitWindow {oop} {

	set win .power$oop
	toplevel $win
	wm title $win [mc Powers]

	wm transient $win [Window main]

	# Start out withdrawn (hidden)
	wm withdraw $win

	# Do stuff when window closes
	wm protocol $win WM_DELETE_WINDOW "NSPower::Close $oop"

	Info $oop win $win

	#
	# Toolbar
	#

	set toolId [NSObject::New NSToolbar 20 $win]
	NSToolbar::AddTool $toolId -image Image_ButtonOptions \
		-command "DoKeymapCmd \033 = {}" -showlabel no
	NSToolbar::AddTool $toolId -image Image_ButtonHelp \
		-command "DoKeymapCmd \033 ? {}" -showlabel no

	#
	# List
	#

	frame $win.frame \
		-borderwidth 1 -relief sunken

	# Get the desired font
	set font [Value font,magic]

	# Column titles
	set canvas2 $win.frame.header
	canvas $canvas2 \
		-borderwidth 0 -highlightthickness 0
	MakeStatus $oop $canvas2 0 0 400 foo
	set fontHeight [font metrics $font -linespace]
	set height [expr {3 + $fontHeight + 3}]
	$canvas2 configure -height $height

	foreach title {Name Level Cost Fail Stat} anchor {nw ne ne ne nw} {
		$canvas2 create text 0 3 -text [mc $title] -fill gray -anchor $anchor \
			-font $font -tags header,$title
	}

	Info $oop header,canvas $canvas2

	set rowHgt [font metrics $font -linespace]
	incr rowHgt 8

	set color [format #%02x%02x%02x 0 0 153]

	set canvistId [NSObject::New NSCanvist $win.frame $rowHgt 400 280 \
		"NSPower::NewItemCmd $oop" "NSPower::HighlightItemCmd $oop"]
	set canvas [NSCanvist::Info $canvistId canvas]
	$canvas configure -background $color -yscrollcommand "$win.frame.scroll set"
	scrollbar $win.frame.scroll \
		-command "$canvas yview" -orient vert

	# Update ourself when the font,magic value changes
	Info $oop clientId,font \
		[NSValueManager::AddClient font,magic \
		"NSPower::ValueChanged_font_magic $oop"]

	# When the window resizes, reposition the canvas items
	bind $canvas <Configure> \
		"NSPower::Configure $oop"

	Info $oop canvistId $canvistId

	pack $win.frame.scroll -side right -fill y
	pack $win.frame.header -side top -expand no -fill x
	pack $canvas -side left -expand yes -fill both

	#
	# Double-click to select spell
	#

	NSCanvist::Info $canvistId invokeCmd \
		"NSPower::Invoke $oop"

	#
	# Geometry
	#

	grid rowconfig $win 0 -weight 0 -minsize 0
	grid rowconfig $win 1 -weight 1 -minsize 0
	grid rowconfig $win 2 -weight 0 -minsize 0
	grid columnconfig $win 0 -weight 1 -minsize 0
 
	pack forget [NSToolbar::Info $toolId frame]
	grid [NSToolbar::Info $toolId frame] \
		-row 0 -column 0 -rowspan 1 -columnspan 1 -sticky ew
	grid $win.frame \
		-row 1 -column 0 -rowspan 1 -columnspan 1 -sticky news

	#
	# Context Menu
	#
	
	set m $win.context
	menu $m -tearoff 0
	bind $canvas <Button-3> "NSPower::ContextMenu $oop $m %X %Y"

	#
	# Feed Term when keys pressed
	#

	bind $win <KeyPress> {
		angband keypress %A
	}

	#
	# Synch the scrollbars when window is shown.
	#

	NSUtils::SynchScrollBar $canvas $win.frame.scroll 1

	bind $win <FocusIn> "
		if {\[string equal %W $win]} {
			focus $canvas
		}
	"

	return
}

# NSPower::DisplayCmd --
#
#	Called by NSWindowManager::Display().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSPower::DisplayCmd {oop message first} {

	switch -- $message {
		preDisplay {
			SetList $oop
		}
		postDisplay {
		}
	}

	return
}

# NSPower::Close --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSPower::Close {oop} {

	angband keypress \033

	return
}

# NSPower::Invoke --
#
#	When a power is double-clicked, "angband keypress" the char.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSPower::Invoke {oop canvistId x y} {

	if {[string compare [angband inkey_flags] INKEY_POWER]} return

	set row [NSCanvist::PointToRow $canvistId $x $y]
	if {$row == -1} return

	set powerChars "abcdefghijklmnopqrstuvwxyz0123456789"
	set char [string index $powerChars $row]
	
	angband keypress $char

	return
}

# NSPower::ContextMenu --
#
#	When a power is right-clicked, pop up a context menu of actions.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSPower::ContextMenu {oop menu x y} {

	variable Priv

	# Get the canvas
	set canvistId [Info $oop canvistId]
	set canvas [NSCanvist::Info $canvistId canvas]

	set font [$menu cget -font]
	
	# Find the hit row
	set x1 [expr {$x - [winfo rootx $canvas]}]
	set y1 [expr {$y - [winfo rooty $canvas]}]
	set row [NSCanvist::PointToRow $canvistId $x1 $y1]

	# Clear the menu
	$menu delete 0 end

	# No row is hit
	if {$row == -1} {

		$menu add command -label [mc Close] -command {angband keypress \033}
		$menu add separator
		$menu add command -label [mc Cancel]

		# Pop up the menu
		tk_popup $menu $x $y

		# Done
		return
	}

	set powerChars "abcdefghijklmnopqrstuvwxyz0123456789"
	set char [string index $powerChars $row]

	$menu add command -label [mc "Select This Power"] \
		-command "angband keypress $char" -font [BoldFont $font]
	$menu add separator
	$menu add command -label [mc Close] -command {angband keypress \033}
	$menu add separator
	$menu add command -label [mc Cancel]

	# Pop up the menu
	tk_popup $menu $x $y

	return
}

# NSPower::SetList --
#
#	Fill the list with powers.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSPower::SetList {oop} {

	variable Priv

	set win [Info $oop win]
	set canvistId [Info $oop canvistId]
	set canvas [NSCanvist::Info $canvistId canvas]

	NSCanvist::DeleteAll $canvistId

	set font [Value font,magic]

	# We are going to calculate the maximum column width for each
	# column, and must consider the width of the column titles
	# in some cases
	set Priv(width,char) 0
	set Priv(width,desc) 0
	set Priv(width,level) [font measure $font "ABLevel"]
	set Priv(width,cost) [font measure $font "ABCost"]
	set Priv(width,fail) [font measure $font "ABFail"]
	set Priv(width,stat) 0

	set powerChars "abcdefghijklmnopqrstuvwxyz0123456789"

	set row 0
	foreach power [angband power get] {

		# Get information about the power
		angband power info $power attrib

		# Get the power char (a-z0-9)
		set attrib(char) [string index $powerChars $row]

		# Append the power to the list
		NSCanvist::Insert $canvistId end $attrib(char) $attrib(name) \
			$attrib(level) $attrib(cost) $attrib(chance) $attrib(stat)

		incr row
	}

	# Arrange all the items
	PositionItems $oop

	return
}

# NSPower::NewItemCmd --
#
#	Called by NSCanvist::InsertItem() to create a list row.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSPower::NewItemCmd {oop canvistId y char desc level cost fail stat} {

	variable Priv

	set c [NSCanvist::Info $canvistId canvas]
	set lineHeight [NSCanvist::Info $canvistId rowHgt]
	set font [Value font,magic]

	# Get the width of the canvas
	set canvasWidth [winfo width $c]

	set fh [font metrics $font -linespace]
	set diff [expr {int([expr {($lineHeight - $fh) / 2}])}]

	# Char
	lappend itemIdList [$c create text 4 [expr {$y + $diff}] \
		-text $char) -anchor nw -font $font -fill White]

	# Description
	set fill White
	if {$fail == 100} {
		set fill gray70
	}
	lappend itemIdList [$c create text 0 [expr {$y + $diff}] \
		-text $desc -anchor nw -font $font -fill $fill -tags description]

	# Level
	lappend itemIdList [$c create text 0 [expr {$y + $diff}] \
		-text $level -anchor ne -justify right -font $font -fill White \
		-tags level]

	# Cost
	lappend itemIdList [$c create text  0 [expr {$y + $diff}] \
		-text $cost -anchor ne -justify right -font $font -fill White \
		-tags cost]

	# Fail
	lappend itemIdList [$c create text 0 [expr {$y + $diff}] \
		-text $fail% -anchor ne -justify right -font $font -fill White \
		-tags fail]

	# Stat
	set stat [string totitle $stat]
	lappend itemIdList [$c create text 0 [expr {$y + $diff}] \
		-text [mc $stat] -anchor nw -justify left -font $font -fill White \
		-tags stat]

	# Selection rectangle inside row
	lappend itemIdList [$c create rectangle 2 [expr {$y + 2}] \
		2 [expr {$y + $lineHeight - 2}] -fill "" -outline "" \
		-tags selrect -width 2.0]

	# Maximum width of char
	set width [font measure $font "$char) "]
	if {$width > $Priv(width,char)} {
		set Priv(width,char) $width
	}

	# Maximum width of description
	set width [font measure $font $desc]
	if {$width > $Priv(width,desc)} {
		set Priv(width,desc) $width
	}

	# Maximum width of level
	set width [font measure $font "AB$level"]
	if {$width > $Priv(width,level)} {
		set Priv(width,level) $width
	}

	# Maximum width of cost
	set width [font measure $font "AB$cost"]
	if {$width > $Priv(width,cost)} {
		set Priv(width,cost) $width
	}

	# Maximum width of fail
	set width [font measure $font "AB${fail}%"]
	if {$width > $Priv(width,fail)} {
		set Priv(width,fail) $width
	}

	return $itemIdList
}

# NSPower::PositionItems --
#
#	Arranges all the canvas items in the list. This is called after all
#	the items are added, and when the <Configure> event indicates the
#	window has been resized. This is the routine that lets variable-width
#	fonts work.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSPower::PositionItems {oop} {

	variable Priv

	set win [Info $oop win]
	set canvistId [Info $oop canvistId]
	set canvas [NSCanvist::Info $canvistId canvas]

	# Nothing to do
	if {![NSCanvist::Info $canvistId count]} return

	# Get the font
	set font [Value font,magic]

	# Get the width of the canvas
	set canvasWidth [winfo width $canvas]

	# Get the header canvas
	set header [Info $oop header,canvas]

	set offset 4

	incr offset $Priv(width,char)

	# Configure the description header
	$header coords header,Name $offset 3
	
	# Position each description
	set coords [$canvas coords description]
	$canvas move description [expr {$offset - [lindex $coords 0]}] 0
	incr offset $Priv(width,desc)

	# Position each level
	incr offset $Priv(width,level)
	set coords [$canvas coords level]
	$canvas move level [expr {$offset - [lindex $coords 0]}] 0

	# Configure the level header
	$header coords header,Level $offset 3

	# Position each cost
	incr offset $Priv(width,cost)
	set coords [$canvas coords cost]
	$canvas move cost [expr {$offset - [lindex $coords 0]}] 0

	# Configure the cost header
	$header coords header,Cost $offset 3

	# Position each fail
	incr offset $Priv(width,fail)
	set coords [$canvas coords fail]
	$canvas move fail [expr {$offset - [lindex $coords 0]}] 0

	# Configure the fail header
	$header coords header,Fail $offset 3

	# Position each stat
	incr offset [font measure $font "AB"]
	set coords [$canvas coords stat]
	$canvas move stat [expr {$offset - [lindex $coords 0]}] 0

	# Configure the stat header
	$header coords header,Stat $offset 3

	# Position each selection rectangle
	set x1 [expr {($canvasWidth - 1) - 2}]
	foreach itemId [$canvas find withtag selrect] {
		scan [$canvas coords $itemId] "%s %s %s %s" c0 c1 c2 c3
		$canvas coords $itemId $c0 $c1 $x1 $c3
	}

	# Set the scrollregion to prevent horizontal scrolling
	scan [$canvas cget -scrollregion] "%s %s %s %s" x1 y1 x2 y2
	$canvas configure -scrollregion "$x1 $y1 $canvasWidth $y2"

	return
}

# NSPower::HighlightItemCmd --
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

proc NSPower::HighlightItemCmd {oop canvistId state args} {

	set canvas [NSCanvist::Info $canvistId canvas]
	set itemIdList $args

	set idRect [FindItemByTag $canvas $itemIdList selrect]

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

# NSPower::Configure --
#
#	Respond to the <Configure> event.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSPower::Configure {oop} {

	# Arrange list items
	PositionItems $oop

	# Resize the list header
	set canvas [Info $oop header,canvas]
	ConfigureStatus $oop $canvas 0 0 [winfo width $canvas] foo

	return
}

# NSPower::MakeStatus --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSPower::MakeStatus {oop canvas x y width where} {

	set font [Value font,magic]
	set fontHeight [font metrics $font -linespace]
	set height [expr {3 + $fontHeight + 3}]

	# 3-pixel border + background
	$canvas create rectangle [expr {$x + 1}] [expr {$y + 1}] \
		[expr {$x + $width - 2}] [expr {$y + $height - 2}] \
		-outline #282828 -fill #0000D4 -width 3.0 \
		-tags $where,bg

	# 1-pixel border
	$canvas create rectangle [expr {$x + 1}] [expr {$y + 1}] \
		[expr {$x + $width - 2}] [expr {$y + $height - 2}] \
		-outline #0070FF -fill {} \
		-tags $where,bd

	return
}

proc NSPower::ConfigureStatus {oop canvas x y width where} {

	set font [Value font,magic]
	set fontHeight [font metrics $font -linespace]
	set height [expr {3 + $fontHeight + 3}]

	# 3-pixel border + background
	$canvas coords $where,bg [expr {$x + 1}] [expr {$y + 1}] \
		[expr {$x + $width - 2}] [expr {$y + $height - 2}]

	# 1-pixel border
	$canvas coords $where,bd [expr {$x + 1}] [expr {$y + 1}] \
		[expr {$x + $width - 2}] [expr {$y + $height - 2}]

	return
}

# NSPower::ValueChanged_font_magic --
#
#	Called when the font,magic value changes.
#	Updates the Power Window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSPower::ValueChanged_font_magic {oop} {

	set win [Info $oop win]
	set canvistId [Info $oop canvistId]
	set canvas [NSCanvist::Info $canvistId canvas]
	
	# Get the desired font
	set font [Value font,magic]

	# Calculate font height
	set fontHeight [font metrics $font -linespace]

	# Calculate row height
	set rowHgt [expr {$fontHeight + 8}]

	# Set row height of list
	NSCanvist::Info $canvistId rowHgt $rowHgt
	$canvas configure -yscrollincrement $rowHgt

	# Set font of list header
	set canvas [Info $oop header,canvas]
	foreach title [list Name Level Cost Fail Stat] {
		$canvas itemconfigure header,$title -font $font
	}

	# Resize the list header
	$canvas configure -height [expr {3 + $fontHeight + 3}]
	ConfigureStatus $oop $canvas 0 0 [winfo width $canvas] foo

	return
}

