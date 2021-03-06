// 08-图9 关键活动
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MaxVertexNum 101
#define MaxSize 101
#define ERROR -1
#define INFINITY 65535

typedef int Vertex; // 用顶点下标表示顶点,为整形
typedef int WeightType; // 边的权值设为int


/* ********************* LGraph *******************/
// 边的定义
typedef struct ENode *PtrENode;
struct ENode
{
    Vertex V1, V2;
    WeightType Weight;
};
typedef PtrENode Edge;

// 邻接点的定义 边表
typedef struct AdjVNode *PtrAdjVNode;
struct AdjVNode
{
    Vertex AdjV; // 邻接点下标
    WeightType Weight; // 边权重
    PtrAdjVNode Next;  // 指向下一个邻接点的致指针
};

// 顶点表头结点的定义 顶点表
struct VNode{
    PtrAdjVNode FirstEdge;  // 边表头指针
    // DataType Data;   // 存顶点数据,一般不用
};
typedef struct VNode AdjList[MaxVertexNum];

// Graph struct
/* 图结点的定义 */
typedef struct GNode * PtrGNode;
struct GNode
{
    int Nv; //顶点数
    int Ne; // 边数
    AdjList G; // 邻接表. 是一个size为MaxVertexNum的数组,每一个元素都是一个结点VNode
    AdjList VG; // 反方向邻接表
};
typedef PtrGNode LGraph;

// 初始化有VertexNum个顶点但没有边的图
LGraph CreateGraph(int VertexNum)
{
    LGraph Graph;
    Vertex V;

    Graph = (LGraph)malloc(sizeof(struct GNode));
    Graph->Nv = VertexNum;
    Graph->Ne = 0;

    for(V=0; V<Graph->Nv; V++)
        Graph->G[V].FirstEdge = NULL; // 每一个顶点v的FirstEdge都是空的, 编号从0到 Graph->Nv-1
        Graph->VG[V].FirstEdge = NULL; 
    return Graph;
}

// 插入一条边
void InsertEdge (LGraph Graph, Edge E)  /* 插入边<V1, V2> */
{
    PtrAdjVNode NewNode;

    /* 为V2建立新的邻接点 */
    NewNode = (PtrAdjVNode)malloc(sizeof(struct AdjVNode)); 
    NewNode->AdjV = E->V2;
    NewNode->Weight = E->Weight;
    /* 将V2插入V1的表头, 在头上插入 */
    NewNode->Next = Graph->G[E->V1].FirstEdge; 
    Graph->G[E->V1].FirstEdge = NewNode;

    /* 为V1建立新的邻接点 */
    NewNode = (PtrAdjVNode)malloc(sizeof(struct AdjVNode)); 
    NewNode->AdjV = E->V1;
    NewNode->Weight = E->Weight;
    /* 将V1插入V的表头 */
    NewNode->Next = Graph->VG[E->V2].FirstEdge; 
    Graph->VG[E->V2].FirstEdge = NewNode;
}


// 建图
LGraph BuildGraph(){
    LGraph Graph;
    Edge E;
    Vertex V, V1, V2;
    int Nv, i;

    scanf("%d", &Nv);
    Graph = CreateGraph(Nv);

    scanf("%d", &(Graph->Ne));
    if (Graph->Ne != 0)
    {
        E = (Edge)malloc(sizeof(struct ENode)); // 创建边结点
        for (i=0; i<Graph->Ne; i++)
        {
           /* 读入边,格式为 起点 终点 权重 */
            scanf("%d %d %d", &V1, &V2, &E->Weight);
            E->V1 = V1 - 1;  // 把从1开始标记,改为从0开始
            E->V2 = V2 - 1;
            InsertEdge(Graph, E);
        }
    }
    return Graph;
}



/**************** Queue ******************/
#define ElementType int

struct QNode {
    ElementType *Data; 
    int rear;
    int front;
};
typedef struct QNode *Queue;

// 创建队
Queue CreateQueue(int N)
{
    Queue Q = (Queue)malloc(sizeof(struct QNode));
    Q->Data = (ElementType*)malloc(N * sizeof(ElementType));
    Q->front = Q->rear = 0;
    return Q;
}

// 空
bool IsEmpty(Queue Q)
{
    return(Q->front ==  Q->rear);
}

// 满
bool IsFull(Queue Q)
{
    return ((Q->rear+1) % MaxSize == Q->front );
}

