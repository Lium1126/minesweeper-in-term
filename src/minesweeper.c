//
//  k19061_0722_MineSweeperWithCurses2.c
//  prg2019
//
//  Created by k19061kk on 2019/07/11.
//

/*-------------------------------------------+
 プログラミング及び演習I 最終課題
 おもしろい・興味深い・役に立つ・びっくりするプログラム
 を作成せよ
   -マインスイーパーを作成した
 +-------------------------------------------*/

// インクルード=========================================================
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ncurses.h>

// 定義===============================================================
#define INIT_N (12)          // マップ1辺の初期マス数
#define MAX_N (15)           // マップ1辺のマス数の最大値
#define MIN_N (5)            // マップ1辺のマス数の最小値
#define INIT_MINE_NUM (12)   // 地雷の初期個数
#define MAX_MINE_NUM (20)    // 地雷の個数の最大値
#define MIN_MINE_NUM (5)     // 地雷の個数の最小値
#define MAIN_MENU_NUM_H (2)  // メインメニューの縦方向の項目数
#define MAIN_MENU_NUM_W (2)  // メインメニューの横方向の項目数
#define SUB_MENU_NUM (2)     // サブメニューの項目数
#define MENU_LEN (256)       // メニュー項目の最大文字列長

//-----------------------------------------------------
// マップの1マスあたりの情報を格納するデータ構造
//-----------------------------------------------------
typedef struct {
    int openFlag;
    /*------------------------------------+
     そのマスが開いているかを示すメンバ変数
     openFlag = 0:まだ開けていない
     openFlag = 1:すでに開けてある
     +------------------------------------*/
    int contentOfCell;
    /*------------------------------------+
     そのマスの内容を表すメンバ変数
     contentOfCell = -1   :地雷
     contentOfCell = -1以外:八近傍の地雷の個数
     +------------------------------------*/
    int lockFlag;
    /*------------------------------------+
     そのマスがロックされているか示すメンバ変数
     lockFlag = 0:マスがロックされていない
     lockFlag = 1:マスがロックされている
     +------------------------------------*/
} Cell;

//-----------------------------------------------------
// カーソルの座標を格納するデータ構造
//-----------------------------------------------------
typedef struct {
    int x, y;
} CursorPT;
typedef CursorPT ArrowPT; // 矢印のポインタとしても使う

//-----------------------------------------------------
// 難易度の情報を格納するデータ構造
//-----------------------------------------------------
typedef struct{
    int mineNum;  // 地雷の個数
    int n;        // マップの1辺のマス数
} Difficulty;

// 関数プロトタイプ宣言===================================================
// 基本操作関数--------------------------------------------
void setOpenFlag(Cell *cell, int n);                                       // cellに開いているかを表す情報nを格納する
int getOpenFlag(Cell cell);                                                // cellが開いているかの情報を取得する
int isOpened(Cell cell);                                                   // cellが開いているか判定する
void cellOpen(Cell *cell);                                                 // cellを開く
void setContentOfCell(Cell *cell, int n);                                  // cellに内容nを格納する
void setMine(Cell *cell);                                                  // cellに地雷を設置する
int getContentOfCell(Cell cell);                                           // cellの内容を取得する
int isContentMine(Cell cell);                                              // cellの内容が地雷か判定する
int isContentZero(Cell cell);                                              // cellの内容が0か判定する
void setLockFlag(Cell *cell, int n);                                       // cellにロック状態を表す情報nを格納する
int getLockFlag(Cell cell);                                                // cellのロック状態を取得する
void cellLock(Cell *cell);                                                 // cellをロックする
void cellUnlock(Cell *cell);                                               // cellのロックを解除する
int isLocked(Cell cell);                                                   // cellがロックされているか判定する
void setX(CursorPT *cursor, int x);                                        // カーソルのx座標を格納する
int getX(CursorPT cursor);                                                 // カーソルのx座標を取得する
void setY(CursorPT *cursor, int y);                                        // カーソルのy座標を格納する
int getY(CursorPT cursor);                                                 // カーソルのy座標を取得する
void initCursorPT(CursorPT *cursor);                                       // カーソルポインタを初期化する
void setMineNum(Difficulty *difficulty, int n);                            // 地雷の個数をn個に設定する
int getMineNum(Difficulty difficulty);                                     // 地雷の個数を取得する
void setN(Difficulty *difficulty, int n);                                  // マップの1辺のマスをn個に設定する
int getN(Difficulty difficulty);                                           // マップの1辺のマス数を取得する
void initDifficulty(Difficulty *difficulty);                               // 難易度を初期化する

// その他の関数--------------------------------------------
void checkScreenSize(int x, int y);                                        // スクリーンサイズが十分な大きさか調べる
int mineSweeper(Difficulty diff);                                          // ゲーム本体を実行する
void initMaps(Cell maps[MAX_N][MAX_N]);                                    // マップmapsを初期化する
void makeMaps(Cell maps[MAX_N][MAX_N], Difficulty diff);                   // ゲーム開始時のマップを作成する
void dispMaps(Cell maps[MAX_N][MAX_N], CursorPT cursor, Difficulty diff);  // マップを表示する
int analyzeInput(Cell maps[MAX_N][MAX_N], CursorPT *cursor, int n);        // 入力を解析し、それに応じた処理をする
void openCell(Cell maps[MAX_N][MAX_N], int x, int y, int n);               // マスを開いてそれに応じた処理をする
int clearCheck(Cell maps[MAX_N][MAX_N], Difficulty diff);                  // ゲームクリアを判定する
int dispResult(int result);                                                // resultに応じたゲームの結果を表示する
void dispOperationExplanation(void);                                       // 操作説明画面を表示する
void changeSetting(Difficulty *diff);                                      // 難易度の変更画面を表示する

