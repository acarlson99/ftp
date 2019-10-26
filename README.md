# ftp

## Build

```sh
make
```

## Run server

```sh
./server 8080		# run server listening on port 8080
./server -d ftpdir 8080	# run specifying directory
```

## Run client

```sh
./client -4 127.0.0.1 8080	# run client on port 8080 with ipv4 addr
./client -l localhost 8080	# run client with hostname
```

## Commands

| command    | example        | summary                      |
|------------|----------------|------------------------------|
| `ls`       | `ls`           | list files in dir            |
| `cd dir`   | `cd ../`       | change dir                   |
| `pwd`      | `pwd`          | print working dir            |
| `get file` | `get file.txt` | download file                |
| `put file` | `put file.txt` | upload file                  |
| `!command` | `!cd /tmp`     | run command on local machine |
