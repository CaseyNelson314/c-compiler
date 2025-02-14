#include "minc.h"

#include <stdio.h>

char* open_file(char *filepath)
{
    if (filepath[0] == '>')
    {
        return filepath + 1/* > */;
    }

    FILE* in_fd = fopen(filepath, "r");
    if (in_fd == NULL)
    {
        error("ファイルを開けませんでした");
    }

    char *out_buf;
    size_t out_len;
    FILE *out_fd = open_memstream(&out_buf, &out_len);
    if (out_fd == NULL)
    {
        error("出力用ストリームの作成に失敗しました");
    }

    for (;;)
    {
        char buf[1024];
        size_t len = fread(buf, 1, sizeof buf, in_fd);
        if (len == 0)
            break;
        fwrite(buf, 1, len, out_fd);
    }

    // 文字列が \n\0 と終わるように加工
    if (out_len == 0 || out_buf[out_len - 1] != '\n')
    {
        fputc('\n', out_fd);
    }

    fputc('\0', out_fd);
    fflush(out_fd);

    fclose(in_fd);
    fclose(out_fd);

    return out_buf;
}
