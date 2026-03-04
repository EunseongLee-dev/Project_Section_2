#include "ArenaLevel.h"
#include "Actor/Player.h"
#include "Render/Renderer.h"

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
                line[ix] = L'■';
                
                break;
            }
            case L'X':
            {
                line[ix] = L'▣';
                break;

            }
            case L'P':
            {
                // 플레이어 객체 생성
                Player* player = new Player();

                // 플레이어 위치 설정
                player->SetPosition(Vector2(ix, y));

                // 플레이어 객체 등록
                this->AddNewActor(player);

                // 기존 위치 정보를 공백으로 바꾸기
                line[ix] = L' ';
                break;
            }
   
            case L'M':
            {
                //Todo: 몬스터 액터 생성 및 위치 설정
                // 기존 위치 정보를 공백으로 바꾸기
                line[ix] = L'♞';
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
    load.close();
    return true;
}

void ArenaLevel::Draw()
{
    for (int y = 0; y < mapData.size(); ++y)
    {
        for (int x = 0; x < mapData[y].size(); ++x)
        {
            wchar_t tile = mapData[y][x];
            Color tileColor = Color::White;

            // 액터 객체별 색상 지정
            if (tile == L'■')
            {
                tileColor = Color::DarkGray;
            }
            else if (tile == L'▣')
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
