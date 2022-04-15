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
	float positionX = -40.0f;

	for (int i = 0; i < 18; i++) {
		// X , Y , Z 方向のスケーリングを設定
		worldTransform_[i].scale_ = {5.0f, 5.0f, 5.0f};

		// X , Y , Z 軸周りの回転角を設定
		//※度数法での指定
		worldTransform_[i].rotation_ = {0.0f, 0.0f, 0.0f};

		if (i < 9) {
			// X , Y , Z 軸周りの平行移動を指定
			worldTransform_[i].translation_ = {positionX, -20.0f, 0.0f};
			// 3Dモデルの位置をずらしていく
			positionX += 9.9f;
		} else {
			if (i == 9) {
				positionX = -40;
			}
			worldTransform_[i].translation_ = {positionX, 20.0f, 0.0f};
			positionX += 9.9f;
		}
	}

	//ワールドトランスフォームの初期化
	for (int i = 0; i < 18; i++) {
		worldTransform_[i].Initialize();
	}

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();

	//サウンドデータの読み込み

	//音声再生
}

void GameScene::Update() {

	//値を含んだ文字列

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
	for (int i = 0; i < 18; i++) {
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
