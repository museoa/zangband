# File: boot.tcl

# Purpose: pick a variant

#
# Copyright (c) 1997-2000 Tim Baker
#
# This software may be copied and distributed for educational, research, and
# not for profit purposes provided that this copyright and statement are
# included in all such copies.
#

namespace eval Boot {
}

proc Boot::Boot {} {

	global tcl_platform
	variable Priv

	# Determine the full path of the OmnibandTk directory
	set path [file dirname [info script]]
	if {$tcl_platform(platform) == "windows"} {
		set path [file attributes $path -longname]
	}
	if {[string compare [file pathtype $path] absolute]} {
		set path [file join [pwd] $path]
	}
	set path [CleanPath $path]
	set Priv(root) [file dirname $path]

	# Create list of installed variants
	set glob [glob -nocomplain -types d [file join $Priv(root) variant *]]
	set glob [lsort -dictionary $glob]
	set Priv(variant,name) {}
	set Priv(variant,shlib) {}
	foreach path $glob {
		set shlib [file join $path angband[info sharedlibextension]]
		if {[file exists $shlib]} {
			lappend Priv(variant,name) [file tail $path]
			lappend Priv(variant,shlib) $shlib
		}
	}

	wm withdraw .
	if {[llength [info commands console]]} {
		console hide
	}

	# If there is only one installed variant, don't ask
	if {[llength $Priv(variant,shlib)] == 1} {
		boot [lindex $Priv(variant,shlib) 0]

	# Try default variant
	} elseif {[TryDefaultVariant]} {
	
	# More than one installed variant, allow the user to choose one
	} else {
uplevel #0 source [file join $Priv(root) tk library utils.tcl]
		Boot::ChooseVariant
	}

	# Clean up
	namespace delete ::Boot

	return
}

# /home/./tnb/foo/../bar ==> /home/tnb/bar
proc Boot::CleanPath {path} {

	set result {}
	foreach elem [file split $path] {
		if {[string equal $elem .]} continue
		if {[string equal $elem ..]} {
			set result [lrange $result 0 end-1]
			continue
		}
		lappend result $elem
	}

	return [eval file join $result]
}

proc Boot::TryDefaultVariant {} {

	variable Priv

	set path [file join $Priv(root) tk config variant]
	if {![file exists $path]} {
		return 0
	}

	set chan [open $path]
	set string [string trim [gets $chan]]
	close $chan

	if {[string length $string]} {
		set i [lsearch -exact $Priv(variant,name) $string]
		if {$i == -1} {
			return 0
		}
		set shlib [lindex $Priv(variant,shlib) $i]
		boot $shlib
		return 1
	}

	return 0
}

proc Boot::SaveDefaultVariant {name} {

	variable Priv

	set path [file join $Priv(root) tk config variant]

	if {!$Priv(always)} {
		if {[file exists $path]} {
			file delete $path
		}
		return
	}

	set chan [open $path w]
	puts $chan $name
	close $chan

	return
}

proc Boot::ChooseVariant {} {

	variable Priv
	global tcl_platform

	set win .chooseVariant
	toplevel $win
	wm title $win "Choose Variant"

	# Set the window icon on Win32
	if {$tcl_platform(platform) == "windows"} {
		wm iconbitmap . -default [file join $Priv(root) tk image angbandtk.ico]
	}

	#
	# Listbox & scrollbar
	#

	set frame $win.frameList
	frame $frame -borderwidth 1 -relief sunken
	listbox $frame.listbox -width 35 -height 6 \
		-yscrollcommand "$frame.yscroll set" \
		-borderwidth 0 -highlightthickness 0 -background White \
		-listvariable ::Boot::Priv(variant,name)
	scrollbar $frame.yscroll -orient vertical -command "$frame.listbox yview"
	pack $frame.listbox $frame.yscroll -side left -fill y

	#
	# Checkbutton
	#

	set Priv(always) 0
	checkbutton $win.check -text "Always choose this variant" \
		-variable ::Boot::Priv(always)

	#
	# Buttons
	#

	set frame $win.frameButton
	frame $frame -borderwidth 0
	button $frame.buttonCont -text OK -command "set Boot::Priv(button) ok" \
		-width 11
	button $frame.buttonQuit -text Quit -command "set Boot::Priv(button) cancel" \
		-width 11
	pack $frame.buttonCont -side left
	pack $frame.buttonQuit -side left -padx 6

	NSUtils::SetDefaultButton $win $frame.buttonCont
	bind $win <KeyPress-Return> "NSUtils::InvokeDefaultButton $win"
	bind $win <KeyPress-Escape> "tkButtonInvoke $frame.buttonQuit"

	pack [frame $win.frameFill -borderwidth 0 -height 6] -side top -fill x
	pack $win.frameList -side top -padx 6
	pack $win.check -side top -pady 6
	pack $win.frameButton -side top -anchor e
	pack [frame $win.frameFill2 -borderwidth 0 -height 6] -side top -fill x

	$win.frameList.listbox selection set 0

	# Center the window
    set x [expr {([winfo screenwidth $win] - [winfo reqwidth $win]) / 2 \
	    - [winfo vrootx $win]}]
    set y [expr {([winfo screenheight $win] - [winfo reqheight $win]) / 3 \
	    - [winfo vrooty $win]}]
    wm geometry $win +$x+$y

	# Wait for a button click
	grab $win
	tkwait variable ::Boot::Priv(button)
	grab release $win

	if {$Priv(button) == "cancel"} exit

	# Tell angband.exe which DLL to load
	set row [$win.frameList.listbox curselection]
	set shlib [lindex $Priv(variant,shlib) $row]
	boot $shlib

	SaveDefaultVariant [lindex $Priv(variant,name) $row]

	destroy $win

	return
}

Boot::Boot

