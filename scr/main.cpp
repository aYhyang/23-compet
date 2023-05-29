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
#include "SearchPath.h"

using namespace std;

typedef pair<double, double> Point;

//***********************************************************************//
//******************************* 全局变量 ******************************//
//***********************************************************************//

// 存储地图
vector<vector<char>> curMap;

// 买点到卖点对
vector<pair<pair<int, int>,pair<int, int>>> buy_sale_pair;
map<pair<pair<int, int>,pair<int, int>>, vector<pair<int,int>>> buy_sale_path;

// 卖点到买点对
vector<pair<pair<int, int>,pair<int, int>>> sale_buy_pair;
map<pair<pair<int, int>,pair<int, int>>, vector<pair<int,int>>> sale_buy_path;

// 机器人数组
vector<Robot*> vecRobot;
int num_7 = 0;

// 工作台坐标映射,用于初始化调整工作台坐标映射
map<Point, Point> usePlatPoint;
unordered_map<int, Point> numbertoPoint;

// 差价比
map<pair<Point, Point>, double> getMoney;

// 工作台可收购材料映射,二进制1表示可卖
unordered_map<int, int> plat_map = { {1, 0},
									{2, 0},
									{3, 0},
									{4, 6},
									{5, 10},
									{6, 12},
									{7, 112},
									{8, 128},
									{9, 254} };

//价值映射表
vector<double> worktoGetMoney = { 3000,3200,3400,7100,7800,8300,29000 };
vector<double> worktoBuyMoney = { 3000,4400,5800,15400,17200,19200,76000 };
vector<double> worktoSaleMoney = { 6000,7600,9200,22500,25000,27500,105000 };


// 记录对应坐标的工作台的状态
map<Point, vector<int>> curPlatStatus;

// 工作台信息
vector<vector<buyPoint>> curBuyPoint(7);
vector<vector<pair<Point,int>>> curSalePoint(7);

// 缺少材料数目计算
unordered_map<int, int> willingPlat;


//***********************************************************************//
//******************************* 函数定义 ******************************//
//***********************************************************************//

// 坐标距离计算
inline double distance_point(Point point1, Point point2)
{
	return sqrt(pow(point1.first - point2.first, 2) + pow(point1.second - point2.second, 2));
}

// 时间系数计算
inline double f(int x)
{
	return (1-sqrt(1-(1-1.0*x/9000)*(1-1.0*x/9000)))*(1-0.8) + 0.8;
}

void changeBuyStrategy()
{
	// todo 目前与距离预计算起冲突，后期解决
	for (int i = 0; i < 4; ++i) {
		if(vecRobot[i]->getCarryId() != 0 || (vecRobot[i]->getCarryId() == 0 && vecRobot[i]->getDis_buy_type() == 0))
		{
			continue;
		}
        for (int j = 0; j < 4; ++j) {
            if (i == j) 
			{
                continue;
            }
			if(vecRobot[j]->getCarryId() != 0 || (vecRobot[j]->getCarryId() == 0 && vecRobot[j]->getDis_buy_type() == 0))
			{
				continue;
			}
            auto dis1 = distance_point(vecRobot[i]->getRobotPoint(), vecRobot[i]->getDist_buy());
			auto dis2 = distance_point(vecRobot[j]->getRobotPoint(), vecRobot[j]->getDist_buy());

			auto dis1_change = distance_point(vecRobot[i]->getRobotPoint(), vecRobot[j]->getDist_buy());
			auto dis2_change = distance_point(vecRobot[j]->getRobotPoint(), vecRobot[i]->getDist_buy());

            if (dis1 + dis2 > dis1_change + dis2_change) 
			{
                // 买卖位置交换
                Point tmp = vecRobot[i]->getDist_buy();
				vecRobot[i]->dist_buy = vecRobot[j]->dist_buy;
				vecRobot[j]->dist_buy = tmp;

				tmp = vecRobot[i]->getDis_sale();
				vecRobot[i]->dist_sale = vecRobot[j]->dist_sale;
				vecRobot[j]->dist_sale = tmp;

				int tmp_type = vecRobot[i]->dist_buy_type;
				vecRobot[i]->dist_buy_type = vecRobot[j]->dist_buy_type;
				vecRobot[j]->dist_buy_type = tmp_type;
            }
        }
    }
}

