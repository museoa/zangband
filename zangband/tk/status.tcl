# File: status.tcl

# Purpose: status messages in the Main Window Widget

#
# Copyright (c) 1997-2001 Tim Baker
#
# This software may be copied and distributed for educational, research, and
# not for profit purposes provided that this copyright and statement are
# included in all such copies.
#

namespace eval NSStatus {

	variable Priv
	variable Trans

# namespace eval NSStatus
}

# NSStatus::InitModule --
#
#	One-time-only-ever initialization.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSStatus::InitModule {} {

	variable Priv
	variable Trans

	# Get the Main Window widget
	set widget [Global main,widget]

	# Get the desired font
	set font [Value font,status]

	# Calculate the height of the text
	set fontHeight [font metrics $font -linespace]

	# Calculate the height of a status item (includes bevel)
	set statusHeight [expr {$fontHeight + 2}]
	
	# Remember a list of status keywords
	set Priv(names) [list state winner cut stun hunger blind confused \
		afraid poisoned speed study]

	# Calculate the minimum required width of a status item
	set maxWidth 0
	foreach string $Priv(names) {
		set width [font measure $font $string]
		if {$width > $maxWidth} {
			set maxWidth $width
		}
	}

	# Calculate the width of a status item (includes bevel)
	set statusWid [expr {$maxWidth + 2}]

	# Remember the number of status items per row
	set Priv(numPerRow) [expr {[winfo reqwidth $widget] / $statusWid}]
	
	#
	# Assign "color" to each message
	#

	foreach status [list cut stun hunger blind confused afraid poisoned slow image] {
		set Priv($status,type) bad
	}
	foreach status [list state winner speed study recall] {
		set Priv($status,type) info
	}

	# Sanity check
	foreach status $Priv(names) {
		if {![info exists Priv($status,type)]} {
			error "NSStatus::InitModule: Priv($status,type) not specified"
		}
	}

	# Check each status keyword
	foreach status $Priv(names) {

		# Remember the item id
		set Priv($status,itemId) 0

		# Default to not showing
		set Priv($status,showing) 0

		# Remember current text
		set Priv($status,text) ""
	}

	# Remember the height of a status item
	set Priv(height) $statusHeight

	# Remember the width of a status item
	set Priv(width) $statusWid

	set data {
		Text -fill
		BG -background
		BL -bevellight
		BD -beveldark
		Text2 -fill2
		BG2 -background2
		BL2 -bevellight2
		BD2 -beveldark2
	}

	# Check each color/option
	foreach {name option} $data {

		# Update ourself when a color changes
		NSValueManager::AddClient statusGood$name \
			"NSStatus::ColorChanged good statusGood$name $option"
		NSValueManager::AddClient statusInfo$name \
			"NSStatus::ColorChanged info statusInfo$name $option"
		NSValueManager::AddClient statusBad$name \
			"NSStatus::ColorChanged bad statusBad$name $option"
	}

	# Update ourself when the font,status value changes
	NSValueManager::AddClient font,status \
		"NSStatus::ValueChanged_font_status"

	# Hack -- Big status message
	InitStatusMessage

	return
}

# NSStatus::CreateItem --
#
#	Create a status item.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSStatus::CreateItem {status} {

	variable Priv

	# Get the Main Window widget
	set widget [Global main,widget]

	# Get the desired font
	set font [Value font,status]

	# Create a new text item, with bevel
	set itemId [$widget create text -width $Priv(width) \
		-height $Priv(height) -bevel yes -visible no -anchor nw \
		-justify center -font $font]

	# Remember the item id
	set Priv($status,itemId) $itemId

	SetColor $status

	return
}

# NSStatus::SetColor --
#
#	Sets the color scheme for a message.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSStatus::SetColor {status {type ""}} {

	variable Priv

	if {[string length $type]} {
		if {[string equal $Priv($status,type) $type]} return
		set Priv($status,type) $type
	}
	
	# bad --> Bad
	set name status[string totitle $Priv($status,type)]

	set widget [Global main,widget]
	$widget itemconfigure $Priv($status,itemId) \
		-fill [Value ${name}Text] -fill2 [Value ${name}Text2] \
		-background [Value ${name}BG] -background2 [Value ${name}BG2] \
		-bevellight [Value ${name}BL] -bevellight2 [Value ${name}BL2] \
		-beveldark [Value ${name}BD] -beveldark2 [Value ${name}BD2]

	return
}

# NSStatus::Arrange --
#
#	Arranges all the status items.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSStatus::Arrange {resize} {

	variable Priv

	set widget [Global main,widget]

	set count 0
	set row 0
	foreach status $Priv(names) {
		if {$Priv($status,showing)} {
			if {$count} {
				$widget itemconfigure $Priv($status,itemId) \
					-x [expr {$count * $Priv(width)}] \
					-y [expr {$row * $Priv(height)}]
			} else {
				$widget itemconfigure $Priv($status,itemId) \
					-x 0 -y [expr {$row * $Priv(height)}]
			}
			if {[incr count] >= $Priv(numPerRow)} {
				set count 0
				incr row
			}
		}
	}

	# If this was called because the widget resized, then position the
	# big status message.
	if {$resize} {
		set itemId $Priv(statusMessage,itemId)
		set x [expr {[winfo reqwidth $widget] / 2}]
		set y 32
		$widget itemconfigure $itemId -x $x -y $y
	}

	return
}

# NSStatus::Configure --
#
#	Called when the main widget resizes.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSStatus::Configure {} {

	variable Priv

	set widget [Global main,widget]

	set statusWid $Priv(width)
	set widgetWid [winfo reqwidth $widget]
	set Priv(numPerRow) [expr {$widgetWid / $statusWid}]

	foreach status $Priv(names) {
		if {!$Priv($status,itemId)} continue
		$widget itemconfigure $Priv($status,itemId) -x 0 -y 0
	}

	Arrange 1

	return
}

