# File: icon-browser.tcl

# Purpose: an icon-type browser

#
# Copyright (c) 1997-2001 Tim Baker
#
# This software may be copied and distributed for educational, research, and
# not for profit purposes provided that this copyright and statement are
# included in all such copies.
#

#
#	An NSIconBrowser allows the user to browse through all the
#	different types of icons. It is based on a NSCanvist2, so you
#	would use the "selectionCmd" option to update your stuff when
#	an icon is selected (or unselected). An NSIconBrowser also
#	knows how to display a specific icon (ie, scroll it into view).
#

namespace eval NSIconBrowser {

	variable IconTypes

# namespace eval NSIconBrowser
}

# NSIconBrowser::InitModule --
#
#	One-time-only-ever initialization.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSIconBrowser::InitModule {} {

	variable IconTypes
	variable Priv

	set IconTypes {}

	# Also track max width of an icon type
	set maxWidth 0
	set maxHeight 0

	foreach iconType [icon gettypes] {
		switch -- $iconType {
			blank -
			default -
			none {
			}
			default {
				# Hack -- Don't show ascii type icons
				if {![icon ascii isascii $iconType]} {
					lappend IconTypes $iconType

					set width [icon width $iconType]
					if {$width > $maxWidth} {
						set maxWidth $width
					}

					set height [icon height $iconType]
					if {$height > $maxHeight} {
						set maxHeight $height
					}
				}
			}
		}
	}

	set Priv(maxIconWidth) $maxWidth
	set Priv(maxIconHeight) $maxHeight

	return
}

# NSIconBrowser::NSIconBrowser --
#
#	Object constructor called by NSObject::New().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSIconBrowser::NSIconBrowser {oop parent} {

	global NSIconBrowser
	variable IconTypes
	variable Priv

	Info $oop iconType ""
	Info $oop iconTypes $IconTypes
	Info $oop maxIconWidth $Priv(maxIconWidth)
	Info $oop maxIconHeight $Priv(maxIconHeight)

	# Call this command while displaying an icon type
	Info $oop clientCmd ""

	Info $oop group,leaveCmd {}
	Info $oop group,motionCmd {}

	Info $oop member,leaveCmd {}
	Info $oop member,motionCmd {}

	InitDisplay $oop $parent

	set groupCanvas [Info $oop groupCanvas]
	bindtags $groupCanvas \
		[concat [bindtags $groupCanvas] IconBrowserGroupBindTag$oop]
	bind IconBrowserGroupBindTag$oop <Motion> \
		"NSIconBrowser::Motion $oop group %x %y"
	bind IconBrowserGroupBindTag$oop <Leave> \
		"NSIconBrowser::Leave $oop group"
	Info $oop group,lastIndex -1

	set memberCanvas [Info $oop memberCanvas]
	bindtags $memberCanvas \
		[concat [bindtags $memberCanvas] IconBrowserMemberBindTag$oop]
	bind IconBrowserMemberBindTag$oop <Motion> \
		"NSIconBrowser::Motion $oop member %x %y"
	bind IconBrowserMemberBindTag$oop <Leave> \
		"NSIconBrowser::Leave $oop member"
	Info $oop member,lastIndex -1

	return
}

# NSIconBrowser::~NSIconBrowser --
#
#	Object destructor called by NSObject::Delete().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSIconBrowser::~NSIconBrowser {oop} {

	NSValueManager::RemoveClient listBG [Info $oop group,clientId]
	NSValueManager::RemoveClient listBG [Info $oop member,clientId]

	return
}

# NSIconBrowser::Info --
#
#	Query and modify info.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSIconBrowser::Info {oop info args} {

	global NSIconBrowser

	# Verify the object
	NSObject::CheckObject NSIconBrowser $oop

	# Set info
	if {[llength $args]} {
		switch -- $info {
			default {
				set NSIconBrowser($oop,$info) [lindex $args 0]
			}
		}

	# Get info
	} else {
		switch -- $info {
			groupCanvas {
				set canvistId [Info $oop group,canvistId]
				return [NSCanvist2::Info $canvistId canvas]
			}
			memberCanvas {
				set canvistId [Info $oop member,canvistId]
				return [NSCanvist2::Info $canvistId canvas]
			}
			default {
				return $NSIconBrowser($oop,$info)
			}
		}
	}

	return
}

