# File: term.tcl

# Purpose: the Main Terminal

#
# Copyright (c) 1997-2001 Tim Baker
#
# This software may be copied and distributed for educational, research, and
# not for profit purposes provided that this copyright and statement are
# included in all such copies.


namespace eval NSTerm {
	
	variable Priv
	
	variable Term
}


# Startup
proc NSTerm::InitModule {} {

	variable Priv

	InitWindow
}


# Cleanup
proc NSTerm::CloseModule {} {

	variable Priv
	
	variable Term

	catch {
		destroy Term
	}

	exit
}

# Close
proc NSTerm::Close {} {

	variable Term

	wm withdraw $Term
}

# Redraw screen
proc NSTerm::Redraw {} {

	variable Priv
	
	variable Term

	
	set width 640
	set height 480

	set fg Black
	set bg White

	set grids $Term.grids
	
	frame $grids
			
# Create rows + columns
	for {set j 0} {$j <= 23} {incr j} {
		frame $grids.row$j
		for {set i 0} {$i <= 79} {incr i} {
			set char [term_window char $i $j]
			label $grids.row$j.col$i -text $char -bg blue 
#			         -font font,fixed,normal
			pack $grids.row$j.col$i -side right
		}
		
		pack $grids.row$j -side top
	}	
	
	pack $grids -padx 1 -pady 1

	update
}

proc NSTerm::InitWindow {} {

	variable Priv
	
	variable Term
	
	set Term .terms

	toplevel $Term
	
	wm title $Term "The Main Terminal"
	wm title . "Not The Main Terminal"
	wm resizable $Term no no

	# Do stuff when window closes
	wm protocol $Term WM_DELETE_WINDOW "NSTerm::Close"


	# Click/Escape to hide the window
	bind $Term <ButtonPress-1> "NSTerm::Close"
	bind $Term <KeyPress-Escape> "NSTerm::Close"
	
	wm minsize $Term 80 24
	
	Redraw
	
	raise $Term
	focus $Term

	update

	return
}

NSTerm::InitModule

