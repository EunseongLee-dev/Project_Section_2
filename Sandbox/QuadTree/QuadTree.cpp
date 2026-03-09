#include "QuadTree.h"
#include "Actor/Actor.h"

Wanted::QuadTree::QuadTree(int x, int y, int w, int h)
    :x(x), y(y), width(w), height(h), isDivided(false)
{
}

Wanted::QuadTree::~QuadTree()
{
    // 구역이 분할된 경우
    if (isDivided)
    {
        for (int ix = 0; ix < 4; ++ix)
        {
            // 자식 노드를 delete
            // 이때 각 자식 노드의 소멸자가 실행되면서 하위 까지 delete됨
            delete children[ix];
            children[ix] = nullptr;
        }
    }
    // 액터 리스트 비우기
    actors.clear();
}

bool Wanted::QuadTree::Insert(Actor* actor)
{
    // 담당 구역 확인
    Vector2 pos = actor->GetPosition();
    if (pos.x < x || pos.x >= x + width ||
        pos.y < y || pos.y >= y + height)
    {
        return false;
    }

    // 자식 노드에 자리가 있고, 구역이 나누어지지 않았을 경우
    if (actors.size() < 4 && !isDivided)
    {
        actors.emplace_back(actor);
        return true;
    }

    // 자식 노드에 자리가 없으면 다시 나눈후 넘긴다
    if (!isDivided)
    {
        // 구역 분할 및 생성 함수 호출
        Subdivide();

        // 소유한 액터를 자식에게 전달
        for (Actor* a : actors)
        {
            for (int ix = 0; ix < 4; ++ix)
            {
                if (children[ix]->Insert(a))
                {
                    break;
                }
            }
        }
        // 기존 액터 초기화
        actors.clear();
    }

    // 새 액터를 자식들 중 한명에게 전달
    for (int ix = 0; ix < 4; ++ix)
    {
        if (children[ix]->Insert(actor))
        {
            return true;
        }
    }
    return false;
}

void Wanted::QuadTree::GetBoundaries(std::vector<Rect>& list)
{
    // 현재 구역 정보를 Rect로 생성후 list에 저장
    Rect myRect = { x, y, width, height };
    list.emplace_back(myRect);

    // 현재 구역이 분할되었다면 자식 노드 정보 가져오기
    if (isDivided)
    {
        for (int ix = 0; ix < 4; ++ix)
        {
            children[ix]->GetBoundaries(list);
        }
    }
}

Wanted::Actor* Wanted::QuadTree::GetActorAt(Vector2 targetPos)
{
    if (targetPos.x < x || targetPos.x >= x + width ||
        targetPos.y < y || targetPos.y >= y + height)
    {
        // 구역이 아닌경우 반환
        return nullptr;
    }

    // 구역이 나누어진 상태인 경우 재귀 호출
    if (isDivided)
    {
        for (int ix = 0; ix < 4; ++ix)
        {
            // 자식 노드에 액터가 있는지 확인
            Actor* result = children[ix]->GetActorAt(targetPos);

            // 자식 노드에 액터가 있는 경우
            if (result)
            {
                return result;
            }
        }
    }
    // 구역이 나누어지지 않은 상태인 경우 본인 액터만 탐색
    else
    {
        for (Actor* a : actors)
        {
            if (a->GetPosition() == targetPos)
            {
                return a;
            }
        }
    }
    // 아예 없는 경우 반환
    return nullptr;
}


void Wanted::QuadTree::Subdivide()
{
    // 예외 처리
    if (width <= 1 || height <= 1)
    {
        return;
    }

    int nw = width / 2;
    int remainingW = width - nw; // 충돌 판정 처리 정교화
    int nh = height / 2;
    int remainingH = height - nh; // 충돌 판정 처리 정교화

    // 4개의 자식 노드 생성
    children[0] = new QuadTree(x, y, nw, nh); // 왼쪽 위(NW)
    children[1] = new QuadTree(x + nw, y, remainingW, nh); // 오른쪽 위(NE)
    children[2] = new QuadTree(x, y + nh, nw, remainingH); // 왼쪽 아래(SW)
    children[3] = new QuadTree(x + nw, y + nh, remainingW, remainingH); // 오른쪽 아래(SE)

    // 구역이 나눠졌으니 상태 변경
    isDivided = true;
}

