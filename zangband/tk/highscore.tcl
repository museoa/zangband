# File: highscore.tcl

# Purpose: the High Scores Window and related commands

#
# Copyright (c) 1997-2001 Tim Baker
#
# This software may be copied and distributed for educational, research, and
# not for profit purposes provided that this copyright and statement are
# included in all such copies.
#

namespace eval NSHighScore {

	# The high score read by highscore_read
	variable highscore
	
	# The score for the current character
	variable highscore_index -1
	
	# List of name/value pairs
	variable highscore_list {}
	
	variable MenuString
	variable Priv

# namespace eval NSHighScore
}

# NSHighScore::InitModule --
#
#	One-time-only-ever initialization.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::InitModule {} {

	variable Priv

	MsgCatInit score

	set Priv(find,string) ""
	set Priv(find,index) -1

	# Create the High Scores Window
	NSObject::New NSHighScore

	if {![NSModule::LoadIfNeeded NSRecord]} {
		#
		# The "wm transient" command always displays the window
		# in the "normal" state. I think this is a bug and the
		# window should be displayed in the same state as the
		# master window (that's what the 8.0.4 comment says).
		#
		# Anyways, when the game is over this causes the
		# Character Record Window to be displayed again,
		# forcing me to hide it. Because this flashes on the
		# screen I move the CRW offscreen, make it transient,
		# and then hide it again.
		#
# Fixed in 8.3b2
if {[info tclversion] >= 8.3} {
#	wm transient [Window record] [Window highscore]
} else {
		set win [Window record]
		set x [winfo x $win]
		set offset [winfo screenwidth .]
		wm geometry $win +$offset+[winfo y $win]
		update idletasks
		wm transient $win [Window highscore]
		wm withdraw $win
		update idletasks
		wm geometry $win +$x+[winfo y $win]
}
	}

	# Important: Since we always reload this module after a character
	# dies, we reload the record info here so the dead character's
	# record (if any) is properly read again. See "angband_display highscore".
	NSRecord::LoadRecords

	return
}

# NSHighScore::CloseModule --
#
#	One-time-only-ever cleanup.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::CloseModule {} {

	catch {
		destroy [Window highscore]
	}

	return
}

# NSHighScore::NSHighScore --
#
#	Object constructor called by NSObject::New().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::NSHighScore {oop} {

	# Assume we will be called from C
	Info $oop interactive 1

	Info $oop current -1
	Info $oop match {}
	Info $oop sortBy pts

	InitWindow $oop

	set win [Info $oop win]

	NSWindowManager::RegisterWindow highscore $win \
		"GetDefaultGeometry $win screen main" "" \
		"NSHighScore::DisplayCmd $oop"

	# Update ourself when the listBG value changes
	Info $oop listBG,clientId \
		[NSValueManager::AddClient listBG \
		"NSHighScore::ValueChanged_listBG $oop"]

	# Destroy the object along with the toplevel (later)
	NSUtils::DestroyObjectWithWidget NSHighScore $oop $win

	#
	# Global list of application windows
	#

	Global highscore,oop $oop
	Window highscore $win

	return
}

# NSHighScore::~NSHighScore --
#
#	Object destructor called by NSObject::Delete().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::~NSHighScore {oop} {

	NSValueManager::RemoveClient listBG [Info $oop listBG,clientId]

	return
}

# NSHighScore::Info --
#
#	Query and modify info.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::Info {oop info args} {

	global NSHighScore

	# Verify the object
	NSObject::CheckObject NSHighScore $oop

	# Set info
	if {[llength $args]} {
		switch -- $info {
			default {
				set NSHighScore($oop,$info) [lindex $args 0]
			}
		}

	# Get info
	} else {
		switch -- $info {
			default {
				return $NSHighScore($oop,$info)
			}
		}
	}

	return
}

