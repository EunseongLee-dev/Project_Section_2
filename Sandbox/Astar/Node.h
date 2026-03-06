#pragma once

#include "Math/Vector2.h"

namespace Wanted
{
	struct Node
	{
		// 노드에 좌표값 저장
		Vector2 pos;

		// gCost: 시작점에서 현재 위치 까지의 이동 거리
		float g = 0;

		// hCost: 현재 위치에서 최종 목적지 까지 예상 거리
		float h = 0;

		// fCost: G + H의 합계 전체 예상 비용
		float f = 0;

		// 경로 역추적 용
		Node* parent = nullptr;

		// 생성자
		Node(Vector2 p)
			: pos(p), g(0), h(0), f(0), parent(nullptr)
		{
		}

		// 비용 갱신 함수
		// g와 h 값이 바뀌었을 때, f 값을 갱신
		inline void UpdateF() { f = g + h; }

		// 연산자 오버로딩
		// 노드 끼리 좌표값 비교
		inline bool operator ==(const Node& other) const
		{
			return pos == other.pos;
		}
	};
}


