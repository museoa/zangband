# File: book-window.tcl

# Purpose: the (new) Book Window and related commands

#
# Copyright (c) 1997-2001 Tim Baker
#
# This software may be copied and distributed for educational, research, and
# not for profit purposes provided that this copyright and statement are
# included in all such copies.
#

namespace eval NSBookWindow {

	variable Priv

# namespace eval NSBookWindow
}

# NSBookWindow::InitModule --
#
#	One-time-only-ever initialization.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBookWindow::InitModule {} {

	InitImageIfNeeded Image_ButtonOptions button-options.gif
	InitImageIfNeeded Image_ButtonHelp button-help.gif

	NSModule::LoadIfNeeded NSToolbar

	NSObject::New NSBookWindow
	
	return
}

# NSBookWindow::CloseModule --
#
#	One-time-only-ever cleanup.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBookWindow::CloseModule {} {
	
	catch {
		destroy [Window book]
	}

	return
}

# NSBookWindow::NSBookWindow --
#
#	Object constructor called by NSObject::New().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBookWindow::NSBookWindow {oop} {

	Info $oop book,current -1
	Info $oop book,match {}
	Info $oop spell,current -1
	Info $oop spell,match {}

	InitWindow $oop

	set win [Info $oop win]

	NSWindowManager::RegisterWindow book $win \
		"GetDefaultGeometry $win reqwidth reqheight" "" \
		"NSBookWindow::DisplayCmd $oop"

	# Destroy the object along with the toplevel (later)
	NSUtils::DestroyObjectWithWidget NSBookWindow $oop $win

	#
	# Global list of application windows
	#

	Global book,oop $oop
	Window book $win

	return
}

# NSBookWindow::~NSBookWindow --
#
#	Object destructor called by NSObject::Delete().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBookWindow::~NSBookWindow {oop} {

	NSValueManager::RemoveClient font,magic [Info $oop clientId,font]

	return
}

# NSBookWindow::Info --
#
#	Query and modify info.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBookWindow::Info {oop info args} {

	global NSBookWindow

	# Verify the object
	NSObject::CheckObject NSBookWindow $oop

	# Set info
	if {[llength $args]} {
		switch -- $info {
			default {
				set NSBookWindow($oop,$info) [lindex $args 0]
			}
		}

	# Get info
	} else {
		switch -- $info {
			default {
				return $NSBookWindow($oop,$info)
			}
		}
	}

	return
}

# NSBookWindow::InitWindow --
#
#	Create the window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBookWindow::InitWindow {oop} {

	set win .book$oop
	toplevel $win
	wm title $win Book

