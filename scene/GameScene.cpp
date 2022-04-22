#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() { delete model_; }

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	//ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");

	// 3Dモデルの生成
	model_ = Model::Create();

	//乱数シード生成器
	std::random_device seed_gen;

	//メルセンヌ・ツイスター
	std::mt19937_64 engine(seed_gen());

	//乱数範囲(回転角用)
	std::uniform_real_distribution<float> rotDist(0.0f, XM_2PI);

	//乱数範囲(座標用)
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);

		// X , Y , Z 方向のスケーリングを設定
		worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

		// X , Y , Z 軸周りの回転角を設定
		worldTransform_.rotation_ = {0,0,0};
		//※度数法での指定
		// worldTransform_.rotation_ = {0.0f, XMConvertToRadians(45.0f), 0.0f};

		// X , Y , Z 軸周りの平行移動を指定
		worldTransform_.translation_ = {0,0,0};

		//ワールドトランスフォームの初期化
		worldTransform_.Initialize();

	for (size_t i = 0; i < _countof(viewProjection_); i++) {
		//カメラ視点座標を設定
		viewProjection_[i].eye = {posDist(engine), posDist(engine), posDist(engine)};

		//カメラ注視点座標を設定
		viewProjection_[i].target = {0, 0, 0};

		//カメラ上方向ベクトルを設定
		viewProjection_[i].up = {rotDist(engine), rotDist(engine), rotDist(engine)};

		//ビュープロジェクションの初期化
		viewProjection_[i].Initialize();
	}

	//サウンドデータの読み込み

	//音声再生
}

void GameScene::Update() {

		//押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_SPACE)) {
		    cameraNumber++;
		    if (cameraNumber > 2) {
			    cameraNumber = 0;
			}
		}

		//デバッグ用表示
		debugText_->SetPos(50, 90);
		debugText_->Printf(
		  "up:(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	// sprite_->Draw();//Draw()はPreDraw()とPosrDraw()の間に絶対書くこと

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// 3Dモデル描写
	model_->Draw(worldTransform_, viewProjection_[cameraNumber], textureHandle_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
