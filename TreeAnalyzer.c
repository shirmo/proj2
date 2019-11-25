//
// Created by shirmo on 25/11/19
// Description: the program parses a text file, decides if it represents a valid tree and outputs the tree's structure
//

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "queue.h"

#define INPUT_LENGTH 4
#define WRONG_INPUT_LENGTH_MSG "Usage: TreeAnalyzer <Graph File Path> <First Vertex> <Second Vertex>\n"
#define EXIT_FAILURE 1
#define	EXIT_SUCCESS	0
#define END_OF_PROGRAM 0
#define INVALID_INPUT "Invalid input\n"
#define VERTEX_1_PLACE 2
#define VERTEX_2_PLACE 3
#define END_OF_LINE '\n'
#define END_OF_LINE_2 "\r\n"
#define EMPTY_LINE "\n"
#define IS_LEAF_1 "-\n"
#define IS_LEAF_2 "-"
#define SPACE " "
#define LINE_SIZE 1024
#define DEFAULT_FATHER -1
#define PREV -1
#define ROOT_PROBLEM -1
#define ROOT_PRINT "Root Vertex:"
#define VERTICES_COUNT "Vertices Count:"
#define EDGES_COUNT "Edges Count:"
#define MINIMAL_BRANCH "Length of Minimal Branch:"
#define MAXIMAL_BRANCH "Length of Maximal Branch:"
#define DIAMETER "Diameter Length:"
#define SHORT "Shortest Path Between"
#define AND "and"

/**
 * Struct representing each node in a given graph
 */
struct Vertex
{
    int isLeaf;
    int *children;
    int amountOfChildren;
    int father;
    unsigned int prev;
    int bfsDist;
} Vertex;


void verticesAmountValidity(const char *ver);
int childrenParse(char *ver, struct Vertex *vertices, int line, int vNum);
int rootValidity(int vNum, struct Vertex *vertices, FILE * file);
void freeMem(struct Vertex * vertices, int vNum, FILE * ptr);
void setAsLeaf(struct Vertex * v);
int findMinBranch(struct Vertex * vertices, int vNum);
int findMaxPath(struct Vertex * vertices, int vNum);
int findMaxNode(struct Vertex * vertices, int vNum);
void BFS(struct Vertex *vertices, int s, int vNum, FILE * file);
void printResults(int RootIndex, int vNum, int minPath, int maxPath, int Diameter, int V1, int V2, struct Vertex * vertices);
int validatedVertex(char * vertex);
void argumentValidation(int argumentAmount);
void inScope(int vNum, int ver);
void initiatingValues(struct Vertex * vertices, int vNum);
void treeCreation(int vNum, FILE * file, struct Vertex * vertices);

/**
 * Operating the program
 * @param argc length of arguments
 * @param argv arguments given from the user
 * @return 1 if failed, 0 if program ran smooth
 */
int main(int argc, char* argv[])
{
    int vNum, V1, V2;
    argumentValidation(argc); //argument check
    // OPEN THE FILE AND CHECK IF IT'S NOT EMPTY
    FILE *ptr;
    ptr = fopen(argv[1], "r");
    if (ptr == NULL)
    {
        fprintf(stderr, INVALID_INPUT);
        return EXIT_FAILURE;
    }
    char parse[LINE_SIZE];
    fgets(parse, LINE_SIZE, ptr); //FIRST_LINE - NUM OF VERTEX (check if is digit)
    verticesAmountValidity(parse); //Parsing first line;
    char *eptr;
    vNum = (int) strtol(parse, &eptr, 10);
    //vertex argument validity check
    verticesAmountValidity(argv[VERTEX_1_PLACE]);
    verticesAmountValidity(argv[VERTEX_2_PLACE]);
    V1 = (int) strtol(argv[VERTEX_1_PLACE], &eptr, 10);
    V2 = (int) strtol(argv[VERTEX_2_PLACE], &eptr, 10);
    // If argument given nodes aren't in Vertices scope, leave program
    inScope(vNum, V1);
    inScope(vNum, V2);
    struct Vertex * vertices = NULL;     //initiating a vertices array
    vertices = (struct Vertex *) malloc(vNum * sizeof(struct Vertex));
    if(vertices == NULL)
    {
        fprintf(stderr, "Memory allocation problem");
        return EXIT_FAILURE;
    }
    initiatingValues(vertices, vNum);
    treeCreation(vNum, ptr, vertices);
    if (fgets(parse, LINE_SIZE, ptr) !=NULL) // checks for extra lines
    {
        freeMem(vertices, vNum, ptr);
        fprintf(stderr, INVALID_INPUT);
        return EXIT_FAILURE;
    }
    int RootIndex = rootValidity(vNum, vertices, ptr); // checks if more than one root or no root at all
    BFS(vertices, RootIndex, vNum, ptr);
    int minPath = findMinBranch(vertices, vNum);
    int maxPath = findMaxPath(vertices, vNum);
    int maxNode = findMaxNode(vertices, vNum);
    BFS(vertices, maxNode, vNum, ptr);
    int Diameter = findMaxPath(vertices, vNum);
    BFS(vertices, V2, vNum, ptr);
    printResults(RootIndex, vNum, minPath, maxPath, Diameter, V1, V2, vertices); //Prints results
    freeMem(vertices, vNum, ptr);
    return END_OF_PROGRAM;
}


