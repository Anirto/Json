#### C开发流程样例

------



##### What this?

------

这是以一个C语言写的json解析库，不仅是coding，更是了解标准的开发流程。



##### Enverment

------

- liunx环境、gcc、make
- 代码覆盖率工具 `sudo apt install lcov`
- 动态调试工具 `sudo apt install valgrind`



##### What can I do ?

------

- 怎样写单测样例？

- 怎样设计api？

- 怎样发现内存泄漏？

- 怎样将json输出为yml？
- 怎样生成代码覆盖报告？
- ...



##### HOW

------

```shell
make
make test
make check
make lcov
```



##### To do

------

- [ ] 怎样根据.h文件生成api文档？

- [ ] 怎样优化key value的存储方式?
- [ ] ...