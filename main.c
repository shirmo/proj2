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


struct Vertex
{
    int isLeaf;
    int isRoot;
    struct Vertex *children;
    int amountOfChildren;
    // If is root - amount of Children == v.degree else, amount of Children +1 == v.degree
    int degree;
} Vertex;


int verticesAmountValidity(const char *ver);
int children_parse(const char *ver, struct Vertex *vertices, int index);
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
        fprintf(stderr, INVALID_INPUT);
        return EXIT_FAILURE;
    }

    char parse[1024];
    //FIRST_LINE - NUM OF VERTEX (check if is digit)
    fgets(parse, 1024, ptr);
   // Parsing first line;
    if(verticesAmountValidity(parse)) {
        fprintf(stderr, INVALID_INPUT);
        return EXIT_FAILURE;
    }
    char *eptr;
    Vnum = (int) strtol(parse,&eptr, 10);

    //vertex argument validity check
    if(verticesAmountValidity(argv[VERTEX_1_PLACE]) || verticesAmountValidity(argv[VERTEX_2_PLACE]))
    {
        fprintf(stderr, INVALID_INPUT);
        return EXIT_FAILURE;
    }
    V1 = (int) strtol(argv[VERTEX_1_PLACE],&eptr, 10);
    V2 = (int) strtol(argv[VERTEX_2_PLACE],&eptr, 10);
    if(V1<0 || V1>=Vnum || V2<0 || V2>=Vnum)     // If argument given nodes aren't in Vertices scope, leave program
    {
        fprintf(stderr, INVALID_INPUT);
        return EXIT_FAILURE;
    }
    //initiating a vertices array
    struct Vertex vertices[Vnum];
    for (int i = 0; i < Vnum; ++i) {
        fgets(parse, 1024, ptr);
        children_parse(parse, vertices, i);
        printf("node %d has %d children",i, vertices[i].amountOfChildren);
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

int children_parse(const char *ver, struct Vertex * vertices, int index)
{
    if (ver[0]=='-')
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
        while(ver[i]!=' ')
        {
            if(isdigit(ver[i])==0)
            {
                return 1;
            }
            i++;
        }
        int child = (int) strtol(eptr, &eptr, 10);
        vertices[index].children[ind] = vertices[child];
        ind++;
    }
    vertices[index].isLeaf = 0;
    vertices[index].amountOfChildren = ind;
    return 0;
}