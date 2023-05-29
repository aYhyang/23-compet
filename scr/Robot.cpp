#include "Robot.h"

void Robot::setId(int id)
{
	robotID = id;
}

int Robot::getCarryId()
{
	return this->carryID;
}

Point Robot::getSalePoint()
{
	return this->dist_sale;
}

int Robot::getWorkId()
{
	return workID;
}

double Robot::getAng()
{
	return angVelocity;
}

double Robot::getOri()
{
	return orientation;
}

Point Robot::getRobotPoint()
{
	return point;
}

Point Robot::getDist_buy()
{
	return dist_buy;
}

Point Robot::getDis_sale()
{
	return dist_sale;
}

int Robot::getDis_buy_type()
{
	return dist_buy_type;
}

pair<double, double> Robot::getV()
{
	return make_pair(xlineVelocity, ylineVelocity);
}

double Robot::getRestDistance()
{
	return rest_distance;
}

bool Robot::getCrash_start()
{
	return crash_start;
}

int Robot::getRobotId()
{
	return this->robotID;
}

void Robot::changeDistBuy(Point &a)
{
	this->dist_buy.first = a.first;
	this->dist_buy.second = a.second;
}

void Robot::changeDistSale(Point &a)
{
	this->dist_sale.first = a.first;
	this->dist_sale.second = a.second;
}

void Robot::changeDistBuyType(int a)
{
	this->dist_buy_type = a;
}

void Robot::changeRestTime(int cur_time)
{
	rest_time = 9000 - cur_time;
}

void Robot::init(int workID, int carryID, double timeValue, double collisionValue, double angVelocity, double xlineVelocity, double ylineVelocity, double orientation, double x, double y)
{
	this->workID = workID;
	this->carryID = carryID;
	this->timeValue = timeValue;
	this->collisionValue = collisionValue;
	this->angVelocity = angVelocity;
	this->xlineVelocity = xlineVelocity;
	this->ylineVelocity = ylineVelocity;
	this->orientation = orientation;
	this->point.first = x;
	this->point.second = y;
}

void Robot::calWorkplacePoint_1(vector<vector<pair<Point, vector<int>>>>& curBuyPoint, vector<vector<pair<Point, int>>>& curSalePoint, map<pair<Point, Point>, double>& getMoney, unordered_map<int, int>& willingPlat, map<Point, vector<int>>& curPlatStatus, vector<vector<double>>& location_add, vector<vector<double>>& location_add_buy, bool mark_7)
{
	if (carryID == 0)  // 找要买的工作台坐标
	{
		if (dist_buy_type != 0)
		{
			return;
		}

		double money_mark = 0;
		if (dist_buy_type > 0)
		{
			money_mark = this->cur_buy_money;
		}

		for (int i = 0; i < 7; ++i)
		{
			for (int j = 0; j < curBuyPoint[i].size(); ++j)
			{
				if (curBuyPoint[i][j].first.first < 0 || find(curBuyPoint[i][j].second.begin(), curBuyPoint[i][j].second.end(), robotID) == curBuyPoint[i][j].second.end())
				{
					continue;
				}
				for (int k = 0; k < curSalePoint[i].size(); ++k)
				{
					if (curSalePoint[i][k].first.first < 0)
					{
						continue;
					}
					if (mark_7 && curPlatStatus[make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second)][0] != 7 && curPlatStatus[curSalePoint[i][k].first][0] == 9)
					{
						continue;
					}

					// 最后时间间隔是否购买
					int time_a = ceil(distance_point(make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second), point) / 0.12);
					int time_b = ceil(distance_point(make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second), curSalePoint[i][k].first) / 0.12);
					
					int all_time = time_a + time_b + 80;
					if (all_time > rest_time)
					{
						continue;
					}

					// 计算可赚差价比
					double cur_getmoney = getMoney[make_pair(make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second), curSalePoint[i][k].first)];
					double cur_distance = distance_point(make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second), point);

					//差价比动态改变倾向
					//购买奖励(4~6)
					if (curPlatStatus[make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second)][0] == 4)
					{
						cur_getmoney += (willingPlat[3] * 20000); //5000
					}
					else if (curPlatStatus[make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second)][0] == 5)
					{
						cur_getmoney += (willingPlat[4] * 20000);
					}
					else if (curPlatStatus[make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second)][0] == 6)
					{
						cur_getmoney += (willingPlat[5] * 20000);
					}

					// 出售奖励(1~3)
					if (curPlatStatus[curSalePoint[i][k].first][0] == 4)
					{
						cur_getmoney += (willingPlat[3] * 100000); //10000
					}
					else if (curPlatStatus[curSalePoint[i][k].first][0] == 5)
					{
						cur_getmoney += (willingPlat[4] * 100000);
					}
					else if (curPlatStatus[curSalePoint[i][k].first][0] == 6)
					{
						cur_getmoney += (willingPlat[5] * 100000);
					}

					// 尽量卖给缺材料少的工作台
					if (curSalePoint[i][k].second != 0)
					{
						if (curPlatStatus[curSalePoint[i][k].first][0] == 7)
						{
							cur_getmoney += 50000 * (3 - curSalePoint[i][k].second);
						}
						else if (curPlatStatus[curSalePoint[i][k].first][0] >= 4 && curPlatStatus[curSalePoint[i][k].first][0] <= 6)
						{
							cur_getmoney += 50000 * (2 - curSalePoint[i][k].second);
						}
					}

					// 最后时刻改变机制
					if (curPlatStatus[make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second)][0] == 7)
					{
						if (rest_time < 1000)  // 最后时刻要拿7
						{
							cur_getmoney += 5000000 * 2;
						}
						else
						{
							cur_getmoney += 50000 * 2 / cur_distance;
						}
					}

					if (rest_time < 600) // 最后时刻拿最贵的卖 600
					{
						// 最后时刻改变机制
						int cur_plat_type = curPlatStatus[make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second)][0];
						cur_getmoney += 5000000 * cur_plat_type;
					}

					cur_getmoney /= ceil(cur_distance / 0.12);
					cur_getmoney = max(cur_getmoney, 0.00001 * getMoney[make_pair(make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second), curSalePoint[i][k].first)]);

					// 前往地点更新并标记
					if (cur_getmoney > money_mark)
					{
						if (dist_buy_type > 0)  // 消除标记点
						{
							// 消除标记点
							for (auto& p_change : curBuyPoint[dist_buy_type - 1])
							{
								if (p_change.first.first == -dist_buy.first && p_change.first.second == dist_buy.second)
								{
									p_change.first.first = -p_change.first.first;
									break;
								}
							}
							// 消除标记点
							for (auto& p_change : curSalePoint[dist_buy_type - 1])
							{
								if (p_change.first.first == -dist_sale.first && p_change.first.second == dist_sale.second)
								{
									p_change.first.first = -p_change.first.first;
									break;
								}
							}
						}

						dist_buy = curBuyPoint[i][j].first;
						dist_buy_type = i + 1;
						dist_sale = curSalePoint[i][k].first;
						money_mark = cur_getmoney;
						this->cur_buy_money = cur_getmoney;

						// 标记，防止其他机器人前往
						curBuyPoint[i][j].first.first = -curBuyPoint[i][j].first.first;
						curSalePoint[i][k].first.first = -curSalePoint[i][k].first.first;
					}
				}
			}
		}
	}
	else  // 更新能卖的工作台坐标
	{
		if(rest_time > 1000)
		{
			return;
		}
		// 带了哪种物品
		int index = this->carryID - 1;

		double curDistance = distance_point(point, dist_sale);
		for (auto& p : curSalePoint[index])
		{
			if (p.first.first < 0)
			{
				continue;
			}
			if (curPlatStatus[p.first][0] == 9)
			{
				continue;
			}

			// 更换出售位置
			double dis = distance_point(p.first, point);
			if (dis < curDistance)
			{
				// 消除标记点
				for (auto& p_change : curSalePoint[index])
				{
					if (p_change.first.first == -dist_sale.first && p_change.first.second == dist_sale.second)
					{
						p_change.first.first = -p_change.first.first;
						break;
					}
				}

				// 更新状态
				curDistance = dis;
				dist_sale = p.first;

				// 设置标记
				p.first.first = -p.first.first;

			}
		}
	}
}

