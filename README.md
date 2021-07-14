# ターミナル用マインスイーパー
このプログラムは、ターミナル上でマインスイーパーを楽しめます。

# 使用ライブラリ
`ncurses`ライブラリを使用しています

# 動作確認環境
| 環境 | バージョン |
| --- | --- |
| OS | macOS 11.4 |
| C言語 | Apple clang version 12.0.5 |

# 実行方法
1. `ncurses`のインストール
    
    - `ncurses`ライブラリは標準ライブラリなので、基本的にインストールは不要です。
    - `ncurses`ライブラリの本体は以下の手順でインストールできます。

    <br>
  
    1. [ダウンロードページ](http://ftp.gnu.org/gnu/ncurses/)から任意のバージョンの`ncurses-ver.tar.gz`をダウンロード
    2. ダウンロードした`ncurses-ver.tar.gz`をダウンロードディレクトリツ内で展開
    3. ターミナルで以下のコマンドを実行
    
        ```
        $ cd ~/Download/ncurses-5.7
        $ ./configure
        $ make
        $ sudo make install
        ```
  
2. GitHubリポジトリのクローン
    
    任意のディレクトリにて、次のコマンドでこのリポジトリをクローンします。
    
    ```
    $ git clone https://github.com/Lium1126/minesweeper-in-term/
    ```
    
3. ディレクトリの移動
    
    次のコマンドを実行して、プログラムのあるディレクトリに移動します
    
    ```
    $ cd src
    ```
    
4. プログラムのコンパイルおよび実行
    
    次のコマンドで、プログラムをコンパイルし実行します。
    
    ```
    $ gcc -lncurses minesweeper.c && ./a.out
    ```
    
# 操作方法
## ホーム画面
        
