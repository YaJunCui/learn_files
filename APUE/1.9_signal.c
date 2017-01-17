#include <apue/apue.h>
#include <sys/wait.h>

static void sig_int(int);         //SIGINT信号处理函数

int main()
{
  char buf[MAXLINE] = {0};
  pid_t pid;
  int status;

  if(signal(SIGINT, sig_int) == SIG_ERR) 
  {
    err_sys("signal error");
  }

  printf("%% ");
  while (fgets(buf, MAXLINE, stdin) != NULL)      //获取命令
  {
    if(buf[strlen(buf) - 1] == '\n')              //将换行符置为NULL
      buf[strlen(buf) - 1] = 0;

    if((pid = fork()) < 0)
    {
      err_sys("fork error");
    }
    else if(pid == 0)               //child
    {
      execlp(buf, buf, (char*)0);
      err_ret("couldn't execute: %s", buf);
      exit(127);
    }

    //parent
    if((pid = waitpid(pid, &status, 0)) < 0)
    {
      err_sys("waitpid error");
    }
    printf("%% ");
  }

  exit(0);
}

void sig_int(int signo)
{
  printf("interrupt\n%% ");
}