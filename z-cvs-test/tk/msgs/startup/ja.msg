set loc ja

::msgcat::mcset $loc copyright "%s: Copyright (c) 2001 Tim Baker"

::msgcat::mcset $loc original-a "Based on Angband: Copyright (c) 2000 R. Ruehlmann"
::msgcat::mcset $loc original-k "Based on KAngband: Copyright (c) 2000 John I'anson-Holton"
::msgcat::mcset $loc original-o "Based on OAngband: Copyright (c) 2000 Bahman Rabii"
::msgcat::mcset $loc original-z "Based on ZAngband (c) the ZAngband Dev Team"

::msgcat::mcset $loc email "Send comments to dbaker@direct.ca"

#Fonts
if {[Platform unix]} {
::msgcat::mcset $loc font,sys "{mincho}"
::msgcat::mcset $loc font,fixed "{mincho}"
}

if {[Platform windows]} {
::msgcat::mcset $loc font,sys "{�l�r �o�S�V�b�N}"
::msgcat::mcset $loc font,fixed "{�l�r �o�S�V�b�N}"
}

# Startup Window: buttons
::msgcat::mcset $loc New �V�K
::msgcat::mcset $loc Open �ĊJ
::msgcat::mcset $loc Setup �ݒ�
::msgcat::mcset $loc Quit �I��

# Choose Game Window: window title
::msgcat::mcset $loc choose-title �L�����N�^�I��

# Choose Game Window: prompt
::msgcat::mcset $loc choose-prompt [join {
lib\\save�f�B���N�g���̃t�@�C������L�����N�^��I��ł��������B
���̃f�B���N�g������I�Ԃɂ�\"�Q��...\"�������Ă��������B
} ""]

# Choose Game Window: column titles
::msgcat::mcset $loc choose-Name ���O
::msgcat::mcset $loc choose-Version �o�[�W����
::msgcat::mcset $loc choose-Description ���e

# Choose Game Window: buttons
::msgcat::mcset $loc choose-Open �J��

# Choose Game Window: misc
::msgcat::mcset $loc choose-file �I���t�@�C��
::msgcat::mcset $loc scan-title "�Z�[�u�t�@�C��������..."
::msgcat::mcset $loc scan-file "\"%s\"��ǂ�ł��܂�"
::msgcat::mcset $loc scan-display "�\����"

# Setup Window: window title
::msgcat::mcset $loc setup-title �ݒ�

# Setup Window: tabs
::msgcat::mcset $loc setup-Icon �O���t�B�b�N
::msgcat::mcset $loc setup-Music �a�f�l
::msgcat::mcset $loc setup-Sound ���ʉ�
::msgcat::mcset $loc setup-Variant Variant

# Setup Window: Icons page
::msgcat::mcset $loc icons-prompt "�O���t�B�b�N��I�����Ă�������:"

# Setup Window: Music page
::msgcat::mcset $loc music-prompt [join {
�T�E���h�J�[�h���Ȃ��ꍇ��A�Q�[���J�n���b�ŃN���b�V������
�ꍇ�� OFF �ɂ��Ă��������B
} ""]

::msgcat::mcset $loc music-use "BGM ���g�p"
::msgcat::mcset $loc music-lib BGM�p���C�u����:
::msgcat::mcset $loc music-desc �ڍ�:
::msgcat::mcset $loc music-no-libs "BGM�p���C�u����������܂���B"

# Platform: windows
::msgcat::mcset $loc music-use-bass "BASS"
::msgcat::mcset $loc music-prompt-bass \
"DirectX 3 �ȏオ�K�v�ł��B"
::msgcat::mcset $loc music-use-midas "MIDAS"
::msgcat::mcset $loc music-prompt-midas \
"MIDAS �f�W�^���E�I�[�f�B�I�E�V�X�e��"

# Setup Window: Sound page
::msgcat::mcset $loc sound-prompt \
	"�T�E���h�J�[�h���Ȃ��ꍇ�I���ɂ��Ȃ��ł��������B"
::msgcat::mcset $loc sound-use "�T�E���h�g�p"
::msgcat::mcset $loc sound-lib �T�E���h�p���C�u����:
::msgcat::mcset $loc sound-desc �ڍ�:
::msgcat::mcset $loc sound-no-libs "�T�E���h�p���C�u����������܂���B"

# Platform: windows
::msgcat::mcset $loc sound-use-directsound "DirectSound �g�p"
::msgcat::mcset $loc sound-prompt-directsound [join {
DirectSound �͍����ɉ���炵�A�C�ӂ̎��g����
�Đ��ł��܂��B�����炪�������߂ł��B
} ""]
::msgcat::mcset $loc sound-use-bass "BASS (samples)"
::msgcat::mcset $loc sound-prompt-bass \
        "DirectX 3 �ȏオ�K�v�ł��B"
::msgcat::mcset $loc sound-use-bass-stream "BASS (streams)"
::msgcat::mcset $loc sound-prompt-bass-stream \
        "DirectX 3 �ȏオ�K�v�ł��B"
::msgcat::mcset $loc sound-use-wavemix "WaveMix �g�p"
::msgcat::mcset $loc sound-prompt-wavemix [join {
����͒x���A����11KHz�ł��������Đ��ł��܂���B
DirectX ���C���X�g�[������ĂȂ��ꍇ�݂̂������I�����Ă��������B
} ""]
# Platform: unix
::msgcat::mcset $loc sound-use-openal "Use OpenAL"
::msgcat::mcset $loc sound-prompt-openal \
	"OpenAL ���C�u�������g�p"
::msgcat::mcset $loc sound-use-sdl "Use SDL Mixer"
::msgcat::mcset $loc sound-prompt-sdl \
	"SDL Mixer ���C�u�������g�p"

# Setup Window: Variant page
::msgcat::mcset $loc variant-always "Always choose this variant"

# UpgradeTool
::msgcat::mcset $loc upgrade-tool-title "�ݒ�t�@�C���̃R�s�["
::msgcat::mcset $loc upgrade-tool-prompt [join {
�A�b�v�O���[�h�E�c�[�����g���ƍ��܂Ŏg�p���Ă���%s��
�Â��o�[�W�����̃Z�[�u�t�@�C����ݒ�t�@�C���������p����
�g�����Ƃ��ł��܂��B���߂Ă��̃Q�[��������ꍇ�͕K�v
����܂���B\n\n�A�b�v�O���[�h�E�c�[�����N�����܂���?
} ""]