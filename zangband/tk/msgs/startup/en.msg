set loc en

::msgcat::mcset $loc copyright "%s: Copyright (c) 2001 Tim Baker"

::msgcat::mcset $loc original-a "Based on Angband: Copyright (c) 2000 R. Ruehlmann"
::msgcat::mcset $loc original-k "Based on KAngband: Copyright (c) 2000 John I'anson-Holton"
::msgcat::mcset $loc original-o "Based on OAngband: Copyright (c) 2000 Bahman Rabii"
::msgcat::mcset $loc original-z "Based on ZAngband (c) the ZAngband Dev Team"

::msgcat::mcset $loc email "Send comments to dbaker@direct.ca"

# Fonts
::msgcat::mcset $loc font,sys "{MS Sans Serif}"
::msgcat::mcset $loc font,fixed "Courier"

# Startup Window: buttons
::msgcat::mcset $loc New
::msgcat::mcset $loc Open
::msgcat::mcset $loc Setup
::msgcat::mcset $loc Quit

# Choose Game Window: window title
::msgcat::mcset $loc choose-title "Choose Game"

# Choose Game Window: prompt
::msgcat::mcset $loc choose-prompt \
	"Choose a game from this list of savefiles\
	in the lib\\save directory. Click \"Browse...\" to choose a savefile\
	from a different directory."

# Choose Game Window: column titles
::msgcat::mcset $loc choose-Name Name
::msgcat::mcset $loc choose-Version Version
::msgcat::mcset $loc choose-Description Description

# Choose Game Window: buttons
::msgcat::mcset $loc choose-Open Open

# Choose Game Window: misc
::msgcat::mcset $loc choose-file "Selected file:"
::msgcat::mcset $loc scan-title "Scanning Saved Games..."
::msgcat::mcset $loc scan-file "Scanning \"%s\""
::msgcat::mcset $loc scan-display Displaying...

# Setup Window: window title
::msgcat::mcset $loc setup-title Setup

# Setup Window: tabs
::msgcat::mcset $loc setup-Icon Icons
::msgcat::mcset $loc setup-Music Music
::msgcat::mcset $loc setup-Sound Sound
::msgcat::mcset $loc setup-Variant Variant

# Setup Window: Icons page
::msgcat::mcset $loc icons-prompt "Select an icon set:"

# Setup Window: Music page
::msgcat::mcset $loc music-prompt \
	"If your computer does not have a sound card, or the game\
	crashes a few seconds after starting, you should disable music."
::msgcat::mcset $loc music-use "Use music"
::msgcat::mcset $loc music-lib Library:
::msgcat::mcset $loc music-desc Description:
::msgcat::mcset $loc music-no-libs "No music libraries are installed."

# Platform: windows
::msgcat::mcset $loc music-use-bass "BASS"
::msgcat::mcset $loc music-prompt-bass \
"Requires DirectX 3 or above."
::msgcat::mcset $loc music-use-midas "MIDAS"
::msgcat::mcset $loc music-prompt-midas \
"The MIDAS Digital Audio System."

# Setup Window: Sound page
::msgcat::mcset $loc sound-prompt \
	"If your computer does not have a sound card, you should\
	choose not to use sound."
::msgcat::mcset $loc sound-use "Use sound"
::msgcat::mcset $loc sound-lib Library:
::msgcat::mcset $loc sound-desc Description:
::msgcat::mcset $loc sound-no-libs "No sound libraries are installed."

# Platform: windows
::msgcat::mcset $loc sound-use-directsound "DirectSound"
::msgcat::mcset $loc sound-prompt-directsound \
	"DirectSound starts playing sounds quickly, and plays\
	sounds at any recorded frequency."
::msgcat::mcset $loc sound-use-bass "BASS (samples)"
::msgcat::mcset $loc sound-prompt-bass \
	"Requires DirectX 3 or above."
::msgcat::mcset $loc sound-use-bass-stream "BASS (streams)"
::msgcat::mcset $loc sound-prompt-bass-stream \
	"Requires DirectX 3 or above."
::msgcat::mcset $loc sound-use-wavemix "WaveMix"
::msgcat::mcset $loc sound-prompt-wavemix \
	"This is a slow sound mixer which plays sounds recorded\
	at 11KHz only. Use this only if your system does not have\
	DirectX installed."

# Platform: unix
::msgcat::mcset $loc sound-use-openal "Use OpenAL"
::msgcat::mcset $loc sound-prompt-openal \
	"Play sounds using the OpenAL library."
::msgcat::mcset $loc sound-use-sdl "Use SDL Mixer"
::msgcat::mcset $loc sound-prompt-sdl \
	"Play sounds using the SDL Mixer library."

# Setup Window: Variant page
::msgcat::mcset $loc variant-always "Always choose this variant"

# UpgradeTool
::msgcat::mcset $loc upgrade-tool-title "Import Old Files"
::msgcat::mcset $loc upgrade-tool-prompt \
	"UpgradeTool can import your settings and savefiles from\
	an older version of %s. If you do not have an older version of\
	the game then you do not need to use UpgradeTool.\n\nDo you want to run\
	UpgradeTool now?"

