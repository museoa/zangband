# File: init-startup.tcl

# Purpose: the Startup Window and related commands

#
# Copyright (c) 1997-2001 Tim Baker
#
# This software may be copied and distributed for educational, research, and
# not for profit purposes provided that this copyright and statement are
# included in all such copies.
#

#	This is the first file sourced during program initialization. It is 
#	sourced before init_angband() is called.

#
# Global --
#
#	Query and modify info.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc Global {info args} {

	global Global

	# Set info
	if {[llength $args]} {
		set Global($info) [lindex $args 0]

	# Get info
	} else {
		return $Global($info)
	}

	return
}

# Platform --
#
#	Return 1 if we are running on any of the given platforms.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc Platform {args} {

	global Angband

	if {![llength $args]} {
		return $Angband(platform)
	}

	foreach name $args {
		if {$::DEBUG} {
			set platformList [list macintosh unix windows]
			if {[lsearch -exact $platformList $name] == -1} {
				tk_messageBox -message "unknown platform \"$name\""
			}
		}
		if {[string equal $name $Angband(platform)]} {
			return 1
		}
	}
	return 0
}

# Window --
#
#	Global window info.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc Window {info args} {

	global Windows

	# Set info
	if {[llength $args]} {
		set Windows($info) [lindex $args 0]

	# Get info
	} else {
		return $Windows($info)
	}

	return
}

# Source --
#
#	Source a .tcl file in the tk directory or a subdirectory of it.
#	Post a warning if the file is not found.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc Source {args} {

	global Angband

	set path [eval file join [list $Angband(dirTK)] $args]
	if {![file exists $path]} {
		error "file not found:\n$path"
	}
	if {[Global tclCompiler]} {
		set path [TbcCompile $path]
	}
	uplevel #0 source $path

	return
}

# TbcCompile --
#
#	Use the Tcl compiler.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc TbcCompile {path} {

	# Put the output files into a subdirectory
	set dir [file join [file dirname $path] .tbc]
	set path2 [file join $dir [file tail $path].tbc]

	# The .tbc file exists
	if {[file exists $path2]} {

		set mtime [file mtime $path]
		set mtime2 [file mtime $path2]

		# If the .tbc file is newer, use it
		if {$mtime <= $mtime2} {

			# Return the location of the compiled file
			return $path2
		}
	}

	# Create the output directory if needed
	if {![file exists $dir]} {
		file mkdir $dir
	}

	# Compile the file
	::compiler::compile $path $path2

	# Return the location of the compiled file
	return $path2
}

# LongName --
#
#	Under Windows, return -longname of the given file.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc LongName {name} {

	if {[string equal $::tcl_platform(platform) windows]} {
		return [file attributes $name -longname]
	}
	return $name
}

# XXX Added this namespace just so I can call MsgCatInit
namespace eval NSInitStartup {
}

# AboutApplication --
#
#	Display program information.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc AboutApplication {} {

	NSModule::LoadIfNeeded NSAbout
	NSAbout::About

	return
}

# NewGame --
#
#	Create a new character.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NewGame {} {

	after idle angband game new

	return
}

# OpenGame --
#
#	Choose a savefile and play a game.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc OpenGame {} {

	# Hack -- Update the button
	update

	# Hack -- If the user types 'o' too fast, this command may be
	# called twice before the Choose Game Window appears. So disable
	# the "Open" button
	.opengame configure -state disabled
	
#	uplevel #0 Source choose-game.tcl
NSModule::RebootModule NSChooseGame
#	NSChooseGame::InitModule

	# Hack -- Enable the "Open" button again
	.opengame configure -state normal
	
	return
}