#	wm resizable $win no no

	wm transient $win [Window main]

	# Start out withdrawn (hidden)
	wm withdraw $win

	# Do stuff when window closes
	wm protocol $win WM_DELETE_WINDOW "NSBookWindow::Close $oop"

	Info $oop win $win

	#
	# Toolbar
	#

	set toolId [NSObject::New NSToolbar 20 $win]
	NSToolbar::AddTool $toolId -image Image_ButtonOptions -showlabel no \
		-command "DoCommandIfAllowed ="
	NSToolbar::AddTool $toolId -image Image_ButtonHelp -showlabel no \
		-command "DoCommandIfAllowed ?"

	set color [format #%02x%02x%02x 0 0 153]

	#
	# List of books on the left
	#

	set iconSize [expr {[icon size] + 8}]
	set frameBook $win.frameBook
	frame $frameBook \
		-borderwidth 0
	set canvistId [NSObject::New NSCanvist $frameBook $iconSize $iconSize 240 \
		"" ""]
	NSCanvist::Info $canvistId newRowCmd \
		"NSBookWindow::NewItemCmd_Book $oop"
	NSCanvist::Info $canvistId highlightCmd \
		"NSBookWindow::HighlightItemCmd_Book $oop"
	NSCanvist::Info $canvistId columns 1
	set canvas [NSCanvist::Info $canvistId canvas]
	$canvas configure -background $color
	$canvas configure -height [expr {$iconSize * 7}]
	$canvas configure -yscrollcommand "$frameBook.yscroll set"
	scrollbar $frameBook.yscroll \
		-command "$canvas yview" -orient vert

	pack $frameBook.yscroll -side right -fill y
	pack $canvas -side left -expand yes -fill both

	Info $oop book,canvistId $canvistId
	Info $oop book,canvas $canvas

	#
	# List of spells on the right
	#

	set frameSpell $win.frameSpell
	frame $frameSpell \
		-borderwidth 0
	set font [Value font,magic]
	set rowHgt [font metrics $font -linespace]
	incr rowHgt 8
	set canvistId [NSObject::New NSCanvist $frameSpell $rowHgt 400 200 \
		"NSBookWindow::NewItemCmd_Spell $oop" "NSBookWindow::HighlightItemCmd_Spell $oop"]
	set canvas [NSCanvist::Info $canvistId canvas]
	$canvas configure -background $color

	# Double-click to select spell
	NSCanvist::Info $canvistId invokeCmd \
		"NSBookWindow::Invoke_Spell $oop"

	# Column titles
	set canvas2 $frameSpell.header
	canvas $canvas2 \
		-borderwidth 0 -highlightthickness 0
	MakeStatus $oop $canvas2 0 0 400 foo
	set font [Value font,magic]
	set fontHeight [font metrics $font -linespace]
	set height [expr {3 + $fontHeight + 3}]
	$canvas2 configure -height $height

	foreach title {Name Level Mana Fail Comment} anchor {nw ne ne ne nw} {
		$canvas2 create text 0 3 -text [mc $title] -fill gray -anchor $anchor \
			-font $font -tags header,$title
	}

	Info $oop header,canvas $canvas2

	# Update ourself when the font,magic value changes
	Info $oop clientId,font \
		[NSValueManager::AddClient font,magic \
		"NSBookWindow::ValueChanged_font_magic $oop"]

	# When the window resizes, reposition the canvas items
	bind $canvas <Configure> \
		"NSBookWindow::Configure $oop"

	pack $canvas2 -side top -expand no -fill x
	pack $canvas -side left -expand yes -fill both

	Info $oop spell,canvistId $canvistId
	Info $oop spell,canvas $canvas

	#
	# Geometry
	#

	grid rowconfig $win 0 -weight 0 -minsize 0
	grid rowconfig $win 1 -weight 1 -minsize 0
	grid columnconfig $win 0 -weight 0 -minsize 0
	grid columnconfig $win 1 -weight 1 -minsize 0
 
	pack forget [NSToolbar::Info $toolId frame]
	grid [NSToolbar::Info $toolId frame] \
		-row 0 -column 0 -rowspan 1 -columnspan 2 -sticky ew
	grid $frameBook \
		-row 1 -column 0 -rowspan 1 -columnspan 1 -sticky ns
	grid $frameSpell \
		-row 1 -column 1 -rowspan 1 -columnspan 1 -sticky news

	#
	# Context Menu
	#
	
	set m $win.context
	menu $m -tearoff 0
	bind $canvas <Button-3> "NSBookWindow::ContextMenu $oop $m %X %Y"

	#
	# Feed Term when keys pressed
	#

	Term_KeyPress_Bind [Info $oop book,canvas]
	Term_KeyPress_Bind [Info $oop spell,canvas]

	# XXX Hack -- Don't feed Tab to the Term
	bind [Info $oop book,canvas] <KeyPress-Tab> {
		focus [tk_focusNext %W]
		break
	}

	# XXX Hack -- Don't feed Tab to the Term
	bind [Info $oop spell,canvas] <KeyPress-Tab> {
		focus [tk_focusNext %W]
		break
	}

	# Left/Right arrow selects list
	bind [Info $oop book,canvas] <KeyPress-Left> \
		"break"
	bind [Info $oop book,canvas] <KeyPress-Right> \
		"NSBookWindow::SwitchList $oop spell ; break"
	bind [Info $oop spell,canvas] <KeyPress-Left> \
		"NSBookWindow::SwitchList $oop book ; break"
	bind [Info $oop spell,canvas] <KeyPress-Right> \
		"break"

	# Never focus on the window itself
	bind $win <FocusIn> "
		if {\[string equal %W $win]} {
			focus [Info $oop book,canvas]
		}
	"

	# "Return" does cast/study
	bind [Info $oop spell,canvas] <KeyPress-Return> \
		"NSBookWindow::InvokeSelected $oop ; break"

	return
}


# NSBookWindow::CalcColumnWidths --
#
#	Calculate the minimum column width of each spell in the given book.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBookWindow::CalcColumnWidths {oop bookNum} {

	variable Priv

	set font [Value font,magic]

	# Get a list of spell indexes
	set spells [angband spell find $bookNum -tester yes]

	# Check each spell
	foreach spell $spells {

		# Get information about this spell
		angband spell info $bookNum $spell attrib

		# Maximum width of char
		set width [font measure $font "$attrib(char)) "]
		if {$width > $Priv(width,char)} {
			set Priv(width,char) $width
		}
	
		# Maximum width of description
		set width [font measure $font $attrib(name)]
		if {$width > $Priv(width,desc)} {
			set Priv(width,desc) $width
		}
	
		# Maximum width of level
		set width [font measure $font "AB$attrib(level)"]
		if {$width > $Priv(width,level)} {
			set Priv(width,level) $width
		}
	
		# Maximum width of mana
		set width [font measure $font "AB$attrib(mana)"]
		if {$width > $Priv(width,mana)} {
			set Priv(width,mana) $width
		}
	
		# Maximum width of fail
		set width [font measure $font "AB${attrib(chance)}%"]
		if {$width > $Priv(width,fail)} {
			set Priv(width,fail) $width
		}
	}

	return
}