# NSIconBrowser::InitDisplay --
#
#	Create our stuff.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSIconBrowser::InitDisplay {oop parent} {

	global NSIconBrowser

	# This frame holds all our stuff
	set frame $parent.iconbrowser$oop
	frame $frame \
		-borderwidth 1 -relief sunken

	# So client can pack us
	Info $oop frame $frame

	#
	# Group List
	#

	set cellWidth [expr {[Info $oop maxIconWidth] + 8}]
	set cellHeight [expr {[Info $oop maxIconHeight] + 8}]

	set frameGroup $frame.frameGroup
	frame $frameGroup \
		-borderwidth 0
	set canvistId [NSObject::New NSCanvist2 $frameGroup $cellWidth $cellHeight $cellWidth 240]
	NSCanvist2::Info $canvistId newItemCmd \
		"NSIconBrowser::NewItemCmd $oop"
	NSCanvist2::Info $canvistId highlightCmd \
		"NSIconBrowser::HighlightItemCmd $oop"
	NSCanvist2::Info $canvistId columns 1
	set canvas [NSCanvist2::Info $canvistId canvas]
	$canvas configure -background [Value listBG]
	$canvas configure -yscrollcommand "$frameGroup.yscroll set"
	scrollbar $frameGroup.yscroll \
		-command "$canvas yview" -orient vertical

	# This call updates the list background color whenever the
	# global list background color changes
	Info $oop group,clientId \
		[NSValueManager::AddClient listBG "ListBackgroundChanged $canvas"]

	# When a group is selected, show the artifacts/monsters in it
	NSCanvist2::Info $canvistId selectionCmd \
		"NSIconBrowser::SelectionChanged_Group $oop"

	# Grid, please
	pack $canvas -side left -expand yes -fill both -anchor nw
	pack $frameGroup.yscroll -side left -fill y

	Info $oop group,canvistId $canvistId

	#
	# Icon List
	#

	set frameIcon $frame.frameIcon
	frame $frameIcon \
		-borderwidth 0
	set canvistId [NSObject::New NSCanvist2 $frameIcon $cellWidth $cellHeight 240 240]
	NSCanvist2::Info $canvistId newItemCmd \
		"NSIconBrowser::NewItemCmd $oop"
	NSCanvist2::Info $canvistId highlightCmd \
		"NSIconBrowser::HighlightItemCmd $oop"
	NSCanvist2::Info $canvistId columns 6
	set canvas [NSCanvist2::Info $canvistId canvas]
	$canvas configure -background [Value listBG]
	$canvas configure -yscrollcommand "$frameIcon.yscroll set"
	scrollbar $frameIcon.yscroll \
		-command "$canvas yview" -orient vertical

	Info $oop member,canvistId $canvistId

	Info $oop member,clientId \
		[NSValueManager::AddClient listBG "ListBackgroundChanged $canvas"]

	pack $canvas -side left -expand yes -fill both -anchor nw
	pack $frameIcon.yscroll -side left -fill y

	#
	# Geometry
	#

	grid rowconfig $frame 0 -weight 1 -minsize 0
	grid columnconfig $frame 0 -weight 0 -minsize 0
	grid columnconfig $frame 1 -weight 1 -minsize 0
 
	grid $frameGroup \
		-row 0 -column 0 -rowspan 1 -columnspan 1 -sticky ns
	grid $frameIcon \
		-row 0 -column 1 -rowspan 1 -columnspan 1 -sticky news

	# Destroy the object along with the toplevel (later)
	NSUtils::DestroyObjectWithWidget NSIconBrowser $oop $frame

	return
}


# NSIconBrowser::SelectionChanged_Group --
#
#	When an icon group is selected, display icons in that group.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSIconBrowser::SelectionChanged_Group {oop canvistId select deselect} {

	# Clear member list if no group is selected
	if {![llength $select]} {
		set canvistId [Info $oop member,canvistId]
		NSCanvist2::DeleteAll $canvistId
		return
	}

	# Get the (first) row
	set row [lindex $select 0]

	# Display icons in that group
	SetList_Member $oop [lindex [Info $oop iconTypes] $row]

	return
}