void Robot::calWorkplacePoint_2(vector<vector<pair<Point, vector<int>>>>& curBuyPoint, vector<vector<pair<Point, int>>>& curSalePoint, map<pair<Point, Point>, double>& getMoney, unordered_map<int, int>& willingPlat, map<Point, vector<int>>& curPlatStatus, vector<vector<double>>& location_add, vector<vector<double>>& location_add_buy, bool mark_7)
{
	if (carryID == 0)  // 找要买的工作台坐标
	{
		if(robotID == 0 && 9000 - rest_time == 5257)
		{
			dist_buy = make_pair(27.75, 18.25);
			dist_sale = make_pair(0.75, 0.75);
			dist_buy_type = 2;
		}
		if(robotID == 3 && 9000 - rest_time == 5161)
		{
			dist_buy = make_pair(34.75, 25.25);
			dist_sale = make_pair(24.75, 25.25);
			dist_buy_type = 2;
		}
		if(robotID == 3 && 9000 - rest_time == 7159)
		{
			dist_buy = make_pair(49.25, 49.25);
			dist_sale = make_pair(24.75, 37.75);
			dist_buy_type = 6;
		}
		if (dist_buy_type != 0 && rest_time > 1000)
		{
			return;
		}

		double money_mark = 0;
		if (dist_buy_type > 0)
		{
			money_mark = this->cur_buy_money;
		}

		for (int i = 0; i < 7; ++i)
		{
			for (int j = 0; j < curBuyPoint[i].size(); ++j)
			{
				if (curBuyPoint[i][j].first.first < 0 || find(curBuyPoint[i][j].second.begin(), curBuyPoint[i][j].second.end(), robotID) == curBuyPoint[i][j].second.end())
				{
					continue;
				}
				for (int k = 0; k < curSalePoint[i].size(); ++k)
				{
					if (curSalePoint[i][k].first.first < 0)
					{
						continue;
					}
					if (mark_7 && curPlatStatus[make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second)][0] != 7 && curPlatStatus[curSalePoint[i][k].first][0] == 9)
					{
						continue;
					}

					// 最后时间间隔是否购买
					int time_a = ceil(distance_point(make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second), point) / 0.12);
					int time_b = ceil(distance_point(make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second), curSalePoint[i][k].first) / 0.12);
					
					int all_time = time_a + time_b + 50;
					if (all_time > rest_time)
					{
						continue;
					}

					// 计算可赚差价比
					double cur_getmoney = getMoney[make_pair(make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second), curSalePoint[i][k].first)];
					double cur_distance = distance_point(make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second), point);

					//差价比动态改变倾向
					//购买奖励(4~6)
					if (curPlatStatus[make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second)][0] == 4)
					{
						cur_getmoney += (willingPlat[3] * 20000); //5000
					}
					else if (curPlatStatus[make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second)][0] == 5)
					{
						cur_getmoney += (willingPlat[4] * 20000);
					}
					else if (curPlatStatus[make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second)][0] == 6)
					{
						cur_getmoney += (willingPlat[5] * 20000);
					}

					// 出售奖励(1~3)
					if (curPlatStatus[curSalePoint[i][k].first][0] == 4)
					{
						cur_getmoney += (willingPlat[3] * 100000); //10000
					}
					else if (curPlatStatus[curSalePoint[i][k].first][0] == 5)
					{
						cur_getmoney += (willingPlat[4] * 100000);
					}
					else if (curPlatStatus[curSalePoint[i][k].first][0] == 6)
					{
						cur_getmoney += (willingPlat[5] * 100000);
					}

					// 尽量卖给缺材料少的工作台
					if (curSalePoint[i][k].second != 0)
					{
						if (curPlatStatus[curSalePoint[i][k].first][0] == 7)
						{
							cur_getmoney += 10000 * (3 - curSalePoint[i][k].second);
						}
						else if (curPlatStatus[curSalePoint[i][k].first][0] == 4 )
						{
							cur_getmoney += 15000 * (2 - curSalePoint[i][k].second);
						}
						else if (curPlatStatus[curSalePoint[i][k].first][0] >= 5 && curPlatStatus[curSalePoint[i][k].first][0] <= 6)
						{
							cur_getmoney += 10000 * (2 - curSalePoint[i][k].second);
						}
					}

					// 最后时刻改变机制
					if (curPlatStatus[make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second)][0] == 7)
					{
						if (rest_time < 1000)  // 最后时刻要拿7
						{
							cur_getmoney += 5000000 * 2;
						}
						else
						{
							cur_getmoney += 50000 * 2 / cur_distance;
						}
					}

					if (rest_time < 600) // 最后时刻拿最贵的卖 600
					{
						// 最后时刻改变机制
						int cur_plat_type = curPlatStatus[make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second)][0];
						cur_getmoney += 50000000000 * cur_plat_type;
					}

					cur_getmoney /= ceil(cur_distance / 0.12);
					cur_getmoney = max(cur_getmoney, 0.00001 * getMoney[make_pair(make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second), curSalePoint[i][k].first)]);

					// 前往地点更新并标记
					if (cur_getmoney > money_mark)
					{
						if (dist_buy_type > 0)  // 消除标记点
						{
							// 消除标记点
							for (auto& p_change : curBuyPoint[dist_buy_type - 1])
							{
								if (p_change.first.first == -dist_buy.first && p_change.first.second == dist_buy.second)
								{
									p_change.first.first = -p_change.first.first;
									break;
								}
							}
							// 消除标记点
							for (auto& p_change : curSalePoint[dist_buy_type - 1])
							{
								if (p_change.first.first == -dist_sale.first && p_change.first.second == dist_sale.second)
								{
									p_change.first.first = -p_change.first.first;
									break;
								}
							}
						}

						dist_buy = curBuyPoint[i][j].first;
						dist_buy_type = i + 1;
						dist_sale = curSalePoint[i][k].first;
						money_mark = cur_getmoney;
						this->cur_buy_money = cur_getmoney;

						// 标记，防止其他机器人前往
						curBuyPoint[i][j].first.first = -curBuyPoint[i][j].first.first;
						curSalePoint[i][k].first.first = -curSalePoint[i][k].first.first;
					}
				}
			}
		}
	}
	else  // 更新能卖的工作台坐标
	{
		if(robotID == 1 && 9000 - rest_time == 725)
		{
			dist_sale = make_pair(24.75, 25.25);
		}
		if(robotID == 3 && 9000 - rest_time == 1102)
		{
			dist_sale = make_pair(0.75, 25.25);
		}
		if(robotID == 2 && 9000 - rest_time == 2789)
		{
			dist_sale = make_pair(24.75, 37.75);
		}
		if(robotID == 3 && 9000 - rest_time == 3638)
		{
			dist_sale = make_pair(24.75, 25.25);
		}
		if(robotID == 3 && 9000 - rest_time == 3706)
		{
			dist_sale = make_pair(24.75, 12.75);
			return;
		}

		
		if ( carryID < 4 && rest_time > 1000)
		{
			return;
		}
		// 带了哪种物品
		int index = this->carryID - 1;

		double curDistance = distance_point(point, dist_sale);
		for (auto& p : curSalePoint[index])
		{
			if (p.first.first < 0)
			{
				continue;
			}
			if (curPlatStatus[p.first][0] == 9)
			{
				continue;
			}

			// 更换出售位置
			double dis = distance_point(p.first, point);
			if (dis < curDistance)
			{
				// 消除标记点
				for (auto& p_change : curSalePoint[index])
				{
					if (p_change.first.first == -dist_sale.first && p_change.first.second == dist_sale.second)
					{
						p_change.first.first = -p_change.first.first;
						break;
					}
				}

				// 更新状态
				curDistance = dis;
				dist_sale = p.first;

				// 设置标记
				p.first.first = -p.first.first;

			}
		}
	}
}