# NSHighScore::InitWindow --
#
#	Create the window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::InitWindow {oop} {

	global Angband

	set win .highscore$oop
	toplevel $win
	wm title $win ["Zangband Hall of Fame"]

	# Hack -- see "angband_display highscore"
	if {![angband player is_dead]} {
		wm transient $win [Window main]
	}

	# Start out withdrawn (hidden)
	wm withdraw $win

	# Do stuff when window closes
	wm protocol $win WM_DELETE_WINDOW "NSHighScore::Close $oop"

	Info $oop win $win

	InitMenus $oop

	# Divider
	MakeDivider $win.divider1 x

	#
	# High Score List
	#

	set font [Global font,fixed,normal]
	set cw [font measure $font "W"]
	set fh [font metrics $font -linespace]
	set rowHeight [expr {$fh * 3 + 8}]
	set height [expr {$rowHeight * 5}]
	set width [expr {$cw * 80}]

	frame $win.frameList \
		-borderwidth 1 -relief sunken
	set canvistId [NSObject::New NSCanvist $win.frameList $rowHeight $width $height \
		"NSHighScore::NewItemCmd $oop" "NSHighScore::HighlightItemCmd $oop"]
	set canvas [NSCanvist::Info $canvistId canvas]
	$canvas configure -background [Value listBG]
	$canvas configure -yscrollcommand "$win.frameList.yscroll set"
	scrollbar $win.frameList.yscroll \
		-command "$canvas yview" -orient vertical

	bind $canvas <Configure> \
		"NSHighScore::Configure $oop $canvas"

	NSCanvist::Info $canvistId selectionCmd \
		"NSHighScore::SelectionChanged $oop"

	NSCanvist::Info $canvistId invokeCmd \
		"NSHighScore::Invoke $oop"

	Info $oop canvas $canvas
	Info $oop canvistId $canvistId

	#
	# Statusbar
	#

	MakeStatusBar $win.statusBar 20

	#
	# Geometry
	#

	grid rowconfigure $win.frameList 0 -weight 1 -minsize 0
	grid columnconfig $win.frameList 0 -weight 1 -minsize 0
	grid columnconfig $win.frameList 1 -weight 0 -minsize 0

	grid $canvas \
		-row 0 -column 0 -rowspan 1 -columnspan 1 -sticky news
	grid $win.frameList.yscroll \
		-row 0 -column 1 -rowspan 1 -columnspan 1 -sticky ns

	grid rowconfigure $win 0 -weight 0
	grid rowconfigure $win 1 -weight 1
	grid rowconfigure $win 2 -weight 0
	grid columnconfig $win 0 -weight 1

	if {[Platform windows]} {
		grid $win.divider1 \
			-row 0 -column 0 -rowspan 1 -columnspan 1 -sticky ew -pady 2
	}
	grid $win.frameList \
		-row 1 -column 0 -rowspan 1 -columnspan 1 -sticky news
	grid $win.statusBar \
		-row 2 -column 0 -rowspan 1 -columnspan 1 -sticky ew

#	Term_KeyPress_Bind $win
#	Term_KeyPress_Bind $canvas

	#
	# ZAngbandTk may show the Highscores Window
	# in the buildings, so we need to feed keys to the Term.
	#
	bind $win <KeyPress> {angband keypress %A}
	
	bind $win <KeyPress-Escape> "NSHighScore::Close $oop"
	bind $win <Control-KeyPress-w> "NSHighScore::Close $oop"
	bind $win <KeyPress-Delete> "NSHighScore::Delete $oop"

	set mbarId [Info $oop mbarId]
	bind $win <KeyPress-Return> "NSHighScore::MenuInvoke $oop $mbarId E_RECORD"
	bind $win <KeyPress-r> "NSHighScore::MenuInvoke $oop $mbarId E_RECORD"
	bind $win <KeyPress-f> "NSHighScore::MenuInvoke $oop $mbarId E_FIND"
	bind $win <KeyPress-g> "NSHighScore::MenuInvoke $oop $mbarId E_FIND_AGAIN"

	# Synch the scrollbars when window is shown.
	NSUtils::SynchScrollBar $canvas $win.frameList.yscroll

	return
}

