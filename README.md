# wav-osilloscope
wavの波形を出すアレです。

## 使い方

実行ファイルのカレントディレクトリに`sample.wav`を置いて実行します。するとwavの波形が出ます。

## リファレンス

ouchi::wave_viewer
----
### メソッド

- ctor

  いずれかのコンストラクタで読み込むファイルを指定します。main.cppでは"sample.wav"を渡しています。

- void draw(sf::RenderTarget & rt, sf::RenderStates rs = sf::RenderStates::Default)
  
   SFMLの[`Drawable`](https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1Drawable.php)を継承しています。`sf::Drawable::draw`と同じ効能です
   
- void update(sf::RenderTarget const & rt)

  内部情報を書き換えて描画に必要なデータを作成/更新します。drawを呼び出す前に呼んでください。

----
### メンバ変数

- bkg_

  背景色です。

- frg_

  前景色です。

- offset_

  画面の左端が音声ファイルの中の何サンプル目かを表します。

- zoom

  x方向のズームです。高ければ高いほどフレームレートも上がります。
  
  
## LICENSE

### wav-osilloscope

MIT License

Copyright (c) 2018 ouchiminh

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  
### SFML

SFML (Simple and Fast Multimedia Library) - Copyright (c) Laurent Gomila

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from
the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim
   that you wrote the original software. If you use this software in a product,
   an acknowledgment in the product documentation would be appreciated but is
   not required.

2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.