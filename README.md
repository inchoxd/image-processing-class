# image-processing-class
## 使い方
### 画像の表示
画像を表示するには以下のように実行してください．
```sh
imread $file_path
```
以下のオプションを使用して表示したい画像のファイルパスを指定することもできます．
```sh
imread -f $file_path
```

### 色空間の指定
色空間を指定するには以下のように行います．`rgb`, `ycrcb`, `mono`が使用できます．
```sh
imread -f $file_path -c rgb
```
