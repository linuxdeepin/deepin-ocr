# Deepin OCR

Deepin OCR provides the base character recognition ability on Deepin.

## Dependencies

### Build dependencies

* pkg-config
* libdtkwidget-dev
* Qt5(>=5.6) with modules:
  * qt5-default
  * qt5-qmake
  * qtbase5-dev
* Installation

### Build from source code

1. Make sure you have installed all dependencies.
````
sudo apt build-dep deepin-ocr
````
If you need to use the designer plugin, you should also install:
````
sudo apt install qttools5-dev
````
2. Build:

````
$ mkdir build
$ cd build
$ cmake ..
$ make
````

3. Install:

````
$ sudo make install
````

## Getting help

Any usage issues can ask for help via

* [Gitter](https://gitter.im/orgs/linuxdeepin/rooms)
* [IRC channel](https://webchat.freenode.net/?channels=deepin)
* [Forum](https://bbs.deepin.org)
* [WiKi](https://wiki.deepin.org/)

## Getting involved

We encourage you to report issues and contribute changes

* [Contribution guide for developers](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers-en). (English)

## License

Deepin OCR is licensed under [GPL-3.0-or-later](LICENSE).

## About model files

The OCR backend is `PaddleOCR` on `NCNN`. Dictionary file is in `assets/dict` and model file is in `assets/model`. The model files are convert by `Paddle Inference` format. They are `NCNN` format now. The conversion process is as follows: 

- Download pre-train model files from https://github.com/PaddlePaddle/PaddleOCR . This project is open source under the [Apache-2.0](LICENSES/Apache-2.0.txt) protocol. The origin model files are trained by `PaddleOCR` team.
- Use `Paddle2ONNX` convert `Paddle Inference` model files you downloaded to `ONNX` format. The `Paddle2ONNX` project is in https://github.com/PaddlePaddle/Paddle2ONNX , open source under the [Apache-2.0](LICENSES/Apache-2.0.txt) protocol.
- Use `onnx-simplifier` to optimize the `ONNX` model files and fix input shape. The `onnx-simplifier` is in https://github.com/daquexian/onnx-simplifier , open source under the [Apache-2.0](LICENSES/Apache-2.0.txt) protocol.
- Use `onnx2ncnn` convert the optimized `ONNX` model files to `NCNN` format. `onnx2ncnn` is under the `NCNN` project, open source under the [BSD-3-Clause](LICENSES/BSD-3-Clause.txt) protocol.