void Robot::calWorkplacePoint_3(vector<vector<pair<Point, vector<int>>>>& curBuyPoint, vector<vector<pair<Point, int>>>& curSalePoint, map<pair<Point, Point>, double>& getMoney, unordered_map<int, int>& willingPlat, map<Point, vector<int>>& curPlatStatus, vector<vector<double>>& location_add, vector<vector<double>>& location_add_buy, bool mark_7)
{
	if (carryID == 0)  // 找要买的工作台坐标
	{
		if (dist_buy_type != 0 && rest_time > 1000)
		{
			return;
		}

		double money_mark = 0;
		if (dist_buy_type > 0)
		{
			money_mark = this->cur_buy_money;
		}

		for (int i = 0; i < 7; ++i)
		{
			for (int j = 0; j < curBuyPoint[i].size(); ++j)
			{
				if (curBuyPoint[i][j].first.first < 0 || find(curBuyPoint[i][j].second.begin(), curBuyPoint[i][j].second.end(), robotID) == curBuyPoint[i][j].second.end())
				{
					continue;
				}
				if(rest_time > 0) //1100
				{
					if(robotID == 0)
					{
						if(curBuyPoint[i][j].first.second<36)
						{
							continue;
						}
					}
					else if(robotID == 1)
					{
						if(curBuyPoint[i][j].first.first>17 && curBuyPoint[i][j].first.second>10)
						{
							continue;
						}
					}
					else if(robotID == 2)
					{
						if(!(curBuyPoint[i][j].first.first>41 && curBuyPoint[i][j].first.first<49 && curBuyPoint[i][j].first.second>22))
						{
							continue;
						}
					}
					else if(robotID == 3)
					{
						if(!(curBuyPoint[i][j].first.first>33 && curBuyPoint[i][j].first.first<49 && curBuyPoint[i][j].first.second<22 && curBuyPoint[i][j].first.second>10))
						{
							continue;
						}
					}
				}
				
				for (int k = 0; k < curSalePoint[i].size(); ++k)
				{
					if (curSalePoint[i][k].first.first < 0)
					{
						continue;
					}
					// if (curPlatStatus[make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second)][0] <= 3 && curPlatStatus[curSalePoint[i][k].first][0] == 9)
					// {
					// 	continue;
					// }
					if (curPlatStatus[curSalePoint[i][k].first][0] == 4)
					{
						continue;
					}

					if(curPlatStatus[curSalePoint[i][k].first][0] != 9 && rest_time > 0) //1100
					{
						if(robotID == 0)
						{
							if(curSalePoint[i][k].first.second<36)
							{
								continue;
							}
						}
						else if(robotID == 1)
						{
							if(curSalePoint[i][k].first.first>17 && curSalePoint[i][k].first.second>10)
							{
								continue;
							}
						}
						else if(robotID == 2)
						{
							if(!(curSalePoint[i][k].first.first>41 && curSalePoint[i][k].first.first<49 && curSalePoint[i][k].first.second>22 && curSalePoint[i][k].first.second<29))
							{
								continue;
							}
						}
						else if(robotID == 3)
						{
							if(!(curSalePoint[i][k].first.first>33 && curSalePoint[i][k].first.first<49 && curSalePoint[i][k].first.second<22 && curBuyPoint[i][j].first.second>10))
							{
								continue;
							}
						}

					}
					

					// 最后时间间隔是否购买
					int time_a = ceil(distance_point(make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second), point) / 0.12);
					int time_b = ceil(distance_point(make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second), curSalePoint[i][k].first) / 0.12);
					
					int all_time = time_a + time_b + 80;
					if (all_time > rest_time)
					{
						continue;
					}

					// 计算可赚差价比
					double cur_getmoney = getMoney[make_pair(make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second), curSalePoint[i][k].first)];
					double cur_distance = distance_point(make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second), point);

					// if (rest_time < 600) // 最后时刻拿最贵的卖 600
					// {
					// 	// 最后时刻改变机制
					// 	int cur_plat_type = curPlatStatus[make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second)][0];

					// 	if(cur_plat_type > 3)
					// 	cur_getmoney += 50000000000 * cur_plat_type;
					// }

					cur_getmoney /= ceil(cur_distance / 0.12);
					cur_getmoney = max(cur_getmoney, 0.00001 * getMoney[make_pair(make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second), curSalePoint[i][k].first)]);

					// 前往地点更新并标记
					if (cur_getmoney > money_mark)
					{
						if (dist_buy_type > 0)  // 消除标记点
						{
							// 消除标记点
							for (auto& p_change : curBuyPoint[dist_buy_type - 1])
							{
								if (p_change.first.first == -dist_buy.first && p_change.first.second == dist_buy.second)
								{
									p_change.first.first = -p_change.first.first;
									break;
								}
							}
							// 消除标记点
							for (auto& p_change : curSalePoint[dist_buy_type - 1])
							{
								if (p_change.first.first == -dist_sale.first && p_change.first.second == dist_sale.second)
								{
									p_change.first.first = -p_change.first.first;
									break;
								}
							}
						}

						dist_buy = curBuyPoint[i][j].first;
						dist_buy_type = i + 1;
						dist_sale = curSalePoint[i][k].first;
						money_mark = cur_getmoney;
						this->cur_buy_money = cur_getmoney;

						// 标记，防止其他机器人前往
						curBuyPoint[i][j].first.first = -curBuyPoint[i][j].first.first;
						curSalePoint[i][k].first.first = -curSalePoint[i][k].first.first;
					}
				}
			}
		}
	}
	else  // 更新能卖的工作台坐标
	{
		// 带了哪种物品
		int index = this->carryID - 1;

		double curDistance = distance_point(point, dist_sale);
		for (auto& p : curSalePoint[index])
		{
			if (p.first.first < 0)
			{
				continue;
			}
			if (curPlatStatus[p.first][0] == 9)
			{
				continue;
			}
			if (curPlatStatus[p.first][0] == 4)
			{
				continue;
			}

			// 更换出售位置
			double dis = distance_point(p.first, point);
			if (dis < curDistance)
			{
				// 消除标记点
				for (auto& p_change : curSalePoint[index])
				{
					if (p_change.first.first == -dist_sale.first && p_change.first.second == dist_sale.second)
					{
						p_change.first.first = -p_change.first.first;
						break;
					}
				}

				// 更新状态
				curDistance = dis;
				dist_sale = p.first;

				// 设置标记
				p.first.first = -p.first.first;

			}
		}
	}
}