# NSIconBrowser::SetList_Group --
#
#	.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSIconBrowser::SetList_Group {oop} {

	set canvistId [Info $oop group,canvistId]

	# Clear the list
	NSCanvist2::DeleteAll $canvistId

	# Add each group
	foreach iconType [Info $oop iconTypes] {

		# Add this group to the list
		NSCanvist2::Append $canvistId $iconType 0
	}

	# Hack -- Clear the icon list
	NSCanvist2::DeleteAll [Info $oop member,canvistId]

	return
}

# NSIconBrowser::SetList_Member --
#
#	.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSIconBrowser::SetList_Member {oop iconType} {

	set canvistId [Info $oop member,canvistId]
	set canvas [NSCanvist2::Info $canvistId canvas]
	set win [winfo toplevel $canvas]

	# Get busy
	set cursor [$win cget -cursor]
	$win configure -cursor watch
	update

	# Call this *before* clearing the list
	set command [Info $oop clientCmd]
	if {[string length $command]} {
	uplevel #0 $command open
	}

	# Clear the list
	NSCanvist2::DeleteAll $canvistId

	# Set cell size based on icon size
	set columnWidth [expr {[icon width $iconType] + 8}]
	set rowHeight [expr {[icon height $iconType] + 8}]
	
	NSCanvist2::Info $canvistId columnWidth $columnWidth
	NSCanvist2::Info $canvistId rowHeight $rowHeight
	$canvas configure -xscrollincrement $columnWidth \
		-yscrollincrement $rowHeight

# Is there a <Configure> event or something?
set width [winfo width $canvas]
set columns [expr {$width / $columnWidth}]
if {$columns == 0} {set columns 1}
NSCanvist2::Info $canvistId columns $columns

	# Get the number of icons of the given type */
	set max [icon count $iconType]

if 0 {
	package require canvashack

	set columnWidth [NSCanvist2::Info $canvistId columnWidth]
	set rowHeight [NSCanvist2::Info $canvistId rowHeight]

	# Widget
	set iw [icon size]
	set ih [icon size]
	set xdiff [expr {int([expr {($columnWidth - $iw) / 2}])}]
	set ydiff [expr {int([expr {($rowHeight - $ih) / 2}])}]

	set x 0
	set y 0
	set firstItemId [canvashack create $canvas $max [list \
		[list rectangle [expr {$x + 2}] [expr {$y + 2}] [expr {$x + $columnWidth - 2}] [expr {$y + $rowHeight - 2}] -fill {} -outline {} -tags enabled -width 2.0] \
		[list widget [expr {$x + $xdiff}] [expr {$y + $ydiff}] -tags enabled] \
	]
	]

	Info $oop canvashack,itemId $firstItemId
	for {set i 0} {$i < $max} {incr i} {
		lappend itemList [list $iconType $i]
	}
	NSCanvist2::AppendMany $canvistId $itemList

} elseif 1 {

	if {$max < 250} {

		# Collect info about each list item
		for {set i 0} {$i < $max} {incr i} {
			lappend itemList [list $iconType $i]
		}
	
		# Add each item to the list
		NSCanvist2::AppendMany $canvistId $itemList

	} else {

		set bump [expr {$max / 20}]
		for {set i 0} {$i < $max} {incr i} {
			lappend itemList [list $iconType $i]
			if {$i && ($i % $bump) == 0} {
				NSCanvist2::AppendMany $canvistId $itemList
				if {[string length $command]} {
					uplevel #0 $command update $i $max
				}
				set itemList {}
			}
		}
		NSCanvist2::AppendMany $canvistId $itemList
	}

} else {

	# Add each icon to the list
	for {set i 0} {$i < $max} {incr i} {

		# Append icon to the list
		NSCanvist2::Append $canvistId $iconType $i

		if {[string length $command]} {
			uplevel #0 $command update $i $max
		}
	}
}

	# Remember which type is currently displayed
	Info $oop iconType $iconType

	if {[string length $command]} {
		uplevel #0 $command close
	}

	# Display icons and restore the cursor
	update
	$win configure -cursor $cursor

	return
}

