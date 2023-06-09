# File: errorInfo.tcl

# Purpose: errorInfo display for debugging

#
# Copyright (c) 1997-2001 Tim Baker
#
# This software may be copied and distributed for educational, research, and
# not for profit purposes provided that this copyright and statement are
# included in all such copies.
#

proc tracecmd {text name1 name2 op} {

	global traceId
	global errorInfo
	global traceInfo
	global traceText

	set errorString $errorInfo
	if {![winfo exists $text]} {
		trace vdelete errorInfo w "tracecmd $text"
		return
	}
	set length [string length $traceInfo]
	set string [string range $errorString 0 [expr {$length - 1}]]
	set newline 1
	if {[string equal $traceInfo $string]} {
		set errorString [string range $errorString $length end]
		set newline 0
	}
	if {[string equal $errorString $traceInfo]} return
	set traceInfo $errorString

	if {$newline} {
		append traceText "\n\n$errorString"
	} else {
		append traceText $errorString
	}
	if {![string length $traceId]} {
		set traceId [after idle traceflush $text]
	}

	return
}

proc traceflush {text} {

	global traceId
	global traceText

	$text insert end $traceText
	scan [$text index end] %d.%d line char
	if {$line > 1000} {
		$text delete 1.0 [expr {$line - 1000}].0
	}
	$text see "end linestart"
	
	set traceId ""
	set traceText ""

	return
}

proc tracewindow {} {

	set win .errors
	toplevel $win
	wm title $win errorInfo

	frame $win.textFrame \
		-relief sunken -borderwidth 1

	scrollbar $win.yscroll \
		-orient vertical -command [list $win.text yview] \
		-takefocus 1

	scrollbar $win.xscroll \
		-orient horizontal -command [list $win.text xview] \
		-takefocus 1

	if {[Platform unix]} {
		set font {Courier 12}
	}
	if {[Platform windows]} {
		set font {Courier 9}
	}

	text $win.text \
		-yscrollcommand [list $win.yscroll set] -wrap none \
		-xscrollcommand [list $win.xscroll set] \
		-width 82 -height 30 -font $font -borderwidth 0 \
		-setgrid no -highlightthickness 0 -padx 4 -pady 2 \
		-background Black -foreground White

	pack $win.textFrame \
		-expand yes -fill both


	grid rowconfig $win.textFrame 0 -weight 1 -minsize 0
	grid columnconfig $win.textFrame 0 -weight 1 -minsize 0

	grid $win.text -in $win.textFrame \
		-row 0 -column 0 -rowspan 1 -columnspan 1 -sticky news
	grid $win.yscroll -in $win.textFrame \
		-row 0 -column 1 -rowspan 1 -columnspan 1 -sticky news
	grid $win.xscroll -in $win.textFrame \
		-row 1 -column 0 -rowspan 1 -columnspan 1 -sticky news

	return
}

proc tracesetup {} {

	global traceId
	global errorInfo
	global traceInfo
	global traceText

	tracewindow

	set win .errors
	set text $win.text
	trace variable errorInfo w "tracecmd $text"
	set traceId ""
	set traceInfo ""
	set traceText ""

	$text insert end "# IGNORE errors about tclIndex. It is okay.\n"
	$text insert end "# (If it is not okay, then ignore this message. ;-\})\n\n"

	return
}

proc Debug {what} {

	if {![winfo exists .errors]} return

	if {![string equal [.errors.text get "end - 2 chars"] "\n"]} {
		.errors.text insert end \n
	}
	.errors.text insert end $what\n
	.errors.text see end

	return
}

if 0 {

set errorInfo {}

rename angband angband_old
proc angband {args} {
	Debug "angband $args"
	eval angband_old $args
}

}


proc InitCommandWindow {} {

	global CommandHistory

	set win .command

	toplevel $win
	wm title $win Command

	entry $win.entry -width 60

	bind $win.entry <Return> {
		lappend CommandHistory(line) [%W get]
		if {[incr CommandHistory(count)] > 20} {
			set CommandHistory(line) [lrange $CommandHistory(line) \
				[expr {$CommandHistory(count) - 20}] end]
			set CommandHistory(count) [llength $CommandHistory(line)]
		}
		set CommandHistory(index) $CommandHistory(count)
		catch {eval [%W get]} result
		%W delete 0 end
		%W insert 0 $result
	}

	bind $win.entry <KeyPress-Up> {
		if {$CommandHistory(index) > 0} {
			incr CommandHistory(index) -1
		}
		%W delete 0 end
		%W insert 0 [lindex $CommandHistory(line) $CommandHistory(index)]
	}

	bind $win.entry <KeyPress-Down> {
		if {$CommandHistory(index) < $CommandHistory(count) - 1} {
			incr CommandHistory(index)
		}
		%W delete 0 end
		%W insert 0 [lindex $CommandHistory(line) $CommandHistory(index)]
	}

	pack $win.entry -side top -expand yes -fill x

	set CommandHistory(line) {}
	set CommandHistory(count) 0
	set CommandHistory(index) 0

	focus $win.entry

	return
}

tracesetup
# InitCommandWindow

