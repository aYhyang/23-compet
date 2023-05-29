#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <math.h>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

#include "Robot.h"

using namespace std;

typedef pair<double, double> Point;


//机器人数组
vector<Robot*> vecRobot;
int num_7 = 0;

//***********************************************************************//
//******************************* 全局变量 ******************************//
//***********************************************************************//

// 工作台坐标映射
map<Point, Point> usePlatPoint;

// 记录对应坐标的工作台的状态   key为坐标，value存类型、原材料格当前状态、原材料格可放状态、产品格状态、剩余生产时间
map<Point, vector<int>> curPlatStatus;

//每个工作台的顺序号对应的坐标
unordered_map<int, Point> numbertoPoint;

// key:当前可买工作台坐标  value:对应当前可卖的工作台坐标
vector<vector<pair<Point, vector<int>>>> curBuyPoint(7);
vector<vector<pair<Point,int>>> curSalePoint(7);

// 从购买地到出售地的坐标以及差价固定比 (按差价固定比降序，初始化后固定不变)
map<pair<Point, Point>, double> getMoney;
unordered_map<int, int> willingPlat;

// 工作台可收购材料映射
unordered_map<int, int> plat_map = { {1, 0}, //每个工作台可以卖的产品，二进制1号工作台对应0010
									{2, 0},
									{3, 0},
									{4, 6},
									{5, 10},
									{6, 12},
									{7, 112},
									{8, 128},
									{9, 254} };

//预期价值映射表
vector<double> worktoMoney = { 3000,3200,3400,7100,7800,8300,29000 };

//***********************************************************************//
//******************************* 函数定义 ******************************//
//***********************************************************************//

// 坐标距离计算
inline double distance_point(Point point1, Point point2)
{
	return sqrt(pow(point1.first - point2.first, 2) + pow(point1.second - point2.second, 2));
}

// 更新能买能卖的状态 API
void getCurEnableStatus()
{
	//重置状态
	for (int i = 0; i < 7; ++i)
	{
		curBuyPoint[i].clear();
		curSalePoint[i].clear();
	}
	willingPlat[3] = 0;
	willingPlat[4] = 0;
	willingPlat[5] = 0;

	//遍历工作台状态来更新可买可卖状态
	for (auto& it : curPlatStatus)
	{
		// 计算机器人到此工作台距离
		vector<double> robot_to_plat;
		for (int i = 0; i < 4; ++i)
		{
			double to_plat_distance = sqrt(pow(it.first.first - vecRobot[i]->getRobotPoint().first, 2) + pow(it.first.second - vecRobot[i]->getRobotPoint().second, 2));
			robot_to_plat.push_back(to_plat_distance);
		}

		// 计算每个机器人到此工作台的使用帧数
		vector<int> robot_to_plat_time;
		for (int i = 0; i < 4; ++i)
		{
			int cur_to_time = ceil(robot_to_plat[i] / 0.12);
			robot_to_plat_time.push_back(cur_to_time);
		}

		// 记录可买状态，可以从这买走
		if (it.second[3] == 1)  // 全部机器人都能买
		{
			curBuyPoint[it.second[0] - 1].emplace_back(make_pair(it.first, vector<int>{0, 1, 2, 3}));//记录1-7工作台目前可以买的坐标有哪些
		}
		else if (it.second[4] > 0) // 一部分机器人能买走
		{
			for (int i = 0; i < 4; ++i)
			{
				if (robot_to_plat_time[i] > it.second[4])
				{
					bool flag_insert = true;
					for (auto& buyP : curBuyPoint[it.second[0] - 1])
					{
						if (buyP.first == it.first)
						{
							buyP.second.push_back(i);
							flag_insert = false;
							break;
						}
					}
					if (flag_insert)
					{
						curBuyPoint[it.second[0] - 1].emplace_back(make_pair(it.first, vector<int>{i}));
					}
				}
			}
		}

		// 记录可卖状态，哪些物品可以卖到这个坐标，显然如果是1，2，3工作台tmp=0
		int tmp = (it.second[1] ^ it.second[2]);
		int loss_num = 0;
		vector<int> tmp_mark;
		for (int i = 0; i <= 7 && tmp; ++i)
		{
			if (tmp & 1)
			{
				if (it.second[0] == 7)
				{
					++willingPlat[i - 1];
				}
				++loss_num;
				tmp_mark.push_back(i - 1);
			}
			tmp >>= 1;
		}
		for (auto& index : tmp_mark)
		{
			curSalePoint[index].emplace_back(make_pair(it.first,loss_num));//记录1-7工作台目前可以卖的坐标有哪些
			// cerr<<" platType: "<<curPlatStatus[curSalePoint[index].back().first][0]<<" Point "<<it.first.first << " "<<it.first.second<< " loss_num "<<loss_num<<endl;
		}
		
	}

	// 遍历四个机器人来更新可买可卖状态
	for (auto& robot : vecRobot)
	{
		int index = robot->getCarryId();
		if (index != 0)  // 可卖状态更新
		{
			Point sale_point = robot->getSalePoint();
			for (auto& point : curSalePoint[index - 1])
			{
				if (point.first == sale_point)
				{
					point.first.first = -point.first.first;
					break;
				}
			}
		}
		else // 可买状态更新
		{
			Point buy_point = robot->getDist_buy();
			int buy_type = robot->getDis_buy_type();
			if (buy_type > 0)
			{
				for (auto& point : curBuyPoint[buy_type - 1])
				{
					if (point.first == buy_point)
					{
						point.first.first = -point.first.first;
						break;
					}
				}
				Point sale_point = robot->getSalePoint();
				for (auto& point : curSalePoint[buy_type - 1])
				{
					if (point.first == sale_point)
					{
						point.first.first = -point.first.first;
						break;
					}
				}
			}
		}
	}
}

