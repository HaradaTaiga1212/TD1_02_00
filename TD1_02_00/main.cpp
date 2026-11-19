#include <Novice.h>
#include <string.h>

const char kWindowTitle[] = "LC1B_17_ハラダ_タイガ_タイトル";

// プレイヤー構造体
struct Player {
	float posX;
	float posY;
	float speedX;
	float speedY;
	float width;
	float height;
	bool isGravityReversed; // 0: 通常(下重力), 1: 反転(上重力)
};

// 壁（障害物）構造体
struct Wall {
	float posX;
	float posY;
	float width;
	float height;
	int isTop; // 1: 上の壁, 0: 下の壁
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// 画面サイズ定義
	const int kWindowWidth = 1280;
	const int kWindowHeight = 720;

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };  

	// --- 定数パラメータ ---
	const float kGravity = 1.2f;       // 重力加速度
	const float kFloorY = 620.0f;       // 下の床の位置
	const float kCeilingY = 100.0f;     // 上の天井の位置

	// --- ゲーム変数 ---
	Player player = {
		100.0f,     // posX
		570.0f,     // posY (初期位置は下の床上)
		6.0f,       // speedX (強制スクロール速度)
		0.0f,       // speedY	
		50.0f,      // width
		50.0f,      // height
		false           // isGravityReversed (通常)
	};

	// マップ上の壁配置 (プレイヤーの進行方向右側へ配置)
	const int kMaxWalls = 5;
	Wall walls[kMaxWalls] = {
		{ 600.0f,  kCeilingY, 80.0f, 150.0f, 1 }, // 上の壁
		{ 900.0f,  kFloorY - 150.0f, 80.0f, 150.0f, 0 }, // 下の壁
		{ 1300.0f, kCeilingY, 100.0f, 200.0f, 1 },
		{ 1700.0f, kFloorY - 180.0f, 80.0f, 180.0f, 0 },
		{ 2100.0f, kCeilingY, 120.0f, 220.0f, 1 }
	};

	int isGameOver = 0;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		if (!isGameOver) {
			//強制横移動（右へ進む）
			player.posX += player.speedX;

			// 2. スペースキー入力による重力反転
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
				player.isGravityReversed = !player.isGravityReversed;
			}

			// 3. 重力の適用
			if (player.isGravityReversed) {
				player.speedY -= kGravity; // 上方向へ加速
			}
			else {
				player.speedY += kGravity; // 下方向へ加速
			}
			player.posY += player.speedY;

			// 4. 天井・床との当たり判定＆めり込み補正
			// 下の床
			if (player.posY + player.height >= kFloorY) {
				player.posY = kFloorY - player.height;
				player.speedY = 0.0f;
			}
			// 上の天井
			if (player.posY <= kCeilingY) {
				player.posY = kCeilingY;
				player.speedY = 0.0f;
			}

			// 5. 壁との衝突判定 (AABB矩形判定)
			for (int i = 0; i < kMaxWalls; i++) {
				if (player.posX < walls[i].posX + walls[i].width &&
					player.posX + player.width > walls[i].posX &&
					player.posY < walls[i].posY + walls[i].height &&
					player.posY + player.height > walls[i].posY) {

					isGameOver = 1; // 障害物に衝突したらゲームオーバー
				}
			}
		}

		// カメラのオフセット計算（自機を中心に捉えるスクロール補正値）
		float cameraOffsetX = player.posX - 200.0f;

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		// 天井と床の背景ライン
		Novice::DrawBox(0, 0, kWindowWidth, (int)kCeilingY, 0.0f, 0x333333FF, kFillModeSolid);
		Novice::DrawBox(0, (int)kFloorY, kWindowWidth, kWindowHeight - (int)kFloorY, 0.0f, 0x333333FF, kFillModeSolid);

		// 壁（障害物）の描画（スクロール移動を反映）
		for (int i = 0; i < kMaxWalls; i++) {
			Novice::DrawBox(
				(int)(walls[i].posX - cameraOffsetX),
				(int)walls[i].posY,
				(int)walls[i].width,
				(int)walls[i].height,
				0.0f, RED, kFillModeSolid
			);
		}

		// 自機の描画（重力反転中は緑、通常時は青）
		Novice::DrawBox(
			(int)(player.posX - cameraOffsetX),
			(int)player.posY,
			(int)player.width,
			(int)player.height,
			0.0f, player.isGravityReversed ? GREEN : BLUE, kFillModeSolid
		);

		// ゲームオーバー表示
		if (isGameOver) {
			Novice::ScreenPrintf(580, 350, "GAME OVER");
		}

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}