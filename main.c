#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "queue.h"

#define INPUT_LENGTH 4
#define WRONG_INPUT_LENGTH_MSG "Usage: TreeAnalayzer <Graph File Path> <First Vertex> <Second Vertex>\\n"
#define EXIT_FAILURE 1
#define END_OF_PROGRAM 0
#define INVALID_INPUT "Invalid input\n"
#define VERTEX_1_PLACE 2
#define VERTEX_2_PLACE 3
#define END_OF_LINE '\n'
#define IS_LEAF_1 "-\n"
#define IS_LEAF_2 "-"
#define LINE_SIZE 1024
#define DEFAULT_FATHER -1
#define IS_ROOT 1
#define PREV -1
#define NONE 0
#define ROOT_PRINT "Root Vertex:"
#define VERTICES_COUNT "Vertices Count:"
#define EDGES_COUNT "Edges Count:"
#define MINIMAL_BRANCH "Length of Minimal Branch:"
#define MAXIMAL_BRANCH "Length of Maximal Branch:"
#define DIAMETER "Diameter Length:"
#define SHORT "Shortest Path Between"
#define AND "and"

struct Vertex
{
    int isLeaf;
    int isRoot;
    int *children;
    int amountOfChildren;
    int father;
    unsigned int prev;
    int bfsDist;
    int distanceFromRoot;
} Vertex;


int verticesAmountValidity(const char *ver);
int children_parse(const char *ver, struct Vertex *vertices, int index, int Vnum);
int rootValidity(int Vnum, struct Vertex *vertices);
void freeMem(struct Vertex * vertices, int Vnum, FILE * ptr);
void setAsLeaf(struct Vertex * v);
void updatePathFromRoot(struct Vertex * vertices, int vertex);
int rootManipulation(struct Vertex * vertices, int Vnum);
int findMinPath(struct Vertex * vertices, int Vnum);
int findMaxPath(struct Vertex * vertices, int Vnum);
int findMaxNode(struct Vertex * vertices, int Vnum);
int BFS(struct Vertex *vertices, int s, int Vnum);
void printResults(int RootIndex, int Vnum, int minPath, int maxPath, int Diameter, int V1, int V2, struct Vertex * vertices);


int main(int argc, char* argv[])
{
    int Vnum, V1, V2;
    if(argc !=INPUT_LENGTH)
    {
        fprintf(stderr, WRONG_INPUT_LENGTH_MSG);
        return EXIT_FAILURE;
    }

    // OPEN THE FILE AND CHECK IF IT'S NOT EMPTY
    FILE *ptr;
    ptr = fopen(argv[1],"r");
    if (ptr == NULL)
    {
        printf("1\n");
        fprintf(stderr, INVALID_INPUT);
        return EXIT_FAILURE;
    }

    char parse[LINE_SIZE];
    //FIRST_LINE - NUM OF VERTEX (check if is digit)
    fgets(parse, LINE_SIZE, ptr);
   // Parsing first line;
    if(verticesAmountValidity(parse)) {
        printf("2\n");
        fprintf(stderr, INVALID_INPUT);
        return EXIT_FAILURE;
    }
    char *eptr;
    Vnum = (int) strtol(parse,&eptr, 10);

    //vertex argument validity check
    if(verticesAmountValidity(argv[VERTEX_1_PLACE]) || verticesAmountValidity(argv[VERTEX_2_PLACE]))
    {
        printf("3\n");
        fprintf(stderr, INVALID_INPUT);
        return EXIT_FAILURE;
    }
    V1 = (int) strtol(argv[VERTEX_1_PLACE],&eptr, 10);
    V2 = (int) strtol(argv[VERTEX_2_PLACE],&eptr, 10);
    if(V1<0 || V1>=Vnum || V2<0 || V2>=Vnum)     // If argument given nodes aren't in Vertices scope, leave program
    {
        printf("4\n");
        fprintf(stderr, INVALID_INPUT);
        return EXIT_FAILURE;
    }
    //initiating a vertices array
    struct Vertex * vertices = NULL;
    vertices = (struct Vertex *) malloc(Vnum*sizeof(struct Vertex));
    if(vertices == NULL)
    {
        fprintf(stderr, "Memory allocation problem");
        return EXIT_FAILURE;
    }
    for (int l = 0; l < Vnum; ++l) {
        vertices[l].father = DEFAULT_FATHER;
        vertices[l].prev = PREV;
        vertices[l].distanceFromRoot = NONE;
    }
    int line =0;
    for (int i = 0; i < Vnum; ++i) {
        line++;
        fgets(parse, LINE_SIZE, ptr);
        int validation = children_parse(parse, vertices, i, Vnum); //parsing children
        if(validation) // checking if children parse succeeded
        {
            freeMem(vertices, Vnum, ptr);
            printf("5\n");
            fprintf(stderr, INVALID_INPUT);
            return EXIT_FAILURE;
        }
        for (int j = 0; j < vertices[i].amountOfChildren; ++j) {
            // checks if children are in Vnum scope + checks if a child equals to it's father
            if(vertices[i].children[j] < 0 || vertices[i].children[j]>=Vnum || vertices[i].children[j] == i)
            {
                freeMem(vertices, Vnum, ptr);
                printf("6\n");
                fprintf(stderr, INVALID_INPUT);
                return EXIT_FAILURE;
            }
            else
            {
                vertices[vertices[i].children[j]].father = i;
            }
        }
    }
    // checks for extra lines
    if (fgets(parse, LINE_SIZE, ptr) !=NULL)
    {
        freeMem(vertices, Vnum, ptr);
        printf("7\n");
        fprintf(stderr, INVALID_INPUT);
        return EXIT_FAILURE;
    }
    // checks if more than one root or no root at all
    int isRootCheck = rootValidity(Vnum, vertices);
    if (isRootCheck)
    {
        freeMem(vertices, Vnum, ptr);
        printf("8\n");
        fprintf(stderr, INVALID_INPUT);
        return EXIT_FAILURE;
    }
    int RootIndex=rootManipulation(vertices, Vnum);
    updatePathFromRoot(vertices, RootIndex);
    int minPath = findMinPath(vertices, Vnum);
    int maxPath = findMaxPath(vertices, Vnum);
    int maxNode = findMaxNode(vertices, Vnum);
    int diameterTest = BFS(vertices, maxNode, Vnum);
    if (diameterTest)
    {
        fprintf(stderr, "Problem with BFS algorithm");
    }
    int Diameter = 0;
    for (int k = 0; k < Vnum; ++k)
    {
        if(Diameter < vertices[k].bfsDist)
        {
            Diameter = vertices[k].bfsDist;
        }
    }
    int pathTest = BFS(vertices, V2, Vnum);
    if (pathTest)
    {
        fprintf(stderr, "Problem with BFS algorithm");
    }

//Prints results
    printResults(RootIndex, Vnum, minPath, maxPath, Diameter, V1, V2, vertices);
    freeMem(vertices, Vnum, ptr);
    return END_OF_PROGRAM;
}