# NSHighScore::InitMenus --
#
#	Create the menus associated with the window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::InitMenus {oop} {

	variable MenuString

	set win [Info $oop win]
	set mod "Ctrl"

	#
	# Menu bar
	#

	set mbarId [NSObject::New NSMenu $win -tearoff 0 \
		-postcommand "NSHighScore::SetupMenus $oop" -identifier MENUBAR]
	Info $oop mbarId $mbarId

	# Context-sensitive help
	NSMenu::Info $mbarId menuSelectCmd "NSHighScore::MenuSelect $oop"

	# Call our command when an entry is invoked
	NSMenu::Info $mbarId invokeCmd "NSHighScore::MenuInvoke $oop"

	#
	# Message Menu
	#

	NSObject::New NSMenu $mbarId -tearoff 0 -identifier MENU_SCORE
	NSMenu::MenuInsertEntry $mbarId -end MENUBAR -type cascade \
		-menu MENU_SCORE -label [mc Score] -underline 0 \
		-identifier M_SCORE

	set entries {}
	lappend entries [list -type command -label [mc "Dump Scores"] \
		-underline 0 -identifier E_DUMP]
	lappend entries [list -type command -label [mc "Show Record"] \
		-underline 5 -accelerator r -identifier E_RECORD]
	lappend entries [list -type separator]
	lappend entries [list -type command -label [mc Find...] \
		-accelerator f -underline 0 -identifier E_FIND]
	lappend entries [list -type command -label [mc "Find Again"] \
		-accelerator g -underline 6 -identifier E_FIND_AGAIN]
	lappend entries [list -type separator]
	lappend entries [list -type command -label [mc Close] \
		-underline 0 -accelerator $mod+W -identifier E_CLOSE]

	NSMenu::MenuInsertEntries $mbarId -end MENU_SCORE $entries

	#
	# Sort Menu
	#

	set menuId [NSObject::New NSMenu $mbarId -tearoff 0 -identifier MENU_SORT]
	NSMenu::MenuInsertEntry $mbarId -end MENUBAR -type cascade \
		-menu MENU_SORT -label [mc Sort] -underline 1 \
		-identifier M_SORT
	NSMenu::Info $menuId setupMode normal
	
	set entries {}
	foreach label [list Class Name Race Score Winner] sortBy [list class who race pts _winner] {
		lappend entries [list -type radiobutton -label [mc $label] \
			-variable NSHighScore($oop,sortBy) -value $sortBy \
			-identifier E_SORT_$sortBy]
	}
	NSMenu::MenuInsertEntries $mbarId -end MENU_SORT $entries

	set MenuString(M_SCORE) \
		"Contains commands for displaying and searching scores."
	set MenuString(E_DUMP) \
		"Writes the scores to a text file."
	set MenuString(E_RECORD) \
		"Displays the record for the selected score."
	set MenuString(E_FIND) \
		"Searches for a score containing a given string."
	set MenuString(E_FIND_AGAIN) \
		"Repeats the previous search."
	set MenuString(E_CLOSE) \
		"Closes the window."

	set MenuString(M_SORT) \
		"Contains commands for sorting scores."
	set MenuString(E_SORT_class) \
		"Sorts the scores by character class."
	set MenuString(E_SORT_who) \
		"Sorts the scores by character name."
	set MenuString(E_SORT_race) \
		"Sorts the scores by character race."
	set MenuString(E_SORT_pts) \
		"Sorts the scores by total points."
	set MenuString(E_SORT__winner) \
		"Sorts the scores by winner status."

	return
}

# NSHighScore::SetupMenus --
#
#	Prepare to post the menus.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::SetupMenus {oop mbarId} {

	variable highscore_record

	lappend identList E_DUMP E_FIND E_FIND_AGAIN E_CLOSE

	set row [Info $oop current]
	if {$row != -1} {
		if {[string length [lindex $highscore_record $row]]} {
			lappend identList E_RECORD
		}
	}

	NSMenu::MenuEnable $mbarId $identList

	[Info $oop win].statusBar cover show

	return
}

# NSHighScore::MenuSelect --
#
#	Displays a help string associated with a menu entry.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::MenuSelect {oop menuId index ident} {

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

