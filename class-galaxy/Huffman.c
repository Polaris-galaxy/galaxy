#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct {
    int weight;
    int parent;
    int lchild;
    int rchild;
} HNode;

void selectMin(HNode *tree, int n, int *s1, int *s2) {
    *s1 = *s2 = 0;
    // 找到第一个最小值
    for (int i = 1; i <= n; i++) {
        if (tree[i].parent == 0) {
            if (*s1 == 0 || tree[i].weight < tree[*s1].weight) {
                *s1 = i;
            }
        }
    }
    // 找到第二个最小值
    for (int i = 1; i <= n; i++) {
        if (i != *s1 && tree[i].parent == 0) {
            if (*s2 == 0 || tree[i].weight < tree[*s2].weight) {
                *s2 = i;
            }
        }
    }
    // 确保s1 < s2，避免重复
    if (*s1 > *s2) {
        int temp = *s1;
        *s1 = *s2;
        *s2 = temp;
    }
}

int main() {
    char str[1000];
    while (scanf("%s", str) != EOF) {
        if (strcmp(str, "0") == 0) break;

        int freq[26] = {0};
        int len = strlen(str);
        for (int i = 0; i < len; i++)
            freq[str[i] - 'a']++;

        // 收集出现的小写字母并排序
        char chars[26];
        int m = 0;
        for (int i = 0; i < 26; i++) {
            if (freq[i] > 0) {
                chars[m++] = 'a' + i;
            }
        }

        // 输出字符频率
        for (int i = 0; i < m; i++) {
            if (i != 0) printf(" ");
            printf("%c:%d", chars[i], freq[chars[i] - 'a']);
        }
        printf("\n");

        if (m == 0) continue;

        // 初始化哈夫曼树
        int totalNodes = 2 * m - 1;
        HNode *huffTree = (HNode *)malloc((totalNodes + 1) * sizeof(HNode)); // 1-based索引

        // 初始化叶子节点
        for (int i = 1; i <= m; i++) {
            huffTree[i].weight = freq[chars[i - 1] - 'a'];
            huffTree[i].parent = 0;
            huffTree[i].lchild = 0;
            huffTree[i].rchild = 0;
        }

        // 构建内部节点
        for (int i = m + 1; i <= totalNodes; i++) {
            int s1, s2;
            selectMin(huffTree, i - 1, &s1, &s2);
            huffTree[s1].parent = i;
            huffTree[s2].parent = i;
            huffTree[i].lchild = s1;
            huffTree[i].rchild = s2;
            huffTree[i].weight = huffTree[s1].weight + huffTree[s2].weight;
            huffTree[i].parent = 0;
        }

        // 输出哈夫曼树存储结构
        for (int i = 1; i <= totalNodes; i++) {
            printf("%d %d %d %d %d\n", i, huffTree[i].weight, huffTree[i].parent, huffTree[i].lchild, huffTree[i].rchild);
        }

        // 生成哈夫曼编码
        typedef struct {
            char c;
            char code[100];
        } HuffCode;
        HuffCode huffCodes[26];
        for (int i = 0; i < m; i++) {
            char c = chars[i];
            int leafNode = i + 1;
            int current = leafNode;
            int parent = huffTree[current].parent;
            char tempCode[100] = {0};
            int codeLen = 0;

            while (parent != 0) {
                if (huffTree[parent].lchild == current) {
                    tempCode[codeLen++] = '0';
                } else {
                    tempCode[codeLen++] = '1';
                }
                current = parent;
                parent = huffTree[current].parent;
            }

            // 反转编码
            for (int j = 0; j < codeLen / 2; j++) {
                char tmp = tempCode[j];
                tempCode[j] = tempCode[codeLen - 1 - j];
                tempCode[codeLen - 1 - j] = tmp;
            }

            // 处理仅有一个字符的情况
            if (codeLen == 0) {
                tempCode[0] = '0';
                codeLen = 1;
            }
            tempCode[codeLen] = '\0';
            strcpy(huffCodes[i].code, tempCode);
            huffCodes[i].c = c;
        }

        // 输出哈夫曼编码
        for (int i = 0; i < m; i++) {
            if (i != 0) printf(" ");
            printf("%c:%s", huffCodes[i].c, huffCodes[i].code);
        }
        printf("\n");

        // 编码原字符串
        char encodedStr[100000] = {0};
        for (int i = 0; i < len; i++) {
            char c = str[i];
            for (int j = 0; j < m; j++) {
                if (huffCodes[j].c == c) {
                    strcat(encodedStr, huffCodes[j].code);
                    break;
                }
            }
        }
        printf("%s\n", encodedStr);

        // 解码
        char decodedStr[1000] = {0};
        int decodedLen = 0;

        if (m == 1) {
            // 处理只有一个字符的情况
            char c = chars[0];
            for (int i = 0; encodedStr[i] != '\0'; i++) {
                decodedStr[decodedLen++] = c;
            }
        } else {
            int root = totalNodes;
            int current = root;
            for (int i = 0; encodedStr[i] != '\0'; i++) {
                current = (encodedStr[i] == '0') ? huffTree[current].lchild : huffTree[current].rchild;

                if (huffTree[current].lchild == 0 && huffTree[current].rchild == 0) {
                    decodedStr[decodedLen++] = chars[current - 1];
                    current = root;
                }
            }
        }
        decodedStr[decodedLen] = '\0';
        printf("%s\n", decodedStr);

        free(huffTree);
    }
    return 0;
}