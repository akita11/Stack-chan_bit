# Stack-chan_bit

[ｽﾀｯｸﾁｬﾝ](https://protopedia.net/prototype/2345Stack-chan)に[micro:bit](https://www.switch-science.com/products/7952)を差し込んで制御するボードです。micro:bitのプログラミングでｽﾀｯｸﾁｬﾝを動かすことができます。

<img src="https://github.com/akita11/Stack-chan_bit/blob/main/Stack-chan_bit1.jpg" width="240px">

<img src="https://github.com/akita11/Stack-chan_bit/blob/main/Stack-chan_bit1.jpg" width="240px">

## 使用例

<img src="https://github.com/akita11/Stack-chan_bit/blob/main/usage1.jpg" width="240px">

<img src="https://github.com/akita11/Stack-chan_bit/blob/main/usage2.jpg" width="240px">

micro:bit側のプログラム（m-asaoka-fap氏作成のカスタムブロックあり）から、ｽﾀｯｸﾁｬﾝを制御できます。→[使用例](https://twitter.com/akita11/status/1656496746321842176)


## 作り方

### 用意するもの

<img src="https://github.com/akita11/Stack-chan_bit/blob/main/build1.jpg" width="240px">

- M5Stack (BasicまたはCore2がオススメ)
- [Stack-chan_Takao_Base](https://github.com/akita11/Stack-chan_Takao_Base)とそれで用いるもの（サーボモータ2個、Groveケーブル、ネジ等、筐体(shell部以外)）※顔部分のM5Stack本体との固定には、BasicではM3x15mm、Core2ではM3x20mmのネジを2本使います
- shell部の筐体(Stack-chan_bit_shell.stlを3Dプリントしたもの): [タカオ版筐体](https://github.com/meganetaaan/stack-chan/tree/dev/v1.0/case/contributed/mongonta_case_for_SG90_and_M5GoBottomBoard/case_for_SG90andM5GoBottomBoard)内の”shell_SG90_basicgraybottom_v1.0.stl"を元に改変


### 組み立て

M5Stackに専用プログラム（FW_PIO/以下。PlatformIO用）を書き込みます。

<img src="https://github.com/akita11/Stack-chan_bit/blob/main/build2.jpg" width="240px">

M5Stackの底板を外し、Stack-chan_bitをさしこみます。

<img src="https://github.com/akita11/Stack-chan_bit/blob/main/build3.jpg" width="240px">

<img src="https://github.com/akita11/Stack-chan_bit/blob/main/build4.jpg" width="240px">

2本のM3ネジで固定します

<img src="https://github.com/akita11/Stack-chan_bit/blob/main/build5.jpg" width="240px">

[Stack-chan_Takao_Base](https://github.com/akita11/Stack-chan_Takao_Base)を使って
Bracket部（サーボ2個が固定される部分）を組み立てつつ、Stack-chan:bit基板と、Groveケーブルで接続します。

残りの部分は、Stack-chan_Takao_BaseからのGroveケーブルは、M5Stack本体ではなく、Stack-chan_bitのGroveコネクタに接続する

残りの部分は、[タカオさんの手順](https://raspberrypi.mongonta.com/how-to-build-easy-stackchan-m5gobottom/)を参考に組み立てます。


## プログラミング

micro:bitとｽﾀｯｸﾁｬﾝ(M5Stack)の間はUART通信(115200bps・N81、コマンド区切りはLF/0x0a/'\n'）で以下のような制御コマンドを送って制御します。[micro:bit側のカスタムブロック](https://github.com/m-asaoka-fap/stackchanbit_2)もあり、MakeCoreで組み込んで使用できます。

- 動作の制御
  - Xnn : パン動作（横方向）nn=角度、範囲＝-90〜90（正面=0）
  - Ynn : チルト動作（縦方向）nn=角度、範囲=0〜30（正面=0から上向きへ）
- 顔の制御
  - Txxxx : 吹き出しに”xxxx”と表示し、それにあわせて口が動く
  - En : 表情を設定。n=0〜5 (0=Angry, 1=Sleepy, 2=Happy, 3=Sat, 4=Doubt, 5=Neutral)


## Author

Junichi Akita (@akita11, akita@ifdl.jp)
