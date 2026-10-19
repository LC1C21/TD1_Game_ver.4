
#include <Novice.h>
#include <math.h>

const char kWindowTitle[] = "LC1C_17_ノガミ_アンジュ_タイトル";

// ウィンドウサイズ
const int kWindowWidth = 1920;
const int kWindowHeight = 1080;

// ノーツの速度設定
const float kNoteSpeed = 10.0f;       // 初期速度
const float kMaxNoteSpeed = 20.0f;    // 最高速度
const float kMinNoteSpeed = 3.0f;     // 最低速度

// ノーツの出現設定
const int kNotesCount = 12;
const int kNoteInterval = 30;

// 2次元座標
struct Vector2
{
    float x;
    float y;
};

// プレイヤーの情報
struct Player
{
    Vector2 position;       // 位置
    Vector2 velocity;       // 速度
    Vector2 acceleration;   // 加速度
    float radius;           // 半径
    float speed;            // 移動速度（現在未使用）
};

// 地面の線
struct Line
{
    Vector2 start;
    Vector2 end;
};

// ノーツの情報
struct Notes
{
    Vector2 position;   // 位置
    float radius;       // 半径
    float speed;        // 移動速度
    int hitFlag;        // 出現・判定状態
};

// 判定用の心臓
struct Heart
{
    Vector2 position;
    float radius;
    int hitFlag;
};

// プレイヤーのジャンプ処理
void Jump(Player& player)
{
    // 速度を更新
    player.velocity.x += player.acceleration.x;
    player.velocity.y += player.acceleration.y;

    // 位置を更新
    player.position.x += player.velocity.x;
    player.position.y += player.velocity.y;

    // 地面より下に行かないようにする
    if (player.position.y <= player.radius)
    {
        player.position.y = player.radius;
        player.velocity.y = 0.0f;
    }
}

// ノーツと心臓の当たり判定
bool IsAlive(Notes& note, Heart& heart)
{
    float dx = note.position.x - heart.position.x;
    float dy = note.position.y - heart.position.y;

    float distance = sqrtf(dx * dx + dy * dy);

    return distance <= note.radius + heart.radius;
}

// ノーツ全体の速度を変更する
void ChangeNoteSpeed(
    Notes notes[],
    int count,
    float& currentSpeed,
    float change
)
{
    currentSpeed += change;

    // 速度の上限
    if (currentSpeed > kMaxNoteSpeed)
    {
        currentSpeed = kMaxNoteSpeed;
    }

    // 速度の下限
    if (currentSpeed < kMinNoteSpeed)
    {
        currentSpeed = kMinNoteSpeed;
    }

    // すべてのノーツに速度を反映
    for (int i = 0; i < count; i++)
    {
        notes[i].speed = currentSpeed;
    }
}

// ノーツの移動とミス判定
void NoteMove(
    Notes notes[],
    int count,
    Heart& heart,
    float& currentSpeed
)
{
    for (int i = 0; i < count; i++)
    {
        // 出現していないノーツは処理しない
        if (notes[i].hitFlag != 0) continue;

        // 左へ移動
        notes[i].position.x -= notes[i].speed;

        // 判定位置を通り過ぎたらミス
        if (notes[i].position.x + notes[i].radius
            < heart.position.x - heart.radius)
        {
            // ミスしたノーツを消す
            notes[i].hitFlag = 1;

            // 失敗したので速度を1下げる
            ChangeNoteSpeed(
                notes, count, currentSpeed, -1.0f
            );
        }
    }
}

// ノーツに当たったか判定し、ジャンプさせる
void CheckHitAndJump(
    Notes notes[], int count,
    Heart& heart, Player& player,
    const char* keys, const char* preKeys
)
{
    for (int i = 0; i < count; i++)
    {
        // 出現中ではないノーツは判定しない
        if (notes[i].hitFlag != 0) continue;

        // ノーツと心臓が重なっているか確認
        if (IsAlive(notes[i], heart))
        {
            // スペースキーを押した瞬間
            if (keys[DIK_SPACE] && !preKeys[DIK_SPACE])
            {
                notes[i].hitFlag = 1;      // ノーツを消す
                player.velocity.y = 20.0f; // ジャンプ
            }
        }
    }
}

