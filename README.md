# PocketLab 温湿度显示（STM32 + ST7735S + AHT20）

基于 STM32F103/W55MH32 的 PocketLab 开发板，1.8 寸 ST7735S 屏（128×160，SPI）显示 AHT20 温湿度。

## 硬件连接

| 信号 | 引脚 |
|---|---|
| LCD SCL | PC11 |
| LCD SDA | PC12 |
| LCD RES | PD6 |
| LCD DC  | PD5 |
| LCD CS  | PC10 |
| LCD BLK | PA8 |
| AHT20 SCL | PB6 |
| AHT20 SDA | PB7 |

## 快速开始

### 1. 获取代码

```bash
git clone https://github.com/cheng-l-ONG/1.8-LCD-STM32F103C8T6.git
```

也可以网页右上 `Code → Download ZIP`，但**要修改并提交回本仓库时请用 clone**，ZIP 方式没有 Git 历史，改动无法合并。

### 2. 编译环境

| 项 | 要求 |
|---|---|
| IDE | Keil MDK 5.x（本工程使用 5.36 验证） |
| 器件包 | **Keil.STM32F1xx_DFP**（缺了会提示找不到 STM32F103C8） |
| 库 | STM32F10x 标准外设库（已随仓库提供，无需另装） |
| 下载器 | ST-Link / J-Link（SWD：3V3、GND、SWDIO、SWCLK） |

打开 `USER/LCD.uvprojx`，编译后下载即可。

## 编码约定（重要）

本工程源码为 **GBK** 编码，`.gitattributes` 中已声明 `*.c/*.h` 的 `working-tree-encoding=GBK`：

- Git 仓库内统一存 **UTF-8**，所以网页上查看中文正常
- 工作区文件是 **GBK**，以配合 Keil 的中文注释与字库

**请勿将 `.c/.h` 另存为 UTF-8**：`HARDWARE/LCD/lcdfont.h` 中的汉字是字库索引

```c
typedef struct {
    unsigned char Index[2];   /* 一个 GBK 汉字占 2 字节 */
    unsigned char Msk[24];
} typFNT_GB12;
```

改成 UTF-8 后一个汉字变 3 字节，`Index[2]` 会被截断，屏幕中文显示全部错乱。

## 目录结构

```
CORE/                 启动文件、CMSIS 内核文件
HARDWARE/
  LCD/                中景园 ST7735S 驱动、字库
  IIC/                软件 IIC
  AHT20/              AHT20 温湿度传感器驱动
  LED/                LED
STM32F10x_FWLib/      STM32 标准外设库
SYSTEM/               delay / sys / usart
USER/                 main.c、工程文件 LCD.uvprojx
```

编译产物（`OBJ/`、`USER/Listings/`、`.uvoptx`、`JLinkSettings.ini` 等）已被 `.gitignore` 排除，属本机文件，不会进入仓库。

## 参与修改

- 直接贡献：由仓库所有者添加为 Collaborator（Write 权限）后 `clone → 修改 → commit → push`
- 外部贡献：`Fork` → 修改 → 提交 `Pull Request`

提交前建议先 `git pull` 同步，避免历史分叉。

## 版权与许可

- 本项目自研部分（`USER/`、`SYSTEM/`、`HARDWARE/AHT20`、`HARDWARE/IIC`、`HARDWARE/LED` 等）采用 **MIT 许可证**，详见 `LICENSE`。
- `HARDWARE/LCD/`（含 `lcdfont.h` 字库）为南京中景园电子提供的驱动，版权归原作者，**仅供学习交流，不得用于商业用途**。
- `STM32F10x_FWLib/` 为 STMicroelectronics 标准外设库，版权归 STMicroelectronics 所有，遵循其附带许可。

MIT 许可仅覆盖本项目自研部分，不改变上述第三方组件的授权条款。