# QuitNoSave --
#
#	Quit the game without saving. If the game is not asking for
#	a command, then call "game abort". Otherwise do a clean exit.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc QuitNoSave {} {

	global Angband

	# Check if game is waiting for a command
	if {[string compare [angband inkey_flags] INKEY_CMD]} {
		angband game abort
		return
	}

	# Ask the user to confirm quit with save
	set answer [tk_messageBox -icon question -type yesno \
		-title "Quit $Angband(name)" -message "Do you really want to\
		quit without saving?"]
	if {[string equal $answer no]} return

	# Quit without saving
	DoUnderlyingCommand ^Ey

	return
}

# Setup --
#
#	Show the Setup Window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc Setup {} {

	NSModule::LoadIfNeeded NSSetup
	NSModule::CloseModule NSSetup

	return
}

# NSInitStartup::UpgradeTool --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSInitStartup::UpgradeTool {} {

	global Angband

	# FIXME: test on Unix
	if {![Platform windows]} return

	# See if tk\config\value exists. If it does, we assume
	# UpgradeTool does not need to be used.
	if {[file exists [PathTk config value]]} return
	
	set message [format [mc upgrade-tool-prompt] $Angband(name)]
	set title [mc upgrade-tool-title]

	# Ask the user if he wants to run UpgradeTool
	set answer [tk_messageBox -message $message -icon question \
		-title $title -type yesno]
	if {[string equal $answer no]} return

	# Hack -- Create a slave interpreter, load Tk into it,
	# set argc/argv so it will use the game directory, and
	# override the 'exit' command to tell us when it is
	# finished.

	set i [interp create]
	load {} Tk $i
	$i eval [list set ::argc 2]
	$i eval [list set ::argv [list -dstdir [Path]]]
	$i alias exit set ::UpgradeToolExit 1
	$i eval source [Pathtk lib UpgradeTool main.tcl]

	# Wait for UpgradeTool to exit
	set ::UpgradeToolExit 0
	tkwait variable ::UpgradeToolExit

	interp delete $i

	# If UpgradeTool was used, then perhaps read in tk\config\value
	if {[file exists [PathTk config value]]} {
		NSValueManager::LoadValueFile
	}

	return
}

# angband_save --
#
#	Called by Angband whenever the game is saved.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc angband_save {savefile} {

	Value savefile [file split [LongName $savefile]]

	return
}

# NSInitStartup::InitStartupScreen --
#
#	Initialize the startup window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSInitStartup::InitStartupScreen {} {

	global Angband

	wm title . "$Angband(name) $Angband(vers)"
	wm resizable . no no

	# Quit application if the user closes the window
	wm protocol . WM_DELETE_WINDOW "exit"

	# Load the "Tcl Powered Logo"
	image create photo Image_PwrdLogo175 \
		-file [PathTk image pwrdLogo175.gif]

	# Program name
	if {[Platform unix]} {
		set font {Times 24 bold}
	}
	if {[Platform windows]} {
		set font {Times 18 bold}
	}
	label .title \
		-text "$Angband(name) $Angband(vers)" -font $font

	# Program info
	set info [format [mc copyright] $Angband(name)]\n
#	append info "$Angband(copy)\n"
append info "http://www.zangband.org\n"
	append info [mc email]
	label .info \
		-text $info -justify left

	# Tcl Powered Logo
	label .logo \
		-image Image_PwrdLogo175

	# Startup progress
	listbox .prompt \
		-width 15 -height 10 -borderwidth 0 -highlightthickness 0
	.prompt insert end "Initializing arrays..."

	# Geometry
	pack .title \
		-side top -expand no -anchor w
	pack .info \
		-side top -expand no -anchor w
	pack .logo \
		-side left -expand no
	pack [frame .filler -borderwidth 0 -height 10] -side top -anchor w
	pack .prompt \
		-side top -expand no -pady 00 -padx 20 -anchor w

	# Position
	WindowPosition . 2 3

	# When the listbox is unpacked, the window may shrink horizontally.
	# So set the desired window geometry to what it is now (with the list).
	wm geometry . [wm geometry .]

	update

	return
}

