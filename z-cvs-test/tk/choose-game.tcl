# File: choose-game.tcl

# Purpose: the Choose Game Window and related commands

#
# Copyright (c) 1997-2001 Tim Baker
#
# This software may be copied and distributed for educational, research, and
# not for profit purposes provided that this copyright and statement are
# included in all such copies.
#

namespace eval NSChooseGame {

	variable Priv

# namespace eval NSChooseGame
}

# NSChooseGame::InitModule --
#
#	One-time-only-ever initialization.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseGame::InitModule {} {

	variable Priv

	MsgCatInit startup

	if {[llength [Value savefile]]} {
		set Priv(recent,path) [eval file join [Value savefile]]
	} else {
		set Priv(recent,path) ""
	}

	NSObject::New NSChooseGame

	return
}

# NSChooseGame::NSChooseGame --
#
#	Object constructor called by NSObject::New().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseGame::NSChooseGame {oop} {
	
	InitWindow $oop

	set win [Info $oop win]

	# Destroy the object along with the toplevel (later)
	NSUtils::DestroyObjectWithWidget NSChooseGame $oop $win

	Info $oop current -1

	# Create a progress window
	set progId [NSObject::New NSProgressWindow]
	Info $oop progId $progId

	# Sort filenames by Name, Version or Description
	Info $oop sortBy 0

	# Configure with the progress window
	set win [NSProgressWindow::Info $progId win]
	wm title $win [mc scan-title]
	wm transient $win .

	# Position and display the progress window
	WindowPosition $win 2 3
	focus [NSProgressWindow::Info $progId win]
	update

	# Let the user choose a saved game
	ModalLoop $oop

	return
}

# NSChooseGame::Info --
#
#	Query and modify info.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseGame::Info {oop info args} {

	global NSChooseGame

	# Verify the object
	NSObject::CheckObject NSChooseGame $oop

	# Set info
	if {[llength $args]} {
		switch -- $info {
			default {
				set NSChooseGame($oop,$info) [lindex $args 0]
			}
		}

	# Get info
	} else {
		switch -- $info {
			default {
				return $NSChooseGame($oop,$info)
			}
		}
	}

	return
}

# NSChooseGame::InitWindow --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseGame::InitWindow {oop} {

	set win .choosegame$oop
	toplevel $win
	wm title $win [mc choose-title]
	
	# Do stuff when window closes
	wm protocol $win WM_DELETE_WINDOW "bell"

	wm transient $win .
	wm resizable $win no no

	wm withdraw $win

	Info $oop win $win

	frame $win.framePrompt \
		-borderwidth 0
	frame $win.framePrompt.filler1 \
		-borderwidth 0 -height 5
	message $win.framePrompt.message \
		-anchor w -width 400 -text [mc choose-prompt] -borderwidth 0
	pack $win.framePrompt.filler1 \
		-side top -fill x
	pack $win.framePrompt.message \
		-side top -fill x

	# Header + List + Scrollbar
	set frame $win.frameList
	frame $frame \
		-borderwidth 2 -relief sunken

	set font [Global font,sys,normal]