# NSHighScore::MenuInvoke --
#
#	Called when a menu entry is invoked.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::MenuInvoke {oop menuId ident} {

	set win [Info $oop win]
	
	switch -glob -- $ident {
		E_DUMP {Dump $oop $win}
		E_RECORD {InvokeRow $oop [Info $oop current]}
		E_FIND {Find $oop 0}
		E_FIND_AGAIN {Find $oop 1}
		E_CLOSE {Close $oop}
		E_SORT_* {
			scan $ident E_SORT_%s sortBy
			Info $oop sortBy $sortBy
			SetList $oop
		}
	}

	return
}

# NSHighScore::DisplayCmd --
#
#	Called by NSWindowManager::Display().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::DisplayCmd {oop message first} {

	global Angband

	switch -- $message {
		preDisplay {
			if {![Info $oop interactive]} {
				wm title [Info $oop win] ["ZAngband Hall of Fame"]
			}
			SetList $oop
			focus [Info $oop canvas]
		}
		postDisplay {
		}
		reDisplay {
			SetList $oop
		}
	}

	return
}

# NSHighScore::Close --
#
#	Do something when closing the window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::Close {oop} {

	# If the character is dead, then we call "tkwait window"
	if {[angband player is_dead]} {
		destroy [Info $oop win]
		return
	}

	if {![Info $oop interactive]} {
		NSWindowManager::Undisplay highscore
		Info $oop interactive 1
		return
	}

	angband keypress \033

	return
}

# NSHighScore::StatusBar --
#
#	Display text in the status bar, perhaps clearing it later.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::StatusBar {oop text zap} {

	set win [Info $oop win]
	set label [$win.statusBar itemcget t1 -label]
	$label configure -text $text
	if {$zap} {
		NSUtils::ZapLabel $label
	}

	return
}

# NSHighScore::NewItemCmd --
#
#	Called by NSCanvist::InsertItem() to create a list row.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::NewItemCmd {oop canvistId y index} {

	variable highscore
	variable highscore_index
	variable highscore_record

	set c [NSCanvist::Info $canvistId canvas]
	set lineHeight [NSCanvist::Info $canvistId rowHgt]

	set font [Global font,fixed,normal]
	set fh [expr {[font metrics $font -linespace] * 3}]
	set diff [expr {int([expr {($lineHeight - $fh) / 2}])}]

	set row [expr {$index - 1}]
	
	set text [FormatScore $oop $index]

	# Get the width of the canvas
	set canvasWidth [winfo width $c]

	# Selection rectangle inside row
	lappend itemIdList [$c create rectangle 2 [expr {$y + 2}] \
		[expr {($canvasWidth - 1) - 2}] [expr {$y + $lineHeight - 2}] \
		-fill "" -outline "" -tags selrect -width 2.0]

	# Text
	set fill White
	if {$index == [expr {$highscore_index + 1}]} {
		set fill [Value TERM_L_BLUE]
	}
	lappend itemIdList [$c create text 0 [expr {$y + $diff}] \
		-text $text -anchor nw -font $font -fill $fill \
		-tags text]

	# This score has a record
	if {[string length [lindex $highscore_record $row]]} {
		set itemId [$c create text 500 [expr {$y + $diff}] \
			-text "More..." -anchor nw -font $font -fill White]
		$c bind $itemId <ButtonRelease-1> \
			"NSHighScore::InvokeRow $oop $row"
		lappend itemIdList $itemId
	}

	return $itemIdList
}

# NSHighScore::HighlightItemCmd --
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

proc NSHighScore::HighlightItemCmd {oop canvistId state args} {

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

# NSHighScore::Configure --
#
#	Called as a <Configure> event script. Resizes the selection rectangles
#	so they fit properly.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::Configure {oop canvas} {

	# Get the width of the canvas
	set canvasWidth [winfo width $canvas]

	foreach itemId [$canvas find withtag selrect] {
		set coords [$canvas coords $itemId]
		set right [expr {($canvasWidth - 1) - 2}]
		set coords [lreplace $coords 2 2 $right]
		eval $canvas coords $itemId $coords
	}

	return
}