# NSStatus::ColorChanged --
#
#	Called when a color (value) changes.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSStatus::ColorChanged {type valueName option} {

	variable Priv

	set widget [Global main,widget]
	set color [Value $valueName]

	foreach status $Priv(names) {
		if {!$Priv($status,itemId)} continue
		if {[string compare $Priv($status,type) $type]} continue
		$widget itemconfigure $Priv($status,itemId) $option $color
	}

	return
}

# NSStatus::InitStatusMessage --
#
#	Initialize the big status message. This is a text item that
#	appears briefly whenever a status message is set.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSStatus::InitStatusMessage {} {

	variable Color
	variable Priv
	variable Type

	set widget [Global main,widget]

	if {[Platform unix]} {
		set font {Times 18 bold}
	}
	if {[Platform windows]} {
		set font {Times 14 bold}
	}
	set x [expr {[winfo reqwidth $widget] / 2}]
	set y 32

	# Hack -- Calculate the maximum dimensions of a status message
	set maxWidth 0

	foreach string $Priv(names) {
		set width [font measure $font $string]
		if {$width > $maxWidth} {
			set maxWidth $width
		}
	}
	set width [expr {$maxWidth + 16}]

	set height [font metrics $font -linespace]
	
	set itemId [$widget create text -width $width -height $height \
		-bevel no -visible no -fill [palette set 35] -fill2 255 \
		-anchor center -background {} -background2 0 \
		-font $font -x $x -y $y -justify center]

	set Priv(statusMessage,itemId) $itemId
	set Priv(statusMessage,afterId) ""
	set Priv(statusMessage,queue) {}
	set Priv(statusMessage,type) {}
	set Priv(statusMessage,visible) 0

	# Messages are in one of 3 types: good, info, or bad. A
	# different color is used for each type.
	set Color(good) [palette set 227]
	set Color(info) [palette set 192]
	set Color(bad) [palette set 35]

	return
}

# NSStatus::SetStatusMessage --
#
#	Set the text of the big status message. If the status message is
#	already showing, then just queue the text to be displayed later.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSStatus::SetStatusMessage {text format type} {

	variable Color
	variable Priv

	# XXX Mega-hack -- Ignore "Rest" and "Repeat" messages
	if {[string first Rest $format] != -1} return
	if {[string first Rep $format] != -1} return

	# Don't re-queue this message
	if {[lsearch -exact $Priv(statusMessage,queue) $text] != -1} return

	lappend Priv(statusMessage,queue) $text
	lappend Priv(statusMessage,type) $type

	if {$Priv(statusMessage,visible)} return

	set widget [Global main,widget]
	set itemId $Priv(statusMessage,itemId)

	after cancel $Priv(statusMessage,afterId)
	$widget itemconfigure $itemId \
		-visible yes -text $text -fill $Color($type)
	set Priv(statusMessage,afterId) [after 1000 NSStatus::ClearStatusMessage]

	set Priv(statusMessage,visible) 1

	return
}

# NSStatus::ClearStatusMessage --
#
#	Called as an "after" script. If the big-status-message queue is empty,
#	then hide the big status message. Otherwise display the next element
#	in the queue.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSStatus::ClearStatusMessage {} {

	variable Color
	variable Priv

	set widget [Global main,widget]
	set itemId $Priv(statusMessage,itemId)

	set Priv(statusMessage,queue) [lrange $Priv(statusMessage,queue) 1 end]
	set Priv(statusMessage,type) [lrange $Priv(statusMessage,type) 1 end]

	# There is another message in the queue
	if {[llength $Priv(statusMessage,queue)]} {
		$widget itemconfigure $itemId \
			-text [lindex $Priv(statusMessage,queue) 0] \
			-fill $Color([lindex $Priv(statusMessage,type) 0])
		set Priv(statusMessage,afterId) \
			[after 1000 NSStatus::ClearStatusMessage]

	# There are no more messages
	} else {
		$widget itemconfigure $itemId -visible no
		set Priv(statusMessage,visible) 0
	}

	return
}

# NSStatus::ValueChanged_font_status --
#
#	Called when the font,status value changes.
#	Updates each status message.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSStatus::ValueChanged_font_status {} {

	variable Priv

	# Get the Main Window widget
	set widget [Global main,widget]

	# Get the desired font
	set font [Value font,status]

	# Calculate the height of a row
	set fontHeight [font metrics $font -linespace]

	# Calculate the height of each status item
	set statusHeight [expr {$fontHeight + 2}]

	# Calculate the minimum required width of a status item
	set maxWidth 0
	foreach string $Priv(names) {
		set width [font measure $font $string]
		if {$width > $maxWidth} {
			set maxWidth $width
		}
	}

	# Calculate the width of each status item
	set statusWid [expr {$maxWidth + 2}]

	# Check each status item
	foreach status $Priv(names) {

		if {!$Priv($status,itemId)} continue

		# Move each item to avoid overflow
		$widget itemconfigure $Priv($status,itemId) -x 0 -y 0

		# Resize each item, and set the font
		$widget itemconfigure $Priv($status,itemId) -width $statusWid \
			-height $statusHeight -font $font
	}

	# Remember the height and width of each item
	set Priv(height) $statusHeight
	set Priv(width) $statusWid

	# Get the width of the Main Window widget in pixels
	set widgetWid [winfo reqwidth $widget]

	# Calculate the number of status items per row
	set Priv(numPerRow) [expr {$widgetWid / $statusWid}]

	# Position each status item
	Arrange 0

	return
}

