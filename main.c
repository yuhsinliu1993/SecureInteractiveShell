#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <utime.h>    // change file last access and modification times
#include <errno.h>

#define BUFFSIZE 4096

void err_sys(const char* msg){
  perror(msg);
  // exit(1);
}

void cat(const char* filepath){
  // Display content of {file}.
  int fd, n;
  char buf[BUFFSIZE];
  if((fd=open(filepath, O_RDONLY)) < 0)
    err_sys("open error");

  while((n=read(fd, buf, BUFFSIZE)) > 0)
    if(write(1, buf, n) != n)
      err_sys("write error");

  if(n < 0)
    err_sys("read error");
}

void cd(const char* path){
  // Switch current working directory to {dir}
  if(chdir(path)==0) return;
  else err_sys("cd error");
}

void _chmod(const char* mode, const char* name){
  // Change the mode (permission) of a file or directory.
  // {mode} is an octal number.
  int i;
  i = strtol(mode, 0, 8);

  if(chmod(name, i) < 0) err_sys("chmod error");
}

void echo(const char* str, const char* filename){
  // Display {str}. If [filename] is given
  // open [filename] and append {str} to the file.
  if(filename){
    int fd;
    fd=open(filename, O_CREAT | O_WRONLY | O_APPEND);
    if(fd < 0) err_sys("open error");
    else dprintf(fd, "%s\n", str);
  } else
    printf("%s\n", str);
}

int find(const char* dirpath){
  // List files/dirs in the current working directory or [dir] if it is given.
  int count = 0;
  DIR *dir;
  struct dirent *ent;

  if((dir=opendir(dirpath)) != NULL){
    printf("  FILENAME               TYPE        SIZE(byte)\n");
    while ((ent=readdir(dir)) != NULL){
      struct stat buf;
      stat(ent->d_name, &buf);
      dprintf(1, "%10s\t", ent->d_name);
      switch (buf.st_mode & S_IFMT) {
        case S_IFBLK:  printf("    block device");     break;
        case S_IFCHR:  printf("character device"); break;
        case S_IFDIR:  printf("       directory");        break;
        case S_IFIFO:  printf("       FIFO/pipe");        break;
        case S_IFLNK:  printf("         symlink");          break;
        case S_IFREG:  printf("    regular file");     break;
        case S_IFSOCK: printf("          socket");           break;
        default:       printf("        unknown?");         break;
      }
      dprintf(1, "%15lld\n", (long long)buf.st_size);

      if(ent->d_name != "." && ent->d_name != "..") count++;
    }

    closedir(dir);
    return count;
  } else
    err_sys("No such directory\n");

  return -1;
}

void help(){
  // Display help message.
  char *str =
     "cat {file}:              Display content of {file}.\n"
     "cd {dir}:                Switch current working directory to {dir}.\n"
     "chmod {mode} {file/dir}: Change the mode (permission) of a file or directory.\n"
     "                         {mode} is an octal number.\n"
     "echo {str} [filename]:   Display {str}. If [filename] is given,\n"
     "                         open [filename] and append {str} to the file.\n"
     "exit:                    Leave the shell.\n"
     "find [dir]:              List files/dirs in the current working directory\n"
     "                         or [dir] if it is given.\n"
     "                         Minimum outputs contatin file type, size, and name.\n"
     "help:                    Display help message.\n"
     "id:                      Show current euid and egid number.\n"
     "mkdir {dir}:             Create a new directory {dir}.\n"
     "pwd:                     Print the current working directory.\n"
     "rm {file}:               Remove a file.\n"
     "rmdir {dir}:             Remove an empty directory.\n"
     "stat {file/dir}:         Display detailed information of the given file/dir.\n"
     "touch {file}:            Create {file} if it does not exist,\n"
     "                         or update its access and modification timestamp.\n"
     "umask {mode}:            Change the umask of the current session.\n";
  printf("%s", str);
}

void id(){
  // Show current euid and egid number.
  unsigned int euid, egid;
  euid = geteuid();
  egid = getegid();

  printf("euid = %d, egid = %d\n", euid, egid);
}

void _mkdir(const char* dirname){
  // Create a new directory {dir}
  if(mkdir(dirname, 0775)==0) return;
  else err_sys("mkdir error");
}

void pwd(){
  // Print the current working directory.
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL){
    printf("%s\n", cwd);
  } else {
    err_sys("pwd error");
  }
}

void rm(const char* filepath){
  // Remove a file.
  if(remove(filepath)==0) return;
  else err_sys("rm error");
}

void _rmdir(const char* dirpath){
  // Remove an empty directory
  int count = 0;
  DIR *dir;
  struct dirent *ent;

  if((dir=opendir(dirpath)) != NULL){
    while ((ent=readdir(dir)) != NULL)
      if(strncmp(ent->d_name, ".", strlen(ent->d_name)) == 0 || strncmp(ent->d_name, "..", strlen(ent->d_name)) == 0){}
      else count++;
    closedir(dir);
  } else
    err_sys("No such directory\n");

  if(count == 0){
    if(rmdir(dirpath)==0) return;
    else err_sys("rmdir error");
  } else
    err_sys("rmdir error\n");
}