// 関数群==============================================================
// main()関数---------------------------------------------
int main(int argc, const char *argv[])
{
    char menu[MAIN_MENU_NUM_H][MAIN_MENU_NUM_W][MENU_LEN] = {
        {"Game start", "Change setting"},
        {"How to play", "Exit"}
    };                                       // メインメニュー項目
    int exitFlag = 0;                        // ゲーム終了を示すフラグ変数(0:ゲーム未終了 1:ゲーム終了)
    int resultFlag = 0;                      // ゲーム結果を示すフラグ変数(0:ゲーム中断 1:ゲームクリア -1:ゲームオーバー)
    int retryFlag = 0;                       // ゲームを繰り返すかを示すフラグ変数(0:リトライしない 1:リトライする)
    int max_x, max_y;                        // スクリーンサイズを格納する変数
    int i, j;                                // カウンタ変数
    ArrowPT arrow;                           // メインメニューを指す矢印
    Difficulty difficulty;                   // ゲームの難易度(地雷の個数とマスの個数)が格納された変数

    initDifficulty(&difficulty);             // 難易度の初期化
    
    /*---curses関連の初期化---*/
    initscr();                               // 画面制御処理の初期化
    noecho();                                // 入力時に入力された文字を表示しない
    keypad(stdscr, TRUE);                    // カーソルキーマクロを有効にする
    setY(&arrow, 0);                         // ---+
    setX(&arrow, 0);                         // ---+-- 矢印の位置を初期化
    
    do{
        /*---画面再描画の前処理---*/
        clear();                             // 画面のクリア
        getmaxyx(stdscr, max_y, max_x);      // スクリーンサイズの取得
        checkScreenSize(max_x, max_y);       // スクリーンサイズを検証
        
        /*---タイトル、メインメニューおよび矢印の表示---*/
        mvaddstr((max_y / 2) - 4, (max_x / 2) - (strlen("Mine Sweeper") / 2), "Mine Sweeper");
        for(i = 0;i < MAIN_MENU_NUM_H;i++){
            for(j = 0;j < MAIN_MENU_NUM_W;j++){
                if(j == 0){
                    mvaddstr((max_y / 2) + i + 1, (max_x / 4) - (strlen(menu[i][j]) / 2), menu[i][j]);
                }else if(j == 1){
                    mvaddstr((max_y / 2) + i + 1, (max_x / 4 * 3) - (strlen(menu[i][j]) / 2), menu[i][j]);
                }
            }
        }
        if(getX(arrow) == 0){
            mvaddstr((max_y / 2) + getY(arrow) + 1, (max_x / 4) - (strlen(menu[getY(arrow)][getX(arrow)]) / 2) - 3, "->");
        }else if(getX(arrow) == 1){
            mvaddstr((max_y / 2) + getY(arrow) + 1, (max_x / 4 * 3) - (strlen(menu[getY(arrow)][getX(arrow)]) / 2) - 3, "->");
        }
        move(max_y - 1, max_x - 1);          // カーソルはスクリーンの端に退避
        
        /*---以降キー押下時の処理---*/
        int ch = getch();                    // 押されたキーを取得
        switch(ch){
            case KEY_UP:                     // カーソルキー(上)押下
            case 'w':                        // 'w'キー押下
                /*---矢印を上に移動させる(y座標をデクリメント)---*/
                /*---一番上の項目を選択していたら、一番下の項目へと移動する---*/
                setY(&arrow, getY(arrow) - 1);
                if(getY(arrow) < 0){
                    setY(&arrow, MAIN_MENU_NUM_H - 1);
                }
                break;
                
            case KEY_DOWN:                   // カーソルキー(下)押下
            case 's':                        // 's'キー押下
                /*---矢印を下に移動させる(y座標をインクリメント)---*/
                /*---一番下の項目を選択していたら、一番上の項目へと移動する---*/
                setY(&arrow, getY(arrow) + 1);
                if(getY(arrow) > MAIN_MENU_NUM_H - 1){
                    setY(&arrow, 0);
                }
                break;
                
            case  KEY_LEFT:                  // カーソルキー(左)押下
            case 'd':                        // 'd'キー押下
                /*---矢印を左に移動させる(x座標をデクリメント)---*/
                /*---一番左の項目を選択していたら、一番右の項目へと移動する---*/
                setX(&arrow, getX(arrow) - 1);
                if(getX(arrow) < 0){
                    setX(&arrow, MAIN_MENU_NUM_W - 1);
                }
                break;
                
            case KEY_RIGHT:                  // カーソルキー(右)押下
            case 'a':                        // 'a'キー押下
                /*---矢印を右に移動させる(x座標をインクリメント)---*/
                /*---一番右の項目を選択していたら、一番左の項目へと移動する---*/
                setX(&arrow, getX(arrow) + 1);
                if(getX(arrow) > MAIN_MENU_NUM_W - 1){
                    setX(&arrow, 0);
                }
                break;
                
            case '\n':                       // エンターキー押下
                if(getY(arrow) == 0 && getX(arrow) == 0){             // "Game start"を選択
                    /*---ユーザがリトライを選択する間、ゲーム本体とリザルト画面の表示を繰り返す---*/
                    do{
                        retryFlag = 0;
                        resultFlag = mineSweeper(difficulty);
                        if(resultFlag != 0){ // ゲームが中断されていないなら
                            retryFlag = dispResult(resultFlag);
                        }
                    }while(retryFlag == 1);
                }else if(getY(arrow) == 0 && getX(arrow) == 1){       // "Change setting"を選択
                    /*---難易度設定を変更する---*/
                    changeSetting(&difficulty);
                }else if(getY(arrow) == 1 && getX(arrow) == 0){       // "How to play"を選択
                    /*---操作説明画面を表示する---*/
                    dispOperationExplanation();
                }else if(getY(arrow) == 1 && getX(arrow) == 1){       // "Exit"を選択
                    /*---ゲーム終了フラグを立てる---*/
                    exitFlag = 1;
                }
        }
    }while(exitFlag == 0);                   // "Exit"が選択されるまで繰り返す
    
    /*---curses関連の終了処理---*/
    endwin();
    
    return(0);
}