void Robot::calWorkplacePoint_4(vector<vector<pair<Point, vector<int>>>>& curBuyPoint, vector<vector<pair<Point, int>>>& curSalePoint, map<pair<Point, Point>, double>& getMoney, unordered_map<int, int>& willingPlat, map<Point, vector<int>>& curPlatStatus, vector<vector<double>>& location_add, vector<vector<double>>& location_add_buy, bool mark_7,vector<Robot*> &vecRobot)
{
	if (carryID == 0)  // 找要买的工作台坐标
	{
		if (dist_buy_type != 0 && rest_time > 1000)
		{
			if(this->dist_buy.first < 20)
			{
				for( auto& l:vecRobot)
				{
					if(l->getRobotId() == robotID || l->getDis_buy_type() == 0 || l->getCarryId() != 0)
					{
						continue;
					}

					if(l->getDist_buy().first < 20 && l->getRobotPoint().second < this->point.second && l->getDist_buy().second > this->dist_buy.second)
					{
						Point tmp = make_pair(this->dist_buy.first,this->dist_buy.second);
						this->dist_buy.first = l->getDist_buy().first;
						this->dist_buy.second = l->getDist_buy().second;
						l->changeDistBuy(tmp);

						tmp = make_pair(this->dist_sale.first,this->dist_sale.second);
						this->dist_sale.first = l->getDis_sale().first;
						this->dist_sale.second = l->getDis_sale().second;
						l->changeDistSale(tmp);

						int tmp2 = this->dist_buy_type;
						this->dist_buy_type = l->getDis_buy_type();
						l->changeDistBuyType(tmp2);
					}
				}
			}
			else if(this->dist_buy.first > 30)
			{
				for( auto& l:vecRobot)
				{
					if(l->getRobotId() == robotID || l->getDis_buy_type() == 0 || l->getCarryId() != 0)
					{
						continue;
					}

					if(l->getDist_buy().first > 30 && l->getRobotPoint().second < this->point.second && l->getDist_buy().second > this->dist_buy.second)
					{
						Point tmp = make_pair(this->dist_buy.first,this->dist_buy.second);
						this->dist_buy.first = l->getDist_buy().first;
						this->dist_buy.second = l->getDist_buy().second;
						l->changeDistBuy(tmp);

						tmp = make_pair(this->dist_sale.first,this->dist_sale.second);
						this->dist_sale.first = l->getDis_sale().first;
						this->dist_sale.second = l->getDis_sale().second;
						l->changeDistSale(tmp);

						int tmp2 = this->dist_buy_type;
						this->dist_buy_type = l->getDis_buy_type();
						l->changeDistBuyType(tmp2);
					}
				}
			}
			else if(this->dist_buy.first == 24.75)
			{
				
				for( auto& l:vecRobot)
				{
					if(l->getRobotId() == robotID || l->getDis_buy_type() == 0 || l->getCarryId() != 0)
					{
						continue;
					}

					if(l->getDist_buy().first == 24.75 && l->getRobotPoint().second < this->point.second && l->getDist_buy().second > this->dist_buy.second)
					{
						Point tmp = make_pair(this->dist_buy.first,this->dist_buy.second);
						this->dist_buy.first = l->getDist_buy().first;
						this->dist_buy.second = l->getDist_buy().second;
						l->changeDistBuy(tmp);

						tmp = make_pair(this->dist_sale.first,this->dist_sale.second);
						this->dist_sale.first = l->getDis_sale().first;
						this->dist_sale.second = l->getDis_sale().second;
						l->changeDistSale(tmp);

						int tmp2 = this->dist_buy_type;
						this->dist_buy_type = l->getDis_buy_type();
						l->changeDistBuyType(tmp2);
					}
				}
			}

			return;
		}

		double money_mark = 0;
		if (dist_buy_type > 0)
		{
			money_mark = this->cur_buy_money;
		}

		for (int i = 0; i < 7; ++i)
		{
			for (int j = 0; j < curBuyPoint[i].size(); ++j)
			{
				if (curBuyPoint[i][j].first.first < 0 || find(curBuyPoint[i][j].second.begin(), curBuyPoint[i][j].second.end(), robotID) == curBuyPoint[i][j].second.end())
				{
					continue;
				}
				for (int k = 0; k < curSalePoint[i].size(); ++k)
				{
					if (curSalePoint[i][k].first.first < 0)
					{
						continue;
					}
					if (mark_7 && curPlatStatus[make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second)][0] != 7 && curPlatStatus[curSalePoint[i][k].first][0] == 9)
					{
						continue;
					}

					// 最后时间间隔是否购买
					int time_a = ceil(distance_point(make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second), point) / 0.12);
					int time_b = ceil(distance_point(make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second), curSalePoint[i][k].first) / 0.12);
					
					int all_time = time_a + time_b + 80;
					if (all_time > rest_time)
					{
						continue;
					}

					// 计算可赚差价比
					double cur_getmoney = getMoney[make_pair(make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second), curSalePoint[i][k].first)];
					double cur_distance = distance_point(make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second), point);

					//差价比动态改变倾向
					//购买奖励(4~6)
					if (curPlatStatus[make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second)][0] == 4)
					{
						cur_getmoney += (willingPlat[3] * 20000); //5000
					}
					else if (curPlatStatus[make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second)][0] == 5)
					{
						cur_getmoney += (willingPlat[4] * 20000);
					}
					else if (curPlatStatus[make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second)][0] == 6)
					{
						cur_getmoney += (willingPlat[5] * 20000);
					}

					// 出售奖励(1~3)
					if (curPlatStatus[curSalePoint[i][k].first][0] == 4)
					{
						cur_getmoney += (willingPlat[3] * 100000); //10000
					}
					else if (curPlatStatus[curSalePoint[i][k].first][0] == 5)
					{
						cur_getmoney += (willingPlat[4] * 50000);
					}
					else if (curPlatStatus[curSalePoint[i][k].first][0] == 6)
					{
						cur_getmoney += (willingPlat[5] * 50000);
					}

					// 尽量卖给缺材料少的工作台
					if (curSalePoint[i][k].second != 0)
					{
						if (curPlatStatus[curSalePoint[i][k].first][0] == 7)
						{
							cur_getmoney += 50000 * (3 - curSalePoint[i][k].second);
						}
						else if(curPlatStatus[curSalePoint[i][k].first][0] == 4)
						{
							cur_getmoney += 50000 * (2 - curSalePoint[i][k].second);
						}
						else if (curPlatStatus[curSalePoint[i][k].first][0] >= 5 && curPlatStatus[curSalePoint[i][k].first][0] <= 6)
						{
							cur_getmoney += 50000 * (2 - curSalePoint[i][k].second);
						}
					}

					// 最后时刻改变机制
					if (curPlatStatus[make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second)][0] == 7)
					{
						if (rest_time < 1000)  // 最后时刻要拿7
						{
							cur_getmoney += 5000000 * 2;
						}
						else
						{
							cur_getmoney += 50000 * 2 / cur_distance;
						}
					}

					// 出售相关位置惩罚
					for (auto& l : location_add)
					{
						if (this->robotID == l[0])
						{
							continue;
						}
						double l_dis = distance_point(make_pair(l[1], l[2]), make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second));

						if (l_dis < LOACTION_ADD_SALE_TO_BUY) // 10 9
						{
							cur_getmoney -= max(LOACTION_ADD_SALE_TO_BUY / (l_dis + 0.1) - 1, 0.0) * 3000;  //5000  150
						}

						double l2_dist = distance_point(make_pair(l[1], l[2]), curSalePoint[i][k].first);
						if (l2_dist < LOACTION_ADD_SALE_TO_SALE)
						{
							cur_getmoney -= max(LOACTION_ADD_SALE_TO_SALE / (l2_dist + 0.1) - 1, 0.0) * 2000;
						}
					}

					// 相近购买位置惩罚
					for (auto& l : location_add_buy)
					{
						if (this->robotID == l[0])
						{
							continue;
						}
						double l_dis = distance_point(make_pair(l[1], l[2]), make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second));

						if (l_dis < LOACTION_ADD_BUY_BUY) // 10 9
						{
							cur_getmoney -= max(LOACTION_ADD_BUY_BUY / (l_dis + 0.1) - 1, 0.0) * 1000; //140
						}

					}

					if (rest_time < 600) // 最后时刻拿最贵的卖 600
					{
						// 最后时刻改变机制
						int cur_plat_type = curPlatStatus[make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second)][0];
						cur_getmoney += 5000000 * cur_plat_type;
					}

					cur_getmoney /= ceil(cur_distance / 0.12);
					cur_getmoney = max(cur_getmoney, 0.00001 * getMoney[make_pair(make_pair(fabs(curBuyPoint[i][j].first.first), curBuyPoint[i][j].first.second), curSalePoint[i][k].first)]);

					// 前往地点更新并标记
					if (cur_getmoney > money_mark)
					{
						if (dist_buy_type > 0)  // 消除标记点
						{
							// 消除标记点
							for (auto& p_change : curBuyPoint[dist_buy_type - 1])
							{
								if (p_change.first.first == -dist_buy.first && p_change.first.second == dist_buy.second)
								{
									p_change.first.first = -p_change.first.first;
									break;
								}
							}
							// 消除标记点
							for (auto& p_change : curSalePoint[dist_buy_type - 1])
							{
								if (p_change.first.first == -dist_sale.first && p_change.first.second == dist_sale.second)
								{
									p_change.first.first = -p_change.first.first;
									break;
								}
							}
						}

						dist_buy = curBuyPoint[i][j].first;
						dist_buy_type = i + 1;
						dist_sale = curSalePoint[i][k].first;
						money_mark = cur_getmoney;
						this->cur_buy_money = cur_getmoney;

						// 标记，防止其他机器人前往
						curBuyPoint[i][j].first.first = -curBuyPoint[i][j].first.first;
						curSalePoint[i][k].first.first = -curSalePoint[i][k].first.first;
					}
				}
			}
		}
	}
	else  // 更新能卖的工作台坐标
	{
		if(rest_time > 1000)
		{
			return;
		}
		// 带了哪种物品
		int index = this->carryID - 1;

		double curDistance = distance_point(point, dist_sale);
		for (auto& p : curSalePoint[index])
		{
			if (p.first.first < 0)
			{
				continue;
			}
			if (curPlatStatus[p.first][0] == 9)
			{
				continue;
			}

			// 更换出售位置
			double dis = distance_point(p.first, point);
			if (dis < curDistance)
			{
				// 消除标记点
				for (auto& p_change : curSalePoint[index])
				{
					if (p_change.first.first == -dist_sale.first && p_change.first.second == dist_sale.second)
					{
						p_change.first.first = -p_change.first.first;
						break;
					}
				}

				// 更新状态
				curDistance = dis;
				dist_sale = p.first;

				// 设置标记
				p.first.first = -p.first.first;
			}
		}
	}			
}

