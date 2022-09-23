# Deepin OCR

Deepint Ocr 提供基础文字识别功能.

您应该首先阅读 [Deepin应用程序规范](\ref doc/Specification).

## 依赖

### 编译依赖

* pkg-config
* libdtkwidget-dev
* Qt5(>=5.6) with modules:
  * qt5-default
  * qt5-qmake
  * qtbase5-dev
  * qttools5-dev-tools

## 安装

### 从源代码构建

1. 确保已经安装了所有的编译依赖.
````bash
sudo apt build-dep deepin-ocr
````
如果需要使用qtcreator的`设计`功能，需要安装以下依赖:
```bash
sudo apt install qttools5-dev
```
2. 构建:

```bash
mkdir build
cd build
cmake ..
make
```

3. 安装:

```bash
sudo make install
```

## 用法

命令行执行 `deepin-ocr`

## 文档

 - 无

## 帮助

任何使用问题都可以通过以下方式寻求帮助:

* [Gitter](https://gitter.im/orgs/linuxdeepin/rooms)
* [IRC channel](https://webchat.freenode.net/?channels=deepin)
* [Forum](https://bbs.deepin.org)
* [WiKi](https://wiki.deepin.org/)

## 参与贡献

我们鼓励您报告问题并作出更改

* [开发者代码贡献指南](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers)

## 协议

Deepin OCR 遵循协议 [GPL-3.0-or-later](LICENSE).

## 关于模型文件

本项目的 OCR 后端算法是 `PaddleOCR` 与 `NCNN` 两个项目的结合，在 `assets/dict` 下存放了字典文件，而在 `assets/model` 下的模型文件是从 `Paddle Inference` 转换到 `NCNN` 的产物，转换过程如下：

- 从 https://github.com/PaddlePaddle/PaddleOCR 项目下选取需要的预训练模型文件，该项目遵循 [Apache-2.0](LICENSES/Apache-2.0.txt) 协议开源，原始的模型文件由 `PaddleOCR` 项目组训练并输出。
- 使用 `Paddle2ONNX `工具将下载好的 `Paddle Inference` 模型文件转换至 `ONNX` 模型文件，`Paddle2ONNX` 项目位于 https://github.com/PaddlePaddle/Paddle2ONNX ，遵循 [Apache-2.0](LICENSES/Apache-2.0.txt) 协议开源。
- 使用 `onnx-simplifier` 工具优化 `ONNX` 模型文件并固定输入 shape，`onnx-simplifier` 项目位于 https://github.com/daquexian/onnx-simplifier ，遵循 [Apache-2.0](LICENSES/Apache-2.0.txt) 协议开源。
- 使用 `NCNN` 项目下的 `onnx2ncnn` 工具将优化好的 `ONNX` 模型文件转换到 `NCNN` 模型文件，此 `NCNN` 模型文件即为本项目下的模型文件。`onnx2ncnn` 工具同 `NCNN` 项目一样，遵循 [BSD-3-Clause](LICENSES/BSD-3-Clause.txt) 协议开源。
