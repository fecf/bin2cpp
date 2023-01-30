# bin2cpp

original code: https://mort.coffee/home/fast-cpp-embeds/

---

example
```
bin2cpp.exe -l 80 bin2cpp.exe stdout
```

```
#ifdef __INTELLISENSE__
const unsigned char bin2cpp_exe[];
const unsigned int bin2cpp_exe_len;
#else
const unsigned char bin2cpp_exe[] =
        "MZ\620\000\003\000\000\000\004\000\000\000\777\777\000\000\670\000\000\000\000\000"
        "\000\000@\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
        "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\760\000\000"
        "\000\016\037\672\016\000\664\t\715!\670\001L\715!This program cannot be run in D"
        "OS mode.\r\r\n$\000\000\000\000\000\000\000\745\741P*\641\600>y\641\600>y\641\600"
        ">y\752\770=x\657\600>y\752\770;x\021\600>y\752\770\072x\667\600>y\752\770\077x\642"
...
#endif
```