# lvgl笔记
1. 使用lvgl默认的内存管理，会导致很多莫名的错误，具体为在使用lv_open等接口时，可以看到`lv_fs_drv_t fs_drv`这个变量成员的地址并不连续，从而导致运行错误，在lv_conf.h中把宏`LV_MEM_CUSTOM`定义为1，使用标准库的内存管理功能即可。
2. 使用lvgl的文件系统时，lv_fs_read/lv_fs_write可以传递一个变量br/bw用于记录成功read/write的个数，在调用时，不能使用同一个变量作为这两个函数的参数传递，否则会读写错误
3. lvgl使用中文
   1. 使用官方的生成方式
      1. 进入网站[https://lvgl.io/tools/fontconverter]，这是lvgl官方指定的中文生成方式
      2. 选择免费字体文件[http://lvgl.100ask.net/8.1/tools/fonts-zh-source.html]，不能使用本地office`C:\Windows\Fonts`的字体文件，会识别错误
      3. 配置
         1. `Name`:字体名字、生成的文件名
         2. `Size`:字体大小
         3. `Bpp`:生成字体的锯齿程度
         4. `TTF/WOFF file`：选择2的字体
         5. `Range`:输入`0x20-0x7F`，使得生成的字库支持英文、数字、符号，也可以输入指定符号的Unicode
         6. `Symbols`：需要显示的中文
      4. 点击`Convert`即可以生成对应的.c文件
      5. 调用:在要使用的文件中，使用`LV_FONT_DECLARE(fontname)`声明字体即可
   2. 使用工具`lvglFontTool`，
      1. 参考支持文件系统[https://blog.csdn.net/qq_44062900/article/details/117199509]
      2. 参考lvglFontTool的使用[https://blog.csdn.net/flamebox/article/details/122202194]
      3. 该方式可以把中文字体生成.bin文件，放在SD卡/flash中使用，也可以生成大数组，直接在文件中调用
      4. 如果生成.bin文件的话，则需要文件系统的支持，文件系统fatfs需要文件cc936.c来支持中文的现实，该文件中也有一个大数组，会导致最终生成的app.bin很大
   3. 比较:使用lvglFontTool,因为涉及文件系统，会导致生成的bin文件很大，而使用官方工具，生成的bin文件只有一百多k，差别明显