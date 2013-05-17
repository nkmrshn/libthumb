libthumb
========

指定したEXIFファイルから、EXIFのサムネイル(JPEGInterchangeFormat)を取得するライブラリです。

ライブラリの作成
----------------

静的ライブラリの生成例：

    $ gcc -c libthumb.c util.c
    $ ar -r libthumb.a libthumb.o util.o

共有ライブラリの生成例：

    $ gcc -shared libthumb.c util.c -o libthumb.so

サンプルの概要
--------------

sample.cをコンパイルし、実行すると第1引数で指定したEXIFファイルのサムネイルを第2引数でしたファイルとして出力します。第2引数を指定しなかった場合は、第1引数のファイル名、サムネイルのサイズ(JPEGInterchangeFormatLength)、回転方向(Orientation)を標準出力します。

詳しくは、sample.cをご覧下さい。

sample.cをコンパイルする例：

（静的ライブラリ）

    $ gcc -c sample.c
    $ gcc -o sample sample.o libthumb.a

（共有ライブラリ）

    $ gcc -I./ -L./ sample.c -o sample -lthumb

実行例：

    $ ./sample ~/Pictures/DSCN0001.JPG Thumbnail.JPG

    $ ./sample ~/Pictures/DSCN0001.JPG
    /Users/nkmrshn/Pictures/DSCN0001.JPG, 12107, 6

関数一覧
--------

###thumb_read

* 書式

  THUMB_DATA *thumb_read(char *path);

* 説明

  pathに指定したEXIFファイルのサムネイルをTHUMB_DATA構造体のdata、そのサイズをlengthに代入します。また、回転方向をorientationに代入します。 

* 戻り値

  THUMB_DATAのポインタを返します。

---

###thumb_free

* 書式

  void thumb_free(THUMB_DATA *thumb);

* 説明

  THUMB_DATAを解放します。

* 戻り値

  無し。

連結リスト
----------

    typedef struct THUMB_DATA_tag {
      unsigned char *data;    //  サムネイル(JPEGInterchangeFormat)
      long length;            //  サイズ(JPEGInterchangeFormatLength)
      short orientation;      //  回転方向(Orientation)
    } THUMB_DATA;

備考
----

ライセンスについては、LICENSE.mdをご覧下さい。
