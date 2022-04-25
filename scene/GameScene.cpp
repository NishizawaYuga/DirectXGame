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
	worldTransform_[PartId::Root].scale_ = {1.0f, 1.0f, 1.0f};

	// X , Y , Z 軸周りの回転角を設定
	worldTransform_[PartId::Root].rotation_ = {0, 0, 0};
	//※度数法での指定
	// worldTransform_.rotation_ = {0.0f, XMConvertToRadians(45.0f), 0.0f};

	// X , Y , Z 軸周りの平行移動を指定
	worldTransform_[PartId::Root].translation_ = {0, 5.0f, 0};

	//ワールドトランスフォームの初期化
	//キャラクターの大元
	worldTransform_[PartId ::Root].Initialize();

	//脊椎
	worldTransform_[PartId::Spine].translation_ = {0, -3.0f, 0};
	worldTransform_[PartId::Spine].parent_ = &worldTransform_[PartId::Root];
	worldTransform_[PartId::Spine].Initialize();

	//上半身
	//胸
	worldTransform_[PartId::Chest].translation_ = {0, 0.0f, 0};
	worldTransform_[PartId::Chest].parent_ = &worldTransform_[PartId::Root];
	worldTransform_[PartId::Chest].Initialize();

	//頭
	worldTransform_[PartId::Head].translation_ = {0, 3.0f, 0};
	worldTransform_[PartId::Head].parent_ = &worldTransform_[PartId::Chest];
	worldTransform_[PartId::Head].Initialize();

	//左腕
	worldTransform_[PartId::ArmL].translation_ = {-3.0f, 0.0f,0.0f};
	worldTransform_[PartId::ArmL].parent_ = &worldTransform_[PartId::Chest];
	worldTransform_[PartId::ArmL].Initialize();

	//右腕
	worldTransform_[PartId::ArmR].translation_ = {3.0, 0.0f, 0.0f};
	worldTransform_[PartId::ArmR].parent_ = &worldTransform_[PartId::Chest];
	worldTransform_[PartId::ArmR].Initialize();

	//下半身
	//尻
	worldTransform_[PartId::Hip].translation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_[PartId::Hip].parent_ = &worldTransform_[PartId::Spine];
	worldTransform_[PartId::Hip].Initialize();

	//左足
	worldTransform_[PartId::LegL].translation_ = {-3.0f, -3.0f, 0.0f};
	worldTransform_[PartId::LegL].parent_ = &worldTransform_[PartId::Hip];
	worldTransform_[PartId::LegL].Initialize();

	//右足
	worldTransform_[PartId::LegR].translation_ = {3.0f, -3.0f, 0.0f};
	worldTransform_[PartId::LegR].parent_ = &worldTransform_[PartId::Hip];
	worldTransform_[PartId::LegR].Initialize();

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();

	//サウンドデータの読み込み

	//音声再生
}

void GameScene::Update() {

	//キャラクター移動処理
	{
		//キャラクターの移動ベクトル
		XMFLOAT3 move = {0, 0, 0};

		//キャラクターの移動速度
		const float kCharacterSpeed = 0.2f;

		//押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_LEFT)) {
			move = {-kCharacterSpeed, 0, 0};
		} else if (input_->PushKey(DIK_RIGHT)) {
			move = {kCharacterSpeed, 0, 0};
		}

		//注視点移動（ベクトルの加算）
		worldTransform_[PartId::Root].translation_.x += move.x;
		worldTransform_[PartId::Root].translation_.y += move.y;
		worldTransform_[PartId::Root].translation_.z += move.z;

		//デバッグ用表示
		debugText_->SetPos(50, 150);
		debugText_->Printf(
		  "Root:(%f,%f,%f)", worldTransform_[PartId::Root].translation_.x,
		  worldTransform_[PartId::Root].translation_.y,
		  worldTransform_[PartId::Root].translation_.z);
	}

	worldTransform_[PartId::Root].UpdateMatrix();
	worldTransform_[PartId::Spine].UpdateMatrix();
	worldTransform_[PartId::Chest].UpdateMatrix();
	worldTransform_[PartId::Head].UpdateMatrix();
	worldTransform_ [PartId::ArmL].UpdateMatrix();
	worldTransform_ [PartId::ArmR].UpdateMatrix();
	worldTransform_ [PartId::Hip].UpdateMatrix();
	worldTransform_ [PartId::LegL].UpdateMatrix();
	worldTransform_ [PartId::LegR].UpdateMatrix();
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
	//model_->Draw(worldTransform_[PartId::Root], viewProjection_, textureHandle_);
	//model_->Draw(worldTransform_[PartId::Spine], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::Chest], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_ [PartId::Head], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_ [PartId::ArmL], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_ [PartId::ArmR], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_ [PartId::Hip], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_ [PartId::LegL], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_ [PartId::LegR], viewProjection_, textureHandle_);

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