# NSIconBrowser::SeeIcon --
#
#	.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSIconBrowser::SeeIcon {oop iconType iconIndex} {

	set row 0
	foreach iconType2 [Info $oop iconTypes] {
		if {[string equal $iconType $iconType2]} break
		incr row
	}

	# Select and display the group the icon is in
	set canvistId [Info $oop group,canvistId]
	NSCanvist2::UpdateSelection $canvistId $row \
		[NSCanvist2::Selection $canvistId]
	NSCanvist2::See $canvistId $row

	# Select and display the icon itself
	set canvistId [Info $oop member,canvistId]
	NSCanvist2::UpdateSelection $canvistId $iconIndex \
		[NSCanvist2::Selection $canvistId]
	NSCanvist2::See $canvistId $iconIndex

	return
}

# NSIconBrowser::NewItemCmd --
#
#	Called by NSCanvist2::InsertItem() to create a list row.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSIconBrowser::NewItemCmd {oop canvistId x y iconType iconIndex} {

	set c [NSCanvist2::Info $canvistId canvas]
	set columnWidth [NSCanvist2::Info $canvistId columnWidth]
	set rowHeight [NSCanvist2::Info $canvistId rowHeight]
if 0 {
	# Selection rectangle around everything
	lappend itemIdList [$c create rectangle [expr {$x + 2}] [expr {$y + 2}] \
		[expr {$x + $columnWidth - 2}] [expr {$y + $rowHeight - 2}] \
		-fill {} -outline {} -tags enabled -width 2.0]
}
	# Widget
	set iw [icon width $iconType]
	set ih [icon height $iconType]
	set xdiff [expr {int([expr {($columnWidth - $iw) / 2}])}]
	set ydiff [expr {int([expr {($rowHeight - $ih) / 2}])}]
	set assign "icon $iconType $iconIndex"
	lappend itemIdList [$c create widget [expr {$x + $xdiff}] \
		[expr {$y + $ydiff}] -assign $assign -tags enabled]

	return $itemIdList
}

# new version using canvashack
proc NSIconBrowser::NewItemCmd_Member {oop canvistId x y iconType iconIndex} {

	set itemId [Info $oop canvashack,itemId]

	set c [NSCanvist2::Info $canvistId canvas]

	# Selection rectangle around everything
	$c move $itemId $x $y
	lappend itemIdList $itemId
	incr itemId

	# Widget
	$c move $itemId $x $y
	set assign "icon $iconType $iconIndex"
	$c itemconfigure $itemId -assign $assign
	lappend itemIdList $itemId
	incr itemId

	Info $oop canvashack,itemId $itemId

	return $itemIdList
}

# NSIconBrowser::HighlightItemCmd --
#
#	Called by NSCanvist2::Select() to highlight a row.
#
# Arguments:
#	oop						OOP ID. See above.
#	canvistId				OOP ID of NSCanvist2 object.
#	state					1 or 0 highlight state.
#	args					List of canvas item ids
#
# Results:
#	What happened.

proc NSIconBrowser::HighlightItemCmd {oop canvistId state args} {

	set canvas [NSCanvist2::Info $canvistId canvas]
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
		$canvas itemconfigure $idRect -fill {} -outline {}
	}

	return
}

proc NSIconBrowser::HighlightItemCmd {oop canvistId state args} {

	set canvas [NSCanvist2::Info $canvistId canvas]
	set itemIdList $args

	set idWidget [lindex $itemIdList 0]

	if {[NSUtils::HasFocus $canvas]} {
		set fill [Value listHilite]
	} else {
		set fill [Value listInactive]
	}

	if {$state} {
		$canvas itemconfigure $idWidget -bordercolor $fill

	} else {
		$canvas itemconfigure $idWidget -bordercolor ""
	}

	return
}

# NSIconBrowser::Motion --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSIconBrowser::Motion {oop which x y} {

	set canvistId [Info $oop $which,canvistId]

	# Get the hit cell
	set index [NSCanvist2::PointToCell $canvistId $x $y]

	if {$index == [Info $oop $which,lastIndex]} {
		return
	}
	Info $oop $which,lastIndex $index

	set command [Info $oop $which,motionCmd]
	if {[string length $command]} {
		uplevel #0 $command $index
	}

	return
}

# NSIconBrowser::Leave --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSIconBrowser::Leave {oop which} {

	set index -1

	if {$index == [Info $oop $which,lastIndex]} {
		return
	}
	Info $oop $which,lastIndex $index

	set command [Info $oop $which,leaveCmd]
	if {[string length $command]} {
		uplevel #0 $command -1
	}

	return
}
