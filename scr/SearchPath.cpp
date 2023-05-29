#include "SearchPath.h"

void SearchPath::getMap(vector<vector<char>> &useMap)
{
    curMap = useMap;
}

vector<pair<int, int>> SearchPath::getRunPath(Point_path& start_p, Point_path& end_p, bool carry)
{
    vector<pair<int, int>> store_path;
    Point_path* end_point;

    if(carry)
    {

    }
    else
    {
        Point_path* end_point = findBestPath_noCarry(start_p, end_p);
    }
    
    while(end_point != nullptr)
    {
        store_path.push_back(make_pair(end_point->x, end_point->y));
        end_point = end_point->parent;
    }

    clear(openQueue);
    closeMap.clear();

    return store_path;
}

Point_path* SearchPath::findBestPath_noCarry(Point_path& start_p, Point_path& end_p)
{
    // 插入第一个位置
    Point_path *first_point = new Point_path(start_p.x, start_p.y);
    openQueue.push(first_point);

    while(!openQueue.empty())
    {
        Point_path *curPoint = openQueue.top();

        openQueue.pop();
        closeMap[make_pair(curPoint->x, curPoint->y)] = curPoint;

        // 寻找周围格子中可以通过的格子
        for(int i = curPoint->x-1;i<=curPoint->x+1;++i)
        {
            for(int j = curPoint->y-1;j<=curPoint->y+1;++j)
            {
                //判定是否可到达
                bool canReach = false;

                if(i<0 || i>curMap[0].size()-1 || j<0 || j<curMap.size() || curMap[i][j] == '#'
                || (i == curPoint->x && j == curPoint->y) || closeMap.find(make_pair(i,j)) != closeMap.end())
                {
                    canReach = false;
                }
                else
                {
                    if(abs(curPoint->x - i) + abs(curPoint->y - j) == 1) // 不在斜角
                    {
                        canReach = true;
                    }
                    else
                    {
                        if(curMap[curPoint->x][j] != '#' && curMap[i][curPoint->y] != '#')
                        {
                            canReach = true;
                        }
                        else
                        {
                            canReach = false;
                        }
                    }
                }

                // 若能到达
                if(canReach)
                {
                    Point_path* canReach_point = new Point_path(i,j);

                    // 查找目标点是否在openQueue队列中
                    bool find_target = false;

                    for(auto &p : (vector<Point_path*>&)openQueue)
                    {
                        if(p->x == i && p->y == j)
                        {
                            find_target = true;
                            break;
                        }
                    }

                    double new_G = calcG_noCarry(canReach_point,curPoint);
                    if(find_target)  // 若在队列中，则依据G值判定是否修改目标点
                    {
                        if(new_G < canReach_point->G) // 若新G值更小，则更新
                        {
                            canReach_point->parent = curPoint;
                            canReach_point->G = new_G;
                            canReach_point->F = canReach_point->G + canReach_point->H;
                        }
                    }
                    else  // 若不在队列中，则添加到openQueue队列中
                    {
                        canReach_point->parent = curPoint;
                        canReach_point->G = new_G;
                        canReach_point->H = sqrt(pow((double)(end_p.x)-canReach_point->x,2) + pow((double)(end_p.y)-canReach_point->y,2))*move_1;
                        canReach_point->F = canReach_point->G + canReach_point->H;

                        openQueue.push(canReach_point);
                    }

                    // 若目标点已加入到openQueue中，则结束
                    if(canReach_point->x == end_p.x && canReach_point->y == end_p.y)
                    {
                        return canReach_point;
                    }
                }
            }
        }
    }
    return nullptr;
}

double SearchPath::calcG_noCarry(Point_path* tmp_point, Point_path* cur_point)
{
    double G_1 = (abs(tmp_point->x-cur_point->x) + abs(tmp_point->y-cur_point->y)) == 1?move_1:move_2;
    double G_2 = tmp_point->parent == nullptr?0:tmp_point->parent->G;

    return G_1 + G_2;
}

void SearchPath::clear(priority_queue<Point_path*,vector<Point_path*>,cmp> &p)
{
    priority_queue<Point_path*,vector<Point_path*>,cmp> empty;
    swap(empty, p);
}