// 基本操作関数---------------------------------------------
//------------------------------------------------------------
// cellに開いているかを表す情報nを格納する
// 引数 Cell *cell: 開閉情報を格納する対象を指すポインタ
//     int n     : 開閉情報を表す数値n
// 返り値 なし
//------------------------------------------------------------
void setOpenFlag(Cell *cell, int n){
    cell -> openFlag = n;
    return;
}

//------------------------------------------------------------
// cellが開いているかの情報を取得する
// 引数 Cell cell: 開閉情報を取得する対象
// 返り値 引数cellの開閉情報
//------------------------------------------------------------
int getOpenFlag(Cell cell){
    return(cell.openFlag);
}

//------------------------------------------------------------
// cellが開いているか判定する
// 引数 Cell cell: 開閉を判定する対象
// 返り値 引数cellが開いているなら1、開いていないなら0
//------------------------------------------------------------
int isOpened(Cell cell){
    if(getOpenFlag(cell) == 1){
        return(1);
    }else{
        return(0);
    }
}

//------------------------------------------------------------
// cellを開く
// 引数 Cell *cell: 開く対象cellへのポインタ
// 返り値 なし
//------------------------------------------------------------
void cellOpen(Cell *cell){
    setOpenFlag(cell, 1);
    return;
}

//------------------------------------------------------------
// cellに内容nを格納する
// 引数 Cell *cell: 内容を格納する対象cellへのポインタ
//     int n     : 内容を表す情報n
// 返り値 なし
//------------------------------------------------------------
void setContentOfCell(Cell *cell, int n){
    cell -> contentOfCell = n;
    return;
}

//------------------------------------------------------------
// cellに地雷を設置する
// 引数 Cell *cell: 地雷を設置する対象cellへのポインタ
// 返り値 なし
//------------------------------------------------------------
void setMine(Cell *cell){
    setContentOfCell(cell, -1);
    return;
}

//------------------------------------------------------------
// cellの内容を取得する
// 引数 Cell cell: 内容を取得する対象cell
// 返り値 cellの内容 -1:地雷 -1以外:八近傍の地雷の個数
//------------------------------------------------------------
int getContentOfCell(Cell cell){
    return(cell.contentOfCell);
}

//------------------------------------------------------------
// cellの内容が地雷か判定する
// 引数 Cell cell: 地雷か判定する対象cell
// 返り値 cellの内容が地雷なら1、そうでないなら0
//------------------------------------------------------------
int isContentMine(Cell cell){
    if(getContentOfCell(cell) == -1){
        return(1);
    }else{
        return(0);
    }
}

//------------------------------------------------------------
// cellの内容が0か判定する
// 引数 Cell cell: 内容が0か判定する対象cell
// 返り値 cellの内容が0なら1、そうでないなら0
//------------------------------------------------------------
int isContentZero(Cell cell){
    if(getContentOfCell(cell) == 0){
        return(1);
    }else{
        return(0);
    }
}

//------------------------------------------------------------
// cellにロック状態を表す情報nを格納する
// 引数 Cell *cell: ロック状態を表す情報を格納する対象cellへのポインタ
//     int n     : ロック状態を表す情報n
// 返り値 なし
//------------------------------------------------------------
void setLockFlag(Cell *cell, int n){
    if(n == 0){
        cell -> lockFlag = 0;
    }else{
        cell -> lockFlag = 1;
    }
    return;
}

//------------------------------------------------------------
// cellのロック状態を取得する
// 引数 Cell cell: ロック状態を表す情報を取得する対象cell
// 返り値 引数cellのロック状態を表す情報
//------------------------------------------------------------
int getLockFlag(Cell cell){
    return(cell.lockFlag);
}

//------------------------------------------------------------
// cellをロックする
// 引数 Cell *cell: ロックする対象cellへのポインタ
// 返り値 なし
//------------------------------------------------------------
void cellLock(Cell *cell){
    setLockFlag(cell, 1);
    return;
}