#set font {Courier 9}
	Info $oop font $font
	
	# Column titles
	set frame $win.frameList.header
	frame $frame \
		-borderwidth 0 -relief raised -height 1
	set i 0
	foreach title {Name Version Description} {
		set label $frame.label$i
		label $label \
			-borderwidth 2 -relief raised -text [mc choose-$title] \
			-font $font -padx 4 -anchor w
		bind $label <ButtonPress-1> {
			%W configure -relief sunken
		}
		bind $label <ButtonRelease-1> "
			%W configure -relief raised
			NSChooseGame::SortBy $oop $i
		"
		incr i
	}
	$frame.label0 configure -font "$font underline"
	set borderWidth [$frame cget -borderwidth]
	set labelHeight [winfo reqheight $frame.label1]
	$frame configure -height [expr {$labelHeight + $borderWidth * 2}]

	# List
	set frame $win.frameList
	set canvistId [NSObject::New NSCanvist $frame 20 400 [expr {20 * 7}] \
		"NSChooseGame::NewItemCmd $oop" "NSChooseGame::HighlightItemCmd $oop"]
	set canvas [NSCanvist::Info $canvistId canvas]
	$canvas configure -yscrollcommand "$frame.yscroll set"
	scrollbar $frame.yscroll \
		-command "$canvas yview"

	NSCanvist::Info $canvistId selectionCmd \
		"NSChooseGame::SelectionChanged $oop"

	bind $canvas <Double-ButtonPress-1> \
		"tkButtonInvoke $win.frameButton.open"

	Info $oop canvistId $canvistId

	pack $win.frameList.header \
		-side top -fill x
	pack $canvas \
		-side left -fill both
	pack $win.frameList.yscroll \
		-side right -fill y

	set frame $win.frameButton
	frame $frame \
		-borderwidth 0
	button $frame.open \
		-text [mc choose-Open] -command "NSChooseGame::Info $oop result open" \
		-width 9 -default active
	button $frame.browse \
		-text [mc Browse...] -command "NSChooseGame::Browse $oop" -width 9
	button $frame.cancel \
		-text [mc Cancel] -command "NSChooseGame::Info $oop result cancel" \
		-width 9

	switch -- [Platform] {
		windows {
			set padx 5
		}
		unix {
			set padx 2
		}
	}
set padx 10

	pack $frame.open \
		-side left -padx 0
	pack $frame.browse \
		-side left -padx $padx
	pack $frame.cancel \
		-side left -padx 0

	label $win.fileName \
		-anchor w -justify left -font $font

	grid rowconfigure $win 0 -weight 0
	grid rowconfigure $win 1 -weight 1
	grid rowconfigure $win 2 -weight 0
	grid rowconfigure $win 3 -weight 0
	grid columnconfigure $win 0 -weight 1
	grid columnconfigure $win 1 -weight 0

	grid $win.framePrompt \
		-row 0 -column 0 -rowspan 1 -columnspan 2 -padx 10 -pady 0 -sticky ew
	grid $win.frameList \
		-row 1 -column 0 -rowspan 1 -columnspan 2 -padx 10 -pady 5 -sticky news
	grid $win.fileName \
		-row 2 -column 0 -rowspan 1 -columnspan 1 -padx 20 -sticky ew
	grid $win.frameButton \
		-row 2 -column 1 -rowspan 1 -columnspan 1 -padx 10 -pady 5
	grid [frame $win.filler1 -borderwidth 0 -height 5] \
		-row 3 -column 1 -rowspan 1 -columnspan 2 -padx 10 -sticky ew
		
	NSUtils::SetDefaultButton $win $win.frameButton.open
	
#	bind $win <KeyPress-o> \
#		"tkButtonInvoke $win.frameButton.open"
#	bind $win <KeyPress-b> \
#		"tkButtonInvoke $win.frameButton.browse"
	bind $win <KeyPress-Return> \
		"NSUtils::InvokeDefaultButton $win"
	bind $win <KeyPress-Escape> \
		"tkButtonInvoke $win.frameButton.cancel"

	# Select a row in the list as the user types
	Info $oop typing,string ""
	Info $oop typing,click 0
	bind $canvas <KeyPress> "NSChooseGame::Typing $oop %A"

	return
}

# NSChooseGame::Close --
#
#	Do something when closing the window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseGame::Close {oop} {

	destroy [Info $oop win]
#	NSObject::Delete NSChooseGame $oop

#	namespace delete ::NSChooseGame

	return
}