// 入队
void AddQ( Queue PtrQ, ElementType item) {
    // if (IsFull(Q))
    if ( (PtrQ->rear+1) % MaxSize == PtrQ->front ) 
    { 
        printf("队列满");
        return;
    }
    PtrQ->rear = (PtrQ->rear + 1)% MaxSize; 
    PtrQ->Data[PtrQ->rear] = item;
}

// 出队
ElementType DeleteQ ( Queue PtrQ ) // front和rear相同
{
    // if (IsEmpty(Q))
    if ( PtrQ->front == PtrQ->rear ) 
    { 
        printf("队列空");
        return ERROR;
    } 
    else 
    {
        PtrQ->front = (PtrQ->front+1)% MaxSize; // front往后移一位指向第一个元素
        return PtrQ->Data[PtrQ->front];
    } 
}

/* *************** TopSort ***************** */

int MAX(int a, int b){
    return a>b? a:b;
}

int MIN(int a, int b){
    return a<b? a:b;
}

int TopSort(LGraph Graph, int indegree[], int outdegree[], int Earliest[]){
    int cnt=0;
    Vertex V;
    PtrAdjVNode W;
    int MaxTime = 0;
    Queue Q = CreateQueue(Graph->Nv);

    // 计算入度 出度
    for (V = 0; V < Graph->Nv; V++){
        for(W=Graph->G[V].FirstEdge; W; W=W->Next){
            indegree[W->AdjV]++;
            outdegree[V]++;
        }
    }

    // 将所有入度为0的顶点入队
    for (V=0; V<Graph->Nv; V++){
        if(indegree[V] == 0)
            AddQ(Q, V);
    }

    // 计算Earlest[]
    while (!IsEmpty(Q)){
        V =DeleteQ(Q);
        cnt++;
        for(W=Graph->G[V].FirstEdge; W; W=W->Next){
            indegree[W->AdjV]--;  // V的每一个邻接点的入度-1
            Earliest[W->AdjV] = MAX(Earliest[W->AdjV], Earliest[V] + W->Weight);  // 如果有更长的时间,则更新时间
            if(indegree[W->AdjV] == 0)  // 若删除V使得W->AdjV入度为0
                AddQ(Q, W->AdjV);
        }
    } 

    if (cnt != Graph->Nv)
        return 0;  // 没把所有顶点都收进来,说明有回路
    else{
        for (V=0; V<Graph->Nv; V++){
            if (MaxTime < Earliest[V])
                MaxTime = Earliest[V];
        }
    }
    return MaxTime;
}


void GetLatest(LGraph Graph, int outdegree[], int Latest[], int MaxTime){
    Vertex V;
    PtrAdjVNode W;
    Queue Q = CreateQueue(Graph->Nv);

// 初始化Latest[]为最大时间值
    for (V=0; V<Graph->Nv; V++)
        Latest[V] = MaxTime; 


    // 所有出度为0的顶点入队
    for (V=0; V<Graph->Nv; V++)
        if (outdegree[V] == 0)
            AddQ(Q, V);

    // 计算Latest[]
    while (!IsEmpty(Q)){
         V = DeleteQ(Q);
        for (W = Graph->VG[V].FirstEdge; W; W=W->Next){
            outdegree[W->AdjV]--;
            Latest[W->AdjV] = MIN(Latest[W->AdjV], Latest[V] - W->Weight);
            if (outdegree[W->AdjV] == 0)  // 若删除V使得W->AdjV出度为0
                 AddQ(Q, W->AdjV);
        }
    }
}


void Important(LGraph Graph, int Latest[], int Earliest[]){

    Vertex V;
    PtrAdjVNode W;

    for (V=0; V<Graph->Nv; V++){
        for (W = Graph->G[V].FirstEdge; W; W=W->Next){
            if (Latest[W->AdjV] - Earliest[V] - W->Weight == 0)  // 是关键活动
                printf("%d->%d\n", V+1, W->AdjV+1);  // 因为从0开始标的,补回+1
        }
    }
}


/* ******************************** */


int main(int argc, char const *argv[])
{
    LGraph Graph;
    int Earliest[MaxVertexNum]={0}, Latest[MaxVertexNum], 
        indegree[MaxVertexNum], outdegree[MaxVertexNum], MaxTime;
    Vertex V;

    Graph = BuildGraph();

    // printf("%d %d\n", Graph->VG[4].FirstEdge->AdjV, Graph->VG[4].FirstEdge->Weight);

    MaxTime = TopSort(Graph, indegree, outdegree, Earliest);
    if (MaxTime){
        printf("%d\n", MaxTime);
        GetLatest(Graph,outdegree,Latest, MaxTime);
        Important(Graph, Latest, Earliest);
    }
    else
        printf("0\n");

    return 0;
}



