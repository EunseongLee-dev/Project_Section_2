#include "player.h"
#include "Level/ArenaLevel.h"
#include "Core/Input.h"

Player::Player()
	: Actor(L"♞")
{
	this->color = Color::Yellow;
}

void Player::BeginPlay()
{

}

void Player::Tick(float deltaTime)
{
	// 현재 위치 가져오기
	Vector2 currentPos = GetPosition();

	// 이동 할 위치 계산
	Vector2 nextPos = currentPos;
	
	// 방향키 입력에 따른 다음 위치 계산
	// Todo: 추후에 방향조작을 GetKey로 변경(테스트용으로 Down사용)
	if (Input::Get().GetKeyDown(VK_UP))
	{
		nextPos.y -= 1;
	}
	if (Input::Get().GetKeyDown(VK_DOWN))
	{
		nextPos.y += 1;
	}
	if (Input::Get().GetKeyDown(VK_LEFT))
	{
		nextPos.x -= 1;
	}
	if (Input::Get().GetKeyDown(VK_RIGHT))
	{
		nextPos.x += 1;
	}

	// 방향 조작을 하지않았을 때
	if (nextPos == currentPos)
	{
		return;
	}

	// 형변환 체크
	ArenaLevel* arena = GetOwner()->As<ArenaLevel>();

	// 이동 하려는 위치에 액터 확인 후 이동 처리
	if (arena && arena->IsWalkable(nextPos.x, nextPos.y))
	{
		SetPosition(nextPos);
	}
}