# NSChooseGame::SelectionChanged --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseGame::SelectionChanged {oop canvistId select deselect} {

	set win [Info $oop win]

	if {![llength $select]} {
		$win.fileName configure -text ""
		$win.frameButton.open configure -state disabled
		Info $oop current -1
		return
	}

	set row [lindex $select 0]
	set path [lindex [Info $oop pathList] $row]
	set pad "    "
	if {[Platform unix]} {
		set pad "  "
	}
	$win.fileName configure -text "[mc choose-file]\n$pad\"[file tail $path]\""

	Info $oop current $row
	$win.frameButton.open configure -state normal

	return
}

# NSChooseGame::ReadSavefileInfo --
#
#	Read the tk\config\savefile file.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseGame::ReadSavefileInfo {oop} {

	global Angband
	variable Priv

	set file [PathTk config savefile]
	if {![file exists $file]} return

	set fileId [open $file]
	catch {
		foreach string [split [read $fileId] \n] {
			if {![string length $string]} continue
			if {[string equal [string index $string 0] #]} continue
			set count [scan $string \
				{"%[^\"]" %d %d %s "%[^\"]" "%[^\"]" "%[^\"]" %d %d %d %d"} \
				file mtime size version name race class level \
				depth max_depth is_dead]
			if {$count == 11} {
				set Priv(savefile,$file) [list file $file mtime $mtime \
					size $size version $version name $name race $race \
					class $class \
					lev $level depth $depth max_depth $max_depth \
					is_dead $is_dead]
			} elseif {$count == 4} {
				set Priv(savefile,$file) [list file $file mtime $mtime \
					size $size version $version name ""]
			}
		}
	}
	close $fileId

	return
}

# NSChooseGame::ModalLoop --
#
#	Present the dialog, interact, then withdraw.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseGame::ModalLoop {oop} {

	global Angband
	global NSChooseGame
	variable Priv

	set win [Info $oop win]
	set canvistId [Info $oop canvistId]
	set canvas [NSCanvist::Info $canvistId canvas]
	set font [Info $oop font]
	set progId [Info $oop progId]

	set elemList {}

	# Read tk\config\savefile, which contains info about each savefile.
	# This is faster than calling "angband game savefile_info".
	ReadSavefileInfo $oop

	# Create tk\config\savefile.
	set fileId [openlf [PathTk config savefile]]
	puts $fileId "# Automatically generated. Do not edit.\n"

	set wd [pwd]
	cd [Path lib save]
	set fileList [lsort -dictionary [glob -nocomplain *]]
	set count [llength $fileList]
	set i 0
	foreach file $fileList {

		# Ignore too-small files here, and hope the
		# version check in savefile_info() catches other
		# non-savefiles
		if {[file size $file] < 5000} continue

		set path [Path lib save $file]
		catch {
			set path [LongName $path]
		}
		set addFile 1
		set doScan 1

		# See if info for this savefile exists in our cache
		if {[info exists Priv(savefile,$file)]} {
			array set attrib $Priv(savefile,$file)

			# If this file appears the same as what was cached, then
			# there is no need to scan it again.
			if {($attrib(mtime) == [file mtime $file]) && \
				($attrib(size) == [file size $file])} {
				set doScan 0
			}
		}

		# Parse this file (it might not be a savefile)
		if {$doScan} {
			if {[catch {
				set attrib(name) ""
				angband game savefile_info $path attrib
			}]} {
				set addFile 0
			}
		}

		# This isn't a valid savefile
		if {!$addFile} {
			incr i
			continue
		}

		# If the name is empty it is an old savefile
		if {![string length $attrib(name)]} {
			lappend elemList [list $file $path $attrib(version) "" 0]
			puts $fileId "\"$file\" [file mtime $file] [file size $file]\
				$attrib(version)"

		# We have complete info about this savefile
		} else {
			lappend elemList [list $attrib(name) $path $attrib(version) \
				"Level $attrib(lev) $attrib(race) $attrib(class),\
				Depth $attrib(depth)/$attrib(max_depth)" $attrib(is_dead)]
			puts -nonewline $fileId "\"$file\" [file mtime $file]\
				[file size $file] $attrib(version)"
			puts -nonewline $fileId " \"$attrib(name)\" \"$attrib(race)\"\
				\"$attrib(class)\" $attrib(lev)"
			puts $fileId " $attrib(depth) $attrib(max_depth) $attrib(is_dead)"
		}

		incr i
		NSProgressWindow::Info $progId prompt [format [mc scan-file] $file]
		NSProgressWindow::Info $progId ratio [expr {$i / double($count)}]
		update idletasks
	}
	cd $wd

	# Close tk\config\savefile
	close $fileId

	NSProgressWindow::Info $progId ratio 1.0
	NSProgressWindow::Info $progId prompt [mc scan-display]
	update idletasks

	# Assume the list will be empty
	$win.frameButton.open configure -state disabled

	Info $oop pathList {}
	Info $oop elemList $elemList

	# Hack -- Caculate maximum needed display size
	set Priv(width,name) [expr {[llength $elemList] ? 0 : 120}]
	set Priv(width,version) [font measure $font Version]
	set Priv(width,other) 0
	
	foreach elem $elemList {
		set name [lindex $elem 0]
		set version [lindex $elem 2]
		set other [lindex $elem 3]
		set width [font measure [BoldFont $font] $name]
		if {$width > $Priv(width,name)} {
			set Priv(width,name) $width
		}
		set width [font measure $font $version]
		if {$width > $Priv(width,version)} {
			set Priv(width,version) $width
		}
		set width [font measure $font $other]
		if {$width > $Priv(width,other)} {
			set Priv(width,other) $width
		}
	}

	# Resize the canvas, if necessary
	set width [expr {$Priv(width,name) + 32 + $Priv(width,version) + 16 + \
		$Priv(width,other) + 8}]
	if {$width > [winfo reqwidth $canvas]} {
		$canvas configure -width $width
	}

	# Are there any files?
	if {[llength $elemList]} {

		# Set the list
		SetList $oop

		# Resize the canvas to the required dimensions
		set bbox [$canvas bbox all]
		$canvas configure \
			-scrollregion [list 0 0 [lindex $bbox 2] [lindex $bbox 3]]

		NSToplevel::NaturalSize $win ""
			
		# Select the initial file
		set recent [Info $oop recent]
		if {$recent != -1} {
			NSCanvist::UpdateSelection $canvistId $recent {}
			NSCanvist::See $canvistId $recent
		}
	} else {

		NSToplevel::NaturalSize $win ""
	}

	# Position the column titles. These values were determined by
	# trial-and-error, although they seem to work correctly for
	# MS Sans Serif and a fixed-width font.
	set frame $win.frameList.header
	set offset 0

	place $frame.label0 -x $offset -y 0 \
		-width [expr {$Priv(width,name) + 32 - 2}] -anchor nw
	incr offset [expr {$Priv(width,name) + 32 - 2}]
	place $frame.label1 -x $offset -y 0 \
		-width [expr {$Priv(width,version) + 16 + 0}] -anchor nw
	incr offset [expr {$Priv(width,version) + 16 + 0}]
	place $frame.label2 -x $offset -y 0 \
		-width [expr {[winfo width $frame] - $offset}] \
		-anchor nw

	NSProgressWindow::Close $progId

	# Position window
	WindowPosition $win 2 3

	# Weird. The yscrollcommand gets called 3 times,
	# but still the scrollbar stays at the top...
	update
	eval $win.frameList.yscroll set [$canvas yview]

	# Set up a grab and claim focus too
	NSUtils::GrabSave $win
	focus $canvas

	# Wait
	Info $oop result ""
	tkwait variable NSChooseGame($oop,result)

	# Release grab and reset focus
	NSUtils::GrabRelease $win

	# Hack -- Update the button
	update

	switch -- [Info $oop result] {
		browse {
			Close $oop
			set path $Priv(recent,path)
			Value savefile [file split $path]
			after idle angband game open [list $path]
		}
		open {
			set index [Info $oop current]
			set pathList [Info $oop pathList]
			set path [lindex $pathList $index]
			set Priv(recent,path) $path
			Value savefile [file split $path]
			Close $oop
			after idle angband game open [list $path]
		}
		cancel {
			Close $oop
		}
	}

	return
}

# NSChooseGame::OpenGame --
#
#	Choose a savefile and play a game.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseGame::Browse {oop} {

	global Angband
	variable Priv

	set win [Info $oop win]

	# Somebody reported that the file list is empty on Windows NT 3.5
	# and suggested I use an initial file name of *.*.
	set types {
		{{All Files} *}
	}
	set fileName [tk_getOpenFile -filetypes $types -parent $win \
		-initialfile *.* \
		-initialdir [Path lib save]]
	if {[string length $fileName]} {
		if {[catch {
			set path [LongName $fileName]
		}]} {
			set path $fileName
		}
		set Priv(recent,path) $path
		Info $oop result browse
	}

	return
}

# NSProjectList::SortBy --
#
#	Sets the list again, using the given sorting method.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseGame::SortBy {oop index} {

	variable Priv

	set canvistId [Info $oop canvistId]
	set current [Info $oop current]
	set pathList [Info $oop pathList]
		
	# Hack -- SetList() will set "recent" to the row of the recently
	# used file. We save the current values here and replace the
	# recent file with the currently selected file. Then SetList()
	# will find the new row of the selected file for us.
	if {$current != -1} {
		set oldPath $Priv(recent,path)
		set oldRecent [Info $oop recent]
		set Priv(recent,path) [lindex $pathList $current]
	}
	
	Info $oop sortBy $index
	SetList $oop

	# Select the previously-selected file.
	if {$current != -1} {
		NSCanvist::UpdateSelection $canvistId [Info $oop recent] all
		NSCanvist::See $canvistId [Info $oop recent]
		set Priv(recent,path) $oldPath
		Info $oop recent $oldRecent
	}

	return
}