//------------------------------------------------------------
// cellのロックを解除する
// 引数 Cell *cell: ロックを解除する対象cellへのポインタ
// 返り値 なし
//------------------------------------------------------------

void cellUnlock(Cell *cell){
    setLockFlag(cell, 0);
    return;
}

//------------------------------------------------------------
// cellがロックされているか判定する
// 引数 Cell cell: ロックされているか判定する対象cell
// 返り値 引数cellがロックされているなら1、そうでないなら0
//------------------------------------------------------------
int isLocked(Cell cell){
    if(getLockFlag(cell) == 1){
        return(1);
    }else{
        return(0);
    }
}

//------------------------------------------------------------
// カーソルのx座標を格納する
// 引数 CursorPT *cursor: x座標を格納する対象のカーソルポインタ
//     int x           : 格納するx座標
// 返り値 なし
//------------------------------------------------------------
void setX(CursorPT *cursor, int x){
    cursor -> x = x;
    return;
}

//------------------------------------------------------------
// カーソルのx座標を取得する
// 引数 CursorPT cursor: x座標の取得元となるカーソルポインタ
// 返り値 cursorに格納されているx座標
//------------------------------------------------------------
int getX(CursorPT cursor){
    return(cursor.x);
}

//------------------------------------------------------------
// カーソルのy座標を格納する
// 引数 CursorPT *cursor: y座標を格納する対象のカーソルポインタ
//     int x           : 格納するy座標
// 返り値 なし
//------------------------------------------------------------
void setY(CursorPT *cursor, int y){
    cursor -> y = y;
    return;
}

//------------------------------------------------------------
// カーソルのy座標を取得する
// 引数 CursorPT cursor: y座標の取得元となるカーソルポインタ
// 返り値 cursorに格納されているy座標
//------------------------------------------------------------
int getY(CursorPT cursor){
    return(cursor.y);
}

//------------------------------------------------------------
// カーソルポインタcursorを初期化する
// 具体的に、(0,0)を設定する
// 引数 CursorPT *cursor: 初期化するカーソルポインタ
// 返り値 なし
//------------------------------------------------------------
void initCursorPT(CursorPT *cursor){
    setX(cursor, 0);
    setY(cursor, 0);
    return;
}

//------------------------------------------------------------
// 地雷の個数をn個に設定する
// 引数 Difficulty *difficulty: 地雷の個数を設定する対象へのポインタ
//     int n                 : 設定する地雷の個数
// 返り値 なし
//------------------------------------------------------------
void setMineNum(Difficulty *difficulty, int n){
    difficulty -> mineNum = n;
    return;
}

//------------------------------------------------------------
// 地雷の個数を取得する
// 引数 Difficulty difficulty: 現在設定されている難易度
// 返り値 現在設定されている地雷の個数を返す
//------------------------------------------------------------
int getMineNum(Difficulty difficulty){
    return(difficulty.mineNum);
}

//------------------------------------------------------------
// マップの1辺のマスをn個に設定する
// 引数 Difficulty *difficulty: 1辺のマス数を設定する対象へのポインタ
//     int n                 : 設定する1辺のマスの個数
// 返り値 なし
//------------------------------------------------------------
void setN(Difficulty *difficulty, int n){
    difficulty -> n = n;
    return;
}

//------------------------------------------------------------
// マップの1辺のマス数を取得する
// 引数 Difficulty difficulty: 現在設定されている難易度
// 返り値 現在設定されている1辺あたりのマスの個数を返す
//------------------------------------------------------------
int getN(Difficulty difficulty){
    return(difficulty.n);
}

//------------------------------------------------------------
// ゲームの難易度difficultyを初期化する
// 具体的に、地雷の個数にINIT_BOMB_NUMを、1辺のマス数にINIT_Nを設定する
// 引数 Difficulty *difficulty: 初期化する難易度変数
// 返り値 なし
//------------------------------------------------------------
void initDifficulty(Difficulty *difficulty){
    setMineNum(difficulty, INIT_MINE_NUM);
    setN(difficulty, INIT_N);
    return;
}

// その他の関数----------------------------------------------
//------------------------------------------------------------
// スクリーンサイズが十分に大きいか検証する
// 具体的に、スクリーンサイズが80x24より小さければ、プログラムを終了する
// 引数 int x: 横軸方向のスクリーンサイズ
//     int y: 縦軸方向のスクリーンサイズ
// 返り値 なし
//------------------------------------------------------------
void checkScreenSize(int x, int y){
    if(x < 80 || y < 24){
        endwin();  // cursesによる画面制御の終了
        fprintf(stderr, "Error: Screen size must be at least 80x24\n");
        exit(EXIT_FAILURE);
    }
    return;
}

