# 说明

该项目是利用ProcExp来关闭内核回调，其中关闭的回调如下，增加字符串的定位的难度，参考了绕过hvci的项目：

PsSetCreateProcessNotifyRoutine（清空数组，PspCreateProcessNotifyRoutineExCount和PspCreateProcessNotifyRoutineCount未清零，这两个变量不好定位所以没处理，当然加载符号处理更方便）

PsSetLoadImageNotifyRoutine（调用对应的注销函数）

PsSetCreateThreadNotifyRoutine（调用对应的注销函数）

ObRegisterCallbacks（调用对应的注销函数）

CmRegisterCallback（调用对应的注销函数）

minifilter（直接清零mj 0-1B的回调；顺便处理了minifilter的ConnectNotify、DisconnectNotify、MessageNotify。如果你不想直接清零，可以返回回调为rop_6 xor rax,rax retn）

运行的时候需要管理员权限。

# 版本

由于只有win 20h2和win 7sp1的虚拟机，所以这里只测试了这两个，特征并不稳定，可能需要你自己稍微改改。

# 参考

https://github.com/Cr4sh/KernelForge