# NSProjectList::SetList --
#
#	Set the list of savefiles.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseGame::SetList {oop} {

	variable Priv

	set win [Info $oop win]
	set canvistId [Info $oop canvistId]
	set canvas [NSCanvist::Info $canvistId canvas]
	set elemList [Info $oop elemList]
	set font [Info $oop font]
	set sortBy [Info $oop sortBy]

	# Clear the list
	NSCanvist::DeleteAll $canvistId

	# Underline the column title according to the sorting method
	set frame $win.frameList.header
	foreach i {0 1 2} {
		if {$i == $sortBy} {
			$frame.label$i configure -font "$font underline"
		} else {
			$frame.label$i configure -font $font
		}
	}

	# Determine the element index to sort by, and sort them.
	# Sorting by name and version works as expected. Sorting by
	# description ends up sorting by experience level.
	set index [lindex [list 0 2 3] $sortBy]
	set elemList [lsort -dictionary -index $index $elemList]

	# Remember the row of the recently-used file
	set recent -1

	# Track the row number
	set row 0

	# Collect info for each row
	set itemList {}

	# Remember the file path for each row
	set pathList {}

	# Remember the character name for each row
	set nameList {}
	
	# Check each savefile
	foreach elem $elemList {

		set name [lindex $elem 0]
		set path [lindex $elem 1]
		set version [lindex $elem 2]
		set other [lindex $elem 3]

		# Living or dead
		if {[lindex $elem 4]} {
			set style normal
		} else {
			set style bold
		}

		# Collect info for each row
		lappend itemList [list $name $version $other $style]

		# Remember the row of the recently-used file
		if {[string equal $Priv(recent,path) $path]} {
			set recent $row
		}

		# Remember the file path for each row
		lappend pathList $path

		# Remember the character name for each row
		lappend nameList $name

		# Track the row number
		incr row
	}

	# Add each row to the list
	NSCanvist::InsertMany $canvistId end $itemList

	# Remember the file path for each row
	Info $oop pathList $pathList

	# Remember the character name for each row
	Info $oop nameList $nameList

	# Remember the row of the recently-used file
	Info $oop recent $recent
	
	return
}