# NSInitStartup::FinalizeStartupWindow --
#
#	Fiddle with the startup window after Angband is initialized.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSInitStartup::FinalizeStartupWindow {} {

	# Remove the prompt, and add some buttons
	destroy .filler
	destroy .prompt

	# Buttons
	button .newgame \
		-text [mc New] -command NewGame -width 11 -underline 0
	button .opengame \
		-text [mc Open] -command OpenGame -width 11 -underline 0
	button .config \
		-text [mc Setup] -command Setup -width 11 -underline 0
	button .quit \
		-text [mc Quit] -command "angband game quit" -width 11 -underline 0

	switch -- [Platform] {
		windows {
			set padTop 20
			set fillHeight 10
		}
		unix {
			set padTop 8
			set fillHeight 2
		}
	}

	# Geometry
	pack [frame .filler1 -height $padTop -borderwidth 0] \
		-side top
	pack .newgame \
		-side top -expand no -pady 0 -padx 20
	pack [frame .filler2 -height $fillHeight -borderwidth 0] \
		-side top
	pack .opengame \
		-side top -expand no -pady 0 -padx 20
	pack [frame .filler3 -height $fillHeight -borderwidth 0] \
		-side top
	pack .config \
		-side top -expand no -pady 0 -padx 20
	pack [frame .filler4 -height $fillHeight -borderwidth 0] \
		-side top
	pack .quit \
		-side top -expand no -pady 0 -padx 20

	NSUtils::SetDefaultButton . .opengame

	# KeyPress bindings
	bind . <KeyPress-n> \
		"tkButtonInvoke .newgame"
	bind . <KeyPress-o> \
		"tkButtonInvoke .opengame"
	bind . <KeyPress-s> \
		"tkButtonInvoke .config"
	bind . <KeyPress-q> \
		"tkButtonInvoke .quit"
	bind . <KeyPress-Return> \
		"NSUtils::InvokeDefaultButton ."
	bind . <KeyPress-Escape> \
		"tkButtonInvoke .quit"

	# Focus on Open Button
	focus .opengame

	return
}

# angband_startup --
#
#	Called by Angband (and below) to display status messages
#	during program initialization.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc angband_startup {what} {

	switch -- $what {

		init_misc {
			set prompt "    misc"
		}
		init_script {
			set prompt "    script"
		}
		init_z_info {
			set prompt "    array sizes"
		}
		init_f_info {
			set prompt "    features"
		}
		init_k_info {
			set prompt "    objects"
		}
		init_a_info {
			set prompt "    artifacts"
		}
		init_e_info {
			set prompt "    ego-items"
		}
		init_r_info {
			set prompt "    monsters"
		}
		init_h_info {
			set prompt "    histories"
		}
		init_p_info {
			set prompt "    races"
		}
		init_b_info {
			set prompt "    owners"
		}
		init_g_info {
			set prompt "    prices"
		}
		init_wilderness {
			set prompt "    wilderness"
		}
		init_towns {
			set prompt "    towns"
		}
		init_quests {
			set prompt "    quests"
		}
		init_plots {
			set prompt "    plots"
		}
		init_v_info {
			set prompt "    vaults"
		}
		init_other {
			set prompt "    other"
		}
		init_alloc {
			set prompt "    alloc"
		}

		default {
			set prompt $what
		}
	}

	.prompt insert end $prompt
	.prompt see end

	update

	return
}

# angband_initialized --
#
#	Called by Angband when program initialization is complete.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc angband_initialized {} {

	.prompt delete 0 end
	.prompt insert end "Sourcing scripts..."

	# Source library files
	angband_startup "    object.tcl"
	Source library object.tcl
	angband_startup "    buttonlabel.tcl"
	Source library buttonlabel.tcl
	angband_startup "    canvist.tcl"
	Source library canvist.tcl
	angband_startup "    canvist2.tcl"
	Source library canvist2.tcl
	angband_startup "    menu.tcl"
	Source library menu.tcl
	angband_startup "    module.tcl"
	Source library module.tcl
	angband_startup "    status-text.tcl"
	Source library status-text.tcl
	angband_startup "    progress.tcl"
	Source library progress1.tcl
	Source library progress2.tcl
	Source library progress-window.tcl
	angband_startup "    tabs.tcl"
	Source library tabs.tcl
	angband_startup "    texist.tcl"
	Source library texist.tcl
	angband_startup "    toplevel.tcl"
	Source library toplevel.tcl