/**
 * validate arguments amount
 * @param argumentAmount argc from the command line
 */
void argumentValidation(int argumentAmount)
{
    if(argumentAmount != INPUT_LENGTH)
    {
        fprintf(stderr, WRONG_INPUT_LENGTH_MSG);
        exit(EXIT_FAILURE);
    }
}


/**
 * Prints all required outputs
 * @param RootIndex int representing the root key
 * @param vNum amount of vertices in Tree
 * @param minPath the minimum path from root
 * @param maxPath the maximum path from root
 * @param Diameter the Diameter of the Tree
 * @param V1 First vertex given in the arguments
 * @param V2 Second vertex given in the arguments
 * @param vertices array that holds all vertices of the tree
 */
void printResults(int RootIndex, int vNum, int minPath, int maxPath, int Diameter, int V1, int V2, struct Vertex * vertices)
{
    printf("%s %d\n", ROOT_PRINT, RootIndex);
    printf("%s% d\n", VERTICES_COUNT, vNum);
    printf("%s %d\n", EDGES_COUNT , vNum-1);
    printf("%s %d\n", MINIMAL_BRANCH, minPath);
    printf("%s %d\n", MAXIMAL_BRANCH, maxPath);
    printf("%s %d\n", DIAMETER, Diameter);
    printf("%s %d %s %d: %d", SHORT, V1, AND, V2, V1);
    int u = V1;
    for (int m = 0; m < vertices[V1].bfsDist; ++m)
    {
        printf(" %d", vertices[u].prev);
        u = (int) vertices[u].prev;
    }
    printf("%c", END_OF_LINE);
}


/**
 * Freeing all malloc arrays
 * @param vertices main array containing vertices that hold another malloc array
 * @param vNum amount of vertices in the main array
 */
void freeMem(struct Vertex * vertices, int vNum, FILE * ptr)
{
    for (int i = 0; i < vNum; i++)
    {
        if (vertices[i].children != NULL)
        {
            free(vertices[i].children);
            vertices[i].children = NULL;
        }

    }
    free(vertices);
    fclose(ptr);
}

/**
 * Parsing the first line of the file
 * @param ver String
 * @return 0 if valid 1 if parse is not
 */
void verticesAmountValidity(const char *ver)
{
    for (int i = 0; i < (int) strlen(ver); ++i)
    {
        if (isdigit(ver[i]) == 0 && ver[i] != END_OF_LINE)
        {
            fprintf(stderr, INVALID_INPUT);
            exit(EXIT_FAILURE);
        }
    }
    char *eptr;
    int vertex = (int) strtol(ver, &eptr, 10);
    if (vertex < 0)
    {
        fprintf(stderr, INVALID_INPUT);
        exit(EXIT_FAILURE);
    }
}

/**
 *
 * @param ver full line from text file
 * @param vertices array of nodes
 * @param line the index of the father node
 * @param vNum mount of nodes in graph
 * @return 0 if parse succeeded, else 1
 */
int childrenParse(char *ver, struct Vertex *vertices, int line, int vNum)
{
    int *children = NULL;
    children = (int *) malloc(vNum * sizeof(int));
    if (children == NULL)
    {
        fprintf(stderr, "Memory allocation problem");
        return EXIT_FAILURE;
    }
    if (strcmp(ver, EMPTY_LINE) == 0)
    {
        free(children);
        return 1;
    }
    ver[strcspn(ver, END_OF_LINE_2)] = 0; // removes end of line
    char *crop;
    crop = strtok(ver, SPACE);
    if (strcmp(crop, IS_LEAF_1) == 0 || strcmp(crop, IS_LEAF_2) == 0)
    {
        setAsLeaf(&vertices[line]); // set one node to be a leaf
        free(children);
        return EXIT_SUCCESS;
    }
    int child;
    int ind = 0;
    while (crop != NULL)
    {
        if (validatedVertex(crop))
        {
            free(children);
            return EXIT_FAILURE;
        }
        child = (int) strtol(crop, NULL, 10);
        if (child == 0 && *crop != 48)
        {
            free(children);
            return EXIT_FAILURE;
        }
        if (child >= vNum || child < 0 || vertices[child].father != DEFAULT_FATHER) // checks if vertex in scope + if it doesn't have a father
        {
            free(children);
            return EXIT_FAILURE;
        }
        vertices[child].father = line;
        children[ind] = child;
        ind++;
        crop = strtok(NULL, SPACE);
    }
    vertices[line].isLeaf = 0;
    vertices[line].children = children;
    vertices[line].amountOfChildren = ind;
    return EXIT_SUCCESS;
}

/**
 * iterates over vertices array and checks every node if it has a father, if there is more than 1 vertex w/o a father
 * or all vertices has a father, returns 1 (invalid tree) else returns 0 and sets the correct vertex to be the root
 * @param vNum mount of nodes in graph
 * @param vertices array of nodes
 * @return
 */