# NSHighScore::SetList --
#
#	Fill the list with scores.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::SetList {oop} {

	variable highscore
	variable highscore_index
	variable highscore_list

	set win [Info $oop win]
	set canvistId [Info $oop canvistId]

	# Clear the list
	NSCanvist::DeleteAll $canvistId

	# List of scores
	GetTheScores $oop

	# Keep a list of score-file indexes for each row
	set match {}

	set index 0
	foreach score $highscore_list {

		array set highscore $score

		# Append the high score, getting info from highscore[] variable
		NSCanvist::Insert $canvistId end [incr index]

		# Keep a list of score-file indexes for each row
		lappend match $highscore(_index)
	}

	# Keep a list of score-file indexes for each row
	Info $oop match $match

	# If the current character has a score, highlight and view it
	if {$highscore_index != -1} {
		NSCanvist::UpdateSelection $canvistId $highscore_index all
		NSCanvist::See $canvistId $highscore_index
	}

	return
}

# NSHighScore::SelectionChanged --
#
#	Called when the list selection changes.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::SelectionChanged {oop canvistId select deselect} {

	variable highscore_record

	set win [Info $oop win]
	
	if {![llength $select]} {

		$win.statusBar itemconfigure t2 -text ""

		# No frame is selected now
		Info $oop current -1
		return
	}

	set row [lindex $select 0]
	Info $oop current $row

	set record [lindex $highscore_record $row]
	if {[string length $record]} {
		set string [format [mc "Record %s."] $record]
	} else {
		set string [mc "No record available."]
	}

	# Indicate the score index
	if {$::DEBUG} {
		set string "#[lindex [Info $oop match] $row] $string"
	}
	
	$win.statusBar itemconfigure t2 -text $string

	return
}

# NSHighScore::GetTheScores --
#
#	Return a list of high scores. Read the high score file if present.
#	Also create an entry for the character, if not dead yet. 
#
# Arguments:
#	oop					OOP ID. See above.
#
# Results:
#	What happened.

proc NSHighScore::GetTheScores {oop} {

	global Angband
	variable highscore
	variable highscore_index
	variable highscore_list
	variable highscore_record

	set highscore_index -1
	set highscore_list {}

	# Predict the character's score
	set predict [angband highscore predict -force highscore_predict]
	set highscore_predict(_index) -1
	if {[string equal $highscore_predict(how) "Ripe Old Age"]} {
		set highscore_predict(_winner) 1
	} else {
		set highscore_predict(_winner) 0
	}

	if {![Info $oop interactive]} {
		set predict 1
	}

	set nameList [lsort [array names highscore_predict]]

	# Build the filename
	set buf [PathTk lib apex scores.raw]

	# See if the file exists
	if {[file exists $buf]} {

		# Read the high scores
		foreach score [angband highscore find -tester yes] {
		
			angband highscore info $score highscore
			set highscore(_index) $score
			if {[string equal $highscore(how) "Ripe Old Age"]} {
				set highscore(_winner) 1
			} else {
				set highscore(_winner) 0
			}
			
			# Because the order of name/value pairs returned by
			# [array get] is undefined, and because we want to sort
			# the scores based on various criteria, we sort each
			# score by name.
			set elem {}
			foreach name $nameList {
				lappend elem $name $highscore($name)
			}
			lappend highscore_list $elem
		}
	}

	# Living character may get an entry
	if {$predict && ![angband player is_dead]} {
		set elem {}
		foreach name $nameList {
			lappend elem $name $highscore_predict($name)
		}
		lappend highscore_list $elem
	}

	# Sort the scores
	set sortBy [Info $oop sortBy]
	set index [lsearch -exact $nameList $sortBy]
	set index [expr {$index * 2 + 1}]
	if {[string equal $sortBy pts] || [string equal $sortBy _winner]} {
		set highscore_list [lsort -integer -decreasing -index $index $highscore_list]
	} else {
		set highscore_list [lsort -dictionary -index $index $highscore_list]
	}
	
	# Sort again by points if needed
	if {[string compare $sortBy pts]} {
		set index [lsearch -exact $nameList pts]
		set index [expr {$index * 2 + 1}]
		set scoreList {}
		set matchWhat ""
		set matchList {}
		foreach score $highscore_list {
			array set highscore $score
			if {$highscore($sortBy) == $matchWhat} {
				lappend matchList $score
				continue
			}
			if {[llength $matchList]} {
				set matchList [lsort -integer -decreasing -index $index $matchList]
				foreach score2 $matchList {
					lappend scoreList $score2
				}
			}
			set matchWhat $highscore($sortBy)
			set matchList [list $score]
		}
		if {[llength $matchList]} {
			set matchList [lsort -integer -decreasing -index $index $matchList]
			foreach score2 $matchList {
				lappend scoreList $score2
			}
		}
		if {[llength $scoreList] != [llength $highscore_list]} {
			tk_messageBox -title Error -message "[llength $scoreList] != [llength $highscore_list]"
		}
		set highscore_list $scoreList
	}
	
	# Look for the current character's score (living or dead)
	set index 0
	foreach score $highscore_list {
		array set highscore $score
		if {[angband player is_dead]} {
			if {[string equal $highscore(who) $highscore_predict(who)] &&
				($highscore(turns) == $highscore_predict(turns)) &&
				($highscore(cur_dun) == $highscore_predict(cur_dun)) &&
				($highscore(max_dun) == $highscore_predict(max_dun)) &&
				($highscore(cur_lev) == $highscore_predict(cur_lev)) &&
				($highscore(max_lev) == $highscore_predict(max_lev)) &&
				($highscore(p_c) == $highscore_predict(p_c)) &&
				($highscore(p_r) == $highscore_predict(p_r)) &&
				($highscore(pts) == $highscore_predict(pts))} {

				set highscore_index $index
				break
			}
		} else {
			if {$highscore(_index) == -1} {
				set highscore_index $index
				break
			}
		}
		incr index
	}

	# Get a list of character records associated with each score
	set highscore_record [NSRecord::MatchRecordsToScores $highscore_list]

	return
}