#	angband_startup "    utils.tcl"
#	Source library utils.tcl
	angband_startup "    window-manager.tcl"
	Source library window-manager.tcl

	NSModule::AddModule NSChooseGame [PathTk choose-game.tcl]
	NSModule::IndexLoad [PathTk library moduleIndex.tcl]

	NSInitStartup::FinalizeStartupWindow

	if {$::DEBUG} { NSValueManager::Verify }

	Source config.tcl
	NSConfig::InitModule

	NSInitStartup::UpgradeTool

	NSModule::AddModule NSSetup [PathTk setup.tcl]

	# Now process other command-line options
	HandleArgv

	return
}

# HandleArgv --
#
#	Process command-line arguments.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc HandleArgv {} {

	global argc
	global argv

	# Assume no savefile
	set savefile ""

	# Show Setup Window if never done
	set doSetup [expr {![Value warning,setup]}]

	for {set i 0} {$i < $argc} {incr i} {
		set arg [lindex $argv $i]
		switch -- $arg {
			-icon-prefix {
				set prefix [lindex $argv [incr i]]
				set exists 0
				foreach {prefix1 desc} $NSConfig::Priv(config) {
					if {[string equal $prefix $prefix1]} {
						set exists 1
					}
				}
				if {$exists} {
					NSConfig::SetPrefix $prefix
				}
			}
			-setup {
				set boolean [lindex $argv [incr i]]
				if {[string is boolean -strict $boolean]} {
					set doSetup [string is true $boolean]
				}
			}
			-savefile {
				set savefile [lindex $argv [incr i]]
			}
			default {
			}
		}
	}

	# Do this before opening the savefile
	if {$doSetup} {
		update
		tkButtonInvoke .config
		Value warning,setup 1
	}

	if {[string length $savefile]} {
		if {[file exists $savefile]} {
			# Note: Want this done in the global namespace, because
			# currently angtk_eval_file() uses the current namespace.
			# Note: This must be "after idle", otherwise you get the
			# "DeleteInterpProc called with active evals" error when
			# quitting. i.e., you can't call it directly
			after idle angband game open [list $savefile]
		}
	}

	return
}

# Because init-other.tcl isn't called before Angband starts calling
# "angband_xxx", I must set a dummy proc's here.

proc angband_display {args} {
}

proc angband_generate {args} {
	return 1
}

# angband_birth --
#
#	Called by Angband to kick off character creation.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc angband_birth {action} {

	global Angband

	if {[catch {

		switch -- $action {
	
			setup {
				NSModule::AddModule NSBirth [PathTk birth.tcl]
				NSModule::LoadIfNeeded NSBirth
			}
		}

	} result]} {
		HandleError $result
	}

	return
}

# InitLoadWindow --
#
#	Creates and displays the Load Window, which is used to display
#	progress during savefile loading and subsequent program initialization.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc InitLoadWindow {} {

	global Angband
	global AngbandPriv

	# Hack -- NSBirth may call "angband_load init" to prevent there
	# being no front windows, so we allow this to get called twice.
	# If there are no front windows, the application is swapped
	# into the background.
	if {[winfo exists .load]} return
	
	set win .load
	toplevel $win -borderwidth 4 -relief ridge
	wm overrideredirect $win yes

	# Busy
	$win configure -cursor watch

	set width 350
	set height 250
	set height2 40

	set fg Black
	set bg White
	
	set canvas $win.canvas
	canvas $canvas \
		-borderwidth 0 -highlightthickness 0 \
		-width $width -height $height -background $bg

	set progId [NSObject::New NSProgress2 $win.canvas 150 8]

