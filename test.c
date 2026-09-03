#include <stdio.h>
#include <stdlib.h>
char* GETDE() {
    char *de = getenv("XDG_CURRENT_DESKTOP");
    if (de && de[0] != '\0') {
        return de;
    }
    return NULL;
}

int main()
{
    char *de = GETDE();
    if (de != NULL){
        printf("%s\n", de);
        free(de);
    }
    return 0;
}