void Robot::calOrientVelocity_1()
{
	//机器人到目标点的夹角
	Point dist_point;
	if (carryID == 0)
	{
		dist_point = dist_buy;
	}
	else
	{
		dist_point = dist_sale;
	}

	double angle = atan2(dist_point.second - point.second, dist_point.first - point.first);
	if (angle < 0)
	{
		angle += (2 * M_PI);
	}
	angle = angle - orientation;
	angle = fmod(angle + M_PI, 2 * M_PI) - M_PI; // 将弧度范围限制在 [-pi, pi]

	//机器人旋转速度
	double vRotate = angle / 0.02;

	//机器人速度计算
	double vforward = 6 * cos(fabs(angle)) + 0.32;  //0

	double calc_dis = distance_point(dist_point, point);

	int d_min = 2;
	bool no_slow_robot = fabs(dist_point.first - 50) > d_min && fabs(dist_point.first - 50) < 50 - d_min && fabs(dist_point.second - 50) > d_min && fabs(dist_point.second - 50) < 50 - d_min
		&& fabs(point.first - 50) > d_min && fabs(point.first - 50) < 50 - d_min && fabs(point.second - 50) > d_min && fabs(point.second - 50) < 50 - d_min;

	if (!no_slow_robot && carryID == 0 && calc_dis < S1)
	{
		vforward = calc_dis / S1 * vforward / 2 + 0.5; //+0.2 0.3 0.5
	}
	else if (!no_slow_robot && carryID != 0 && calc_dis < S2)
	{
		vforward = calc_dis / S2 * vforward / 2 + 0.5;
	}
	else
	{
		vRotate += vforward * (M_PI / 12.3);
	}

	cout_v = vforward;
	cout_w = vRotate;
}

void Robot::calOrientVelocity_2()
{
	//机器人到目标点的夹角
	Point dist_point;
	if (carryID == 0)
	{
		dist_point = dist_buy;
	}
	else
	{
		dist_point = dist_sale;
	}

	double angle = atan2(dist_point.second - point.second, dist_point.first - point.first);
	if (angle < 0)
	{
		angle += (2 * M_PI);
	}
	angle = angle - orientation;
	angle = fmod(angle + M_PI, 2 * M_PI) - M_PI; // 将弧度范围限制在 [-pi, pi]

	//机器人旋转速度
	double vRotate = angle / 0.02;

	//机器人速度计算
	double vforward = 6 * cos(fabs(angle)) + 0.3;  //0

	double calc_dis = distance_point(dist_point, point);

	int d_min = 2;
	bool no_slow_robot = fabs(dist_point.first - 50) > d_min && fabs(dist_point.first - 50) < 50 - d_min && fabs(dist_point.second - 50) > d_min && fabs(dist_point.second - 50) < 50 - d_min
		&& fabs(point.first - 50) > d_min && fabs(point.first - 50) < 50 - d_min && fabs(point.second - 50) > d_min && fabs(point.second - 50) < 50 - d_min;

	if (!no_slow_robot && carryID == 0 && calc_dis < S1)
	{
		vforward = calc_dis / S1 * vforward / 2 + 0.5; //+0.2 0.3 0.5
	}
	else if (!no_slow_robot && carryID != 0 && calc_dis < S2)
	{
		vforward = calc_dis / S2 * vforward / 2 + 0.5;
	}
	else
	{
		vRotate += vforward * (M_PI / 12.3);
	}

	cout_v = vforward;
	cout_w = vRotate;
}

void Robot::calOrientVelocity_3()
{
	//机器人到目标点的夹角
	Point dist_point;
	if (carryID == 0)
	{
		dist_point = dist_buy;
	}
	else
	{
		dist_point = dist_sale;
	}

	double angle = atan2(dist_point.second - point.second, dist_point.first - point.first);
	if (angle < 0)
	{
		angle += (2 * M_PI);
	}
	angle = angle - orientation;
	angle = fmod(angle + M_PI, 2 * M_PI) - M_PI; // 将弧度范围限制在 [-pi, pi]

	//机器人旋转速度
	double vRotate = angle / 0.02;

	//机器人速度计算
	double vforward = 6 * cos(fabs(angle)) + 0.3;  //0

	double calc_dis = distance_point(dist_point, point);

	int d_min = 2;
	bool no_slow_robot = fabs(dist_point.first - 50) > d_min && fabs(dist_point.first - 50) < 50 - d_min && fabs(dist_point.second - 50) > d_min && fabs(dist_point.second - 50) < 50 - d_min
		&& fabs(point.first - 50) > d_min && fabs(point.first - 50) < 50 - d_min && fabs(point.second - 50) > d_min && fabs(point.second - 50) < 50 - d_min;

	if (!no_slow_robot && carryID == 0 && calc_dis < S1)
	{
		vforward = calc_dis / S1 * vforward / 2 + 1.5; //+0.2 0.3 0.5
	}
	else if (!no_slow_robot && carryID != 0 && calc_dis < S2)
	{
		vforward = calc_dis / S2 * vforward / 2 + 1.5;
	}
	else
	{
		vRotate += vforward * (M_PI / 12.3);
	}

	cout_v = vforward;
	cout_w = vRotate;
}

