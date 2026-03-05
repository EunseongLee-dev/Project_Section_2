#pragma once

#include "Actor/Actor.h"

using namespace Wanted;

// 전방 선언
class Player;

class Monster : public Actor
{
	RTTI_DECLARATIONS(Monster, Actor)

public:
	// 상태 확인 체크
	enum class State
	{
		Idle,
		Chase,
		Return
	};

	Monster();

	virtual void Tick(float deltaTime) override;
	virtual void BeginPlay() override;
	virtual void Reset() override;

	//virtual void Draw() override;

	// 몬스터 생성 위치를 저장하는 함수
	inline void SetSpawnPoint(const Vector2& pos) { spawnPoint = pos; }

private:
	// 거리측정 및 상태 업데이트 함수
	void UpdateState();

private:
	// 초기 상태 세팅
	State currentState = State::Idle;

	// 스폰 포인트(생성 위치) 저장
	Vector2 spawnPoint;

	// 플레이어 감지 거리
	float detectionRange = 10.0f;

	// Player 객체
	Player* targetPlayer = nullptr;

	// 프레임에 지나간 시간 체크 타이머
	float flashTimer = 0.0f;
	
	// 색상 정보 배열
	Color flashColors[3] = { Color::BrightRed, Color::BrightYellow, Color::LightBlue };

	// 색상 변경 인덱스
	int colorIndex = 0;
};