# NSBookWindow::NewItemCmd_Book --
#
#	Called by NSCanvist::InsertItem() to create a list row.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBookWindow::NewItemCmd_Book {oop canvistId y assign} {

	set c [NSCanvist::Info $canvistId canvas]
	set rowHeight [NSCanvist::Info $canvistId rowHgt]

	set columnWidth [expr {[icon size] + 8}]
	set x 0

	# Selection rectangle around everything
	lappend itemIdList [$c create rectangle [expr {$x + 2}] [expr {$y + 2}] \
		[expr {$x + $columnWidth - 2}] [expr {$y + $rowHeight - 2}] \
		-fill {} -outline {} -tags enabled -width 2.0]

	# Widget
	set iw [icon size]
	set ih [icon size]
	set xdiff [expr {int([expr {($columnWidth - $iw) / 2}])}]
	set ydiff [expr {int([expr {($rowHeight - $ih) / 2}])}]
	lappend itemIdList [$c create widget [expr {$x + $xdiff}] \
		[expr {$y + $ydiff}] -assign $assign -tags enabled]

	return $itemIdList
}

# NSBookWindow::HighlightItemCmd_Book --
#
#	Called by NSCanvist::Select() to highlight a row.
#
# Arguments:
#	oop						OOP ID. See above.
#	canvistId				OOP ID of NSCanvist object.
#	state					1 or 0 highlight state.
#	args					List of canvas item ids
#
# Results:
#	What happened.

proc NSBookWindow::HighlightItemCmd_Book {oop canvistId state args} {

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
		$canvas itemconfigure $idRect -fill {} -outline {}
	}

	return
}