// 初始化地图函数，包含对初始信息的计算
void initStatus()
{
	//--------------------------读取地图并初始化全局变量-------------------//

	int i = 0; // 记录地图列号
	int w_num = 0;//当前的工作台序号
	int robot_id = 0;

	vector<Point> tmp_platPoint;
	string line;
	while (getline(cin, line))
	{
		if (line == "OK")
		{
			break;
		}

		// 对地图每行记录处理
		for (int j = 0; j < line.size(); ++j)
		{
			if (line[j] == 'A')
			{
				//四个机器人预处理
				Robot* myRobot = new Robot();
				myRobot->setId(robot_id);
				++robot_id;

				vecRobot.push_back(myRobot);
			}
			else if (line[j] >= '1' && line[j] <= '9')
			{
				if (line[j] == '7')
				{
					++num_7;
				}

				// 工作台状态初始化
				double x = 0.5 * j + 0.25;        // 横坐标
				double y = 50 - (0.5 * i + 0.25); // 纵坐标
				int index = line[j] - '0';        // 机器类型号

				Point realPoint = make_pair(x, y);

				// 去顺序化
				for (auto& p : tmp_platPoint)
				{
					if (distance_point(p, make_pair(x, y)) < 0.53)
					{
						x -= 0.1;
						y -= 0.1;
					}
				}
				Point getVirPoint = make_pair(x, y);
				tmp_platPoint.push_back(make_pair(x, y));
				usePlatPoint[realPoint] = getVirPoint;

				// 保存记录
				numbertoPoint[w_num++] = usePlatPoint[realPoint]; //每个工作台序号对应的坐标
				curPlatStatus[usePlatPoint[realPoint]] = { index, 0, plat_map[index], (line[j] == '8' || line[j] == '9') ? 0 : 1, -1 };
			}
		}
		++i;
	}

	//--------------------------初始化买地到卖地金钱比重-------------------//

	getCurEnableStatus(); // 调用更新能买能卖的api

	// 计算资金比
	for (int i = 0; i < 7; ++i)
	{
		double money = worktoMoney[i];

		for (auto& curBuy : curBuyPoint[i])
		{
			for (auto& curSale : curSalePoint[i])
			{
				// 资金比计算  todo->(后续可根据加速度改进计算方式)
				double dist = distance_point(curBuy.first, curSale.first);
				int tmp_money = money / ceil(dist / 0.12);

				// 状态存储
				pair<Point, Point> tmp_key = make_pair(curBuy.first, curSale.first);
				getMoney[tmp_key] = tmp_money;
			}
		}
	}
}