// 靠近墙和碰撞时改变速度和转速
void FixStrategy()
{
    for (auto &robot : vecRobot) {
        robot->CalcNpoint();
		Point robot_nPoint = robot->getRobotNPoint();
        if (robot_nPoint.first > 49.75 || robot_nPoint.second > 49.75 || robot_nPoint.first < 0.25 || robot_nPoint.second < 0.25) {
            robot->cout_v *= -1;
        }
    }

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (i == j) 
			{
                continue;
            }
            auto &robotI = vecRobot[i];
            auto &robotJ = vecRobot[j];
            auto dis = distance_point(robotI->getRobotPoint(), robotJ->getRobotPoint());
			double r_i = robotI->getCarryId()?0.53:0.45;
			double r_j = robotJ->getCarryId()?0.53:0.45;
            if (dis <= r_i + r_j + 0.002) 
			{
                // 已经碰撞了
                if (robotI->getRobotPoint().first < robotJ->getRobotPoint().first) 
				{
                    if (robotI->getOri() > 0)
					{
                        robotI->cout_w += (M_PI + 0.3125) / 4.5;
                    } 
					else {
                        robotI->cout_w += -(M_PI + 0.3125) / 4.5;
                    }
                }
				else 
				{
                    if (robotI->getOri() > 0) 
					{
                        robotI->cout_w += -(M_PI + 0.3125) / 4.5;
                    } 
					else 
					{
                        robotI->cout_w += (M_PI + 0.3125) / 4.5;
                    }
                }
            }
        }
    }
}

// 更新能买能卖的状态
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
		vector<double> robot_to_plat;
		for (int i = 0; i < 4; ++i)
		{
			double to_plat_distance = distance_point(it.first, vecRobot[i]->getRobotPoint());
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
		if (it.second[3] == 1)
		{
			buyPoint tmp;
			tmp.p = it.first;
			tmp.canBuyRobot = vector<int>{1,2,3,4};
			tmp.toBuyRobotId = 0;
			tmp.nextBuyRobotId = 0;
			tmp.nextBuyRobot.clear();

			curBuyPoint[it.second[0] - 1].emplace_back(tmp);
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
						if (buyP.p == it.first)
						{
							buyP.canBuyRobot.push_back(i+1);
							flag_insert = false;
							break;
						}
					}
					if (flag_insert)
					{
						buyPoint tmp;
						tmp.p = it.first;
						tmp.canBuyRobot = vector<int>{i+1};
						tmp.toBuyRobotId = 0;
						tmp.nextBuyRobotId = 0;
						tmp.nextBuyRobot.clear();

						curBuyPoint[it.second[0] - 1].emplace_back(tmp);
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
			curSalePoint[index].emplace_back(make_pair(it.first,loss_num));
		}
		
	}

	// 遍历四个机器人来更新可买可卖状态
	for (auto& robot : vecRobot)
	{
		int index = robot->getCarryId();
		if (index != 0) 
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
			int robot_id = robot->getRobotId();
			if (buy_type > 0)
			{
				for (auto& point : curBuyPoint[buy_type - 1])
				{
					if (point.p == buy_point && point.toBuyRobotId == 0)
					{
						point.toBuyRobotId = robot_id+1;

						// 计算还能前往购买的机器人(1~3号工作台)
						if(buy_type < 4)
						{
							double dis = distance_point(point.p, buy_point);
							int need_time = ceil(dis / 0.12) + 50;
							
							for (auto& robot : vecRobot)
							{
								if(robot->getCarryId() == 0 && ceil(distance_point(point.p, robot->getRobotPoint())/0.12) > need_time)
								{
									point.nextBuyRobot.push_back(robot->getRobotId()+1);
								}
							}
							point.nextBuyRobot.push_back(robot_id+1);
						}

						break;
					}
					else if(point.p == buy_point && point.toBuyRobotId != 0)
					{
						if(distance_point(point.p, buy_point) < distance_point(point.p, vecRobot[point.toBuyRobotId-1]->getDist_buy()))
						{
							point.nextBuyRobotId = point.toBuyRobotId;
							point.toBuyRobotId = robot_id+1;

							// 计算还能前往购买的机器人(1~3号工作台)
							if(buy_type < 4)
							{
								double dis = distance_point(point.p, buy_point);
								int need_time = ceil(dis / 0.12) + 50;
								
								for (auto& robot : vecRobot)
								{
									if(robot->getCarryId() == 0 && ceil(distance_point(point.p, robot->getRobotPoint())/0.12) > need_time)
									{
										point.nextBuyRobot.push_back(robot->getRobotId()+1);
									}
								}
								point.nextBuyRobot.push_back(robot_id+1);
							}
						}
						else
						{
							point.nextBuyRobotId = robot_id+1;
						}

						break;
					}
				}

				// 可卖状态更新
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
			// 记录地图
			curMap[i][j] = line[j];

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
				numbertoPoint[w_num++] = usePlatPoint[realPoint];
				curPlatStatus[usePlatPoint[realPoint]] = { index, 0, plat_map[index], (line[j] == '8' || line[j] == '9') ? 0 : 1, -1 };
			}
		}
		++i;
	}

	//--------------------------初始化买地到卖地金钱比重-------------------//

	getCurEnableStatus();

	// 计算资金比
	for (int i = 0; i < 7; ++i)
	{
		double money = worktoSaleMoney[i];

		for (auto& curBuy : curBuyPoint[i])
		{
			int buy_j = (curBuy.p.first - 0.25)*2;
			int buy_i = ((50-curBuy.p.second)-0.25)*2;
			for (auto& curSale : curSalePoint[i])
			{
				// 资金比计算  todo->(后续可根据加速度改进计算方式)
				double dist = distance_point(curBuy.p, curSale.first);
				int tmp_money = money * f(ceil(dist / 0.12)) - worktoBuyMoney[i];

				// 状态存储
				pair<Point, Point> tmp_key = make_pair(curBuy.p, curSale.first);
				getMoney[tmp_key] = tmp_money;

				// 存储买点到卖点对
				int sale_j = (curSale.first.first - 0.25)*2;
				int sale_i = ((50-curSale.first.second)-0.25)*2;
				pair<int,int> buy_pair = make_pair(buy_i, buy_j);
				pair<int,int> sale_pair = make_pair(sale_i, sale_j);

				buy_sale_pair.push_back(make_pair(buy_pair, sale_pair));
			}
		}
	}

	// 存储卖点到买点对
	for(auto &curSale_i:curSalePoint)
	{
		for(auto &curSale:curSale_i)
		{
			int sale_j = (curSale.first.first - 0.25)*2;
			int sale_i = ((50-curSale.first.second)-0.25)*2;

			for(auto &curBuy_i:curBuyPoint)
			{
				for(auto &curBuy:curBuy_i)
				{
					int buy_j = (curBuy.p.first - 0.25)*2;
					int buy_i = ((50-curBuy.p.second)-0.25)*2;

					pair<int,int> buy_pair = make_pair(buy_i, buy_j);
					pair<int,int> sale_pair = make_pair(sale_i, sale_j);

					sale_buy_pair.push_back(make_pair(sale_pair, buy_pair));
				}
			}
		}
	}

	// 计算最优路径并存储
	SearchPath toSearch;
	for(auto &calc_pair:buy_sale_pair)  // 买点到卖点路径
	{
		Point_path start_point(calc_pair.first.first, calc_pair.first.second);
		Point_path end_point(calc_pair.second.first, calc_pair.second.second);
		buy_sale_path[calc_pair] = toSearch.getRunPath(start_point,end_point,true);
	}
	for(auto &calc_pair:sale_buy_pair)  // 卖点到买点路径
	{
		Point_path start_point(calc_pair.first.first, calc_pair.first.second);
		Point_path end_point(calc_pair.second.first, calc_pair.second.second);
		sale_buy_path[calc_pair] = toSearch.getRunPath(start_point,end_point,false);
	}
	

}

