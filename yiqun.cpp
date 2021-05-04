#include <stdio.h>
#include <cmath>
#include <iostream> 
#include <fstream>
#include <time.h>
using namespace std;

const int iAntCount = 34;//蚂蚁数量 
const int iCityCount = 51;//城市数量 
const int iItCount = 2000;//最大跌代次数 
const double Q = 100;
const double alpha = 1;
const double beta = 5;
const double rou = 0.5;

int besttour[iCityCount];//最优路径列表 

double  rnd(int low, double uper)//获得随机数 
{
    double p = (rand() / (double)RAND_MAX) * ((uper)-(low)) + (low);
    return (p);
};
int rnd(int uper)
{
    return (rand() % uper);
};
class GInfo//tsp地图信息，包含了信息素，城市距离，和信息素变化矩阵 
{
public:
    double m_dDeltTrial[iCityCount][iCityCount];
    double m_dTrial[iCityCount][iCityCount];
    double distance[iCityCount][iCityCount];

};
GInfo Map;
class ant
{
private:
    int ChooseNextCity();//选择城市 
    double prob[iCityCount];
    int m_iCityCount;
    int AllowedCity[iCityCount];//没有走过的城市 
public:
    void addcity(int city);
    int tabu[iCityCount];
    void Clear();
    void UpdateResult();
    double m_dLength;
    double m_dShortest;
    void move();
    ant();
    void move2last();

};
void ant::move2last()
{
    int i;
    for (i = 0; i < iCityCount; i++)

        if (AllowedCity[i] == 1)
        {
            addcity(i);
            break;
        }
}