# NSBookWindow::NewItemCmd_Spell --
#
#	Called by NSCanvist::InsertItem() to create a list row.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBookWindow::NewItemCmd_Spell {oop canvistId y char desc level mana fail info} {

	variable Priv

	set c [NSCanvist::Info $canvistId canvas]
	set lineHeight [NSCanvist::Info $canvistId rowHgt]
	set font [Value font,magic]

	set fh [font metrics $font -linespace]
	set diff [expr {int([expr {($lineHeight - $fh) / 2}])}]

	# Char
	lappend itemIdList [$c create text 4 [expr {$y + $diff}] \
		-text $char) -anchor nw -font $font -fill White]

	# Colorize the spell name (from OAngband)
	switch -- $info {
		unknown {
			set fill gray70
			
			# The character can learn this spell
			if {[angband player new_spells] && 
				($level <= [angband player level])} {
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

	# Description
	lappend itemIdList [$c create text 0 [expr {$y + $diff}] \
		-text $desc -anchor nw -font $font -fill $fill -tags description]

	# Level
	lappend itemIdList [$c create text 0 [expr {$y + $diff}] \
		-text $level -anchor ne -justify right -font $font -fill White \
		-tags level]

	# Mana
	lappend itemIdList [$c create text 0 [expr {$y + $diff}] \
		-text $mana -anchor ne -justify right -font $font -fill White \
		-tags mana]

	# Fail
	lappend itemIdList [$c create text 0 [expr {$y + $diff}] \
		-text $fail% -anchor ne -justify right -font $font -fill White \
		-tags fail]

	# Comment
	lappend itemIdList [$c create text 0 [expr {$y + $diff}] \
		-text $info -anchor nw -justify left -font $font -fill White \
		-tags comment]

	# Selection rectangle inside row
	lappend itemIdList [$c create rectangle 2 [expr {$y + 2}] \
		2 [expr {$y + $lineHeight - 2}] -fill "" -outline "" \
		-tags selrect -width 2.0]

	return $itemIdList
}

# NSBookWindow::HighlightItemCmd_Spell --
#
#	Called by NSCanvist::Select() to highlight a row.
#
# Arguments:
#	oop						OOP ID. See above.
#	canvistId				OOP ID of NSCanvist object.
#	state					1 or 0 highlight state.
#	args					List of canvas item ids
#
# Results:
#	What happened.

proc NSBookWindow::HighlightItemCmd_Spell {oop canvistId state args} {

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

# NSBookWindow::PositionItems --
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

proc NSBookWindow::PositionItems {oop} {

	variable Priv

	set win [Info $oop win]
	set canvistId [Info $oop spell,canvistId]
	set canvas [NSCanvist::Info $canvistId canvas]

	set header [Info $oop header,canvas]

	# Nothing to do
	if {![NSCanvist::Info $canvistId count]} return

	# Get the font
	set font [Value font,magic]

	# Get the width of the canvas
	set canvasWidth [winfo width $canvas]

	set offset 4

	incr offset $Priv(width,char)

	# Position each description
	$header coords header,Name $offset 3
	set coords [$canvas coords description]
	$canvas move description [expr {$offset - [lindex $coords 0]}] 0
	incr offset $Priv(width,desc)

	# Position each level
	incr offset $Priv(width,level)
	$header coords header,Level $offset 3
	set coords [$canvas coords level]
	$canvas move level [expr {$offset - [lindex $coords 0]}] 0

	# Position each mana
	incr offset $Priv(width,mana)
	$header coords header,Mana $offset 3
	set coords [$canvas coords mana]
	$canvas move mana [expr {$offset - [lindex $coords 0]}] 0

	# Position each fail
	incr offset $Priv(width,fail)
	$header coords header,Fail $offset 3
	set coords [$canvas coords fail]
	$canvas move fail [expr {$offset - [lindex $coords 0]}] 0

	# Position each comment
	incr offset [font measure $font "AB"]
	$header coords header,Comment $offset 3
	set coords [$canvas coords comment]
	$canvas move comment [expr {$offset - [lindex $coords 0]}] 0

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



# NSBookWindow::InvokeRow --
#
#	Do something when a spell is double-clicked or <Enter>'d.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBookWindow::InvokeRow {oop row} {

	# No row was hit
	if {$row == -1} return

	# Get the book number
	set bookNum [Info $oop book,current]

	# Get the spell index
	set spellIndex [lindex [Info $oop spell,match] $row]

	set command [GetSpellCommand $oop $bookNum $spellIndex label]
	if {[string length command]} {
		eval $command
	}

	return
}

# NSBookWindow::InvokeSelected --
#
#	Invoke the selected spell.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBookWindow::InvokeSelected {oop} {

	InvokeRow $oop [Info $oop spell,current]

	return
}

# NSBookWindow::Invoke_Spell --
#
#	Do something when a spell is double-clicked.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBookWindow::Invoke_Spell {oop canvistId x y} {

	# Find the hit row
	set row [NSCanvist::PointToRow $canvistId $x $y]

	# Do something
	InvokeRow $oop $row

	return
}

# NSBookWindow::ContextMenu --
#
#	When a spell is right-clicked, pop up a context menu of actions.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBookWindow::ContextMenu {oop menu x y} {

	# Get the canvas
	set canvistId [Info $oop spell,canvistId]
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

	# Get the book number
	set bookNum [Info $oop book,current]

	# Get the spell index
	set spellIndex [lindex [Info $oop spell,match] $row]

	set command [GetSpellCommand $oop $bookNum $spellIndex label]
	if {[string length $command]} {
		$menu add command -label $label -command $command \
			-font [BoldFont $font]
		$menu add separator
	}

	$menu add command -label [mc Close] -command {angband keypress \033}
	$menu add separator
	$menu add command -label [mc Cancel]

	# Let the user choose a command from the menu
	tk_popup $menu $x $y

	return
}

# NSBookWindow::Configure --
#
#	Respond to the <Configure> event.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBookWindow::Configure {oop} {

	# Arrange list items
	PositionItems $oop

	# Resize the list header
	set canvas [Info $oop header,canvas]
	ConfigureStatus $oop $canvas 0 0 [winfo width $canvas] foo

	return
}

# NSBookWindow::MakeStatus --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBookWindow::MakeStatus {oop canvas x y width where} {

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

proc NSBookWindow::ConfigureStatus {oop canvas x y width where} {

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

# NSBookWindow::ValueChanged_font_magic --
#
#	Called when the font,magic value changes.
#	Updates the Book Window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBookWindow::ValueChanged_font_magic {oop} {

	set win [Info $oop win]
	set canvistId [Info $oop spell,canvistId]
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
	foreach title [list Name Level Mana Fail Comment] {
		$canvas itemconfigure header,$title -font $font
	}

	# Resize the list header
	$canvas configure -height [expr {3 + $fontHeight + 3}]
	ConfigureStatus $oop $canvas 0 0 [winfo width $canvas] foo
	
	return
}

