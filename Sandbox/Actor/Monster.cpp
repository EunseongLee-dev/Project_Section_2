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
	this->sortingOrder = 2;
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
	currentPath.clear();
}

void Monster::Draw()
{
	// 부모 Draw 함수 호출
	Actor::Draw();

	// 경로 시각화 (Chase & Return 상태인 경우)
	if (currentState == State::Chase || currentState == State::Return)
	{
		// 상태에 따른 출력 컬러
		Color pathColor = (currentState == State::Chase) ?
			Color::BrightYellow : Color::White;

		// 탐색 경로를 순회하며 점 찍기
		for (const Vector2& pathPos : currentPath)
		{
			// x좌표 값에 *2
			Vector2 visualPos = Vector2(pathPos.x * 2, pathPos.y);

			// 몬스터와 겹치는 위치는 패스
			if (pathPos == GetPosition())
			{
				continue;
			}

			Renderer::Get().Submit(L"·", visualPos, pathColor,1);
		}
	}
}

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
		break;
	}
	case State::Return:
	{
		// 플레이어와 일정거리 이상 멀어져 다시 복귀 할때
		this->color = Color::Blue;

		// 길 찾기 추가 타이머
		pathUpdateTimer += deltaTime;
		if (pathUpdateTimer >= pathUpdateDelay)
		{
			// 형변환 체크
			ArenaLevel* arena = GetOwner()->As<ArenaLevel>();

			if (arena)
			{
				// 목적지 spawnPoint
				currentPath = Astar::FindPath(
					GetPosition(),
					spawnPoint,
					arena);
			}
			// 타이머 초기화
			pathUpdateTimer = 0.0f;
		}
		break;
	}
	}
	// 이동처리 해야할 상태인 경우에만
	if (currentState != State::Idle)
	{
		// 실제 이동
		moveTimer += deltaTime;
		if (moveTimer >= moveDelay)
		{
			// 경로 탐색 함수 호출
			MoveAlongPath();

			moveTimer = 0.0f;
		}
	}
}

void Monster::UpdateState()
{
	// 예외 처리(플레이어가 없거나, 삭제 요청된 상태)
	if (targetPlayer == nullptr || targetPlayer->DestroyRequested())
	{
		// 몬스터 행동은 우선 대기 상태로
		currentState = State::Idle;
		return;
	}

	State oldState = currentState;

	// 몬스터와 플레이어의 거리 차이
	Vector2 diff = 
		targetPlayer->GetPosition() - this->GetPosition();

	// 거리 사이의 제곱 구하기
	float distSq = diff.LengthSq();

	// 이미 추격중인 경우
	if (currentState == State::Chase)
	{
		// 추격 중단 거리를 기준으로 체크
		if (distSq > loseRange * loseRange)
		{
			currentState = State::Return;
		}
	}
	else
	{
		// 대기중 이거나 복귀 중일 때는 플레이어 감지 범위로 체크
		if (distSq < detectionRange * detectionRange)
		{
			// 플레이어가 감지 범위안에 들어왔을 때
			currentState = State::Chase;
		}
	}

	// 스폰 포인터에 도달했는지 체크
	if (currentState == State::Return && GetPosition() == spawnPoint)
	{
		currentState = State::Idle;
	}

	// 로직 이후 상태를 비교
	if (oldState != currentState)
	{	
		// 이전 상태와 비교시 현재 상태가 변경되었으면 초기화
		currentPath.clear();
	}

}

void Monster::MoveAlongPath()
{
	// 경로가 있는지 확인 (추격: 2칸 / 나머지:1칸)
	int minPathSize = (currentState == State::Chase) ? 2 : 1;

	if (currentPath.size() > minPathSize)
	{
		Vector2 nextPos = currentPath[1];
		// 형변환 체크
		ArenaLevel* arena = GetOwner()->As<ArenaLevel>();

		if (arena)
		{
			// 쿼드트리로 충돌 체크
			if (arena->GetRootNode()->GetActorAt(nextPos) == nullptr)
			{
				SetPosition(nextPos);
				// 이동 이후 이전 위치 프레임은 제거
				currentPath.erase(currentPath.begin());
			}
		}
	}
}