###
	
	if {[Platform unix]} {
		set font {Times 24 bold}
		set font2 {Times 12}
	}
	if {[Platform windows]} {
		set font {Times 18 bold}
		set font2 {Times 10}
	}
	set anchor nw
	set x 11
	set y 11
	set lineSpace [font metrics $font -linespace]

	# Create a "shadow" for the text below
	$canvas create text $x $y -font $font -anchor $anchor -fill gray \
		-text "$Angband(name) $Angband(vers)"
	$canvas create text $x [expr {$y + $lineSpace}] -font $font2 \
		-text "Copyright (c) 1997-2001 Tim Baker" -anchor $anchor -fill gray

	# Draw text over the shadow created above
	incr x -1
	incr y -1
	$canvas create text $x $y -text "$Angband(name) $Angband(vers)" \
		-font $font -anchor $anchor -fill $fg
	$canvas create text $x [expr {$y + $lineSpace}] -font $font2 \
		-text "Copyright (c) 1997-2001 Tim Baker" -anchor $anchor -fill $fg \
		-tags copyright

	# Scrolling text
	scan [$canvas bbox copyright] "%s %s %s %s" left top right bottom
	set height3 [expr {$height - ($height2 + 8) - ($bottom + 8)}]
	$canvas create rectangle 0 [expr {$bottom + 8}] $width \
		[expr {$bottom + 8 + $height3}] -fill gray90 -outline {}

	# Rectangle at bottom
	$canvas create rectangle 0 [expr {$height - $height2}] $width $height \
		-fill $fg
	$canvas create text [expr {$width / 2}] [expr {$height - $height2 + 4}] \
		-text "" -fill $bg -anchor n -tags message

###

	set font [Global font,sys,normal]
	set rowHeight [font metrics $font -linespace]
	set rowCount [expr {($height3 - 6) / $rowHeight}]
	set AngbandPriv(load,rowCount) $rowCount
	set y [expr {$height - $height2 - 8 - ($height3 - ($rowHeight * $rowCount)) / 2}]
	for {set row 1} {$row <= $rowCount} {incr row} {
		$canvas create text [expr {$width / 2}] \
			[expr {$y - $row * $rowHeight}] \
			-fill Black -font $font -anchor n -tags row$row
	}

	pack $canvas -padx 1 -pady 1
	place [NSProgress2::Info $progId frame] -x [expr {$width / 2}] \
		-y [expr {$height - 8}] -anchor s

###

	# Position the window
	WindowPosition $win 2 3

	# This inocuous call insures an *active* front window exists
	focus $win

	# Hide the startup screen
	wm withdraw .

	# Cleanup the startup screen
	foreach window [winfo children .] {
		if {[string compare [winfo class $window] Toplevel]} {
			destroy $window
		}
	}
	image delete Image_PwrdLogo175
	foreach tag [bind .] {
		bind . $tag {}
	}

	set AngbandPriv(load,win) $win
	set AngbandPriv(load,prog) $progId
	set AngbandPriv(load,message) {}

	return
}

# LoadNote --
#
#	Inserts the given message to the head of the message queue, then
#	displays all the messages following the message in the Load Window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc LoadNote {message} {

	global AngbandPriv

	set win .load
	set canvas $win.canvas

	set AngbandPriv(load,message) \
		[linsert $AngbandPriv(load,message) 0 $message]
	set AngbandPriv(load,message) \
		[lrange $AngbandPriv(load,message) 0 $AngbandPriv(load,rowCount)]
	
	set row 1
	foreach message [lrange $AngbandPriv(load,message) 1 end] {
		$canvas itemconfigure row$row -text $message
		incr row
	}

	return
}