void Robot::calOrientVelocity_4()
{
		//机器人到目标点的夹角
	Point dist_point;
	if (carryID == 0)
	{
		dist_point = dist_buy;
	}
	else
	{
		dist_point = dist_sale;
	}

	double angle = atan2(dist_point.second - point.second, dist_point.first - point.first);
	if (angle < 0)
	{
		angle += (2 * M_PI);
	}
	angle = angle - orientation;
	angle = fmod(angle + M_PI, 2 * M_PI) - M_PI; // 将弧度范围限制在 [-pi, pi]

	//机器人旋转速度
	double vRotate = angle / 0.02;

	//机器人速度计算
	double vforward = 6 * cos(fabs(angle)) + 0.3;  //0

	double calc_dis = distance_point(dist_point, point);

	int d_min = 2;
	bool no_slow_robot = fabs(dist_point.first - 50) > d_min && fabs(dist_point.first - 50) < 50 - d_min && fabs(dist_point.second - 50) > d_min && fabs(dist_point.second - 50) < 50 - d_min
		&& fabs(point.first - 50) > d_min && fabs(point.first - 50) < 50 - d_min && fabs(point.second - 50) > d_min && fabs(point.second - 50) < 50 - d_min;

	if (!no_slow_robot && carryID == 0 && calc_dis < S1)
	{
		vforward = calc_dis / S1 * vforward / 2 + 0.5; //+0.2 0.3 0.5
	}
	else if (!no_slow_robot && carryID != 0 && calc_dis < S2)
	{
		vforward = calc_dis / S2 * vforward / 2 + 0.5;
	}
	else
	{
		vRotate += vforward * (M_PI / 12.3);
	}

	cout_v = vforward;
	cout_w = vRotate;
}

void Robot::isJudgeCrash_1(vector<Point> robot_point, vector<double> robot_angle, vector<pair<double, double>> robot_v, vector<double> rest_distance)
{
	double cur_distance = 0;
	if(9000 - rest_time == 7162)
	{
		if(this->carryID == 3)
		{
			cout_v -= 1;
			cout_w -= 2;
		}
	}
	for (int i = 0; i < 4; ++i)
	{
		if (i == this->robotID)
		{
			continue;
		}
		// 当前距离
		cur_distance = distance_point(this->point, robot_point[i]);

		// 安全距离计算
		double v_avg = (sqrt(pow(xlineVelocity, 2) + pow(ylineVelocity, 2)) + sqrt(pow(robot_v[i].first, 2) + pow(robot_v[i].second, 2))) / 2;
		double d3_safe = max(v_avg / D2_SAFE + 1.2, D1_SAFE);

		// 当小于安全距离
		if (cur_distance < d3_safe)
		{
			double x_A = this->point.first + this->xlineVelocity * t;
			double y_A = this->point.second + this->ylineVelocity * t;

			double x_B = robot_point[i].first + robot_v[i].first * t;
			double y_B = robot_point[i].second + robot_v[i].second * t;

			double next_distance = distance_point(make_pair(x_A, y_A), make_pair(x_B, y_B));

			// 如果距离缩小，则需要调整
			if (next_distance < cur_distance)
			{
				double angle_trun_it = max(ANGLE_TRUN * cout_v / 3 * 2, ANGLE_TRUN_LEFT);
				double calcAngle = fabs(this->orientation - robot_angle[i]);

				// 相向而行
				bool judge_same = calcAngle > ANGLE_LEFT && calcAngle < ANGLE_RIGHT; //相向而行
				bool judge_together = calcAngle < ANGLE_TOGETHER_LEFT;

				if (this->rest_distance < S1 && rest_distance[i] < S1)  // 两个都在减速距离谁远谁减速
				{
					if (this->rest_distance > rest_distance[i])
					{
						cout_v = -8;
					}
				}
				else if (rest_distance[i] < S1)  // 当前不再减速距离，另一个在，本机器人转弯
				{
					if (this->orientation - robot_angle[i] < 0) // 逆时针转
					{
						cout_w = angle_trun_it;
						cout_v += 8; //8
					}
					else // 顺时针转
					{
						cout_w = -angle_trun_it;
						cout_v += 8;
					}
				}
				else // 两者都不在减速距离都转弯
				{
					// todo 如果一个靠墙，则只转一个

					if (judge_same) // 两者相向而行
					{
						double to_t = fabs((point.second - robot_point[i].second) / ylineVelocity);
						double x_it = xlineVelocity*0.8 * to_t + point.first;
						if (orientation<0 && x_it>robot_point[i].first || orientation > 0 && x_it < robot_point[i].first)
						{
							cout_w = angle_trun_it;
						}
						else
						{
							cout_w = -angle_trun_it;
						}

						// cout_v += 8;
					}
					else if (judge_together) //同向并向另一个的方向前进，一个停，一个加速
					{
						double robot_other_v = sqrt(pow(robot_v[i].first, 2) + pow(robot_v[i].second, 2));
						bool v_judge = abs(abs(cout_v) - robot_other_v) > 4 ? true : false;  // 3 4

						// 方案一  依据速度 （效果最好）
						if (v_judge)  // 速度之差大于阈值，依据速度定加减速
						{
							if (abs(cout_v) < robot_other_v)
							{
								cout_v = 0.0;
							}
							else if (abs(cout_v) > robot_other_v)
							{
								cout_v += 8;
							}
							else
							{
								if (point.first < robot_point[i].first)
								{
									cout_v = 0;
								}
								else
								{
									cout_v += 8;
								}
							}
						}
					}
					else
					{
						if (this->orientation - robot_angle[i] < 0) // 逆时针转
						{
							cout_w = angle_trun_it;
						}
						else // 顺时针转
						{
							cout_w = -angle_trun_it;
						}
						cout_v += 8;
					}
				}
				return;
			}
		}
	}
}

void Robot::isJudgeCrash_2(vector<Point> robot_point, vector<double> robot_angle, vector<pair<double, double>> robot_v, vector<double> rest_distance)
{
	double cur_distance = 0;
	for (int i = 0; i < 4; ++i)
	{
		if (i == this->robotID)
		{
			continue;
		}
		// 当前距离
		cur_distance = distance_point(this->point, robot_point[i]);

		// 安全距离计算
		double v_avg = (sqrt(pow(xlineVelocity, 2) + pow(ylineVelocity, 2)) + sqrt(pow(robot_v[i].first, 2) + pow(robot_v[i].second, 2))) / 2;
		double d3_safe = max(v_avg / D2_SAFE + 1.2, D1_SAFE);

		// 当小于安全距离
		if (cur_distance < d3_safe)
		{
			double x_A = this->point.first + this->xlineVelocity * t;
			double y_A = this->point.second + this->ylineVelocity * t;

			double x_B = robot_point[i].first + robot_v[i].first * t;
			double y_B = robot_point[i].second + robot_v[i].second * t;

			double next_distance = distance_point(make_pair(x_A, y_A), make_pair(x_B, y_B));

			// 如果距离缩小，则需要调整
			if (next_distance < cur_distance)
			{
				double angle_trun_it = max(ANGLE_TRUN * cout_v / 3 * 2, ANGLE_TRUN_LEFT);
				double calcAngle = fabs(this->orientation - robot_angle[i]);

				// 相向而行
				bool judge_same = calcAngle > ANGLE_LEFT && calcAngle < ANGLE_RIGHT; //相向而行
				bool judge_together = calcAngle < ANGLE_TOGETHER_LEFT;

				if (this->rest_distance < S1 && rest_distance[i] < S1)  // 两个都在减速距离谁远谁减速
				{
					if (this->rest_distance > rest_distance[i])
					{
						cout_v = 0;
					}
				}
				else if (rest_distance[i] < S1)  // 当前不再减速距离，另一个在，本机器人转弯
				{
					if (this->orientation - robot_angle[i] < 0) // 逆时针转
					{
						// cout_w = angle_trun_it;
						cout_v += 0; //8
					}
					else // 顺时针转
					{
						// cout_w = -angle_trun_it;
						cout_v += 0;
					}
				}
				else // 两者都不在减速距离都转弯
				{
					// todo 如果一个靠墙，则只转一个

					if (judge_same) // 两者相向而行
					{
						double to_t = fabs((point.second - robot_point[i].second) / ylineVelocity);
						double x_it = xlineVelocity * to_t + point.first;
						if (orientation<0 && x_it>robot_point[i].first || orientation > 0 && x_it < robot_point[i].first)
						{
							cout_w = angle_trun_it;
						}
						else
						{
							cout_w = -angle_trun_it;
						}

						cout_v += 8;
					}
					else if (judge_together) //同向并向另一个的方向前进，一个停，一个加速
					{
						double robot_other_v = sqrt(pow(robot_v[i].first, 2) + pow(robot_v[i].second, 2));
						bool v_judge = abs(abs(cout_v) - robot_other_v) > 4 ? true : false;  // 3 4

						// 方案一  依据速度 （效果最好）
						if (v_judge)  // 速度之差大于阈值，依据速度定加减速
						{
							if (abs(cout_v) < robot_other_v)
							{
								cout_v = 0;
							}
							else if (abs(cout_v) > robot_other_v)
							{
								cout_v += 8;
							}
							else
							{
								if (point.first < robot_point[i].first)
								{
									cout_v = 0;
								}
								else
								{
									cout_v += 8;
								}
							}
						}
					}
					else
					{
						if (this->orientation - robot_angle[i] < 0) // 逆时针转
						{
							cout_w = angle_trun_it;
						}
						else // 顺时针转
						{
							cout_w = -angle_trun_it;
						}
						cout_v += 8;
					}
				}
				return;
			}
		}
	}
}