int rootValidity(int vNum, struct Vertex *vertices, FILE * file)
{
    int count = 0;
    int isRoot = ROOT_PROBLEM;
    for (int i = 0; i < vNum; ++i)
    {
        if(vertices[i].father == DEFAULT_FATHER)
        {
            count++;
            isRoot = i;
        }
    }
    if(count != 1)
    {
        freeMem(vertices, vNum, file);
        fprintf(stderr, INVALID_INPUT);
        exit(EXIT_FAILURE);
    }
    return isRoot;
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
 *  looks for the leaf with the shortest distance from root
 * @param vertices array of nodes
 * @param vNum amount of nodes in graph
 * @return the shortest path from root to leaf
 */
int findMinBranch(struct Vertex * vertices, int vNum)
{
    int minPath = vNum-1;
    for (int i = 0; i < vNum; ++i)
    {
        if(vertices[i].isLeaf)
        {
            if(vertices[i].bfsDist < minPath)
            {
                minPath = vertices[i].bfsDist;
            }
        }
    }
    return minPath;
}

/**
 * looks for the leaf with the longest distance from root
 * @param vertices array of nodes
 * @param vNum amount of nodes in graph
 * @return the longest path from root to leaf
 */
int findMaxPath(struct Vertex * vertices, int vNum)
{
    int maxPath = 0;
    for (int i = 0; i < vNum; ++i)
    {
        if(vertices[i].bfsDist > maxPath)
        {
            maxPath = vertices[i].bfsDist;
        }
    }
    return maxPath;
}

/**
 * Finds the deepest leaf in the tree
 * @param vertices
 * @param vNum number of vertices
 * @return the index of the deepest node
 */
int findMaxNode(struct Vertex * vertices, int vNum)
{
    int maxPath = 0;
    int maxNode = 0;
    for (int i = 0; i < vNum; ++i)
    {
        if(vertices[i].isLeaf)
        {
            if(vertices[i].bfsDist > maxPath)
            {
                maxPath = vertices[i].bfsDist;
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
 * @param vNum number of vertices in G
 * @return 0 if succeeded else 1
 */
void BFS(struct Vertex *vertices, int s, int vNum, FILE * file)
{
    for (int i = 0; i < vNum; ++i)
    {
        vertices[i].prev = PREV;
        vertices[i].bfsDist = -1;
    }
    vertices[s].bfsDist = 0;
    Queue * q = allocQueue();
    if(q == NULL)
    {
        freeMem(vertices, vNum, file);
        free(q);
        fprintf(stderr, "Memory allocation problem");
        exit(EXIT_FAILURE);
    }
    enqueue(q, s);
    while(!queueIsEmpty(q))
    {
        unsigned int u = dequeue(q);
        int HasFather = 0;
        if(vertices[u].father != DEFAULT_FATHER)
        {
            HasFather = 1;
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
}

/**
 * Helps parser with non digit char
 * @param vertex
 * @return 0 if succeeded else 1
 */
int validatedVertex(char * vertex)
{
    for (int i = 0; i < (int) strlen(vertex); ++i)
    {
        if(!isdigit(vertex[i]))
        {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

/**
 * checks if a given vertex in scope or not
 * @param vNum amount of nodes in Graph
 * @param ver the given parsed vertex
 */
void inScope(int vNum, int ver)
{
    if(ver < 0 || ver >= vNum)
    {
        fprintf(stderr, INVALID_INPUT);
        exit(EXIT_FAILURE);
    }
}

/**
 * Initiating values into all vertices
 * @param vertices array of nodes
 * @param vNum number of nodes in Graph
 */
void initiatingValues(struct Vertex * vertices, int vNum)
{
    for (int m = 0; m < vNum; ++m)
    {
        vertices[m].children = NULL;
        vertices[m].father = DEFAULT_FATHER;
        vertices[m].prev = PREV;
    }
}

/**
 * Parses line by line and builds the tree. exits in cases of invalid input
 * @param vNum number of vertices in graph
 * @param file pointer to the file to parse
 * @param vertices an array of nodes
 */
void treeCreation(int vNum, FILE * file, struct Vertex * vertices)
{
    char parse[LINE_SIZE];
    for (int i = 0; i < vNum; ++i)
    {
        fgets(parse, LINE_SIZE, file);
        int validation = childrenParse(parse, vertices, i, vNum); //parsing children
        if(validation) // checking if children parse succeeded
        {
            freeMem(vertices, vNum, file);
            fprintf(stderr, INVALID_INPUT);
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < vertices[i].amountOfChildren; ++j)
        {
            // checks if children are in vNum scope + checks if a child equals to it's father
            if(vertices[i].children[j] < 0 || vertices[i].children[j] >= vNum || vertices[i].children[j] == i)
            {
                freeMem(vertices, vNum, file);
                fprintf(stderr, INVALID_INPUT);
                exit(EXIT_FAILURE);
            }
        }
    }
}