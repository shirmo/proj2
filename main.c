#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
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


struct Vertex
{
    int isLeaf;
    int isRoot;
    int *children;
    int amountOfChildren;
    // If is root - amount of Children == v.degree else, amount of Children +1 == v.degree
    int degree;
    int father;
} Vertex;


int verticesAmountValidity(const char *ver);
int children_parse(const char *ver, struct Vertex *vertices, int index, int Vnum);
//int const *Vnum;  //Number of E = Vnum - 1

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
    struct Vertex vertices[Vnum];
    int line =0;
    for (int i = 0; i < Vnum; ++i) {
        line++;
        fgets(parse, LINE_SIZE, ptr);
        int validation = children_parse(parse, vertices, i, Vnum); //parsing children
        if(validation) // checking if children parse succeeded
        {
            printf("%d\n", line);
            printf("5\n");
            fprintf(stderr, INVALID_INPUT);
            return EXIT_FAILURE;
        }
        for (int j = 0; j < vertices[i].amountOfChildren; ++j) {
            // checks if children are in Vnum scope + checks if a child equals to it's father
            if(vertices[i].children[j] < 0 || vertices[i].children[j]>=Vnum || vertices[i].children[j] == i)
            {
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
        printf("7\n");
        fprintf(stderr, INVALID_INPUT);
        return EXIT_FAILURE;
    }


    for (int l = 0; l <Vnum ; ++l)
    {
        printf("vertex %d has %d children\n", l, vertices[l].amountOfChildren);
        for (int i = 0; i < vertices[l].amountOfChildren; ++i) {
            printf("%d is the father of %d\n" ,l, vertices[l].children[i]);
        }
        if(vertices[l].isLeaf)
        {
            printf("%d is also a leaf\n", l);
        }
        printf("%d's father is %d\n",l, vertices[l].father);
    }
    for (int k = 0; k < Vnum; ++k)
    {
        free(vertices[k].children);
    }
    return END_OF_PROGRAM;
}






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

int children_parse(const char *ver, struct Vertex *vertices, int index, int Vnum) //must handle end of line
{
    int* children = NULL;
    children = (int*) malloc(Vnum* sizeof(int));
    if (children ==NULL)
    {
        fprintf(stderr, "Memory allocation problem");
        return EXIT_FAILURE;
    }
    if (strcmp(ver,IS_LEAF_1)==0 || strcmp(ver,IS_LEAF_2)==0)
    {
        vertices[index].isLeaf = 1;
        vertices[index].children = NULL;
        vertices[index].degree = 1;
        vertices[index].amountOfChildren = 0;
        return 0;
    }
    char *eptr = ver;
    int ind = 0;
    for (int i = 0; i < (int) strlen(ver); ++i)
    {
        while(ver[i]!=' ' && ver[i]!=END_OF_LINE)
        {
            printf("%c", ver[i]);
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