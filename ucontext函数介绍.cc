typedef struct ucontext {      //上下文结构体
    struct ucontext *uc_link;  // 该上下文执行完时要恢复的上下文
    sigset_t         uc_sigmask;  
    stack_t          uc_stack;  //使用的栈
    mcontext_t       uc_mcontext;  
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