// ゲームの開始地点
int WINAPI WinMain(_In_ HINSTANCE,_In_opt_ HINSTANCE,_In_ LPSTR,_In_ int)
{
    Novice::Initialize(kWindowTitle,kWindowWidth,kWindowHeight);

    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

    // プレイヤーの初期設定
    Player player{
        {960.0f, 50.0f},
        {0.0f, 20.0f},
        {0.0f, -0.8f},
        50.0f,
        0.1f
    };

    // 地面の設定
    Line line{
        {0.0f, 900.0f},
        {kWindowWidth, 900.0f}
    };
    
  // =====================================
  // ノーツの設定
  // =====================================

    // 次のノーツまでの経過時間
    int noteTimer = 0;  
    // 次に出現させるノーツ番号
    int noteIndex = 1;   
    // 現在のノーツ速度
    float currentNoteSpeed = kNoteSpeed;

    // ノーツの初期配置
    // hitFlag：0 = 出現中、-1 = 待機中、1 = 消去済み
    Notes notes[kNotesCount] = {
        // 1セット目
        {{kWindowWidth, 960.0f}, 50.0f, kNoteSpeed, 0}, //Red
        {{kWindowWidth, 960.0f}, 15.0f, kNoteSpeed, -1},
        {{kWindowWidth, 960.0f}, 15.0f, kNoteSpeed, -1},
        {{kWindowWidth, 960.0f}, 15.0f, kNoteSpeed, -1},

        // 2セット目
        {{kWindowWidth, 960.0f}, 50.0f, kNoteSpeed, -1}, //Red
        {{kWindowWidth, 960.0f}, 15.0f, kNoteSpeed, -1},
        {{kWindowWidth, 960.0f}, 15.0f, kNoteSpeed, -1},
        {{kWindowWidth, 960.0f}, 15.0f, kNoteSpeed, -1},

        // 3セット目
        {{kWindowWidth, 960.0f}, 50.0f, kNoteSpeed, -1}, //Red
        {{kWindowWidth, 960.0f}, 15.0f, kNoteSpeed, -1},
        {{kWindowWidth, 960.0f}, 15.0f, kNoteSpeed, -1},
        {{kWindowWidth, 960.0f}, 15.0f, kNoteSpeed, -1}
    };

    // 判定用の心臓
    Heart heart{
        {960.0f, 960.0f},
        50.0f,
        0
    };

    // ゲームループ
    while (Novice::ProcessMessage() == 0)
    {
        Novice::BeginFrame();

        // キー入力を取得
        memcpy(preKeys, keys, 256);
        Novice::GetHitKeyStateAll(keys);

        // =====================================
        // 更新処理
        // =====================================

        // 一定時間ごとに次のノーツを出現させる
        noteTimer++;

        if (noteTimer >= kNoteInterval)
        {
            // 次のノーツを画面右端に配置
            notes[noteIndex].position.x =
                static_cast<float>(kWindowWidth);

            // ノーツを出現状態にする
            notes[noteIndex].hitFlag = 0;

            // 次のノーツ番号へ進む
            noteIndex++;

            // 12個目の次は0番に戻る
            if (noteIndex >= kNotesCount)
            {
                noteIndex = 0;
            }

            // タイマーをリセット
            noteTimer = 0;
        }

        // ノーツを移動させる
        NoteMove(notes, kNotesCount);

        // 当たり判定とジャンプ
        CheckHitAndJump(
            notes, kNotesCount,
            heart, player, keys, preKeys
        );

        // プレイヤーの位置を更新
        Jump(player);

        // =====================================
        // 描画処理
        // =====================================

        // 地面を描画
        Novice::DrawLine(
            (int)line.start.x, (int)line.start.y,
            (int)line.end.x, (int)line.end.y,
            WHITE
        );

        // プレイヤーを描画
        Novice::DrawEllipse(
            (int)player.position.x,
            (int)(player.position.y * -1 + 900),
            (int)player.radius,
            (int)player.radius,
            0.0f, BLACK, kFillModeSolid
        );

        // 心臓を描画
        Novice::DrawEllipse(
            (int)heart.position.x,
            (int)heart.position.y,
            (int)heart.radius,
            (int)heart.radius,
            0.0f, WHITE, kFillModeSolid
        );

        // 出現中のノーツだけ描画
        for (int i = 0; i < kNotesCount; i++)
        {
            if (notes[i].hitFlag != 0) continue;

            // 大きい円は赤、小さい円は青
            int color = (notes[i].radius > 20) ? RED : BLUE;

            Novice::DrawEllipse(
                (int)notes[i].position.x,
                (int)notes[i].position.y,
                (int)notes[i].radius,
                (int)notes[i].radius,
                0.0f, color, kFillModeSolid
            );
        }

        // =====================================
        // デバッグ表示
        // =====================================

        // タイマーの進行状況
        Novice::ScreenPrintf(
            10, 10,
            "Timer: %d / %d",
            noteTimer, kNoteInterval
        );

        // 出現中のノーツ数を数える
        int activeNoteCount = 0;

        for (int i = 0; i < kNotesCount; i++)
        {
            if (notes[i].hitFlag == 0)
            {
                activeNoteCount++;
            }
        }

        // 出現中のノーツ数
        Novice::ScreenPrintf(
            10, 35,
            "Active Notes: %d / %d",
            activeNoteCount, kNotesCount
        );

        // 次に出現するノーツ番号
        Novice::ScreenPrintf(
            10, 60,
            "Next Note Index: %d",
            noteIndex
        );

        // 全ノーツの座標と状態
        for (int i = 0; i < kNotesCount; i++)
        {
            Novice::ScreenPrintf(
                10, 100 + i * 25,
                "note[%d] X = %d  hitFlag = %d",
                i,
                static_cast<int>(notes[i].position.x),
                notes[i].hitFlag
            );
        }

        Novice::EndFrame();

        // ESCキーでゲーム終了
        if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0)
        {
            break;
        }
    }

    Novice::Finalize();
    return 0;
}