void InitRobot()
{
	if(num_7 == 8)
	{
		vecRobot[0]->setMapArg_1();
		vecRobot[1]->setMapArg_1();
		vecRobot[2]->setMapArg_1();
		vecRobot[3]->setMapArg_1();
	}
	else if(num_7 == 2)
	{
		vecRobot[0]->setMapArg_2();
		vecRobot[1]->setMapArg_2();
		vecRobot[2]->setMapArg_2();
		vecRobot[3]->setMapArg_2();
	}
	else if(num_7 == 0)
	{
		vecRobot[0]->setMapArg_3();
		vecRobot[1]->setMapArg_3();
		vecRobot[2]->setMapArg_3();
		vecRobot[3]->setMapArg_3();
	}
	else if(num_7 == 1)
	{
		vecRobot[0]->setMapArg_4();
		vecRobot[1]->setMapArg_4();
		vecRobot[2]->setMapArg_4();
		vecRobot[3]->setMapArg_4();
	}
}

// 读取每一帧并更新状态
bool readUntilOK()
{
	string line;//每一行的字符
	stringstream ss;
	int index = 0;

	// 金钱数
	int frame_money;

	getline(cin, line);
	ss << line;
	ss >> frame_money;
	ss.clear();

	//工作台的数量
	int k;
	getline(cin, line);
	ss << line;
	ss >> k;
	ss.clear();

	while (getline(cin, line))
	{
		if (line == "OK")
		{
			//API：调用更新能卖能买
			getCurEnableStatus();
			return true;
		}

		ss << line;
		if (k)  // 存储工作台状态
		{
			int plat_id, plat_time, plat_buy, plat_sale;
			double x, y;

			ss >> plat_id;
			ss >> x >> y;
			ss >> plat_time >> plat_buy >> plat_sale;
			curPlatStatus[usePlatPoint[make_pair(x, y)]] = { plat_id, plat_buy, plat_map[plat_id], plat_sale, plat_time };

			--k;
			ss.clear();
			continue;
		}

		int plat_id, carry_id;
		double timeValue, collisionValue, angVelocity, xlineVelocity, ylineVelocity, orientation, x, y;

		ss >> plat_id >> carry_id;
		ss >> timeValue >> collisionValue >> angVelocity >> xlineVelocity >> ylineVelocity >> orientation >> x >> y;

		vecRobot[index]->init(plat_id, carry_id, timeValue, collisionValue, angVelocity, xlineVelocity, ylineVelocity, orientation, x, y);
		++index;

		ss.clear();
	}
	return false;
}


