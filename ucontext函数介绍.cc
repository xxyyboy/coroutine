//在类System V环境中,在头文件< ucontext.h > 中定义了两个结构类型，mcontext_t和ucontext_t
//四个函数getcontext(),setcontext(),makecontext(),swapcontext().利用它们可以在一个进程中实现用户级的线程切换。
//mcontext_t类型与机器相关，并且不透明.ucontext_t结构体则至少拥有以下几个域:


//当当前上下文(如使用makecontext创建的上下文）运行终止时系统会恢复uc_link指向的上下文；
//uc_sigmask为该上下文中的阻塞信号集合；
//uc_stack为该上下文中使用的栈；
//uc_mcontext保存的上下文的特定机器表示，包括调用线程的特定寄存器等。

typedef struct ucontext {      //上下文结构体
    struct ucontext *uc_link;  // 该上下文执行完时要恢复的上下文
    sigset_t         uc_sigmask;  //为该上下文中的阻塞信号集合
    stack_t          uc_stack;  //为该上下文中使用的栈
    mcontext_t       uc_mcontext;  //保存的上下文的特定机器表示，包括调用线程的特定寄存器等
    ...  
} ucontext_t;  
int getcontext(ucontext_t *ucp); //将当前上下文保存到ucp
int setcontext(const ucontext_t *ucp); //切换到上下文ucp
void makecontext(ucontext_t *ucp, void (*func)(), int argc, ...); //修改上下文入口函数
int swapcontext(ucontext_t *oucp, ucontext_t *ucp); //保存当前上下文到oucp，切换到上下文ucp



#include <stdio.h>
#include <ucontext.h>

static ucontext_t ctx[3];

static void func1(void)
{
    // 切换到func2
    swapcontext(&ctx[1], &ctx[2]);

    // 返回后，切换到ctx[1].uc_link，也就是main的swapcontext返回处
}
static void func2(void)
{
    // 切换到func1
    swapcontext(&ctx[2], &ctx[1]);

    // 返回后，切换到ctx[2].uc_link，也就是func1的swapcontext返回处
}

int main (void)
{
    // 初始化context1，绑定函数func1和堆栈stack1
    char stack1[8192];
    getcontext(&ctx[1]);
    ctx[1].uc_stack.ss_sp   = stack1;
    ctx[1].uc_stack.ss_size = sizeof(stack1);
    ctx[1].uc_link = &ctx[0];
    makecontext(&ctx[1], func1, 0);

    // 初始化context2，绑定函数func2和堆栈stack2
    char stack2[8192];
    getcontext(&ctx[2]);
    ctx[2].uc_stack.ss_sp   = stack2;
    ctx[2].uc_stack.ss_size = sizeof(stack1);
    ctx[2].uc_link = &ctx[1];
    makecontext(&ctx[2], func2, 0);

    // 保存当前context，然后切换到context2上去，也就是func2
    swapcontext(&ctx[0], &ctx[2]);
    return 0;
}




#include<stdio.h>
#include<ucontext.h>

void funtion(){
    printf("run this\n");
}

int main()
{
    printf("in main\n");
    char stack[1024];
    ucontext_t main,other;
    getcontext(&main);   //获取当前上下文
    
    main.uc_stack.ss_sp = stack;          //指定栈空间
    main.uc_stack.ss_size = sizeof(stack);//指定栈空间大小
    main.uc_stack.ss_flags = 0;
    main.uc_link = &other;                //将后继上下文指向other

    makecontext(&main,funtion,0);         //为main指定要执行的函数
    
    swapcontext(&other,&main);            //激活main，并将当前上下文保存到other
    printf("in main\n");
    return 0;
}

output

in main
run this
in main
核心代码

ucontext_t main,other;
getcontext(&main);                    //获取当前上下文
main.uc_stack.ss_sp = stack;          //指定栈空间
main.uc_stack.ss_size = sizeof(stack);//指定栈空间大小
main.uc_stack.ss_flags = 0;
main.uc_link = &other;                //将后继上下文指向other
makecontext(&main,funtion,0);         //为main指定要执行的函数
swapcontext(&other,&main);            //激活main，并将当前上下文保存到other
