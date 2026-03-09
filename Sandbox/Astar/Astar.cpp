#include "Astar.h"
#include <algorithm>

std::vector<Vector2> Wanted::Astar::FindPath(Vector2 start, Vector2 end, ArenaLevel* arena)
{
	// 메모리 누수 방어 리스트
	std::vector<Node*> allNodes;
	// 역추적 리스트 저장
	std::vector<Vector2> finalPath;
	// 탐색 해야할 노드 저장
	std::vector<Node*> Openlist;
	// 탐색이 끝난 노드 저장
	std::vector<Node*> Closedlist;

	// 출발점 노드 생성 및 초기화
	Node* startNode = new Node(start);
	allNodes.emplace_back(startNode);
	startNode->g = 0;
	startNode->h = GetHeuristic(start, end);
	startNode->UpdateF();

	// 첫 번째 후보지 등록
	Openlist.emplace_back(startNode);

	/*============= 1단계 =============*/

	// 탐색 시작 (후보지가 남아있는 동안 반복)
	while (!Openlist.empty())
	{
		// 낮은 f값을 저장할 변수
		Node* currentNode = GetLeastCostNode(Openlist);

		// 플레이어 위치(end)에 도달했을 경우
		if (currentNode->pos == end)
		{
			// 역추적 로직
			Node* temp = currentNode;
			
			// 부모로 거슬러 올라가기
			while (temp != nullptr)
			{
				// 현재 노드의 좌표를 리스트에 저장
				finalPath.emplace_back(temp->pos);

				// 부모 노드로 이동
				temp = temp->parent;
			}
			// 저장된 리스트 뒤집기
			std::reverse(finalPath.begin(), finalPath.end());

			break;
		}

		// 탐색이 끝난 노드는 OpenList에서 제거
		// find 함수로 위치 찾기
		auto it = std::find(
			Openlist.begin(), Openlist.end(), currentNode);
		
		// 삭제할 노드를 찾았을 경우
		if (it != Openlist.end())
		{
			Openlist.erase(it);
		}

		// ClosedList에 탐색 끝난 노드 추가
		Closedlist.emplace_back(currentNode);

		/*============= 2단계 =============*/

		// currentNode가 end에 도달하지 못했을 경우
		// 주변 8칸을 탐색하여 OpenList에 등록
		for (int dy = -1; dy <= 1; ++dy)
		{
			for (int dx = -1; dx <= 1; ++dx)
			{
				// 위치가 (0,0) 본인위치 임으로 continue
				if (dy == 0 && dx == 0)
				{
					continue;
				}

				// 주변 좌표 계산
				Vector2 neighborPos(
					currentNode->pos.x + dx, currentNode->pos.y + dy);

				// 해당 좌표가 벽인지 체크하여 벽이라면 건너뜀
				if (arena->IsWalkable(
					neighborPos.x, neighborPos.y) == false)
				{
					continue;
				}

				// 이미 조사된 칸 인지 확인후 조사끝난곳이면 건너뜀
				if (FindNodeinList(Closedlist, neighborPos) != nullptr)
				{
					continue;
				}

				/*============= 3단계 =============*/

				// 2단계를 넘겼다면 새로운 후보인지 기존 노드인지 체크

				// OpenList에서 해당 좌표를 가진 노드가 있는지 확인
				Node* openNode = FindNodeinList(Openlist, neighborPos);

				// 이동 비용 계산
				float moveCost = (abs(dx) + abs(dy) == 2) ? 1.414f : 1.0f;
				float nextG = currentNode->g + moveCost;

				// OpenList에 없는 위치인 경우
				if (openNode == nullptr)
				{
					// 새로운 노드 생성
					Node* newNode = new Node(neighborPos);
					allNodes.emplace_back(newNode);

					// 거리 비용 입력
					newNode->g = nextG;
					newNode->h = GetHeuristic(neighborPos, end);
					newNode->UpdateF();

					// 역추적 부모 설정
					newNode->parent = currentNode;

					// 리스트에 등록
					Openlist.emplace_back(newNode);
				}

				// OpenList에 이미 존재하는 위치인 경우
				else
				{
					// 이미 저장된 노드 보다 gCost가 더 짧은지 체크
					if (nextG < openNode->g)
					{
						// 새로 발견한 gCost값이 더 작은경우
						// 더 작은 g값으로 갱신
						openNode->g = nextG;
						// 부모 노드 설정
						openNode->parent = currentNode;
						// 갱신된 g값으로 다시 fCost 계산
						openNode->UpdateF();
					}
				}
			}
		}
	}
	// 메모리 해제
	for (Node* n : allNodes)
	{
		delete n;
	}
	allNodes.clear();

	// 최종 경로 반환
	return finalPath;
}

float Wanted::Astar::GetHeuristic(Vector2 start, Vector2 end)
{
	// 가로 거리 절대값 + 세로 거리 절대값
	return abs(start.x - end.x) + abs(start.y - end.y);
}

Node* Wanted::Astar::GetLeastCostNode(std::vector<Node*>& openList)
{
	// 예외 처리
	if (openList.empty())
	{
		return nullptr;
	}

	// 배열에서 가장 첫 노드를 우선 적용
	Node* bestNode = openList[0];

	for (int ix = 1; ix < openList.size(); ++ix)
	{
		// 만약 지금 보고있는 노드의 f값이 더 작을경우
		if (openList[ix]->f < bestNode->f)
		{
			// 더 작은 값을 bestNode에 갱신
			bestNode = openList[ix];
		}
	}
	// 최종적으로 가장 낮은 f값을 가진 노드를 반환
	return bestNode;
}

Node* Wanted::Astar::FindNodeinList(std::vector<Node*>& list, Vector2 pos)
{
	// 리스트에서 노드 포인터 순회
	for (Node* node : list)
	{
		// 노드 좌표와 찾을 좌표가 같은지 확인
		if (node->pos == pos)
		{
			// 찾았다면 해당 노드의 주소를 반환
			return node;
		}
	}
	// 찾지 못했다면 nullptr 반환
	return nullptr;
}

