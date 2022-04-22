#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

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

	//並べる用の変数
	float positionX = 0.0f;
	float positionY = 5.0f;
	float speed = 0.2f;

	for (int i = 0; i < 10; i++) {

		//X , Y , Z のスケールを設定
		worldTransform_[i].scale_ = {1.0f, 1.0f, 1.0f};
		
		// X , Y , Z 軸周りの回転角を設定
		//※度数法での指定
		worldTransform_[i].rotation_ = {0.0f, 0.0f, 0.0f};

		// X , Y , Z 軸周りの平行移動を指定
		worldTransform_[i].translation_ = {positionX, positionY, 0.0f};
	}

	//ワールドトランスフォームの初期化
	for (int i = 0; i < 10; i++) {
			worldTransform_[i].Initialize();
	}

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();

	//回転開始地点の設定
	for (int i = 0; i < 10; i++) {
		posAngle[i] += 0.7f * i;
	}

	//サウンドデータの読み込み

	//音声再生
}

void GameScene::Update() {

	//値を含んだ文字列
	const float objectSpeed = 0.03f;

	for (int i = 0; i < 10; i++) {
		posAngle[i] += objectSpeed;
		posAngle[i] = fmodf(posAngle[i], XM_2PI);

		worldTransform_[i].translation_ = {cosf(posAngle[i]) * 10, sinf(posAngle[i]) * 10, 0.0f};

		worldTransform_[i].UpdateMatrix();
	}

	//デバックテキストの表示
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
	for (int i = 0; i < 10; i++) {
			model_->Draw(worldTransform_[i], viewProjection_, textureHandle_);
	}

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