# angband_load --
#
#	Called by Angband to display info during savefile loading.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc angband_load {action {message ""}} {

	global AngbandPriv
	
	switch -- $action {
		init {
			InitLoadWindow
		}
		kill {
			set win $AngbandPriv(load,win)
			destroy $win
		}
		note {
			set canvas $AngbandPriv(load,win).canvas
			$canvas itemconfigure message -text $message
			LoadNote $message
		}
		progress {
			NSProgress2::SetDoneRatio $AngbandPriv(load,prog) $message
		}
		prompt {
			set canvas $AngbandPriv(load,win).canvas
			$canvas itemconfigure message -text $message
			LoadNote $message
		}
	}

	update

	return
}

# NSInitStartup::InitStartup --
#
#	The main initialization command.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSInitStartup::InitStartup {} {

	global auto_path
	global Angband
	global DEBUG
	
	# Angband's directory
	set Angband(dir) [angband game directory ANGBAND_DIR_ROOT]
	set Angband(dir) [LongName $Angband(dir)]

	# The lib/user directory (user-specific on Unix)
	set Angband(dir,user) [angband game directory ANGBAND_DIR_USER]
	set Angband(dir,user) [LongName $Angband(dir,user)]
	
	# The tk directory
	set Angband(dirTK) [angband game directory ANGBAND_DIR_TK]
	set Angband(dirTK) [LongName $Angband(dirTK)]

	# The msgs directory (message catalog)
	set Angband(dir,msgs) [file join $Angband(dirTK) msgs]
	
	# List of object tval's.
	set Angband(tval) [angband info tval]

	# Remember the platform
	switch -- $::tcl_platform(platform) {
		macintosh -
		unix -
		windows {
			set Angband(platform) $::tcl_platform(platform)
		}
		default {
			error "unknown platform \"$::tcl_platform(platform)\""
		}
	}

	# Tell Tcl where to find our packages. Make sure Tcl loads packages from
	# our CommonTk\lib directory before looking anywhere else, in case external
	# packages with the same name exist.
	set auto_path [concat [list [file join $Angband(dirTK) lib]] $auto_path]
	
	# This call makes sure Tcl reads in info about all available packages.
	# Without this call, the "package names" command returns an empty list.
	# Also, "package require Tk" doesn't scan external packages.
	catch {package require no-such-package}
	
	### Use "file attributes $path -longname" instead!
	# package require Cxrlwin

	# Work around a bug on Windows 95.
	if {[string equal [angband system osversion] "Windows 95"]} {
		load [file join $Angband(dirTK) lib \
			tk_chooseDirectory tk_chooseDirectory.dll]
	}

	# If the Tcl compiler (from TclPro) is available, use it
	Global tclCompiler [expr {[lsearch -exact [package names] compiler] != -1}]