int main()
{

	// std::ofstream logfile("/home/yyh/Desktop/debug.log");
	// std::cerr.rdbuf(logfile.rdbuf());
	// cerr<<"start!"<<endl;

	initStatus();
	InitRobot();
	puts("OK");
	fflush(stdout);
	int frameID;
	while (scanf("%d", &frameID) != EOF)
	{
		// cerr<<frameID<<endl;

		readUntilOK();

		vector<Point> robot_point;
		vector<double> robot_angle;
		vector<pair<double, double>> robot_v;
		vector<double> rest_distance;

		vector<vector<double>> location_add_sale;
		vector<vector<double>> location_add_buy;

		// 惩罚点计算
		for (int robotId = 0; robotId < 4; robotId++)
		{
			if (vecRobot[robotId]->getCarryId() != 0)
			{
				Point point_sale = vecRobot[robotId]->getDis_sale();
				double res_distance = vecRobot[robotId]->getRestDistance();

				location_add_sale.emplace_back(vector<double>({ double(robotId), point_sale.first, point_sale.second, res_distance }));
			}
			else
			{
				if (vecRobot[robotId]->getDis_buy_type() != 0)
				{
					Point point_buy = vecRobot[robotId]->getDist_buy();

					location_add_buy.emplace_back(vector<double>({ double(robotId), point_buy.first, point_buy.second, vecRobot[robotId]->getRobotPoint().first, vecRobot[robotId]->getRobotPoint().second, vecRobot[robotId]->getDis_sale().first, vecRobot[robotId]->getDis_sale().second}));
				}
			}
		}

		// 防碰撞相关变量计算  目标工作台计算  剩余帧计算
		for (int robotId = 0; robotId < 4; robotId++)
		{
			vecRobot[robotId]->changeRestTime(frameID);
			if(num_7 == 8)
			{
				vecRobot[robotId]->calWorkplacePoint_1(curBuyPoint, curSalePoint, getMoney, willingPlat, curPlatStatus, location_add_sale, location_add_buy, num_7);
			}
			else if(num_7 == 2)
			{
				vecRobot[robotId]->calWorkplacePoint_2(curBuyPoint, curSalePoint, getMoney, willingPlat, curPlatStatus, location_add_sale, location_add_buy, num_7);
			}
			else if(num_7 == 0)
			{
				vecRobot[robotId]->calWorkplacePoint_3(curBuyPoint, curSalePoint, getMoney, willingPlat, curPlatStatus, location_add_sale, location_add_buy, num_7);
			}
			else if(num_7 == 1)
			{
				vecRobot[robotId]->calWorkplacePoint_4(curBuyPoint, curSalePoint, getMoney, willingPlat, curPlatStatus, location_add_sale, location_add_buy, num_7, vecRobot);
			}
			
			vecRobot[robotId]->calcRestDistance();

			robot_point.push_back(vecRobot[robotId]->getRobotPoint());
			robot_angle.push_back(vecRobot[robotId]->getOri());
			robot_v.push_back(vecRobot[robotId]->getV());
		}

		// 剩余距离计算
		for (int robotId = 0; robotId < 4; robotId++)
		{
			rest_distance.push_back(vecRobot[robotId]->getRestDistance());
		}

		printf("%d\n", frameID);

		// 速度 角速度计算 是否买卖判断
		for (int robotId = 0; robotId < 4; robotId++)
		{
			if(num_7 == 8)
			{
				vecRobot[robotId]->calOrientVelocity_1();
				vecRobot[robotId]->isJudgeCrash_1(robot_point, robot_angle, robot_v, rest_distance);

				// 打印输出
				vecRobot[robotId]->print_v();
				vecRobot[robotId]->isJudgeSellBuy_1(numbertoPoint);
			}
			else if(num_7 == 2)
			{
				vecRobot[robotId]->calOrientVelocity_2();
				vecRobot[robotId]->isJudgeCrash_2(robot_point, robot_angle, robot_v, rest_distance);

				// 打印输出
				vecRobot[robotId]->print_v();
				vecRobot[robotId]->isJudgeSellBuy_2(numbertoPoint);
			}
			else if(num_7 == 0)
			{
				vecRobot[robotId]->calOrientVelocity_3();
				vecRobot[robotId]->isJudgeCrash_3(robot_point, robot_angle, robot_v, rest_distance);

				// 打印输出
				vecRobot[robotId]->print_v();
				vecRobot[robotId]->isJudgeSellBuy_3(numbertoPoint);
			}
			else if(num_7 == 1)
			{
				vecRobot[robotId]->calOrientVelocity_4();
				vecRobot[robotId]->isJudgeCrash_4(robot_point, robot_angle, robot_v, rest_distance);

				// 打印输出
				vecRobot[robotId]->print_v();
				vecRobot[robotId]->isJudgeSellBuy_4(numbertoPoint);
			}
		}

		// cerr<<endl<<endl;

		cout << "OK" << endl;
		fflush(stdout);
	}
	return 0;
}
