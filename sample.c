#include <stdio.h>
#include "libthumb.h"

int main(int argc, char *argv[])
{
  THUMB_DATA *thumb;
  FILE *fp;

  if(argc > 1  &&
      (thumb = thumb_read(argv[1])) != NULL &&
      thumb->data != NULL &&
      thumb->length > 0) {
    if(argc > 2 && (fp = fopen(argv[2], "wb")) != NULL) {
      //  第2引数を指定している場合
      fwrite(thumb->data, sizeof(unsigned char), thumb->length, fp);
      fclose(fp);
    } else 
      //  ファイル名, サイズ, 画像方向(Orientation)
      printf("%s, %ld, %d\n", argv[1], thumb->length, thumb->orientation);
      
    thumb_free(thumb);
  }

  return 0;
}
