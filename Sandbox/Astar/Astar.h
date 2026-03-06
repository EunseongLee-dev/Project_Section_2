#pragma once

#include "Astar/Node.h"
#include "Level/ArenaLevel.h"
#include <vector>

namespace Wanted
{

	class Astar
	{
	public:
		// 길찾기 함수
		// 결과값으로 이동해야 할 좌표들의 리스트로 돌려줌
		static std::vector<Vector2> FindPath(
			Vector2 start,
			Vector2 end,
			ArenaLevel* arena);

	private:
		// hConst 비용 계산 함수
		static float GetHeuristic(Vector2 start, Vector2 end);

		// Open List 중에서 가장 총 비용이 저렴한 노드를 찾을 함수
		static Node* GetLeastCostNode(std::vector<Node*>& openList);

		// 이미 리스트(Open/Closed)에 특정 좌표의 노드가 있는지 체크 함수
		static Node* FindNodeinList(std::vector<Node*>& list, Vector2 pos);

	};

}