Global tclCompiler 0
	if {[Global tclCompiler]} {
		package require compiler
	}

	if {$Angband(platform) == "windows"} {
		# Set the default window icon
		if {[string compare 8.3.3 [info patchlevel]] >= 0} {
			wm iconbitmap . -default [file join $Angband(dirTK) image angbandtk.ico]
		}
	}

	# Development debug support
	set path [file join $Angband(dirTK) errorInfo.tcl]
	if {[file exists $path]} {
	
		set DEBUG 1
	
#		debughook bindings no
#		debughook commands no
#		debughook widgets no
		
		Source errorInfo.tcl
	
		proc ::ASSERT {condition message} {
			if {![uplevel expr $condition]} {
				error $message
			}
			return
		}
	
	} else {
	
		set DEBUG 0
		proc ::Debug {string} {}
		proc ::ASSERT {condition message} {}
	}
	
	# Error handling
	Source bgerror.tcl

	# Internationalization
	Source msgcat.tcl
	
	# Internationalization
	MsgCatInit startup
			
	# Global program name
	set Angband(name) ZAngbandTk
	
	# Global version string
	set Angband(vers) [angband game version]r5
	
	# Global copyright blurb
	set Angband(copy) [mc original-z]
	
	# The Tk console!
	if {[llength [info commands console]]} {
		console hide
		console title "Console - $Angband(name)"
		bind all <Control-KeyPress-C> {console show}

		# Hack -- Change the font
		console eval ".console configure -font {Courier 9}"

		# Hack -- Show initial prompt (picky me)
		console eval tkConsolePrompt

		# Hack -- Add a "Clear" menu item
		console eval {.menubar.edit add command -label "Clear" -underline 4 \
			-command {.console delete 1.0 end ; tkConsolePrompt}}

		proc ::evalclip {} {
			uplevel #0 [selection get -selection CLIPBOARD]
		}
	}

	# Hack -- Require WindowPosition() command
	Source library utils.tcl
	
	# Various game-related commands
	Source misc.tcl
	
	# Create a universal "empty" image
	image create photo Image_Empty

	# Poor-Man's Font Management
	set screenWidth [winfo screenwidth .]
	if {[Platform windows]} {
		set fontSys [mc font,sys]
		set fontFixed [mc font,fixed]
		if {$screenWidth > 800} {
			Global font,sys,small "$fontSys 8"
			Global font,sys,normal "$fontSys 8"
			Global font,sys,large "$fontSys 10"
			Global font,fixed,small "$fontFixed 9"
			Global font,fixed,normal "$fontFixed 10"
		} elseif {$screenWidth > 640} {
			Global font,sys,small "$fontSys 8"
			Global font,sys,normal "$fontSys 8"
			Global font,sys,large "$fontSys 8"
			Global font,fixed,small "$fontFixed 8"
			Global font,fixed,normal "$fontFixed 9"
		} else {
			Global font,sys,small "$fontSys 8"
			Global font,sys,normal "$fontSys 8"
			Global font,sys,large "$fontSys 8"
			Global font,fixed,small "$fontFixed 8"
			Global font,fixed,normal "$fontFixed 8"
		}
	# Platform win
	}
	if {[Platform unix]} {
		Global font,sys,small {Helvetica 10}
		Global font,sys,normal {Helvetica 12}
		Global font,sys,large {Helvetica 12}
		Global font,fixed,small {Courier 10}
		Global font,fixed,normal {Courier 12}
	}
	
	# The MS Windows version of Tk supports platform-specific color names
	# all beginning with "System". Each of these colors is set here for
	# each platform.

	# Get these from library/utils.tcl
	Global SystemButtonFace $::SystemButtonFace
	Global SystemButtonHighlight $::SystemButtonHighlight
	Global SystemButtonShadow $::SystemButtonShadow
	Global SystemHighlight $::SystemHighlight
	Global SystemHighlightText $::SystemHighlightText
	Global System3dLight $::System3dLight
	Global System3dDarkShadow $::System3dDarkShadow
	Global SystemInfoBackground $::SystemInfoBackground

	if {[Platform unix]} {
		option add *Button.default normal widgetDefault
		option add *Button.font {Helvetica 12} widgetDefault
		option add *Checkbutton.font {Helvetica 12} widgetDefault
		option add *Dialog.msg.font {Helvetica 12}
		option add *Entry.background White widgetDefault
		option add *Label.font {Helvetica 12} widgetDefault
		option add *Listbox.font {Helvetica 12} widgetDefault
		option add *Menu.font {Helvetica 12} widgetDefault
		option add *Message.font {Helvetica 12} widgetDefault
		option add *Radiobutton.font {Helvetica 12} widgetDefault
		option add *Scale.font {Helvetica 12} widgetDefault
		option add *selectForeground [Global SystemHighlightText] 100
		option add *selectBackground [Global SystemHighlight] 
	}

	# If a new character is created, this is set to 1
	Global isNewGame 0

	# Value Manager (needed for Birth Options Window)
	Source value-manager.tcl
	NSValueManager::InitModule

	InitStartupScreen

	return
}

# Begin
NSInitStartup::InitStartup

if {$DEBUG} {
	raise .
	focus .
	if {[winfo exists .errors]} { wm iconify .errors }
	if {[winfo exists .command]} { wm iconify .command }
}

