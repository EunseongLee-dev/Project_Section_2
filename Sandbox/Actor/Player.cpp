#include "player.h"
#include "Level/ArenaLevel.h"
#include "Core/Input.h"
#include "Monster.h"

Player::Player()
	: Actor(L"♞")
	//: Actor(L"⚔")
{
	this->color = Color::BrightYellow;
	this->sortingOrder = 100;
}

void Player::BeginPlay()
{

}

void Player::Reset()
{
	SetPosition(spawnPoint);
}


void Player::Tick(float deltaTime)
{
	// 시간 누적 체크
	moveTimer += deltaTime;

	// 이동 가능 쿨타임 체크하여 예외 처리
	if (moveTimer < moveDelay)
	{
		return;
	}

	// 현재 위치 가져오기
	Vector2 currentPos = GetPosition();

	// 이동 할 위치 계산
	Vector2 nextPos = currentPos;
	
	// 방향키 입력에 따른 다음 위치 계산
	if (Input::Get().GetKey(VK_UP))
	{
		nextPos.y -= 1;
	}
	if (Input::Get().GetKey(VK_DOWN))
	{
		nextPos.y += 1;
	}
	if (Input::Get().GetKey(VK_LEFT))
	{
		nextPos.x -= 1;
	}
	if (Input::Get().GetKey(VK_RIGHT))
	{
		nextPos.x += 1;
	}

	// 방향 조작을 하지않았을 때
	if (nextPos != currentPos)
	{
		// 형변환 체크
		ArenaLevel* arena = GetOwner()->As<ArenaLevel>();

		if (arena)
		{
			// 벽 & 장애물이 있는지 확인
			bool canWalk = arena->IsWalkable(nextPos.x, nextPos.y);

			// 다른 액터가 없는지 체크
			bool noActor = (arena->GetRootNode()->GetActorAt(nextPos) == nullptr);

			// 조건 만족 시 이동 처리
			if (canWalk && noActor)
			{
				SetPosition(nextPos);
				moveTimer = 0.0f;
			}
		}
	}

	// 플레이어 광역 공격
	if (Input::Get().GetKeyDown(VK_SPACE))
	{
		// 형변환 체크
		ArenaLevel* arena = GetOwner()->As<ArenaLevel>();
		
		if (arena && arena->GetRootNode())
		{
			// 플레이어 주변 8방향 탐색
			for (int iy = -1; iy <= 1; ++iy)
			{
				for (int ix = -1; ix <= 1; ++ix)
				{
					// 플레이어 본인 제외
					if (ix == 0 && iy == 0)
					{
						continue;
					}

					// 공격 좌표 계산
					Vector2 targetPos = GetPosition() + Vector2(ix, iy);

					// 쿼드트리에 액터 탐색
					Actor* target = arena->GetRootNode()->GetActorAt(targetPos);

					if (target)
					{
						// 몬스터인지 체크하여 처리
						Monster* monster = target->As<Monster>();
						// 조건 체크
						if (monster)
						{
							// 랜덤 생성된 액터인지 확인
							if (monster->IsGenerated())
							{
								// 랜덤 생성된 액터라면 삭제 함수 호출
								monster->Destroy();
							}
							else
							{
								// 원래 맵에 있던 몬스터는 생성 위치로 이동
								monster->Reset();
							}
						}
					}
				}
			}
		}
	}
}