/**
 *
 * @param RootIndex int representing the root key
 * @param Vnum amount of vertices in Tree
 * @param minPath the minimum path from root
 * @param maxPath the maximum path from root
 * @param Diameter the Diameter of the Tree
 * @param V1 First vertex given in the arguments
 * @param V2 Second vertex given in the arguments
 * @param vertices array that holds all vertices of the tree
 */
void printResults(int RootIndex, int Vnum, int minPath, int maxPath, int Diameter, int V1, int V2, struct Vertex * vertices)
{
    printf("%s %d\n",ROOT_PRINT, RootIndex);
    printf("%s% d\n", VERTICES_COUNT, Vnum);
    printf("%s %d\n",EDGES_COUNT ,Vnum-1);
    printf("%s %d\n", MINIMAL_BRANCH, minPath);
    printf("%s %d\n",MAXIMAL_BRANCH, maxPath);
    printf("%s %d\n",DIAMETER, Diameter);
    printf("%s %d %s %d: %d ", SHORT, V1, AND, V2, V1);
    int u = V1;
    for (int m = 0; m < vertices[V1].bfsDist; ++m) {
        printf("%d ",vertices[u].prev);
        u = vertices[u].prev;
    }
    printf("%c",END_OF_LINE);
}


/**
 * Freeing all malloc arrays
 * @param vertices main array containing vertices that hold another malloc array
 * @param Vnum amount of vertices in the main array
 */
void freeMem(struct Vertex * vertices, int Vnum, FILE * ptr)
{
    for (int i = 0; i < Vnum; i++)
    {
        free(vertices[i].children);
        vertices[i].children = NULL;
    }
    free(vertices);
    fclose(ptr);
}

/**
 * Parsing the first line of the file
 * @param ver String
 * @return 0 if valid 1 if parse is not
 */
int verticesAmountValidity(const char *ver)
{
    for (int i = 0; i < (int) strlen(ver); ++i)
    {
        if (isdigit(ver[i])==0 && ver[i]!= END_OF_LINE)
        {

            return 1;
        }
    }
    char *eptr;
    int vertex = (int) strtol(ver,&eptr, 10);
    if (vertex <0)
    {
        return 1;
    }
    return 0;
}

/**
 *
 * @param ver
 * @param vertices
 * @param index
 * @param Vnum
 * @return
 */
int children_parse(const char *ver, struct Vertex *vertices, int index, int Vnum) //must handle end of line
{
    int* children = NULL;
    children = (int*) malloc(Vnum * sizeof(int));
    if (children ==NULL)
    {
        fprintf(stderr, "Memory allocation problem");
        return EXIT_FAILURE;
    }
    if (strcmp(ver,IS_LEAF_1)==0 || strcmp(ver,IS_LEAF_2)==0)
    {
        setAsLeaf(&vertices[index]); // set one node to be a leaf
        free(children);
        return 0;
    }
    char *eptr = ver;
    int ind = 0;
    for (int i = 0; i < (int) strlen(ver); ++i)
    {
        while(ver[i]!=' ' && ver[i]!=END_OF_LINE)
        {
            if(ver[i] == '\0')
            {
                break;
            }
            if(isdigit(ver[i])==0)
            {
                return 1;
            }
            i++;
        }
        int child = (int) strtol(eptr, &eptr, 10);
        children[ind] = child;
        ind++;
    }
    vertices[index].isLeaf = 0;
    vertices[index].children = children;
    vertices[index].amountOfChildren = ind;
    return 0;
}