# init_askfor_display --
#
#	Creates the window that is used to ask the user for information
#	when importing old savefiles.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc init_askfor_display {} {

	global AngbandPriv
	
	set win .askfor
	toplevel $win
	wm transient $win $AngbandPriv(load,win)

	set label $win.prompt
	label $label \
		-font "{MS Sans Serif} 10 bold underline"

	set label $win.info
	message $label \
		-width 350

	set frame $win.content
	frame $frame \
		-borderwidth 0

	MakeDivider $win.divider1 x

	set frame $win.buttons
	frame $frame \
		-borderwidth 0
	button $frame.ok \
		-command "set AngbandPriv(askfor_quest,result) 1" -text "OK" \
		-underline 0 -width 9 -default active
	button $frame.cancel \
		-command "angband game abort" -text "Quit" -underline 0 \
		-width 9

	pack $frame.cancel \
		-side right -pady 5 -padx 5
	pack $frame.ok \
		-side right -pady 5 -padx 5

	grid rowconfigure $win 0 -weight 0
	grid rowconfigure $win 1 -weight 0
	grid rowconfigure $win 2 -weight 1
	grid rowconfigure $win 3 -weight 0
	grid rowconfigure $win 4 -weight 0
	grid columnconfigure $win 0 -weight 1

	grid $win.prompt \
		-row 0 -col 0 -rowspan 1 -columnspan 1 -sticky w -padx 5 -pady 5
	grid $win.info \
		-row 1 -col 0 -rowspan 1 -columnspan 1 -sticky w -padx 10 -pady 5
	grid $win.content \
		-row 2 -col 0 -rowspan 1 -columnspan 1 -sticky news -padx 10 -pady 5
	grid $win.divider1 \
		-row 3 -col 0 -rowspan 1 -columnspan 1 -sticky ew -padx 10
	grid $win.buttons \
		-row 4 -col 0 -rowspan 1 -columnspan 1 -sticky ew -padx 5 -pady 5

	NSUtils::SetDefaultButton $win $win.buttons.ok

	bind $win <KeyPress-Return> \
		"NSUtils::InvokeDefaultButton $win"

	return $win
}

proc DisableImage {source dest} {

	set iw [image width $source]
	set ih [image height $source]

	set dot [list [list white]]
	for {set y 0 ; set y2 1} {$y < $ih} {incr y ; incr y2} {
		for {set x 0 ; set x2 1} {$x < $iw} {incr x ; incr x2} {
			if {[scan [photoget $source $x $y] "%d %d %d" r g b] == 3} {
				$dest put $dot -to $x2 $y2
			}
		}
	}

	if {[Platform unix]} {
		set color [Global SystemButtonShadow]
	}
	if {[Platform windows]} {
		scan [angband system color SystemButtonShadow] \
			"%d %d %d" red green blue
		set color [format #%02x%02x%02x $red $green $blue]
	}
	set dot [list [list $color]]
	for {set y 0} {$y < $ih} {incr y} {
		for {set x 0} {$x < $iw} {incr x} {
			if {[scan [photoget $source $x $y] "%d %d %d" r g b] == 3} {
				$dest put $dot -to $x $y
			}
		}
	}
	return
}

proc Win98MenuCmd {button} {

	set canvas [$button info canvas]
	set x [winfo rootx $canvas]
	set y [expr {[winfo rooty $canvas] + [winfo height $canvas]}]
	set menu [$button info canvas].menu
	tk_popup $menu $x $y
	after idle $button hidemenu
	return
}

proc Win98Config {} {

	global Win98

	if {!$Win98(showLabel)} {
		set wid 22
		set hgt 22
	} else {
		set wid 16
		set hgt 16
	}
set hgt 16
set wid 16
	foreach num {1 2 3} {
		[Global win98,button$num] configure \
			-showimage $Win98(showImage) \
			-showlabel $Win98(showLabel) \
			-widthimage $wid -heightimage $hgt
	}
}

