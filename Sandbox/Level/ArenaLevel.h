#pragma once

#include "Level/Level.h"

#include <string>

using namespace Wanted;

// 전방 선언
class Player;

class ArenaLevel: public Level
{
	RTTI_DECLARATIONS(ArenaLevel, Level)

public:
	ArenaLevel();
	virtual ~ArenaLevel();
	
	// 맵 로드 함수
	bool LoadMap(const wchar_t* filename);

	// override 
	virtual void Draw() override;
	virtual void Tick(float deltaTime) override;

	// 좌표가 이동 가능한 곳인지 확인하는 함수
	bool IsWalkable(int x, int y);

	// Getter
	inline Vector2 const GetcameraPos() { return cameraPos; }
	inline Player* const GetPlayer() { return mainPlayer; }

	// 리셋 함수
	void ResetAllActors(); 

private:
	// 카메라 위치 계산 함수
	void UpdateCamera();

private:
	// map 자료용 변수
	// 맵 데이터만 저장(플레이어 및 몬스터에 정보는 별도관리)
	std::vector<std::wstring> mapData;

	// 맵 크기 저장
	int mapWidth = 0;
	int mapHeight = 0;

	// 플레이어 객체 저장
	Player* mainPlayer = nullptr;

	// 카메라의 월드 좌표 (화면 왼쪽 위 끝 지점)
	Vector2 cameraPos;
	// 화면에 보여줄 가로 칸 수
	int viewWidth = 60;
	// 화면에 보여줄 세로 칸 수
	int viewHeight = 30;
	
	

};