// 依据地图序号初始化机器人运动参数
void InitRobot()
{
	vecRobot[0]->setMapArg();
	vecRobot[1]->setMapArg();
	vecRobot[2]->setMapArg();
	vecRobot[3]->setMapArg();

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
	initStatus();
	InitRobot();
	puts("OK");
	fflush(stdout);
	int frameID;
	while (scanf("%d", &frameID) != EOF)
	{

		readUntilOK();

		// 位置记录
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

					location_add_buy.emplace_back(vector<double>({ double(robotId), point_buy.first, point_buy.second }));
				}
			}
		}

		vector<Point> robot_point;
		vector<double> robot_angle;
		vector<pair<double, double>> robot_v;
		
		// 剩余帧计算 目标工作台计算 防碰撞相关变量计算   
		for (int robotId = 0; robotId < 4; robotId++)
		{
			vecRobot[robotId]->changeRestTime(frameID);
			vecRobot[robotId]->calWorkplacePoint(curBuyPoint, curSalePoint, getMoney, willingPlat, curPlatStatus, location_add_sale, location_add_buy, num_7, vecRobot);
			changeBuyStrategy();
			vecRobot[robotId]->calcRestDistance();

			robot_point.push_back(vecRobot[robotId]->getRobotPoint());
			robot_angle.push_back(vecRobot[robotId]->getOri());
			robot_v.push_back(vecRobot[robotId]->getV());
		}

		vector<double> rest_distance;
		
		// 剩余距离存储
		for (int robotId = 0; robotId < 4; robotId++)
		{
			rest_distance.push_back(vecRobot[robotId]->getRestDistance());
		}

		printf("%d\n", frameID);


		// 速度 角速度计算 防碰撞 是否买卖判断
		for (int robotId = 0; robotId < 4; robotId++)
		{

			vecRobot[robotId]->calOrientVelocity_1();
			vecRobot[robotId]->isJudgeCrash(robot_point, robot_angle, robot_v, rest_distance);
			// FixStrategy(); //使用方式二计算速度时用到

			// 打印输出
			vecRobot[robotId]->print_v();
			vecRobot[robotId]->isJudgeSellBuy(numbertoPoint);
		}

		cout << "OK" << endl;
		fflush(stdout);
	}
	return 0;
}
