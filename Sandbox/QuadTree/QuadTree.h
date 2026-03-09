#pragma once
#include "Math/Vector2.h"
#include <vector>

namespace Wanted
{

	// 좌표 및 크기 저장 구조체
	struct Rect
	{
		int x;
		int y;
		int width;
		int height;
	};

	// 전방 선언
	class Actor;

	class QuadTree
	{
	public:
		QuadTree(int x, int y, int w, int h);
		~QuadTree();

		// 액터 저장 재귀 함수
		bool Insert(Actor* actor);

		// 구역 나누기 함수
		void Subdivide();

		// 시각화 재귀 함수
		void GetBoundaries(std::vector<Rect>& list);

		//// 탐색 최적화 재귀 함수
		Actor* GetActorAt(Vector2 targetPos);

		// 영역 정보
		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;

		// 구역 안에 있는 액터
		std::vector<Actor*> actors;

		// 4개 자식 노드
		QuadTree* children[4] = { nullptr };
		bool isDivided = false;

	};

}

