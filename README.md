# image-processing-class
## 使い方
### 画像の表示
画像を表示するには以下のように実行してください．
```shell
./imread $file_path
```
以下のオプションを使用して表示したい画像のファイルパスを指定することもできます．
```shell
./imread -f $file_path
```

### 色空間の指定
色空間を指定するには以下のように行います．`rgb`, `ycrcb`, `mono`が使用できます．指定しないと`rgb`で表示されます．
```shell
./imread -f $file_path -c rgb
```

### パターンの指定
パターンを指定するには以下のように行います．`ulc`, `zebra`, `checkered`, `mozaic`が指定できます．
```shell
./imread -f $file_path -p zebra
```

### 表示方法の指定
表示方法を指定するには以下のように行います．`single`と`tiled`が選べます．指定しないと画像１枚のみ表示される`single`が適用されます．`tiled`を使用すると`mozaic`を除いたすべてのパターンが1枚の画面に表示できます．パターンを指定している場合は`tiled`を選べません．
```shell
./imread -f $file_path -d tiled
```

### JPEG圧縮
JPEG圧縮して画像を表示するには以下のコマンドを使用します．0-100の範囲でクオリティを入力します．色空間を`mono`に指定していると使用できません．
```shell
./imread -f $file_path -d tiled
```
