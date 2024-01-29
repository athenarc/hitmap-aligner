
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
    FILE *fp = fopen("../../../../NCBI/data/GCA_000001635.9/GCA_000001635.9_GRCm39_genomic.fna", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    char *line = NULL;
    char *line2 = ">CM001012.3 Mus musculus chromosome 19, GRCm39 reference primary assembly C57BL/6J";
    size_t len = 0;
    while ((getline(&line, &len, fp)) != -1)
    {
        if(strcmp(line, line2) == 0)
        {
            // found the 19th chromosome
            printf("%s\n", line);
        }
    }
    fclose(fp);
    if (line)
        free(line);
}