//------------------------------------------------------------
// ゲーム本体を実行する
// 引数 Difficulty diff: ゲームの難易度情報
// 返り値 ゲームクリアしたら1、ゲームオーバーしたら-1、ゲームを中断したら0
//       を返す
//------------------------------------------------------------
int mineSweeper(Difficulty diff){
    Cell maps[MAX_N][MAX_N];       // マップ情報を格納する二次元配列
    CursorPT cursor;               // カーソルポインタ
    int escapeFlag = 0;            // 入力に'q'が与えられたかを示すフラグ変数
    int clearFlag = 0;             // ゲームクリアしたかを示すフラグ変数
    
    /*---カーソルポインタの初期化---*/
    initCursorPT(&cursor);         // カーソルポインタの初期化
    
    /*---マップを初期化し、表示---*/
    initMaps(maps);                // マップの初期化
    makeMaps(maps, diff);          // マップ生成
    dispMaps(maps, cursor, diff);  // マップの表示
    
    /*---入力を解析・処理しつつゲームの状態を更新---*/
    do{
        escapeFlag = analyzeInput(maps, &cursor, getN(diff));
        clearFlag = clearCheck(maps, diff);
        dispMaps(maps, cursor, diff);
    }while(clearFlag == 0 && escapeFlag == 0);
    
    return(clearFlag);
}

//------------------------------------------------------------
// マップmapsを初期化する
// 具体的に、マップの全ての要素に対して、開いていない・地雷でない・ロック
// されていない状態にする
// 引数 Cell maps[][]: 初期化するマップ
// 返り値 なし
//------------------------------------------------------------
void initMaps(Cell maps[MAX_N][MAX_N]){
    int i, j;        // カウンタ変数
    
    for(i = 0;i < MAX_N;i++){
        for(j = 0;j < MAX_N;j++){
            setOpenFlag(&maps[i][j], 0);
            setContentOfCell(&maps[i][j], 0);
            setLockFlag(&maps[i][j], 0);
        }
    }
    return;
}

//------------------------------------------------------------
// ゲーム開始時のマップを作成する
// 引数 Cell maps[][]  : 作成する対象のマップ
//     Difficulty diff: 現在設定されている難易度情報
// 返り値 なし
//------------------------------------------------------------
void makeMaps(Cell maps[MAX_N][MAX_N], Difficulty diff){
    int i, j;                                 // カウンタ変数
    int x, y;                                 // 地雷を設置する座標
    int nx, ny;                               // (x,y)の八近傍の座標を格納する変数
    int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};  //----+
    int dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};  //----+-- (x,y)と(nx,ny)の差
    
    srand((unsigned)time(NULL));  // 乱数のシード値を設定
    
    /*---地雷をランダムな座標に配置し、その八近傍について地雷の個数をインクリメント---*/
    for(i = 0;i < getMineNum(diff);i++){
        /*---地雷を設置する座標を求める---*/
        do{
            x = rand() % getN(diff);
            y = rand() % getN(diff);
        }while(isContentMine(maps[y][x]));
        setMine(&maps[y][x]);
        
        /*---設置したマスの八近傍に対して地雷の個数をインクリメント---*/
        for(j = 0;j < 8;j++){
            nx = x + dx[j];
            ny = y + dy[j];
            if((0 <= nx && nx < getN(diff)) && (0 <= ny && ny < getN(diff)) && !isContentMine(maps[ny][nx])){
                setContentOfCell(&maps[ny][nx], getContentOfCell(maps[ny][nx]) + 1);
            }
        }
    }
    
    return;
}

//------------------------------------------------------------
// マップを表示する
// 引数 Cell maps[][]  : 表示する対象のマップ
//     CursorPT cursor: カーソルポインタ
//     Difficulty diff: 現在設定されている難易度情報
// 返り値 なし
//------------------------------------------------------------
void dispMaps(Cell maps[MAX_N][MAX_N], CursorPT cursor, Difficulty diff){
    int max_x, max_y;        // スクリーンサイズを格納する変数
    int cursor_x, cursor_y;  // カーソルを表示する座標
    int x, y;                // マスを出力する際の座標を格納する変数
    int i, j;                // カウンタ変数
    
    /*---画面のクリア---*/
    clear();
    
    /*---スクリーンサイズの取得および検証---*/
    getmaxyx(stdscr, max_y, max_x);
    checkScreenSize(max_x, max_y);
    
    /*---地雷の数およびマップの表示---*/
    mvprintw(0, 0, "MINE_NUM:%d", getMineNum(diff));
    for(i = 0;i < getN(diff);i++){
        for(j = 0;j < getN(diff);j++){
            y = (max_y / 2) - (getN(diff) / 2) + i; //---+
            x = (max_x / 2) - (getN(diff) / 2) + j; //---+-- マスを出力する座標を求める
            if(isLocked(maps[i][j])){             // ロックされていたら'@'と表示
                mvaddch(y, x, '@');
            }else if(!isOpened(maps[i][j])){      // 開いていなかったら'#'と表示
                mvaddch(y, x, '#');
            }else if(isContentMine(maps[i][j])){  // 地雷が開かれている場合は'*'と表示
                mvaddch(y, x, '*');
            }else if(isContentZero(maps[i][j])){  // マスの内容が0だったら'.'を表示
                mvaddch(y, x, '.');
            }else{                                // 地雷以外が開かれている場合はその内容を表示
                mvprintw(y, x, "%d", getContentOfCell(maps[i][j]));
            }
        }
        printw("\n");
    }
    
    /*---カーソルを表示---*/
    cursor_y = (max_y / 2) - (getN(diff) / 2) + getY(cursor);
    cursor_x = (max_x / 2) - (getN(diff) / 2) + getX(cursor);
    move(cursor_y, cursor_x);
    refresh();
    
    return;
}

