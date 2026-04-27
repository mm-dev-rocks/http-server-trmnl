#include <stdio.h>
#include <string.h>

int main() {
    char *src = "GET /path HTTP/1.1\r\n";

    if (src == NULL) {
        return -1;
    }
    if (*src == '\0') {
        return -1;
    }

    int srcIndex = 0;
    int tokensCount = 0;
    char tokens[3][128];

    while (src[srcIndex] != '\0') {
        printf("while 1, src: %c\n", src[srcIndex]);
        int destIndex = 0;
        while (src[srcIndex] != ' ') {
            printf("while 2, src: %c\n", src[srcIndex]);
            if (src[srcIndex] == '\n') {
                if (tokensCount == 2) {
                    // reached the end
                    // if (src[srcIndex + 1] == '\n') {
                    //    printf("point1\n");
                    //    return 0;
                    //}
                    // printf("point1\n");
                    for (int i = 0; i < 3; i++) {
                        printf("%s\n", tokens[i]);
                    }
                    printf("point2\n");
                    return 0;
                } else {
                    // invalid
                    printf("error\n");
                    return -1;
                }
            }
            tokens[tokensCount][destIndex] = src[srcIndex];
            srcIndex++;
            destIndex++;
        }
        // Null terminate token
        tokens[tokensCount][destIndex] = '\0';
        // Go past space
        srcIndex++;
        // Go to next token
        tokensCount++;
    }
    tokens[tokensCount][srcIndex] = '\0';

    for (int i = 0; i < 3; i++) {
        printf("%s\n", tokens[i]);
    }
    printf("point2\n");
    return 0;
}
