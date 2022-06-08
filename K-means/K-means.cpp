#include "wl.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Cluster 가짓 수와 이미지 파일명, 가로 * 세로 해상도를 최상단에서 변경 및 가능
#define CLUSTER_K 8
#define WIDTH 800
#define HEIGHT 540
#define RAW "Golf.raw"

// wl.cpp 에서 가져온 min 함수입니다.
inline int min(int a, int b) { return (a < b ? a : b); }

void main()
{
	// while 문의 종료를 알릴 신호와 각 cluster 내 총계, cluster 내 빈도수를 저장하는 2차원 배열입니다.
	int center_sum[CLUSTER_K][2] = { 0 };
	int flag = 0;

	CharImage Input(RAW, WIDTH, HEIGHT);

	// 첫 회차의 Centroid 는 임의의 K 개 픽셀 값으로 지정되며, 각 Cluster 의 평균 값으로 대체 됩니다.
	BYTE centroids[CLUSTER_K];
	BYTE temp_buffer, temp_diff;

	srand(time(NULL));

	// 최초 Centroid 를 지정합니다.
	for (int i = 0; i < CLUSTER_K; i++)
	{
		int number = rand();
		centroids[i] = { Input.pixel(number % WIDTH, number % HEIGHT) };
	}
	
	// 회차 별 이전의 Centroid 와 다음 현 Centroid 를 비교해, 달라지지 않았다면 종점으로 판단합니다.
	while (1) 
	{
		int temp_checker = centroids[0];
		
		for (int dx = 0; dx < WIDTH; dx++)
			for (int dy = 0; dy < HEIGHT; dy++)
			{
				temp_buffer = Input.pixel(dx, dy);
				BYTE temp_min = 255;
				int temp_loc = 0;

				for (int i = 0; i < CLUSTER_K; i++)
				{
					// Centroid ( mean 값 ) 마다 픽셀 값의 차이를 구하고, 최소를 기준으로 군집에 해당 시킵니다.
					temp_diff = abs(centroids[i] - temp_buffer);
					temp_min = min(temp_diff, temp_min);
					if (temp_diff == temp_min) temp_loc = i;
				}
				// 군집 별 픽셀 값 누계와 빈도 수를 저장해 평균을 계산할때 사용합니다.
				center_sum[temp_loc][0] += temp_buffer;
				center_sum[temp_loc][1] += 1;

				// 종점 신호를 받았을때만, 모든 픽셀을 해당 하는 군집의 평균으로 대체 합니다.
				if (flag) Input.pixel(dx, dy) = centroids[temp_loc];
			}
		// 종점 신호를 기점으로 마지막 회차는 저장 및 최종 확인 단계로, 저장을 마치고 종료됩니다.
		if (flag) break;
		for (int i = 0; i < CLUSTER_K; i++) centroids[i] = center_sum[i][0] / center_sum[i][1];

		// 종점 신호입니다.
		if (temp_checker == centroids[0]) flag = 1;
	}
	Input.Save("Out_Golf8.raw");
}