#ifndef ROBOT_H // 防止头文件重复引用
#define ROBOT_H

#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <math.h>
#include <fstream>
#include <sstream>

using namespace std;

#define PI acos(-1)
#define a_nocarry 19.648758406406830801
#define a_carry 14.164733276245577542
#define t 0.02

typedef pair<double, double> Point;

class Robot {

public:
	Robot() = default;
	void setId(int id);

	int getCarryId();
	Point getSalePoint();

	//tmp val
	int getWorkId();
	double getAng();
	double getOri();
	Point getRobotPoint();
	Point getDist_buy();
	Point getDis_sale();
	int getDis_buy_type();
	pair<double, double> getV();
	double getRestDistance();
	bool getCrash_start();
	int getRobotId();

	void changeDistBuy(Point &a);
	void changeDistSale(Point &a);
	void changeDistBuyType(int a);

	// 剩余帧数更新
	void changeRestTime(int cur_time);

	// 机器人状态更新函数
	void init(int workID, int carryID, double timeValue, double collisionValue, double angVelocity, double xlineVelocity,
		double ylineVelocity, double orientation, double x, double y);

	// 前往地点计算函数
	void calWorkplacePoint_1(vector<vector<pair<Point, vector<int>>>>& curBuyPoint, vector<vector<pair<Point, int>>>& curSalePoint, map<pair<Point, Point>, double>& getMoney, unordered_map<int, int>& willingPlat, map<Point, vector<int>>& curPlatStatus, vector<vector<double>>& location_add, vector<vector<double>>& location_add_buy, bool mark_7);
	void calWorkplacePoint_2(vector<vector<pair<Point, vector<int>>>>& curBuyPoint, vector<vector<pair<Point, int>>>& curSalePoint, map<pair<Point, Point>, double>& getMoney, unordered_map<int, int>& willingPlat, map<Point, vector<int>>& curPlatStatus, vector<vector<double>>& location_add, vector<vector<double>>& location_add_buy, bool mark_7);
	void calWorkplacePoint_3(vector<vector<pair<Point, vector<int>>>>& curBuyPoint, vector<vector<pair<Point, int>>>& curSalePoint, map<pair<Point, Point>, double>& getMoney, unordered_map<int, int>& willingPlat, map<Point, vector<int>>& curPlatStatus, vector<vector<double>>& location_add, vector<vector<double>>& location_add_buy, bool mark_7);
	void calWorkplacePoint_4(vector<vector<pair<Point, vector<int>>>>& curBuyPoint, vector<vector<pair<Point, int>>>& curSalePoint, map<pair<Point, Point>, double>& getMoney, unordered_map<int, int>& willingPlat, map<Point, vector<int>>& curPlatStatus, vector<vector<double>>& location_add, vector<vector<double>>& location_add_buy, bool mark_7, vector<Robot*> &vecRobot);

	// 速度和角速度计算函数
	void calOrientVelocity_1();
	void calOrientVelocity_2();
	void calOrientVelocity_3();
	void calOrientVelocity_4();

	// 机器人相撞调整函数
	void isJudgeCrash_1(vector<Point> robot_point, vector<double> robot_angle, vector<pair<double, double>> robot_v, vector<double> rest_distance);
	void isJudgeCrash_2(vector<Point> robot_point, vector<double> robot_angle, vector<pair<double, double>> robot_v, vector<double> rest_distance);
	void isJudgeCrash_3(vector<Point> robot_point, vector<double> robot_angle, vector<pair<double, double>> robot_v, vector<double> rest_distance);
	void isJudgeCrash_4(vector<Point> robot_point, vector<double> robot_angle, vector<pair<double, double>> robot_v, vector<double> rest_distance);

	// 计算到目标点距离
	void calcRestDistance();

	// 速度输出函数
	void print_v();

	// 是否买卖判断
	void isJudgeSellBuy_1(unordered_map<int, Point>& numbertoPoint);
	void isJudgeSellBuy_2(unordered_map<int, Point>& numbertoPoint);
	void isJudgeSellBuy_3(unordered_map<int, Point>& numbertoPoint);
	void isJudgeSellBuy_4(unordered_map<int, Point>& numbertoPoint);

	// 设置参数
	void setMapArg_1();
	void setMapArg_2();
	void setMapArg_3();
	void setMapArg_4();

	// 两坐标点距离计算
	inline double distance_point(Point point1, Point point2)
	{
		return sqrt(pow(point1.first - point2.first, 2) + pow(point1.second - point2.second, 2));
	}

private:

	int robotID; // 0~3

	int workID;//所处工作台ID，按出现顺序编号
	int carryID;//所携带物品类型，0表示未携带
	double timeValue;//携带物品时为[0.8, 1]的浮点数,不携带物品时为 0
	double collisionValue;//携带物品时为[0.8, 1]的浮点数,不携带物品时为 0
	double angVelocity;//单位:弧度/秒,正数:表示逆时针,负数:表示顺时针
	double xlineVelocity;//x线速度
	double ylineVelocity;//y线速度
	double orientation;//弧度,范围[-π,π]。方向示例:0:表示右方向,π/2:表示上方向,-π/2:表示下方向
	Point point; // 机器人坐标

	// 当前买卖地点
	int dist_buy_type; // carryID为0时，将要买的物品的类型，0表示无目标
	Point dist_sale;

	// 输出速度值
	double cout_v;
	double cout_w;

	// 到达目的地的剩余距离
	double rest_distance;

	// 是否启动防碰撞
	bool crash_start;

	// 记录剩余帧数
	int rest_time;
	double cur_buy_money;

	// 调优
	Point dist_buy;
	double S1;
	double S2;
	double D1_SAFE;// 1.6
	double D2_SAFE; // 3
	double ANGLE_LEFT;
	double ANGLE_RIGHT;
	double ANGLE_TOGETHER_LEFT;
	double ANGLE_TRUN;
	double ANGLE_TRUN_LEFT;
	double LOACTION_ADD_SALE_TO_BUY;
	double LOACTION_ADD_SALE_TO_SALE;
	double LOACTION_ADD_BUY_BUY;

};
// 头文件内容

#endif // ROBOT_H
