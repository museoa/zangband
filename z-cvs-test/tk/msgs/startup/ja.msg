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
::msgcat::mcset $loc font,sys "{ＭＳ Ｐゴシック}"
::msgcat::mcset $loc font,fixed "{ＭＳ Ｐゴシック}"
}

# Startup Window: buttons
::msgcat::mcset $loc New 新規
::msgcat::mcset $loc Open 再開
::msgcat::mcset $loc Setup 設定
::msgcat::mcset $loc Quit 終了

# Choose Game Window: window title
::msgcat::mcset $loc choose-title キャラクタ選択

# Choose Game Window: prompt
::msgcat::mcset $loc choose-prompt [join {
lib\\saveディレクトリのファイルからキャラクタを選んでください。
他のディレクトリから選ぶには\"参照...\"を押してください。
} ""]

# Choose Game Window: column titles
::msgcat::mcset $loc choose-Name 名前
::msgcat::mcset $loc choose-Version バージョン
::msgcat::mcset $loc choose-Description 内容

# Choose Game Window: buttons
::msgcat::mcset $loc choose-Open 開く

# Choose Game Window: misc
::msgcat::mcset $loc choose-file 選択ファイル
::msgcat::mcset $loc scan-title "セーブファイル検索中..."
::msgcat::mcset $loc scan-file "\"%s\"を読んでいます"
::msgcat::mcset $loc scan-display "表示中"

# Setup Window: window title
::msgcat::mcset $loc setup-title 設定

# Setup Window: tabs
::msgcat::mcset $loc setup-Icon グラフィック
::msgcat::mcset $loc setup-Music ＢＧＭ
::msgcat::mcset $loc setup-Sound 効果音
::msgcat::mcset $loc setup-Variant Variant

# Setup Window: Icons page
::msgcat::mcset $loc icons-prompt "グラフィックを選択してください:"

# Setup Window: Music page
::msgcat::mcset $loc music-prompt [join {
サウンドカードがない場合や、ゲーム開始数秒でクラッシュする
場合は OFF にしてください。
} ""]

::msgcat::mcset $loc music-use "BGM を使用"
::msgcat::mcset $loc music-lib BGM用ライブラリ:
::msgcat::mcset $loc music-desc 詳細:
::msgcat::mcset $loc music-no-libs "BGM用ライブラリがありません。"

# Platform: windows
::msgcat::mcset $loc music-use-bass "BASS"
::msgcat::mcset $loc music-prompt-bass \
"DirectX 3 以上が必要です。"
::msgcat::mcset $loc music-use-midas "MIDAS"
::msgcat::mcset $loc music-prompt-midas \
"MIDAS デジタル・オーディオ・システム"

# Setup Window: Sound page
::msgcat::mcset $loc sound-prompt \
	"サウンドカードがない場合オンにしないでください。"
::msgcat::mcset $loc sound-use "サウンド使用"
::msgcat::mcset $loc sound-lib サウンド用ライブラリ:
::msgcat::mcset $loc sound-desc 詳細:
::msgcat::mcset $loc sound-no-libs "サウンド用ライブラリがありません。"

# Platform: windows
::msgcat::mcset $loc sound-use-directsound "DirectSound 使用"
::msgcat::mcset $loc sound-prompt-directsound [join {
DirectSound は高速に音を鳴らし、任意の周波数で
再生できます。こちらがおすすめです。
} ""]
::msgcat::mcset $loc sound-use-bass "BASS (samples)"
::msgcat::mcset $loc sound-prompt-bass \
        "DirectX 3 以上が必要です。"
::msgcat::mcset $loc sound-use-bass-stream "BASS (streams)"
::msgcat::mcset $loc sound-prompt-bass-stream \
        "DirectX 3 以上が必要です。"
::msgcat::mcset $loc sound-use-wavemix "WaveMix 使用"
::msgcat::mcset $loc sound-prompt-wavemix [join {
これは遅く、かつ11KHzでしか音を再生できません。
DirectX がインストールされてない場合のみこちらを選択してください。
} ""]
# Platform: unix
::msgcat::mcset $loc sound-use-openal "Use OpenAL"
::msgcat::mcset $loc sound-prompt-openal \
	"OpenAL ライブラリを使用"
::msgcat::mcset $loc sound-use-sdl "Use SDL Mixer"
::msgcat::mcset $loc sound-prompt-sdl \
	"SDL Mixer ライブラリを使用"

# Setup Window: Variant page
::msgcat::mcset $loc variant-always "Always choose this variant"

# UpgradeTool
::msgcat::mcset $loc upgrade-tool-title "設定ファイルのコピー"
::msgcat::mcset $loc upgrade-tool-prompt [join {
アップグレード・ツールを使うと今まで使用していた%sの
古いバージョンのセーブファイルや設定ファイルを引き継いで
使うことができます。初めてこのゲームをする場合は必要
ありません。\n\nアップグレード・ツールを起動しますか?
} ""]