# NSHighScore::FormatScore --
#
#	Return a formatted string for the current high score. 
#
# Arguments:
#	oop					OOP ID. See above.
#
# Results:
#	What happened.

proc NSHighScore::FormatScore {oop index} {

	variable highscore

	append text [format "%3d.%9s  %s the %s %s, Level %d" \
		$index $highscore(pts) $highscore(who) $highscore(race) \
		$highscore(class) $highscore(cur_lev)]

	# Append a "maximum level"
	if {$highscore(max_lev) > $highscore(cur_lev)} {
		append text " (Max $highscore(max_lev))"
	}

	if {$highscore(cur_dun)} {
		append text [format "\n               Killed by %s on %s %d" \
			$highscore(how) "Dungeon Level" $highscore(cur_dun)]
	} else {
		append text [format "\n               Killed by %s in the Town" \
			$highscore(how)]
	}

	# Append a "maximum level"
	if {$highscore(max_dun) > $highscore(cur_dun)} {
		append text " (Max $highscore(max_dun))"
	}

	# Hack -- extract the gold and such
	set user [string trimleft $highscore(uid)]
	set when [string trimleft $highscore(day)]
	set gold [string trimleft $highscore(gold)]
	set turn [string trimleft $highscore(turns)]

	# And still another line of info
	append text \
		[format "\n               (User %s, Date %s, Gold %s, Turn %s)." \
		$user $when $gold $turn]

	return $text
}

# NSHighScore::Dump --
#
#	Dump a list of high scores to a text file. 
#
# Arguments:
#	oop					OOP ID. See above.
#
# Results:
#	What happened.

proc NSHighScore::Dump {oop parent} {

	global Angband
	variable highscore
	variable highscore_list

	set win [Info $oop win]

	set fileName [tk_getSaveFile -initialfile scores.txt \
		-initialdir [PathUser] -parent $parent]
	if {![string length $fileName]} return

	if {[catch {openlf $fileName} fileId]} {
		set msg "The following error occurred while attempting to open "
		append msg "the score-dump file for writing:\n\n$fileId"
		tk_messageBox -title Oops -message $msg
		return
	}

	puts $fileId "  \[ZAngband Hall of Fame\]\n"

	GetTheScores $oop

	set index 0
	foreach score $highscore_list {
		array set highscore $score
		puts $fileId [FormatScore $oop [incr index]]\n
	}

	close $fileId

	return
}

