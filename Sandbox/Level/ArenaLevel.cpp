#include "ArenaLevel.h"
#include "Actor/Player.h"
#include "Actor/Monster.h"
#include "Render/Renderer.h"
#include "Core/Input.h"

#include <fstream>
#include <iostream>
#include <vector>

ArenaLevel::ArenaLevel()
{
}

ArenaLevel::~ArenaLevel()
{
}

bool ArenaLevel::LoadMap(const wchar_t* filename)
{
    // 맵 파일 로드
    std::wifstream load(filename);

    // 예외 처리
    if (!load.is_open())
    {
        std::cout << "맵 로드 실패\n";
        return false;
    }
    
    std::wstring line;
    int y = 0;

    while (std::getline(load, line))
    {
        for (int ix = 0; ix < line.size(); ++ix)
        {
            // wstring 문자열에서 wchar_t로 문자 하나 저장
            wchar_t tile = line[ix];

            switch (tile)
            {
            case L'#':
            {
                line[ix] = L'▩';
                
                break;
            }
            case L'X':
            {
                line[ix] = L'※';
                break;

            }
            case L'P':
            {
                // 플레이어 객체 생성
                mainPlayer = new Player();

                // 플레이어 위치 설정
                mainPlayer->SetPosition(Vector2(ix, y));

                // 플레이어 스폰 포인트 설정
                mainPlayer->SetSpawnPoint(Vector2(ix, y));

                // 플레이어 객체 등록
                this->AddNewActor(mainPlayer);

                // 기존 위치 정보를 공백으로 바꾸기
                line[ix] = L' ';
                break;
            }
   
            case L'M':
            {
                // 몬스터 객체 생성
                Monster* monster = new Monster();
                
                // 몬스터 위치 설정
                monster->SetPosition(Vector2(ix, y));
                
                // 몬스터 스폰 포인트 설정
                monster->SetSpawnPoint(Vector2(ix, y));

                // 몬스터를 배열에 저장
                this->AddNewActor(monster);

                // 기존 위치 정보를 공백으로 바꾸기
                line[ix] = L' ';
                break;
            }

            case L' ':
            {
                line[ix] = L' ';
                break;
            }
            }
        }
        // 벽(#) 및 장애물(X)은 그대로 데이터에 저장
        // line 한줄 처리후 y좌표 증가
        mapData.emplace_back(line);
        ++y;
    }
    // 맵 크기 저장
    mapHeight = mapData.size();
    if (mapHeight > 0)
    {
        mapWidth = mapData[0].size();
    }

    // 파일 닫기
    load.close();

    return true;
}

void ArenaLevel::Draw()
{
    // 기본 버퍼 세팅
    wchar_t tileStr[2] = { L' ', L'\0' };

    // 카메라가 비추는 시작점 부터 끝점까지만 루프
    for (int y = cameraPos.y; y < cameraPos.y + viewHeight; ++y)
    {
        // 맵 범위를 벗어나지 않도록
        if (y < 0 || y >= mapHeight)
        {
            continue;
        }

        for (int x = cameraPos.x; x < cameraPos.x + viewWidth; ++x)
        {
            // 맵 범위를 벗어나지 않도록
            if (x < 0 || x >= mapWidth)
            {
                continue;
            }

            // 월드 좌표(x, y) 정보 가져오기
            wchar_t tile = mapData[y][x];

            // 액터 객체별 색상 지정
            Color tileColor = Color::White;
            //▣
            if (tile == L'▩')
            {
                tileColor = Color::DarkGray;
            }
            else if (tile == L'※')
            {
                tileColor = Color::Green;
            }

            // 해당 칸에 글자 담기
            tileStr[0] = tile;

            // x 좌표에 2를 곱해서 한 칸씩 출력
            Renderer::Get().Submit (tileStr,
                Vector2(x * 2, y), tileColor);
        }
    }

    // 엔진 Level의 Draw() 함수 호출
    Level::Draw();

    // 쿼드트리에 루트 노드가 있고, 토글 버튼이 true일때
    if (showQuadTree && rootNode)
    {
        // 구역 정보를 저장 할 배열 생성
        std::vector<Rect> boundaries;
        // 각 구역 정보 저장 함수 호출
        rootNode->GetBoundaries(boundaries);

        // 저장된 구역 좌표값 순회
        for (const Rect& r : boundaries)
        {
            // 가로 테두리 그리기
            for (int ix = 0; ix <= r.width; ++ix)
            {
                // 가로 2배
                int curX = (r.x + ix) * 2;

                // 상단 좌표에 그리기
                Renderer::Get().Submit(
                    L"━",
                    Vector2((r.x + ix) * 2, r.y),
                    Color::Cyan, 100);

                // 하단 좌표에 그리기
                Renderer::Get().Submit(
                    L"━",
                    Vector2((r.x + ix) * 2, (r.y + r.height)),
                    Color::Cyan, 100);
            }

            // 세로 테두리 그리기
            for (int iy = 0; iy <= r.height; ++iy)
            {
                int curY = r.y + iy;

                // 좌측 좌표에 그리기
                Renderer::Get().Submit(
                    L"┃",
                    Vector2(r.x * 2, (r.y + iy)),
                    Color::Cyan, 100);

                // 우측 좌표에 그리기
                Renderer::Get().Submit(
                    L"┃",
                    Vector2((r.x + r.width) * 2, (r.y + iy)),
                    Color::Cyan, 100);
            }
        }
    }
} 