# NSProjectList::NewItemCmd --
#
#	Called by NSCanvist::InsertItem() to create a list row.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseGame::NewItemCmd {oop canvistId y name version info style} {

	variable Priv

	set c [NSCanvist::Info $canvistId canvas]
	set lineHeight [NSCanvist::Info $canvistId rowHgt]

	set font [Info $oop font]
	set fw [font measure [BoldFont $font] $name]
	set fh [font metrics $font -linespace]
	set diff [expr {int([expr {($lineHeight - $fh) / 2}])}]
	set yText [expr {$y + $diff}]
	
	# Background under name
	lappend itemIdList [$c create rectangle 0 [expr {$y + 1}] \
		[winfo reqwidth $c] [expr {$y + $lineHeight - 1}] -fill #dedede \
		-outline #dedede -tags sel]

	# Name
	set left 4
	lappend itemIdList [$c create text $left $yText \
		-text $name -anchor nw -font [AddStyle $font $style] -tags text]

	# Version
	incr left [expr {$Priv(width,name) + 32}]
	lappend itemIdList [$c create text $left $yText \
		-text $version -anchor nw -font $font -tags text]

	# Other
	incr left [expr {$Priv(width,version) + 16}]
	lappend itemIdList [$c create text $left $yText \
		-text $info -anchor nw -font $font -tags text]

	return $itemIdList
}