# NSHighScore::Find --
#
#	Search for a score containing a string.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::Find {oop again} {

	variable highscore
	variable highscore_list
	variable Priv
	
	set canvistId [Info $oop canvistId]

	# Repeat the last search
	if {$again && [string length $Priv(find,string)]} {

		set string $Priv(find,string)

	# Enter a string to find, start from the beginning
	} else {

		# Ask the user for a string
		set string [NSUtils::StringBox -title [mc Find] \
			-initial $Priv(find,string) -prompt [mc find-prompt] \
			-buttons [list [mc Find] [mc Cancel]] -parent [Info $oop win]]
		if {![string length $string]} return
		set Priv(find,string) $string
	}
	incr Priv(find,index)

	set string [string tolower $string]

	set row $Priv(find,index)
	foreach score [lrange $highscore_list $row end] {
		array set highscore $score
		set text [string tolower [FormatScore $oop [expr {$row + 1}]]]
		if {[string first $string $text] != -1} {
			NSCanvist::UpdateSelection $canvistId $row all
			NSCanvist::See $canvistId $row
			set Priv(find,index) $row
			return
		}
		incr row
	}

	set max [incr row -1]
	set row 0
	foreach score [lrange $highscore_list 0 $max] {
		array set highscore $score
		set text [string tolower [FormatScore $oop [expr {$row + 1}]]]
		if {[string first $string $text] != -1} {
			NSCanvist::UpdateSelection $canvistId $row all
			NSCanvist::See $canvistId $row
			set Priv(find,index) $row
			return
		}
		incr row
	}

	return
}

# NSHighScore::InvokeRow --
#
#	Called when a list item is double-clicked etc.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::InvokeRow {oop row} {

	variable highscore_record

	if {$row == -1} return

	set record [lindex $highscore_record $row]
	if {[string length $record]} {
		if {[NSModule::LoadIfNeeded NSRecord]} {
			NSRecord::LoadRecords
		}
		NSWindowManager::Display record $record
	}
	
	return
}

# NSHighScore::Invoke --
#
#	Called when a list item is double-clicked.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::Invoke {oop canvistId x y} {

	set row [NSCanvist::PointToRow $canvistId $x $y]
	if {$row == -1} return

	InvokeRow $oop $row
	
	return
}

# NSHighScore::DeleteRow --
#
#	Delete a score (and record).
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::DeleteRow {oop row} {

	global Angband
	variable highscore_record

	set win [Info $oop win]
	
	set index [lindex [Info $oop match] $row]
	if {$index == -1} return

	set answer [tk_messageBox -message "Really delete this score?" \
		-parent $win -title "Delete Score" -type yesno]
	if {[string equal $answer no]} return

	angband highscore delete $index
	
	set record [lindex $highscore_record $row]
	if {[string length $record]} {
		set answer [tk_messageBox -message "Delete $record?" \
			-parent $win  -title "Delete Record" -type yesno]
		if {[string equal $answer yes]} {
			set path [PathUser $record]
			file delete $path
		}
	}

	# Preserve the scroll position and selection
	set canvistId [Info $oop canvistId]
	set canvas [NSCanvist::Info $canvistId canvas]
	set rowHeight [$canvas cget -yscrollincrement]
	set rowTop [expr {int([$canvas canvasy 0 $rowHeight] / $rowHeight)}]
	set current [Info $oop current]

	SetList $oop

	# Restore the scroll position and selection
	$canvas yview scroll $rowTop units
	if {$current != -1} {
		set count [NSCanvist::Info $canvistId count]
		if {$current >= $count} {
			set current [expr {$count - 1}]
		}
		NSCanvist::UpdateSelection $canvistId $current all
	}
	
	return
}

# NSHighScore::Delete --
#
#	Delete the selected score (and any record, if desired).
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::Delete {oop} {

	set current [Info $oop current]
	if {$current == -1} return
	DeleteRow $oop $current

	return
}

# NSHighScore::ValueChanged_listBG --
#
#	Called when the listBG value changes.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSHighScore::ValueChanged_listBG {oop} {

	set color [Value listBG]

	[Info $oop canvas] configure -background $color

	return
}