//------------------------------------------------------------
// 入力を解析し、それに応じた処理をする
// 引数 Cell maps[][]  : 処理対象のマップ
//     CursorPT cursor: カーソルポインタ
//     int n          : マップ1辺のマス数
// 返り値 エスケープキーが押されたら1、それ以外は0
//------------------------------------------------------------
int analyzeInput(Cell maps[MAX_N][MAX_N], CursorPT *cursor, int n){
    int ch;  // 入力値を格納する変数
    
    ch = getch();
    switch(ch){
        /*---以下カーソルキー及び'w','a','s','d'キー押下でカーソルを移動---*/
        case KEY_UP:       // カーソルキー(上)押下
        case 'w':          // 'w'キー押下
            if(getY(*cursor) > 0){
                setY(cursor, getY(*cursor) - 1);
            }
            break;
            
        case KEY_DOWN:     // カーソルキー(下)押下
        case 's':          // 's'キー押下
            if(getY(*cursor) < n - 1){
                setY(cursor, getY(*cursor) + 1);
            }
            break;
            
        case KEY_LEFT:     // カーソルキー(左)押下
        case 'a':          // 'a'キー押下
            if(getX(*cursor) > 0){
                setX(cursor, getX(*cursor) - 1);
            }
            break;
            
        case KEY_RIGHT:    // カーソルキー(右)押下
        case 'd':          // 'd'キー押下
            if(getX(*cursor) < n - 1){
                setX(cursor, getX(*cursor) + 1);
            }
            break;
            
        case '\n':         // エンターキー押下
            /*---カーソルのあるマスを開く---*/
            openCell(maps, getX(*cursor), getY(*cursor), n);
            break;
            
        case ' ':          // スペースキー押下
            /*---カーソルのあるマスをロック(もしくはアンロック)する---*/
            if(!isOpened(maps[getY(*cursor)][getX(*cursor)])){
                if(isLocked(maps[getY(*cursor)][getX(*cursor)])){
                    cellUnlock(&maps[getY(*cursor)][getX(*cursor)]);
                }else{
                    cellLock(&maps[getY(*cursor)][getX(*cursor)]);
                }
            }
            break;
            
        case 0x1B:         // エスケープキー押下
            /*---終了信号を返す---*/
            return(1);
            
    }
    
    return(0);
}

//------------------------------------------------------------
// マスを開いてそれに応じた処理をする
// 引数 Cell maps[][]: 開くマスがあるマップ
//     int x        : 開くマスのx座標
//     int y        : 開くマスのy座標
//     int n        : マップ1辺のマス数
// 返り値 なし
//------------------------------------------------------------
void openCell(Cell maps[MAX_N][MAX_N], int x, int y, int n){
    int i;                                    // カウンタ変数
    int nx, ny;                               // (x,y)の八近傍の座標を格納する変数
    int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};  //---+
    int dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};  //---+-- (x,y)と(nx,ny)の差
    
    if(isOpened(maps[y][x]) || isLocked(maps[y][x])){        // 対象のマスが既に開かれている、またはロックされている
        return;                 // 何もせずreturn
    }else{                                                   // 対象のマスは開くべきマス
        cellOpen(&maps[y][x]);  // 対象のマスを開く
        
        if(isContentZero(maps[y][x])){                       // 対象もマスの八近傍に地雷がない
            /*---深さ優先探索---*/
            for(i = 0;i < 8;i++){
                nx = x + dx[i];
                ny = y + dy[i];
                if((0 <= nx && nx < n) && (0 <= ny && ny < n)){
                    openCell(maps, nx, ny, n);
                }
            }
        }
    }
    
    return;
}

//------------------------------------------------------------
// ゲームクリアを判定する
// 地雷が設置されているマスの全てがロックされていて、かつ地雷が設置されて
// いないマスが全て開いているときゲームクリアとする
// 引数 Cell maps[][]  : ゲームクリアを判定する対象マップ
//     Difficulty diff: 現在設定されている難易度情報
// 返り値 ゲームクリアなら1、そうでないなら0、ゲームオーバーなら-1
//------------------------------------------------------------
int clearCheck(Cell maps[MAX_N][MAX_N], Difficulty diff){
    int i, j;         // カウンタ変数
    int lockCnt = 0;  // ロックされていて、かつ地雷が設置されているマスの個数
    int openCnt = 0;  // 開かれていて、かつ地雷が設置されていないマスの個数
    
    for(i = 0;i < getN(diff);i++){
        for(j = 0;j < getN(diff);j++){
            if(isOpened(maps[i][j]) && isContentMine(maps[i][j])){        // 地雷が設置されているマスが開いている
                return(-1);  // ゲームオーバー
            }else if(isContentMine(maps[i][j]) && isLocked(maps[i][j])){  // 地雷が設置されていて、かつロックされている
                lockCnt++;
            }else if(!isContentMine(maps[i][j]) && isOpened(maps[i][j])){ // 地雷が設置されておらず、かつ開かれている
                openCnt++;
            }
        }
    }
    
    /*---ゲームクリアか判定---*/
    if(lockCnt == getMineNum(diff) && openCnt == (getN(diff) * getN(diff) - getMineNum(diff))){
        return(1);
    }else{
        return(0);
    }
}

