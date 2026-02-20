#pragma once

#include "Common/RTTI.h"
#include "Math/Vector2.h"

// STL(Standard Template Library - C++에서 기본 제공).
// 크기가 알아서 변경되는 배열.
#include <vector>

namespace Wanted
{
	// 전방 선언.
	class Actor;

	// 담당 임무: 레벨에 있는 모든 액터(물체) 관리.
	class WANTED_API Level : public RTTI
	{
		RTTI_DECLARATIONS(Level, RTTI)

	public:
		Level();
		virtual ~Level();

		// 게임 플레이 이벤트.
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

		// 액터 추가 함수.
		void AddNewActor(Actor* newActor);

		// 액터 추가/제거 처리 함수.
		void ProcessAddAndDestroyActors();

		// 벽 확인 처리용 함수
		Actor* GetActorAt(const Vector2& position);

		// 맵 사이즈 반환 함수
		virtual Vector2 GetRenderSize() const;

		// 카메라 고정 함수
		virtual Vector2 GetCameraTarget() const;


	protected:
		// 액터 배열.
		std::vector<Actor*> actors;

		// 실행 중에 추가 요청된 액터의 배열.
		std::vector<Actor*> addRequestedActors;
	};
}