# NSBookWindow::GetSpellCommand --
#
#	Return a command that will allow the user to cast or study the
#	given spell in the given book.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBookWindow::GetSpellCommand {oop bookNum spellIndex _label} {

	upvar $_label label

	# Get information about the spell
	angband spell info $bookNum $spellIndex attrib

	# Get the spell char
	set charSpell $attrib(char)

	# Get the command char and spell type for the character
	GetMagicInfo charCmd spellType

	# We are waiting for a spell
	if {[string equal [angband inkey_flags] INKEY_SPELL]} {

		set label [format [mc "Select This %s"] [string totitle [mc $spellType]]]

		# Append a command to select the item
		return "angband keypress $charSpell"

	# We are waiting for a command
	} else {

		# The spell is known
		if {$attrib(known)} {
			set hook cast
		} else {
			set hook study
		}

		# Find a book in the inventory
		set match [angband inventory find -limit 1 -k_idx $bookNum -hook $hook]

		set charToggle ""
		set doToggle 0

		# The book is in the inventory
		if {[llength $match]} {

			# Get information about this book
			angband inventory info [lindex $match 0] iattrib
	
			# Get the inventory char
			set charBook $iattrib(char)
	
		# The book isn't in the inventory, so it must be on the floor.
		} else {

			set easy_floor [Setting easy_floor]

			# We need to figure out if the '-' char needs to be used.
			if {[llength [angband inventory find -hook $hook]] ||
				!$easy_floor} {
				set charToggle -
				set doToggle 1
			}

			# Get a list of any books we can cast/study from
			set match [angband floor find -hook $hook]

			set query_flag [Setting other_query_flag]

			# With easy_floor
			if {$easy_floor} {

				# Multiple items, with easy_floor
				if {[llength $match] > 1} {

					# Find the char of the current book
					set index 0
					foreach o_idx $match {
						set k_idx [angband o_list set $o_idx k_idx]
						if {$k_idx == $bookNum} break
						incr index
					}
					set charBook [string index "abcdefghijklmnopqrstuvw" $index]

				# One item, with easy_floor
				} else {

					if {$doToggle && !$query_flag} {
						set charBook ""
					} else {
						set charBook a
					}
				}

			# Without easy_floor
			} else {
			
				# Multiple items, without easy_floor
				if {[llength $match] > 1} {
	
					# Multiple items, without easy_floor, with floor_query_flag
					if {$query_flag} {
	
						foreach o_idx $match {
							set k_idx [angband o_list set $o_idx k_idx]
							if {$k_idx == $bookNum} break
							append charToggle n
						}
						append charToggle y
						set charBook ""
	
					# Multiple items, without easy_floor, without floor_query_flag
					} else {
	
						set o_idx [lindex $match 0]
						set k_idx [angband o_list set $o_idx k_idx]
	
						# If the top item isn't the book, we can't select it!
						if {$k_idx != $bookNum} return ""
	
						set charBook ""
					}
	
				# One item, without easy_floor
				} else {
	
					# One item, without easy_floor, with floor_query_flag
					if {$query_flag} {
	
						append charToggle y
						set charBook ""
		
					# One item, without easy_floor, without floor_query_flag
					} else {
						set charBook ""
					}
				}
			}
		}

		# The spell is known
		if {$attrib(known)} {

			switch -- $spellType {
				prayer {
					set label [mc Recite]
				}
				spell {
					set label [mc Cast]
				}
			}

			# Allow the user to cast the spell
			return "DoUnderlyingCommand $charCmd$charToggle$charBook$charSpell"

		# The spell is not known
		} else {

			# The character can learn this spell
			if {[angband player new_spells] && ($attrib(level) <=
				[angband player level])} {

				# Build the command to study this spell or prayer.
				set command "DoUnderlyingCommand G"

				# Hack -- Bypass the "You can learn N new spells prompt"
				append command "\\n"

				# Which book
				append command $charToggle$charBook

				# Which spell (prayers are random)
				if {[string equal $spellType spell]} {
					append command $charSpell
				}

				set label [mc Study]
				return $command
			}
		}
	}

	set label ""
	return ""
}

# NSBookWindow::SwitchList --
#
#	Focus on the book or spell list, updating the selection if desired.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSBookWindow::SwitchList {oop which} {

	set canvas [Info $oop $which,canvas]
	set canvistId [Info $oop $which,canvistId]
	set current [Info $oop $which,current]

	if {![NSCanvist::Info $canvistId count]} return

	focus $canvas
	if {$current == -1} {
		NSCanvist::UpdateSelection $canvistId 0 {}
	}

	return
}