void ant::Clear()
{
    m_dLength = 0;
    int i;
    for (i = 0; i < iCityCount; i++)
    {
        prob[i] = 0;
        AllowedCity[i] = 1;
        i = tabu[iCityCount - 1];
        m_iCityCount = 0;
        addcity(i);
    }
}
ant::ant()
{
    m_dLength = m_dShortest = 0;
    m_iCityCount = 0;
    int i;
    for (i = 0; i < iCityCount; i++) {

        AllowedCity[i] = 1;
        prob[i] = 0;
    }
}
void ant::addcity(int city)
{
    //add city to tabu; 
    tabu[m_iCityCount] = city;
    m_iCityCount++;
    AllowedCity[city] = 0;
}
int ant::ChooseNextCity()
{
    //Update the probability of path selection 
    //select a path from tabu[m_iCityCount-1] to next
    int i;
    int j = 10000;
    double temp = 0;
    int curCity = tabu[m_iCityCount - 1];
    for (i = 0; i < iCityCount; i++) {

        if ((AllowedCity[i] == 1))
        {
            temp += pow((1.0 / Map.distance[curCity][i]), beta) * pow((Map.m_dTrial[curCity][i]), alpha);
        }
    }
    double sel = 0;
    for (i = 0; i < iCityCount; i++) {

        if ((AllowedCity[i] == 1))
        {
            prob[i] = pow((1.0 / Map.distance[curCity][i]), (int)beta) * pow((Map.m_dTrial[curCity][i]), (int)alpha) / temp;
            sel += prob[i];
        }
        else
            prob[i] = 0;
    }
    double mRate = rnd(0, sel);
    double mSelect = 0;
    for (i = 0; i < iCityCount; i++) {

        if ((AllowedCity[i] == 1))
            mSelect += prob[i];
        if (mSelect >= mRate) { j = i; break; }
    }
    if (j == 10000)
    {
        temp = -1;
        for (i = 0; i < iCityCount; i++) {

            if ((AllowedCity[i] == 1))
                if (temp) {
                    temp = pow((1.0 / Map.distance[curCity][i]), beta) * pow((double)(Map.m_dTrial[curCity][i]), alpha);
                    j = i;
                }
        }
    }
    return j;
}
void ant::UpdateResult()
{
    // Update the length of tour 
    int i;
    for (i = 0; i < iCityCount - 1; i++)

        m_dLength += Map.distance[tabu[i]][tabu[i + 1]];
    m_dLength += Map.distance[tabu[iCityCount - 1]][tabu[0]];
}
void ant::move()
{
    //the ant move to next town and add town ID to tabu. 
    int j;
    j = ChooseNextCity();
    addcity(j);
}
class project
{
public:
    void UpdateTrial();
    double m_dLength;
    void initmap();
    ant ants[iAntCount];
    void GetAnt();
    void StartSearch();
    project();
};
void project::UpdateTrial()
{
    //calculate the changes of trial information 
    int i;
    int j;
    for (i = 0; i < iAntCount; i++) {
        for (j = 0; j < iCityCount - 1; j++)
        {
            Map.m_dDeltTrial[ants[i].tabu[j]][ants[i].tabu[j + 1]] += Q / ants[i].m_dLength;
            Map.m_dDeltTrial[ants[i].tabu[j + 1]][ants[i].tabu[j]] += Q / ants[i].m_dLength;
        }
        Map.m_dDeltTrial[ants[i].tabu[iCityCount - 1]][ants[i].tabu[0]] += Q / ants[i].m_dLength;
        Map.m_dDeltTrial[ants[i].tabu[0]][ants[i].tabu[iCityCount - 1]] += Q / ants[i].m_dLength;
    }
    for (i = 0; i < iCityCount; i++) {
        for (j = 0; j < iCityCount; j++)
        {
            Map.m_dTrial[i][j] = (rou * Map.m_dTrial[i][j] + Map.m_dDeltTrial[i][j]);
            Map.m_dDeltTrial[i][j] = 0;
        }
    }
}
void project::initmap()
{
    int i;
    int j;
    for (i = 0; i < iCityCount; i++)
        for (j = 0; j < iCityCount; j++)
        {


            Map.m_dTrial[i][j] = 1;
            Map.m_dDeltTrial[i][j] = 0;
        }
}
project::project()
{
    //initial map,read map infomation from file . et. 
    initmap();
    m_dLength = 10e9;


    ifstream in("eil51.tsp");

    struct city
    {
        int num;
        int x;
        int  y;
    }cc[iCityCount];

    for (int i = 0; i < iCityCount; i++)
    {
        in >> cc[i].num >> cc[i].x >> cc[i].y;
        besttour[i] = 0;
    }
    int j;
    for (int i = 0; i < iCityCount; i++)
        for (j = 0; j < iCityCount; j++)
        {
            {
                Map.distance[i][j] = sqrt(pow((double)(cc[i].x - cc[j].x), 2) + pow((double)(cc[i].y - cc[j].y), 2));
            }
        }
}
void project::GetAnt()
{
    //randomly put ant into map 
    int i = 0;
    int city;
    srand((unsigned)time(NULL) + rand());
    for (i = 0; i < iAntCount; i++)
    {
        city = rnd(iCityCount);
        ants[i].addcity(city);
    }
}
void project::StartSearch()
{
    //begin to find best solution 
    int max = 0;//every ant tours times 
    int i;
    int j;
    double temp;
    int temptour[iCityCount];
    while (max < iItCount)
    {
        for (j = 0; j < iAntCount; j++)
        {
            for (i = 0; i < iCityCount - 1; i++)
                ants[j].move();
        }
        for (j = 0; j < iAntCount; j++)
        {
            ants[j].move2last();
            ants[j].UpdateResult();
        }
        //find out the best solution of the step and put it into temp 
        int t;
        temp = ants[0].m_dLength;
        for (t = 0; t < iCityCount; t++)
            temptour[t] = ants[0].tabu[t];
        for (j = 0; j < iAntCount; j++)
        {
            if (temp > ants[j].m_dLength) {
                temp = ants[j].m_dLength;
                for (t = 0; t < iCityCount; t++)
                    temptour[t] = ants[j].tabu[t];
            }
        }
        if (temp < m_dLength) {
            m_dLength = temp;
            for (t = 0; t < iCityCount; t++)
                besttour[t] = temptour[t];
        }
        printf("%d : %f\n", max, m_dLength);
        UpdateTrial();
        for (j = 0; j < iAntCount; j++)
            ants[j].Clear();
        max++;
    }
    printf("The shortest toure is : %f\n", m_dLength);
    for (int t = 0; t < iCityCount; t++)
        printf(" %d ", besttour[t]);
}
int main()
{
    project TSP;
    TSP.GetAnt();
    TSP.StartSearch();
    return 0;
}