/**
 * iterates over vertices array and checks every node if it has a father, if there is more than 1 vertex w/o a father
 * or all vertices has a father, returns 1 (invalid tree) else returns 0 and sets the correct vertex to be the root
 * @param Vnum
 * @param vertices
 * @return
 */
int rootValidity(int Vnum, struct Vertex *vertices)
{
    int count =0;
    for (int i = 0; i < Vnum; ++i) {
        if(vertices[i].father == DEFAULT_FATHER)
        {
            count++;
            vertices[i].isRoot = IS_ROOT;
        }
    }
    if(count != 1)
    {
        return 1;
    }
    return 0;
}

/**
 * set a node to be a leaf
 * @param v vertex pointer
 */
void setAsLeaf(struct Vertex * v)
{
    v->isLeaf = 1;
    v->children = NULL;
    v->amountOfChildren = 0;
}



/**
 *
 * @param vertices
 * @param Vnum
 * @return
 */
int rootManipulation(struct Vertex * vertices, int Vnum)
{
    for (int i = 0; i < Vnum; ++i)
    {
        if(vertices[i].isRoot)
        {
            vertices[i].distanceFromRoot=0;
            return i;
        }
    }
}


/**
 *
 * @param vertices
 * @param vertex
 */
void updatePathFromRoot(struct Vertex * vertices, int vertex)
{
    for (int i = 0; i <vertices[vertex].amountOfChildren ; ++i)
    {
        if(vertices[vertices[vertex].children[i]].distanceFromRoot == NONE)
        {
            vertices[vertices[vertex].children[i]].distanceFromRoot = vertices[vertex].distanceFromRoot+1;
        }
        updatePathFromRoot(vertices, vertices[vertex].children[i]);
    }
}

/**
 *
 * @param vertices
 * @param Vnum
 * @return
 */
int findMinPath(struct Vertex * vertices, int Vnum)
{
    int minPath = Vnum-1;
    for (int i = 0; i < Vnum; ++i)
    {
        if(vertices[i].isLeaf)
        {
            if(vertices[i].distanceFromRoot < minPath)
            {
                minPath = vertices[i].distanceFromRoot;
            }
        }
    }
    return minPath;
}

/**
 *
 * @param vertices
 * @param Vnum
 * @return
 */
int findMaxPath(struct Vertex * vertices, int Vnum)
{
    int maxPath = 0;
    for (int i = 0; i < Vnum; ++i)
    {
        if(vertices[i].isLeaf)
        {
            if(vertices[i].distanceFromRoot > maxPath)
            {
                maxPath = vertices[i].distanceFromRoot;
            }
        }
    }
    return maxPath;
}

int findMaxNode(struct Vertex * vertices, int Vnum)
{
    int maxPath = 0;
    int maxNode = 0;
    for (int i = 0; i < Vnum; ++i)
    {
        if(vertices[i].isLeaf)
        {
            if(vertices[i].distanceFromRoot > maxPath)
            {
                maxPath = vertices[i].distanceFromRoot;
                maxNode = i;
            }
        }
    }
    return maxNode;
}

/**
 *
 * @param vertices Data structure that holds all Graph's nodes
 * @param s Start node
 * @param Vnum number of vertices in G
 * @return
 */
int BFS(struct Vertex *vertices, int s, int Vnum)
{
    for (int i = 0; i < Vnum; ++i)
    {
        vertices[i].prev = PREV;
        vertices[i].bfsDist = -1;
    }
    vertices[s].bfsDist = 0;
    Queue * q = allocQueue();
    if(q==NULL)
    {
        fprintf(stderr, "Memory allocation problem");
        return EXIT_FAILURE;
    }
    enqueue(q, s);
    while(!queueIsEmpty(q))
    {
        unsigned int u = dequeue(q);
        int HasFather=0;
        if(vertices[u].father!=DEFAULT_FATHER)
        {
            HasFather =1;
        }
        for (int i = 0; i < vertices[u].amountOfChildren; ++i) {
            unsigned int w = vertices[u].children[i];
            if(vertices[w].bfsDist == -1)
            {
                enqueue(q, w);
                vertices[w].prev = u;
                vertices[w].bfsDist = vertices[u].bfsDist + 1;
            }
        }
        if(HasFather)
        {
            unsigned int f = vertices[u].father;
            if(vertices[f].bfsDist == -1)
            {
                enqueue(q, f);
                vertices[f].prev = u;
                vertices[f].bfsDist = vertices[u].bfsDist + 1;
            }
        }
    }
    free(q);
    return 0;
}