void _stat(const char* name){
  // Display detailed information of the given file/dir.
  struct stat sb;
  stat(name, &sb);

  printf("File type:                ");
  switch (sb.st_mode & S_IFMT) {
    case S_IFBLK:  printf("block device\n");     break;
    case S_IFCHR:  printf("character device\n"); break;
    case S_IFDIR:  printf("directory\n");        break;
    case S_IFIFO:  printf("FIFO/pipe\n");        break;
    case S_IFLNK:  printf("symlink\n");          break;
    case S_IFREG:  printf("regular file\n");     break;
    case S_IFSOCK: printf("socket\n");           break;
    default:       printf("unknown?\n");         break;
  }

  printf("I-node number:            %ld\n", (long) sb.st_ino);
  printf("Mode:                     %lo (octal)\n", (unsigned long) sb.st_mode);
  printf("Link count:               %ld\n", (long) sb.st_nlink);
  printf("Ownership:                UID=%ld   GID=%ld\n", (long) sb.st_uid, (long) sb.st_gid);
  printf("Preferred I/O block size: %ld bytes\n", (long) sb.st_blksize);
  printf("File size:                %lld bytes\n", (long long) sb.st_size);
  printf("Blocks allocated:         %lld\n", (long long) sb.st_blocks);
  printf("Last status change:       %s", ctime(&sb.st_ctime));
  printf("Last file access:         %s", ctime(&sb.st_atime));
  printf("Last file modification:   %s", ctime(&sb.st_mtime));
}

void touch(const char* filename){
  // Create {file} if it does not exist,
  // or update its access and modification timestamp.
  int fd;
  mode_t old_mask = umask((mode_t) 0);
  int flag = 0666 & ~old_mask;

  // Ensure  that  this  call creates the file: if this flag is specified in conjunction with
  // O_CREAT, and pathname already exists, then open() will fail.
  fd = open(filename, O_CREAT | O_WRONLY | O_EXCL, flag);
  umask(old_mask);

  if(fd < 0){
    // open failure
    if(errno == EEXIST){
      // file exists, update timestamp
      struct stat buf;
      stat(filename, &buf);

      struct utimbuf new_times;
      new_times.actime = time(NULL);     // set to current time
      new_times.modtime = time(NULL);    // set to current time

      utime(filename, &new_times);
    } else
      err_sys("touch error");
  }
}

mode_t _umask(const char* mode){
   // Change the umask of the current session.
   int i;
   i = strtol(mode, 0, 8);

   mode_t old_mask, new_mask;

   old_mask = umask((mode_t) i);

   printf("Old mask = %04o\n", (int) old_mask);

   return old_mask;
}


int main(int argc, char* argv[])
{
    char cmd[256];
    uid_t euid;
    gid_t egid;

    setbuf(stdout, NULL);  // printf without buffer

    if(argc == 3){
      euid = atoi(argv[1]);
      egid = atoi(argv[2]);
    } else {
      euid = 0; egid = 0;
    }

    seteuid(euid);
    setegid(egid);

    while(1){
      int n_spaces = 0, i = 0, k, z;

      memset(cmd, 0, 256);

      write(1, "> ", 2);
      read(0, cmd, sizeof(cmd));

      cmd[strcspn(cmd, "\r\n")] = 0;

      char *arg[10];
      char *p = strtok(cmd, " ");

      while(p != NULL){
        arg[i++] = p;
        p = strtok(NULL, " ");
      }

      if(strncmp(arg[0], "cat", strlen(arg[0])) == 0) cat(arg[1]);
      else if(strncmp(arg[0], "cd", strlen(arg[0])) == 0) cd(arg[1]);
      else if(strncmp(arg[0], "chmod", strlen(arg[0])) == 0) _chmod(arg[1], arg[2]);
      else if(strncmp(arg[0], "echo", strlen(arg[0])) == 0){
        if(i==3) echo(arg[1], arg[2]);
        else if(i==2) echo(arg[1], NULL);
      }
      else if(strncmp(arg[0], "find", strlen(arg[0])) == 0){
        if(i==1) find(".");
        else if(i==2) find(arg[1]);
      }
      else if(strncmp(arg[0], "id", strlen(arg[0])) == 0) id();
      else if(strncmp(arg[0], "mkdir", strlen(arg[0])) == 0) _mkdir(arg[1]);
      else if(strncmp(arg[0], "help", strlen(arg[0])) == 0) help();
      else if(strncmp(arg[0], "pwd", strlen(arg[0])) == 0) pwd();
      else if(strncmp(arg[0], "rm", strlen(arg[0])) == 0) rm(arg[1]);
      else if(strncmp(arg[0], "rmdir", strlen(arg[0])) == 0) _rmdir(arg[1]);
      else if(strncmp(arg[0], "stat", strlen(arg[0])) == 0) _stat(arg[1]);
      else if(strncmp(arg[0], "touch", strlen(arg[0])) == 0) touch(arg[1]);
      else if(strncmp(arg[0], "umask", strlen(arg[0])) == 0) _umask(arg[1]);
      else if(strncmp(arg[0], "exit", strlen(arg[0])) == 0) break;
    }

    return 0;
}