//------------------------------------------------------------
// resultに応じたゲームの結果を表示する
// resultの値が規定以外のものの場合、結果は何も表示しない
// 引数 int result: ゲームの結果を表す変数
//                 1: ゲームクリア  -1:ゲームオーバー
// 返り値 ゲームを繰り返すなら1,そうでないなら0
//------------------------------------------------------------
int dispResult(int result){
    int max_x, max_y;                      // スクリーンサイズを格納する変数
    int returnFlag = 0;                    // "Home"を選択したか示すフラグ変数(0:選択決定待ち 1:”Home"が選択された)
    int ch;                                // 入力値を格納する変数
    int arrowY;                            // 矢印のy座標(0~SUB_MENU_NUM-1)
    int i;                                 // カウンタ変数
    char menu[SUB_MENU_NUM][MENU_LEN] = {  // サブメニュー項目
        "Retry",
        "Home"
    };
    
    arrowY = 0;                            // 初期値ではサブメニューの最初の項目を選択

    do{
        /*---画面再描画の前処理---*/
        clear();                           // 画面のクリア
        getmaxyx(stdscr, max_y, max_x);    // スクリーンサイズの取得
        checkScreenSize(max_x, max_y);     // スクリーンサイズを検証
        
        /*---引数からゲーム結果を表示---*/
        if(result == 1){
            mvaddstr((max_y / 2) - 1, (max_x / 2) - (strlen("Game Clear!") / 2), "Game Clear!");
        }else if(result == -1){
            mvaddstr((max_y / 2) - 1, (max_x / 2) - (strlen("Game Over...") / 2), "Game Over...");
        }
        
        /*---サブメニューおよび矢印を表示---*/
        for(i = 0;i < SUB_MENU_NUM;i++){
            mvaddstr((max_y / 2) + 2 + i, (max_x / 2) - (strlen(menu[i]) / 2), menu[i]);
        }
        mvaddstr((max_y / 2) + 2 + arrowY, (max_x / 2) - (strlen(menu[arrowY]) / 2) - 3, "->");
        
        move(max_y - 1, max_x - 1);       // カーソルはスクリーンの端に退避
        
        /*---キー押下時の処理---*/
        ch = getch();
        switch(ch){
            case KEY_UP:                  // カーソルキー(上)押下
            case 'w':                     // 'w'キー押下
            case KEY_DOWN:                // カーソルキー(下)押下
            case 's':                     // 's'キー押下
                /*---arrowYの内容の0/1を切り替える---*/
                arrowY = (arrowY + 1) % 2;
                break;
                
            case '\n':                    // エンターキー押下
                if(arrowY == 0){          // "Retry"を選択
                    return(1);            // ゲームを繰り返すことを表す返り値を返す
                }else if(arrowY == 1){    // "Home"を選択
                    returnFlag = 1;
                }
        }
    }while(returnFlag == 0);              // "Home"が選択されていない間繰り返す
    
    return(0);
}

//------------------------------------------------------------
// 操作説明画面を表示する
// 引数 なし
// 返り値 なし
//------------------------------------------------------------
void dispOperationExplanation(void){
    char maps[12][13] = {
        "..1#1.......",
        "..1#1.......",
        "112#1111..11",
        "######@1..1@",
        "113####11121",
        "..2#######1.",
        ".12#212#211.",
        "12##1.1#1...",
        "###@1.111...",
        "11111.......",
        "............",
        "............"
    };                                                           // ゲーム画面の例
    int i;                                                       // カウンタ変数
    int max_x, max_y;                                            // スクリーンサイズを格納する変数
    int ch;                                                      // 入力された文字を格納する変数
    char *back_to_home = "Please press Enter key";               // ホーム画面に戻る方法の説明文
    char square_explanation[2][19] = {
        "<- @:Locked square",
        "#:Closed square ->"
    };                                                           // マスの説明文
    char operation_explanation[3][32] = {
        "Opening square:Press Enter key.",
        "Locking square:Press Space key.",
        "Stopping game:Press Escape key."
    };                                                           // 操作方法の説明文
    char condition_of_clear[2][52] = {
        "If you lock all squares with mine",
        "and open squeaers without mine, you clear the game!"
    };                                                           // ゲームクリア条件の説明文
    
    do{
        /*---画面再描画の前処理---*/
        clear();                         // 画面のクリア
        getmaxyx(stdscr, max_y, max_x);  // スクリーンサイズの取得
        checkScreenSize(max_x, max_y);   // スクリーンサイズを検証
        
        /*---各種説明文を表示---*/
        //-ゲーム画面の例を表示-
        for(i = 0;i < 12;i++){
            mvaddstr((max_y / 2) - 10 + i, (max_x / 2) - (strlen(maps[i]) / 2), maps[i]);
        }
        //-マスの説明文を表示-
        mvaddstr((max_y / 2) - 7, (max_x / 2) + (strlen(maps[3]) / 2) + 1, square_explanation[0]);
        mvaddstr((max_y / 2) - 2, (max_x / 2) - (strlen(maps[8]) / 2) - (strlen(square_explanation[1]) + 1), square_explanation[1]);
        //-操作方法の説明文を表示-
        for(i = 0;i < 3;i++){
            mvaddstr((max_y / 2) + 3 + i, (max_x / 2) - (strlen(operation_explanation[i]) / 2), operation_explanation[i]);
        }
        //-ゲームクリア条件の説明文を表示-
        for(i = 0;i < 2;i++){
            mvaddstr((max_y / 2) + 7 + i, (max_x / 2) - (strlen(condition_of_clear[i]) / 2), condition_of_clear[i]);
        }
        //-エンターキー押下の要求を表示-
        mvaddstr(max_y - 1, (max_x - 1) - strlen(back_to_home), back_to_home);
    }while((ch = getch()) != '\n');      // エンターキーが押されるまで画面を再描画し続ける
    
    return;
}

