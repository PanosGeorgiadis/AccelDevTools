## LINUX UTILS

    top - process monitor
    vmstat - virtual memory statistics
    lsof - list open files
    tcpdump - network packet analyzer
    netstat - network statistics
    htop - process monitor
    iotop - monitor disk i/o
    ps - process snapshot
    lsmem - list available memory ranges and their status
    scp - secure (/remote) copy



--------------------------------------------------


## Useful commands

- How to see which terminal I am using?

```
> echo $SHELL
```
_or_
```
> echo $0
```

- How to check if a Twinscan process is running

```
ps aux | grep CCPROCNAME
```

- How to search for text

```
fgrep -R text
```

- How to display subdirectories and their total size

```
du -sh *
```
_note: that's short version of_
```
du --summarize --human-readable *
```

- How to find the Linux version you are running:

```
uname -srm
```

- How to find the Linux distribution you are using:

```
cat /etc/os-release
```

- How to find which GNOME version is running:

```
gnome-shell --version 
```

- how to see the version of installed rpm packages containing 'code' in the name (i.e. for VsCode)

```
rpm -qa | code
```
- what is todays date?

```
date +%Y.%m.%d
```

## Shell scripting quick reference

https://www.shellscript.sh/quickref.html

- how to change file/dir access rights:

```shell
stat -c %a {filename}

```
