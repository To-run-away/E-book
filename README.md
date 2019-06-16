这是一个可以在linux上移植使用的电子书项目.

目前实现的功能会包括:
1.汉字和英文字符的的显示;

2.支持的字符编码格式包括(utf8[大小任意], ansi[16*8],utf-16le和utf-l6be[16*16]);

3.支持的字体格式包括(ascii, gbk , freetype);

4.支持输入文件的显示;

5.支持标准输入翻页,推出;

6.输入子系统的支持,即翻页,字体放大都通过触摸屏或按键来控制;

7......

前提：
1.提前编译好freetype库,并放在开发板上;

2.提前编译好tslib库,并放在开发板上;




#-------------------------------------------
使用tslib库需要把下面几个宏导出为环境变量,
这个我也会放在tools目录下,建议设置为脚本,在开机启动时自动导出到环境变量

export TSLIB_TSDEVICE=/dev/input/event0

export TSLIB_CONFFILE=/etc/ts.conf

export TSLIB_PLUGINDIR=/lib/ts

export TSLIB_CONSOLEDEVICE=none

export TSLIB_FBDEVICE=/dev/fb0

#-------------------------------------------


电子书使用步骤：
1.根据自己的使用平台替换编译器,也可以使用指定编译工具链来编译,比如:

make CROSS_COMPILE=arm-none-linux-gnueabi-

2.把可执行文件放在目标单板的文件系统上


3.可以先执行下面命令查看支持的显示器,字体,解码器
./e-book [-l list] 

4.可以通过下面命令来指定显示的格式
./e-book [-s Size] [-d Display] [-f font_file] [-h hzk] <text_file>
比如:
./e-book -s 40 -h hzk16 -f ./simkai.ttf zhuxian.txt 

[-s Size] 其中Size表示要显示的字体大小

[-d Display] Display表示显示器为什么,默认现在是fb

[-f font_file] font_file这个用在freetype使用矢量字体时,这个是矢量字体文件

[-h hzk] hzk这个是使用在gbk编码的中文时,使用,目前gbk编码的只支持字体大小为16*16的大小

<text_file> 这个是必选项,表示要显示的文件


5.在tools目录下有几个使用的几个字体文件