//------------------------------------------------------------
// 難易度変更画面の表示および難易度変更を行う
// 引数 Difficulty *diff: 現在の難易度を格納している変数へのポインタ
// 返り値 なし
//------------------------------------------------------------
void changeSetting(Difficulty *diff){
    char back_to_home[2][23] = {
        "Cancel:Press 'x' key.",
        "Decide:Pree Enter key."
    };                                    // 操作メニュー項目
    int ch;                               // 入力値を受け取る変数
    int max_x, max_y;                     // スクリーンサイズを格納する変数
    int i;                                // カウンタ変数
    int tempMineNum = getMineNum(*diff);  // 暫定での地雷の個数
    int tempN = getN(*diff);              // 暫定での1辺あたりのマス数
    int arrowX = 0;                       // 矢印が指している項目(0か1)
    int enterFlag = 0;                    // エンターキーが押下されたか示すフラグ変数
                                          // 0:エンターキー押下待ち 1:エンターキーが押された
    
    /*---スクリーンサイズを取得---*/
    getmaxyx(stdscr, max_y, max_x);
    
    /*---以降操作を受け付けながら画面を再描画---*/
    do{
        /*---画面再描画の前処理---*/
        clear();                          // 画面のクリア
        getmaxyx(stdscr, max_y, max_x);   // スクリーンサイズの取得
        checkScreenSize(max_x, max_y);    // スクリーンサイズを検証
        
        /*---暫定の設定値、矢印および操作説明を表示---*/
        mvprintw((max_y / 2) - 2, (max_x / 4) - (strlen("Mines:12") / 2), "Mines:%2d", tempMineNum);
        mvprintw((max_y / 2) - 2, (max_x / 4 * 3) - (strlen("Side:12") / 2), "Side:%2d", tempN);
        if(arrowX == 0){
            mvaddstr((max_y / 2) - 2, (max_x / 4) - (strlen("Mines:12") / 2) - 3, "->");
        }else if(arrowX == 1){
            mvaddstr((max_y / 2) - 2, (max_x / 4 * 3) - (strlen("Side:12") / 2) - 3, "->");
        }
        for(i = 0;i < 2;i++){
            mvaddstr((max_y / 2) + 5 + i, (max_x / 2) - (strlen(back_to_home[1]) / 2), back_to_home[i]);
        }
        
        move(max_y - 1, max_x - 1);  // カーソルは画面端に退避
        
        /*---入力を受け付け、それに応じた処理をする---*/
        ch = getch();
        switch(ch){
            case KEY_UP:      // カーソルキー(上)押下
            case 'w':         // 'w'キー押下
                if(arrowX == 0){        //-地雷の個数を変更する項目を選択中なら-
                    /*---MAX_MINE_NUMを超えない範囲で暫定的な地雷の個数をインクリメント---*/
                    if(tempMineNum < MAX_MINE_NUM){
                        tempMineNum++;
                    }
                }else if(arrowX == 1){  //-マップの1辺あたりのマス数を変更する項目を選択中なら-
                    /*---MAX_Nを超えない範囲で暫定的な1辺あたりのマス数をインクリメント---*/
                    if(tempN < MAX_N){
                        tempN++;
                    }
                }
                break;
                
            case KEY_DOWN:    // カーソルキー(下)押下
            case 's':         // 's'キー押下
                if(arrowX == 0){        //-地雷の個数を変更する項目を選択中なら-
                    /*---MIN_MINE_NUMを下回らない範囲で暫定的な地雷の個数をデクリメント---*/
                    if(tempMineNum > MIN_MINE_NUM){
                        tempMineNum--;
                    }
                }else if(arrowX == 1){  //-マップの1辺あたりのマス数を変更する項目を選択中なら-
                    /*---MIN_Nを下回らない範囲で暫定的な1辺あたりのマス数をデクリメント---*/
                    if(tempN > MIN_N){
                        tempN--;
                    }
                }
                break;
                
            case KEY_LEFT:   // カーソルキー(左)押下
            case 'a':        // 'a'キー押下
            case KEY_RIGHT:  // カーソルキー(右)押下
            case 'd':        // 'd'キー押下
                /*---arrowXの内容の0/1を切り替える---*/
                arrowX = (arrowX + 1) % 2;
                break;
                
            case '\n':       // エンターキー押下
                /*---暫定的な設定値を反映させる---*/
                setMineNum(diff, tempMineNum);
                setN(diff, tempN);
                enterFlag = 1;
                break;
                
            case 'x':        // 'x'キー押下
                /*---暫定的な設定値を反映させずに終了(キャンセル)---*/
                enterFlag = 1;
                break;
        }
    }while(enterFlag == 0);  // エンターキーが押されていない間繰り返す
    
    return;
}
