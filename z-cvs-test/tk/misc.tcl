# File: misc.tcl

# Purpose: various commands

#
# Copyright (c) 1997-2001 Tim Baker
#
# This software may be copied and distributed for educational, research, and
# not for profit purposes provided that this copyright and statement are
# included in all such copies.
#

# cnv_stat --
#
#	Converts a raw stat value into a human-readable value. Values from
#	3 to 18 are returned unchanged.
#		ex. "118" becomes "18/100"
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc cnv_stat {val} {

	# Above 18
	if {$val > 18} {

		set bonus [expr {$val - 18}];
		if {$bonus >= 10} {
			return 18/$bonus
		}
		return 18/0$bonus

	# From 3 to 18
	} else {

		return $val
	}
}

# cnv_stat_disp --
#
#	Same as cnv_stat(), but any bonus greater than 220 is displayed as
#	"***".
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc cnv_stat_disp {val} {

	# Above 18
	if {$val > 18} {

		set bonus [expr {$val - 18}];
		if {$bonus >= 220} {
			return 18/***
		} elseif {$bonus >= 10} {
			return 18/$bonus
		}
		return 18/0$bonus

	# From 3 to 18
	} else {

		return $val
	}
}

# ImageExists --
#
#	Return true if an image with the given name exists.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc ImageExists {imageName} {

	set imageList [image names]
	return [expr {[lsearch -exact $imageList $imageName] != -1}]
}

# InitImageIfNeeded --
#
#	Creates a new photo image from the given file. If the image
#	already exists, nothing happens.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc InitImageIfNeeded {imageName fileName args} {

	global Angband

	if {[ImageExists $imageName]} {
		return 0
	}

	# Look in CommonTk/tk/image and subdirectories, then user-supplied tk
	# subdirectories
	foreach elem [concat [list {image} {image dg} {image dg misc-win}] $args] {
		set path [eval CPathTk $elem [list $fileName]]
		if {[file exists $path]} {
			image create photo $imageName -file $path
			return 1
		}
	}

	# Look in tk/image and subdirectories, then user-supplied tk
	# subdirectories
	foreach elem [concat [list {image} {image dg} {image dg misc-win}] $args] {
		set path [eval PathTk $elem [list $fileName]]
		if {[file exists $path]} {
			image create photo $imageName -file $path
			return 1
		}
	}

	error "can't find image file \"$fileName\""
	
	return 1
}

# openlf --
#
#	Open a file for writing, and set the translation mode to "lf" as well.
#	It seems that writing unix-style files is faster as well as smaller.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc openlf {fileName} {

	set id [open $fileName w]
	fconfigure $id -translation lf
	return $id
}

# NewItemCmd_Assign --
#
#	Called by NSCanvist2::InsertItem() to create a list row.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NewItemCmd_Assign {canvistId x y assign} {

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
	set iw [icon size]
	set ih [icon size]
	set xdiff [expr {int([expr {($columnWidth - $iw) / 2}])}]
	set ydiff [expr {int([expr {($rowHeight - $ih) / 2}])}]
	lappend itemIdList [$c create widget [expr {$x + $xdiff}] \
		[expr {$y + $ydiff}] -assign $assign -tags enabled]

	return $itemIdList
}

# SetWindowIcon --
#
#	When a toplevel is mapped for the first time, set the icon,
#	if the window is not transient.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc SetWindowIcon {win} {

	global WindowIcon

	if {![info exists WindowIcon($win)]} {
		if {![string length [wm transient $win]]} {
			angband system windowicon $win
		}
		set WindowIcon($win) 1
	}

	return
}

if {[string compare 8.3.3 [info patchlevel]] < 0} {

bind Toplevel <Map> {+
	SetWindowIcon %W
}

# The pesky root window does not have the Toplevel bindtag!!!
bind . <Map> {+
	SetWindowIcon .
}

# < 8.3.3
}

# fmt_wgt --
#
#	Convert 10ths of lb to ib, or kg.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc fmt_wgt {wgt {showUnits 0}} {

	set units lb

	set result [format "%d.%d" [expr {$wgt / 10}] [expr {$wgt % 10}]]
	if {$showUnits} {
		append result " $units"
	}
	return $result
}

# ChangeCharacterName --
#
#	Allow the user to change the name of the character.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc ChangeCharacterName {parent} {

	# Ask until a valid name is entered, or cancel
	while 1 {

		# Ask for a new name
		set name [NSUtils::StringBox -title "Character Name" \
			-initial [angband player name] -prompt "Name" \
			-buttons [list OK Cancel] -parent $parent \
			-entrywidth 20 -maxlen 15]

		# Validate the name
		if {[string length $name]} {
			if {[catch {angband player name $name} result]} {
				set message "\"$name\" is not a valid character name.\n"
				append message "The character name must contain no more than 15 characters."
				tk_messageBox -icon info -title "Invalid Name" \
					-message $message
				continue
			}
		}
		break
	}
	
	return
}


# MessageDumpAux --
#
#	Dump a list of messages to the given file. Similar messages are
#	combined into a single line.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc MessageDumpAux {filePath} {

	set max [angband message count]

	set curMsg ""
	set count 0
	
	for {set i [expr {$max - 1}]} {$i >= 0} {incr i -1} {
		set nextMsg [angband message get $i]
		if {[string compare $curMsg $nextMsg]} {
			if {[string length $curMsg]} {
				if {$count > 1} {
					append curMsg " (x$count)"
				}
				append buffer $curMsg\n
			}
			set curMsg $nextMsg
			set count 1
		} else {
			incr count
		}
	}
	if {$count > 1} {
		append curMsg " (x$count)"
	}
	append buffer $curMsg\n

	if {[catch {openlf $filePath} fileId]} {
		set msg "The following error occurred while attempting to open "
		append msg "the message log file for writing:\n\n$fileId"
		tk_messageBox -title Oops -message $msg
		return 1
	}

	puts $fileId "# Message Dump for [angband player name]\n"
	puts $fileId $buffer
	close $fileId

	return 0
}

# MessageDump --
#
#	Dump a list of messages to a file. 
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc MessageDump {parent} {

	global Angband

	set fileName [tk_getSaveFile -initialfile [angband player base_name].msg \
		-initialdir [PathUser] -parent $parent]
	if {![string length $fileName]} return

	MessageDumpAux $fileName

	return
}

# AbortGame --
#
#	Quit without saving.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc AbortGame {} {

	global Angband

	set answer [tk_messageBox -type yesno -title "Abort $Angband(name)" \
		-message "Write config files?"]
	if {[string equal $answer yes]} {
		angband_close_game
	}

	# In any event, delete the temp photo.txt
	if {[info exists ::Global(photoText)]} {
		set tempFile [Global photoText]
		if {[string length $tempFile] && [file exists $tempFile]} {
			file delete $tempFile
		}
	}

	# Bye!
	angband game abort -noask
	
	return
}

# MakeStatusBar --
#
#	Creates a typical status bar.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc MakeStatusBar {statusBar args} {

	NSModule::LoadIfNeeded NSStatusBar

	lappend sizes 0
	lappend weights 1
	lappend tags t1
	
	set i 2
	foreach size $args {
		lappend sizes $size
		lappend weights 0
		lappend tags t$i
		incr i
	}
	
	statusbar $statusBar -sizes $sizes -weights $weights -tags $tags

	return
}

# MakeDivider --
#
#	Creates a typical divider.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc MakeDivider {divider orient} {

	if {[Platform unix]} {
		set relief sunken
	}
	if {[Platform windows]} {
		set relief groove
	}
	switch -- $orient {
		x {set option -height}
		y {set option -width}
	}

	return [frame $divider -borderwidth 1 $option 2 -relief $relief]
}

# Path --
#
#	Create a path relative to Angband(dir)
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc Path {args} {

	global Angband

	return [eval file join [list $Angband(dir)] $args]
}

# PathTk --
#
#	Create a path relative to Angband(dirTK)
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc PathTk {args} {

	global Angband

	return [eval file join [list $Angband(dirTK)] $args]
}

# CPath --
#
#	Create a path relative to Angband(dir,common)
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc CPath {args} {

	global Angband

	return [eval file join [list $Angband(dir,common)] $args]
}

# CPathTk --
#
#	Create a path relative to Angband(dir,common,tk)
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc CPathTk {args} {

	global Angband

	return [eval file join [list $Angband(dir,common,tk)] $args]
}

# PathUser --
#
#	Create a path relative to Angband(dir,user)
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc PathUser {args} {

	global Angband

	return [eval file join [list $Angband(dir,user)] $args]
}

# AddStyle --
#
#	Add one or more styles to the given font description if needed.
#	If you just say "$font bold" and $font is "Times 12 {bold italic}"
#	you get an error.
#	FIXME: The font family may change (ex from Times to {Times New Roman}).
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc AddStyle {font args} {

	array set desc [fontdesc $font]
	set result "[list $desc(-family)] $desc(-size)"
	foreach style $args {
		switch -- $style {
			bold {
				set desc(-weight) bold
			}
			italic {
				set desc(-slant) italic
			}
			overstrike {
				set desc(-overstrike) 1
			}
			underline {
				set desc(-underline) 1
			}
		}
	}
	if {[string equal $desc(-weight) bold]} {
		append result " bold"
	}
	if {[string equal $desc(-slant) italic]} {
		append result " italic"
	}
	if {$desc(-underline)} {
		append result " underline"
	}
	if {$desc(-overstrike)} {
		append result " overstrike"
	}
	return $result
}

# BoldFont --
#
#	Add "bold" style to the given font description if needed.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc BoldFont {font} {

	return [AddStyle $font bold]
}

