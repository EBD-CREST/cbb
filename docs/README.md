# Cloud Burst File System 

Cloud Burst File System is a file system implementation with Cloud Burst Buffer feature.


## 概要

クラウドにあるデータをFUSEでマウントし、分散アクセスすることによって転送速度を上げるツールです。


## 依存関係（ライブラリ）

- [CMake](http://www.cmake.org/) >= 2.8.12
- [FUSE](http://fuse.sourceforge.net/) >= 2.8.3
- [MessagePack-RPC for C++](http://download.jubat.us/files/source/jubatus_msgpack-rpc/) >= 0.4.4
- [jubatus-mpio](http://download.jubat.us/files/source/jubatus_mpio/) >= 0.4.0
- [boost](http://www.boost.org/) >= 1.50.0


## ビルド＆インストール方法

ビルドツールには CMake を使用しています。

1. $ cd .; mkdir build; cd build
2. $ export JUBATUS\_MSGPACK\_RPC\_DIR=/path/to/{jubatus\_msgpack-rpc}; export JUBATUS\_MPIO\_DIR=/path/to/{jubatus\_mpio}
3. $ cmake -DCMAKE\_INSTALL\_PREFIX=/path/to/directory ..
4. $ make; make install (root privilege may be required)


## CBB設定ファイル

CBBを動かすための設定ファイルを設置します。

	$ vi /etc/cbb.conf

設定内容は次の通りです。

**サーバー側設定**

	[Server]
	;CBBサーバー(MsgPack)側のIPアドレスになります。シングルサーバーでローカル設定の場合 127.0.0.1 でも大丈夫です。
	host=127.0.0.1
	
	;CBBサーバー(MsgPack)の待ち受けポート番号です。
	port=9091
	
	;CBBサーバー(MsgPack)を開始するスレッド数を指定します。
	thread=2
	
	;CBBサーバーのローカルストレージにするパスを指定します。
	local_strage_path=/tmp/local
	
	;CBBサーバーのセカンドストレージにするパスを指定します。AWSのS3のマウント先等のパスになります。 
	secondary_storage_path=/tmp/second
	
	;local_strage_pathとsecondary_storage_pathのファイルを比較してコピーを行う監視間隔時間を分で指定します。
	interval_time=1
	
**クライアント側設定**
	
	[Client]
	;分散するサーバー(MsgPack)のIPを列挙します。複数サーバーの場合はカンマで区切ります。
	;host=192.168.1.1,192.168.1.2,192.168.1.3
	host=127.0.0.1
	
	;CBBサーバー(MsgPack)に接続するポート番号を指定します。
	port=9091

サーバー側、クライアント側の設定ファイルは同じ `/etc/cbb.conf` ファイルになるので、
同じPCの場合はファイルの中に両方の設定を記述してください。 

サンプルファイルは `<cbb source dir>/cbb/work/cbb.conf` にあります。


## 実行方法

シングルサーバーでの実行方法は次の通りです。

１．使用するディレクトリを作成します。

* ワークディレクトリ : `/work`
* ローカルストレージディレクトリ : `/tmp/local`
* セカンダリストレージディレクトリ : `/tmp/second`

２．`/etc/cbb.conf` を次のような内容で設定します。

	[Server]
	host=127.0.0.1
	port=9091
	thread=2
	local_strage_path=/tmp/local
	secondary_storage_path=/tmp/second
	interval_time=1
	
	[Client]
	host=127.0.0.1

３．コンソールからサーバー側モジュールの `cbb` を実行する

	$ cbb

４．別のコンソールからクライアント側モジュールの `cbfs` を実行する

	$ cbfs /work

５．ワークディレクトリに移動して作業を行う

６．マウントしたワークディレクトリを解放する

	$ sudo umount -l /work

７．サーバー側モジュールの `cbb` を CTRL+C で終了させる


## 使用方法

単純なファイルの読み書き

	# 「実行方法」に書かれている上記のサーバー側、クライアント側モジュールを実行後に下記のコマンドを入力してください
	$ cd /work
	$ echo "cbb test" > test.txt
	$ cat test.txt

tar を展開してコンパイル

	# 「実行方法」に書かれている上記のサーバー側、クライアント側モジュールを実行後に下記のコマンドを入力してください
	$ cd /work
	$ tar xvfz jubatus_mpio-0.4.5.tar.gz
	$ cd jubatus_mpio-0.4.5
	$ ./configure
	$ make


## テスト方法

テストデータのディレクトリは < cbb source dir >/cbb/tests/cases/testdata になります。

	# シングルサーバーのテスト
	$ cd <cbb mount dir>
	$ sh <cbb source dir>/cbb/tests/cases/test_setup.sh <test data dir>
	$ sh <cbb source dir>/cbb/tests/cases/test_run.sh

	# 複数サーバーのテスト
	$ cd <cbb mount dir>
	$ sh <cbb source dir>/cbb/tests/cases/test_setup.sh <test data dir>
	$ sh <cbb source dir>/cbb/tests/cases/test2_run.sh


## License

Cloud Burst File System is released under [Apache License Version 2.0](http://www.apache.org/licenses/LICENSE-2.0).

Copyright (C) 2015 Tokyo Institute of Technology