# NSProjectList::HighlightItemCmd --
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

proc NSChooseGame::HighlightItemCmd {oop canvistId state args} {

	set canvas [NSCanvist::Info $canvistId canvas]
	set itemIdList $args

	set idRect [FindItemByTag $canvas $itemIdList sel]
	set idText [FindItemByTag $canvas $itemIdList text]

	if {$state} {

		set fill [Global SystemHighlight]
		$canvas itemconfigure $idRect -fill $fill -outline $fill

		set fill [Global SystemHighlightText]
		foreach id $idText {
			$canvas itemconfigure $id -fill $fill
		}

	} else {

		$canvas itemconfigure $idRect -fill #dedede -outline #dedede
		foreach id $idText {
			$canvas itemconfigure $id -fill Black
		}
	}

	return
}

# NSChooseGame::Typing --
#
#	Highlight a savefile as the user types in a name.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSChooseGame::Typing {oop string} {

	if {![string match \[A-Za-z0-9\] $string]} return

	set canvistId [Info $oop canvistId]
	set current [Info $oop current]
	set nameList [Info $oop nameList]

	# This starts at row 0 if current is -1
	incr current
	
	set max [NSCanvist::Info $canvistId count]

	set clickPrev [Info $oop typing,click]
	set clickCur [clock clicks -milliseconds]

	# Eh? "clock clicks" goes negative on my Linux box.
	if {[Platform unix] && $clickCur < 0} {
		set clickCur [expr abs($clickCur)]
	}

	if {abs($clickCur - $clickPrev) < 300} {
		set string [Info $oop typing,string]$string
	}
	Info $oop typing,click $clickCur
	Info $oop typing,string $string

	set string [string tolower $string]

	for {set row $current} {$row < $max} {incr row} {
		set name [string tolower [lindex $nameList $row]]
		if {[string match $string* $name]} {
			NSCanvist::UpdateSelection $canvistId $row all
			NSCanvist::See $canvistId $row
			return
		}
	}

	for {set row 0} {$row < $current} {incr row} {
		set name [string tolower [lindex $nameList $row]]
		if {[string match $string* $name]} {
			NSCanvist::UpdateSelection $canvistId $row all
			NSCanvist::See $canvistId $row
			return
		}
	}

	return
}

