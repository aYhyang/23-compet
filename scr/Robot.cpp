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

int Robot::getRobotId()
{
	return robotID;
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

Point Robot::getRobotNPoint()
{
	return nPoint;
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

void Robot::changeRestTime(int cur_time)
{
	rest_time = 9000 - cur_time;
}

void Robot::changeV(double newv)
{
	cout_v *= newv;
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

void Robot::calWorkplacePoint(vector<vector<buyPoint>>& curBuyPoint, vector<vector<pair<Point, int>>>& curSalePoint, map<pair<Point, Point>, double>& getMoney, unordered_map<int, int>& willingPlat, map<Point, vector<int>>& curPlatStatus, vector<vector<double>>& location_add, vector<vector<double>>& location_add_buy, bool mark_7,vector<Robot*> &vecRobot)
{
	if (carryID == 0)  // 找要买的工作台坐标
	{
		// 不可改变购买点，可改进（优化算法去掉）
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
				if (curBuyPoint[i][j].nextBuyRobotId != 0)
				{
					continue;
				}
				else if(curBuyPoint[i][j].nextBuyRobotId == 0 && curBuyPoint[i][j].toBuyRobotId != 0 && find(curBuyPoint[i][j].nextBuyRobot.begin(), curBuyPoint[i][j].nextBuyRobot.end(), robotID+1) == curBuyPoint[i][j].nextBuyRobot.end())
				{
					continue;
				}
				else if(curBuyPoint[i][j].toBuyRobotId == 0 && find(curBuyPoint[i][j].canBuyRobot.begin(), curBuyPoint[i][j].canBuyRobot.end(), robotID+1) == curBuyPoint[i][j].canBuyRobot.end())
				{
					continue;
				}

				int save_mark = 0;
				if(curBuyPoint[i][j].toBuyRobotId == 0)
				{
					save_mark = 0;
				}
				else
				{
					save_mark = 1;
				}
				for (int k = 0; k < curSalePoint[i].size(); ++k)
				{
					if (curSalePoint[i][k].first.first < 0)
					{
						continue;
					}

					// 若有9号工作台且有7号工作台时，不向9卖1～6材料
					if (mark_7 && curPlatStatus[curBuyPoint[i][j].p][0] != 7 && curPlatStatus[curSalePoint[i][k].first][0] == 9)
					{
						continue;
					}

					// 最后时间间隔是否购买
					int time_a = ceil(distance_point(curBuyPoint[i][j].p, point) / 0.12);
					int time_b = ceil(distance_point(curBuyPoint[i][j].p, curSalePoint[i][k].first) / 0.12);
					
					int all_time = time_a + time_b + 80;
					if (all_time > rest_time)
					{
						continue;
					}

					// 得到差价比和距可买工作台距离
					double cur_getmoney = getMoney[make_pair(curBuyPoint[i][j].p, curSalePoint[i][k].first)];
					double cur_distance = distance_point(curBuyPoint[i][j].p, point);

					//差价比动态改变倾向
					//购买奖励(4~6)
					if (curPlatStatus[curBuyPoint[i][j].p][0] == 4)
					{
						cur_getmoney += (willingPlat[3] * 20000); //5000
					}
					else if (curPlatStatus[curBuyPoint[i][j].p][0] == 5)
					{
						cur_getmoney += (willingPlat[4] * 20000);
					}
					else if (curPlatStatus[curBuyPoint[i][j].p][0] == 6)
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
					if (curPlatStatus[curBuyPoint[i][j].p][0] == 7)
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
						int cur_plat_type = curPlatStatus[curBuyPoint[i][j].p][0];
						cur_getmoney += 5000000 * cur_plat_type;
					}

					cur_getmoney /= ceil(cur_distance / 0.12);
					cur_getmoney = max(cur_getmoney, 0.00001 * getMoney[make_pair(curBuyPoint[i][j].p, curSalePoint[i][k].first)]);

					// 前往地点更新并标记
					if (cur_getmoney > money_mark)
					{
						if (dist_buy_type > 0)  // 消除标记点
						{
							// 消除标记点
							for (auto& p_change : curBuyPoint[dist_buy_type - 1])
							{
								if (p_change.p == dist_buy)
								{
									if(p_change.nextBuyRobotId == robotID + 1)
									{
										p_change.nextBuyRobotId = 0;
									}
									else if(p_change.nextBuyRobotId == 0)
									{
										p_change.toBuyRobotId = 0;
										p_change.nextBuyRobot.clear();
									}
									else
									{
										p_change.toBuyRobotId = p_change.nextBuyRobotId;
										p_change.nextBuyRobotId = 0;

										if(dist_buy_type < 4)
										{
											double dis = distance_point(vecRobot[p_change.toBuyRobotId-1]->getRobotPoint(), vecRobot[p_change.toBuyRobotId-1]->getDist_buy());
											int need_time = ceil(dis / 0.12) + 50;
											
											for (auto& robot : vecRobot)
											{
												if(robot->getCarryId() == 0 && ceil(distance_point(curBuyPoint[i][j].p, robot->getRobotPoint())/0.12) > need_time)
												{
													curBuyPoint[i][j].nextBuyRobot.push_back(robot->getRobotId()+1);
												}
											}
											curBuyPoint[i][j].nextBuyRobot.push_back(vecRobot[p_change.toBuyRobotId-1]->getRobotId()+1);
										}
									}

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

						dist_buy = curBuyPoint[i][j].p;
						dist_buy_type = i + 1;
						dist_sale = curSalePoint[i][k].first;
						money_mark = cur_getmoney;
						this->cur_buy_money = cur_getmoney;

						// 标记
						if(curBuyPoint[i][j].toBuyRobotId == 0)
						{
							curBuyPoint[i][j].toBuyRobotId = robotID + 1;
							if(i+1 < 4)
							{
								double dis = distance_point(point, dist_buy);
								int need_time = ceil(dis / 0.12) + 50;
								
								for (auto& robot : vecRobot)
								{
									if(robot->getCarryId() == 0 && ceil(distance_point(curBuyPoint[i][j].p, robot->getRobotPoint())/0.12) > need_time)
									{
										curBuyPoint[i][j].nextBuyRobot.push_back(robot->getRobotId()+1);
									}
								}
								curBuyPoint[i][j].nextBuyRobot.push_back(robotID+1);
							}
						}
						else if(curBuyPoint[i][j].toBuyRobotId == robotID+1)
						{
							if(i+1 < 4)
							{
								double dis = distance_point(point, dist_buy);
								int need_time = ceil(dis / 0.12) + 50;
								
								for (auto& robot : vecRobot)
								{
									if(robot->getCarryId() == 0 && ceil(distance_point(curBuyPoint[i][j].p, robot->getRobotPoint())/0.12) > need_time)
									{
										curBuyPoint[i][j].nextBuyRobot.push_back(robot->getRobotId()+1);
									}
								}
								curBuyPoint[i][j].nextBuyRobot.push_back(robotID+1);
							}
						}
						else
						{
							curBuyPoint[i][j].nextBuyRobotId = robotID + 1;
						}
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

	//机器人旋转速度,速度计算
	double vRotate;
	double vforward;
	double maxRotateSpeed = (angle > 0 ? MAX_ANGLE_SPEED_ROBOT : -MAX_ANGLE_SPEED_ROBOT);
	if(fabs(angle)<ANGLE_NO_TURN)
	{
		vRotate = 0;
		vforward = 6;
	}
	else if(fabs(angle) > M_PI_2)
	{
		vRotate = maxRotateSpeed;
		vforward = 1.2;
	}
	else
	{
		vRotate = maxRotateSpeed * sin(fabs(angle));
		vforward = 6 * cos(fabs(angle)) + 0.32;
	}

	cout_v = vforward;
	cout_w = vRotate;
}

void Robot::CalcNpoint()
{
	nPoint.first = point.first + xlineVelocity * PREFRAME / ALLFRAME;
	nPoint.second = point.second + ylineVelocity * PREFRAME / ALLFRAME;
}

void Robot::isJudgeCrash(vector<Point> robot_point, vector<double> robot_angle, vector<pair<double, double>> robot_v, vector<double> rest_distance)
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

void Robot::isJudgeSellBuy(unordered_map<int, Point>& numbertoPoint)
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

			// 消除可买坐标
			dist_buy_type = 0;
			dist_buy.first = 50;
			dist_buy.second = 50;
		}
	}
}

void Robot::setMapArg()
{
	dist_buy = make_pair(45,5);
	S1 = 1.31608841778678362644;
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
	ANGLE_NO_TURN = 0.08;
	PREFRAME = 15;
	ALLFRAME = 50;
}
