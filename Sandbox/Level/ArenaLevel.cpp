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

            // x 좌표에 2를 곱해서 한 칸씩 출력
            Renderer::Get().Submit (std::wstring(1, tile).c_str(),
                Vector2(x * 2, y), tileColor);
        }
    }

    // 엔진 Level의 Draw() 함수 호출
    Level::Draw();
}

void ArenaLevel::Tick(float deltaTime)
{
    Level::Tick(deltaTime);

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
        actor->Reset();
    }
}