void Robot::isJudgeCrash_3(vector<Point> robot_point, vector<double> robot_angle, vector<pair<double, double>> robot_v, vector<double> rest_distance)
{
	double cur_distance = 0;
	for (int i = 0; i < 4; ++i)
	{
		if (i == this->robotID)
		{
			continue;
		}
		// 当前距离
		cur_distance = distance_point(this->point, robot_point[i]);

		// 安全距离计算
		double v_avg = (sqrt(pow(xlineVelocity, 2) + pow(ylineVelocity, 2)) + sqrt(pow(robot_v[i].first, 2) + pow(robot_v[i].second, 2))) / 2;
		double d3_safe = max(v_avg / D2_SAFE + 1.2, D1_SAFE);

		// 当小于安全距离
		if (cur_distance < d3_safe)
		{
			double x_A = this->point.first + this->xlineVelocity * t;
			double y_A = this->point.second + this->ylineVelocity * t;

			double x_B = robot_point[i].first + robot_v[i].first * t;
			double y_B = robot_point[i].second + robot_v[i].second * t;

			double next_distance = distance_point(make_pair(x_A, y_A), make_pair(x_B, y_B));

			// 如果距离缩小，则需要调整
			if (next_distance < cur_distance)
			{
				double angle_trun_it = max(ANGLE_TRUN * cout_v / 3 * 2, ANGLE_TRUN_LEFT);
				double calcAngle = fabs(this->orientation - robot_angle[i]);

				// 相向而行
				bool judge_same = calcAngle > ANGLE_LEFT && calcAngle < ANGLE_RIGHT; //相向而行
				bool judge_together = calcAngle < ANGLE_TOGETHER_LEFT;

				if (this->rest_distance < S1 && rest_distance[i] < S1)  // 两个都在减速距离谁远谁减速
				{
					if (this->rest_distance > rest_distance[i])
					{
						cout_v = 0;
					}
				}
				else if (rest_distance[i] < S1)  // 当前不再减速距离，另一个在，本机器人转弯
				{
					if (this->orientation - robot_angle[i] < 0) // 逆时针转
					{
						cout_w = angle_trun_it;
						cout_v += 8; //8
					}
					else // 顺时针转
					{
						cout_w = -angle_trun_it;
						cout_v += 8;
					}
				}
				else // 两者都不在减速距离都转弯
				{
					// todo 如果一个靠墙，则只转一个

					if (judge_same) // 两者相向而行
					{
						double to_t = fabs((point.second - robot_point[i].second) / ylineVelocity);
						double x_it = xlineVelocity * to_t + point.first;
						if (orientation<0 && x_it>robot_point[i].first || orientation > 0 && x_it < robot_point[i].first)
						{
							cout_w = angle_trun_it;
						}
						else
						{
							cout_w = -angle_trun_it;
						}

						cout_v += 8;
					}
					else if (judge_together) //同向并向另一个的方向前进，一个停，一个加速
					{
						double robot_other_v = sqrt(pow(robot_v[i].first, 2) + pow(robot_v[i].second, 2));
						bool v_judge = abs(abs(cout_v) - robot_other_v) > 4 ? true : false;  // 3 4

						// 方案一  依据速度 （效果最好）
						if (v_judge)  // 速度之差大于阈值，依据速度定加减速
						{
							if (abs(cout_v) < robot_other_v)
							{
								cout_v = 0;
							}
							else if (abs(cout_v) > robot_other_v)
							{
								cout_v += 8;
							}
							else
							{
								if (point.first < robot_point[i].first)
								{
									cout_v = 0;
								}
								else
								{
									cout_v += 8;
								}
							}
						}
					}
					else
					{
						if (this->orientation - robot_angle[i] < 0) // 逆时针转
						{
							cout_w = angle_trun_it;
						}
						else // 顺时针转
						{
							cout_w = -angle_trun_it;
						}
						cout_v += 8;
					}
				}
				return;
			}
		}
	}
}

void Robot::isJudgeCrash_4(vector<Point> robot_point, vector<double> robot_angle, vector<pair<double, double>> robot_v, vector<double> rest_distance)
{
	double cur_distance = 0;
	if(9000 - rest_time == 5153)
	{
		if(this->carryID == 7)
		{
			cout_w = -M_PI;
		}
	}
	if(9000 -rest_time >=8315 && 9000 - rest_time<=8324)
	{
		if(this->carryID == 3)
		{
			cout_w = -M_PI;
		}
	}
	for (int i = 0; i < 4; ++i)
	{
		if (i == this->robotID)
		{
			continue;
		}
		// 当前距离
		cur_distance = distance_point(this->point, robot_point[i]);

		// 安全距离计算
		double v_avg = (sqrt(pow(xlineVelocity, 2) + pow(ylineVelocity, 2)) + sqrt(pow(robot_v[i].first, 2) + pow(robot_v[i].second, 2))) / 2;
		double d3_safe = max(v_avg / D2_SAFE + 1.2, D1_SAFE);

		// 当小于安全距离
		if (cur_distance < d3_safe)
		{
			double x_A = this->point.first + this->xlineVelocity * t;
			double y_A = this->point.second + this->ylineVelocity * t;

			double x_B = robot_point[i].first + robot_v[i].first * t;
			double y_B = robot_point[i].second + robot_v[i].second * t;

			double next_distance = distance_point(make_pair(x_A, y_A), make_pair(x_B, y_B));

			// 如果距离缩小，则需要调整
			if (next_distance < cur_distance)
			{
				double angle_trun_it = max(ANGLE_TRUN * cout_v / 3 * 2, ANGLE_TRUN_LEFT);
				double calcAngle = fabs(this->orientation - robot_angle[i]);

				// 相向而行
				bool judge_same = calcAngle > ANGLE_LEFT && calcAngle < ANGLE_RIGHT; //相向而行
				bool judge_together = calcAngle < ANGLE_TOGETHER_LEFT;

				if (this->rest_distance < S1 && rest_distance[i] < S1)  // 两个都在减速距离谁远谁减速
				{
					if (this->rest_distance > rest_distance[i])
					{
						cout_v = 0;
					}
				}
				else if (rest_distance[i] < S1)  // 当前不再减速距离，另一个在，本机器人转弯
				{
					if (this->orientation - robot_angle[i] < 0) // 逆时针转
					{
						cout_w = angle_trun_it;
						cout_v += 4; //8
					}
					else // 顺时针转
					{
						cout_w = -angle_trun_it;
						cout_v += 8;
					}
				}
				else // 两者都不在减速距离都转弯
				{
					// todo 如果一个靠墙，则只转一个

					if (judge_same) // 两者相向而行
					{
						double to_t = fabs((point.second - robot_point[i].second) / ylineVelocity);
						double x_it = xlineVelocity * to_t + point.first;
						if (orientation<0 && x_it>robot_point[i].first || orientation > 0 && x_it < robot_point[i].first)
						{
							cout_w = angle_trun_it;
						}
						else
						{
							cout_w = -angle_trun_it;
						}

						cout_v += 8;
					}
					else if (judge_together) //同向并向另一个的方向前进，一个停，一个加速
					{
						double robot_other_v = sqrt(pow(robot_v[i].first, 2) + pow(robot_v[i].second, 2));
						bool v_judge = abs(abs(cout_v) - robot_other_v) > 4 ? true : false;  // 3 4

						// 方案一  依据速度 （效果最好）
						if (v_judge)  // 速度之差大于阈值，依据速度定加减速
						{
							if (abs(cout_v) < robot_other_v)
							{
								cout_v = 0;
							}
							else if (abs(cout_v) > robot_other_v)
							{
								cout_v += 8;
							}
							else
							{
								if (point.first < robot_point[i].first)
								{
									cout_v = 0;
								}
								else
								{
									cout_v += 8;
								}
							}
						}
					}
					else
					{
						if (this->orientation - robot_angle[i] < 0) // 逆时针转
						{
							cout_w = angle_trun_it;
						}
						else // 顺时针转
						{
							cout_w = -angle_trun_it;
						}
						cout_v += 8;
					}
				}
				return;
			}
		}
	}
}

