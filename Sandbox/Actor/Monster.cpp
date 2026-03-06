#include "Monster.h"
#include "Level/Level.h"
#include "Actor/Player.h"
#include "Level/ArenaLevel.h"
#include "Render/Renderer.h"
#include "Astar/Astar.h"



Monster::Monster()
	: Actor(L"★")
	//: Actor(L"☠")
{
	this->color = Color::Red;
}

void Monster::BeginPlay()
{
	// 부모 클래스 호출
	Actor::BeginPlay();

	// 형변환 체크
	ArenaLevel* arena = GetOwner()->As<ArenaLevel>();

	// 플레이어 주소 반환
	if (arena)
	{
		targetPlayer = arena->GetPlayer();
	}
	
}

void Monster::Reset()
{
	// 초기 생성 위치로 설정 및 상태 변화
	SetPosition(spawnPoint);
	currentState = State::Idle;
}

//void Monster::Draw()
//{
//	// 부모 Draw 함수 호출
//	Actor::Draw();
//
//	// 버퍼 에러 방지
//	if (position.y <= 0)
//	{
//		return;
//	}
//
//	// 상태에 따른 출력 설정
//	const wchar_t* emotion = L"";
//	Color emotionColor = Color::White;
//
//	//if (currentState == State::Idle)
//	//{
//	//	emotion = L"？";
//	//	emotionColor = Color::White;
//	//}
//	if (currentState == State::Chase)
//	{
//		emotion = L"！";
//		emotionColor = Color::Red;
//	}
//	//else if (currentState == State::Return)
//	//{
//	//	emotion = L"．．";
//	//	emotionColor = Color::DarkGray;
//	//}
//
//	// 몬스터 머리 위(y - 1)에 그리기
//	Vector2 headPos = Vector2(position.x * 2, position.y - 1);
//	Renderer::Get().Submit(emotion, headPos, emotionColor);
//}

void Monster::Tick(float deltaTime)
{
	
	// 프레임 마다 거리를 측정하여 상태 업데이트
	UpdateState();

	switch (currentState)
	{
	case State::Idle:
	{
		// 플레이어를 인식하지 않은 상태
		this->color = Color::Red;
		break;
	}
	case State::Chase:
	{
		// RGB 색상 조절
		// 색상 변경을 위해 flashTimer 값 누적 갱신
		flashTimer += deltaTime;

		// 0.2초가 지났는지 확인 (깜빡임 속도)
		if (flashTimer >= 0.2f)
		{
			// 다음 색상 번호로 넘기기
			colorIndex++;
			
			// 색상 변경 맴버 값이 2를 넘어가면 다시 0으로 세팅
			if (colorIndex > 2)
			{
				colorIndex = 0;
			}

			// 번호에 맞춰서 실제 색상 변경
			this->color = flashColors[colorIndex];

			// 타이머를 다시 0으로 세팅
			flashTimer = 0.0f;
		}

		// A* 경로 갱신
		pathUpdateTimer += deltaTime;
		// 딜레이 설정 값 보다 클 때만 A* 갱신
		if (pathUpdateTimer >= pathUpdateDelay)
		{
			// 형변환 체크
			ArenaLevel* arena = GetOwner()->As<ArenaLevel>();
			
			// 형변환 성공 했다면
			if (arena)
			{
				// A* 호출하여 경로 저장
				currentPath = Astar::FindPath(
					GetPosition(),
					targetPlayer->GetPosition(),
					arena);
			}
			// 타이머 시간 초기화
			pathUpdateTimer = 0.0f;
		}

		// 실제 거리 이동
		moveTimer += deltaTime;
		// 딜레이 값보다 클 때만 이동
		if (moveTimer >= moveDelay)
		{
			// 경로를 찾았고, 내 다음 인덱스가 있다면 이동
			if (currentPath.size() > 1)
			{
				SetPosition(currentPath[1]);
			}
			// 타이머 초기화
			moveTimer = 0.0f;
		}
		break;
	}
	case State::Return:
	{
		// 플레이어와 일정거리 이상 멀어져 다시 복귀 할때
		this->color = Color::Blue;
		break;
	}
	}
}

void Monster::UpdateState()
{
	// 예외 처리
	if (!targetPlayer)
	{
		return;
	}

	// 몬스터와 플레이어의 거리 차이
	Vector2 diff = 
		targetPlayer->GetPosition() - this->GetPosition();

	// 거리 사이의 제곱 구하기
	float distSq = diff.LengthSq();

	// 플레이어 감지 체크
	if (distSq < detectionRange * detectionRange)
	{
		// 플레이어가 감지 범위안에 들어왔을 때
		currentState = State::Chase;
	}
	else
	{
		// 플레이어가 멀어졌을 때

		// 1. 몬스터가 리스폰 위치에 이미 도달해 있다면
		if (this->GetPosition() == spawnPoint)
		{
			currentState = State::Idle;
		}
		// 복귀 중인 경우
		else
		{
			currentState = State::Return;	
		}
	}

}
