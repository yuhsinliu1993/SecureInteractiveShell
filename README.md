# Secure Interactive Shell

**A secure interactive shell running in a chroot'ed environment.**

**No system binaries nor libraries are available in the chroot'ed environment.**

## Requirements

Launch the program as a service using [xinetd](https://en.wikipedia.org/wiki/Xinetd)

    # How to install
    $ sudo apt-get install xinetd

    # How to config the xinetd service (See the example configuration in xinetd.conf file)
    $ sudo vi /etc/xinetd.conf

    # Restart xinetd
    $ sudo /etc/init.d/xinetd restart

## Compile & Test locally (For Linux env)

    # Compile the code statically (meaning this program does not depend on any external libraries)
    $ mkdir bin && gcc -static main.c -o bin/main

    # Test locally
    $ telnet localhost 33445


## Supported commands
```
cat {file}:              Display content of {file}.
cd {dir}:                Switch current working directory to {dir}.
chmod {mode} {file/dir}: Change the mode (permission) of a file or directory.
                         {mode} is an octal number.
echo {str} [filename]:   Display {str}. If [filename] is given,
                         open [filename] and append {str} to the file.
exit:                    Leave the shell.
find [dir]:              List files/dirs in the current working directory
                         or [dir] if it is given.
                         Minimum outputs contatin file type, size, and name.
help:                    Display help message.
id:                      Show current euid and egid number.
mkdir {dir}:             Create a new directory {dir}.
pwd:                     Print the current working directory.
rm {file}:               Remove a file.
rmdir {dir}:             Remove an empty directory.
stat {file/dir}:         Display detailed information of the given file/dir.
touch {file}:            Create {file} if it does not exist,
                         or update its access and modification timestamp.
umask {mode}:            Change the umask of the current session.
```
