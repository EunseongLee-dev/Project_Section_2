#pragma once

#include "Actor/Actor.h"

using namespace Wanted;

class Player : public Actor
{
	RTTI_DECLARATIONS(Player, Actor)

public:
	Player();
	
	virtual void BeginPlay() override;

	virtual void Tick(float deltaTime) override;

	virtual void Reset() override;

	// Setter
	inline void SetSpawnPoint(const Vector2& pos) { spawnPoint = pos; }

private:
	// 이동 속도 조절
	// 마지막 이동 후 얼마나 시간이 흘렀는지 체크
	float moveTimer = 0.0f;

	// 이동 속도를 조절할 시간 값
	float moveDelay = 0.13f;

	// 플레이어 스폰 위치 저장
	Vector2 spawnPoint;
};

