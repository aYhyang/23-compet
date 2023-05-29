#pragma once

#include <vector>
#include <queue>
#include <unordered_map>
#include <map>
#include <math.h>

using namespace std;

#define move_1 10.0
#define move_2 14.14

class cmp {
public:
	bool operator()(const Point_path* left, const Point_path* right)const {
		return left->F < right->F;
	}
};

struct Point_path
{
    int x;
    int y;
    double F;
    double G;
    double H;

    Point_path* parent;

    Point_path()=default;
    Point_path(int point_x, int point_y):x(point_x), y(point_y), F(0), G(0), H(0), parent(nullptr){}
};


class SearchPath
{

public:
    void getMap(vector<vector<char>> &useMap);
    vector<pair<int, int>> getRunPath(Point_path& start_p, Point_path& end_p, bool carry);

private:
    Point_path* findBestPath_noCarry(Point_path& start_p, Point_path& end_p);

    double calcG_noCarry(Point_path* tmp_point, Point_path* cur_point); 

    // 清空优先对列
    void clear(priority_queue<Point_path*,vector<Point_path*>,cmp> &p);

    vector<vector<char>> curMap;
    
    //定义优先队列，方便查找
    priority_queue<Point_path*,vector<Point_path*>,cmp> openQueue;
    map<pair<int,int>,Point_path*> closeMap;

};