void ArenaLevel::Tick(float deltaTime)
{

    // 기존 루트 노드 해제(쿼드트리)
    if (rootNode)
    {
        delete rootNode;
        rootNode = nullptr;
    }

    // 루트 노드 생성(쿼드트리)
    rootNode = new QuadTree(0, 0, mapWidth, mapHeight);

    // 액터 객체 순회
    for (Actor* actor : actors)
    {
        // 액터가 실제 살아있는지 확인
        if (actor->IsActive())
        {
            // 쿼드트리에 액터 넘기기
            rootNode->Insert(actor);
        }
    }

    Level::Tick(deltaTime);

    // 쿼드트리 시각화 토글
    if (Input::Get().GetKeyDown('Q'))
    {
        // NOT연산자로 bool 타입 변경
        showQuadTree = !showQuadTree;
    }

    if (Input::Get().GetKeyDown('G'))
    {
        for (int ix = 0; ix < 100; ++ix)
        {
            // 랜덤 스폰 좌표 및 성공여부
            Vector2 spawnPos;
            bool isSuccess = false;

            // 성공할 좌표를 찾을 때까지 반복
            while (!isSuccess)
            {
                spawnPos.x = rand() % mapWidth;
                spawnPos.y = rand() % mapHeight;

                // 랜덤 위치 값에 다른 액터가 있는지 확인
                if (IsWalkable(spawnPos.x, spawnPos.y) &&
                    GetActorAt(spawnPos) == nullptr)
                {
                    // 위치에 다른 액터가 없을경우
                    isSuccess = true;
                }
            }

            // 해당 위치에 몬스터 생성
            Monster* monster = new Monster();
            // 해당 액터의 주인 명시
            monster->SetOwner(this);
            // 랜덤 스폰된 객체 명시
            monster->SetGenerated(true);

            monster->BeginPlay();
            monster->SetPosition(spawnPos);
            // 리셋 대비
            monster->SetSpawnPoint(spawnPos);
            // 최종 엔진에 등록
            AddNewActor(monster);
        }
    }

    // R(리셋) 키가 눌렸다면 리셋 함수 호출
    if (Input::Get().GetKeyDown('R'))
    {
        ResetAllActors();
    }

    // 카메라 갱신 함수 호출
    UpdateCamera();
}

bool ArenaLevel::IsWalkable(int x, int y)
{
    // 예외 처리(맵 밖으로 나가는 상황 체크)
    // x, y 좌표가 음수 이거나, mapData 보다 클 경우 예외 처리
    if (y < 0 || x < 0 || 
        y >= mapData.size() || x >= mapData[y].size())
    {
        return false;
    }

    // 좌표 위치 액터 확인하여 반환
    return mapData[y][x] == L' ';
}

void ArenaLevel::UpdateCamera()
{
    // 플레이어 좌표 값 
    Vector2 playerPos = mainPlayer->GetPosition();

    // 카메라의 시작점 계산
    cameraPos.x = playerPos.x - (viewWidth / 2);
    cameraPos.y = playerPos.y - (viewHeight / 2);

    // 경계 예외 처리
    // 왼쪽 경계
    if (cameraPos.x < 0)
    {
        cameraPos.x = 0;
    }
    
    // 위쪽 경계
    if (cameraPos.y < 0)
    {
        cameraPos.y = 0;
    }

    // 오른쪽 경계
    int maxCameraX = mapWidth - viewWidth;
    if (cameraPos.x > maxCameraX)
    {
        cameraPos.x = maxCameraX;
    }

    // 아래쪽 경계
    int maxCameraY = mapHeight - viewHeight;
    if (cameraPos.y > maxCameraY)
    {
        cameraPos.y = maxCameraY;
    }

    // 그릴 범위를 지정하여 렌더러 호출
    Vector2 offset = Vector2(cameraPos.x * 2, cameraPos.y);
    Renderer::Get().SetWorldOffset(offset);
}

void ArenaLevel::ResetAllActors()
{
    // 모든 액터를 순회하여 리셋 호출
    for (Actor* actor : actors)
    {
        // 형변환 체크
        Monster* monster = actor->As<Monster>();
        
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
        else
        {
            // 몬스터가 아닌 액터는 리셋
            actor->Reset();
        }
    }
}