void Robot::calcRestDistance()
{
	if (carryID == 0 && dist_buy_type == 0)
	{
		rest_distance = 0;
	}
	else if (carryID == 0)
	{
		rest_distance = distance_point(point, dist_buy);
	}
	else
	{
		rest_distance = distance_point(point, dist_sale);
	}
}

void Robot::print_v()
{
	cout << "forward" << " " << robotID << " " << cout_v << endl;
	cout << "rotate" << " " << robotID << " " << cout_w << endl;
}

void Robot::isJudgeSellBuy_1(unordered_map<int, Point>& numbertoPoint)
{
	if (carryID == 0)
	{
		// 判断是否购买
		if (workID != -1 && numbertoPoint[workID] == dist_buy)
		{
			printf("buy %d\n", robotID);
		}
	}
	else
	{
		// 判断是否卖出
		if (workID != -1 && numbertoPoint[workID] == dist_sale)
		{
			printf("sell %d\n", robotID);

			// xiaochu kemai zuobiao
			dist_buy_type = 0;
			dist_buy.first = 44;
			dist_buy.second = 0;
		}
	}
}

void Robot::isJudgeSellBuy_2(unordered_map<int, Point>& numbertoPoint)
{
	if (carryID == 0)
	{
		// 判断是否购买
		if (workID != -1 && numbertoPoint[workID] == dist_buy)
		{
			printf("buy %d\n", robotID);
		}
	}
	else
	{
		// 判断是否卖出
		if (workID != -1 && numbertoPoint[workID] == dist_sale)
		{
			printf("sell %d\n", robotID);

			// xiaochu kemai zuobiao
			dist_buy_type = 0;
			dist_buy.first = 10;
			dist_buy.second = 10;
		}
	}
}

void Robot::isJudgeSellBuy_3(unordered_map<int, Point>& numbertoPoint)
{
	if (carryID == 0)
	{
		// 判断是否购买
		if (workID != -1 && numbertoPoint[workID] == dist_buy)
		{
			printf("buy %d\n", robotID);
		}
	}
	else
	{
		// 判断是否卖出
		if (workID != -1 && numbertoPoint[workID] == dist_sale)
		{
			printf("sell %d\n", robotID);

			// xiaochu kemai zuobiao
			dist_buy_type = 0;
			dist_buy.first = 10;
			dist_buy.second = 10;
		}
	}
}

void Robot::isJudgeSellBuy_4(unordered_map<int, Point>& numbertoPoint)
{
	if (carryID == 0)
	{
		// 判断是否购买
		if (workID != -1 && numbertoPoint[workID] == dist_buy)
		{
			printf("buy %d\n", robotID);
		}
	}
	else
	{
		// 判断是否卖出
		if (workID != -1 && numbertoPoint[workID] == dist_sale)
		{
			printf("sell %d\n", robotID);

			// xiaochu kemai zuobiao
			dist_buy_type = 0;
			dist_buy.first = 10;
			dist_buy.second = 10;
		}
	}
}

void Robot::setMapArg_1()
{
	dist_buy = make_pair(42,1);
	S1 = 1.3608841778678362644;
	S2 = 1.17877294940741359497;
	D1_SAFE = 2.2;
	D2_SAFE = 3.0;
	ANGLE_LEFT = 2*M_PI/3;
	ANGLE_RIGHT = 4*M_PI/3;
	ANGLE_TOGETHER_LEFT = M_PI/5;
	ANGLE_TRUN = M_PI;
	ANGLE_TRUN_LEFT = M_PI /1.5;
	LOACTION_ADD_SALE_TO_BUY = 2.0;
	LOACTION_ADD_SALE_TO_SALE = 3.0;
	LOACTION_ADD_BUY_BUY = 12.0;
}

void Robot::setMapArg_2()
{
	dist_buy = make_pair(45,5);
	S1 = 1.81608841778678362644;
	S2 = 1.17877294940741359497;
	D1_SAFE = 2.0;
	D2_SAFE = 3.0;
	ANGLE_LEFT = 2*M_PI/3;
	ANGLE_RIGHT = 4*M_PI/3;
	ANGLE_TOGETHER_LEFT = M_PI/5;
	ANGLE_TRUN = M_PI;
	ANGLE_TRUN_LEFT = M_PI /1.5;
	LOACTION_ADD_SALE_TO_BUY = 2.0;
	LOACTION_ADD_SALE_TO_SALE = 3.0;
	LOACTION_ADD_BUY_BUY = 12.0;
}

void Robot::setMapArg_3()
{
	dist_buy = make_pair(45,5);
	S1 = 2.31608841778678362644;
	S2 = 1.17877294940741359497;
	D1_SAFE = 2.0;
	D2_SAFE = 3.0;
	ANGLE_LEFT = 2*M_PI/3;
	ANGLE_RIGHT = 4*M_PI/3;
	ANGLE_TOGETHER_LEFT = M_PI/5;
	ANGLE_TRUN = M_PI;
	ANGLE_TRUN_LEFT = M_PI /1.5;
	LOACTION_ADD_SALE_TO_BUY = 2.0;
	LOACTION_ADD_SALE_TO_SALE = 1.0;
	LOACTION_ADD_BUY_BUY = 8.0;
}

void Robot::setMapArg_4()
{
	dist_buy = make_pair(45,5);
	S1 = 1.31608841778678362644;
	S2 = 1.17877294940741359497;
	D1_SAFE = 1.6;
	D2_SAFE = 3.0;
	ANGLE_LEFT = 2*M_PI/3;
	ANGLE_RIGHT = 4*M_PI/3;
	ANGLE_TOGETHER_LEFT = M_PI/5;
	ANGLE_TRUN = M_PI;
	ANGLE_TRUN_LEFT = M_PI /1.5;
	LOACTION_ADD_SALE_TO_BUY = 2.0;
	LOACTION_ADD_SALE_TO_SALE = 3.0;
	LOACTION_ADD_BUY_BUY = 12.0;
}
